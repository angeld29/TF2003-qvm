/*
 *  QWProgs-TF2003
 *  Copyright (C) 2004  [sd] angel
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 *  $Id$
 */

#include "g_local.h"

// CSQC-механизмы сервера. Детали и ограничения — в include/g_csqc.h.
// Полный конвейер (SendEntity-эмит, MSG_CSQC, статы 32..127) есть только в
// fteqw QVM; на mvdsv всё guarded через G_CSQC_OK(), чтобы не дёргать
// erroring-заглушки движка.

static qboolean csqc_ok_initialized = false;
static qboolean csqc_ok = false;

// Канарейка pointerstat: тикает ~1/с в G_CSQC_Example_Frame (проверка канала).
int g_csqc_tick;

// Тестовые глобалы pointerstat с фиксированными значениями разных типов.
static int   g_ps_int   = 123456789;  // 0x075BCD15
static float g_ps_float = 12345.5f;   // 0x4640E600
static float g_ps_vec[3] = {1.0f, 2.0f, 3.0f};

// Полная поддержка CSQC движком = доступны clientstat+pointerstat+setsendneeded.
// На mvdsv clientstat/pointerstat не мапятся вовсе, а setsendneeded хоть и
// мапится, но его обработчик — SV_Error. Комбинация отличает fteqw от mvdsv
// без риска краша.
static qboolean any_client_csqc_active( void );	// определена ниже, нужна эху/фрейм-хуку
qboolean G_CSQC_OK( void )
{
	if ( !csqc_ok_initialized )
	{
		csqc_ok = HAVEEXT( G_CLIENTSTAT )
			&& HAVEEXT( G_POINTERSTAT )
			&& HAVEEXT( G_SETSENDNEEDED );
		csqc_ok_initialized = true;
		if ( cvar( "developer" ) )
			G_dprintf( "G_CSQC_OK: %s\n", csqc_ok ? "full CSQC support" : "no CSQC support" );
	}
	return csqc_ok;
}

// Есть ли у клиента активный csqc-модуль (движок выставляет infokey "csqcactive").
qboolean G_ClientCSQCActive( gedict_t *client )
{
	char buf[8];

	if ( !client )
		return false;
	infokey( client, GCSQC_INFOKEY_CSQCACTIVE, buf, sizeof( buf ) );
	return ( buf[0] == '1' );
}

//===========================================================================
// GAME_QCREQUEST: sendevent клиент→сервер (референс-реализация).
// Контракт (движок и мод совпадают) — docs/ezquake_csqc_pr2.md §5.2:
//   self=client, arg0=argcount (0..6). Имя события движок кладёт в cmd-argv
//   одним сырым аргументом (без токенизации) — мод читает его trap_CmdArgv(0);
//   значения аргументов мод получает трапом qcrequestarg (idx, buf, size):
//   движок копирует значение в память мода и возвращает тип GCSQC_QCREQ_*
//   (float/vector — сырые байты, int/entity — int, string — строка).
// Референс читает и логирует под cvar "developer"; возврат 1 = обработано.
//===========================================================================

// Эхо-ответ на sendevent "csqc_echo": шлёт клиенту cgamepacket "echo" с теми
// же аргументами (сырые значения: coord/long/string/entity), чтобы клиент
// мог отобразить их и подтвердить сквозную передачу типов. Бинарное кодирование
// (без форматирования float на сервере — QVM не дружит с %f в snprintf).
void G_CSQC_Example_Echo( gedict_t *cl, int argcount )
{
	char buf[12];
	int i;
	int type;

	if ( !cl || argcount < 0 || argcount > GCSQC_QCREQ_MAXARGS )
		return;
	if ( !G_CSQC_OK() || !any_client_csqc_active() )
		return;

	trap_WriteByte( MSG_MULTICAST, GCSQC_SVC_CGAMEPACKET );
	trap_WriteString( MSG_MULTICAST, "echo" );
	trap_WriteByte( MSG_MULTICAST, argcount );

	for ( i = 0; i < argcount; i++ )
	{
		type = G_Ext_QCRequestArg( i, buf, sizeof( buf ) );
		trap_WriteByte( MSG_MULTICAST, type );

		switch ( type )
		{
		case GCSQC_QCREQ_FLOAT:
			trap_WriteCoord( MSG_MULTICAST, *( float * )buf );
			break;
		case GCSQC_QCREQ_VECTOR:
			trap_WriteCoord( MSG_MULTICAST, ( ( float * )buf )[0] );
			trap_WriteCoord( MSG_MULTICAST, ( ( float * )buf )[1] );
			trap_WriteCoord( MSG_MULTICAST, ( ( float * )buf )[2] );
			break;
		case GCSQC_QCREQ_INT:
			trap_WriteLong( MSG_MULTICAST, *( int * )buf );
			break;
		case GCSQC_QCREQ_STRING:
			trap_WriteString( MSG_MULTICAST, buf );
			break;
		case GCSQC_QCREQ_ENTITY:
			{
				gedict_t *e = PROG_TO_EDICT( *( int * )buf );
				trap_WriteEntity( MSG_MULTICAST, e ? NUM_FOR_EDICT( e ) : 0 );
			}
			break;
		default:
			trap_WriteString( MSG_MULTICAST, "?" );
			break;
		}
	}

	trap_multicast( PASSVEC3( cl->s.v.origin ), MULTICAST_PHS );
}

