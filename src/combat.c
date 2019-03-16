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
 *  $Id: combat.c,v 1.10 2006-11-29 23:19:23 AngelD Exp $
 */
#include "g_local.h"
void    ClientObituary( gedict_t * e1, gedict_t * e2 );

//============================================================================

/*================
monster_death_use (from monsters.qc)

When a monster dies, it fires all of its targets with the current
enemy as activator.
================
*/
void monster_death_use(  )
{

	if ( ( int ) self->s.v.flags & FL_FLY )
		self->s.v.flags -= FL_FLY;
	if ( ( int ) self->s.v.flags & FL_SWIM )
		self->s.v.flags -= FL_SWIM;
	if ( !self->s.v.target )
		return;
	activator = PROG_TO_EDICT( self->s.v.enemy );
	SUB_UseTargets(  );
}

/*
============
CanDamage

Returns true if the inflictor can directly damage the target.  Used for
explosions and melee attacks.
============
*/

qboolean CanDamage( gedict_t * targ, gedict_t * inflictor )
{
// bmodels need special checking because their origin is 0,0,0
	if ( targ->s.v.movetype == MOVETYPE_PUSH )
	{

		traceline( PASSVEC3( inflictor->s.v.origin ),
			   0.5 * ( targ->s.v.absmin[0] + targ->s.v.absmax[0] ),
			   0.5 * ( targ->s.v.absmin[1] + targ->s.v.absmax[1] ),
			   0.5 * ( targ->s.v.absmin[2] + targ->s.v.absmax[2] ), true, self );

		if ( g_globalvars.trace_fraction == 1 )
			return true;

		if ( PROG_TO_EDICT( g_globalvars.trace_ent ) == targ )
			return true;

		return false;
	}
	traceline( PASSVEC3( inflictor->s.v.origin ), PASSVEC3( targ->s.v.origin ), true, self );
	if ( g_globalvars.trace_fraction == 1 )
		return true;

	traceline( PASSVEC3( inflictor->s.v.origin ),
		   targ->s.v.origin[0] + 15, targ->s.v.origin[1] + 15, targ->s.v.origin[2] + 0, true, self );
	if ( g_globalvars.trace_fraction == 1 )
		return true;

	traceline( PASSVEC3( inflictor->s.v.origin ),
		   targ->s.v.origin[0] - 15, targ->s.v.origin[1] - 15, targ->s.v.origin[2] + 0, true, self );
	if ( g_globalvars.trace_fraction == 1 )
		return true;

	traceline( PASSVEC3( inflictor->s.v.origin ),
		   targ->s.v.origin[0] - 15, targ->s.v.origin[1] + 15, targ->s.v.origin[2] + 0, true, self );
	if ( g_globalvars.trace_fraction == 1 )
		return true;

	traceline( PASSVEC3( inflictor->s.v.origin ),
		   targ->s.v.origin[0] + 15, targ->s.v.origin[1] - 15, targ->s.v.origin[2] + 0, true, self );
	if ( g_globalvars.trace_fraction == 1 )
		return true;


	return false;
}

/*
============
Killed
============
*/
void Killed( gedict_t * targ, gedict_t * attacker )
{
	gedict_t *oself;

	oself = self;
	self = targ;

	// don't let sbar look bad if a player
	if ( self->s.v.health < -99 )
		self->s.v.health = -99;	

	// doors, triggers, etc
	if ( self->s.v.movetype == MOVETYPE_PUSH || self->s.v.movetype == MOVETYPE_NONE )
	{			
		self->th_die(  );
		self = oself;
		return;
	}
	self->s.v.enemy = EDICT_TO_PROG( attacker );

        // bump the monster counter
	if ( ( ( int ) ( self->s.v.flags ) ) & FL_MONSTER )
	{
		g_globalvars.killed_monsters++;
		trap_WriteByte( MSG_ALL, SVC_KILLEDMONSTER );
	}

	ClientObituary( self, attacker );

	self->s.v.takedamage = DAMAGE_NO;
	self->s.v.touch = ( func_t ) SUB_Null;
	self->s.v.effects = 0;

	monster_death_use(  );
	self->th_die(  );

	self = oself;
}

