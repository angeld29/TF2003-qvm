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
 *  $Id: spectate.c,v 1.3 2004-12-23 03:16:15 AngelD Exp $
 */
//===========================================================================
// Spectator
// 
//===========================================================================
#include "g_local.h"
////////////////
// GlobalParams:
// time
// self
// params
///////////////
void SpectatorConnect(  )
{
	G_bprint( PRINT_MEDIUM, "Spectator %s entered the game\n", self->s.v.netname );

	self->s.v.goalentity = EDICT_TO_PROG( world );	// used for impulse 1 below

}

////////////////
// GlobalParams:
// self
///////////////
void SpectatorDisconnect(  )
{
	G_bprint( PRINT_MEDIUM, "Spectator %s  left the game\n", self->s.v.netname );
}

/*
================
SpectatorImpulseCommand

Called by SpectatorThink if the spectator entered an impulse
================
*/
void SpectatorImpulseCommand(  )
{
	gedict_t *goal;

	if ( self->s.v.impulse == 1 )
	{
		// teleport the spectator to the next spawn point
		// note that if the spectator is tracking, this doesn't do
		// much
		goal = PROG_TO_EDICT( self->s.v.goalentity );
		goal = trap_find( goal, FOFS( s.v.classname ), "info_player_deathmatch" );
		if ( !goal )
			goal = trap_find( world, FOFS( s.v.classname ), "info_player_deathmatch" );
		if ( goal )
		{
			setorigin( self, PASSVEC3( goal->s.v.origin ) );
			VectorCopy( goal->s.v.angles, self->s.v.angles );
			self->s.v.fixangle = true;	// turn this way immediately
		} else
			goal = world;
		self->s.v.goalentity = EDICT_TO_PROG( goal );
	}

	self->s.v.impulse = 0;
}

////////////////
// GlobalParams:
// time
// self
///////////////
void SpectatorThink(  )
{
	if ( self->s.v.impulse )
		SpectatorImpulseCommand(  );

}
