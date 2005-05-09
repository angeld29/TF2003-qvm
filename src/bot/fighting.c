/*
 *  QWProgs-TF2003
 *  Copyright (C) 2004  [sd] angel
 *
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
 *  $Id: fighting.c,v 1.3 2005-05-09 00:33:02 AngelD Exp $
 */
#include "g_local.h"

void ReloadWeapons(  )
{
	if ( ( self->tfstate & TFSTATE_GRENPRIMED ) || ( self->tfstate & TFSTATE_GRENTHROWING ) )
		return;

	if ( ( self->weapons_carried & WEAP_ROCKET_LAUNCHER ) && self->reload_rocket_launcher != 0 )
	{
		self->s.v.impulse = ( self->current_weapon == WEAP_ROCKET_LAUNCHER ) ? TF_RELOAD : 7;
		return;
	}
	if ( ( self->weapons_carried & WEAP_GRENADE_LAUNCHER ) && self->reload_grenade_launcher != 0 )
	{
		self->s.v.impulse = ( self->current_weapon == WEAP_GRENADE_LAUNCHER ) ? TF_RELOAD : 6;

		return;
	}
	if ( ( self->weapons_carried & WEAP_SUPER_SHOTGUN ) && self->reload_super_shotgun != 0 )
	{
		self->s.v.impulse = ( self->current_weapon == WEAP_SUPER_SHOTGUN ) ? TF_RELOAD : 3;

		return;
	}
	if ( ( self->weapons_carried & WEAP_SHOTGUN ) && self->reload_shotgun != 0 )
	{
		self->s.v.impulse = ( self->current_weapon == WEAP_SHOTGUN ) ? TF_RELOAD : 2;
	}
}

void ClearAllTargets(  )
{
	self->target1 = self->target2 = self->target3 = NULL;
	self->s.v.button0 = 0;
	self->s.v.button2 = 0;
}
