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
 *  $Id: player.c,v 1.16 2005-11-28 18:30:57 AngelD Exp $
 */
#include "g_local.h"


void player_touch(  )
{
	gedict_t *Bio;
	gedict_t *te;

	float   found;

	if ( !tfset(invis_only) && ( self->playerclass == PC_SPY || other->playerclass == PC_SPY ) )
	{
		if ( streq( other->s.v.classname, "player" ) )
		{
			if ( self->undercover_team || self->undercover_skin )
			{
				if ( ( other->playerclass == PC_SPY || other->playerclass == PC_SCOUT )
				     && !TeamFortress_isTeamsAllied (other->team_no , self->team_no) )
				{
					TF_AddFrags( other, 1 );
					G_bprint( 1, "%s uncovered an enemy spy!\n", other->s.v.netname );
					Spy_RemoveDisguise( self );
				}
			}
			if ( other->undercover_team || other->undercover_skin )
			{
				if ( ( self->playerclass == PC_SPY || self->playerclass == PC_SCOUT )
				     && !TeamFortress_isTeamsAllied (self->team_no , other->team_no) )
				{
					TF_AddFrags( self, 1 );
					G_bprint( 1, "%s uncovered an enemy spy!\n", self->s.v.netname );
					Spy_RemoveDisguise( other );
				}
			}
		}
	}
	if ( self->can_give_goal && tf_data.cb_prematch_time < g_globalvars.time )
	{
		if ( streq( other->s.v.classname, "player" ) && other->playerclass && other->team_no == self->team_no )
		{
			te = trap_find( world, FOFS( s.v.classname ), "item_tfgoal" );
			while ( te )
			{
				if ( te->s.v.owner == EDICT_TO_PROG( self ) )
				{
					if ( self->last_goal_give < g_globalvars.time )
					{
						gedict_t* Goal = trap_find( world, FOFS( s.v.classname ), "info_tfdetect" );
						tfgoalitem_TransferToPlayer( te, self, other, Goal );
						sound( self, 2, te->s.v.noise, 1, 1 );
						self->last_goal_give = g_globalvars.time + 1;
						break;
					}
				}
				te = trap_find( te, FOFS( s.v.classname ), "item_tfgoal" );
			}
			te = NULL;
		}
	}
	if ( ( self->tfstate & TFSTATE_INFECTED ) && tf_data.cb_prematch_time < g_globalvars.time )
	{
		if ( streq( other->s.v.classname, "player" ) && other->playerclass )
		{
			if ( !( other->tfstate & TFSTATE_INFECTED ) )
			{
				if ( other->playerclass != PC_MEDIC )
				{
					if ( !
					     ( ( teamplay & TEAMPLAY_NOEXPLOSIVE )
                           && PROG_TO_EDICT( self->s.v.owner )->team_no
					       && PROG_TO_EDICT( self->s.v.owner )->team_no ==
					       PROG_TO_EDICT( self->s.v.enemy )->team_no) )
					{
						found = 0;
						te = trap_find( world, FOFS( s.v.classname ), "timer" );
						while ( te && !found )
						{
							if ( PROG_TO_EDICT( te->s.v.owner ) == self
							     && te->s.v.think == ( func_t ) BioInfection_Decay )
							     {
								found = 1;
								break;
							     }
							else
								te = trap_find( te, FOFS( s.v.classname ), "timer" );
						}
						if(!te)
						{
							G_dprintf( "***BUG TFSTATE_INFECTED without BioInfection_Decay timer ***\n" );
							return;
						}
						Bio = spawn(  );
						Bio->s.v.nextthink = 2;
						Bio->s.v.think = ( func_t ) BioInfection_Decay;
						Bio->s.v.owner = EDICT_TO_PROG( other );
						Bio->s.v.classname = "timer";
						Bio->s.v.enemy = te->s.v.enemy;
						other->tfstate = other->tfstate | TFSTATE_INFECTED;
						other->infection_team_no = self->infection_team_no;
						G_sprint( other, 1, "You have been infected by %s!\n", self->s.v.netname );
						G_sprint( self, 1, "You have infected %s!\n", other->s.v.netname );
					}
				}
			}
		}
	}
}
void player_stand1(  )
{
	self->s.v.frame = 17;
	self->s.v.think = ( func_t ) player_stand1;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 0;
	if ( self->s.v.velocity[0] || self->s.v.velocity[1] )
	{
		self->walkframe = 0;
		player_run();
		return;
	}
	if ( self->current_weapon <= 16 )
	{
		if ( self->walkframe >= 12 )
			self->walkframe = 0;
		self->s.v.frame = 17 + self->walkframe;
	} else
	{
		if ( self->walkframe >= 5 )
			self->walkframe = 0;
		self->s.v.frame = 12 + self->walkframe;
	}
	self->walkframe = self->walkframe + 1;
}

