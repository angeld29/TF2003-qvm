#include "g_local.h"
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

void Grapple_Track(  )
{
//	vec3_t  spray;
	gedict_t *owner = PROG_TO_EDICT( self->s.v.owner );
	gedict_t *enemy = PROG_TO_EDICT( self->s.v.enemy );

	if ( !owner->on_hook || owner->s.v.health <= 0 )
	{
		Reset_Grapple( self );
		return;
	}
	if ( strneq( enemy->s.v.classname, "player" ) )
		VectorCopy( enemy->s.v.velocity, self->s.v.velocity );
	self->s.v.nextthink = g_globalvars.time + 0.1;
}

gedict_t *MakeLink( )
{
	newmis = spawn(  );
	g_globalvars.newmis = EDICT_TO_PROG( newmis );
	newmis->s.v.movetype = MOVETYPE_FLYMISSILE;
	newmis->s.v.solid = SOLID_NOT;
	newmis->s.v.owner = EDICT_TO_PROG( self );
	SetVector( newmis->s.v.avelocity, 200, 200, 200 );
	setmodel( newmis, "progs/s_spike.mdl" );
	setorigin( newmis, PASSVEC3( self->s.v.origin ) );
	setsize( newmis, 0, 0, 0, 0, 0, 0 );
	return newmis;
}

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

int Check_Overhead(  )
{
	vec3_t  src;
	vec3_t  end;
	vec3_t  tmp;
	gedict_t *owner = PROG_TO_EDICT( self->s.v.owner );

	makevectors( owner->s.v.angles );
	VectorCopy( owner->s.v.origin, src );
	VectorCopy( owner->s.v.origin, end );
	src[2] -= 24;
	end[2] -= 24;

	traceline( PASSVEC3( src ), PASSVEC3( end ), 0, owner );
	if ( g_globalvars.trace_fraction != 1 )
		return 0;

	VectorCopy( owner->s.v.origin, src );
	VectorCopy( owner->s.v.origin, end );
	src[2] -= 24;
	end[2] -= 24;
	VectorScale( g_globalvars.v_forward, 16, tmp );
	VectorSubtract( src, tmp, src );
	VectorCopy( src, end );
	end[2] += 58;
	traceline( PASSVEC3( src ), PASSVEC3( end ), 0, owner );
	if ( g_globalvars.trace_fraction != 1 )
		return 0;

	VectorCopy( owner->s.v.origin, src );
	VectorCopy( owner->s.v.origin, end );
	src[2] -= 24;
	end[2] -= 24;
	VectorScale( g_globalvars.v_forward, 16, tmp );
	VectorAdd( src, tmp, src );
	VectorCopy( src, end );
	end[2] += 58;
	traceline( PASSVEC3( src ), PASSVEC3( end ), 0, owner );
	if ( g_globalvars.trace_fraction != 1 )
		return 0;

	VectorCopy( owner->s.v.origin, src );
	VectorCopy( owner->s.v.origin, end );
	src[2] -= 24;
	end[2] -= 24;
	VectorScale( g_globalvars.v_right, 16, tmp );
	VectorSubtract( src, tmp, src );
	VectorCopy( src, end );
	end[2] += 58;
	traceline( PASSVEC3( src ), PASSVEC3( end ), 0, owner );
	if ( g_globalvars.trace_fraction != 1 )
		return 0;

	VectorCopy( owner->s.v.origin, src );
	VectorCopy( owner->s.v.origin, end );
	src[2] -= 24;
	end[2] -= 24;
	VectorScale( g_globalvars.v_right, 16, tmp );
	VectorAdd( src, tmp, src );
	VectorCopy( src, end );
	end[2] += 58;
	traceline( PASSVEC3( src ), PASSVEC3( end ), 0, owner );
	if ( g_globalvars.trace_fraction != 1 )
		return 0;

	return 1;
}

