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
 *  $Id: coop.c,v 1.8 2006-11-29 23:19:23 AngelD Exp $
 */
#include "g_local.h"
void    key_touch(  );

void DroppedKeyThink(  )
{
	self->s.v.think = ( func_t ) SUB_Null;
	self->s.v.touch = ( func_t ) key_touch;
	self->s.v.owner = EDICT_TO_PROG( world );
}

char   *key_models1[3] = { "progs/w_s_key.mdl", "progs/m_s_key.mdl", "progs/b_s_key.mdl" };
char   *key_names1[3] = { "silver key", "silver runekey", "silver keycard" };
char   *key_sound1[3] = { "misc/medkey.wav", "misc/runekey.wav", "misc/basekey.wav" };

char   *key_models2[3] = { "progs/w_g_key.mdl", "progs/m_g_key.mdl", "progs/b_g_key.mdl" };
char   *key_names2[3] = { "gold key", "gold runekey", "gold keycard" };
char   *key_sound2[3] = { "misc/medkey.wav", "misc/runekey.wav", "misc/basekey.wav" };

void DropKey(  )
{

	if ( ( ( int ) self->s.v.items & IT_KEY1 ) || ( ( int ) self->s.v.items & IT_KEY2 ) )
	{
		newmis = spawn(  );
		g_globalvars.newmis = EDICT_TO_PROG( newmis );
		if ( ( int ) self->s.v.items & IT_KEY1 )
		{
			self->s.v.items = ( int ) self->s.v.items - ( ( int ) self->s.v.items & IT_KEY1 );
			newmis->s.v.items = IT_KEY1;

			setmodel( newmis, key_models1[world->worldtype] );
			newmis->s.v.netname = key_names1[world->worldtype];
			newmis->s.v.noise = key_sound1[world->worldtype];

		} else
		{
			if ( ( int ) self->s.v.items & IT_KEY2 )
			{
				self->s.v.items = ( int ) self->s.v.items - ( ( int ) self->s.v.items & IT_KEY2 );
				newmis->s.v.items = IT_KEY2;
				setmodel( newmis, key_models2[world->worldtype] );
				newmis->s.v.netname = key_names2[world->worldtype];
				newmis->s.v.noise = key_sound2[world->worldtype];
			}
		}
		newmis->s.v.owner = EDICT_TO_PROG( self );
		newmis->s.v.touch = ( func_t ) SUB_Null;
		setorigin( newmis, self->s.v.origin[0], self->s.v.origin[1], self->s.v.origin[2] + 16 );
		trap_makevectors( self->s.v.v_angle );

		VectorNormalize( g_globalvars.v_forward );
		VectorScale( g_globalvars.v_forward, 300, newmis->s.v.velocity );
		newmis->s.v.velocity[2] += 200;
		//newmis->s.v.velocity = normalize(v_forward) * 300 + '0 0 200';

		newmis->s.v.movetype = MOVETYPE_TOSS;
		newmis->s.v.solid = SOLID_TRIGGER;
		newmis->s.v.deadflag = DEAD_DYING;
		setsize( newmis, -16, -16, -24, 16, 16, 32 );
		newmis->s.v.think = ( func_t ) DroppedKeyThink;
		newmis->s.v.nextthink = g_globalvars.time + 1.5;
	} else
		G_sprint( self, 2, "You don't have a key\n" );
}

int DoorShouldOpen(  )
{
	gedict_t *ptr;
	int     plyrcount;
	gedict_t *plyr1 = NULL;
	gedict_t *plyr2 = NULL;

	if ( coop != 2 )
		return 1;
	plyrcount = 0;
	ptr = trap_find( world, FOFS( s.v.classname ), "player" );
	while ( ptr != world )
	{
		if ( !( ptr->tf_items & ( int ) self->s.v.items ) && ptr->playerclass
		     && ptr->s.v.solid && ptr->s.v.model )
		{
			plyrcount++;
			if ( plyrcount == 1 )
				plyr1 = ptr;
			else
			{
				if ( plyrcount == 2 )
					plyr2 = ptr;
			}
		}
		ptr = trap_find( ptr, FOFS( s.v.classname ), "player" );
	}
	if ( plyrcount )
	{
		if ( plyrcount == 1 )
		{
			G_bprint( 2, "%s needs", plyr1->s.v.netname );
		} else
		{
			if ( plyrcount == 2 )
			{
				G_bprint( 2, "%s and %s", plyr1->s.v.netname, plyr2->s.v.netname );
			} else
				G_bprint( 2, "More players need" );
		}
		G_bprint( 2, " to unlock the " );
		if ( ( int ) self->s.v.items & IT_KEY1 )
			G_bprint( 2, "silver" );
		else
			G_bprint( 2, "gold" );
		G_bprint( 2, " door\n" );
		return 0;
	}
	G_bprint( 2, "The " );
	if ( ( int ) self->s.v.items & IT_KEY1 )
		G_bprint( 2, "silver" );
	else
		G_bprint( 2, "gold" );
	G_bprint( 2, " door has been unlocked\n" );
	return 1;
}
