#include "g_local.h"
#ifdef TG
void    TG_LoadSettings(  );
#endif
void    CTF_FlagCheck(  );
void    BirthdayTimer(  );
void    CeaseFire_think(  );
void    PreMatch_Think(  );
void    player_pain( gedict_t * attacker, float take );
void    PlayerDie(  );

//===========================================================================
// Client
// 
//===========================================================================

vec3_t  VEC_ORIGIN = { 0, 0, 0 };
vec3_t  VEC_HULL_MIN = { -16, -16, -24 };
vec3_t  VEC_HULL_MAX = { 16, 16, 32 };

vec3_t  VEC_HULL2_MIN = { -32, -32, -24 };
vec3_t  VEC_HULL2_MAX = { 32, 32, 64 };
int     modelindex_eyes, modelindex_player, modelindex_null;
int     last_id;
float   rj;

/*
=============================================================================

    LEVEL CHANGING / INTERMISSION

=============================================================================
*/
float   intermission_running;
float   intermission_exittime;
char    nextmap[64] = "";

/*QUAKED info_intermission (1 0.5 0.5) (-16 -16 -16) (16 16 16)
This is the camera point for the intermission.
Use mangle instead of angle, so you can set pitch or roll as well as yaw.  'pitch roll yaw'
*/
void SP_info_intermission(  )
{
	if ( !CheckExistence(  ) )
	{
		dremove( self );
		return;
	}
// so C can get at it
	VectorCopy( self->mangle, self->s.v.angles );	//self.angles = self.mangle;      
}

void SetNewParms( void )
{
	g_globalvars.parm1 = 0;
	g_globalvars.parm2 = 100;
	g_globalvars.parm3 = 0;
	g_globalvars.parm4 = 25;
	g_globalvars.parm5 = 0;
	g_globalvars.parm6 = 0;
	g_globalvars.parm6 = 0;
	g_globalvars.parm8 = 1;
	g_globalvars.parm9 = 0;
	g_globalvars.parm10 = 0;
	g_globalvars.parm11 = 0;
	g_globalvars.parm12 = 0;
	g_globalvars.parm13 = 0;
	g_globalvars.parm14 = 0;
	g_globalvars.parm15 = 0;
};

void SetChangeParms(  )
{

	if ( self->s.v.health <= 0 )
	{
		SetNewParms(  );
		return;
	}
// remove items
	self->s.v.items -= ( ( int ) self->s.v.items &
			     ( IT_KEY1 | IT_KEY2 | IT_INVISIBILITY | IT_INVULNERABILITY | IT_SUIT | IT_QUAD ) );


// cap super health
	if ( self->s.v.health > 100 )
		self->s.v.health = 100;

	if ( self->s.v.health < 50 )
		self->s.v.health = 50;

	g_globalvars.parm1 = self->s.v.items;
	g_globalvars.parm2 = self->s.v.health;
	g_globalvars.parm3 = self->s.v.armorvalue;

	if ( self->s.v.ammo_shells < 25 )
		g_globalvars.parm4 = 25;
	else
		g_globalvars.parm4 = self->s.v.ammo_shells;

	g_globalvars.parm5 = self->s.v.ammo_nails;
	g_globalvars.parm6 = self->s.v.ammo_rockets;
	g_globalvars.parm7 = self->s.v.ammo_cells;
	g_globalvars.parm8 = self->current_weapon;
	g_globalvars.parm9 = self->s.v.armortype * 100;

	g_globalvars.parm10 = 0;
	g_globalvars.parm11 = 0;
	g_globalvars.parm12 = 0;
	g_globalvars.parm13 = self->StatusBarRes;
	g_globalvars.parm14 = self->StatusBarSize;
	g_globalvars.parm15 = self->is_admin;
};


void autoteam_think(  )
{
	tf_data.toggleflags |= TFLAG_AUTOTEAM;
	dremove( self );
};

void autokick_think(  )
{
	PROG_TO_EDICT( self->s.v.owner )->teamkills = 0;
	dremove( self );
};

void DecodeLevelParms(  )
{
	char    st[10];
	gedict_t *ent;
	gedict_t *te;

	// local "21?TeamFortress"
	// local "info_player_team1"
	// local "Mapname: "
	// local "temp1"
	float   autoteam_time;

	if ( g_globalvars.serverflags )
	{
		if ( streq( world->s.v.model, "maps/start.bsp" ) )
			SetNewParms(  );	// take away all stuff on starting new episode
	}
	self->s.v.items = g_globalvars.parm1;
	self->s.v.health = g_globalvars.parm2;
	self->s.v.armorvalue = g_globalvars.parm3;
	self->s.v.ammo_shells = g_globalvars.parm4;
	self->s.v.ammo_nails = g_globalvars.parm5;
	self->s.v.ammo_rockets = g_globalvars.parm6;
	self->s.v.ammo_cells = g_globalvars.parm7;
	self->current_weapon = g_globalvars.parm8;
	self->s.v.armortype = g_globalvars.parm9 * 0.01;

	if ( !( tf_data.toggleflags & TFLAG_FIRSTENTRY ) )
	{
		tf_data.toggleflags = g_globalvars.parm10;
		tf_data.flagem_checked = 0;
		tf_data.allow_hook = 0;
		tf_data.invis_only = 0;
		if ( coop || !deathmatch )
			tf_data.toggleflags |= TFLAG_CLASS_PERSIST;
		strncpy( nextmap, g_globalvars.mapname, 64 );
		tf_data.allow_hook = 1;

		ent = find( world, FOFS( s.v.classname ), "info_tfdetect" );
		if ( ent )
		{
			if ( !teamplay )
				trap_cvar_set( "teamplay", "21?TeamFortress" );

			ParseTFDetect( ent );

			if ( number_of_teams <= 0 || number_of_teams >= 5 )
				number_of_teams = 4;
		} else
		{
			ent = find( world, FOFS( s.v.classname ), "info_player_team1" );
			if ( ent || CTF_Map == 1 )
			{
				CTF_Map = 1;
				if ( !teamplay )
					trap_cvar_set( "teamplay", "21?TeamFortress" );
				ent = spawn(  );
				ent->s.v.nextthink = g_globalvars.time + 30;
				ent->s.v.think = ( func_t ) CTF_FlagCheck;
				number_of_teams = 2;
			} else
				number_of_teams = 4;
#ifdef LAN_SERVER
//   cvar_set("sv_aim", "1");
#endif
			teamlives[0] = -1;
			teamlives[1] = -1;
			teamlives[2] = -1;
			teamlives[3] = -1;
			teamlives[4] = -1;
			illegalclasses[0] = 0;
			illegalclasses[1] = 0;
			illegalclasses[2] = 0;
			illegalclasses[3] = 0;
			illegalclasses[4] = 0;
			teammaxplayers[0] = 100;
			teammaxplayers[1] = 100;
			teammaxplayers[2] = 100;
			teammaxplayers[3] = 100;
			teammaxplayers[4] = 100;

			civilianteams = 0;
		}
		G_bprint( 2, "Mapname: %s\n", g_globalvars.mapname );
		SetupTeamEqualiser(  );
		teamfrags[0] = 0;
		teamfrags[1] = 0;
		teamfrags[2] = 0;
		teamfrags[3] = 0;
		teamfrags[4] = 0;
		teamscores[0] = 0;
		teamscores[1] = 0;
		teamscores[2] = 0;
		teamscores[3] = 0;
		teamscores[4] = 0;

		tf_data.autokick_kills = 0;
		tf_data.autokick_time = 0;
		tf_data.cease_fire = 0;

		tf_data.toggleflags -= ( tf_data.toggleflags & TFLAG_TEAMFRAGS );

		tf_data.toggleflags -= ( tf_data.toggleflags & TFLAG_CHEATCHECK );

		tf_data.toggleflags |= 256 | GetSVInfokeyInt( "temp1", NULL, 0 );

		autoteam_time = 30;

		GetSVInfokeyString( "bd", "birthday", st, sizeof( st ), "off" );
		if ( !strcmp( st, "on" ) )
		{
			tf_data.birthday = 1;
			te = spawn(  );
			te->s.v.weapon = 10;
			te->s.v.nextthink = g_globalvars.time + 60;
			te->s.v.think = ( func_t ) BirthdayTimer;
		} else
		{
			tf_data.birthday = 0;
		}
		GetSVInfokeyString( "c", "clan", st, sizeof( st ), "off" );
		if ( !strcmp( st, "on" ) )
		{
			tf_data.clanbattle = 1;
			tf_data.clan_scores_dumped = 0;
			tf_data.game_locked = 0;
			tf_data.cb_prematch_time = g_globalvars.time + GetSVInfokeyFloat( "pm", "prematch", 0 ) * 60;
			if ( timelimit && ( ( g_globalvars.time + timelimit ) < tf_data.cb_prematch_time ) )
			{
				timelimit += tf_data.cb_prematch_time;
				sprintf( st, "%d", ceil( timelimit / 60 ) );
				trap_cvar_set( "timelimit", st );
			}
			if ( tf_data.cb_prematch_time > g_globalvars.time )
			{
				tf_data.cb_prematch_time += 5;
				ent = spawn(  );
				ent->s.v.think = ( func_t ) PreMatch_Think;
				ent->s.v.nextthink = g_globalvars.time + 5;
			}
			tf_data.cb_ceasefire_time = GetSVInfokeyFloat( "cft", "ceasefire_time", 0 );
			if ( tf_data.cb_ceasefire_time )
			{
				tf_data.cb_ceasefire_time = g_globalvars.time + tf_data.cb_ceasefire_time * 60;
				if ( tf_data.cb_prematch_time < tf_data.cb_ceasefire_time )
				{
					tf_data.cb_prematch_time = tf_data.cb_ceasefire_time + 5;
					if ( timelimit && g_globalvars.time + timelimit < tf_data.cb_prematch_time )
					{
						timelimit += tf_data.cb_ceasefire_time;
						sprintf( st, "%d", ceil( timelimit / 60 ) );
						trap_cvar_set( "timelimit", st );
					}
				}
				tf_data.cease_fire = 1;
				G_bprint( 2, "CEASE FIRE\n" );
				te = find( world, FOFS( s.v.classname ), "player" );
				while ( te )
				{
					CenterPrint( te, "CEASE FIRE\n" );
//     te.immune_to_check = g_globalvars.time + 10;
					te->tfstate = te->tfstate | TFSTATE_CANT_MOVE;
					TeamFortress_SetSpeed( te );
					te = find( te, FOFS( s.v.classname ), "player" );
				}
				te = spawn(  );
				te->s.v.classname = "ceasefire";
				te->s.v.think = ( func_t ) CeaseFire_think;
				te->s.v.nextthink = g_globalvars.time + 5;
				te->s.v.weapon = 1;
			}
			GetSVInfokeyString( "lg", "locked_game", st, sizeof( st ), "off" );
			if ( !strcmp( st, "on" ) )
				tf_data.game_locked = 1;
		} else
			tf_data.clanbattle = 0;

		GetSVInfokeyString( "a", "autoteam", st, sizeof( st ), "" );
		if ( !strcmp( st, "on" ) )
			tf_data.toggleflags |= TFLAG_AUTOTEAM;
		else
		{
			if ( !strcmp( st, "off" ) )
				tf_data.toggleflags -= ( tf_data.toggleflags & TFLAG_AUTOTEAM );
			else
			{
				if ( atoi( st ) )
				{
					tf_data.toggleflags |= TFLAG_AUTOTEAM;
					autoteam_time = atoi( st );
				}
			}
		}
		tf_data.autokick_time = GetSVInfokeyInt( "akt", "autokick_time", 0 );
		if ( tf_data.autokick_time )
		{
			tf_data.autokick_kills = GetSVInfokeyInt( "akk", "autokick_kills", 0 );
		}
		GetSVInfokeyString( "t", "teamfrags", st, sizeof( st ), "" );
		if ( !strcmp( st, "on" ) )
			tf_data.toggleflags |= TFLAG_TEAMFRAGS;
		else
		{
			if ( !strcmp( st, "off" ) )
				tf_data.toggleflags -= ( tf_data.toggleflags & TFLAG_TEAMFRAGS );
		}
		GetSVInfokeyString( "fts", "fullteamscore", st, sizeof( st ), "off" );
		if ( !strcmp( st, "on" ) )
			tf_data.toggleflags |= TFLAG_FULLTEAMSCORE;

		GetSVInfokeyString( "g", "grapple", st, sizeof( st ), "off" );
		if ( !strcmp( st, "off" ) )
			tf_data.allow_hook = 0;

		if ( !( tf_data.toggleflags & TFLAG_GRAPPLE ) && strcmp( st, "on" ) )
			tf_data.allow_hook = 0;

		if ( !GetSVInfokeyString( "og", "old_grenades", st, sizeof( st ), "on" ) )
			GetSVInfokeyString( "old_grens", NULL, st, sizeof( st ), "on" );

		if ( !strcmp( st, "on" ) )
			tf_data.old_grens = 1;

		GetSVInfokeyString( "spy", NULL, st, sizeof( st ), "on" );
		if ( !strcmp( st, "off" ) )
			tf_data.spy_off = 1;

		GetSVInfokeyString( "s", "spyinvis", st, sizeof( st ), "off" );

		if ( !strcmp( st, "on" ) || ( tf_data.toggleflags & TFLAG_SPYINVIS ) )
			tf_data.invis_only = 1;
		else
		{
			if ( !strcmp( st, "off" ) )
				tf_data.invis_only = 0;
		}


		tf_data.cheat_pause = GetSVInfokeyInt( "cp", NULL, 1 );
		if ( tf_data.cheat_pause <= 0 )
			tf_data.cheat_pause = 1;

////top colors
		tf_data.topcolor_check = 0;

		tf_data.tc1 = GetSVInfokeyInt( "tc1", NULL, -1 );
		if ( tf_data.tc1 >= 0 && tf_data.tc1 <= 15 )
			tf_data.topcolor_check = 1;
		else
			tf_data.tc1 = TeamFortress_TeamGetColor( 1 ) - 1;

		tf_data.tc2 = GetSVInfokeyInt( "tc2", NULL, -1 );
		if ( tf_data.tc2 >= 0 && tf_data.tc2 <= 15 )
			tf_data.topcolor_check = 1;
		else
			tf_data.tc2 = TeamFortress_TeamGetColor( 2 ) - 1;


		tf_data.tc3 = GetSVInfokeyInt( "tc3", NULL, -1 );
		if ( tf_data.tc3 >= 0 && tf_data.tc3 <= 15 )
			tf_data.topcolor_check = 1;
		else
			tf_data.tc3 = TeamFortress_TeamGetColor( 3 ) - 1;

		tf_data.tc4 = GetSVInfokeyInt( "tc4", NULL, -1 );
		if ( tf_data.tc4 >= 0 && tf_data.tc4 <= 15 )
			tf_data.topcolor_check = 1;
		else
			tf_data.tc4 = TeamFortress_TeamGetColor( 4 ) - 1;


		GetSVInfokeyString( "adg", "allow_drop_goal", st, sizeof( st ), "off" );
		if ( !strcmp( st, "on" ) )
			tf_data.allow_drop_goal = 1;
		else
			tf_data.allow_drop_goal = 0;

		GetSVInfokeyString( "add_pipe", NULL , st, sizeof( st ), "on" );
		if ( !strcmp( st, "on" ) )
			tf_data.add_pipe = 1;
		else
			tf_data.add_pipe = 0;

		GetSVInfokeyString( "nf", "new_flash", st, sizeof( st ), "on" );
		if ( !strcmp( st, "on" ) )
			tf_data.new_flash = 1;
		else
			tf_data.new_flash = 0;

		tf_data.new_gas = GetSVInfokeyInt( "new_gas", NULL,  GAS_DEFAULT );
		

		tf_data.sentry_type = SENTRY_NEW;

		GetSVInfokeyString( "sg", NULL, st, sizeof( st ), "new" );
		if ( !strcmp( st, "old" ) )
			tf_data.sentry_type = SENTRY_OLD;
		else if ( !strcmp( st, "fix" ) )
			tf_data.sentry_type = SENTRY_FIX;
		else if ( !strcmp( st, "oldmtfl" ) )
			tf_data.sentry_type = SENTRY_MTFL;
		else if ( !strcmp( st, "mtflf" ) )
			tf_data.sentry_type = SENTRY_MTFL_NEWFIND;
		else if ( !strcmp( st, "oldf" ) )
			tf_data.sentry_type = SENTRY_OLD_NEWFIND;
		else
			tf_data.sentry_type = SENTRY_NEW;


		tf_data.sgppl = GetSVInfokeyInt( "sgppl", NULL, 12 );
		if ( tf_data.sgppl < 0 )
			tf_data.sgppl = 0;

		tf_data.disable_grens = GetSVInfokeyInt( "dg", "disable_grens", 0 );

		GetSVInfokeyString( "dtpb", NULL, st, sizeof( st ), "on" );
		if ( !strcmp( st, "off" ) )
			tf_data.detpack_block = 0;
		else
			tf_data.detpack_block = 1;

		GetSVInfokeyString( "dp", "disable_powerups", st, sizeof( st ), "off" );
		if ( !strcmp( st, "on" ) )
			tf_data.disable_powerups = 1;

		GetSVInfokeyString( "ft", "flag_timer", st, sizeof( st ), "on" );
		if ( !strcmp( st, "on" ) )
			tf_data.flag_timer = 1;


		tf_data.snip_fps = GetSVInfokeyInt( "sf", "snip_fps", 72 );
		if ( tf_data.snip_fps < 0 )
			tf_data.snip_fps = 72;

		GetSVInfokeyString( "srf", "snip_range_fix", st, sizeof( st ), "off" );
		if ( !strcmp( st, "on" ) )
			tf_data.snip_range_fix = 1;

		tf_data.snip_ammo = GetSVInfokeyInt( "snip_ammo", NULL, 1 );
		if ( tf_data.snip_ammo < 0 )
			tf_data.snip_ammo = 1;

		tf_data.snip_time = GetSVInfokeyFloat( "snip_time", NULL, 1.5 );
		if ( tf_data.snip_time < 0 )
			tf_data.snip_time = 1.5;


		tf_data.gren2box = GetSVInfokeyInt( "g2b", NULL, 0 );
		if ( tf_data.gren2box < 0 )
			tf_data.gren2box = 0;


		GetSVInfokeyString( "rts", "random_team_spawn", st, sizeof( st ), "on" );
		if ( !strcmp( st, "on" ) )
			tf_data.random_tf_spawn = 1;
#ifdef TG
//  if(!defaults_loaded)
		TG_LoadSettings(  );
#endif

//////////////
		tf_data.respawn_delay_time = GetSVInfokeyFloat( "rd", "respawn_delay", 0 );

		if ( tf_data.respawn_delay_time )
			tf_data.toggleflags |= TFLAG_RESPAWNDELAY;
		if ( ( tf_data.toggleflags & TFLAG_RESPAWNDELAY ) && !tf_data.respawn_delay_time )
			tf_data.respawn_delay_time = 5;
		if ( tf_data.toggleflags & TFLAG_AUTOTEAM )
		{
			tf_data.toggleflags -= ( tf_data.toggleflags & TFLAG_AUTOTEAM );
			ent = spawn(  );
			ent->s.v.nextthink = g_globalvars.time + autoteam_time;
			ent->s.v.think = ( func_t ) autoteam_think;
		}

		GetSVInfokeyString( "mtfl", NULL, st, sizeof( st ), "off" );
		if ( !strcmp( st, "on" ) )
		{
			tf_data.mtfl = 1;
			tf_data.allow_hook = 0;
			tf_data.old_grens = 1;
			tf_data.invis_only = 0;
			tf_data.cheat_pause = 1;
			tf_data.topcolor_check = 1;
//			tf_data.tc1 = 13;
//			tf_data.tc2 = 4;
			tf_data.allow_drop_goal = 0;
			tf_data.add_pipe = 1;
			tf_data.new_flash = 1;
			tf_data.new_gas = GAS_DEFAULT;

			tf_data.sentry_type = SENTRY_NEW;
			tf_data.sgppl = 12;
			tf_data.disable_grens = 0;

			tf_data.detpack_block = 1;
			tf_data.disable_powerups = 0;
			tf_data.flag_timer = 1;
			tf_data.snip_fps = 72;
			tf_data.snip_range_fix = 0;
			tf_data.snip_ammo = 1;
			tf_data.snip_time = 1.5;
			tf_data.gren2box = 0;
			tf_data.random_tf_spawn = 1;
		} else
			tf_data.mtfl = 0;


	}
	if ( g_globalvars.parm11 )
		self->tfstate = g_globalvars.parm11;
	if ( !self->playerclass )
		self->playerclass = g_globalvars.parm12;
/* if (parm13) 
  self.StatusBarRes = parm13;
 if (parm14) 
  self.StatusBarSize = parm14;*/
	if ( g_globalvars.parm15 )
	{
		self->is_admin = g_globalvars.parm15;
	}
};