void Anchor_Grapple(  )
{
	gedict_t *owner = PROG_TO_EDICT( self->s.v.owner );
	int     test;

	if ( other == owner )
		return;
	if ( streq( other->s.v.classname, "missile" ) || streq( other->s.v.classname, "grenade" )
	     || streq( other->s.v.classname, "spike" ) || streq( other->s.v.classname, "hook" ) )
		return;
	if ( trap_pointcontents( PASSVEC3( self->s.v.origin ) ) == -6 )
	{
		Reset_Grapple( self );
		return;
	}
	if ( streq( other->s.v.classname, "player" ) )
	{
		Reset_Grapple( self );
		return;
	} else
	{
		sound( self, 1, "player/axhit2.wav", 1, 1 );
		if ( other->s.v.takedamage )
			T_Damage( other, self, owner, 1 );
		SetVector( self->s.v.velocity, 0, 0, 0 );
		SetVector( self->s.v.avelocity, 0, 0, 0 );
	}
	sound( owner, 1, "weapons/tink1.wav", 1, 1 );
	if ( !owner->s.v.button0 )
	{
		Reset_Grapple( self );
		return;
	}
	test = Check_Overhead(  );
	if ( !test )
	{
		Reset_Grapple( self );
		return;
	}
	owner->on_hook = 1;
	if ( ( int ) owner->s.v.flags & 512 )
	{
		owner->s.v.flags = ( int ) owner->s.v.flags - 512;
		setorigin( owner, PASSVEC3( owner->s.v.origin ) + 1 );
	}
	owner->lefty = 1;
	self->s.v.enemy = EDICT_TO_PROG( other );
	self->s.v.think = ( func_t ) Grapple_Track;
	self->s.v.nextthink = g_globalvars.time;
	self->s.v.solid = SOLID_NOT;
	self->s.v.touch = ( func_t ) SUB_Null;
}

void Throw_Grapple(  )
{
	vec3_t  v;

	if ( self->hook_out )
		return;
	KickPlayer( -1, self );
	newmis = spawn(  );
	g_globalvars.newmis = EDICT_TO_PROG( newmis );
	newmis->s.v.movetype = MOVETYPE_FLYMISSILE;
	newmis->s.v.solid = SOLID_BBOX;
	newmis->s.v.owner = EDICT_TO_PROG( self );
	self->hook = newmis;
	newmis->s.v.classname = "hook";
	makevectors( self->s.v.v_angle );
	VectorScale( g_globalvars.v_forward, 800, newmis->s.v.velocity );
	vectoangles( newmis->s.v.velocity, newmis->s.v.angles );
	newmis->s.v.touch = ( func_t ) Anchor_Grapple;
	newmis->s.v.think = ( func_t ) Build_Chain;
	newmis->s.v.nextthink = g_globalvars.time + 0.1;
	setmodel( newmis, "progs/hook.mdl" );
	VectorScale( g_globalvars.v_forward, 16, v );
	VectorAdd( self->s.v.origin, v, v );
	v[2] += 16;
	setorigin( newmis, PASSVEC3( v ) );
	setsize( newmis, 0, 0, 0, 0, 0, 0 );
	self->hook_out = 1;
	self->fire_held_down = 1;
}

void Service_Grapple(  )
{
	vec3_t  hook_dir;
	gedict_t *enemy;
	gedict_t *goalentity;

	if ( !self->s.v.button0 )
	{
		self->fire_held_down = 0;
		if ( self->current_weapon == WEAP_HOOK )
			Reset_Grapple( self->hook );
	}
	enemy = PROG_TO_EDICT( self->hook->s.v.enemy );
	if ( streq( enemy->s.v.classname, "player" ) )
	{
		VectorSubtract( enemy->s.v.origin, self->s.v.origin, hook_dir );
	}
	else
	{
		if ( strneq( enemy->s.v.classname, "player" ) )
			VectorSubtract( self->hook->s.v.origin, self->s.v.origin, hook_dir );
	}
	normalize( hook_dir, self->s.v.velocity );
	VectorScale( self->s.v.velocity, self->maxfbspeed * 1.5, self->s.v.velocity );
	if ( vlen( hook_dir ) <= 100 && self->lefty )
	{
		if ( self->hook->s.v.goalentity )
		{
			goalentity = PROG_TO_EDICT( self->hook->s.v.goalentity );
			goalentity->s.v.think = ( func_t ) Remove_Chain;
			goalentity->s.v.nextthink = g_globalvars.time;
		}
		self->lefty = 0;
	}
}