/*
============
T_Damage

The damage is coming from inflictor, but get mad at attacker
This should be the only function that ever reduces health.
============
*/
#define allowedClasname( attacker, targ ) \
( streq( attacker->s.v.classname, "player" ) \
	     && ( streq( targ->s.v.classname, "player" ) \
		  || streq( targ->s.v.classname, "building_sentrygun" ) \
		  || streq( targ->s.v.classname, "building_dispenser" ) \
		  || streq( targ->s.v.classname, "building_teleporter_entrance" ) \
		  || streq( targ->s.v.classname, "building_teleporter_exit" ) ) )

#define isTeamDamage( attacker, targ ) \
    ( targ->team_no > 0 && targ->team_no == attacker->team_no \
                 && targ != attacker && ( T_flags & TF_TD_NOTTEAM ) )

gedict_t *damage_attacker, *damage_inflictor;
void T_Damage( gedict_t * targ, gedict_t * inflictor, gedict_t * attacker, float damage )
{
	vec3_t  dir;
	gedict_t *oldself, *te;
	float   save;
	float   take;

//      char    attackerteam[10], targteam[10];

	if ( !targ->s.v.takedamage )
		return;

	if ( streq( attacker->s.v.classname, "player" ) )
		damage = damage * 0.9;
	if ( streq( attacker->s.v.classname, "player" ) )
	{
		if ( attacker->super_damage_finished > g_globalvars.time )
			damage = damage * 4;
		if ( strneq( targ->s.v.classname, "player" ) && strneq( targ->s.v.classname, "bot" ) )
		{
			if ( !Activated( targ, attacker ) )
			{
				if ( targ->else_goal )
				{
					te = Findgoal( targ->else_goal );
					if ( te )
						AttemptToActivate( te, attacker, targ );
				}
				return;
			}
		}
	}
// used by buttons and triggers to set activator for target firing
	damage_attacker = attacker;

	if ( teamplay & ( TEAMPLAY_LESSSCOREHELP | TEAMPLAY_LESSPLAYERSHELP ) )
		damage = TeamEqualiseDamage( targ, attacker, damage );
// save damage based on the target's armor level

	save = ceil( targ->s.v.armortype * damage );
	if ( save >= targ->s.v.armorvalue )
	{
		save = targ->s.v.armorvalue;
		targ->s.v.armortype = 0;	// lost all armor
		targ->s.v.items -= ( ( int ) targ->s.v.items & ( IT_ARMOR1 | IT_ARMOR2 | IT_ARMOR3 ) );
	}

	targ->s.v.armorvalue = targ->s.v.armorvalue - save;
	take = ceil( damage - save );

// add to the damage total for clients, which will be sent as a single
// message at the end of the frame
// FIXME: remove after combining shotgun blasts?
	if ( ( int ) targ->s.v.flags & FL_CLIENT )
	{
		targ->s.v.dmg_take += take;
		targ->s.v.dmg_save += save;
		targ->s.v.dmg_inflictor = EDICT_TO_PROG( inflictor );
		targ->eff_info.dmg_curr += 0.5 * take + 0.5 * save;
		targ->eff_info.dmg = targ->eff_info.dmg_curr;
	}
	damage_inflictor = inflictor;
// figure momentum add
	if ( ( inflictor != world ) && ( targ->s.v.movetype == MOVETYPE_WALK ) )
	{

		dir[0] = targ->s.v.origin[0] - ( inflictor->s.v.absmin[0] + inflictor->s.v.absmax[0] ) * 0.5;
		dir[1] = targ->s.v.origin[1] - ( inflictor->s.v.absmin[1] + inflictor->s.v.absmax[1] ) * 0.5;
		dir[2] = targ->s.v.origin[2] - ( inflictor->s.v.absmin[2] + inflictor->s.v.absmax[2] ) * 0.5;
		VectorNormalize( dir );
		if ( ( damage < 60 ) && ( streq( attacker->s.v.classname, "player" ) ) 
                && ( streq( targ->s.v.classname, "player" ) ) 
                && strneq( attacker->s.v.netname, targ->s.v.netname ) )
		{
			targ->s.v.velocity[0] += dir[0] * damage * 11;
			targ->s.v.velocity[1] += dir[1] * damage * 11;
			targ->s.v.velocity[2] += dir[2] * damage * 11;

		} else
		{
			targ->s.v.velocity[0] += dir[0] * damage * 8;
			targ->s.v.velocity[1] += dir[1] * damage * 8;
			targ->s.v.velocity[2] += dir[2] * damage * 8;
		}
		if ( ( rj > 1 )
		     && ( ( streq( attacker->s.v.classname, "player" ) )
			  && streq( targ->s.v.classname, "player" ) )
		     && streq( attacker->s.v.netname, targ->s.v.netname ) )
		{
			targ->s.v.velocity[0] += dir[0] * damage * rj;
			targ->s.v.velocity[1] += dir[1] * damage * rj;
			targ->s.v.velocity[2] += dir[2] * damage * rj;

		}
	}
	if ( streq( targ->s.v.classname, "player" ) && tg_data.godmode )
		return;

// check for godmode or invincibility
	if ( ( int ) targ->s.v.flags & FL_GODMODE )
		return;

	if ( targ->invincible_finished >= g_globalvars.time )
	{
		if ( self->invincible_sound < g_globalvars.time )
		{
			sound( targ, CHAN_ITEM, "items/protect3.wav", 1, ATTN_NORM );
			self->invincible_sound = g_globalvars.time + 2;
		}
		return;
	}

    if ( allowedClasname( attacker, targ ) )
	{
		if ( targ->team_no > 0 && targ->team_no == attacker->team_no && targ != attacker )
		{
			if ( teamplay & TEAMPLAY_NOEXPLOSIVE )
				return;
			else
			{
				if ( teamplay & TEAMPLAY_HALFEXPLOSIVE )
					take = take / 2;
			}
		}
	}
	if ( take < 1 && take )
		take = 1;
	targ->s.v.health = targ->s.v.health - take;
	if ( targ->s.v.armorvalue < 1 )
	{
		targ->armorclass = 0;
		targ->s.v.armorvalue = 0;
	}
	if ( targ->s.v.health <= 0 )
	{
		Killed( targ, attacker );
		return;
	}
	oldself = self;
	self = targ;
	if ( self->th_pain )
	{
		self->th_pain( attacker, take );
		if ( skill >= 3 )
			self->pain_finished = g_globalvars.time + 5;
	}
	self = oldself;
}

