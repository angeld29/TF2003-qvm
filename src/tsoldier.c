#include "g_local.h"
void    NailGrenadeExplode(  );
void    NailGrenadeNailEm(  );
void    NailGrenadeLaunchNail(  );
void    GrenadeExplode(  );
void    superspike_touch(  );

void NailGrenadeTouch(  )
{
	if ( EDICT_TO_PROG( other ) == self->s.v.owner )
		return;
	sound( self, 1, "weapons/bounce.wav", 1, 1 );
	if ( VectorCompareF( self->s.v.velocity, 0, 0, 0 ) )
		SetVector( self->s.v.avelocity, 0, 0, 0 );
};

void NailGrenadeExplode(  )
{
	gedict_t *te, *owner;
	vec3_t  neworigin;

	owner = PROG_TO_EDICT( self->s.v.owner );
	owner->no_active_nail_grens += 1;
	if ( owner->no_active_nail_grens > 2 )
	{

		te = find( world, FOFS( s.v.classname ), "grenade" );
		while ( te )
		{
			if ( te->s.v.owner == self->s.v.owner && te->no_active_nail_grens == 1 )
			{
				te->s.v.weapon = 9;
				te->s.v.think = ( func_t ) GrenadeExplode;
				te->s.v.nextthink = g_globalvars.time + 0.1;
			}
			te = find( te, FOFS( s.v.classname ), "grenade" );
		}
	}
	self->no_active_nail_grens = owner->no_active_nail_grens;
	self->s.v.movetype = MOVETYPE_FLY;
	neworigin[0] = 0;
	neworigin[1] = 0;
	neworigin[2] = 32;
#ifndef	TG
	traceline( PASSVEC3( self->s.v.origin ), self->s.v.origin[0] + neworigin[0],
		   self->s.v.origin[1] + neworigin[1], self->s.v.origin[2] + neworigin[2], 1, self );

	if ( g_globalvars.trace_fraction != 1 )
	{
		neworigin[2] = neworigin[2] * g_globalvars.trace_fraction - 1;
	}
#endif
        VectorAdd( self->s.v.origin , neworigin, neworigin);
	setorigin( self, PASSVEC3(neworigin) );

	SetVector( self->s.v.avelocity, 0, 500, 0 );
	self->s.v.nextthink = g_globalvars.time + 0.7;
	self->s.v.think = ( func_t ) NailGrenadeNailEm;
}

void NailGrenadeNailEm(  )
{
	SetVector( self->s.v.velocity, 0, 0, 0 );
	self->s.v.nextthink = g_globalvars.time + 0.1;
	self->s.v.think = ( func_t ) NailGrenadeLaunchNail;
	self->playerclass = 0;
};

void NailGrenadeLaunchNail(  )
{
	float   i;
	float   j;
	float   current_yaw;

	i = 0;
	while ( i < 3 )
	{
		j = ( g_random(  ) + 2 ) * 5;
		current_yaw = anglemod( self->s.v.angles[1] + j );
		self->s.v.angles[1] = current_yaw;
		self->s.v.angles[0] = 0;
		self->s.v.angles[2] = 0;
		makevectors( self->s.v.angles );
		tf_data.deathmsg = DMSG_GREN_NAIL;
		launch_spike( self->s.v.origin, g_globalvars.v_forward );
		newmis->s.v.touch = ( func_t ) superspike_touch;
		newmis->s.v.weapon = 9;
		i = i + 1;
	}
	self->playerclass += 1;
	self->s.v.nextthink = g_globalvars.time + 0.1;
	if ( self->playerclass > 50 )
	{
		self->s.v.weapon = 9;
		self->s.v.think = ( func_t ) GrenadeExplode;
	}
};
