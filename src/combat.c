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
 *  $Id: combat.c,v 1.6 2004-12-23 03:16:15 AngelD Exp $
 */
#include "g_local.h"
void    ClientObituary( gedict_t * e1, gedict_t * e2 );

//============================================================================

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

	if ( self->s.v.health < -99 )
		self->s.v.health = -99;	// don't let sbar look bad if a player

	if ( self->s.v.movetype == MOVETYPE_PUSH || self->s.v.movetype == MOVETYPE_NONE )
	{			// doors, triggers, etc
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
	}
	damage_inflictor = inflictor;
// figure momentum add
	if ( ( inflictor != world ) && ( targ->s.v.movetype == MOVETYPE_WALK ) )
	{

		dir[0] = targ->s.v.origin[0] - ( inflictor->s.v.absmin[0] + inflictor->s.v.absmax[0] ) * 0.5;
		dir[1] = targ->s.v.origin[1] - ( inflictor->s.v.absmin[1] + inflictor->s.v.absmax[1] ) * 0.5;
		dir[2] = targ->s.v.origin[2] - ( inflictor->s.v.absmin[2] + inflictor->s.v.absmax[2] ) * 0.5;
		VectorNormalize( dir );
		if ( ( damage <
		       60 ) & ( streq( attacker->s.v.classname,
				       "player" ) ) & ( streq( targ->s.v.classname,
							       "player" ) ) &
		     strneq( attacker->s.v.netname, targ->s.v.netname ) )
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

	if ( streq( attacker->s.v.classname, "player" )
	     && ( streq( targ->s.v.classname, "player" ) || streq( targ->s.v.classname, "building_sentrygun" ) ) )
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
	damage_attacker = attacker;
	if ( teamplay & ( TEAMPLAY_LESSSCOREHELP | TEAMPLAY_LESSPLAYERSHELP ) )
		damage = TeamEqualiseDamage( targ, attacker, damage );
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
				if ( teamplay & 1024 )
					save = 0;
				else
				{
					if ( teamplay & 512 )
						save = save / 2;
				}
			} else
			{
				if ( teamplay & 256 )
					save = 0;
				else
				{
					if ( teamplay & 128 )
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
			targ->s.v.items -= ( ( int ) targ->s.v.items & ( IT_ARMOR1 | IT_ARMOR2 | IT_ARMOR3 ) );
		}

		targ->s.v.armorvalue = targ->s.v.armorvalue - save;
		take = ceil( damage - save );
	}
	if ( ( int ) targ->s.v.flags & FL_CLIENT )
	{
		targ->s.v.dmg_take += take;
		targ->s.v.dmg_save += save;
		targ->s.v.dmg_inflictor = EDICT_TO_PROG( inflictor );
	}
	damage_inflictor = inflictor;

	if ( inflictor != world && targ->s.v.movetype == MOVETYPE_WALK && !( targ->tfstate & TFSTATE_CANT_MOVE ) )
	{
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
			
			if ( moment < 60 && streq( attacker->s.v.classname, "player" )
			     && streq( targ->s.v.classname, "player" )
			     && strneq( attacker->s.v.netname, targ->s.v.netname ) )
			{
				targ->s.v.velocity[0] += dir[0] * moment * 11;
				targ->s.v.velocity[1] += dir[1] * moment * 11;
				targ->s.v.velocity[2] += dir[2] * moment * 11;

			} else
			{
				targ->s.v.velocity[0] += dir[0] * moment * 8;
				targ->s.v.velocity[1] += dir[1] * moment * 8;
				targ->s.v.velocity[2] += dir[2] * moment * 8;
			}
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
	if ( streq( attacker->s.v.classname, "player" )
	     && ( streq( targ->s.v.classname, "player" )
		  || streq( targ->s.v.classname, "building_sentrygun" )
		  || streq( targ->s.v.classname, "building_dispenser" )
		  || streq( targ->s.v.classname, "building_teleporter_entrance" )
		  || streq( targ->s.v.classname, "building_teleporter_exit" ) ) )
	{
		if ( targ->team_no > 0 && targ->team_no == attacker->team_no
		     && targ != attacker && ( T_flags & TF_TD_NOTTEAM ) )
		{
			if ( T_AttackType & TF_TD_EXPLOSION )
			{
				if ( teamplay & 16 )
					no_damage = 1;
				else
				{
					if ( teamplay & 8 )
						take = take / 2;
				}
			} else
			{
				if ( teamplay & 4 )
					no_damage = 1;
				else
				{
					if ( teamplay & 2 )
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
	if ( take < 1 )
		take = 1;
	take = rint( take );
	//take = floor( take );

	if ( !no_damage )
	{
		targ->s.v.health = targ->s.v.health - take;
	}

	if ( streq( attacker->s.v.classname, "player" )
	     && ( streq( targ->s.v.classname, "player" )
		  || streq( targ->s.v.classname, "building_sentrygun" )
		  || streq( targ->s.v.classname, "building_dispenser" )
		  || streq( targ->s.v.classname, "building_teleporter_entrance" )
		  || streq( targ->s.v.classname, "building_teleporter_exit" ) ) )
	{
		if ( targ->team_no > 0 && targ->team_no == attacker->team_no
		     && targ != attacker && ( T_flags & TF_TD_NOTTEAM ) )
		{
			olddmsg = tf_data.deathmsg;
			if ( T_AttackType & TF_TD_EXPLOSION )
			{
				tf_data.deathmsg = 37;
				if ( teamplay & 16384 )
					TF_T_Damage( attacker, world, world, take, 1, 0 );
				else
				{
					if ( teamplay & 8192 )
						TF_T_Damage( attacker, world, world, take / 2, 1, 0 );
				}
			} else
			{
				tf_data.deathmsg = 37;
				if ( teamplay & 4096 )
					TF_T_Damage( attacker, world, world, take, 1, 0 );
				else
				{
					if ( teamplay & 2048 )
						TF_T_Damage( attacker, world, world, take / 2, 1, 0 );
				}
			}
			tf_data.deathmsg = olddmsg;
		}
	}
	if ( no_damage == 1 )
		return;
	if ( targ->s.v.armorvalue < 1 )
	{
		targ->armorclass = 0;
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

void T_RadiusDamage( gedict_t * inflictor, gedict_t * attacker, float damage, gedict_t * ignore )
{
	float   points;
	gedict_t *head;

//      gedict_t *te;
	vec3_t  org;

	head = findradius( world, inflictor->s.v.origin, damage + 40 );
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
					{
						if ( streq( head->s.v.classname, "monster_shambler" ) )
							T_Damage( head, inflictor, attacker, points * 0.5 );
						else
							TF_T_Damage( head, inflictor, attacker, points, 2, 4 );
					}
				}
			}
		}
		head = findradius( head, inflictor->s.v.origin, damage + 40 );
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

	head = findradius( world, attacker->s.v.origin, damage + 40 );
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
		head = findradius( head, attacker->s.v.origin, damage + 40 );
	}
}
