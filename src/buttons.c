#include "g_local.h"

void    button_return(  );

void button_wait(  )
{
	self->state = STATE_TOP;
	self->s.v.nextthink = self->s.v.ltime + self->wait;
	self->s.v.think = ( func_t ) button_return;
	activator = PROG_TO_EDICT( self->s.v.enemy );

	SUB_UseTargets(  );

	self->s.v.frame = 1;	// use alternate textures
}

void button_done(  )
{
	self->state = STATE_BOTTOM;
}

void button_return(  )
{
	self->goal_state = 2;

	self->state = STATE_DOWN;

	SUB_CalcMove( self->pos1, self->speed, button_done );

	self->s.v.frame = 0;	// use normal textures

	if ( self->s.v.health )
		self->s.v.takedamage = DAMAGE_YES;	// can be shot again
};

void button_blocked(  )
{				// do nothing, just don't ome all the way back out
}

void button_fire(  )
{
	if ( self->state == STATE_UP || self->state == STATE_TOP )
		return;

	sound( self, CHAN_VOICE, self->s.v.noise, 1, ATTN_NORM );

	self->state = STATE_UP;

	SUB_CalcMove( self->pos2, self->speed, button_wait );

};

void button_use(  )
{
	self->s.v.enemy = EDICT_TO_PROG( activator );
	button_fire(  );
}


void button_touch(  )
{
	gedict_t *te;

	if ( tf_data.cb_prematch_time > g_globalvars.time )
		return;

	if ( other->s.v.health <= 0 )
		return;

	if ( strneq( other->s.v.classname, "player" ) )
		return;

	if ( self->goal_activation & 8 )
		return;
	if ( !other->playerclass )
		return;

	if ( !Activated( self, other ) )
	{
		if ( self->else_goal )
		{
			te = Findgoal( self->else_goal );
			if ( te )
				AttemptToActivate( te, other, self );
		}
		return;
	}
	self->s.v.enemy = EDICT_TO_PROG( other );
	button_fire(  );
};

void button_killed(  )
{
	if ( self->goal_activation & 8 )
		return;
	self->s.v.enemy = EDICT_TO_PROG( damage_attacker );
	self->s.v.health = self->s.v.max_health;
	self->s.v.takedamage = DAMAGE_NO;	// wil be reset upon return

	button_fire(  );
};

/*QUAKED func_button (0 .5 .8) ?
When a button is touched, it moves some distance in the direction of it's angle, triggers all of it's targets, waits some time, then returns to it's original position where it can be triggered again.

"angle"  determines the opening direction
"target" all entities with a matching targetname will be used
"speed"  override the default 40 speed
"wait"  override the default 1 second wait (-1 = never return)
"lip"  override the default 4 pixel lip remaining at end of move
"health" if set, the button must be killed instead of touched
"sounds"
0) steam metal
1) wooden clunk
2) metallic click
3) in-out
*/

void SP_func_button(  )
{
	float   ftemp;

	if ( !CheckExistence(  ) )
	{
		dremove( self );
		return;
	}
	if ( self->s.v.sounds == 0 )
	{
		trap_precache_sound( "buttons/airbut1.wav" );
		self->s.v.noise = "buttons/airbut1.wav";
	}
	if ( self->s.v.sounds == 1 )
	{
		trap_precache_sound( "buttons/switch21.wav" );
		self->s.v.noise = "buttons/switch21.wav";
	}
	if ( self->s.v.sounds == 2 )
	{
		trap_precache_sound( "buttons/switch02.wav" );
		self->s.v.noise = "buttons/switch02.wav";
	}
	if ( self->s.v.sounds == 3 )
	{
		trap_precache_sound( "buttons/switch04.wav" );
		self->s.v.noise = "buttons/switch04.wav";
	}

	SetMovedir(  );

	self->s.v.movetype = MOVETYPE_PUSH;
	self->s.v.solid = SOLID_BSP;
	setmodel( self, self->s.v.model );

	self->s.v.blocked = ( func_t ) button_blocked;
	self->s.v.use = ( func_t ) button_use;

	if ( self->s.v.health )
	{
		self->s.v.max_health = self->s.v.health;
		self->th_die = button_killed;
		self->s.v.takedamage = DAMAGE_YES;
	} else
		self->s.v.touch = ( func_t ) button_touch;

	if ( !self->speed )
		self->speed = 40;

	if ( !self->wait )
		self->wait = 1;

	if ( !self->lip )
		self->lip = 4;

	self->state = STATE_BOTTOM;

	VectorCopy( self->s.v.origin, self->pos1 );
	ftemp = ( fabs( DotProduct( self->s.v.movedir, self->s.v.size ) ) - self->lip );

	self->pos2[0] = self->pos1[0] + self->s.v.movedir[0] * ftemp;
	self->pos2[1] = self->pos1[1] + self->s.v.movedir[1] * ftemp;
	self->pos2[2] = self->pos1[2] + self->s.v.movedir[2] * ftemp;

	if ( ( int ) ( self->s.v.spawnflags ) & 32 )
		button_fire(  );
};