int G_GameQCRequest( int argcount )
{
	int i;
	gedict_t *cl;
	char evname[256];
	char buf[12];	// vector (12 байт) — наибольшее копируемое значение

	if ( argcount < 0 || argcount > GCSQC_QCREQ_MAXARGS )
		return 0;

	cl = PROG_TO_EDICT( g_globalvars.self );
	if ( !cl )
		return 0;

	// событие: единственный raw argv(0) (движок ставит без токенизации)
	trap_CmdArgv( 0, evname, sizeof( evname ) );

	// тест-событие: замкнутый круг sendevent → qcrequestarg → cgamepacket
	if ( !strcmp( evname, "csqc_echo" ) )
	{
		G_CSQC_Example_Echo( cl, argcount );
		if ( !cvar( "developer" ) )
			return 1;
	}

	if ( !cvar( "developer" ) )
		return 1;	// принято; логирование не требуется

	G_dprintf( "GAME_QCREQUEST client %d event \"%s\" argc=%d\n",
		NUM_FOR_EDICT( cl ), evname, argcount );

	for ( i = 0; i < argcount; i++ )
	{
		int type = G_Ext_QCRequestArg( i, buf, sizeof( buf ) );

		switch ( type )
		{
		case GCSQC_QCREQ_FLOAT:
			G_dprintf( "  [%d] float %f\n", i, *( float * )buf );
			break;
		case GCSQC_QCREQ_INT:
			G_dprintf( "  [%d] int %d\n", i, *( int * )buf );
			break;
		case GCSQC_QCREQ_VECTOR:
			G_dprintf( "  [%d] vector %f %f %f\n", i,
				( ( float * )buf )[0], ( ( float * )buf )[1], ( ( float * )buf )[2] );
			break;
		case GCSQC_QCREQ_ENTITY:
			{
				gedict_t *e = PROG_TO_EDICT( *( int * )buf );
				G_dprintf( "  [%d] entity %d\n", i, e ? NUM_FOR_EDICT( e ) : -1 );
			}
			break;
		case GCSQC_QCREQ_STRING:
			G_dprintf( "  [%d] string \"%s\"\n", i, buf );
			break;
		default:
			G_dprintf( "  [%d] type %d (value not delivered)\n", i, type );
			break;
		}
	}

	return 1;
}

//===========================================================================
// SendEntity: назначение функции сериализации эдикта.
// Колбек хранится в C-поле gedict_t.SendEntity (вызывается нашим обработчиком
// GAME_EDICT_CSQCSEND), движку сообщается флаг через расширенное поле
// "SendEntity": Ptr-путь (mvdsv) либо Field-путь (fteqw).
//===========================================================================

static intptr_t field_ref_sendentity = 0;

void ExtFieldSetSendEntity( gedict_t *ed, func_t callback )
{
	ed->SendEntity = callback;

	if ( !field_ref_sendentity && G_EXT_BASE_PTR_OK )
		field_ref_sendentity = trap_MapExtFieldPtr( "SendEntity" );

	if ( field_ref_sendentity )
	{
		int enable = 1;
		trap_SetExtFieldPtr( ed, field_ref_sendentity, (void *)&enable, sizeof( int ) );
	}
	else if ( HAVEEXT( G_SETEXTFIELD ) )
	{
		trap_SetExtField( ed, "SendEntity", 1 );
	}
	else if ( cvar( "developer" ) )
	{
		G_dprintf( "ExtFieldSetSendEntity: no SendEntity field support in server\n" );
	}
}

//===========================================================================
// Запись CSQC-payload (внутри SendEntity-колбека).
// На mvdsv MSG_CSQC — ошибка движка, поэтому guarded через G_CSQC_OK().
//===========================================================================

void G_CSQC_WriteByte( int data )
{
	if ( G_CSQC_OK() )
		trap_WriteByte( MSG_CSQC, data );
}

void G_CSQC_WriteChar( int data )
{
	if ( G_CSQC_OK() )
		trap_WriteChar( MSG_CSQC, data );
}