/*
============
FindIntermission

Returns the entity to view from
============
*/
gedict_t *FindIntermission(  )
{
	gedict_t *spot;
	int     cyc;

// look for info_intermission first
	spot = find( world, FOFS( s.v.classname ), "info_intermission" );
	if ( spot )
	{			// pick a random one
		cyc = g_random(  ) * 1;
/*		while ( cyc > 1 )
		{
			spot = find( spot, FOFS( s.v.classname ), "info_intermission" );
			if ( !spot )
				spot = find( world, FOFS( s.v.classname ), "info_intermission" );
			cyc = cyc - 1;
		}*/
		return spot;
	}
// then look for the start position
	spot = find( world, FOFS( s.v.classname ), "info_player_start" );
	if ( spot )
		return spot;
// look for deathmatch
	spot = find( world, FOFS( s.v.classname ), "info_player_deathmatch" );
	if ( spot )
	{			// pick a random one
		cyc = g_random(  ) * 7;
		while ( cyc > 1 )
		{
			spot = find( spot, FOFS( s.v.classname ), "info_player_deathmatch" );
			if ( !spot )
				spot = find( world, FOFS( s.v.classname ), "info_player_deathmatch" );
			cyc = cyc - 1;
		}
		return spot;
	}

	G_Error( "FindIntermission: no spot" );
	return NULL;
}


gedict_t *FindNextIntermission( gedict_t * start_point )
{
	gedict_t *spot;

//      float   cyc;

	if ( deathmatch )
	{
		if ( streq( start_point->s.v.classname, "info_intermission" ) || start_point == world || !start_point )
		{
			spot = find( start_point, FOFS( s.v.classname ), "info_intermission" );
			if ( spot )
				return spot;
			else
				start_point = world;
		}
		if ( streq( start_point->s.v.classname, "info_player_deathmatch" )
		     || start_point == world || !start_point )
		{
			spot = find( start_point, FOFS( s.v.classname ), "info_player_deathmatch" );
			if ( spot )
				return spot;
		}
		spot = find( world, FOFS( s.v.classname ), "info_intermission" );
		if ( spot )
			return spot;
		spot = find( world, FOFS( s.v.classname ), "info_player_deathmatch" );
		if ( spot )
			return spot;
	} else
	{
		spot = find( world, FOFS( s.v.classname ), "info_player_start" );
		if ( spot )
			return spot;
	}
	return FindIntermission(  );
};

void TF_MovePlayer(  )
{
	gedict_t *place;

	place = FindNextIntermission( self->observer_list );
	self->observer_list = place;
	setorigin( self, place->s.v.origin[0], place->s.v.origin[1], place->s.v.origin[2] + 1 );
	VectorCopy( place->s.v.angles, self->s.v.angles );
	self->s.v.fixangle = 1;
};

void GotoNextMap(  )
{
	int     nextlevel;
	char    str[64];

//      gedict_t *te;

	if ( strncmp( nextmap, g_globalvars.mapname, 64 ) )
	{
		trap_changelevel( nextmap );
		tf_data.already_chosen_map = 1;
	}
	if ( GetSVInfokeyString( g_globalvars.mapname, NULL, str, sizeof( str ), "" ) )
	{
		strncpy( nextmap, str, 64 );
		tf_data.already_chosen_map = 1;
		return;
	}

	if ( !tf_data.already_chosen_map )
	{
		nextlevel = GetSVInfokeyInt( "n", NULL, 0 );
		nextlevel++;


		localcmd( "serverinfo n %d\n", nextlevel );
		GetSVInfokeyString( "cd", "cycledir", str, sizeof( str ), "qwmcycle" );
		localcmd( "exec %s/map%d.cfg", str, nextlevel );
		tf_data.already_chosen_map = 1;
	}
	if ( GetSVInfokeyInt( "n", NULL, 0 ) == 0 )
		tf_data.already_chosen_map = 0;
};

