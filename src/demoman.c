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
 *  $Id: demoman.c,v 1.16 2005-05-28 18:33:52 AngelD Exp $
 */
#include "g_local.h"

void    NormalGrenadeTouch(  );
void    NormalGrenadeExplode(  );
void    MirvGrenadeTouch(  );
void    MirvGrenadeExplode(  );
void    MirvGrenadeLaunch( vec3_t org, gedict_t * shooter );
void    TeamFortress_DetpackSet(  );
void    TeamFortress_DetpackExplode(  );
void    TeamFortress_DetpackTouch(  );
void    TeamFortress_DetpackDisarm(  );
void    TeamFortress_DetpackCountDown(  );

//=========================================================================
// Detonate all thrown pipebombs
void TeamFortress_DetonatePipebombs(  )
{
	gedict_t *e;
	// Find all this players pipebombs
	for ( e = world; (e = trap_find( e, FOFS( s.v.classname ), "pipebomb" )); )
	{
		if ( e->s.v.owner == EDICT_TO_PROG( self ) )
			e->s.v.nextthink = g_globalvars.time;
	}
}

//=========================================================================
// Pipebomb touch function
void PipebombTouch(  )
{
	sound( self, CHAN_WEAPON, "weapons/bounce.wav", 1, ATTN_NORM );  // bounce sound
	if ( VectorCompareF( self->s.v.velocity, 0, 0, 0 ) )
		SetVector( self->s.v.avelocity, 0, 0, 0 );
}

//=========================================================================
// Touch Function for Mirv Grenade
// Mirv Grenade heavily influenced by the Firewall Grenade by Steve Bond (wedge@nuc.net)
void MirvGrenadeTouch(  )
{
        // If the Mirv Grenade hits a player, it just bounces off
	sound( self, CHAN_WEAPON, "weapons/bounce.wav", 1, ATTN_NORM );
	if ( VectorCompareF( self->s.v.velocity, 0, 0, 0 ) )
		SetVector( self->s.v.avelocity, 0, 0, 0 );
}

//=========================================================================
// Mirv Grenade explode function, for when the PRIMETIME runs out
void MirvGrenadeExplode(  )
{
	float   i;

	tf_data.deathmsg = DMSG_GREN_MIRV;
	T_RadiusDamage( self, PROG_TO_EDICT( self->s.v.owner ), 100, world );
	trap_WriteByte( MSG_BROADCAST, SVC_TEMPENTITY );
	trap_WriteByte( MSG_BROADCAST, TE_EXPLOSION );
	trap_WriteCoord( MSG_BROADCAST, self->s.v.origin[0] );
	trap_WriteCoord( MSG_BROADCAST, self->s.v.origin[1] );
	trap_WriteCoord( MSG_BROADCAST, self->s.v.origin[2] );
	trap_multicast( PASSVEC3( self->s.v.origin ), 1 );
	self->s.v.solid = SOLID_NOT;
	// Launch mirvs
	for ( i = 0; i < GR_TYPE_MIRV_NO; i++)
	{
		MirvGrenadeLaunch( self->s.v.origin, PROG_TO_EDICT( self->s.v.owner ) );
	}
	BecomeExplosion(  );
}

//=========================================================================
// Launch a Mirv
void GrenadeTouch(  );
void GrenadeExplode(  );

void MirvGrenadeLaunch( vec3_t org, gedict_t * shooter )
{
	float   xdir;
	float   ydir;
	float   zdir;

	xdir = 150 * g_random(  ) - 75;
	ydir = 150 * g_random(  ) - 75;
	zdir = 40 * g_random(  );
	newmis = spawn(  );
	g_globalvars.newmis = EDICT_TO_PROG( newmis );
	newmis->s.v.owner = EDICT_TO_PROG( shooter );
	newmis->s.v.movetype = MOVETYPE_BOUNCE;
	newmis->s.v.solid = SOLID_BBOX;
	newmis->s.v.classname = "grenade";
	newmis->s.v.weapon = 10;
	if( tf_data.lan_mode )
	{
		newmis->s.v.touch = ( func_t ) GrenadeTouch;
		newmis->s.v.think = ( func_t ) GrenadeExplode;
	}else
	{
		newmis->s.v.touch = ( func_t ) NormalGrenadeTouch;
		newmis->s.v.think = ( func_t ) NormalGrenadeExplode;
	}

	newmis->s.v.nextthink = g_globalvars.time + 2 + g_random(  );
	newmis->s.v.velocity[0] = xdir * 2;
	newmis->s.v.velocity[1] = ydir * 2;
	newmis->s.v.velocity[2] = zdir * 15 + 1;
	SetVector( newmis->s.v.avelocity, 250, 300, 400 );
	if ( tf_data.birthday == 1 && g_random(  ) < 0.3 )
		setmodel( newmis, "progs/grenade3.mdl" );
	else
		setmodel( newmis, "progs/grenade2.mdl" );

	setsize( newmis, 0, 0, 0, 0, 0, 0 );
	setorigin( newmis, PASSVEC3( org ) );
}