void G_CSQC_WriteShort( int data )
{
	if ( G_CSQC_OK() )
		trap_WriteShort( MSG_CSQC, data );
}

void G_CSQC_WriteLong( int data )
{
	if ( G_CSQC_OK() )
		trap_WriteLong( MSG_CSQC, data );
}

void G_CSQC_WriteAngle( float data )
{
	if ( G_CSQC_OK() )
		trap_WriteAngle( MSG_CSQC, data );
}

void G_CSQC_WriteCoord( float data )
{
	if ( G_CSQC_OK() )
		trap_WriteCoord( MSG_CSQC, data );
}

void G_CSQC_WriteString( const char *data )
{
	if ( G_CSQC_OK() )
		trap_WriteString( MSG_CSQC, data );
}

void G_CSQC_WriteEntity( gedict_t *ed )
{
	if ( G_CSQC_OK() )
		trap_WriteEntity( MSG_CSQC, NUM_FOR_EDICT( ed ) );
}

//===========================================================================
// Регистрация клиентских/глобальных статов (GCSQC_STAT_FIRST..GCSQC_STAT_MAX).
//===========================================================================

static qboolean stat_in_range( int statnum )
{
	if ( statnum < GCSQC_STAT_FIRST || statnum > GCSQC_STAT_MAX )
	{
		if ( cvar( "developer" ) )
			G_dprintf( "G_RegisterStat: %d вне диапазона %d..%d\n",
				   statnum, GCSQC_STAT_FIRST, GCSQC_STAT_MAX );
		return false;
	}
	return true;
}

void G_RegisterClientStat( int statnum, int type, int fieldoffset )
{
	if ( !stat_in_range( statnum ) )
		return;
	if ( G_CSQC_OK() )
	{
		G_Ext_ClientStat( statnum, type, fieldoffset );
		if ( cvar( "developer" ) )
			G_dprintf( "G_RegisterClientStat: %d type %d ofs %d\n", statnum, type, fieldoffset );
	}
}

void G_RegisterPointerStat( int statnum, int type, void *ptr )
{
	if ( !stat_in_range( statnum ) )
		return;
	if ( G_CSQC_OK() )
	{
		G_Ext_PointerStat( statnum, type, ptr );
		if ( cvar( "developer" ) )
			G_dprintf( "G_RegisterPointerStat: %d type %d\n", statnum, type );
	}
}

//===========================================================================
// Отправка CSQC-события (svcfte_cgamepacket) клиентам с активным CSQC.
// Движок fteqw сам фильтрует по PEXT_CSQC (net_preparse), но без CSQC-клиентов
// пакет бессмыслен, поэтому дополнительно проверяем наличие таковых.
//===========================================================================

static qboolean any_client_csqc_active( void )
{
	gedict_t *p;

	for ( p = trap_nextclient( world ); p; p = trap_nextclient( p ) )
	{
		if ( G_ClientCSQCActive( p ) )
			return true;
	}
	return false;
}

void G_SendCSQCEvent( vec3_t org, const char *name )
{
	if ( !G_CSQC_OK() )
		return;
	if ( !any_client_csqc_active() )
		return;

	trap_WriteByte( MSG_MULTICAST, GCSQC_SVC_CGAMEPACKET );
	trap_WriteString( MSG_MULTICAST, name );
	trap_multicast( PASSVEC3( org ), MULTICAST_PHS );
}

//===========================================================================
// ПРИМЕР использования CSQC-механизмов в TF2003 (под cvar "g_csqc", default 0).
// Клиентский модуль (csprogs.dat) отсутствует — это только серверная сторона:
// регистрация статов и SendEntity для флагов. Включается администратором на
// CSQC-ориентированном сервере; на обычных клиентах лишние статы (≥ GCSQC_STAT_FIRST)
// могут быть непонятны, поэтому по умолчанию выключено.
//===========================================================================

