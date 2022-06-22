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
 *  $Id: hook.c,v 1.5 2006-11-29 23:19:23 AngelD Exp $
 */
/*
===========================================================================
Quakeworld-friendly grapple hook code by Wedge (Steve Bond)
           visit Quake Command http://www.nuc.net/quake 


Original 'Morning Star' (Grapple Hook) by "Mike" <amichael@asu.alasu.edu> 
I took care to preserve the speed and damage values of the original
Morning Star. Depending on latency, performance should be near exact.
===========================================================================
*/

#include "g_local.h"
//
// Reset_Grapple - Removes the hook and resets its owner's state.
//                 expects a pointer to the hook
//
void Reset_Grapple( gedict_t * rhook )
{
	gedict_t *owner = PROG_TO_EDICT( rhook->s.v.owner );

	sound( owner, 1, "weapons/bounce.wav", 1, 0 );
	owner->on_hook = 0;
	owner->hook_out = 0;
	owner->fire_held_down = 0;
	owner->s.v.weaponframe = 0;
	rhook->s.v.think = ( func_t ) SUB_Remove;
	rhook->s.v.nextthink = g_globalvars.time;
}

//
// Grapple_Track - Constantly updates the hook's position relative to
//                 what it's hooked to. Inflicts damage if attached to
//                 a player that is not on the same team as the hook's
//                 owner.
//
void Grapple_Track(  )
{
//	vec3_t  spray;
	gedict_t *owner = PROG_TO_EDICT( self->s.v.owner );
	gedict_t *enemy = PROG_TO_EDICT( self->s.v.enemy );

	// drop the hook if owner is dead or has released the button
	if ( !owner->on_hook || owner->s.v.health <= 0 )
	{
		Reset_Grapple( self );
		return;
	}
        // If the hook is not attached to the player, constantly copy
        // copy the target's velocity. Velocity copying DOES NOT work properly
        // for a hooked client. 
	if ( strneq( enemy->s.v.classname, "player" ) )
		VectorCopy( enemy->s.v.velocity, self->s.v.velocity );
	self->s.v.nextthink = g_globalvars.time + 0.1;
}

//
// MakeLink - spawns the chain link entities
//
gedict_t *MakeLink( )
{
	newmis = spawn(  );

	g_globalvars.newmis = EDICT_TO_PROG( newmis );
	newmis->s.v.movetype = MOVETYPE_FLYMISSILE;
	newmis->s.v.solid = SOLID_NOT;
	newmis->s.v.owner = EDICT_TO_PROG( self ); // SELF is the hook!

	SetVector( newmis->s.v.avelocity, 200, 200, 200 );

	setmodel( newmis, "progs/s_spike.mdl" );
	setorigin( newmis, PASSVEC3( self->s.v.origin ) );
	setsize( newmis, 0, 0, 0, 0, 0, 0 );

	return newmis;
}

//
// Remove_Chain - Removes all chain link entities; this is a separate
//                function because CLIENT also needs to be able
//                to remove the chain. Only one function required to
//                remove all links.
//
void Remove_Chain(  )
{
	gedict_t *goalentity;

	self->s.v.think = ( func_t ) SUB_Remove;
	self->s.v.nextthink = g_globalvars.time;
	if ( self->s.v.goalentity )
	{
		goalentity = PROG_TO_EDICT( self->s.v.goalentity );
		goalentity->s.v.think = ( func_t ) SUB_Remove;
		goalentity->s.v.nextthink = g_globalvars.time;
		if ( goalentity->s.v.goalentity )
		{
			goalentity = PROG_TO_EDICT( goalentity->s.v.goalentity );
			goalentity->s.v.think = ( func_t ) SUB_Remove;
			goalentity->s.v.nextthink = g_globalvars.time;
		}
	}
}

//
// Update_Chain - Repositions the chain links each frame. This single function
//                maintains the positions of all of the links. Only one link
//                is thinking every frame. 
//
void Update_Chain(  )
{
	vec3_t  temp, v;
	gedict_t *owner = PROG_TO_EDICT( self->s.v.owner );

	if ( !owner->hook_out )
	{
		self->s.v.think = ( func_t ) Remove_Chain;
		self->s.v.nextthink = g_globalvars.time;
		return;
	}
	VectorSubtract( owner->hook->s.v.origin, owner->s.v.origin, temp );

        // These numbers are correct assuming 3 links.
        // 4 links would be *20 *40 *60 and *80
	VectorScale( temp, 0.25, v );
	VectorAdd( v, owner->s.v.origin, v );
	setorigin( self, PASSVEC3( v ) );

	VectorScale( temp, 0.5, v );
	VectorAdd( v, owner->s.v.origin, v );
	setorigin( self, PASSVEC3( v ) );

	VectorScale( temp, 0.75, v );
	VectorAdd( v, owner->s.v.origin, v );
	setorigin( self, PASSVEC3( v ) );

	self->s.v.nextthink = g_globalvars.time + 0.1;
}

