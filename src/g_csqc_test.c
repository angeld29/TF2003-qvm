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
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307  USA
 *
 *
 *  $Id$
 */

#include "g_local.h"

/*
 * CSQC test support (server side).
 *
 * Тест-поддержка CSQC-модуля вынесена из основных файлов: SP_worldspawn()
 * вызывает только G_CSQC_Test_Precache(). Модели, которые модуль уже получает
 * из обычного precache игры (progs/player.mdl world.c:221, progs/backpack.mdl
 * world.c:250, progs/tf_flag.mdl world.c:264, progs/s_light.spr world.c:312),
 * здесь не дублируются. maps/b_rock1.bsp в моде precache-ится только условно
 * (items.c:1126/1232 — при спавне ammo с WEAPON_BIG2/WEAPON_ROCKET|WEAPON_BIG),
 * поэтому CSQC-клиенту он недоступен на картах без такого ammo; precache-им
 * безусловно при g_csqc. Детали рендера CSQC brush-моделей —
 * docs/plans/ezquake_csqc_client_brushmodel_render.md, docs/ezquake_csqc_client_parity_audit.md.
 */
void G_CSQC_Test_Precache( void )
{
	if ( !cvar( "g_csqc" ) )
		return;

	trap_precache_model( "maps/b_rock1.bsp" );
}