void ExitIntermission(  )
{
	G_dprint( "Exiting intermission...\n" );
	if ( deathmatch )
	{
		G_dprint( "Exit Intermission in Deathmatch.\n" );
		GotoNextMap(  );
		return;
	}
	intermission_exittime = g_globalvars.time + 1;
	intermission_running += 1;
	if ( intermission_running == 2 )
	{
		if ( !strcmp( world->s.v.model, "maps/e1m7.bsp" ) )
		{
			trap_WriteByte( 2, 32 );
			trap_WriteByte( 2, 2 );
			trap_WriteByte( 2, 3 );
			if ( !trap_cvar( "registered" ) )
			{
				trap_WriteByte( 2, 31 );
				trap_WriteString( 2,
						  "As the corpse of the monstrous entity\nChthon sinks back into the lava whence\nit rose, you grip the Rune of Earth\nMagic tightly. Now that you have\nconquered the Dimension of the Doomed,\nrealm of Earth Magic, you are ready to\ncomplete your task in the other three\nhaunted lands of Quake. Or are you? If\nyou don't register Quake, you'll never\nknow what awaits you in the Realm of\nBlack Magic, the Netherworld, and the\nElder World!" );
			} else
			{
				trap_WriteByte( 2, 31 );
				trap_WriteString( 2,
						  "As the corpse of the monstrous entity\nChthon sinks back into the lava whence\nit rose, you grip the Rune of Earth\nMagic tightly. Now that you have\nconquered the Dimension of the Doomed,\nrealm of Earth Magic, you are ready to\ncomplete your task. A Rune of magic\npower lies at the end of each haunted\nland of Quake. Go forth, seek the\ntotality of the four Runes!" );
			}
			return;
		} else
		{
			if ( !strcmp( world->s.v.model, "maps/e2m6.bsp" ) )
			{
				trap_WriteByte( 2, 32 );
				trap_WriteByte( 2, 2 );
				trap_WriteByte( 2, 3 );
				trap_WriteByte( 2, 31 );
				trap_WriteString( 2,
						  "The Rune of Black Magic throbs evilly in\nyour hand and whispers dark thoughts\ninto your brain. You learn the inmost\nlore of the Hell-Mother; Shub-Niggurath!\nYou now know that she is behind all the\nterrible plotting which has led to so\nmuch death and horror. But she is not\ninviolate! Armed with this Rune, you\nrealize that once all four Runes are\ncombined, the gate to Shub-Niggurath's\nPit will open, and you can face the\nWitch-Goddess herself in her frightful\notherworld cathedral." );
				return;
			} else
			{
				if ( !strcmp( world->s.v.model, "maps/e3m6.bsp" ) )
				{
					trap_WriteByte( 2, 32 );
					trap_WriteByte( 2, 2 );
					trap_WriteByte( 2, 3 );
					trap_WriteByte( 2, 31 );
					trap_WriteString( 2,
							  "The charred viscera of diabolic horrors\nbubble viscously as you seize the Rune\nof Hell Magic. Its heat scorches your\nhand, and its terrible secrets blight\nyour mind. Gathering the shreds of your\ncourage, you shake the devil's shackles\nfrom your soul, and become ever more\nhard and determined to destroy the\nhideous creatures whose mere existence\nthreatens the souls and psyches of all\nthe population of Earth." );
					return;
				} else
				{
					if ( !strcmp( world->s.v.model, "maps/e4m7.bsp" ) )
					{
						trap_WriteByte( 2, 32 );
						trap_WriteByte( 2, 2 );
						trap_WriteByte( 2, 3 );
						trap_WriteByte( 2, 31 );
						trap_WriteString( 2,
								  "Despite the awful might of the Elder\nWorld, you have achieved the Rune of\nElder Magic, capstone of all types of\narcane wisdom. Beyond good and evil,\nbeyond life and death, the Rune\npulsates, heavy with import. Patient and\npotent, the Elder Being Shub-Niggurath\nweaves her dire plans to clear off all\nlife from the Earth, and bring her own\nfoul offspring to our world! For all the\ndwellers in these nightmare dimensions\nare her descendants! Once all Runes of\nmagic power are united, the energy\nbehind them will blast open the Gateway\nto Shub-Niggurath, and you can travel\nthere to foil the Hell-Mother's plots\nin person." );
						return;
					}
				}
			}
		}
		GotoNextMap(  );
	}
	if ( intermission_running == 3 )
	{
		if ( !trap_cvar( "registered" ) )
		{
			trap_WriteByte( 2, 33 );
			return;
		}
		if ( ( ( int ) ( g_globalvars.serverflags ) & 15 ) == 15 )
		{
			trap_WriteByte( 2, 31 );
			trap_WriteString( 2,
					  "Now, you have all four Runes. You sense\ntremendous invisible forces moving to\nunseal ancient barriers. Shub-Niggurath\nhad hoped to use the Runes Herself to\nclear off the Earth, but now instead,\nyou will use them to enter her home and\nconfront her as an avatar of avenging\nEarth-life. If you defeat her, you will\nbe remembered forever as the savior of\nthe planet. If she conquers, it will be\nas if you had never been born." );
			return;
		}
	}
	G_dprint( "Exit Intermission.\n" );
	GotoNextMap(  );
};

void IntermissionThink(  )
{
	int     minp;
	int     maxp;
	int     currp;
	char    sl[64], str[64];

	if ( g_globalvars.time < intermission_exittime )
		return;

	if ( !self->s.v.button0 && !self->s.v.button1 && !self->s.v.button2 )
		return;


	G_dprint( "Intermission think.\n" );
	GotoNextMap(  );

	if ( GetSVInfokeyString( "nmap", NULL, sl, sizeof( sl ), "" ) )
	{
		minp = GetSVInfokeyInt( "minp", NULL, 0 );
		maxp = GetSVInfokeyInt( "maxp", NULL, 0 );
		currp = TeamFortress_GetNoPlayers(  );
		G_dprint( "MAP CYCLING: Player Counting... " );
		if ( currp >= minp && currp <= maxp )
		{
			G_dprint( "within range. Changing maps...\n" );

			GetSVInfokeyString( "cd", "cycledir", str, sizeof( str ), "qwmcycle" );
			localcmd( "exec %s/%s.cfg", str, sl );
		} else
		{
			G_dprint( "outside range. Next map.\n" );
			tf_data.already_chosen_map = 0;
		}
		localcmd( "localinfo minp \"\"\n" );
		localcmd( "localinfo maxp \"\"\n" );
		localcmd( "localinfo nmap \"\"\n" );
	}
};

void execute_changelevel(  )
{
	gedict_t *pos;

	G_dprint( "execute_changelevel()\n" );

	intermission_running = 1;

// enforce a wait time before allowing changelevel
	intermission_exittime = g_globalvars.time + 5;

	pos = FindIntermission(  );

// play intermission music
	trap_WriteByte( MSG_ALL, SVC_CDTRACK );
	trap_WriteByte( MSG_ALL, 3 );

	trap_WriteByte( MSG_ALL, SVC_INTERMISSION );
	trap_WriteCoord( MSG_ALL, pos->s.v.origin[0] );
	trap_WriteCoord( MSG_ALL, pos->s.v.origin[1] );
	trap_WriteCoord( MSG_ALL, pos->s.v.origin[2] );
	trap_WriteAngle( MSG_ALL, pos->mangle[0] );
	trap_WriteAngle( MSG_ALL, pos->mangle[1] );
	trap_WriteAngle( MSG_ALL, pos->mangle[2] );
	other = find( world, FOFS( s.v.classname ), "player" );
	while ( other )
	{
		other->s.v.takedamage = DAMAGE_NO;
		other->s.v.solid = SOLID_NOT;
		other->s.v.movetype = MOVETYPE_NONE;
		other->s.v.modelindex = 0;
		other = find( other, FOFS( s.v.classname ), "player" );
	}

	if ( !tf_data.clan_scores_dumped )
	{
		DumpClanScores(  );
		tf_data.clan_scores_dumped = 1;
	}
};

void changelevel_touch(  )
{
//      gedict_t *pos;
//      float   ne;

	//gedict_t*    pos;

	if ( strneq( other->s.v.classname, "player" ) )
		return;

	if ( ( trap_cvar( "samelevel" ) == 2 )
	     || ( ( trap_cvar( "samelevel" ) == 3 ) && ( strneq( g_globalvars.mapname, "start" ) ) ) )
	{
		return;
	}

	G_bprint( PRINT_HIGH, "%s exited the level\n", other->s.v.netname );
	strcpy( nextmap, self->map );

	SUB_UseTargets(  );

	if ( ( ( int ) ( self->s.v.spawnflags ) & 1 ) && !deathmatch )
	{
		GotoNextMap(  );
		return;
	}
	self->s.v.touch = ( func_t ) SUB_Null;

// we can't move people right now, because touch functions are called
// in the middle of C movement code, so set a think g_globalvars.time to do it
	self->s.v.think = ( func_t ) execute_changelevel;
	self->s.v.nextthink = g_globalvars.time + 0.1;
};

/*QUAKED trigger_changelevel (0.5 0.5 0.5) ? NO_INTERMISSION
When the player touches this, he gets sent to the map listed in the "map" variable.  Unless the NO_INTERMISSION flag is set, the view will go to the info_intermission spot and display stats.
*/

void SP_trigger_changelevel(  )
{
	if ( !CheckExistence(  ) )
	{
		dremove( self );
		return;
	}
	if ( !self->map )
		G_Error( "chagnelevel trigger doesn't have map" );

	InitTrigger(  );
	self->s.v.touch = ( func_t ) changelevel_touch;
};

/*
=============================================================================

    PLAYER GAME EDGE FUNCTIONS

=============================================================================
*/

void    set_suicide_frame(  );

void respawn(  )
{
	if ( self->has_disconnected == 1 )
		return;
	if ( tf_data.cease_fire )
		return;
	if ( coop )
	{
		// make a copy of the dead body for appearances sake
		CopyToBodyQue( self );
		// set default spawn parms
		SetNewParms(  );
		// respawn              
		PutClientInServer(  );
	} else
	{
		if ( deathmatch )
		{
			// make a copy of the dead body for appearances sake
			CopyToBodyQue( self );
			// set default spawn parms
			SetNewParms(  );
			// respawn              
			PutClientInServer(  );
		} else
			localcmd( "restart\n" );
	}
};

/*
============
ClientKill

Player entered the suicide command
============
*/

void ClientKill(  )
{
	gedict_t *te;

	if ( self->has_disconnected == 1 )
		return;
	if ( self->suicide_time > g_globalvars.time )
		return;
	if ( self->s.v.deadflag )
		return;
	if ( !self->playerclass )
		return;
	self->suicide_time = g_globalvars.time + 5 + g_random(  ) * 5;
	G_bprint( PRINT_MEDIUM, "%s suicides\n", self->s.v.netname );

	set_suicide_frame(  );
	self->s.v.modelindex = modelindex_player;

	if ( self->tfstate & TFSTATE_INFECTED )
	{
		te = find( world, FOFS( s.v.classname ), "timer" );
		while ( te )
		{
			if ( te->s.v.owner == EDICT_TO_PROG( self ) && te->s.v.think == ( func_t ) BioInfection_Decay )
			{
				logfrag( te, self );
				TF_AddFrags( PROG_TO_EDICT( te->s.v.enemy ), 1 );
			}
			te = find( te, FOFS( s.v.classname ), "timer" );
		}
	} else
		logfrag( self, self );
	TF_AddFrags( self, -1 );
	self->s.v.weaponmodel = "";

	SetVector( self->s.v.view_ofs, 0, 0, -8 );
	self->s.v.movetype = MOVETYPE_NONE;
	TeamFortress_RemoveTimers(  );
	TeamFortress_SetupRespawn( 1 );
	self->s.v.health = -1;
	self->th_die(  );
	self->s.v.deadflag = 3;
	self->tfstate |= TFSTATE_RESPAWN_READY;
	self->s.v.takedamage = 0;
};

gedict_t *lastspawn_team[5] = { NULL, NULL, NULL, NULL, NULL };
char   *team_spawn_str[5] = { "", "ts1", "ts2", "ts3", "ts4" };

int CheckTelefragSpot( vec3_t v )
{
	gedict_t *at_spot;

	for ( at_spot = findradius( world, v, 40 ); at_spot; at_spot = findradius( at_spot, v, 40 ) )
	{
		if ( streq( at_spot->s.v.classname, "player" ) && !at_spot->s.v.deadflag
		     && at_spot->team_no == self->team_no && at_spot != self )
		{
			return 0;
		}
	}
	return 1;
}


gedict_t *FindTeamSpawnPoint( int team_num )
{
	gedict_t *spot;

// gedict_t* at_spot;
	int     spot_found;
	int     attempts;

	spot = lastspawn_team[team_num];
	if ( !spot )
		spot = world;
	for ( attempts = 0;; attempts++ )
	{
		spot = find( spot, FOFS( team_str_home ), team_spawn_str[team_num] );
		if ( !spot )
			spot = find( world, FOFS( team_str_home ), team_spawn_str[team_num] );
		if ( !spot )
			return world;
		spot_found = CheckTelefragSpot( spot->s.v.origin );

		if ( !Activated( spot, self ) )
			spot_found = 0;
		if ( spot_found || attempts >= 30 )
		{
			lastspawn_team[team_num] = spot;
			return spot;
		}
	}
	return world;
}