//
// Build_Chain - Builds the chain (linked list)
//
void Build_Chain(  )
{
	gedict_t *goalentity;

	goalentity = MakeLink(  );
	self->s.v.goalentity = EDICT_TO_PROG( goalentity );
	goalentity->s.v.think = ( func_t ) Update_Chain;
	goalentity->s.v.nextthink = g_globalvars.time + 0.1;
	goalentity->s.v.owner = self->s.v.owner;

	goalentity->s.v.goalentity = EDICT_TO_PROG( MakeLink(  ) );
	PROG_TO_EDICT( goalentity->s.v.goalentity )->s.v.goalentity = EDICT_TO_PROG( MakeLink(  ) );
}

//
// Check_Overhead - Makes sure there is sufficient headroom above the player
//                  so that setorigin doesn't stick them into a wall. I tried
//                  to compare pointcontents, but that was too flaky.
//

static int checkDir( gedict_t* owner, const vec3_t v, int sign)
{
    traceline(
            owner->s.v.origin[0]        + sign * v[0] * 16,
            owner->s.v.origin[1]        + sign * v[1] * 16,
            owner->s.v.origin[2] - 24   + sign * v[2] * 16,
            owner->s.v.origin[0]        + sign * v[0] * 16,
            owner->s.v.origin[1]        + sign * v[1] * 16,
            owner->s.v.origin[2] - 24   + sign * v[2] * 16 + 58,
            0, owner);
    return g_globalvars.trace_fraction;
}

int Check_Overhead(  )
{
	gedict_t *owner = PROG_TO_EDICT( self->s.v.owner );

	trap_makevectors( owner->s.v.angles );

        // The following comparisons could be optimized by doing away with
        // SRC and END, and plugging the values directly into the traceline
        // function calls. Using SRC and END made debugging easier. You
        // decide if it's worth it.

        // quick check right above head
    if( checkDir( owner, VEC_ORIGIN, 0) != 0 )
        return 0;

    if( checkDir( owner, g_globalvars.v_forward, -1) != 0 )
        return 0;

    if( checkDir( owner, g_globalvars.v_forward, 1) != 0 )
        return 0;

    if( checkDir( owner, g_globalvars.v_right, -1) != 0 )
        return 0;

    if( checkDir( owner, g_globalvars.v_right, 1) != 0 )
        return 0;

	return 1;
}

//
// Anchor_Grapple - Tries to anchor the grapple to whatever it touches
//
void Anchor_Grapple(  )
{
	gedict_t *owner = PROG_TO_EDICT( self->s.v.owner );
	int     test;

	if ( other == owner )
		return;

        // DO NOT allow the grapple to hook to any projectiles, no matter WHAT!
        // if you create new types of projectiles, make sure you use one of the
        // classnames below or write code to exclude your new classname so
        // grapples will not stick to them.
	if ( streq( other->s.v.classname, "missile" ) || streq( other->s.v.classname, "grenade" )
	     || streq( other->s.v.classname, "spike" ) || streq( other->s.v.classname, "hook" ) )
		return;

        // Don't stick the the sky.
	if ( trap_pointcontents( PASSVEC3( self->s.v.origin ) ) == CONTENT_SKY )
	{
		Reset_Grapple( self );
		return;
	}
	// Cannot hurt players with the grapple
	if ( streq( other->s.v.classname, "player" ) )
	{
		Reset_Grapple( self );
		return;
	} else
	{
		sound( self, 1, "player/axhit2.wav", 1, 1 );

                // One point of damage inflicted upon impact. Subsequent
                // damage will only be done to PLAYERS... this way secret
                // doors and triggers will only be damaged once.
		if ( other->s.v.takedamage )
			T_Damage( other, self, owner, 1 );
		SetVector( self->s.v.velocity, 0, 0, 0 );
		SetVector( self->s.v.avelocity, 0, 0, 0 );
	}
        // conveniently clears the sound channel of the CHAIN1 sound,
        // which is a looping sample and would continue to play. Tink1 is
        // the least offensive choice, as NULL.WAV loops and clogs the
        // channel with silence
	sound( owner, 1, "weapons/tink1.wav", 1, 1 );
	if ( !owner->s.v.button0 )
	{
		Reset_Grapple( self );
		return;
	}
        // our last chance to avoid being picked up off of the ground.
        // check over the client's head to make sure there is one unit
        // clearance so we can lift him off of the ground.
	test = Check_Overhead(  );
	if ( !test )
	{
		Reset_Grapple( self );
		return;
	}
	owner->on_hook = 1;
	if ( ( int ) owner->s.v.flags & 512 )
	{
		owner->s.v.flags = ( int ) owner->s.v.flags - FL_ONGROUND;
		setorigin( owner, PASSVEC3( owner->s.v.origin ) + 1 );
	}
        // CHAIN2 is a looping sample. Use LEFTY as a flag so that client.qc
        // will know to only play the tink sound ONCE to clear the weapons
        // sound channel. (Lefty is a leftover from AI.QC, so I reused it to
        // avoid adding a field)
	owner->lefty = 1;
	self->s.v.enemy = EDICT_TO_PROG( other );// remember this guy!
	self->s.v.think = ( func_t ) Grapple_Track;
	self->s.v.nextthink = g_globalvars.time;
	self->s.v.solid = SOLID_NOT;
	self->s.v.touch = ( func_t ) SUB_Null;
}