void G_CSQC_Example_RegisterStats( void )
{
	// Включение: fteqw — "g_csqc 1" в консоли/конфиге, mvdsv — "set g_csqc 1"
	// (mvdsv требует заранее зарегистрированный cvar). Применяется на следующей
	// загрузке карты (GAME_INIT).
	if ( cvar( "developer" ) )
		G_dprintf( "G_CSQC_Example_RegisterStats: g_csqc=%f\n", cvar( "g_csqc" ) );
	if ( !cvar( "g_csqc" ) )
		return;

	// Клиентские статы (смещение относительно начала gedict_t; entvars_t по offset 0).
	G_RegisterClientStat( GCSQC_STAT_FIRST + 0, GCSQC_EV_INTEGER, FOFS( playerclass ) );
	G_RegisterClientStat( GCSQC_STAT_FIRST + 1, GCSQC_EV_INTEGER, FOFS( team_no ) );
	G_RegisterClientStat( GCSQC_STAT_FIRST + 2, GCSQC_EV_INTEGER, FOFS( real_frags ) );
	// Текущее оружие: точный WEAP_* бит (s.v.weapon в TF2003 = 0, STAT_ITEMS
	// аккумулирует слоты, STAT_WEAPONMODELI не доставляется в QW-протоколе).
	G_RegisterClientStat( GCSQC_STAT_WEAPON, GCSQC_EV_INTEGER, FOFS( current_weapon ) );
	// Канарейка clientstat: текущие патроны (всегда >0, пока есть оружие).
	G_RegisterClientStat( GCSQC_STAT_FIRST + 4, GCSQC_EV_INTEGER, FOFS( s.v.currentammo ) );

	// Глобальные статы (общая память мода): канарейка-тик + счёт команд.
	G_RegisterPointerStat( GCSQC_STAT_FIRST + 8, GCSQC_EV_INTEGER, &g_csqc_tick );
	G_RegisterPointerStat( GCSQC_STAT_FIRST + 9, GCSQC_EV_INTEGER, &teamscores[1] );
	G_RegisterPointerStat( GCSQC_STAT_FIRST + 10, GCSQC_EV_INTEGER, &teamscores[2] );
	G_RegisterPointerStat( GCSQC_STAT_FIRST + 11, GCSQC_EV_INTEGER, &teamscores[3] );

	// Тест-глобалы фикс. значений (диагностика чтения памяти pointerstat).
	G_RegisterPointerStat( GCSQC_STAT_FIRST + 18, GCSQC_EV_INTEGER, &g_ps_int );
	G_RegisterPointerStat( GCSQC_STAT_FIRST + 19, GCSQC_EV_FLOAT, &g_ps_float );
	G_RegisterPointerStat( GCSQC_STAT_FIRST + 20, GCSQC_EV_VECTOR, g_ps_vec );
}

// SendEntity-колбек для флага: пишет мини-payload для CSQC-клиента.
// self=flag-эдикт, other=viewer, arg0=sendflags (биты GCSQC_SENDFLAG_*).
// Возврат 0=не слать, !=0=слать.
int G_CSQC_Example_FlagSendEntity( int sendflags )
{
	if ( !G_CSQC_OK() )
		return 0;

	// Экономия: если изменились только координаты и состояние не трогалось,
	// всё равно шлём (флаг — маленькая сущность); STATE-бит — соглашение примера.
	( void )sendflags;

	G_CSQC_WriteEntity( self );       // номер сущности
	G_CSQC_WriteShort( ( int )self->s.v.skin );  // 1/2 = команда
	G_CSQC_WriteByte( ( int )self->s.v.frame );  // 0=на базе, 1=несётся
	G_CSQC_WriteLong( ( int )( g_globalvars.time * 10 ) ); // время в 0.1с
	return 1;
}

// Назначает SendEntity флагам (goal-сущности с моделью tf_flag.mdl).
// Вызывается из TF_PlaceItem при спавне.
void G_CSQC_Example_PlaceItem( gedict_t *ent )
{
	if ( !ent || !cvar( "g_csqc" ) )
		return;
	if ( ent->mdl && streq( ent->mdl, "progs/tf_flag.mdl" ) )
	{
		ExtFieldSetSendEntity( ent, ( func_t )G_CSQC_Example_FlagSendEntity );
		if ( cvar( "developer" ) )
			G_dprintf( "G_CSQC_Example_PlaceItem: SendEntity set on flag edict %d\n", NUM_FOR_EDICT( ent ) );
	}
}

// Периодический дирт CSQC-сущностей (раз в ~1 c). На mvdsv эмиссия идёт только
// по dirt (setsendneeded/SendFlags), поэтому клиент, включивший csqc после
// спавна карты, без этого не увидел бы ни одной CSQC-сущности. Вызывается из
// StartFrame; фактически работает только при g_csqc и наличии CSQC-клиента.
void G_CSQC_Example_Frame( void )
{
	static float last_dirt = 0;
	int n;
	gedict_t *e;

	if ( !cvar( "g_csqc" ) )
		return;
	if ( !any_client_csqc_active() )
		return;
	if ( g_globalvars.time - last_dirt < 1.0f )
		return;
	last_dirt = g_globalvars.time;

	g_csqc_tick++;

	for ( n = trap_nextent( 0 ); n; n = trap_nextent( n ) )
	{
		e = &g_edicts[n];
		if ( e->SendEntity )
			G_Ext_SetSendNeeded( e, GCSQC_SENDFLAG_STATE, NULL );
	}
}