/*==============================
 TF_T_Damage
 same thing as T_Damage (see above), just with some more details

 T_Flags:
	#TF_TD_IGNOREARMOUR: bypasses the armour of the target
	#TF_TD_NOTTEAM: doesn't damage a team member
	#TF_TD_NOTSELF: doesn't damage self

 The following is used to determine whether this attack is affected 
 the type of armor the defender is wearing.
 T_AttackType:		
	#TF_TD_OTHER			:	type ignored
	#TF_TD_SHOT			: 	bullet damage
	#TF_TD_NAIL			:	nailgun damage
	#TF_TD_EXPLOSION		:	explosion damage
	#TF_TD_ELECTRICITY	:	electricity damage
	#TF_TD_FIRE			:	fire damage

	#TF_TD_NOSOUND		:	Special Value. Health is adjusted without
						    any sound, painframe, etc
							Health is _set_ to damage, not altered.

==============================*/

void TF_T_Damage( gedict_t * targ, gedict_t * inflictor, gedict_t * attacker,
		  float damage, int T_flags, int T_AttackType )
{
	vec3_t  dir;
	gedict_t *oldself;
	gedict_t *te;
	float   save;
	float   take;
	int     olddmsg;
	float   no_damage;
	float   moment;

	if ( !targ->s.v.takedamage )
		return;
	if ( T_AttackType & TF_TD_NOSOUND )
	{
	        if(streq(targ->s.v.classname,"player"))
	        	G_sprint(targ,2,"!!!BUG BUG BUG!!! killed by TF_TD_NOSOUND\n");

		targ->s.v.health = damage;
		return;
	}
	if ( tf_data.cease_fire )
		return;
	no_damage = 0;
	if ( streq( attacker->s.v.classname, "player" ) )
	{
		damage = damage * 0.9;
		// check for quad damage powerup on the attacker
		if ( attacker->super_damage_finished > g_globalvars.time )
			damage = damage * 4;

		if ( strneq( targ->s.v.classname, "player" )
		     && strneq( targ->s.v.classname, "bot" )
		     && strneq( targ->s.v.classname, "building_sentrygun" )
		     && strneq( targ->s.v.classname, "building_dispenser" )
		     && strneq( targ->s.v.classname, "building_teleporter_entrance" )
		     && strneq( targ->s.v.classname, "building_teleporter_exit" ) )
		{
			if ( !Activated( targ, attacker ) )
			{
			        // If an else goal should be activated, activate it
				if ( targ->else_goal )
				{
					te = Findgoal( targ->else_goal );
					if ( te )
						AttemptToActivate( te, attacker, targ );
				}
				return;
			}
		}
	}
	// used by buttons and triggers to set activator for target firing
	damage_attacker = attacker;

	if ( teamplay & ( TEAMPLAY_LESSSCOREHELP | TEAMPLAY_LESSPLAYERSHELP ) )
		damage = TeamEqualiseDamage( targ, attacker, damage );

	// Do armorclass stuff
	if ( targ->armorclass && T_AttackType )
	{
		if ( ( targ->armorclass & AT_SAVESHOT ) && ( T_AttackType & TF_TD_SHOT ) )
			damage = floor( damage * 0.5 );
		if ( ( targ->armorclass & AT_SAVENAIL ) && ( T_AttackType & TF_TD_NAIL ) )
			damage = floor( damage * 0.5 );
		if ( ( targ->armorclass & AT_SAVEEXPLOSION ) && ( T_AttackType & TF_TD_EXPLOSION ) )
			damage = floor( damage * 0.5 );
		if ( ( targ->armorclass & AT_SAVEELECTRICITY ) && ( T_AttackType & TF_TD_ELECTRICITY ) )
			damage = floor( damage * 0.5 );
		if ( ( targ->armorclass & AT_SAVEFIRE ) && ( T_AttackType & TF_TD_FIRE ) )
			damage = floor( damage * 0.5 );
	}
	// save damage based on the target's armor level
	if ( T_flags & TF_TD_IGNOREARMOUR )
	{
		take = damage;
		save = 0;
	} else
	{
		save = ceil( targ->s.v.armortype * damage );
		if ( streq( attacker->s.v.classname, "player" ) && targ->team_no > 0
		     && targ->team_no == attacker->team_no && targ != attacker && ( T_flags & TF_TD_NOTTEAM ) )
		{
			if ( T_AttackType & TF_TD_EXPLOSION )
			{
				if ( teamplay & TEAMPLAY_NOARMOR_EXPLOSIVE )
					save = 0;
				else
				{
					if ( teamplay & TEAMPLAY_HALFARMOR_EXPLOSIVE )
						save = save / 2;
				}
			} else
			{
				if ( teamplay & TEAMPLAY_NOARMOR_DIRECT )
					save = 0;
				else
				{
					if ( teamplay & TEAMPLAY_HALFARMOR_DIRECT )
						save = save / 2;
				}
			}
		}
// save damage based on the target's armor level

		save = ceil( targ->s.v.armortype * damage );
		if ( save >= targ->s.v.armorvalue )
		{
			save = targ->s.v.armorvalue;
			targ->s.v.armortype = 0;	// lost all armor
			targ->armorclass = 0;	       // lost special armor
			targ->s.v.items -= ( ( int ) targ->s.v.items & ( IT_ARMOR1 | IT_ARMOR2 | IT_ARMOR3 ) );
		}

		targ->s.v.armorvalue = targ->s.v.armorvalue - save;
		take = ceil( damage - save );
	}
	// add to the damage total for clients, which will be sent as a single
	// message at the end of the frame
	// FIXME: remove after combining shotgun blasts?
	if ( ( int ) targ->s.v.flags & FL_CLIENT )
	{
		targ->s.v.dmg_take += take;
		targ->s.v.dmg_save += save;
		targ->s.v.dmg_inflictor = EDICT_TO_PROG( inflictor );
		targ->eff_info.dmg_curr += ( 0.5 * take + 0.5 * save ) * ( T_flags & TF_TD_MOREKICK ? 4 : 1 );
		targ->eff_info.dmg = targ->eff_info.dmg_curr;
	}
	damage_inflictor = inflictor;

	// figure momentum add
	if ( inflictor != world && targ->s.v.movetype == MOVETYPE_WALK && !( targ->tfstate & TFSTATE_CANT_MOVE ) )
	{
	        // Nail Gren doesn't knock ppl
		if ( tf_data.deathmsg != DMSG_GREN_NAIL )
		{
                        //   targ.immune_to_check = g_globalvars.time + damage / 20;
			dir[0] = targ->s.v.origin[0] - ( inflictor->s.v.absmin[0] + inflictor->s.v.absmax[0] ) * 0.5;
			dir[1] = targ->s.v.origin[1] - ( inflictor->s.v.absmin[1] + inflictor->s.v.absmax[1] ) * 0.5;
			dir[2] = targ->s.v.origin[2] - ( inflictor->s.v.absmin[2] + inflictor->s.v.absmax[2] ) * 0.5;
			VectorNormalize( dir );
			
			if ( targ->playerclass == PC_HVYWEAP )
			{
#ifdef NEWHWGUY
				if ( targ != attacker )
				{
#endif
					
					moment = damage / 3;
					if ( damage <= 50 )
						moment = 0;
#ifdef NEWHWGUY
				} else
				{
					moment = damage / 2;
				}
#endif
			} else
				moment = damage;
			
			// Set kickback for smaller weapons
			// Read: only if it's not yourself doing the damage
			if ( moment < 60 && streq( attacker->s.v.classname, "player" )
			     && streq( targ->s.v.classname, "player" )
			     && strneq( attacker->s.v.netname, targ->s.v.netname ) )
			{
				targ->s.v.velocity[0] += dir[0] * moment * 11;
				targ->s.v.velocity[1] += dir[1] * moment * 11;
				targ->s.v.velocity[2] += dir[2] * moment * 11;

			} else
			// Otherwise, these rules apply to rockets and grenades                        
			// for blast velocity
			{
				targ->s.v.velocity[0] += dir[0] * moment * 8;
				targ->s.v.velocity[1] += dir[1] * moment * 8;
				targ->s.v.velocity[2] += dir[2] * moment * 8;
			}
			// Rocket Jump modifiers
			if ( ( rj > 1 )
			     && ( ( streq( attacker->s.v.classname, "player" ) )
				  && streq( targ->s.v.classname, "player" ) )
			     && streq( attacker->s.v.netname, targ->s.v.netname ) )
			{
				targ->s.v.velocity[0] += dir[0] * moment * rj;
				targ->s.v.velocity[1] += dir[1] * moment * rj;
				targ->s.v.velocity[2] += dir[2] * moment * rj;

			}
		}
	}
// check for godmode or invincibility
	if ( streq( targ->s.v.classname, "player" ) && tg_data.godmode )
		return;

	if ( ( int ) targ->s.v.flags & FL_GODMODE )
		return;

	if ( targ->invincible_finished >= g_globalvars.time )
	{
		if ( self->invincible_sound < g_globalvars.time )
		{
			sound( targ, CHAN_ITEM, "items/protect3.wav", 1, ATTN_NORM );
			self->invincible_sound = g_globalvars.time + 2;
		}
		return;
	}
	// team play damage avoidance
	if ( allowedClasname( attacker, targ ) )
    {
		if ( isTeamDamage( attacker, targ ) )
		{
			if ( T_AttackType & TF_TD_EXPLOSION )
			{
				if ( teamplay & TEAMPLAY_NOEXPLOSIVE )
					no_damage = 1;
				else
				{
					if ( teamplay & TEAMPLAY_HALFEXPLOSIVE )
						take /= 2;
				}
			} else
			{
				if ( teamplay & TEAMPLAY_NODIRECT )
					no_damage = 1;
				else
				{
					if ( teamplay & TEAMPLAY_HALFDIRECT )
						take = take / 2;
				}
			}
		}
	}
	if ( T_flags & TF_TD_NOTSELF )
	{
		if ( targ == attacker )
			return;
	}
	// do the damage, min 1
	if ( take < 1 )
		take = 1;
	take = Q_rint( take );	//take = floor( take );

	if ( !no_damage )
	{
		targ->s.v.health = targ->s.v.health - take;
	}

    if ( allowedClasname( attacker, targ ) )
	{
        if ( isTeamDamage( attacker, targ ) )
		{
			olddmsg = tf_data.deathmsg;
			if ( T_AttackType & TF_TD_EXPLOSION )
			{
				tf_data.deathmsg = DMSG_TEAMKILL;
				if ( teamplay & TEAMPLAY_MIRROR_EXPLOSIVE )
					TF_T_Damage( attacker, world, world, take, TF_TD_IGNOREARMOUR, 0 );
				else
				{
					if ( teamplay & TEAMPLAY_HALFMIRROR_EXPLOSIVE )
						TF_T_Damage( attacker, world, world, take / 2, TF_TD_IGNOREARMOUR, 0 );
				}
			} else
			{
				tf_data.deathmsg = DMSG_TEAMKILL;
				if ( teamplay & TEAMPLAY_MIRROR_DIRECT )
					TF_T_Damage( attacker, world, world, take, TF_TD_IGNOREARMOUR, 0 );
				else
				{
					if ( teamplay & TEAMPLAY_HALFMIRROR_DIRECT )
						TF_T_Damage( attacker, world, world, take / 2, TF_TD_IGNOREARMOUR, 0 );
				}
			}
			tf_data.deathmsg = olddmsg;
		}
	}
	if ( no_damage == 1 )
		return;
	if ( targ->s.v.armorvalue < 1 )
	{
		targ->armorclass = 0; // lost special armor
		targ->s.v.armorvalue = 0;
	}
	if ( targ->s.v.health <= 0 )
	{
		if ( streq( inflictor->s.v.classname, "detpack" )
		     && inflictor->weaponmode == 1 && inflictor->s.v.enemy == EDICT_TO_PROG( targ ) )
			tf_data.deathmsg = DMSG_DETPACK_DIS;
		Killed( targ, attacker );
		return;
	}
	// react to the damage
	oldself = self;
	self = targ;
	if ( self->th_pain )
	{
		self->th_pain( attacker, take );
		// nightmare mode monsters don't go into pain frames often
		if ( skill >= 3 )
			self->pain_finished = g_globalvars.time + 5;
	}
	self = oldself;
}