gedict_t *FindRandomTeamSpawnPoint( int team_num )
{
	gedict_t *spot;

	int     numspots = 0, numallspots = 0;
	int		rndspot;

	for(spot = world;spot = find( spot, FOFS( team_str_home ), team_spawn_str[team_num] );)
	{
     if(!Activated( spot, self )) 
		 continue;
	 numallspots++;
	 if(CheckTelefragSpot( spot->s.v.origin ))
		 numspots++;
	}
	if( !numallspots)
		return world;
	if(numspots)
	{
		rndspot = g_random() * numspots;
		for(spot = world;spot = find( spot, FOFS( team_str_home ), team_spawn_str[team_num] );)
		{

			if(!Activated( spot, self )) 
				continue;
			if(!CheckTelefragSpot( spot->s.v.origin ))
				continue;
			if(!rndspot)
				break;
			rndspot--;
		}
	}else // not found no telefrag spot
	{
		rndspot = g_random() * numallspots;
		for(spot = world;spot = find( spot, FOFS( team_str_home ), team_spawn_str[team_num] );)
		{

			if(!Activated( spot, self )) 
				continue;
			if(!rndspot)
				break;
			rndspot--;
		}
	}
	if(spot)
		return spot;
	
    G_dprint("FindRandomTeamSpawnPoint: error\n");

	return world;
}

gedict_t *lastspawn;

gedict_t *FindDeathmatchSpawnPoint(  )
{
	gedict_t *spot;

	int     spot_found;
	int     attempts;

	spot = lastspawn;
	if ( !spot )
		spot = world;
	for ( attempts = 0;; attempts++ )
	{
		spot = find( spot, FOFS( s.v.classname ), "info_player_deathmatch"  );
		if ( !spot )
			spot = find( world, FOFS( s.v.classname ), "info_player_deathmatch"  );
		if ( !spot )
			return world;

		spot_found = CheckTelefragSpot( spot->s.v.origin );

		if ( spot_found || attempts >= 10 )
		{
			lastspawn = spot;
			return spot;
		}
	}
	return world;
}


void ValidateUser( gedict_t * e )
{

};


gedict_t *SelectSpawnPoint(  )
{
	gedict_t *spot;

	if ( self->team_no )
	{
		if ( tf_data.random_tf_spawn )
			spot = FindRandomTeamSpawnPoint( self->team_no );
		else
			spot = FindTeamSpawnPoint( self->team_no );
		if ( spot != world )
			return spot;
	}
	if ( coop )
	{
		lastspawn = find( lastspawn, FOFS( s.v.classname ), "info_player_coop" );
		if ( !lastspawn )
			lastspawn = find( world, FOFS( s.v.classname ), "info_player_coop" );
		if ( lastspawn )
			return lastspawn;
	} else
	{
		if ( deathmatch )
		{
			spot = FindDeathmatchSpawnPoint();
			if ( spot != world )
				return spot;

/*			spot = find( lastspawn, FOFS( s.v.classname ), "info_player_deathmatch" );
			if ( !spot )
				spot = find( world, FOFS( s.v.classname ), "info_player_deathmatch" );
			for ( ; spot; spot = find( spot, FOFS( s.v.classname ), "info_player_deathmatch" ) )
			{
				if ( CheckTelefragSpot( spot->s.v.origin ) )
				{
					lastspawn = spot;
					return spot;
				}
			}*/
		}
	}
	if ( ( int ) ( g_globalvars.serverflags ) )
	{
		spot = find( world, FOFS( s.v.classname ), "info_player_start2" );
		if ( spot )
			return spot;
	}
	spot = find( world, FOFS( s.v.classname ), "info_player_start" );
	if ( !spot )
		G_Error( "PutClientInServer: no info_player_start on level\n" );
	return spot;
}


/*void () PlayerDie;
void () TeamFortress_SetHealth;
void () TeamFortress_SetEquipment;
void () player_touch;*/

////////////////
// GlobalParams:
// time
// self
// called after ClientConnect
///////////////

void PutClientInServer(  )
{
	int     iszoom;
	int     oldclass;
	gedict_t *spot;
	gedict_t *te;
	vec3_t  v;
	char    s[10];

	self->s.v.touch = ( func_t ) player_touch;
	self->s.v.classname = "player";
	self->s.v.health = 100;
	self->s.v.solid = SOLID_SLIDEBOX;
	self->s.v.movetype = MOVETYPE_WALK;
	self->show_hostile = 0;
	self->FlashTime = 0;
	self->s.v.max_health = 100;
	self->s.v.flags = FL_CLIENT;
	self->air_finished = g_globalvars.time + 12;
	self->dmg = 2;
	self->super_damage_finished = 0;
	self->radsuit_finished = 0;
	self->invisible_finished = 0;
	self->invincible_finished = 0;
	self->s.v.effects = 0;
	self->invincible_time = 0;
	self->reload_shotgun = 0;
	self->reload_super_shotgun = 0;
	self->reload_grenade_launcher = 0;
	self->reload_rocket_launcher = 0;
// self.immune_to_check = g_globalvars.time + 10;
	self->immune_to_check = g_globalvars.time + tf_data.cheat_pause;
	self->on_hook = 0;
	self->hook_out = 0;
	self->fire_held_down = 0;
	DecodeLevelParms(  );
	if ( !self->playerclass )
	{
		if ( TeamFortress_TeamIsCivilian( self->team_no ) )
		{
			self->s.v.impulse = 1;
			TeamFortress_ChangeClass(  );
		}
	}
	if ( deathmatch == 3 && self->nextpc )
	{
		self->playerclass = self->nextpc;
		self->nextpc = 0;
		if ( self->playerclass == PC_RANDOM )
			self->tfstate |= TFSTATE_RANDOMPC;
		else
		{
			self->tfstate -= ( self->tfstate & TFSTATE_RANDOMPC );
			TeamFortress_ExecClassScript( self );
		}
	}
	iszoom = 0;
	if ( self->tfstate & TFSTATE_ZOOMOFF )
		iszoom = 1;
	if ( self->tfstate & TFSTATE_RANDOMPC )
	{
		oldclass = self->playerclass;
		self->playerclass = 1 + (int)( g_random(  ) * ( 10 - 1 ) );
		while ( !IsLegalClass( self->playerclass )
			|| self->playerclass == oldclass || ClassIsRestricted( self->team_no, self->playerclass ) )
			self->playerclass = 1 + (int)( g_random(  ) * ( 10 - 1 ) );
		self->tfstate = TFSTATE_RANDOMPC;
		TeamFortress_ExecClassScript( self );
	} else
		self->tfstate = 0;

	if ( iszoom == 1 )
		self->tfstate |= TFSTATE_ZOOMOFF;
#ifndef TG
	if ( self->playerclass != PC_ENGINEER )
		Engineer_RemoveBuildings( self );
#endif
	self->s.v.takedamage = 2;
	TeamFortress_PrintClassName( self, self->playerclass, self->tfstate & 8 );
	TeamFortress_SetEquipment(  );
	TeamFortress_SetHealth(  );
	TeamFortress_SetSpeed( self );
	TeamFortress_SetSkin( self );
	stuffcmd( self, "v_idlescale 0\nfov 90\n" );
	stuffcmd( self, "v_cshift; wait; bf\n" );
	SetTeamName( self );
	W_SetCurrentAmmo(  );
	self->attack_finished = g_globalvars.time + 0.3;
	self->th_pain = player_pain;
	self->th_die = PlayerDie;
	if ( self->height )
	{
		self->height = 0;
		TF_zoom( 90 );
	}
	self->s.v.deadflag = 0;
	self->pausetime = 0;
	spot = SelectSpawnPoint(  );
	if ( self->playerclass )
		spawn_tdeath( spot->s.v.origin, self );
	self->observer_list = spot;
	VectorCopy( spot->s.v.origin, self->s.v.origin );
	self->s.v.origin[2]++;

	VectorCopy( spot->s.v.angles, self->s.v.angles );

	self->s.v.fixangle = 1;
	if ( streq( spot->s.v.classname, "info_player_teamspawn" ) && tf_data.cb_prematch_time < g_globalvars.time )
	{
		if ( spot->s.v.items )
		{
			te = Finditem( spot->s.v.items );
			if ( te != world )
				tfgoalitem_GiveToPlayer( te, self, self );
			if ( !( spot->goal_activation & TFSP_MULTIPLEITEMS ) )
				spot->s.v.items = 0;
		}
		if ( spot->s.v.message )
		{
			CenterPrint( self, spot->s.v.message );
			if ( !( spot->goal_activation & TFSP_MULTIPLEMSGS ) )
				spot->s.v.message = NULL;
		}
		if ( spot->activate_goal_no )
		{
			te = Findgoal( spot->activate_goal_no );
			if ( te )
				AttemptToActivate( te, self, spot );
		}
		if ( spot->goal_effects == TFSP_REMOVESELF )
		{
			spot->s.v.classname = "deadpoint";
			spot->team_str_home = "";
			spot->s.v.nextthink = g_globalvars.time + 1;
			spot->s.v.think = ( func_t ) SUB_Remove;
		}
	}
	setmodel( self, "" );
	modelindex_null = self->s.v.modelindex;
// oh, this is a hack!
	setmodel( self, "progs/eyes.mdl" );
	modelindex_eyes = self->s.v.modelindex;

	setmodel( self, "progs/player.mdl" );
	modelindex_player = self->s.v.modelindex;

	if ( !self->playerclass )
	{
		self->s.v.modelindex = modelindex_null;
		self->current_menu = MENU_DEFAULT;
	}
	setsize( self, PASSVEC3( VEC_HULL_MIN ), PASSVEC3( VEC_HULL_MAX ) );
	SetVector( self->s.v.view_ofs, 0, 0, 22 );
	SetVector( self->s.v.velocity, 0, 0, 0 );

	player_stand1(  );
	if ( deathmatch || coop )
	{
		makevectors( self->s.v.angles );
		if ( self->playerclass )
		{
			VectorScale( g_globalvars.v_forward, 20, v );
			VectorAdd( v, self->s.v.origin, v );
			spawn_tfog( v );
		}
	}
	infokey( world, "rj", s, sizeof( s ) );
	if ( atof( s ) != 0 )
	{
		rj = atof( s );
	} else
		rj = 1;

	if ( tf_data.cease_fire )
	{
		G_sprint( self, 2, "\n\nCEASE FIRE MODE\n" );
		self->tfstate |= TFSTATE_CANT_MOVE;
		TeamFortress_SetSpeed( self );
	}
}


void SP_Null_tf_spawn(  )
{
	if ( !CheckExistence(  ) )
	{
		dremove( self );
		return;
	}
};


void PrintClientScore( gedict_t * c )
{
	G_bprint( 1, "%3d %s\n", c->s.v.frags, c->s.v.netname );
};


void DumpScore(  )
{
///FIX ME !!!!!!!! not used???
/* gedict_t* e;
 gedict_t* sort;
 gedict_t* walk;

 if (world.chain) 
  error("DumpScore: world.chain is set");
 e = find(world, FOFS(s.v.classname), "player");
 sort = world;
 while (e) {
  if (!sort) {
   sort = e;
   e.chain = world;
  }
  else {
   if (e.frags > sort.frags) {
    e.chain = sort;
    sort = e;
   }
   else {
    walk = sort;
    do {
     if (!walk.chain) {
      e.chain = world;
      walk.chain = e;
     }
     else {
      if (walk.chain.frags < e.frags) {
       e.chain = walk.chain;
       walk.chain = e;
      }
      else 
       walk = walk.chain;
     }
    } while (walk.chain != e);
   }
  }
  e = find(e, FOFS(s.v.classname), "player");
 }
 bprint(1, "\n");
 while (sort) {
  PrintClientScore(sort);
  sort = sort.chain;
 }
 bprint(1, "\n");
 */
};

/*
===============================================================================

RULES

===============================================================================
*/
int     already_cycled;

/*
go to the next level 
*/

void NextLevel(  )
{
	gedict_t *o;

	if ( already_cycled )
		return;
	already_cycled = 1;
	o = spawn(  );
	o->map = nextmap;

	o->s.v.think = ( func_t ) execute_changelevel;
	o->s.v.nextthink = g_globalvars.time + 0.1;
};

/*
============
CheckRules

Exit games upon conditions
============
*/

void CheckRules(  )
{
	if ( timelimit && g_globalvars.time >= timelimit )
		NextLevel(  );

	if ( fraglimit && self->s.v.frags >= fraglimit )
		NextLevel(  );
};

//============================================================================

void PlayerDeathThink(  )
{
	//gedict_t* old_self;
	float   forward;

	if ( ( ( int ) ( self->s.v.flags ) ) & FL_ONGROUND )
	{
		forward = vlen( self->s.v.velocity );
		forward = forward - 20;
		if ( forward <= 0 )
			SetVector( self->s.v.velocity, 0, 0, 0 );
		else
		{
			VectorNormalize( self->s.v.velocity );
			VectorScale( self->s.v.velocity, forward, self->s.v.velocity );
		}
	}
// wait for all buttons released
	if ( self->s.v.deadflag == DEAD_DEAD )
	{
		if ( self->s.v.button2 || self->s.v.button1 || self->s.v.button0 )
			return;
		self->s.v.deadflag = DEAD_RESPAWNABLE;
		self->tfstate -= ( self->tfstate & TFSTATE_RESPAWN_READY );
		return;
	}
// wait for any button down
	if ( !self->s.v.button2 && !self->s.v.button1 && !self->s.v.button0 )
	{
		if ( self->tfstate & TFSTATE_RESPAWN_READY )
		{
			if ( self->respawn_time <= g_globalvars.time )
			{
				self->s.v.button0 = 0;
				self->s.v.button1 = 0;
				self->s.v.button2 = 0;
				respawn(  );
			}
		}
		return;
	} else
	{
		self->tfstate |= TFSTATE_RESPAWN_READY;
		if ( self->respawn_time <= g_globalvars.time )
		{
			self->s.v.button0 = 0;
			self->s.v.button1 = 0;
			self->s.v.button2 = 0;
			respawn(  );
		}
		return;
	}
};