//=========================================================================
// Handles the Setting of Detpacks

// Training Ground Stuff
void Detpack_SetClip(  )
{
	gedict_t *te;

	for ( te = world; (te = trap_find( te, FOFS( s.v.classname ), "detpack" )); )
	{
	        switch(tg_data.detpack_clip)
	        {
	        	case TG_DETPACK_CLIP_ALL:
	        		te->s.v.solid = SOLID_NOT;
	        		break;
	        	case TG_DETPACK_SOLID_ALL:
				te->s.v.solid = SOLID_BBOX;
				te->s.v.owner = EDICT_TO_PROG( world );
	        		break;

	        	case TG_DETPACK_CLIP_OWNER:
	        	default:	
				te->s.v.solid = SOLID_BBOX;
				te->s.v.owner = EDICT_TO_PROG( te->real_owner );
				break;
	        }
	}
}

void TeamFortress_Cmd_Detpack()
{
        int time;
        char    value[1024];

        if( trap_CmdArgc() != 2)
        	return;

        trap_CmdArgv( 1, value, sizeof( value ) );
        if( !strcmp(value,"stop"))
        {
        	TeamFortress_DetpackStop();
        	return;
        }

        time = atoi(value);
        if (  time > 255 )
        	time = 255;
        TeamFortress_SetDetpack( time );
}

void TeamFortress_SetDetpack( float timer )
{
	gedict_t *te;
	gedict_t *at_spot;

	// prevent detpack impulse from triggering anything else
	self->s.v.impulse = 0;
	self->last_impulse = 0;

	if ( !( self->weapons_carried & WEAP_DETPACK ) && !tg_data.tg_enabled )
		return;
	if ( (tf_data.disable_grens & DG_TYPE_DETPACK) && !tg_data.tg_enabled )
		return;
	if ( self->ammo_detpack <= 0 && !tg_data.tg_enabled )
		return;

	for ( at_spot = world; (at_spot = findradius( at_spot, self->s.v.origin, 65 )); )
	{
		if ( streq( at_spot->s.v.classname, "player" ) && self != at_spot )
		{
			G_sprint( self, 2, "You can't set a detpack on someone!\n" );
			return;
		}
		if ( streq( at_spot->mdl, "progs/turrbase.mdl" ) || streq( at_spot->mdl, "progs/turrgun.mdl" ) )
		{
			G_sprint( self, 2, "You can't set a detpack on a sentry gun!\n" );
			return;
		}
		if ( streq( at_spot->mdl, "progs/disp.mdl" ) )
		{
			G_sprint( self, 2, "You can't set a detpack on a dispenser!\n" );
			return;
		}
		if ( tf_data.birthday == 1 && streq( at_spot->mdl, "progs/detpack2.mdl" ) )
		{
			G_sprint( self, 2, "Detpacks can't be stacked!\n" );
			return;
		}
		if ( streq( at_spot->s.v.classname, "detpack" ) )
		{
			G_sprint( self, 2, "Detpacks can't be stacked!\n" );
			return;
		}
	}
	if ( tf_data.detpack_block )
	{
		for ( at_spot = world; (at_spot = findradius( at_spot, self->s.v.origin, 100 )); )
		{
			if ( tf_data.birthday == 1 && streq( at_spot->mdl, "progs/detpack2.mdl" ) )
			{
				G_sprint( self, 2, "Detpacks can't be stacked!\n" );
				return;
			}
			if ( streq( at_spot->s.v.classname, "detpack" ) )
			{
				G_sprint( self, 2, "Detpacks can't be stacked!\n" );
				return;
			}
		}
	}
	// Cant set detpack if you're in the air
	if ( !( ( int ) self->s.v.flags & FL_ONGROUND ) )
	{
		G_sprint( self, PRINT_HIGH, "You can't set detpacks in the air!\n" );
		return;
	}
        if( !tg_data.tg_enabled )
        {
        	for ( te = world; (te = trap_find( te, FOFS( s.v.classname ), "detpack" )); )
        	{
        		if ( te->s.v.owner == EDICT_TO_PROG( self ) )
        		{
        			G_sprint( self, PRINT_HIGH, "You can only have 1 detpack active at a time.\n" );
        			return;
        		}
        	}
        }
	if ( timer < 5 )
	{
		G_sprint( self, 2, "You can't set detpacks for less that 5 seconds.\n" );
		return;
	}
	self->is_detpacking = 1;
	self->ammo_detpack = self->ammo_detpack - 1;
	self->tfstate = self->tfstate | TFSTATE_CANT_MOVE;
	// Save the current weapon and remove it
	self->s.v.weapon = self->current_weapon;
	self->current_weapon = 0;
	self->s.v.weaponmodel = "";
	self->s.v.weaponframe = 0;

	TeamFortress_SetSpeed( self );
	self->pausetime = g_globalvars.time + WEAP_DETPACK_SETTIME;
	G_sprint( self, 2, "Setting detpack for %.0f seconds...\n", timer );
	newmis = spawn(  );
	g_globalvars.newmis = EDICT_TO_PROG( newmis );

	newmis->s.v.owner = EDICT_TO_PROG( self );
	newmis->real_owner = self;

	newmis->s.v.classname = "timer";
	newmis->s.v.netname = "detpack_timer";
	newmis->s.v.nextthink = g_globalvars.time + WEAP_DETPACK_SETTIME;
	newmis->s.v.think = ( func_t ) TeamFortress_DetpackSet;
	newmis->s.v.health = timer;
}

