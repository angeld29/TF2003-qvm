#include "g_local.h"

/*
==============================================================================

MOVETARGET CODE

The angle of the movetarget effects standing and bowing direction, but has no effect on movement, which allways heads to the next target.

targetname
must be present.  The name of this movetarget.

target
the next spot to move to.  If not present, stop here for good.

pausetime
The number of seconds to spend standing or bowing for path_stand or path_bow

==============================================================================
*/


/*
=============
t_movetarget

Something has bumped into a movetarget.  If it is a monster
moving towards it, change the next destination and continue.
==============
*/
void t_movetarget(  )
{
	gedict_t *temp;
	vec3_t  tmpv;

	if ( other->movetarget != self )
		return;

	if ( other->s.v.enemy )
		return;		// fighting, not following a path

	temp = self;
	self = other;
	other = temp;

	self->movetarget = trap_find( world, FOFS( s.v.targetname ), other->s.v.target );
	self->s.v.goalentity = EDICT_TO_PROG( self->movetarget );

	VectorSubtract( self->movetarget->s.v.origin, self->s.v.origin, tmpv );
	self->s.v.ideal_yaw = vectoyaw( tmpv );
	if ( !self->movetarget )
	{
		self->pausetime = g_globalvars.time + 999999;
//  self.th_stand (); //monster code
		return;
	}
}


void movetarget_f(  )
{
	if ( !self->s.v.targetname )
		G_Error( "monster_movetarget: no targetname" );

	self->s.v.solid = SOLID_TRIGGER;
	self->s.v.touch = ( func_t ) t_movetarget;
	setsize( self, -8, -8, -8, 8, 8, 8 );

}

/*QUAKED path_corner (0.5 0.3 0) (-8 -8 -8) (8 8 8)
Monsters will continue walking towards the next target corner.
*/
void SP_path_corner(  )
{
	if ( !CheckExistence(  ) )
	{
		dremove( self );
		return;
	}
	movetarget_f(  );
}

//============================================================================

/*//REMOVE!!!
void HuntTarget()
{
 self.goalgedict_t* = self->s.v.enemy;
 self->s.v.think = (func_t) self.th_run;
 self.ideal_yaw = vectoyaw(self->s.v.enemy->s.v.origin - self->s.v.origin);
 self->s.v.nextthink = g_globalvars.time + 0.1;
 SUB_AttackFinished(1);
}

float FindTarget()
{
 gedict_t* client;

 client = checkclient();
 if (!client) 
  return 0;
 if (client.flags & 128) 
  return 0;
 if (client->s.v.items & 524288) 
  return 0;
 if (!visible(client)) 
  return 0;
 if (strneq(client->s.v.classname, "player")) 
  return;
 self->s.v.enemy = client;
 HuntTarget();
 return 1;
};*/
