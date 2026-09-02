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

#ifndef G_CSQC_H
#define G_CSQC_H

// CSQC-механизмы сервера (по образцу FTE QVM / ktx-orig / FTE pr_csqc).
// Полноценно работают только на движках с полной поддержкой CSQC (fteqw QVM,
// sv_progtype 2): mvdsv умеет лишь согласование (PEXT_CSQC, enablecsqc,
// csqcactive, *csprogs-ключи), но MSG_CSQC/setsendneeded там — ошибка движка,
// а эмит CSQC-сущностей отсутствует. Поэтому все операции guarded через
// G_CSQC_OK(), который истинен только на fteqw.

//===========================================================================
// Константы CSQC (значения из fteqw: common/protocol.h, server/server.h,
// common/bothdefs.h). В qwprot (mvdsv) их нет — мод определяет локально.
//===========================================================================

//--- Сообщения протокола, server→client (fteqw common/protocol.h) ---
#define GCSQC_SVC_CSQCENTITIES         76   // entity lump for csqc
#define GCSQC_SVC_CGAMEPACKET          83   // ssqc→csqc, только multicast
#define GCSQC_SVC_CGAMEPACKET_SIZED    90   // cgamepacket с префиксом длины (sv_csqcdebug)
#define GCSQC_SVC_CSQCENTITIES_SIZED   92   // csqcentities с префиксом длины

//--- Сообщение клиента, client→server ---
#define GCSQC_CLC_QCREQUEST            81   // sendevent (клиентский CSQC → сервер)

//--- Бит расширения протокола ---
#define GCSQC_PEXT_CSQC                0x40000000

//--- Dest для записи CSQC-payload внутри SendEntity (fteqw server.h:1082,
//    mvdsv pr2_cmds.c:1183) ---
#ifndef MSG_CSQC
#define MSG_CSQC   5
#endif
#define GCSQC_MSG_CSQC                 MSG_CSQC

//--- Типы клиентских статов (enum etype_t из FTE qclib/pr_comp.h:
//    ev_void=0, ev_string=1, ev_float=2, ev_vector=3, ev_entity=4,
//    ev_field=5, ev_function=6, ev_pointer=7, ev_integer=8) ---
#define GCSQC_EV_STRING    1
#define GCSQC_EV_FLOAT     2
#define GCSQC_EV_VECTOR    3
#define GCSQC_EV_ENTITY    4
#define GCSQC_EV_INTEGER   8

//--- Коды аргументов sendevent (экспорт GAME_QCREQUEST; движок и мод обязаны
//    совпадать — docs/ezquake_csqc_pr2.md §5.2). Экспорт передаёт только
//    argcount (0..GCSQC_QCREQ_MAXARGS); имя события движок кладёт в cmd-argv
//    одним сырым аргументом (без токенизации) — читается trap_CmdArgv(0);
//    значения аргументов движок отдаёт трапом qcrequestarg(idx, buf, size):
//    копирует значение в buf и возвращает код типа GCSQC_QCREQ_*. ---
#define GCSQC_QCREQ_FLOAT    0
#define GCSQC_QCREQ_VECTOR   1
#define GCSQC_QCREQ_STRING   2
#define GCSQC_QCREQ_ENTITY   3
#define GCSQC_QCREQ_INT      4
#define GCSQC_QCREQ_UNKNOWN  5   // mvdsv: тип распознан, но значение не доставлено
#define GCSQC_QCREQ_MAXARGS  6

//--- Статы: диапазон регистрируемых (0..31 заняты движком) ---
#define GCSQC_STAT_FIRST               32   // первый свободный стат
#define GCSQC_STAT_MAX                 255  // MAX_CL_STATS-1 (fteqw bothdefs.h:1088)
#define GCSQC_STAT_WEAPON              35   // current_weapon (WEAP_* бит, точное оружие)

//--- Стандартные QW-статы 0..31 (fteqw common/bothdefs.h) ---
#define GCSQC_STAT_HEALTH              0
#define GCSQC_STAT_WEAPONMODELI        2
#define GCSQC_STAT_AMMO                3
#define GCSQC_STAT_ARMOR               4
#define GCSQC_STAT_WEAPONFRAME         5
#define GCSQC_STAT_SHELLS              6
#define GCSQC_STAT_NAILS               7
#define GCSQC_STAT_ROCKETS             8
#define GCSQC_STAT_CELLS               9
#define GCSQC_STAT_ACTIVEWEAPON        10
#define GCSQC_STAT_TOTALSECRETS        11
#define GCSQC_STAT_TOTALMONSTERS       12
#define GCSQC_STAT_SECRETS             13
#define GCSQC_STAT_MONSTERS            14
#define GCSQC_STAT_ITEMS               15
#define GCSQC_STAT_VIEWHEIGHT          16
#define GCSQC_STAT_TIME                17
#define GCSQC_STAT_MATCHSTARTTIME      18
#define GCSQC_STAT_VIEWZOOM            21
#define GCSQC_STAT_IDEALPITCH          25
#define GCSQC_STAT_PUNCHANGLE_X        26
#define GCSQC_STAT_PUNCHANGLE_Y        27
#define GCSQC_STAT_PUNCHANGLE_Z        28
#define GCSQC_STAT_PUNCHVECTOR_X       29
#define GCSQC_STAT_PUNCHVECTOR_Y       30
#define GCSQC_STAT_PUNCHVECTOR_Z       31

