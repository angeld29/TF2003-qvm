//===========================================================================
// Spectator
// 
//===========================================================================
#include "g_local.h"
////////////////
// GlobalParams:
// time
// self
// params
///////////////
void SpectatorConnect(  )
{
	G_bprint( PRINT_MEDIUM, "Spectator %s entered the game\n", self->s.v.netname );

	self->s.v.goalentity = EDICT_TO_PROG( world );	// used for impulse 1 below

}

////////////////
// GlobalParams:
// self
///////////////
void SpectatorDisconnect(  )
{
	G_bprint( PRINT_MEDIUM, "Spectator %s  left the game\n", self->s.v.netname );
}

/*
================
SpectatorImpulseCommand

Called by SpectatorThink if the spectator entered an impulse
================
*/
void SpectatorImpulseCommand(  )
{
	gedict_t *goal;

	if ( self->s.v.impulse == 1 )
	{
		// teleport the spectator to the next spawn point
		// note that if the spectator is tracking, this doesn't do
		// much
		goal = PROG_TO_EDICT( self->s.v.goalentity );
		goal = find( goal, FOFS( s.v.classname ), "info_player_deathmatch" );
		if ( !goal )
			goal = find( world, FOFS( s.v.classname ), "info_player_deathmatch" );
		if ( goal )
		{
			setorigin( self, PASSVEC3( goal->s.v.origin ) );
			VectorCopy( goal->s.v.angles, self->s.v.angles );
			self->s.v.fixangle = true;	// turn this way immediately
		} else
			goal = world;
		self->s.v.goalentity = EDICT_TO_PROG( goal );
	}

	self->s.v.impulse = 0;
}

////////////////
// GlobalParams:
// time
// self
///////////////
void SpectatorThink(  )
{
	if ( self->s.v.impulse )
		SpectatorImpulseCommand(  );

}