//=========================================================================
// Stops the setting of the detpack
void TeamFortress_DetpackStop(  )
{
	gedict_t *detpack_timer;

	for ( detpack_timer = world; (detpack_timer = trap_find( detpack_timer, FOFS( s.v.netname ), "detpack_timer" )); )
	{
		if ( detpack_timer->s.v.owner == EDICT_TO_PROG( self ) )
			break;
	}
	if ( !detpack_timer )
		return;
	G_sprint( self, 2, "Detpack retrieved.\n" );
	self->ammo_detpack = self->ammo_detpack + 1;
	dremove( detpack_timer );
	self->tfstate = self->tfstate - ( self->tfstate & TFSTATE_CANT_MOVE );
	self->is_detpacking = 0;
	self->current_weapon = self->s.v.weapon;
	W_SetCurrentAmmo(  );
	TeamFortress_SetSpeed( self );
	self->pausetime = g_globalvars.time;
}

//=========================================================================
// The detpack is set, let the player go and start timer
void TeamFortress_DetpackSet(  )
{
	gedict_t *countd;
	gedict_t *oldself, *owner;

	owner = PROG_TO_EDICT( self->s.v.owner );
	self->is_detpacking = 0;
	owner->tfstate = owner->tfstate - ( owner->tfstate & TFSTATE_CANT_MOVE );
	TeamFortress_SetSpeed( owner );
	sound( owner, 2, "doors/medtry.wav", 1, 1 );
	oldself = self;
	self = owner;
	self->is_detpacking = 0;
	self->current_weapon = self->s.v.weapon;
	W_SetCurrentAmmo(  );
	self = oldself;
	newmis = spawn(  );
	g_globalvars.newmis = EDICT_TO_PROG( newmis );
	newmis->s.v.owner = self->s.v.owner;
	VectorCopy( owner->s.v.origin, newmis->s.v.origin );
	newmis->s.v.origin[2] -= 23;

	newmis->real_owner = self->real_owner;

	newmis->s.v.movetype = MOVETYPE_BOUNCE;
	newmis->s.v.solid = SOLID_BBOX;
	newmis->s.v.classname = "detpack";
	newmis->s.v.flags = 256;
	SetVector( newmis->s.v.angles, 90, 0, 0 );
	newmis->s.v.angles[1] = owner->s.v.angles[1];
	SetVector( newmis->s.v.velocity, 0, 0, 0 );
	SetVector( newmis->s.v.avelocity, 0, 0, 0 );

	newmis->weaponmode = 0;        // Detpack weaponmode = 1 when disarming
	newmis->s.v.touch = ( func_t ) TeamFortress_DetpackTouch;

	if ( tf_data.birthday == 1 )
		setmodel( newmis, "progs/detpack2.mdl" );
	else
		setmodel( newmis, "progs/detpack.mdl" );
	setsize( newmis, -16, -16, 0, 16, 16, 8 );
	setorigin( newmis, PASSVEC3( owner->s.v.origin ) );
	
	// Create the CountDown entity
	countd = spawn(  );
	newmis->linked_list = countd;
	countd->s.v.think = ( func_t ) TeamFortress_DetpackCountDown;
	countd->s.v.health = self->s.v.health - 1;
	countd->s.v.owner = self->s.v.owner;
	countd->s.v.classname = "countdown_timer";// Don't call it timer, because we
                                                  // don't want it removed if player dies
	countd->s.v.enemy = EDICT_TO_PROG( newmis );
	newmis->oldenemy = countd;
	if ( self->s.v.health <= 255 )
		countd->s.v.nextthink = g_globalvars.time + 1;
	else
	{
		countd->s.v.nextthink = g_globalvars.time + self->s.v.health - 10;
		countd->s.v.health = 9;
	}
	newmis->s.v.nextthink = g_globalvars.time + self->s.v.health;
	newmis->s.v.think = ( func_t ) TeamFortress_DetpackExplode;
	G_sprint( owner, 2, "Detpack set!\n" );
	dremove( self );
}