/*
============
T_RadiusDamage
============
*/
void T_RadiusDamage( gedict_t * inflictor, gedict_t * attacker, float damage, gedict_t * ignore )
{
	float   points;
	gedict_t *head;

//      gedict_t *te;
	vec3_t  org;

	head = trap_findradius( world, inflictor->s.v.origin, damage + 40 );
	while ( head )
	{
		if ( head != ignore )
		{
			if ( head->s.v.takedamage )
			{
				org[0] =
				    inflictor->s.v.origin[0] - ( head->s.v.origin[0] +
								 ( head->s.v.mins[0] + head->s.v.maxs[0] ) * 0.5 );
				org[1] =
				    inflictor->s.v.origin[1] - ( head->s.v.origin[1] +
								 ( head->s.v.mins[1] + head->s.v.maxs[1] ) * 0.5 );
				org[2] =
				    inflictor->s.v.origin[2] - ( head->s.v.origin[2] +
								 ( head->s.v.mins[2] + head->s.v.maxs[2] ) * 0.5 );
				points = 0.5 * vlen( org );
				if ( points < 0 )
					points = 0;
				points = damage - points;
				if ( head == attacker )
					points = points * 0.75;
				if ( points > 0 )
				{
					if ( CanDamage( head, inflictor ) )
					{// shambler takes half damage from all explosions
						if ( streq( head->s.v.classname, "monster_shambler" ) )
							T_Damage( head, inflictor, attacker, points * 0.5 );
						else
							TF_T_Damage( head, inflictor, attacker, points, TF_TD_NOTTEAM, TF_TD_EXPLOSION );
					}
				}
			}
		}
		head = trap_findradius( head, inflictor->s.v.origin, damage + 40 );
	}
}

