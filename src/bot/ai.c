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
 *  $Id: ai.c,v 1.3 2005-05-06 14:01:37 AngelD Exp $
 */
#include "g_local.h"


#define BOT_COUNT 1
void Bot_AI(  )
{
	float   fTemp;
	gedict_t *eTemp;
	gedict_t *eGren;
	vec3_t  vtemp2;

	if ( self->s.v.health <= 0 )
	{
		if ( self->action & ( BOT_RESUPPLYING + BOT_MOVING ) )
			fTemp = 0;
		if ( self->fBotMessageTime == 0 )
		{
			self->fBotMessageTime =
			    g_globalvars.time + tf_data.botResupplyBetweenKills * 1.5 + g_random(  ) * BOT_COUNT * 0.3;
			return;
		}
		if ( self->fBotMessageTime < g_globalvars.time )
		{
			ClearAllWaypoints(  );
			ClearAllTargets(  );
			self->obs_time = 0;
			self->action = BOT_IDLE;
			self->s.v.button0 = floor( g_random(  ) * 2 );
			self->s.v.button1 = 0;
			self->s.v.button2 = 0;
			self->keys = 0;
			self->fBotMessageTime = 0;
			return;
		}
	}
	if ( ( self->action & ( BOT_DEFEND + BOT_FIGHTING ) ) && self->FlashTime == 0 )
		FindTargets(  );
	if ( self->target1 )
		AttackTargets(  );

	if ( self->action & BOT_RESUPPLYING )
	{
		ReloadWeapons(  );
		if ( self->team_no == 1 )
		{
			vtemp2[0] = self->s.v.origin[0] - 256;
			vtemp2[1] = self->s.v.origin[1] + 840;
			vtemp2[2] = self->s.v.origin[2] + 936;

			if ( vlen( vtemp2 ) < 200 )
				SetVector( self->waypoint1, 256, -840, -936 );
			else
			{
				SetVector( self->waypoint1, 256, -550, -936 );
				SetVector( self->waypoint2, 256, -840, -936 );
			}
		} else
		{
			vtemp2[0] = self->s.v.origin[0] - 256;
			vtemp2[1] = self->s.v.origin[1] - 430;
			vtemp2[2] = self->s.v.origin[2] + 936;

			if ( vlen( vtemp2 ) < 200 )
				SetVector( self->waypoint1, 256, 430, -936 );
			else
			{
				SetVector( self->waypoint1, 256, 200, -936 );
				SetVector( self->waypoint2, 256, 430, -936 );
			}
		}
		self->action = BOT_MOVING;
	}
	if ( self->action & BOT_DEFEND )
	{
		PickBestWeapon(  );
		if ( CheckForResupply(  ) == true && tf_data.botResupplyBetweenKills )
			GoForResupply(  );
	}
	if ( self->action & BOT_IDLE )
	{
		ReloadWeapons(  );
		if ( CheckForResupply(  ) == true && tf_data.botResupplyBetweenKills )
			GoForResupply(  );
		else
		{
			if ( self->team_no == 1 )
			{
				if ( self->s.v.origin[1] < -600 )
				{
					SetVector( vtemp2, 256, -550, -936 );
					GotoLocation( vtemp2 );
				} else
				{
					self->keys = 0;
					self->action = BOT_DEFEND;
				}
			} else
			{
				if ( self->s.v.origin[1] > 250 )
				{
					SetVector( vtemp2, 256, 200, -936 );
					GotoLocation( vtemp2 );
				} else
				{
					self->keys = 0;
					self->action = BOT_DEFEND;
				}
			}
		}
	}
	if ( self->action == BOT_MOVING )
	{
		PickBestWeapon(  );
		ReloadWeapons(  );
		DoMovement(  );
	}
/*	if ( self.debug_report_action )
	{
		bprint( 2, "My client number is:" );
		strTemp = ftos( self.fClientNo );
		bprint( 2, strTemp );
		bprint( 2, "\n" );
		self.debug_report_action = 0;
	}*/
	DodgeEnemyGrenades(  );
	if ( self->FlashTime > 0 )
	{
		if ( self->FlashTime > 23 )
		{
			ClearAllTargets(  );
			self->keys = KEY_MOVEBACK;
			self->s.v.button0 = 1;
		} else
		{
			self->keys = 0;
			self->s.v.button0 = 0;
			self->FlashTime = 0;
			self->action = BOT_IDLE;
		}
	}
	for ( eTemp = world; ( eTemp = trap_find( eTemp, FOFS( s.v.classname ), "timer" ) ); )
	{
		if ( eTemp->s.v.think != ( func_t ) ConcussionGrenadeTimer && 
		           eTemp->s.v.think != ( func_t ) OldConcussionGrenadeTimer)
			continue;

		if ( PROG_TO_EDICT( eTemp->s.v.owner ) != self )
			continue;

		if ( eTemp->s.v.health > 80 )
		{
			self->s.v.v_angle[0] += 18 * ( g_random(  ) - 0.5 );
			self->s.v.v_angle[1] += 18 * ( g_random(  ) - 0.5 );
		} else
		{
			if ( eTemp->s.v.health > 60 )
			{
				self->s.v.v_angle[0] += 9 * ( g_random(  ) - 0.5 );
				self->s.v.v_angle[1] += 9 * ( g_random(  ) - 0.5 );
			} else
				eTemp->s.v.health = 5;
		}
	}
	if ( self->obs_time > g_globalvars.time )
	{
		if ( self->action & ( BOT_DEFEND + BOT_IDLE ) )
			self->obs_time = 0;
//              else
//                      self->keys = self->keys; ??????
	}
	if ( self->current_menu > MENU_DEFAULT )
		self->s.v.impulse = BotImpulseMenu(  );
	if ( !self->target1 )
	{
		if ( ( self->tfstate & TFSTATE_GRENPRIMED ) || ( self->tfstate & FL_PARTIALGROUND ) )
		{
			eGren = NULL;
			for ( eTemp = world;
			      ( eTemp = trap_find( eTemp, FOFS( s.v.classname ), "primer" ) ); )
			{
				if ( PROG_TO_EDICT( eTemp->s.v.owner ) == self )
					eGren = eTemp;
			}
			if ( eGren )
			{
				if ( eGren->heat < g_globalvars.time + 0.6 )
					self->s.v.impulse = TF_GRENADE_T;
			}
		}
	}
}