//=========================================================================
// The detpack goes BOOM!
void TeamFortress_DetpackExplode(  )
{
	float   pos;
	float   points;
	gedict_t *head;
	gedict_t *te, *owner;
	vec3_t  org, vtemp;

	owner = PROG_TO_EDICT( self->s.v.owner );
	G_bprint( 1, "FIRE IN THE HOLE!\n" );

	sound( self, 1, "weapons/detpack.wav", 1, 0 );
	if ( tf_data.birthday == 1 )
		G_bprint( 1, "%s spreads good cheer!\n", self->real_owner->s.v.netname );

	// Check the pointcontents to prevent detpack outside the world
	pos = trap_pointcontents( PASSVEC3( self->s.v.origin ) );

	if ( pos != CONTENT_SOLID && pos != CONTENT_SKY && self->real_owner->has_disconnected != 1 )
	{
		tf_data.deathmsg = DMSG_DETPACK;
		for ( head = world; (head = findradius( head, self->s.v.origin, WEAP_DETPACK_SIZE )); )
		{
			if ( streq( head->s.v.classname, "info_tfgoal" ) )
			{
				if ( ( head->goal_activation & TFGA_TOUCH_DETPACK ) && !head->search_time )
				{
					traceline( PASSVEC3( self->s.v.origin ), PASSVEC3( head->s.v.origin ), 1,
						   self );
					if ( g_globalvars.trace_fraction == 1 )
					{
						if ( Activated( head, self->real_owner ) )
							DoResults( head, self->real_owner, 1 );
						else
						{
							if ( head->else_goal )
							{
								te = Findgoal( head->else_goal );
								if ( te )
									AttemptToActivate( te, self->real_owner, head );
							}
						}
					}
				}
			} else
			{
				VectorSubtract( head->s.v.origin, self->s.v.origin, vtemp );
				if ( head->s.v.takedamage && vlen( vtemp ) <= 700 )
				{
					VectorAdd( head->s.v.mins, head->s.v.maxs, org );
					VectorScale( org, 0.5, org );
					VectorAdd( head->s.v.origin, org, org );
					VectorSubtract( head->s.v.origin, org, vtemp );
					points = 0.5 * vlen( vtemp );
					if ( points < 0 )
						points = 0;
					points = 700 - points;
					if ( points )
					{
						if ( CanDamage( head, self ) )
							TF_T_Damage( head, self, self->real_owner, points * 2, 2, 4 );
					}
				}
			}
		}
		trap_WriteByte( 4, SVC_TEMPENTITY );
		trap_WriteByte( 4, TE_EXPLOSION );
		trap_WriteCoord( 4, self->s.v.origin[0] );
		trap_WriteCoord( 4, self->s.v.origin[1] );
		trap_WriteCoord( 4, self->s.v.origin[2] );
		trap_multicast( PASSVEC3( self->s.v.origin ), 1 );
	} else
		G_sprint( self->real_owner, 2, "Your detpack fizzled out.\n" );
	// This code handles a disarming scout with protection
	if ( self->weaponmode == 1 )// Detpack was being disarmed
	{
		TeamFortress_SetSpeed( PROG_TO_EDICT( self->s.v.enemy ) );
		dremove( self->oldenemy );            // CountDown
		dremove( self->observer_list );       // Disarm timer
	}
	BecomeExplosion(  );
}