void player_run(  )
{
	self->s.v.frame = 6;
	self->s.v.think = ( func_t ) player_run;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 0;
	if ( !self->s.v.velocity[0] && !self->s.v.velocity[1] )
	{
		self->walkframe = 0;
		player_stand1(  );
		return;
	}
	if ( self->current_weapon <= 16 )
	{
		if ( self->walkframe >= 6 )
			self->walkframe = 0;
		self->s.v.frame = 0 + self->walkframe;
	} else
	{
		if ( self->walkframe >= 6 )
			self->walkframe = 0;
		self->s.v.frame = self->s.v.frame + self->walkframe;
	}
	self->walkframe = self->walkframe + 1;
}
void player_shot_think()
{
	self->s.v.nextthink += 0.1;
    self->s.v.frame += 1;
    self->s.v.weaponframe += 1;
    if( self->s.v.frame >= self->frame_info.end ){
        self->s.v.think = (func_t)player_run;
    }
}

void player_shot( int start )
{
	self->s.v.frame = start;
	self->s.v.think = (func_t)player_shot_think;
	self->s.v.nextthink = g_globalvars.time + 0.1;
    self->frame_info.start = start;
    self->frame_info.end = start + 5;

	self->s.v.weaponframe = 1;
	muzzleflash(  );
}

void player_autorifle1(  )
{
	self->s.v.frame = 113;
	self->s.v.think = ( func_t ) player_autorifle2;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 1;
	muzzleflash(  );
}

void player_autorifle2(  )
{
	self->s.v.frame = 114;
	self->s.v.think = ( func_t ) player_autorifle3;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 2;
}

void player_autorifle3(  )
{
	self->s.v.frame = 118;
	self->s.v.think = ( func_t ) player_run;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 6;
}

void player_naxe4(  )
{
	self->s.v.frame += 1;
	self->s.v.weaponframe += 1;
	self->s.v.think = ( func_t ) player_run;
	self->s.v.nextthink = g_globalvars.time + 0.1;
}

void player_naxe3(  )
{
	self->s.v.frame += 1;
	self->s.v.weaponframe += 1;
	self->s.v.think = ( func_t ) player_naxe4;
	self->s.v.nextthink = g_globalvars.time + 0.1;

    self->frame_info.last_func();
}