void PlayerJump(  )
{
//      vec3_t  start;
//      vec3_t  end;

	if ( self->has_disconnected == 1 )
		return;
	if ( ( ( int ) ( self->s.v.flags ) ) & FL_WATERJUMP )
		return;

	if ( self->s.v.waterlevel >= 2 )
	{
		if ( self->s.v.watertype == -3 )
			self->s.v.velocity[2] = 100;
		else
		{
			if ( self->s.v.watertype == -4 )
				self->s.v.velocity[2] = 80;
			else
				self->s.v.velocity[2] = 50;
		}
		if ( self->swim_flag < g_globalvars.time )
		{
			self->swim_flag = g_globalvars.time + 1;
			if ( g_random(  ) < 0.5 )
				sound( self, 4, "misc/water1.wav", 1, 1 );
			else
				sound( self, 4, "misc/water2.wav", 1, 1 );
		}
/*  if (self.fire_held_down && self.current_weapon == 32768) {
   stuffcmd(self, "v_idlescale 0\n");
   self->tfstate = self->tfstate - (self->tfstate & 2048);
   TeamFortress_SetSpeed(self);
   self->s.v.weaponframe = 0;
   self.heat = 0;
   self.count = 1;
   player_assaultcannondown1();
  }*/

		return;
	}
	if ( !( ( int ) self->s.v.flags & FL_ONGROUND ) )
		return;
	if ( !( ( int ) self->s.v.flags & FL_JUMPRELEASED ) )
		return;
	self->s.v.flags = ( int ) self->s.v.flags - ( ( int ) self->s.v.flags & FL_JUMPRELEASED );
	self->s.v.button2 = 0;
	sound( self, 4, "player/plyrjmp8.wav", 1, 1 );
/* if (self.fire_held_down && self.current_weapon == 32768) {
  stuffcmd(self, "v_idlescale 0\n");
  sprint(self, 1, "You cannot fire the assault cannon without your feet on the ground...\n");
  self->tfstate = self->tfstate - (self->tfstate & 2048);
  TeamFortress_SetSpeed(self);
  self->s.v.weaponframe = 0;
  self.count = 1;
  self.heat = 0;
  player_assaultcannondown1();
 }*/

};



void WaterMove(  )
{
	if ( self->s.v.movetype == MOVETYPE_NOCLIP )
		return;

	if ( self->s.v.health < 0 )
		return;
	if ( self->s.v.waterlevel != 3 )
	{
		if ( self->air_finished < g_globalvars.time )
			sound( self, CHAN_VOICE, "player/gasp2.wav", 1, ATTN_NORM );
		else if ( self->air_finished < g_globalvars.time + 9 )
			sound( self, CHAN_VOICE, "player/gasp1.wav", 1, ATTN_NORM );

		self->air_finished = g_globalvars.time + 12;
		self->dmg = 2;

	} else if ( self->air_finished < g_globalvars.time )
	{			// drown!
		if ( self->pain_finished < g_globalvars.time )
		{
			self->dmg = self->dmg + 2;
			if ( self->dmg > 15 )
				self->dmg = 10;

			TF_T_Damage( self, world, world, self->dmg, 1, 0 );
			self->pain_finished = g_globalvars.time + 1;
		}
	}
	if ( !self->s.v.waterlevel )
	{
		if ( ( ( int ) ( self->s.v.flags ) ) & FL_INWATER )
		{
			// play leave water sound
			sound( self, CHAN_BODY, "misc/outwater.wav", 1, ATTN_NORM );
			self->s.v.flags -= FL_INWATER;
		}
		return;
	}

	if ( self->s.v.watertype == CONTENT_LAVA )
	{			// do damage
		if ( self->dmgtime < g_globalvars.time )
		{
			if ( self->radsuit_finished > g_globalvars.time )
				self->dmgtime = g_globalvars.time + 1;
			else
				self->dmgtime = g_globalvars.time + 0.2;

			TF_T_Damage( self, world, world, 10 * self->s.v.waterlevel, 0, 16 );
		}

	} else
	{
		if ( self->s.v.watertype == CONTENT_SLIME )
		{		// do damage
			if ( self->dmgtime < g_globalvars.time && self->radsuit_finished < g_globalvars.time )
			{
				self->dmgtime = g_globalvars.time + 1;
				T_Damage( self, world, world, 4 * self->s.v.waterlevel );
			}
		}
	}
	if ( !( ( ( int ) ( self->s.v.flags ) ) & FL_INWATER ) )
	{

// player enter water sound

		if ( self->s.v.watertype == CONTENT_LAVA )
			sound( self, CHAN_BODY, "player/inlava.wav", 1, ATTN_NORM );
		if ( self->s.v.watertype == CONTENT_WATER )
			sound( self, CHAN_BODY, "player/inh2o.wav", 1, ATTN_NORM );
		if ( self->s.v.watertype == CONTENT_SLIME )
			sound( self, CHAN_BODY, "player/slimbrn2.wav", 1, ATTN_NORM );

		self->s.v.flags += FL_INWATER;
		self->dmgtime = 0;
	}
};

void CheckWaterJump(  )
{
	vec3_t  start;
	vec3_t  end;

// check for a jump-out-of-water
	makevectors( self->s.v.angles );

	VectorCopy( self->s.v.origin, start );	//start = self->s.v.origin;
	start[2] = start[2] + 8;
	g_globalvars.v_forward[2] = 0;

	VectorNormalize( g_globalvars.v_forward );
	VectorScale( g_globalvars.v_forward, 24, end );
	VectorAdd( start, end, end );	//end = start + v_forward*24;

	traceline( PASSVEC3( start ), PASSVEC3( end ), true, self );

	if ( g_globalvars.trace_fraction < 1 )
	{			// solid at waist
		start[2] = start[2] + self->s.v.maxs[2] - 8;
		VectorScale( g_globalvars.v_forward, 24, end );
		VectorAdd( start, end, end );	//end = start + v_forward*24;
		VectorScale( g_globalvars.trace_plane_normal, -50, self->s.v.movedir );	//self->s.v.movedir = trace_plane_normal * -50;

		traceline( PASSVEC3( start ), PASSVEC3( end ), true, self );

		if ( g_globalvars.trace_fraction == 1 )
		{		// open at eye level
			self->s.v.flags = ( ( int ) ( self->s.v.flags ) ) | FL_WATERJUMP;
			self->s.v.velocity[2] = 225;
			self->s.v.flags -= ( ( ( int ) ( self->s.v.flags ) ) & FL_JUMPRELEASED );
			self->s.v.teleport_time = g_globalvars.time + 2;	// safety net
			return;
		}
	}
};

////////////////
// GlobalParams:
// time
// frametime
// self
///////////////
/*
================
PlayerPreThink

Called every frame before physics are run
================
*/
void PlayerPreThink(  )
{
//      float   mspeed;
//      float   aspeed;
//      float   r;
//      vec3_t  src;

	if ( self->is_feigning && self->s.v.waterlevel == 1 )
	{
		self->s.v.watertype = -3;
		self->s.v.waterlevel = 3;
	}
	if ( self->cheat_level > 0 )
		self->cheat_level -= 1;

	if ( intermission_running )
	{
		IntermissionThink(  );	// otherwise a button could be missed between
		return;		// the think tics
	}

	makevectors( self->s.v.v_angle );	// is this still used

	if ( !self->playerclass )
	{
		if ( self->s.v.button2 && self->current_menu == MENU_TEAM && !self->team_no )
		{
			if ( TeamFortress_TeamPutPlayerInTeam(  ) )
			{
				self->current_menu = MENU_CLASS;
				self->menu_count = MENU_REFRESH_RATE;
			}
		}
	}
	if ( self->s.v.view_ofs[0] == 0 && self->s.v.view_ofs[1] == 0 && self->s.v.view_ofs[2] == 0 )
		return;		// intermission or finale

	CheckRules(  );
	if ( self->playerclass )
		WaterMove(  );
	if ( self->s.v.deadflag >= DEAD_DEAD )
	{
		PlayerDeathThink(  );
		return;
	}

	if ( self->undercover_team || self->undercover_skin || self->is_undercover )
	{
		if ( ( int ) self->s.v.effects & ( 8 | 4 ) )
		{
			G_sprint( self, 1, "The glowing removes your disguise.\n" );
			Spy_RemoveDisguise( self );
		}
	}
	if ( self->s.v.deadflag == DEAD_DYING )
		return;
	if ( !self->is_feigning )
	{
		if ( self->s.v.button2 )
			PlayerJump(  );
		else
			self->s.v.flags = ( int ) self->s.v.flags | FL_JUMPRELEASED;
	} else
	{
		if ( self->s.v.waterlevel )
			self->s.v.velocity[2] = -100;
	}
	if ( g_globalvars.time < self->pausetime || tf_data.cease_fire == 1 )
		SetVector( self->s.v.velocity, 0, 0, 0 );
	if ( g_globalvars.time > self->attack_finished && !self->s.v.currentammo && self->s.v.weapon > 16 )
	{
		self->s.v.weapon = W_BestWeapon(  );
		W_SetCurrentAmmo(  );
	}
	if ( self->on_hook )
		Service_Grapple(  );
};

/////////////////////////////////////////////////////////////////
/*
================
CheckPowerups

Check for turning off powerups
================
*/
void CheckPowerups(  )
{
	float   lighton;
	gedict_t *te;

	if ( self->s.v.health <= 0 )
		return;

	if ( !self->playerclass )
		self->s.v.modelindex = modelindex_null;
	else
	{
		if ( self->is_undercover == 1 && tf_data.invis_only == 1 )
		{
			self->s.v.frame = 0;
			self->s.v.modelindex = modelindex_eyes;
		} else
		{
			if ( self->invisible_finished )
			{
				if ( self->tfstate & 64 )
				{
					if ( self->invisible_finished < g_globalvars.time + 10 )
						self->invisible_finished = g_globalvars.time + 666;
				}
				if ( self->invisible_sound < g_globalvars.time )
				{
					sound( self, 0, "items/inv3.wav", 0.5, 2 );
					self->invisible_sound = g_globalvars.time + g_random(  ) * 3 + 1;
				}
				if ( self->invisible_finished < g_globalvars.time + 3 )
				{
					if ( self->invisible_time == 1 )
					{
						G_sprint( self, 2, "Ring of Shadows magic is fading\n" );
						stuffcmd( self, "bf\n" );
						sound( self, 0, "items/inv2.wav", 1, 1 );
						self->invisible_time = g_globalvars.time + 1;
					}
					if ( self->invisible_time < g_globalvars.time )
					{
						self->invisible_time = g_globalvars.time + 1;
						stuffcmd( self, "bf\n" );
					}
				}
				if ( self->invisible_finished < g_globalvars.time )
				{
					self->s.v.items = self->s.v.items - 524288;
					self->invisible_finished = 0;
					self->invisible_time = 0;
				}
				self->s.v.frame = 0;
				self->s.v.modelindex = modelindex_eyes;
			} else
				self->s.v.modelindex = modelindex_player;
		}
	}
	if ( self->invincible_finished )
	{
		if ( self->tfstate & TFSTATE_INVINCIBLE )
		{
			if ( self->invincible_finished < g_globalvars.time + 10 )
				self->invincible_finished = g_globalvars.time + 666;
		}
		if ( self->invincible_finished < g_globalvars.time + 3 )
		{
			if ( self->invincible_time == 1 )
			{
				G_sprint( self, 2, "Protection is almost burned out\n" );
				stuffcmd( self, "bf\n" );
				sound( self, 0, "items/protect2.wav", 1, 1 );
				self->invincible_time = g_globalvars.time + 1;
			}
			if ( self->invincible_time < g_globalvars.time )
			{
				self->invincible_time = g_globalvars.time + 1;
				stuffcmd( self, "bf\n" );
			}
		}
		if ( self->invincible_finished < g_globalvars.time )
		{
			self->s.v.items = ( int ) self->s.v.items - IT_INVULNERABILITY;
			self->invincible_time = 0;
			self->invincible_finished = 0;
		}
		if ( self->invincible_finished > g_globalvars.time )
		{
			self->s.v.effects = ( int ) self->s.v.effects | 8;
			self->s.v.effects = ( int ) self->s.v.effects | 128;
		} else
		{
			lighton = 0;
			te = find( world, FOFS( s.v.classname ), "item_tfgoal" );
			while ( te )
			{
				if ( te->s.v.owner == EDICT_TO_PROG( self ) )
				{
					if ( te->goal_activation & 1 )
						lighton = 1;
				}
				te = find( te, FOFS( s.v.classname ), "item_tfgoal" );
			}
			if ( !lighton )
			{
				self->s.v.effects = ( int ) self->s.v.effects - ( ( int ) self->s.v.effects & 8 );
				self->s.v.effects = ( int ) self->s.v.effects - ( ( int ) self->s.v.effects & 128 );
			}
		}
	}
	if ( self->super_damage_finished )
	{
		if ( self->tfstate & TFSTATE_QUAD )
		{
			if ( self->super_damage_finished == g_globalvars.time + 10 )
				self->super_damage_finished = g_globalvars.time + 666;
		}
		if ( self->super_damage_finished < g_globalvars.time + 3 )
		{
			if ( self->super_time == 1 )
			{
				G_sprint( self, 2, "Quad Damage is wearing off\n" );
				stuffcmd( self, "bf\n" );
				sound( self, 0, "items/damage2.wav", 1, 1 );
				self->super_time = g_globalvars.time + 1;
			}
			if ( self->super_time < g_globalvars.time )
			{
				self->super_time = g_globalvars.time + 1;
				stuffcmd( self, "bf\n" );
			}
		}
		if ( self->super_damage_finished < g_globalvars.time )
		{
			self->s.v.items = ( int ) self->s.v.items - IT_QUAD;
			self->super_damage_finished = 0;
			self->super_time = 0;
		}
		if ( self->super_damage_finished > g_globalvars.time )
		{
			self->s.v.effects = ( int ) self->s.v.effects | 8;
			self->s.v.effects = ( int ) self->s.v.effects | 64;
		} else
		{
			lighton = 0;
			te = find( world, FOFS( s.v.classname ), "item_tfgoal" );
			while ( te )
			{
				if ( te->s.v.owner == EDICT_TO_PROG( self ) )
				{
					if ( te->goal_activation & 1 )
						lighton = 1;
				}
				te = find( te, FOFS( s.v.classname ), "item_tfgoal" );
			}
			if ( !lighton )
			{
				self->s.v.effects = ( int ) self->s.v.effects - ( ( int ) self->s.v.effects & 8 );
				self->s.v.effects = ( int ) self->s.v.effects - ( ( int ) self->s.v.effects & 64 );
			}
		}
	}
	if ( self->radsuit_finished )
	{
		self->air_finished = g_globalvars.time + 12;
		if ( self->tfstate & 256 )
		{
			if ( self->radsuit_finished == g_globalvars.time + 10 )
				self->radsuit_finished = g_globalvars.time + 666;
		}
		if ( self->radsuit_finished < g_globalvars.time + 3 )
		{
			if ( self->rad_time == 1 )
			{
				G_sprint( self, 2, "Air supply in Biosuit expiring\n" );
				stuffcmd( self, "bf\n" );
				sound( self, 0, "items/suit2.wav", 1, 1 );
				self->rad_time = g_globalvars.time + 1;
			}
			if ( self->rad_time < g_globalvars.time )
			{
				self->rad_time = g_globalvars.time + 1;
				stuffcmd( self, "bf\n" );
			}
		}
		if ( self->radsuit_finished < g_globalvars.time )
		{
			self->s.v.items = ( int ) self->s.v.items - IT_SUIT;
			self->rad_time = 0;
			self->radsuit_finished = 0;
		}
	}
};