//=========================================================================
// The detpack touch function. Scouts can disarm it.
void TeamFortress_DetpackTouch(  )
{
	gedict_t *disarm, *owner;
	vec3_t  source;
	vec3_t  dest;

	if ( tg_data.detpack_drop )
		CheckBelowBuilding( self );

	if ( tg_data.disable_disarm )
		return;

	if ( strneq( other->s.v.classname, "player" ) )
		return;
	if ( other->playerclass != PC_SCOUT )
		return;
	if ( other->s.v.deadflag )
		return;
	if ( self->weaponmode == 1 )
		return;
	owner = PROG_TO_EDICT( self->s.v.owner );

	if ( (owner->team_no && TeamFortress_isTeamsAllied (other->team_no , owner->team_no)) && !tg_data.tg_enabled )
		return;

	makevectors( other->s.v.v_angle );
	VectorCopy( other->s.v.origin, source );
	source[2] += 16;

	VectorScale( g_globalvars.v_forward, 64, dest );
	VectorAdd( source, dest, dest );
	traceline( PASSVEC3( source ), PASSVEC3( dest ), 0, other );
	if ( g_globalvars.trace_fraction == 1 || g_globalvars.trace_ent != EDICT_TO_PROG( self ) )
		return;
	other->tfstate = other->tfstate | TFSTATE_CANT_MOVE;
	G_sprint( other, 2, "Disarming detpack...\n" );
	TeamFortress_SetSpeed( other );
	
	// Spawn disarming entity
	disarm = spawn(  );
	disarm->s.v.owner = EDICT_TO_PROG( other );// the scout
	disarm->s.v.enemy = EDICT_TO_PROG( self ); // the detpack
	disarm->s.v.classname = "timer";
	disarm->s.v.nextthink = g_globalvars.time + WEAP_DETPACK_DISARMTIME;
	disarm->s.v.think = ( func_t ) TeamFortress_DetpackDisarm;
	self->weaponmode = 1;                          // indicates disarming
	self->s.v.enemy = EDICT_TO_PROG( other );      // points to scout
	self->observer_list = disarm;
}

//=========================================================================
// The detpack disarm function. Scout has finished disarming it
// .enemy is the detpack
// .owner is the scout
void TeamFortress_DetpackDisarm(  )
{
	gedict_t *owner = PROG_TO_EDICT( self->s.v.owner );
	gedict_t *enemy = PROG_TO_EDICT( self->s.v.enemy );

	if ( owner->s.v.deadflag )
	{
		dremove( self );
		return;
	}
	if ( tf_data.birthday == 1 )
	{
		G_bprint( 1, "%s stole %s's birthday present!\n", owner->s.v.netname, enemy->real_owner->s.v.netname );
	} else
	{
		G_bprint( 1, "%s's detpack was defused by %s\n", enemy->real_owner->s.v.netname, owner->s.v.netname );
	}
	owner->tfstate = owner->tfstate - ( owner->tfstate & TFSTATE_CANT_MOVE );
	TF_AddFrags( owner, 1 );
	// Reset speeds of scout
	TeamFortress_SetSpeed( owner );

	dremove( enemy->oldenemy ); // remove count down                
	dremove( enemy );           // remove detpack                   
	dremove( self );            // remove this timer
}

//=========================================================================
// The Detpack CountDown function. Displays the seconds left before the
// detpack detonates to the owner of the detpack, if <10
void TeamFortress_DetpackCountDown(  )
{
	gedict_t *owner = PROG_TO_EDICT( self->s.v.owner );
	gedict_t *enemy = PROG_TO_EDICT( self->s.v.enemy );

	if ( tg_data.detpack_drop )
		CheckBelowBuilding( self );

	self->s.v.nextthink = g_globalvars.time + 1;
	self->s.v.health = self->s.v.health - 1;
	if ( self->s.v.health < 10 )
	{
		G_sprint( owner, 2, "%.0f...\n", self->s.v.health+1 );
		if ( self->s.v.health < 5 && !self->has_disconnected )
		{
			sound( enemy, 2, "doors/baseuse.wav", 1, 1 );
			self->has_disconnected = 1;
		}
		// Flash the red light
		if ( self->s.v.health < 5 && !enemy->s.v.skin )
			enemy->s.v.skin = 1;
		else
			enemy->s.v.skin = 0;
		if ( !self->s.v.health )
			dremove( self );
	}
}