void player_naxe2(  )
{
	self->s.v.frame += 1;
	self->s.v.weaponframe += 1;
	self->s.v.think = ( func_t ) player_naxe3;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_naxe( int frame, int wframe, th_die_func_t f  )
{
	self->s.v.frame = frame;
	self->s.v.think = ( func_t ) player_naxe2;
	self->s.v.nextthink = g_globalvars.time + 0.1;
    self->frame_info.last_func = f;

	self->s.v.weaponframe = wframe;
}

void player_chain1(  )
{
	self->s.v.frame = 137;
	self->s.v.think = ( func_t ) player_chain1a;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 1;
	Throw_Grapple(  );
}

void player_chain1a(  )
{
	self->s.v.frame = 137;
	self->s.v.think = ( func_t ) player_chain2;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 2;
}

void player_chain2(  )
{
	self->s.v.frame = 138;
	self->s.v.think = ( func_t ) player_chain2a;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 3;
}

void player_chain2a(  )
{
	self->s.v.frame = 138;
	self->s.v.think = ( func_t ) player_chain3;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 4;
}

void player_chain3(  )
{
	self->s.v.frame = 139;
	self->s.v.think = ( func_t ) player_chain3;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 6;
	if ( !self->hook_out )
	{
		player_chain5(  );
		return;
	}
	if ( vlen( self->s.v.velocity ) >= 750 )
	{
		player_chain4(  );
		return;
	}
}
void player_chain4(  )
{
	self->s.v.frame = 73;
	self->s.v.think = ( func_t ) player_chain4;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 6;
	if ( !self->hook_out )
	{
		player_chain5(  );
		return;
	}
	if ( vlen( self->s.v.velocity ) < 750 )
	{
		player_chain3(  );
		return;
	}
}
void player_chain5(  )
{
	self->s.v.frame = 140;
	self->s.v.think = ( func_t ) player_run;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 0;
}

void player_nail1(  )
{
	self->s.v.frame = 103;
	self->s.v.think = ( func_t ) player_nail2;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	muzzleflash(  );
	if ( !self->s.v.button0 || intermission_running )
	{
		player_run( );
		return;
	}
	self->s.v.weaponframe = self->s.v.weaponframe + 1;
	if ( self->s.v.weaponframe == 9 )
		self->s.v.weaponframe = 1;
	SuperDamageSound(  );
	if ( !self->nailpos )
	{
		W_FireSpikes( 4 );
		self->nailpos = 1;
	} else
	{
		W_FireSpikes( -4 );
		self->nailpos = 0;
	}
	Attack_Finished( 0.2 );
}

void player_nail2(  )
{
	self->s.v.frame = 104;
	self->s.v.think = ( func_t ) player_nail1;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	if ( !self->s.v.button0 || intermission_running )
	{
		player_run(  );
		return;
	}
	self->s.v.weaponframe = self->s.v.weaponframe + 1;
	if ( self->s.v.weaponframe == 9 )
		self->s.v.weaponframe = 1;
	Attack_Finished( 0.2 );
}

void player_assaultcannonup1(  )
{
	self->s.v.frame = 103;
	self->s.v.think = ( func_t ) player_assaultcannonup2;
	self->s.v.nextthink = g_globalvars.time + 0.1;


	if ( !self->s.v.button0 || self->s.v.ammo_shells <= self->assault_min_shells || intermission_running )
	{
		self->tfstate = self->tfstate - ( self->tfstate & TFSTATE_AIMING );
		TeamFortress_SetSpeed( self );
		self->count = 1;
		self->heat = 0;
		player_assaultcannondown1(  );
		return;
	}
	self->fire_held_down = 1;
	if ( self->heat == 1 )
		sound( self, 1, "weapons/asscan1.wav", 1, 1 );
	SuperDamageSound(  );
	Attack_Finished( 0.1 );
	if ( self->heat != 2 && self->heat != 4 )
	{
		if ( self->s.v.weaponframe >= 3 )
			self->s.v.weaponframe = 0;
		else
			self->s.v.weaponframe = self->s.v.weaponframe + 1;
	}
	self->heat = self->heat + 1;
	if ( self->heat >= 7 )
	{
		self->heat = 0;
		self->ScanRange = 0;
		player_assaultcannon1(  );
	}
}
void player_assaultcannonup2(  )
{
	self->s.v.frame = 103;
	self->s.v.think = ( func_t ) player_assaultcannonup1;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	if ( !self->s.v.button0 || self->s.v.ammo_shells <= self->assault_min_shells || intermission_running )
	{
		self->tfstate = self->tfstate - ( self->tfstate & TFSTATE_AIMING );
		TeamFortress_SetSpeed( self );
		self->count = 1;
		self->heat = 0;
		player_assaultcannondown1(  );
		return;
	}
	SuperDamageSound(  );
	Attack_Finished( 0.1 );
	if ( self->heat != 2 && self->heat != 4 && self->heat != 7 )
	{
		if ( self->s.v.weaponframe == 2 && self->heat >= 9 )
			self->s.v.weaponframe = 0;
		else
		{
			if ( self->s.v.weaponframe >= 3 )
				self->s.v.weaponframe = 0;
			else
				self->s.v.weaponframe = self->s.v.weaponframe + 1;
		}
	}
	self->heat = self->heat + 1;
	if ( self->heat >= 13 )
	{
		self->heat = 0;
		self->ScanRange = 0;
		player_assaultcannon1(  );
	}
}
static void player_assaultcannon( gedict_t* self,  int frame )
{
	vec3_t  v;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	VectorCopy( self->s.v.velocity, v );
	v[2] = 0;
#ifndef NEWHWGUY
    if( frame == 2 )
        stuffcmd( self, "v_idlescale 0\n" );
#endif
// if (vlen(self->s.v.velocity) < 50 && (self.flags & #FL_ONGROUND)) {
	if ( vlen( v ) < 20 && ( ( int ) self->s.v.flags & FL_ONGROUND ) )
	{
		if ( self->s.v.weaponframe == 2 )
			self->s.v.weaponframe = 4;
		else
			self->s.v.weaponframe = 2;

        if( frame == 1 )
        {
            muzzleflash(  );
            sound( self, 1, "weapons/asscan2.wav", 1, 1 );
        }
		SuperDamageSound(  );
		W_FireAssaultCannon(  );

        if( frame == 1 ){
#ifndef NEWHWGUY
		stuffcmd( self, "v_idlescale %d\n", min( (int)(self->heat*4), 20) );
#endif
        }else{
            self->heat = self->heat + 0.1;
            stuffcmd( self, "bf\n" );
        }
	} else
	{
		if ( self->s.v.weaponframe == 2 )
			self->s.v.weaponframe = 0;
		else
			self->s.v.weaponframe = 2;

        if( frame == 1 ){
            sound( self, 1, "weapons/asscan4.wav", 1, 1 );
#ifndef NEWHWGUY
            stuffcmd( self, "v_idlescale 5\n" );
#endif
        }
	}
	if ( !self->s.v.button0 || self->s.v.ammo_shells <= self->assault_min_shells || intermission_running )
	{
#ifndef  NEWHWGUY
		stuffcmd( self, "v_idlescale 0\n" );
#endif
		self->tfstate = self->tfstate - ( self->tfstate & TFSTATE_AIMING );
		TeamFortress_SetSpeed( self );
		self->s.v.weaponframe = 0;
		self->count = 1;
		player_assaultcannondown1(  );
		return;
	}
	Attack_Finished( 0.1 );

}
void player_assaultcannon1(  )
{
	self->s.v.frame = 103;
	self->s.v.think = ( func_t ) player_assaultcannon2;
    player_assaultcannon( self, 1 );
}

void player_assaultcannon2(  )
{

	self->s.v.frame = 104;
	self->s.v.think = ( func_t ) player_assaultcannon1;
    player_assaultcannon( self,  2 );
}

void player_assaultcannondown1(  )
{
//      gedict_t *tcool;

	self->s.v.frame = 103;
	self->s.v.think = ( func_t ) player_assaultcannondown1;
	self->s.v.nextthink = g_globalvars.time + 0.1;


	if ( self->count == 1 )
		sound( self, 1, "weapons/asscan3.wav", 1, 1 );
	if ( self->count >= 15 )
	{
		self->heat = 0;
		self->fire_held_down = 0;
		self->tfstate = self->tfstate - ( self->tfstate & TFSTATE_AIMING );
		TeamFortress_SetSpeed( self );
		if ( self->s.v.ammo_shells <= self->assault_min_shells || self->s.v.ammo_cells < 4 )
		{
			self->last_weapon = self->current_weapon;
			self->last_weaponmode = self->weaponmode;
			self->current_weapon = W_BestWeapon(  );
			W_SetCurrentAmmo(  );
			W_PrintWeaponMessage(  );
			return;
		}
		player_run(  );
		return;
	}
	if ( self->count != 8 && self->count != 10 && self->count != 12 && self->count != 14 )
	{
		if ( self->s.v.weaponframe == 3 )
			self->s.v.weaponframe = 0;
		else
			self->s.v.weaponframe = self->s.v.weaponframe + 1;
	}
	self->count = self->count + 1;
	Attack_Finished( 0.1 );
}

void player_light1(  )
{
	self->s.v.frame = 105;
	self->s.v.think = ( func_t ) player_light2;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	muzzleflash(  );
	if ( !self->s.v.button0 || intermission_running )
	{
		player_run(  );
		return;
	}
	self->s.v.weaponframe = self->s.v.weaponframe + 1;
	if ( self->s.v.weaponframe == 5 )
		self->s.v.weaponframe = 1;
	SuperDamageSound(  );
	W_FireLightning(  );
	Attack_Finished( 0.2 );
}

void player_light2(  )
{
	self->s.v.frame = 106;
	self->s.v.think = ( func_t ) player_light1;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	if ( !self->s.v.button0 || intermission_running )
	{
		player_run(  );
		return;
	}
	self->s.v.weaponframe = self->s.v.weaponframe + 1;
	if ( self->s.v.weaponframe == 5 )
		self->s.v.weaponframe = 1;
	SuperDamageSound(  );
	W_FireLightning(  );
	Attack_Finished( 0.2 );
}



void    DeathBubbles( int num_bubbles );

void PainSound(  )
{
	float   rs;

	if ( self->s.v.health < 0 )
		return;
	if ( streq( damage_attacker->s.v.classname, "teledeath" ) )
	{
		sound( self, 2, "player/teledth1.wav", 1, 0 );
		return;
	}
	if ( self->s.v.watertype == -3 && self->s.v.waterlevel == 3 )
	{
		DeathBubbles( 1 );
		if ( g_random(  ) > 0.5 )
			sound( self, 2, "player/drown1.wav", 1, 1 );
		else
			sound( self, 2, "player/drown2.wav", 1, 1 );
		return;
	}
	if ( self->s.v.watertype == -4 )
	{
		if ( g_random(  ) > 0.5 )
			sound( self, 2, "player/lburn1.wav", 1, 1 );
		else
			sound( self, 2, "player/lburn2.wav", 1, 1 );
		return;
	}
	if ( self->s.v.watertype == -5 )
	{
		if ( g_random(  ) > 0.5 )
			sound( self, 2, "player/lburn1.wav", 1, 1 );
		else
			sound( self, 2, "player/lburn2.wav", 1, 1 );
		return;
	}
	if ( self->pain_finished > g_globalvars.time )
	{
		self->axhitme = 0;
		return;
	}
	self->pain_finished = g_globalvars.time + 0.5;
	if ( self->axhitme == 1 )
	{
		self->axhitme = 0;
		sound( self, 2, "player/axhit1.wav", 1, 1 );
		return;
	}
	rs = ( int ) ( g_random(  ) * 5 + 1 );
	self->s.v.noise = "";
	if ( rs == 1 )
		self->s.v.noise = "player/pain1.wav";
	else
	{
		if ( rs == 2 )
			self->s.v.noise = "player/pain2.wav";
		else
		{
			if ( rs == 3 )
				self->s.v.noise = "player/pain3.wav";
			else
			{
				if ( rs == 4 )
					self->s.v.noise = "player/pain4.wav";
				else
				{
					if ( rs == 5 )
						self->s.v.noise = "player/pain5.wav";
					else
						self->s.v.noise = "player/pain6.wav";
				}
			}
		}
	}
	sound( self, 2, self->s.v.noise, 1, 1 );
	return;
}

void player_pain( gedict_t * attacker, float take  )
{
	if ( self->s.v.weaponframe )
	{
		if ( tf_data.deathmsg == DMSG_TEAMKILL )
			PainSound(  );
		return;
	}
	if ( self->invisible_finished > g_globalvars.time )
		return;
	if ( self->is_feigning )
	{
		PainSound(  );
		return;
	}
	if ( self->s.v.button0 && self->current_weapon == WEAP_ASSAULT_CANNON )
		return;
	PainSound(  );
	self->s.v.weaponframe = 0;
	if ( self->current_weapon <= 16 )
        set_think( self, 29 , 34, NULL, NULL, player_run );
		//player_axpain1(  );
	else
        set_think( self, 35 , 40, NULL, NULL, player_run );
		//player_pain1(  );
}


void DeathBubblesSpawn(  )
{
	if ( PROG_TO_EDICT( self->s.v.owner )->s.v.waterlevel != 3 )
		return;
	newmis = spawn(  );
	setmodel( newmis, "progs/s_bubble.spr" );
	setorigin( newmis, PROG_TO_EDICT( self->s.v.owner )->s.v.origin[0],
		   PROG_TO_EDICT( self->s.v.owner )->s.v.origin[1],
		   PROG_TO_EDICT( self->s.v.owner )->s.v.origin[2] + 24 );

	newmis->s.v.movetype = MOVETYPE_NOCLIP;
	newmis->s.v.solid = SOLID_NOT;
	SetVector( newmis->s.v.velocity, 0, 0, 15 );
	newmis->s.v.nextthink = g_globalvars.time + 0.5;
	newmis->s.v.think = ( func_t ) bubble_bob;
	newmis->s.v.classname = "bubble";
	newmis->s.v.frame = 0;
	newmis->cnt = 0;
	setsize( newmis, -8, -8, -8, 8, 8, 8 );
	self->s.v.nextthink = g_globalvars.time + 0.1;
	self->s.v.think = ( func_t ) DeathBubblesSpawn;
	self->air_finished = self->air_finished + 1;
	if ( self->air_finished >= self->bubble_count )
		dremove( self );
}

void DeathBubbles( int num_bubbles )
{
	gedict_t *bubble_spawner;

	bubble_spawner = spawn(  );
	setorigin( bubble_spawner, PASSVEC3( self->s.v.origin ) );
	bubble_spawner->s.v.movetype = MOVETYPE_NONE;
	bubble_spawner->s.v.solid = SOLID_NOT;
	bubble_spawner->s.v.nextthink = g_globalvars.time + 0.1;
	bubble_spawner->s.v.think = ( func_t ) DeathBubblesSpawn;
	bubble_spawner->air_finished = 0;
	bubble_spawner->s.v.owner = EDICT_TO_PROG( self );
	bubble_spawner->bubble_count = num_bubbles;
	return;
}

void DeathSound(  )
{
	float   rs;

	if ( self->s.v.waterlevel == 3 )
	{
		if ( self->is_feigning )
			DeathBubbles( 2 );
		else
			DeathBubbles( 10 );
		sound( self, 2, "player/h2odeath.wav", 1, 0 );
		return;
	}
	rs = ( int ) ( g_random(  ) * 5 + 1 );
	if ( rs == 1 )
		self->s.v.noise = "player/death1.wav";
	if ( rs == 2 )
		self->s.v.noise = "player/death2.wav";
	if ( rs == 3 )
		self->s.v.noise = "player/death3.wav";
	if ( rs == 4 )
		self->s.v.noise = "player/death4.wav";
	if ( rs == 5 )
		self->s.v.noise = "player/death5.wav";
	sound( self, 2, self->s.v.noise, 1, 0 );
	return;
}

void PlayerDead(  )
{
	self->s.v.nextthink = -1;
	self->s.v.deadflag = DEAD_DEAD;
}



void    DeathBubbles( int num_bubbles );

void VelocityForDamage( float dm, vec3_t v )
{

	v[0] = 100 * crandom(  );
	v[1] = 100 * crandom(  );
	v[2] = 200 + 100 * g_random(  );
	if ( dm > -50 )
		VectorScale( v, 0.7, v );
	else
	{
		if ( dm > -200 )
			VectorScale( v, 2, v );
		else
			VectorScale( v, 10, v );
	}
	return;
}

void ThrowGib( char *gibname, float dm )
{
	newmis = spawn(  );
	VectorCopy( self->s.v.origin, newmis->s.v.origin );
	if ( tfset(birthday) && g_random(  ) < 0.3 )
	{
		setmodel( newmis, "progs/grenade3.mdl" );
		newmis->s.v.skin = g_random(  ) * 4;
		newmis->s.v.skin = floor( newmis->s.v.skin );
	} else
		setmodel( newmis, gibname );
	setsize( newmis, 0, 0, 0, 0, 0, 0 );
	VelocityForDamage( dm, newmis->s.v.velocity );
	newmis->s.v.movetype = MOVETYPE_BOUNCE;
	newmis->s.v.solid = SOLID_NOT;
	newmis->s.v.avelocity[0] = g_random(  ) * 600;
	newmis->s.v.avelocity[1] = g_random(  ) * 600;
	newmis->s.v.avelocity[2] = g_random(  ) * 600;
	newmis->s.v.think = ( func_t ) SUB_Remove;
	newmis->s.v.ltime = g_globalvars.time;
	newmis->s.v.nextthink = g_globalvars.time + 10 + g_random(  ) * 10;
	newmis->s.v.frame = 0;
	newmis->s.v.flags = 0;
}

void ThrowHead( char *gibname, float dm )
{
	setmodel( self, gibname );
	self->s.v.skin = 0;
	self->s.v.frame = 0;
	self->s.v.nextthink = -1;
	self->s.v.movetype = MOVETYPE_BOUNCE;
	self->s.v.takedamage = 0;
	self->s.v.solid = SOLID_NOT;
	SetVector( self->s.v.view_ofs, 0, 0, 8 );
	setsize( self, -16, -16, 0, 16, 16, 56 );
	VelocityForDamage( dm, self->s.v.velocity );
	self->s.v.origin[2] = self->s.v.origin[2] - 24;
	self->s.v.flags = ( int ) self->s.v.flags - ( ( int ) self->s.v.flags & FL_ONGROUND );
	SetVector( self->s.v.avelocity, 0, crandom(  ) * 600, 0 );
}

void HeadShotThrowHead( char *gibname )
{
	setmodel( self, gibname );
	self->s.v.frame = 0;
	self->s.v.nextthink = -1;
	self->s.v.movetype = MOVETYPE_BOUNCE;
	self->s.v.takedamage = 0;
	self->s.v.solid = SOLID_NOT;
	SetVector( self->s.v.view_ofs, 0, 0, 8 );
	setsize( self, -16, -16, 0, 16, 16, 56 );
	normalize( self->head_shot_vector, self->s.v.velocity );
	VectorScale( self->s.v.velocity, 600, self->s.v.velocity );
	//self->s.v.velocity = normalize(self->head_shot_vector) * 600;
	self->s.v.origin[2] = self->s.v.origin[2] + 24;
	self->s.v.flags = ( int ) self->s.v.flags - ( ( int ) self->s.v.flags & FL_ONGROUND );
	SetVector( self->s.v.avelocity, 0, 0, 0 );
}

void KillPlayer(  )
{
	PROG_TO_EDICT( self->s.v.owner )->s.v.deadflag = DEAD_DEAD;
	dremove( self );
}

#define HEADLESS_GIB
void GibPlayer(  )
{
#ifdef HEADLESS_GIB
	if (tf_data.deathmsg == DMSG_SNIPERHEADSHOT || tf_data.deathmsg == DMSG_BACKSTAB)
	{
		if (tf_data.deathmsg == DMSG_SNIPERHEADSHOT)
			HeadShotThrowHead("progs/h_player.mdl");
		else
			ThrowHead("progs/h_player.mdl", -60);

		newmis = spawn();
		g_globalvars.newmis = EDICT_TO_PROG( newmis );
		newmis->s.v.solid = SOLID_NOT;
		setmodel(newmis, "progs/headless.mdl");
		setsize(newmis, 0,0,0, 0,0,0);
		setorigin(newmis, PASSVEC3(self->s.v.origin)-47);
		newmis->s.v.movetype = MOVETYPE_STEP;
		VectorCopy(self->s.v.angles,newmis->s.v.angles);
		//VectorCopy(self->s.v.velocity,newmis->s.v.velocity);
		newmis->s.v.colormap = self->s.v.colormap ;
		//newmis->s.v.colormap = TeamFortress_TeamGetColor(self->team_no);
		newmis->s.v.skin = self->s.v.skin;
		newmis->s.v.think = ( func_t )Headless_Think;
		newmis->s.v.nextthink = g_globalvars.time + 0.1;
		if (self->current_weapon <= WEAP_AXE)
			newmis->s.v.frame = 0;
		else
			newmis->s.v.frame = 8; 

	}
	else
	{
#endif
        	ThrowHead( "progs/h_player.mdl", self->s.v.health );
        	ThrowGib( "progs/gib1.mdl", self->s.v.health );
        	ThrowGib( "progs/gib2.mdl", self->s.v.health );
        	ThrowGib( "progs/gib3.mdl", self->s.v.health );
#ifdef HEADLESS_GIB
	}
#endif
	if ( tf_data.deathmsg == DMSG_TRIGGER )
	{
		newmis = spawn(  );
		newmis->s.v.owner = EDICT_TO_PROG( self );
		newmis->s.v.think = ( func_t ) KillPlayer;
		newmis->s.v.nextthink = g_globalvars.time + 1;
	} else
		self->s.v.deadflag = DEAD_DEAD;

	TeamFortress_SetupRespawn( 0 );
	if ( streq( damage_attacker->s.v.classname, "teledeath" ) )
	{
		sound( self, 2, "player/teledth1.wav", 1, 0 );
		self->respawn_time = self->respawn_time + 2 + g_random(  ) * 2;
		return;
	}
	if ( streq( damage_attacker->s.v.classname, "teledeath2" ) )
	{
		sound( self, 2, "player/teledth1.wav", 1, 0 );
		self->respawn_time = self->respawn_time + 2 + g_random(  ) * 2;
		return;
	}
	if ( g_random(  ) < 0.5 )
		sound( self, 2, "player/gib.wav", 1, 0 );
	else
		sound( self, 2, "player/udeath.wav", 1, 0 );
}
#define player_die(start,end, end_func) set_think( self, start, end, NULL, end_func, NULL )
void PlayerSetDieFrames(int isdead)
{
	if ( self->current_weapon <= 16 )
	{
		//player_die_ax1(  );
        player_die(41, 49, isdead ? PlayerDead: NULL);
		return;
	}
	switch((int)( g_random(  ) * 5 ))
	{
		case 0:
			//player_diea1(  );
            player_die( 50, 60, isdead? PlayerDead: NULL);
			break;
		case 1:
			//player_dieb1(  );
            player_die(61, 69, isdead? PlayerDead: NULL);
			break;
		case 2:
            player_die(70, 84, isdead? PlayerDead: NULL);
			//player_diec1(  );
			break;
		case 3:
            player_die(85, 93, isdead? PlayerDead: NULL);
			//player_died1(  );
			break;
		default:
            player_die(94, 102, isdead? PlayerDead: NULL);
			//player_diee1(  );
			break;
	
	}
}

void PlayerDie(  )
{
//	float   i;
	gedict_t *te;
	gedict_t *saveself;

	if ( self->hook_out )
	{
		Reset_Grapple( self->hook );
		Attack_Finished( 0.75 );
		self->hook_out = 1;
	}
	self->tf_impulse = 0;
	self->imp1 = 0;
	self->imp2 = 0;
	self->imp3 = 0;
	self->imp4 = 0;

       	self->s.v.button0 = 0;
       	self->s.v.button1 = 0;
       	self->s.v.button2 = 0;

	for(te = world; (te = trap_find( te, FOFS( s.v.classname ), "primer" ));)
	{
		if(te->s.v.owner == EDICT_TO_PROG( self ))
			break;
	
	}
	if ( te )
//  te->s.v.nextthink = g_globalvars.time;
	{
		self->s.v.deadflag = DEAD_DYING;
		saveself = self;
		self = te;
		( ( void ( * )(  ) ) ( self->s.v.think ) ) (  );
		self = saveself;
		self->s.v.deadflag = DEAD_NO;
	}
	self->s.v.items = ( int ) self->s.v.items - ( ( int ) self->s.v.items & IT_INVISIBILITY );
	self->invisible_finished = 0;
	self->invincible_finished = 0;
	self->super_damage_finished = 0;
	self->radsuit_finished = 0;
	self->s.v.modelindex = modelindex_player;
	if ( ( self->tfstate & TFSTATE_INFECTED ) && self == PROG_TO_EDICT( self->s.v.enemy ) )
	{
		te = trap_find( world, FOFS( s.v.classname ), "timer" );
		while ( te )
		{
			if ( PROG_TO_EDICT( te->s.v.owner ) == self && te->s.v.think == ( func_t ) BioInfection_Decay )
			{
				logfrag( PROG_TO_EDICT( te->s.v.enemy ), self );
				TF_AddFrags( PROG_TO_EDICT( te->s.v.enemy ), 1 );
			}
			te = trap_find( te, FOFS( s.v.classname ), "timer" );
		}
	}
	TeamFortress_RemoveTimers(  );
	if ( deathmatch || coop )
		DropBackpack(  );
	self->s.v.weaponmodel = "";
	SetVector( self->s.v.view_ofs, 0, 0, -8 );
	self->s.v.deadflag = DEAD_DYING;
	self->s.v.solid = SOLID_NOT;
	self->s.v.flags = ( int ) self->s.v.flags - ( ( int ) self->s.v.flags & FL_ONGROUND );
	self->s.v.movetype = MOVETYPE_TOSS;
	if ( self->s.v.velocity[2] < 10 )
		self->s.v.velocity[2] = self->s.v.velocity[2] + g_random(  ) * 300;
	if ( self->s.v.health < -40 )
	{
		GibPlayer(  );
		return;
	}
	DeathSound(  );
	self->s.v.angles[0] = 0;
	self->s.v.angles[2] = 0;
    PlayerSetDieFrames(1);

	TeamFortress_SetupRespawn( 0 );
}

void set_suicide_frame(  )
{
	if ( strneq( self->s.v.model, "progs/player.mdl" ) )
		return;
	setmodel( self, "" );
	setsize( self, -16, -16, -24, 16, 16, 32 );
}

void Headless_Think(  )
{
	self->s.v.frame = self->s.v.frame + 1;
	if ( self->s.v.frame == 7 || self->s.v.frame == 18 )
	{
		self->s.v.nextthink = g_globalvars.time + 10 + g_random(  ) * 10;
		self->s.v.think = ( func_t ) SUB_Remove;
		return;
	}
	self->s.v.nextthink = g_globalvars.time + 0.1;
}