////////////////
// GlobalParams:
// time
// self
///////////////
void PlayerPostThink(  )
{
//      float   mspeed;
//      float   aspeed;
//      float   r;
	float   fdmg;

	if ( self->s.v.view_ofs[0] == 0 && self->s.v.view_ofs[1] == 0 && self->s.v.view_ofs[2] == 0 )
		return;		// intermission or finale
	if ( self->s.v.deadflag )
	{
		DeadImpulses(  );
		self->s.v.impulse = 0;
		return;
	}
	if ( self->jump_flag < -300 && ( ( int ) self->s.v.flags & FL_ONGROUND ) && self->s.v.health > 0 )
	{
		if ( self->s.v.watertype == CONTENT_WATER )
			sound( self, 4, "player/h2ojump.wav", 1, 1 );
		else
		{
			if ( self->jump_flag < -650 )
			{
				fdmg = 5;
				fdmg = fdmg * self->jump_flag / 300 * -1;
				if ( self->playerclass == 1 )
					fdmg = fdmg / 2;
				else
				{
					if ( self->playerclass == 6 )
						fdmg = fdmg * 1.5;
				}
				fdmg = ( int ) fdmg;
				TF_T_Damage( self, world, world, fdmg, 1, 0 );
				sound( self, 2, "player/land2.wav", 1, 1 );
				self->deathtype = "falling";
			} else
				sound( self, 2, "player/land.wav", 1, 1 );
		}
	}
	self->jump_flag = self->s.v.velocity[2];
	CheckPowerups(  );
	W_WeaponFrame(  );
	if ( self->motd <= 95 )
		TeamFortress_MOTD(  );
	else
	{
		if ( !self->cheat_check )
			self->cheat_check = g_globalvars.time + 5;
		else
		{
			if ( g_globalvars.time > self->StatusRefreshTime )
			{
				if ( self->StatusBarSize )
					RefreshStatusBar( self );
			}
		}
	}
	if ( self->cheat_check <= g_globalvars.time )
	{
		TeamFortress_CheckTeamCheats(  );
		self->cheat_check = g_globalvars.time + 3;
	}
};

////////////////
// GlobalParams:
// time
// self
// params
///////////////

void ClientConnect(  )
{
	gedict_t *te;
	int     sbres;
	char    st[64];
	char    st2[64];

	int     got_one;

	G_bprint( PRINT_HIGH, "%s entered the game\n", self->s.v.netname );

	self->tfstate |= TFSTATE_ZOOMOFF;
	self->motd = 0;
	self->got_aliases = 0;
	if ( !self->s.v.netname[0] )
		KickCheater( self );

	sbres = GetInfokeyInt( self, "sbr", "sbar_res", 200 );
	switch ( sbres )
	{
	case 738:
		self->StatusBarRes = 8;
		break;
	case 600:
		self->StatusBarRes = 7;
		break;
	case 480:
		self->StatusBarRes = 6;
		break;
	case 400:
		self->StatusBarRes = 5;
		break;
	case 384:
		self->StatusBarRes = 4;
		break;
	case 350:
		self->StatusBarRes = 3;
		break;
	case 300:
		self->StatusBarRes = 2;
		break;
	case 240:
		self->StatusBarRes = 1;
		break;
	case 200:
		self->StatusBarRes = 0;
		break;
	}
	self->StatusBarSize = GetInfokeyInt( self, "sbs", "sbar_size", 0 );
	if ( self->StatusBarSize > 2 || self->StatusBarSize < 0 )
		self->StatusBarSize = 0;

	TeamFortress_ExecMapScript( self );
	self->has_disconnected = 0;

	if ( intermission_running )
		GotoNextMap(  );

	GetSVInfokeyString( "apw", "adminpwn", st2, sizeof( st2 ), "" );
	GetInfokeyString( self, "apw", "adminpwn", st, sizeof( st ), "" );

	if ( st2[0] && st[0] && !strcmp( st, st2 ) )
	{
		self->is_admin = 1;
		stuffcmd( self, "setinfo apw \"" );
		stuffcmd( self, "\"\n" );
		TeamFortress_Alias( "countplayers", 192, 0 );
		TeamFortress_Alias( "kick", 190, 0 );
		TeamFortress_Alias( "yes", 191, 0 );
		TeamFortress_Alias( "no", 195, 0 );
		TeamFortress_Alias( "ceasefire", 193, 0 );
	} else
		self->is_admin = 0;
	if ( tf_data.clanbattle && self->has_disconnected != 1 )
	{
		got_one = 0;

		self->tf_id = GetInfokeyInt( self, "tf_id", NULL, 0 );
		if ( self->tf_id )
		{
			G_sprint( self, 2, "Welcome back!\n" );
			te = find( world, FOFS( s.v.classname ), "ghost" );
			while ( te )
			{
				if ( te->tf_id == self->tf_id )
				{
					got_one = 1;
					TeamFortress_TeamSet( te->team_no );
					self->s.v.frags = te->s.v.frags;
					self->real_frags = te->real_frags;
					if ( !( tf_data.toggleflags & 128 ) && !( tf_data.toggleflags & 2048 ) )
						self->s.v.frags = self->real_frags;
					self->playerclass = te->playerclass;
					self->tfstate = te->tfstate;
					dremove( te );
					te = world;
				} else
					te = find( te, FOFS( s.v.classname ), "ghost" );
			}
		}
		if ( !got_one )
		{
			if ( tf_data.game_locked && tf_data.cb_prematch_time < g_globalvars.time )
			{
				G_sprint( self, 2, "Closed Server. Clan Battle in progress.\n" );
				KickCheater( self );
				return;
			}
			last_id = last_id + 20 + g_random(  ) * 10;
			self->tf_id = floor( g_random(  ) * 10 + last_id );
			stuffcmd( self, "setinfo tf_id %d\n", self->tf_id );
			G_sprint( self, 2, "Your Battle ID is %d\n", self->tf_id );
		}
	}
	if ( tf_data.cb_prematch_time > g_globalvars.time )
		G_sprint( self, 2, "CURRENTLY IN PREMATCH TIME\n" );
#ifdef TG
	stuffcmd( self, "exec tg.cfg\n" );
#endif
};

////////////////
// GlobalParams:
// self
///////////////


