#include "g_local.h"
void    autokick_think(  );
void Admin_CountPlayers(  )
{

	G_sprint( self, 2, "Players In Game : %d\n", TeamFortress_GetNoPlayers(  ) );
	if ( number_of_teams > 0 )
	{
		G_sprint( self, 2, "Players In Team1: %d\n", TeamFortress_TeamGetNoPlayers( 1 ) );
	}
	if ( number_of_teams > 1 )
	{
		G_sprint( self, 2, "Players In Team2: %d\n", TeamFortress_TeamGetNoPlayers( 2 ) );
	}
	if ( number_of_teams > 2 )
	{
		G_sprint( self, 2, "Players In Team3: %d\n", TeamFortress_TeamGetNoPlayers( 3 ) );
	}
	if ( number_of_teams > 3 )
	{
		G_sprint( self, 2, "Players In Team4: %d\n", TeamFortress_TeamGetNoPlayers( 4 ) );
	}
}

void Admin_CycleKick(  )
{

	if ( TeamFortress_GetNoPlayers(  ) <= 1 )
	{
		G_sprint( self, 2, "No other players in the game.\n" );
		self->admin_use = world;
		self->admin_mode = 0;
		return;
	}
	if ( !self->admin_use )
		self->admin_use = world;
	for ( ; ( self->admin_use = trap_find( self->admin_use, FOFS( s.v.classname ), "player" ) ); )
	{
		if ( !self->admin_use->s.v.netname || self->admin_use == self )
			continue;
		break;
	}
	if ( self->admin_use )
	{
		self->admin_mode = 1;
		G_sprint( self, 2, "Kick %s?\n", self->admin_use->s.v.netname );
	} else
	{
		self->admin_mode = 0;
		G_sprint( self, 2, "End of player list\n" );
	}
}

void Admin_DoKick(  )
{
	G_bprint( 2, "%s was kicked by %s\n", self->admin_use->s.v.netname, self->s.v.netname );
	KickCheater( self->admin_use );
	self->admin_mode = 0;
	self->admin_use = world;
}

void CeaseFire_think(  )
{
	gedict_t *te;

	if ( g_globalvars.time > tf_data.cb_ceasefire_time && self->s.v.weapon == 1 && tf_data.cease_fire == 1 )
	{
		tf_data.cease_fire = 0;
		G_bprint( 2, "RESUME FIRE\n" );
		for ( te = world; (te = trap_find( te, FOFS( s.v.classname ), "player" )); )
		{
			G_centerprint( te, "RESUME FIRE\n" );
			te->tfstate = te->tfstate - ( te->tfstate & TFSTATE_CANT_MOVE );
			TeamFortress_SetSpeed( te );
		}
		dremove( self );
		return;
	}
	if ( !tf_data.cease_fire )
	{
		dremove( self );
		return;
	}
	for ( te = world; (te = trap_find( te, FOFS( s.v.classname ), "player" )); )
	{
		G_centerprint( te, "CEASE FIRE\n" );
	}
	self->s.v.nextthink = g_globalvars.time + 5;
}

void Admin_CeaseFire(  )
{
	gedict_t *te;

	if ( !tf_data.cease_fire )
	{
		tf_data.cease_fire = 1;
		G_bprint( 2, "CEASE FIRE\n" );
		for ( te = world; (te = trap_find( te, FOFS( s.v.classname ), "player" )); )
		{
			G_centerprint( te, "CEASE FIRE\n" );
			te->tfstate = te->tfstate | TFSTATE_CANT_MOVE;
			TeamFortress_SetSpeed( te );
		}
		te = spawn(  );
		te->s.v.classname = "ceasefire";
		te->s.v.think = ( func_t ) CeaseFire_think;
		te->s.v.nextthink = g_globalvars.time + 5;
	} else
	{
		te = trap_find( world, FOFS( s.v.classname ), "ceasefire" );
		if ( te )
			dremove( te );
		tf_data.cease_fire = 0;
		G_bprint( 2, "RESUME FIRE\n" );
		for ( te = world; (te = trap_find( te, FOFS( s.v.classname ), "player" )); )
		{

			G_centerprint( te, "RESUME FIRE\n" );
			te->tfstate = te->tfstate - ( te->tfstate & TFSTATE_CANT_MOVE );
			TeamFortress_SetSpeed( te );
		}
	}
}

void CheckAutoKick( gedict_t * p )
{
	float   rnum;
	gedict_t *te;

	if ( p->teamkills >= tf_data.autokick_kills && tf_data.autokick_kills )
	{
		G_bprint( 2, "%s was kicked for killing teammates.\n", p->s.v.netname );
		G_sprint( p, 2, "You were kicked for killing teammates.\n" );
		KickCheater( p );
	} else
	{
		if ( tf_data.autokick_kills )
		{
			if ( p->teamkills == tf_data.autokick_kills - 1 )
				G_sprint( p, 2, "Kill one more teammate, and you're outta here.\n" );
			rnum = 0;
			for ( te = world; (te = trap_find( te, FOFS( s.v.classname ), "ak_timer" )); )
			{
				if ( te->s.v.owner == EDICT_TO_PROG( p ) )
				{
					rnum = 1;
					break;
				}
			}
			if ( !rnum )
			{
				te = spawn(  );
				te->s.v.classname = "ak_timer";
				te->s.v.owner = EDICT_TO_PROG( p );
				te->s.v.think = ( func_t ) autokick_think;
				te->s.v.nextthink = g_globalvars.time + tf_data.autokick_time;
			}
		}
	}
}