//
// Throw_Grapple - called from WEAPONS.QC, 'fires' the grapple
//
void Throw_Grapple(  )
{
	vec3_t  v;

	if ( self->hook_out )// reject subsequent calls from player.qc
		return;

	KickPlayer( -1, self );

	newmis = spawn(  );
	g_globalvars.newmis = EDICT_TO_PROG( newmis );
	newmis->s.v.movetype = MOVETYPE_FLYMISSILE;
	newmis->s.v.solid = SOLID_BBOX;
	newmis->s.v.owner = EDICT_TO_PROG( self );// newmis belongs to me
	self->hook = newmis;// This is my newmis
	newmis->s.v.classname = "hook";

	trap_makevectors( self->s.v.v_angle );
	VectorScale( g_globalvars.v_forward, 800, newmis->s.v.velocity );
	// set the facing of the grapple
	vectoangles( newmis->s.v.velocity, newmis->s.v.angles );

	newmis->s.v.touch = ( func_t ) Anchor_Grapple;
	newmis->s.v.think = ( func_t ) Build_Chain;
	newmis->s.v.nextthink = g_globalvars.time + 0.1; // don't jam newmis and links into same packet

	setmodel( newmis, "progs/hook.mdl" );
	VectorScale( g_globalvars.v_forward, 16, v );
	VectorAdd( self->s.v.origin, v, v );
	v[2] += 16;
	setorigin( newmis, PASSVEC3( v ) );
	setsize( newmis, 0, 0, 0, 0, 0, 0 );

	self->hook_out = 1;
	self->fire_held_down = 1;
}

//
// Service_Grapple - called each frame by CLIENT.QC if client is ON_HOOK
//
void Service_Grapple(  )
{
	vec3_t  hook_dir;
	gedict_t *enemy;
	gedict_t *goalentity;

	// drop the hook if player lets go of button
	if ( !self->s.v.button0 )
	{
		self->fire_held_down = 0;
		if ( self->current_weapon == WEAP_HOOK )
			Reset_Grapple( self->hook );
	}
	enemy = PROG_TO_EDICT( self->hook->s.v.enemy );
	// If hooked to a player, track them directly!
	if ( streq( enemy->s.v.classname, "player" ) )
	{
		VectorSubtract( enemy->s.v.origin, self->s.v.origin, hook_dir );
	}
	// else, track to hook
	else
	{
		if ( strneq( enemy->s.v.classname, "player" ) )
			VectorSubtract( self->hook->s.v.origin, self->s.v.origin, hook_dir );
	}
	normalize( hook_dir, self->s.v.velocity );
	VectorScale( self->s.v.velocity, self->maxfbspeed * 1.5, self->s.v.velocity );
	if ( vlen( hook_dir ) <= 100 && self->lefty )
	{
                // If there is a chain, ditch it now. We're
                // close enough. Having extra entities lying around
                // is never a good idea.
		if ( self->hook->s.v.goalentity )
		{
			goalentity = PROG_TO_EDICT( self->hook->s.v.goalentity );
			goalentity->s.v.think = ( func_t ) Remove_Chain;
			goalentity->s.v.nextthink = g_globalvars.time;
		}
		self->lefty = 0;// we've reset the sound channel.
	}
}
