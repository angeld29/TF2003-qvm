#include "g_local.h"
void    RemoveFlare(  );
void    RemoveOldFlare( int tno );
int     num_team_flares[5] = { 0, 0, 0, 0, 0 };

void FlareGrenadeTouch(  )
{
	sound( self, 1, "weapons/bounce.wav", 1, 1 );
	if ( trap_pointcontents( PASSVEC3( self->s.v.origin ) ) == CONTENT_SKY )
	{
		dremove( self );
		return;
	}
	if ( other == world )
	{
		self->s.v.movetype = MOVETYPE_NONE;
		SetVector( self->s.v.velocity, 0, 0, 0 );
	}
	if ( VectorCompareF( self->s.v.velocity, 0, 0, 0 ) )
	{
		SetVector( self->s.v.avelocity, 0, 0, 0 );
		self->s.v.touch = ( func_t ) SUB_Null;
	}
}

void FlareGrenadeThink(  )
{
	float   rnum;
	float   time_left;

	time_left = self->s.v.health - g_globalvars.time;
	if ( time_left > 33 )
	{
		rnum = g_random(  );
		if ( rnum < 0.5 )
			self->s.v.effects = 8;
		else
			self->s.v.effects = 0;
		self->s.v.nextthink = g_globalvars.time + 0.05 + g_random(  ) * 0.1;
	} else
	{
		if ( time_left > 31 )
		{
			rnum = g_random(  );
			if ( rnum < 0.5 )
				self->s.v.effects = 4;
			else
				self->s.v.effects = 8;
			self->s.v.nextthink = g_globalvars.time + 0.05 + g_random(  ) * 0.1;
		} else
		{
			if ( time_left > 15 )
			{
				self->s.v.effects = 4;
				self->s.v.nextthink = g_globalvars.time + 10;
			} else
			{
				if ( time_left < 1 )
					RemoveFlare(  );
				else
				{
					self->s.v.effects = 8;
					self->s.v.nextthink = g_globalvars.time + time_left;
				}
			}
		}
	}
}


void FlareGrenadeExplode(  )
{


	if ( self->s.v.weapon )
	{
		num_team_flares[( int ) self->s.v.weapon]++;
		if ( num_team_flares[( int ) self->s.v.weapon] > 9 / number_of_teams )
			RemoveOldFlare( self->s.v.weapon );
	} else
	{
		num_team_flares[0]++;
		if ( num_team_flares[0] > 9 )
			RemoveOldFlare( 0 );
	}
	self->s.v.skin = 1;
	self->s.v.health = g_globalvars.time + 40;
	self->s.v.nextthink = g_globalvars.time + 0.05 + g_random(  ) * 0.1;
	self->s.v.think = ( func_t ) FlareGrenadeThink;
}

void RemoveFlare(  )
{
	self->s.v.effects = ( int ) self->s.v.effects - ( ( int ) self->s.v.effects & 4 );
	dremove( self );
	if ( num_team_flares[( int ) self->s.v.weapon] )
		num_team_flares[( int ) self->s.v.weapon]--;
}


void RemoveOldFlare( int tno )
{
	gedict_t *old;
	int     index;

	index = num_team_flares[tno];
	index = index - 9 / number_of_teams;

	old = find( world, FOFS( mdl ), "flare" );
	while ( index > 0 )
	{
		if ( !old )
		{
			num_team_flares[0] = 0;
			num_team_flares[1] = 0;
			num_team_flares[2] = 0;
			num_team_flares[3] = 0;
			num_team_flares[4] = 0;
			return;
		}
		if ( old->s.v.weapon == tno || !tno )
		{
			old->s.v.think = ( func_t ) RemoveFlare;
			old->s.v.nextthink = g_globalvars.time + 0.1;
			index -= 1;
		}
		old = find( old, FOFS( mdl ), "flare" );
	}
}