void Bot_AI1(  )
{
	if ( self->s.v.health <= 0 || self->s.v.deadflag )
	{
		if ( self->s.v.button2 || self->s.v.button0 || self->s.v.button1 )
		{
			self->s.v.button2 = 0;
			self->s.v.button1 = 0;
			self->s.v.button0 = 0;

		} else
		{
			self->s.v.button2 = 1;
			self->s.v.button1 = 1;
			self->s.v.button0 = 1;
		}
		return;
	}

	self->keys = 0;
	if ( g_random(  ) > 0.3 )
		self->keys |= KEY_MOVELEFT;
	if ( g_random(  ) > 0.3 )
		self->keys |= KEY_MOVERIGHT;
	if ( g_random(  ) > 0.8 )
		self->keys |= KEY_MOVEBACK;
	if ( g_random(  ) > 0.3 )
		self->keys |= KEY_MOVEFORWARD;

	self->s.v.angles[1] += g_random(  ) * 60;
	self->s.v.angles[1] -= g_random(  ) * 60;

	if ( ( ( ( int ) ( self->s.v.flags ) ) & FL_ONGROUND ) && !self->s.v.button2 )
	{
		self->s.v.button2 = 1;
		self->s.v.button0 = 1;
	} else
	{
		self->s.v.button2 = 0;
		self->s.v.button0 = 0;
	}

}
