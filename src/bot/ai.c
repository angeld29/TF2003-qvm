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
 *  $Id: ai.c,v 1.9 2005-06-03 21:19:46 AngelD Exp $
 */
#include "g_local.h"


#define BOT_COUNT 1
qboolean ThrowGrenade(  )
{
	gedict_t *eTemp;
	gedict_t *eGren;

	if ( !( self->tfstate & TFSTATE_GRENPRIMED ) )
	{
		return false;
	}

	eGren = NULL;
	for ( eTemp = world; ( eTemp = trap_find( eTemp, FOFS( s.v.classname ), "primer" ) ); )
	{
		if ( PROG_TO_EDICT( eTemp->s.v.owner ) == self )
			eGren = eTemp;
	}
	if ( eGren )
	{
		if ( eGren->heat < g_globalvars.time + 0.6 )
			self->s.v.impulse = TF_GRENADE_T;
		return true;
	}
	return false;

}

void SelectWP();
void Bot_AI(  )
{
	if ( self->s.v.health <= 0 )
	{
		if ( self->fBotMessageTime == 0 )
		{
			self->fBotMessageTime =
			    g_globalvars.time + tf_data.botResupplyBetweenKills * 1.5 +
			    g_random(  ) * BOT_COUNT * 0.3;
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
	if ( !self->playerclass )
	{
		if ( self->current_menu > MENU_DEFAULT )
			self->s.v.impulse = BotImpulseMenu(  );
		return;
	}
      retry:
	switch ( self->action )
	{
	case BOT_IDLE:
//              if ( CheckForResupply(  ) == true && tf_data.botResupplyBetweenKills )
//              {
//                      self->action = BOT_RESUPPLYING;
//              } else
		self->action = BOT_DEFEND;
		goto retry;
	case BOT_DEFEND:
//	        if( VectorCompareF( self->waypoint1, 0, 0, 0 ) )
                SelectWP();
		break;
/*		if ( !ThrowGrenade(  ) )
			ReloadWeapons(  );
		if ( self->team_no == 1 )
		{
			if ( self->s.v.origin[1] < -610 )
			{
				SetVector( self->waypoint1, 256, -550, -936 );
				break;
			}
		} else
		{
			if ( self->s.v.origin[1] > 260 )
			{
				SetVector( self->waypoint1, 256, 200, -936 );
				break;
			}
		}

		//if ( self->FlashTime == 0 )
		FindTargets(  );

		if ( self->target1 )
		{
			self->action = BOT_FIGHTING;
			goto retry;
		}
		break;*/
	case BOT_RESUPPLYING:
		self->action = BOT_IDLE;
		goto retry;
/*		if ( !ThrowGrenade(  ) )
			ReloadWeapons(  );
		if ( !CheckForResupply(  ) )
		{
			self->action = BOT_IDLE;
			goto retry;
		}

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
		}*/
		break;

	case BOT_FIGHTING:
	        self->action = BOT_IDLE;
	        goto retry;
/*		//UpdateTargets //remove lost targets
//              FindTargets(  );
//              if ( !self->target1 )
		{
			self->action = BOT_IDLE;
			goto retry;
		}
//              if ( self->attack_finished > g_globalvars.time )
//              {
		//DodgeEnemy
//              } else
		{
//                      AttackTargets(  );
		}*/
		break;
	}

//      DodgeEnemyGrenades(  );

	if ( self->current_menu > MENU_DEFAULT )
		self->s.v.impulse = BotImpulseMenu(  );

	DoMovement(  );
}

//vec3_t end_pos = { 1510,0,24 };
vec3_t end_pos = { 0,0,24 };

void SelectWP()
{

        if( self->s.v.health <= 0 )
                return;
	if ( !VectorCompareF( self->waypoint1, 0, 0, 0 ) )
	{
	        return;
	}

	
	self->wp_path = WaypointFindPath( self->s.v.origin, end_pos );
	if(!self->wp_path)
	{
	        return;
	}
	VectorCopy( self->wp_path->link->src_wp->origin, self->waypoint1 );

/*	while( path )
	{
	        tmp = path->next;
	        free(path);
	        path = tmp;
	}

	if( self->s.v.origin[0] > 200 )
	{
	        SetVector( self->waypoint1, 1800, 0, 24 );
	        return;
	}
	        
	nexty = g_random() * 700 *2 + (- 700); 

	SetVector( self->waypoint1, 200, nexty, 24 );
	SetVector( self->waypoint2, 1800, 0, 24 );*/

}
void SelectWPOld()
{
        int nexty;

        nexty = g_random() * 600 *2 + (- 600); 

      	if ( self->team_no == 1 )
      	{
      		if ( self->s.v.origin[0] < -1300 )
      		{
      			SetVector( self->waypoint1, 1600, nexty, 24 );
      		}
      	} else
      	{
      		if ( self->s.v.origin[0] > 1300 )
      		{
      			SetVector( self->waypoint1, -1600, nexty, 24 );
      		}
      		
      	}
        return;

}