void ClientDisconnect(  )
{
	gedict_t *te;

//      float   fr;

	G_bprint( PRINT_HIGH, "%s left the game with %.0f frags\n", self->s.v.netname, self->s.v.frags );

	sound( self, CHAN_BODY, "player/tornoff2.wav", 1, ATTN_NONE );

	self->has_disconnected = 1;
	TeamFortress_RemoveTimers(  );
	Engineer_RemoveBuildings( self );
	te = find( world, FOFS( s.v.classname ), "detpack" );
	while ( te )
	{
#ifndef TG
		if ( te->s.v.owner == EDICT_TO_PROG( self ) )
		{
#else
		if ( te->real_owner == self )
		{
#endif
			if ( te->weaponmode == 1 )
			{
				TeamFortress_SetSpeed( PROG_TO_EDICT( te->s.v.enemy ) );
				dremove( te->oldenemy );
				dremove( te->observer_list );
			}
			dremove( te );
			te = world;
		}
		te = find( te, FOFS( s.v.classname ), "detpack" );
	}
	if ( tf_data.clanbattle && self->tf_id )
	{
		te = spawn(  );
		te->s.v.classname = "ghost";
		te->tf_id = self->tf_id;
		te->team_no = self->team_no;
		te->s.v.frags = self->s.v.frags;
		te->real_frags = self->real_frags;
		//te->netname = self->netname;
		te->playerclass = self->playerclass;
		if ( self->tfstate & 8 )
			te->tfstate = 8;
	}
	set_suicide_frame(  );
	//self->s.v.netname = "";
	self->team_no = 0;
	self->s.v.solid = 0;
	setsize( self, 0, 0, 0, 0, 0, 0 );
};

/*
===========
ClientObituary

called when a player dies
============
*/

//FIX ME
void ClientObituary( gedict_t * targ, gedict_t * attacker )
{
 /*float rnum;
 string deathstring;
 string deathstring2;
 float attackerteam;
 float targteam;
 gedict_t* te;

 rnum = g_random();
 if (cb_prematch_time + 3 > g_globalvars.time) 
  return;
 if (streq(targ->s.v.classname, "player")) {
  if (streq(attacker->s.v.classname, "teledeath")) {
   bprint(1, targ.netname);
   bprint(1, " was telefragged by ");
   bprint(1, attacker->s.v.owner.netname);
   bprint(1, "\n");
   if (attacker->s.v.owner.team_no != targ.team_no || attacker->s.v.owner.team_no < 1) 
    TF_AddFrags(attacker->s.v.owner, 1);
   return;
  }
  if (streq(attacker->s.v.classname, "teledeath2")) {
   bprint(1, "Satan's power deflects ");
   bprint(1, targ.netname);
   bprint(1, "'s telefrag\n");
   TF_AddFrags(targ, -1);
   logfrag(targ, targ);
   return;
  }
  if (deathmsg == 37) {
   bprint(1, targ.netname);
   bprint(1, " shoots his teammate one too many times.\n");
   return;
  }
  if (streq(attacker->s.v.classname, "info_tfgoal") || attacker->s.v.classname  == "item_tfgoal") {
   if (attacker->deathtype != "") {
    bprint(1, targ.netname);
    bprint(1, attacker->deathtype);
   }
   logfrag(targ, targ);
   return;
  }
  if (streq(attacker->s.v.classname, "player") || attacker->s.v.classname  == "bot") {
   if (targ == attacker) {
    TF_AddFrags(attacker, -1);
    if (birthday == 1 && random() < 0.3) {
     if (random() < 0.1) {
      bprint(1, "It's ");
      bprint(1, targ.netname);
      bprint(1, "'s party and he'll cry if he wants to!\n");
      return;
     }
     bprint(1, targ.netname);
     if (random() < 0.5) 
      bprint(1, " gets too selfish with his gifts\n");
     else 
      bprint(1, " wasn't born so beautiful after all\n");
     return;
    }
    bprint(1, targ.netname);
    if (deathmsg == 8) {
     if (targ->playerclass == 2) 
      deathstring = " got splattered by his own grenade\n";
     else {
      if (targ->playerclass == 3) 
       deathstring = " sat on his own grenade\n";
      else {
       if (targ->playerclass == 4) 
        deathstring = " got to know his grenade too well\n";
       else {
        if (targ->playerclass == 5) 
         deathstring = " caught the end of his own grenade\n";
        else {
         if (targ->playerclass == 6) 
          deathstring = " got too close to his own grenade\n";
         else {
          if (targ->playerclass == 7) 
           deathstring = " let his own grenade get the best of him\n";
          else {
           if (targ->playerclass == 8) 
            deathstring = " tiptoed over his own grenade\n";
           else {
            if (targ->playerclass == 9) 
             deathstring = " stared at his grenade too long\n";
            else 
             deathstring = " grenades himself\n";
           }
          }
         }
        }
       }
      }
     }
    }
    else {
     if (deathmsg == 9) 
      deathstring = " hammers himself\n";
     else {
      if (deathmsg == 10) {
       if (targ->playerclass == 4) 
        deathstring = " practiced his own Mirv dance\n";
       else {
        if (targ->playerclass == 6) 
         deathstring = " allowed his Mirv to turn against him\n";
        else 
         deathstring = " goes to pieces\n";
       }
      }
      else {
       if (deathmsg == 11) 
        deathstring = " ambushes himself with his own pipebombs\n";
       else {
        if (deathmsg == 40) 
         deathstring = " tried to juggle his own pipebombs\n";
        else {
         if (deathmsg == 24) 
          deathstring = " chokes on his own gas\n";
         else {
          if (deathmsg == 30) 
           deathstring = " explodes his ammo and body\n";
          else {
           if (deathmsg == 41) 
            deathstring = " stepped on too many of his own caltrops\n";
           else {
            if (deathmsg == 35) 
             deathstring = " is charred by his own flash grenade\n";
            else {
             if (deathmsg == 31) 
              deathstring = " detonates an ammo box too close to him\n";
             else {
              if (deathmsg == 12) 
               deathstring = " set the detpack and forgot to run\n";
              else {
               if (deathmsg == 13) 
                deathstring = " died impossibly!\n";
               else {
                if (deathmsg == 6) {
                 if (rnum) 
                  deathstring = " becomes bored with life\n";
                 else 
                  deathstring = " checks if his weapon is loaded\n";
                }
                else {
                 if (deathmsg == 33) 
                  deathstring = " chars himself with an incendiary rocket\n";
                 else {
                  if (deathmsg == 5) 
                   deathstring = " tries to put the pin back in\n";
                  else {
                   if (deathmsg == 15) 
                    deathstring = " torches himself\n";
                   else {
                    if (deathmsg == 7 && targ->s.v.waterlevel > 1) {
                     bprint(1, " discharges into the water.\n");
                     return;
                    }
                    else {
                     if (deathmsg == 38) 
                      deathstring = " gets too friendly with his sentrygun.\n";
                     else {
                      if (deathmsg == 39) 
                       deathstring = " dispenses with himself.\n";
                     }
                    }
                   }
                  }
                 }
                }
               }
              }
             }
            }
           }
          }
         }
        }
       }
      }
     }
    }
    bprint(1, deathstring);
    return;
   }
   else {
    if (teamplay && attacker.team_no == targ.team_no && attacker.team_no > 0) {
     TF_AddFrags(attacker, -1);
     if (!targ.undercover_team) 
      attacker.teamkills = attacker.teamkills + 1;
     if (birthday == 1 && random() < 0.3) {
      if (random() < 0.3) 
       bprint2(1, targ.netname, " is a party-pooper!\n");
      else 
       bprint2(1, targ.netname, " gives gifts to his teammates!\n");
      return;
     }
     deathstring2 = "\n";
     if (rnum < 0.25) {
      deathstring = " mows down teammate ";
      deathstring2 = "\n";
     }
     else {
      if (rnum < 0.5) {
       deathstring = " checks his glasses after killing ";
       deathstring2 = "\n";
      }
      else {
       if (rnum < 0.75) {
        deathstring = " gets a frag for the other team with ";
        deathstring2 = "'s death\n";
       }
       else {
        deathstring = " killed his supposed friend ";
        deathstring2 = "\n";
       }
      }
     }
     if (deathmsg == 23) {
      bprint(1, targ.netname);
      bprint(1, " didn't survive the operation by ");
      bprint(1, attacker.netname);
      bprint(1, "\n");
      return;
     }
     bprint(1, attacker.netname);
     bprint(1, deathstring);
     bprint(1, targ.netname);
     bprint(1, deathstring2);
     CheckAutoKick(attacker);
     return;
    }
    else {
     TF_AddFrags(attacker, 1);
     logfrag(attacker, targ);
     if (birthday == 1 && random() < 0.5) {
      if (deathmsg == 9) {
       deathstring = " chews on ";
       deathstring2 = "'s nails!\n";
      }
      else {
       if (deathmsg == 10) {
        deathstring = " gathers the darling buds of ";
        deathstring2 = "'s Mirv grenade\n";
       }
       else {
        if (deathmsg == 30) {
         deathstring = "'s presents go up in ";
         deathstring2 = "'s EMP presence.\n";
        }
        else {
         if (deathmsg == 35) {
          bprint(1, targ.netname);
          bprint(1, " goes POP!\n");
          return;
         }
         else {
          if (deathmsg == 31) {
           deathstring = " stands to near ";
           deathstring2 = "'s birthday surprise\n";
          }
          else {
           if (deathmsg == 12) {
            deathstring = " eats ";
            deathstring2 = "'s good cheer!\n";
            if (random() < 0.1) 
             bprint(1, "Damn that jokey smurf!\n");
           }
           else {
            if (deathmsg == 16) {
             deathstring = " cut the red ribbon of ";
             deathstring2 = "'s detpack\n";
            }
            else {
             if (deathmsg == 5) {
              deathstring = " recieves a gift from ";
              deathstring2 = "\n";
              if (targ->s.v.health < -40) {
               deathstring = " plays pass-the-parcel with ";
               deathstring2 = "\n";
              }
             }
             else {
              if (deathmsg == 6) {
               deathstring = " rides ";
               deathstring2 = "'s firecracker\n";
               if (targ->s.v.health < -40) {
                bprint(1, targ.netname);
                bprint(1, " gets turned into little ");
                bprint(1, targ.netname);
                bprint(1, "'s by ");
                bprint(1, attacker.netname);
                bprint(1, deathstring2);
                return;
               }
              }
              else {
               if (deathmsg == 15) {
                if (rnum < 0.5) {
                 deathstring = " gets too close to ";
                 deathstring2 = "'s kitchen\n";
                }
                else {
                 deathstring = " plays with ";
                 deathstring2 = "'s fire\n";
                }
               }
               else {
                if (deathmsg == 18) {
                 if (rnum <= 0.3) {
                  deathstring = " blocks ";
                  deathstring2 = "'s birthday bullet with his chest\n";
                 }
                 else {
                  deathstring = " gets party popped by ";
                  deathstring2 = "\n";
                 }
                }
                else {
                 if (deathmsg == 29) {
                  if (rnum <= 0.5) {
                   deathstring = " ALMOST catches ";
                   deathstring2 = "'s bullet between his teeth!\n";
                  }
                  else {
                   deathstring = " loves snipers like ";
                   deathstring2 = "\n";
                  }
                 }
                 else {
                  if (deathmsg == 28) {
                   deathstring = " won't run crying to ";
                   deathstring2 = " anymore\n";
                  }
                  else {
                   if (deathmsg == 19) {
                    deathstring = " collects ";
                    deathstring2 = "'s highspeed gifts.\n";
                   }
                   else {
                    if (deathmsg == 20) {
                     deathstring = " died. I blame ";
                     deathstring2 = "\n";
                    }
                    else {
                     if (deathmsg == 22) {
                      deathstring = " gets a gift in the back from ";
                      deathstring2 = "\n";
                     }
                     else {
                      if (deathmsg == 2) {
                       deathstring = " gets a double of ";
                       deathstring2 = "'s buck\n";
                      }
                      else {
                       if (deathmsg == 25) {
                        deathstring = " is all partied out by ";
                        deathstring2 = "\n";
                       }
                       else {
                        if (deathmsg == 26) {
                         deathstring = " gets derailed by ";
                         deathstring2 = "\n";
                        }
                        else {
                         if (deathmsg == 3) {
                          deathstring = " gets no say in it, no say in it at all! sings ";
                          deathstring2 = "\n";
                         }
                         else {
                          if (deathmsg == 33) {
                           deathstring = " gets all fired up by ";
                           deathstring2 = "\n";
                          }
                          else {
                           if (random() < 0.5) 
                            deathstring = " recieves a gift from ";
                           else 
                            deathstring = " has a happy birthday, thanks to ";
                           deathstring2 = "\n";
                          }
                         }
                        }
                       }
                      }
                     }
                    }
                   }
                  }
                 }
                }
               }
              }
             }
            }
           }
          }
         }
        }
       }
      }
      bprint(1, targ.netname);
      bprint(1, deathstring);
      bprint(1, attacker.netname);
      bprint(1, deathstring2);
      return;
     }
     if (deathmsg == 8) {
      if (attacker->playerclass == 2) {
       deathstring = " got up-close and personal with ";
       deathstring2 = "'s grenade\n";
      }
      else {
       if (attacker->playerclass == 3) {
        deathstring = " played catch with ";
        deathstring2 = "'s grenade\n";
       }
       else {
        if (attacker->playerclass == 4) {
         deathstring = " received a pineapple enema from ";
         deathstring2 = "\n";
        }
        else {
         if (attacker->playerclass == 5) {
          deathstring = " fetched ";
          deathstring2 = "'s pineapple\n";
         }
         else {
          if (attacker->playerclass == 6) {
           deathstring = " caught too much shrapnel from ";
           deathstring2 = "'s grenade\n";
          }
          else {
           if (attacker->playerclass == 7) {
            deathstring = " tried to pick up ";
            deathstring2 = "'s hot potato\n";
           }
           else {
            if (attacker->playerclass == 8) {
             deathstring = " thought ";
             deathstring2 = " was tossing him a spare grenade\n";
            }
            else {
             if (attacker->playerclass == 9) {
              deathstring = " stops to ponder the technical details of ";
              deathstring2 = "'s grenade\n";
             }
             else {
              deathstring = " surfs on a grenade from ";
              deathstring2 = "\n";
             }
            }
           }
          }
         }
        }
       }
      }
     }
     else {
      if (deathmsg == 9) {
       deathstring = " gets flayed by ";
       deathstring2 = "'s nail grenade\n";
      }
      else {
       if (deathmsg == 10) {
        if (attacker->playerclass == 4) {
         deathstring = " does a dance on ";
         deathstring2 = "'s Mirv grenade\n";
        }
        else {
         deathstring = " gets spammed by ";
         deathstring2 = "'s Mirv grenade\n";
        }
       }
       else {
        if (deathmsg == 11) {
         deathstring = " is caught by ";
         deathstring2 = "'s pipebomb trap\n";
        }
        else {
         if (deathmsg == 40) {
          deathstring = " fell victim to ";
          deathstring2 = "'s fireworks\n";
         }
         else {
          if (deathmsg == 24) {
           deathstring = " gags on ";
           deathstring2 = "'s noxious gasses\n";
          }
          else {
           if (deathmsg == 30) {
            deathstring = "'s ammo detonates him as ";
            deathstring2 = "'s EMP fries it\n";
           }
           else {
            if (deathmsg == 41) {
             deathstring = " stepped on too many of ";
             deathstring2 = "'s caltrops\n";
            }
            else {
             if (deathmsg == 35) {
              deathstring = " is charred by ";
              deathstring2 = "'s flash grenade\n";
             }
             else {
              if (deathmsg == 31) {
               deathstring = " stands near some ammo as ";
               deathstring2 = "'s EMP nukes it\n";
              }
              else {
               if (deathmsg == 12) {
                deathstring = " reaches orbit via ";
                deathstring2 = "'s detpack\n";
               }
               else {
                if (deathmsg == 16) {
                 deathstring = " cut the red wire of ";
                 deathstring2 = "'s detpack\n";
                }
                else {
                 if (deathmsg == 13) {
                  deathstring = " dies from ";
                  deathstring2 = "'s mysterious tropical disease\n";
                 }
                 else {
                  if (deathmsg == 14) {
                   deathstring = " escapes infection from ";
                   deathstring2 = " by dying first\n";
                  }
                  else {
                   if (deathmsg == 5) {
                    deathstring = " eats ";
                    deathstring2 = "'s pineapple\n";
                    if (targ->s.v.health < -40) {
                     deathstring = " was gibbed by ";
                     deathstring2 = "'s grenade\n";
                    }
                   }
                   else {
                    if (deathmsg == 6) {
                     deathstring = " rides ";
                     deathstring2 = "'s rocket\n";
                     if (targ->s.v.health < -40) {
                      deathstring = " was gibbed by ";
                      deathstring2 = "'s rocket\n";
                     }
                    }
                    else {
                     if (deathmsg == 15) {
                      if (rnum < 0.2) {
                       deathstring = " is burnt up by ";
                       deathstring2 = "'s flame\n";
                      }
                      else {
                       if (rnum < 0.4) {
                        deathstring = " is fried by ";
                        deathstring2 = "'s fire\n";
                       }
                       else {
                        if (rnum < 0.6) {
                         deathstring = " feels ";
                         deathstring2 = "'s fire of wrath\n";
                        }
                        else {
                         if (rnum < 0.8) {
                          deathstring = " is reduced to ashes by ";
                          deathstring2 = "\n";
                         }
                         else {
                          deathstring = " is grilled by ";
                          deathstring2 = "'s flame\n";
                         }
                        }
                       }
                      }
                     }
                     else {
                      if (deathmsg == 17) {
                       deathstring2 = "\n";
                       if (attacker->playerclass == 8) 
                        deathstring = " was knife-murdered by ";
                       else {
                        if (attacker->playerclass == 1) 
                         deathstring = "'s mellon was split by ";
                        else {
                         if (attacker->playerclass == 2) 
                          deathstring = " was put on the chop block by ";
                         else {
                          if (attacker->playerclass == 3) {
                           deathstring = " was sliced and diced by ";
                           deathstring2 = "'s blade\n";
                          }
                          else {
                           if (attacker->playerclass == 4) {
                            deathstring = " is split from crotch to sternum by ";
                            deathstring2 = "'s axe swing\n";
                           }
                           else {
                            if (attacker->playerclass == 6) 
                             deathstring = " is split in two with a powerful axe blow from ";
                            else {
                             if (attacker->playerclass == 7) {
                              deathstring = "'s death put another notch on ";
                              deathstring2 = "'s axe\n";
                             }
                             else 
                              deathstring = " was ax-murdered by ";
                            }
                           }
                          }
                         }
                        }
                       }
                      }
                      else {
                       if (deathmsg == 32) {
                        deathstring = " was spanner-murdered by ";
                        deathstring2 = "\n";
                       }
                       else {
                        if (deathmsg == 1) {
                         if (attacker->playerclass == 1) {
                          deathstring = " got too close to ";
                          deathstring2 = "'s muzzleflash\n";
                         }
                         else {
                          if (attacker->playerclass == 3) {
                           deathstring = " practices being ";
                           deathstring2 = "'s clay pigeon\n";
                          }
                          else {
                           if (attacker->playerclass == 4) {
                            deathstring = " was on the receiving end of ";
                            deathstring2 = "'s shotgun barrel\n";
                           }
                           else {
                            if (attacker->playerclass == 5) {
                             deathstring = " was fed a lead diet by ";
                             deathstring2 = "\n";
                            }
                            else {
                             if (attacker->playerclass == 6) {
                              deathstring = " got blasted by ";
                              deathstring2 = "'s last resort\n";
                             }
                             else {
                              if (attacker->playerclass == 7) {
                               deathstring = " got more than a powderburn from ";
                               deathstring2 = "'s shotgun blast\n";
                              }
                              else {
                               deathstring = " chewed on ";
                               deathstring2 = "'s boomstick\n";
                              }
                             }
                            }
                           }
                          }
                         }
                        }
                        else {
                         if (deathmsg == 2) {
                          if (attacker->playerclass == 3) {
                           deathstring = " was turned into swiss cheese by ";
                           deathstring2 = "'s buckshot\n";
                          }
                          else {
                           if (attacker->playerclass == 5) {
                            deathstring = " got a double-dose of ";
                            deathstring2 = "'s buckshot\n";
                           }
                           else {
                            if (attacker->playerclass == 6) {
                             deathstring = " unfortunately forgot ";
                             deathstring2 = " carried a super-shotgun\n";
                            }
                            else {
                             if (attacker->playerclass == 8) {
                              deathstring = " gets ventilated by ";
                              deathstring2 = "'s super-shotgun blast\n";
                             }
                             else {
                              if (attacker->playerclass == 9) {
                               deathstring = "'s body got chuck full of ";
                               deathstring2 = "'s lead pellets\n";
                              }
                              else {
                               deathstring = " ate 2 loads of ";
                               deathstring2 = "'s buckshot\n";
                              }
                             }
                            }
                           }
                          }
                         }
                         else {
                          if (deathmsg == 3) {
                           if (attacker->playerclass == 1) {
                            deathstring = " caught one too many nails from ";
                            deathstring2 = "\n";
                           }
                           else {
                            if (attacker->playerclass == 2) {
                             deathstring = " ran into ";
                             deathstring2 = "'s nails\n";
                            }
                            else {
                             if (attacker->playerclass == 8) {
                              deathstring = " was turned into ";
                              deathstring2 = "'s pin-cushion\n";
                             }
                             else {
                              deathstring = " was nailed by ";
                              deathstring2 = "\n";
                             }
                            }
                           }
                          }
                          else {
                           if (deathmsg == 4) {
                            deathstring = " was punctured by ";
                            deathstring2 = "\n";
                           }
                           else {
                            if (deathmsg == 7) {
                             deathstring = " accepts ";
                             if (attacker->s.v.waterlevel > 1) 
                              deathstring2 = "'s discharge\n";
                             else 
                              deathstring2 = "'s shaft\n";
                            }
                            else {
                             if (deathmsg == 21) {
                              deathstring = " grappled with ";
                              deathstring2 = "\n";
                             }
                             else {
                              if (deathmsg == 18) {
                               if (rnum <= 0.3) {
                                deathstring = " takes a bullet in the chest from ";
                                deathstring2 = "\n";
                               }
                               else {
                                deathstring = " succumbs to sniperfire from ";
                                deathstring2 = "\n";
                               }
                              }
                              else {
                               if (deathmsg == 29) {
                                if (rnum <= 0.5) {
                                 deathstring = " gets a third eye from ";
                                 deathstring2 = "\n";
                                }
                                else {
                                 deathstring = " gets his head blown off by ";
                                 deathstring2 = "\n";
                                }
                               }
                               else {
                                if (deathmsg == 28) {
                                 if (rnum <= 0.5) {
                                  deathstring = " is made legless by ";
                                  deathstring2 = "\n";
                                 }
                                 else {
                                  deathstring = " gets his legs blown off by ";
                                  deathstring2 = "\n";
                                 }
                                }
                                else {
                                 if (deathmsg == 19) {
                                  deathstring = " collects ";
                                  deathstring2 = "'s bullet spray.\n";
                                 }
                                 else {
                                  if (deathmsg == 20) {
                                   deathstring = " gets sawn in half by ";
                                   deathstring2 = "\n";
                                  }
                                  else {
                                   if (deathmsg == 22) {
                                    deathstring = " gets knifed from behind by ";
                                    deathstring2 = "\n";
                                   }
                                   else {
                                    if (deathmsg == 25) {
                                     deathstring = " is put to sleep by ";
                                     deathstring2 = "\n";
                                    }
                                    else {
                                     if (deathmsg == 26) {
                                      deathstring = " gets a hole in his heart from ";
                                      deathstring2 = "'s railgun\n";
                                     }
                                     else {
                                      if (deathmsg == 33) {
                                       deathstring = " gets well done by ";
                                       deathstring2 = "'s incendiary rocket\n";
                                      }
                                      else {
                                       if (deathmsg == 38) {
                                        deathstring = " gets destroyed by ";
                                        deathstring2 = "'s exploding sentrygun\n";
                                       }
                                       else {
                                        if (deathmsg == 39) {
                                         deathstring = " didn't insert the correct change into ";
                                         deathstring2 = "'s dispenser.\n";
                                        }
                                       }
                                      }
                                     }
                                    }
                                   }
                                  }
                                 }
                                }
                               }
                              }
                             }
                            }
                           }
                          }
                         }
                        }
                       }
                      }
                     }
                    }
                   }
                  }
                 }
                }
               }
              }
             }
            }
           }
          }
         }
        }
       }
      }
     }
     bprint(1, targ.netname);
     bprint(1, deathstring);
     bprint(1, attacker.netname);
     bprint(1, deathstring2);
    }
   }
   return;
  }
  else {
   if (streq(attacker->s.v.classname, "building_sentrygun")) {
    if (targ == attacker.real_owner) {
     if (deathmsg == 34) 
      deathstring = " intercepts his sentry gun's rocket\n";
     else {
      if (deathmsg == 27) 
       deathstring = " crossed his sentry gun's line of fire\n";
     }
     bprint(1, targ.netname);
     bprint(1, deathstring);
    }
    else {
     if (teamplay && attacker.team_no == targ.team_no && attacker.team_no > 0) {
      bprint(1, targ.netname);
      bprint(1, " obstructs his team's sentry gun\n");
     }
     else {
      TF_AddFrags(attacker.real_owner, 1);
      logfrag(attacker.real_owner, targ);
      if (deathmsg == 34) {
       deathstring = " hates ";
       deathstring2 = "'s sentry gun\n";
      }
      else {
       if (deathmsg == 27) {
        deathstring = " is mown down by ";
        deathstring2 = "'s sentry gun\n";
       }
      }
      bprint(1, targ.netname);
      bprint(1, deathstring);
      bprint(1, attacker.real_owner.netname);
      bprint(1, deathstring2);
     }
    }
   }
   else {
    logfrag(targ, targ);
    TF_AddFrags(targ, -1);
    rnum = targ.watertype;
    bprint(2, targ.netname);
    if (rnum == -3) {
     if (birthday == 1) {
      if (random() < 0.5) 
       deathstring = " bobs for apples\n";
      else 
       deathstring = " drowns in the punch\n";
     }
     else {
      if (random() < 0.5) 
       deathstring = " sleeps with the fishes\n";
      else 
       deathstring = " sucks it down\n";
     }
    }
    else {
     if (rnum == -4) {
      if (random() < 0.5) 
       deathstring = " gulped a load of slime\n";
      else 
       deathstring = " can't exist on slime alone\n";
     }
     else {
      if (rnum == -5) {
       if (targ->s.v.health < -15) 
        deathstring = " burst into flames\n";
       else {
        if (random() < 0.5) 
         deathstring = " turned into hot slag\n";
        else 
         deathstring = " visits the Volcano God\n";
       }
      }
      else {
       if (streq(attacker->s.v.classname, "explo_box")) 
        deathstring = " blew up\n";
       else {
        if (attacker->s.v.solid  == 4 && attacker != world) 
         deathstring = " was squished\n";
        else {
         if (targ->deathtype == "falling") {
          targ->deathtype = string_null;
          deathstring = " fell to his death\n";
         }
         else {
          if (streq(attacker->s.v.classname, "trap_shooter") || attacker->s.v.classname  == "trap_spikeshooter") 
           deathstring = " was spiked\n";
          else {
           if (streq(attacker->s.v.classname, "fireball")) 
            deathstring = " ate a lavaball\n";
           else {
            if (streq(attacker->s.v.classname, "trigger_changelevel")) 
             deathstring = " tried to leave\n";
            else 
             deathstring = " died\n";
           }
          }
         }
        }
       }
      }
     }
    }
    bprint(1, deathstring);
   }
  }
 }
 else {
  if (streq(targ->s.v.classname, "building_sentrygun")) {
   if (streq(attacker->s.v.classname, "teledeath")) {
    bprint(1, targ.real_owner.netname);
    bprint(1, "'s sentrygun was telefragged by ");
    bprint(1, attacker->s.v.owner.netname);
    bprint(1, "\n");
    return;
   }
   if (streq(attacker->s.v.classname, "player")) {
    if (attacker == targ.real_owner) {
     bprint(1, targ.real_owner.netname);
     bprint(1, " destroys his sentrygun\n");
     return;
    }
    bprint(1, targ.real_owner.netname);
    bprint(1, "'s sentrygun was destroyed by ");
    bprint(1, attacker.netname);
    bprint(1, "\n");
    if (attacker.team_no > 0 && attacker.team_no == targ.real_owner.team_no) {
     TF_AddFrags(attacker, -1);
     attacker.teamkills = attacker.teamkills + 1;
     CheckAutoKick(attacker);
    }
    else 
     TF_AddFrags(attacker, 1);
    return;
   }
  }
  else {
   if (streq(targ->s.v.classname, "building_dispenser")) {
    if (attacker.team_no > 0 && attacker.team_no == targ.real_owner.team_no) {
     TF_AddFrags(attacker, -1);
     attacker.teamkills = attacker.teamkills + 1;
     CheckAutoKick(attacker);
    }
   }
  }
 }*/
};