/*
============
T_BeamDamage
============
*/
void T_BeamDamage( gedict_t * attacker, float damage )
{
	vec3_t  tmpv;
	float   points;
	gedict_t *head;

	head = trap_findradius( world, attacker->s.v.origin, damage + 40 );
	while ( head )
	{
		if ( head->s.v.takedamage )
		{
			VectorSubtract( attacker->s.v.origin, head->s.v.origin, tmpv ) points = 0.5 * vlen( tmpv );
			if ( points < 0 )
				points = 0;
			points = damage - points;
			if ( head == attacker )
				points = points * 0.5;
			if ( points > 0 )
			{
				if ( CanDamage( head, attacker ) )
					T_Damage( head, attacker, attacker, points );
			}
		}
		head = trap_findradius( head, attacker->s.v.origin, damage + 40 );
	}
}


void ApplyDmgRoll( gedict_t* self )
{
    float dmg = self->eff_info.dmg_curr;
    float val;

    if( dmg <= 0 ) return;
    //self->s.v.v_angle[ROLL]  += conc_idle * sin(g_globalvars.time * v_iroll_cycle) * 0.1;

//#define DMG_ROLL 0.2
#define DMG_ROLL 0.2
    val = dmg * DMG_ROLL;
    self->s.v.v_angle[PITCH] += val * crandom();
    self->s.v.v_angle[YAW]   += val * crandom();
}

void  TempEffectCoord( vec3_t origin, int type )
{
    trap_WriteByte( MSG_MULTICAST, SVC_TEMPENTITY );
    trap_WriteByte( MSG_MULTICAST, type );
    trap_WriteCoord( MSG_MULTICAST, origin[0] );
    trap_WriteCoord( MSG_MULTICAST, origin[1] );
    trap_WriteCoord( MSG_MULTICAST, origin[2] );
    trap_multicast( PASSVEC3( origin ), MULTICAST_PHS );
}

void  TempEffectCoordPVS( vec3_t origin, int type )
{
    trap_WriteByte( MSG_MULTICAST, SVC_TEMPENTITY );
    trap_WriteByte( MSG_MULTICAST, type );
    trap_WriteCoord( MSG_MULTICAST, origin[0] );
    trap_WriteCoord( MSG_MULTICAST, origin[1] );
    trap_WriteCoord( MSG_MULTICAST, origin[2] );
    trap_multicast( PASSVEC3( origin ), MULTICAST_PVS );
}