//--- pvsflags (fteqw server.h:1029-1034) ---
#define GCSQC_PVSF_NORMALPVS           0x0
#define GCSQC_PVSF_NOTRACECHECK        0x1
#define GCSQC_PVSF_USEPHS              0x2
#define GCSQC_PVSF_IGNOREPVS           0x3
#define GCSQC_PVSF_MODE_MASK           0x3
#define GCSQC_PVSF_NOREMOVE            0x80

//--- SendFlags (движковые, fteqw server.h:588-592; для справки) ---
#define GCSQC_SENDFLAGS_PRESENT        0x1u   // эдикт присутствует у клиента
#define GCSQC_SENDFLAGS_REMOVED        0x2u   // эдикт удалён (обработка потери пакета)
#define GCSQC_SENDFLAGS_SHIFT          2u     // пользовательские биты начинаются с 0
// Пользовательские биты для setsendneeded(subject,flags,to) и аргумента sendflags
// в GAME_EDICT_CSQCSEND — на усмотрение мода (движок сдвигает их на SENDFLAGS_SHIFT).
// Соглашение для примера TF2003:
#define GCSQC_SENDFLAG_STATE           0x1    // изменилось состояние флага

//--- Инфокей клиента (движок выставляет для подключённых) ---
#define GCSQC_INFOKEY_CSQCACTIVE       "csqcactive"

//--- Serverinfo star-ключи (выставляются движком, для справки) ---
#define GCSQC_SVKEY_CSPROGS            "*csprogs"
#define GCSQC_SVKEY_CSPROGSSIZE        "*csprogssize"
#define GCSQC_SVKEY_CSPROGSNAME        "*csprogsname"
#define GCSQC_SVKEY_ANYCSQC            "anycsqc"

//--- Команды клиента включения CSQC (обрабатывает движок, для справки) ---
#define GCSQC_CMD_ENABLECSQGC          "enablecsqc"
#define GCSQC_CMD_DISABLECSQGC         "disablecsqc"

//===========================================================================
// API
//===========================================================================

// Полная поддержка CSQC движком: true только на fteqw. clientstat/pointerstat
// мапятся только там; на mvdsv их нет вовсе, а setsendneeded — заглушка-ошибка.
qboolean G_CSQC_OK( void );

// Есть ли у клиента активный csqc-модуль (infokey "csqcactive", 1/0).
// На любом движке безопасен.
qboolean G_ClientCSQCActive( gedict_t *client );

// Разбор и обработка sendevent клиент→сервер (экспорт GAME_QCREQUEST,
// вызывается движком: self=client, arg0=argcount). Имя события — trap_CmdArgv(0);
// значения аргументов — трап qcrequestarg (см. G_Ext_QCRequestArg).
// Возврат !=0 = обработано. Референс-реализация логирует под cvar "developer";
// конкретные события мод вешает на эту же точку входа.
int G_GameQCRequest( int argcount );

// Назначить эдикту функцию сериализации CSQC. Движок вызывает её через
// экспорт GAME_EDICT_CSQCSEND: self=ent, other=viewer, аргумент sendflags.
// Колбек пишет payload через G_CSQC_Write* и возвращает 0=не слать, !=0=слать.
// На mvdsv флаг эдикта выставить можно, но эмит отсутствует — безвредно.
void ExtFieldSetSendEntity( gedict_t *ed, func_t callback );

// Запись CSQC-payload. Допустимо только внутри SendEntity-колбека (движок
// активен только на время GAME_EDICT_CSQCSEND); на движках без MSG_CSQC — no-op.
void G_CSQC_WriteByte( int data );
void G_CSQC_WriteChar( int data );
void G_CSQC_WriteShort( int data );
void G_CSQC_WriteLong( int data );
void G_CSQC_WriteAngle( float data );
void G_CSQC_WriteCoord( float data );
void G_CSQC_WriteString( const char *data );
void G_CSQC_WriteEntity( gedict_t *ed );

// Регистрация клиентского/глобального стата (номера GCSQC_STAT_FIRST..
// GCSQC_STAT_MAX; 0..31 заняты движком). Клиентский CSQC читает их через
// getstatf/getstati.
// fieldoffset — смещение поля относительно начала gedict_t (у нас entvars_t
// лежит по offset 0, QVM API>=16 => wasted_edict_t_size==0).
// На движках без clientstat/pointerstat — no-op.
void G_RegisterClientStat( int statnum, int type, int fieldoffset );
void G_RegisterPointerStat( int statnum, int type, void *ptr );

// Отправить CSQC-событие (svcfte_cgamepacket) всем клиентам с активным CSQC
// в радиусе слышимости. name — имя события, за ним (опционально) аргументы
// через G_CSQC_Write* в MSG_MULTICAST. Клиентский CSQC получает его в
// CSQC_Parse_Event. No-op, если CSQC не поддерживается или CSQC-клиентов нет.
void G_SendCSQCEvent( vec3_t org, const char *name );

// Демонстрационный пример использования (cvar "g_csqc", default 0; включение:
// fteqw — "g_csqc 1", mvdsv — "set g_csqc 1" в конфиге/консоли, применяется при
// следующей загрузке карты):
// регистрация статов игрока/команд и SendEntity для флагов (tf_flag.mdl).
void G_CSQC_Example_RegisterStats( void );
int  G_CSQC_Example_FlagSendEntity( int sendflags );
void G_CSQC_Example_PlaceItem( gedict_t *ent );
void G_CSQC_Example_Frame( void );   // per-frame; периодический дирт CSQC-сущностей (~1 c)

#endif
