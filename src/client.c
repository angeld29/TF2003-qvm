#include "g_local.h"

void    CTF_FlagCheck();
void    BirthdayTimer();
void    CeaseFire_think();
void    PreMatch_Think();
void    player_pain( gedict_t * attacker, float take );
void    PlayerDie();

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
void SP_info_intermission()
{
	if ( !CheckExistence() )
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
}

void SetChangeParms()
{

	if ( self->s.v.health <= 0 )
	{
		SetNewParms();
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
}


void autoteam_think()
{
	tf_data.toggleflags |= TFLAG_AUTOTEAM;
	dremove( self );
}

void autokick_think()
{
	PROG_TO_EDICT( self->s.v.owner )->teamkills = 0;
	dremove( self );
}
extern int     team_top_colors[5];
void DecodeLevelParms()
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
			SetNewParms();	// take away all stuff on starting new episode
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
				ent = spawn();
				ent->s.v.nextthink = g_globalvars.time + 30;
				ent->s.v.think = ( func_t ) CTF_FlagCheck;
				number_of_teams = 2;
			} else
				number_of_teams = 4;

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
		SetupTeamEqualiser();
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
			te = spawn();
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
				ent = spawn();
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
				te = spawn();
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

		team_top_colors[1] = GetSVInfokeyInt( "tc1", NULL, -1 );
		if ( team_top_colors[1] >= 0 && team_top_colors[1] <= 15 )
			tf_data.topcolor_check = 1;
		else
			team_top_colors[1] = TeamFortress_TeamGetColor( 1 ) - 1;

		team_top_colors[2] = GetSVInfokeyInt( "tc2", NULL, -1 );
		if ( team_top_colors[2] >= 0 && team_top_colors[2] <= 15 )
			tf_data.topcolor_check = 1;
		else
			team_top_colors[2] = TeamFortress_TeamGetColor( 2 ) - 1;


		team_top_colors[3] = GetSVInfokeyInt( "tc3", NULL, -1 );
		if ( team_top_colors[3] >= 0 && team_top_colors[3] <= 15 )
			tf_data.topcolor_check = 1;
		else
			team_top_colors[3] = TeamFortress_TeamGetColor( 3 ) - 1;

		team_top_colors[4] = GetSVInfokeyInt( "tc4", NULL, -1 );
		if ( team_top_colors[4] >= 0 && team_top_colors[4] <= 15 )
			tf_data.topcolor_check = 1;
		else
			team_top_colors[4] = TeamFortress_TeamGetColor( 4 ) - 1;


		GetSVInfokeyString( "adg", "allow_drop_goal", st, sizeof( st ), "off" );
		if ( !strcmp( st, "on" ) )
			tf_data.allow_drop_goal = 1;
		else
			tf_data.allow_drop_goal = 0;

		GetSVInfokeyString( "add_pipe", NULL, st, sizeof( st ), "on" );
		if ( !strcmp( st, "on" ) )
			tf_data.add_pipe = 1;
		else
			tf_data.add_pipe = 0;

		GetSVInfokeyString( "nf", "new_flash", st, sizeof( st ), "on" );
		if ( !strcmp( st, "on" ) )
			tf_data.new_flash = 1;
		else
			tf_data.new_flash = 0;

		tf_data.new_gas = GetSVInfokeyInt( "new_gas", NULL, GAS_DEFAULT );


		
		tf_data.sg_newfind = true;
		tf_data.sg_sfire   = SG_SFIRE_NEW;

		GetSVInfokeyString( "sg", NULL, st, sizeof( st ), "new" );
		if ( !strcmp( st, "old" ) )
		{
			tf_data.sg_newfind = false;
			tf_data.sg_sfire   = SG_SFIRE_281;
		}
		if ( !strcmp( st, "fix" ) )
		{
			tf_data.sg_newfind = false;
			tf_data.sg_sfire   = SG_SFIRE_MTFL2;
		}

		if ( !strcmp( st, "oldmtfl" ) )
		{
			tf_data.sg_newfind = false;
			tf_data.sg_sfire   = SG_SFIRE_MTFL1;
		}
		if ( !strcmp( st, "mtflf" ) )
		{
			tf_data.sg_newfind = true;
			tf_data.sg_sfire   = SG_SFIRE_MTFL1;
		}

		if ( !strcmp( st, "oldf" ) )
		{
			tf_data.sg_newfind = true;
			tf_data.sg_sfire   = SG_SFIRE_281;
		}

		GetSVInfokeyString( "sg_newfind", NULL, st, sizeof( st ), "on" );
		if ( !strcmp( st, "off" ) )
			tf_data.sg_newfind = false;
		else
			tf_data.sg_newfind = true;


		GetSVInfokeyString( "sg_sfire", NULL, st, sizeof( st ), "new" );

		if( !strcmp(st, "old"))
			tf_data.sg_sfire = SG_SFIRE_281;

		if( !strcmp(st, "mtfl1"))
			tf_data.sg_sfire = SG_SFIRE_MTFL1;

		if( !strcmp(st, "mtfl2"))
			tf_data.sg_sfire = SG_SFIRE_MTFL2;
			

		GetSVInfokeyString( "sg_rfire", NULL, st, sizeof( st ), "old" );

		if( !strcmp(st, "new"))
			tf_data.sg_rfire = true;
		else
			tf_data.sg_rfire = false;

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

		GetSVInfokeyString( "lan", NULL, st, sizeof( st ), "off" );
		if ( !strcmp( st, "on" ) )
			tf_data.lan_mode = 1;


		memset( &tg_data, 0, sizeof(tg_data));
		GetSVInfokeyString( "tg", "training_ground", st, sizeof( st ), "off" );
		if ( !strcmp( st, "on" ) )
			tg_data.tg_enabled = 1;
//////////////
		tf_data.respawn_delay_time = GetSVInfokeyFloat( "rd", "respawn_delay", 0 );

		if ( tf_data.respawn_delay_time )
			tf_data.toggleflags |= TFLAG_RESPAWNDELAY;
		if ( ( tf_data.toggleflags & TFLAG_RESPAWNDELAY ) && !tf_data.respawn_delay_time )
			tf_data.respawn_delay_time = 5;
		if ( tf_data.toggleflags & TFLAG_AUTOTEAM )
		{
			tf_data.toggleflags -= ( tf_data.toggleflags & TFLAG_AUTOTEAM );
			ent = spawn();
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
//                      team_top_colors[1] = 13;
//                      team_top_colors[2] = 4;
			tf_data.allow_drop_goal = 0;
			tf_data.add_pipe = 1;
			tf_data.new_flash = 1;
			tf_data.new_gas = GAS_DEFAULT;

			tf_data.sg_newfind = true;
			tf_data.sg_sfire = SG_SFIRE_NEW; //FIX ME
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
			tf_data.lan_mode = 0;
			tg_data.tg_enabled = 0;
		} else
			tf_data.mtfl = 0;

//SETUP LAN CONSTANTS
		if( !tf_data.lan_mode ) 
		{
			FLAME_MAXWORLDNUM	=20;	 	// maximum number of flames in the world. DO NOT PUT BELOW 20.
			MAX_WORLD_PIPEBOMBS	=15;	 	// This is divided between teams - this is the most you should have on a net server
			MAX_WORLD_AMMOBOXES	=20;		// This is divided between teams - this is the most you should have on a net server
			GR_TYPE_MIRV_NO		=4;	 	// Number of Mirvs a Mirv Grenade breaks into
			GR_TYPE_NAPALM_NO	=8; 	 	// Number of flames napalm grenade breaks into (unused if net server)
		}else
		{
			FLAME_MAXWORLDNUM	=60;	 	// maximum number of flames in the world. DO NOT PUT BELOW 20.
			MAX_WORLD_PIPEBOMBS	=30;	 	// This is divided between teams - this is the most you should have on a net server
			MAX_WORLD_AMMOBOXES	=20;		// This is divided between teams - this is the most you should have on a net server
			GR_TYPE_MIRV_NO		=12;	 	// Number of Mirvs a Mirv Grenade breaks into
			GR_TYPE_NAPALM_NO	=12; 	 	// Number of flames napalm grenade breaks into (unused if net server)
		}
                if( tg_data.tg_enabled )
			TG_LoadSettings();
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
}

/*
============
FindIntermission

Returns the entity to view from
============
*/
gedict_t *FindIntermission()
{
	gedict_t *spot;
	int     cyc;

// look for info_intermission first
	spot = find( world, FOFS( s.v.classname ), "info_intermission" );
	if ( spot )
	{			// pick a random one
		cyc = g_random() * 1;
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
		cyc = g_random() * 7;
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
	return FindIntermission();
}

void TF_MovePlayer()
{
	gedict_t *place;

	place = FindNextIntermission( self->observer_list );
	self->observer_list = place;
	setorigin( self, place->s.v.origin[0], place->s.v.origin[1], place->s.v.origin[2] + 1 );
	VectorCopy( place->s.v.angles, self->s.v.angles );
	self->s.v.fixangle = 1;
}

void GotoNextMap()
{
	int     nextlevel;
	char    str[64];

//      gedict_t *te;

	if ( Q_strncmp( nextmap, g_globalvars.mapname, 64 ) )
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
}

void ExitIntermission()
{
	G_dprint( "Exiting intermission...\n" );
	if ( deathmatch )
	{
		G_dprint( "Exit Intermission in Deathmatch.\n" );
		GotoNextMap();
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
		GotoNextMap();
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
	GotoNextMap();
}

void IntermissionThink()
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
	GotoNextMap();

	if ( GetSVInfokeyString( "nmap", NULL, sl, sizeof( sl ), "" ) )
	{
		minp = GetSVInfokeyInt( "minp", NULL, 0 );
		maxp = GetSVInfokeyInt( "maxp", NULL, 0 );
		currp = TeamFortress_GetNoPlayers();
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
}

void execute_changelevel()
{
	gedict_t *pos;

	G_dprint( "execute_changelevel()\n" );

	intermission_running = 1;

// enforce a wait time before allowing changelevel
	intermission_exittime = g_globalvars.time + 5;

	pos = FindIntermission();

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
		DumpClanScores();
		tf_data.clan_scores_dumped = 1;
	}
}

void changelevel_touch()
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

	SUB_UseTargets();

	if ( ( ( int ) ( self->s.v.spawnflags ) & 1 ) && !deathmatch )
	{
		GotoNextMap();
		return;
	}
	self->s.v.touch = ( func_t ) SUB_Null;

// we can't move people right now, because touch functions are called
// in the middle of C movement code, so set a think g_globalvars.time to do it
	self->s.v.think = ( func_t ) execute_changelevel;
	self->s.v.nextthink = g_globalvars.time + 0.1;
}

/*QUAKED trigger_changelevel (0.5 0.5 0.5) ? NO_INTERMISSION
When the player touches this, he gets sent to the map listed in the "map" variable.  Unless the NO_INTERMISSION flag is set, the view will go to the info_intermission spot and display stats.
*/

void SP_trigger_changelevel()
{
	if ( !CheckExistence() )
	{
		dremove( self );
		return;
	}
	if ( !self->map )
		G_Error( "chagnelevel trigger doesn't have map" );

	InitTrigger();
	self->s.v.touch = ( func_t ) changelevel_touch;
}

/*
=============================================================================

    PLAYER GAME EDGE FUNCTIONS

=============================================================================
*/

void    set_suicide_frame();

void respawn()
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
		SetNewParms();
		// respawn              
		PutClientInServer();
	} else
	{
		if ( deathmatch )
		{
			// make a copy of the dead body for appearances sake
			CopyToBodyQue( self );
			// set default spawn parms
			SetNewParms();
			// respawn              
			PutClientInServer();
		} else
			localcmd( "restart\n" );
	}
}

/*
============
ClientKill

Player entered the suicide command
============
*/

void ClientKill()
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
	self->suicide_time = g_globalvars.time + 5 + g_random() * 5;
	G_bprint( PRINT_MEDIUM, "%s suicides\n", self->s.v.netname );

//	set_suicide_frame();
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
	TeamFortress_RemoveTimers();
	TeamFortress_SetupRespawn( 1 );
	self->s.v.health = -1;
	self->th_die();
	self->s.v.deadflag = 3;
	self->tfstate |= TFSTATE_RESPAWN_READY;
	self->s.v.takedamage = 0;
}

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
	int     rndspot;

	for ( spot = world; spot = find( spot, FOFS( team_str_home ), team_spawn_str[team_num] ); )
	{
		if ( !Activated( spot, self ) )
			continue;
		numallspots++;
		if ( CheckTelefragSpot( spot->s.v.origin ) )
			numspots++;
	}
	if ( !numallspots )
		return world;
	if ( numspots )
	{
		rndspot = g_random() * numspots;
		for ( spot = world; spot = find( spot, FOFS( team_str_home ), team_spawn_str[team_num] ); )
		{

			if ( !Activated( spot, self ) )
				continue;
			if ( !CheckTelefragSpot( spot->s.v.origin ) )
				continue;
			if ( !rndspot )
				break;
			rndspot--;
		}
	} else			// not found no telefrag spot
	{
		rndspot = g_random() * numallspots;
		for ( spot = world; spot = find( spot, FOFS( team_str_home ), team_spawn_str[team_num] ); )
		{

			if ( !Activated( spot, self ) )
				continue;
			if ( !rndspot )
				break;
			rndspot--;
		}
	}
	if ( spot )
		return spot;

	G_dprint( "FindRandomTeamSpawnPoint: error\n" );

	return world;
}

gedict_t *lastspawn;

gedict_t *FindDeathmatchSpawnPoint()
{
	gedict_t *spot;

	int     spot_found;
	int     attempts;

	spot = lastspawn;
	if ( !spot )
		spot = world;
	for ( attempts = 0;; attempts++ )
	{
		spot = find( spot, FOFS( s.v.classname ), "info_player_deathmatch" );
		if ( !spot )
			spot = find( world, FOFS( s.v.classname ), "info_player_deathmatch" );
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

}


gedict_t *SelectSpawnPoint()
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



////////////////
// GlobalParams:
// time
// self
// called after ClientConnect
///////////////

void PutClientInServer()
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
	DecodeLevelParms();
	if ( !self->playerclass )
	{
		if ( TeamFortress_TeamIsCivilian( self->team_no ) )
		{
			self->s.v.impulse = 1;
			TeamFortress_ChangeClass();
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
		self->playerclass = 1 + ( int ) ( g_random() * ( 10 - 1 ) );
		while ( !IsLegalClass( self->playerclass )
			|| self->playerclass == oldclass || ClassIsRestricted( self->team_no, self->playerclass ) )
			self->playerclass = 1 + ( int ) ( g_random() * ( 10 - 1 ) );
		self->tfstate = TFSTATE_RANDOMPC;
		TeamFortress_ExecClassScript( self );
	} else
		self->tfstate = 0;

	if ( iszoom == 1 )
		self->tfstate |= TFSTATE_ZOOMOFF;
	if ( self->playerclass != PC_ENGINEER && !tg_data.tg_enabled )
		Engineer_RemoveBuildings( self );

	self->s.v.takedamage = 2;
	TeamFortress_PrintClassName( self, self->playerclass, self->tfstate & 8 );
	TeamFortress_SetEquipment();
	TeamFortress_SetHealth();
	TeamFortress_SetSpeed( self );
	TeamFortress_SetSkin( self );
	stuffcmd( self, "v_idlescale 0\nfov 90\n" );
	stuffcmd( self, "v_cshift; wait; bf\n" );
	SetTeamName( self );
	W_SetCurrentAmmo();
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
	spot = SelectSpawnPoint();
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

	player_stand1();
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


void SP_Null_tf_spawn()
{
	if ( !CheckExistence() )
	{
		dremove( self );
		return;
	}
}


void PrintClientScore( gedict_t * c )
{
	G_bprint( 1, "%3d %s\n", c->s.v.frags, c->s.v.netname );
}


void DumpScore()
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
}

/*
===============================================================================

RULES

===============================================================================
*/
int     already_cycled;

/*
go to the next level 
*/

void NextLevel()
{
	gedict_t *o;

	if ( already_cycled )
		return;
	already_cycled = 1;
	o = spawn();
	o->map = nextmap;

	o->s.v.think = ( func_t ) execute_changelevel;
	o->s.v.nextthink = g_globalvars.time + 0.1;
}

/*
============
CheckRules

Exit games upon conditions
============
*/

void CheckRules()
{
	if ( timelimit && g_globalvars.time >= timelimit )
		NextLevel();

	if ( fraglimit && self->s.v.frags >= fraglimit )
		NextLevel();
}

//============================================================================

void PlayerDeathThink()
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
				respawn();
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
			respawn();
		}
		return;
	}
}

void PlayerJump()
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
			if ( g_random() < 0.5 )
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

}



void WaterMove()
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
}

void CheckWaterJump()
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
}

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
void PlayerPreThink()
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
		IntermissionThink();	// otherwise a button could be missed between
		return;		// the think tics
	}

	makevectors( self->s.v.v_angle );	// is this still used

	if ( !self->playerclass )
	{
		if ( self->s.v.button2 && ( self->current_menu == MENU_TEAM ) && !self->team_no )
		{
			if ( TeamFortress_TeamPutPlayerInTeam() )
			{
				self->current_menu = MENU_CLASS;
				self->menu_count = MENU_REFRESH_RATE;
			}
		}
	}
	if ( self->s.v.view_ofs[0] == 0 && self->s.v.view_ofs[1] == 0 && self->s.v.view_ofs[2] == 0 )
		return;		// intermission or finale

	CheckRules();
	if ( self->playerclass )
		WaterMove();
	if ( self->s.v.deadflag >= DEAD_DEAD )
	{
		PlayerDeathThink();
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
			PlayerJump();
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
		self->s.v.weapon = W_BestWeapon();
		W_SetCurrentAmmo();
	}
	if ( self->on_hook )
		Service_Grapple();
}

/////////////////////////////////////////////////////////////////
/*
================
CheckPowerups

Check for turning off powerups
================
*/
void CheckPowerups()
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
					self->invisible_sound = g_globalvars.time + g_random() * 3 + 1;
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
}

////////////////
// GlobalParams:
// time
// self
///////////////
void PlayerPostThink()
{
//      float   mspeed;
//      float   aspeed;
//      float   r;
	float   fdmg;
	float   dtime;

	dtime = g_globalvars.time - self->lasttime;

 	VectorSubtract(self->s.v.velocity,self->lastvel,self->accel);
	VectorScale(self->accel, 1/dtime , self->accel);
	VectorCopy(self->s.v.velocity, self->lastvel);
	self->lasttime = g_globalvars.time;
	

	if ( self->s.v.view_ofs[0] == 0 && self->s.v.view_ofs[1] == 0 && self->s.v.view_ofs[2] == 0 )
		return;		// intermission or finale
	if ( self->s.v.deadflag )
	{
		DeadImpulses();
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
	CheckPowerups();
	W_WeaponFrame();
	if ( self->motd <= 95 )
		TeamFortress_MOTD();
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
		TeamFortress_CheckTeamCheats();
		self->cheat_check = g_globalvars.time + 3;
	}
}

////////////////
// GlobalParams:
// time
// self
// params
///////////////

void ClientConnect()
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
		GotoNextMap();

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
			last_id = last_id + 20 + g_random() * 10;
			self->tf_id = floor( g_random() * 10 + last_id );
			stuffcmd( self, "setinfo tf_id %d\n", self->tf_id );
			G_sprint( self, 2, "Your Battle ID is %d\n", self->tf_id );
		}
	}
	if ( tf_data.cb_prematch_time > g_globalvars.time )
		G_sprint( self, 2, "CURRENTLY IN PREMATCH TIME\n" );
        if( tg_data.tg_enabled )
		stuffcmd( self, "exec tg.cfg\n" );
}

////////////////
// GlobalParams:
// self
///////////////


void ClientDisconnect()
{
	gedict_t *te,*saveself;

//      float   fr;

	G_bprint( PRINT_HIGH, "%s left the game with %.0f frags\n", self->s.v.netname, self->s.v.frags );

	sound( self, CHAN_BODY, "player/tornoff2.wav", 1, ATTN_NONE );

	te = find( world, FOFS( s.v.classname ), "primer" );
	while ( te && te->s.v.owner != EDICT_TO_PROG( self ) )
		te = find( te, FOFS( s.v.classname ), "primer" );
	if ( te )
//  te->s.v.nextthink = g_globalvars.time;
	{
		self->s.v.deadflag = 1;
		saveself = self;
		self = te;
		( ( void ( * )(  ) ) ( self->s.v.think ) ) (  );
		self = saveself;
		self->s.v.deadflag = 0;
	}

	self->has_disconnected = 1;
	TeamFortress_RemoveTimers();


	Engineer_RemoveBuildings( self );
	te = find( world, FOFS( s.v.classname ), "detpack" );
	while ( te )
	{
		if ( te->real_owner == self )
		{
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
		te = spawn();
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
	set_suicide_frame();
	//self->s.v.netname = "";
	self->team_no = 0;
	self->s.v.solid = 0;
	setsize( self, 0, 0, 0, 0, 0, 0 );
}

/*
===========
ClientObituary

called when a player dies
============
*/

void ClientObituary( gedict_t * targ, gedict_t * attacker )
{
	float   rnum;
	char   *deathstring = "";
	//char   *deathstring2 = "";
	//float   attackerteam;
	//float   targteam;
	//gedict_t *te;

	rnum = g_random();
	if ( tf_data.cb_prematch_time + 3 > g_globalvars.time )
		return;
	if ( strneq( targ->s.v.classname, "player" ) )
	{
		if ( streq( targ->s.v.classname, "building_sentrygun" ) )
		{
			if ( streq( attacker->s.v.classname, "teledeath" ) )
			{
				G_bprint( 1, "%s's sentrygun was telefragged by %s\n", targ->real_owner->s.v.netname,
					  PROG_TO_EDICT( attacker->s.v.owner )->s.v.netname );
				return;
			}
			if ( streq( attacker->s.v.classname, "player" ) )
			{
				if ( attacker == targ->real_owner )
				{
					G_bprint( 1, "%s destroys his sentrygun\n", targ->real_owner->s.v.netname );
					return;
				}
				G_bprint( 1, "%s's sentrygun was destroyed by %s\n", targ->real_owner->s.v.netname,
					  attacker->s.v.netname );
				if ( attacker->team_no > 0 && attacker->team_no == targ->real_owner->team_no )
				{
					TF_AddFrags( attacker, -1 );
					attacker->teamkills = attacker->teamkills + 1;
					CheckAutoKick( attacker );
				} else
					TF_AddFrags( attacker, 1 );
				return;
			}
		} else
		{
			if ( streq( targ->s.v.classname, "building_dispenser" ) )
			{
				if ( attacker->team_no > 0 && attacker->team_no == targ->real_owner->team_no )
				{
					TF_AddFrags( attacker, -1 );
					attacker->teamkills = attacker->teamkills + 1;
					CheckAutoKick( attacker );
				}
			}
		}
	}
	if ( strneq( targ->s.v.classname, "player" ) )
		return;
//player deaths
	if ( streq( attacker->s.v.classname, "teledeath" ) )
	{
		G_bprint( 1, "%s was telefragged by %s\n", targ->s.v.netname,
			  PROG_TO_EDICT( attacker->s.v.owner )->s.v.netname );
		if ( PROG_TO_EDICT( attacker->s.v.owner )->team_no != targ->team_no
		     || PROG_TO_EDICT( attacker->s.v.owner )->team_no < 1 )
			TF_AddFrags( PROG_TO_EDICT( attacker->s.v.owner ), 1 );
		return;
	}
	if ( streq( attacker->s.v.classname, "teledeath2" ) )
	{
		G_bprint( 1, "Satan's power deflects %s's telefrag\n", targ->s.v.netname );
		TF_AddFrags( targ, -1 );
		logfrag( targ, targ );
		return;
	}
	if ( tf_data.deathmsg == DMSG_TEAMKILL )
	{
		G_bprint( 1, "%s shoots his teammate one too many times.\n", targ->s.v.netname );
		return;
	}
	if ( streq( attacker->s.v.classname, "info_tfgoal" ) || streq( attacker->s.v.classname, "item_tfgoal" ) )
	{
		if ( attacker->deathtype && attacker->deathtype[0] )
		{
			G_bprint( 1, "%s%s", targ->s.v.netname, attacker->deathtype );
		}
		logfrag( targ, targ );
		return;
	}
//player vs player or bot
	if ( streq( attacker->s.v.classname, "player" ) || streq( attacker->s.v.classname, "bot" ) )
	{
		if ( targ == attacker )
		{
			TF_AddFrags( attacker, -1 );
			if ( tf_data.birthday == 1 && g_random() < 0.3 )
			{
				if ( g_random() < 0.1 )
				{
					G_bprint( 1, "It's %s's party and he'll cry if he wants to!\n",
						  targ->s.v.netname );
					return;
				}
				if ( g_random() < 0.5 )
					G_bprint( 1, "%s gets too selfish with his gifts\n", targ->s.v.netname );
				else
					G_bprint( 1, "%s wasn't born so beautiful after all\n", targ->s.v.netname );
				return;
			}
			switch ( tf_data.deathmsg )
			{
			case 8:
				switch ( targ->playerclass )
				{
				case 2:
					deathstring = "%s got splattered by his own grenade\n";
					break;
				case 3:
					deathstring = "%s sat on his own grenade\n";
					break;
				case 4:
					deathstring = "%s got to know his grenade too well\n";
					break;
				case 5:
					deathstring = "%s caught the end of his own grenade\n";
					break;
				case 6:
					deathstring = "%s got too close to his own grenade\n";
					break;
				case 7:
					deathstring = "%s let his own grenade get the best of him\n";
					break;
				case 8:
					deathstring = "%s tiptoed over his own grenade\n";
					break;
				case 9:
					deathstring = "%s stared at his grenade too long\n";
					break;
				default:
					deathstring = "%s grenades himself\n";
					break;
				}
				break;
			case 9:
				deathstring = "%s hammers himself\n";
				break; 
			case 10:
				switch ( targ->playerclass )
				{
				case 4:
					deathstring = "%s practiced his own Mirv dance\n";
					break;
				case 6:
					deathstring = "%s allowed his Mirv to turn against him\n";
					break;
				default:
					deathstring = "%s goes to pieces\n";
					break;

				}
				break;
			case 11:
				deathstring = "%s ambushes himself with his own pipebombs\n";
				break;
			case 40:
				deathstring = "%s tried to juggle his own pipebombs\n";
				break;
			case 24:
				deathstring = "%s chokes on his own gas\n";
				break;
			case 30:
				deathstring = "%s explodes his ammo and body\n";
				break;
			case 41:
				deathstring = "%s stepped on too many of his own caltrops\n";
				break;
			case 35:
				deathstring = "%s is charred by his own flash grenade\n";
				break;
			case 31:
				deathstring = "%s detonates an ammo box too close to him\n";
				break;
			case 12:
				deathstring = "%s set the detpack and forgot to run\n";
				break;
			case 13:
				deathstring = "%s died impossibly!\n";
				break;
			case 6:
				if ( rnum < 0.5 )
					deathstring = "%s becomes bored with life\n";
				else
					deathstring = "%s checks if his weapon is loaded\n";
				break;
			case 33:
				deathstring = "%s chars himself with an incendiary rocket\n";
				break;
			case 5:
				deathstring = "%s tries to put the pin back in\n";
				break;
			case 15:
				deathstring = "%s torches himself\n";
				break;
			case 7:
				if ( targ->s.v.waterlevel > 1 )
					deathstring = "%s discharges into the water.\n";
				break;
			case 38:
				deathstring = "%s gets too friendly with his sentrygun.\n";
				break;
			case 39:
				deathstring = "%s dispenses with himself.\n";
				break;
			}
			G_bprint( 1, deathstring, targ->s.v.netname );
			return;
		}		//end of sucides
		else
		{
			if ( teamplay && attacker->team_no == targ->team_no && attacker->team_no > 0 )
			{
				TF_AddFrags( attacker, -1 );
				if ( !targ->undercover_team )
					attacker->teamkills = attacker->teamkills + 1;
				if ( tf_data.birthday == 1 && g_random() < 0.3 )
				{
					if ( g_random() < 0.3 )
						G_bprint( 1, "%s is a party-pooper!\n", targ->s.v.netname );
					else
						G_bprint( 1, "%s gives gifts to his teammates!\n", targ->s.v.netname );
					return;
				}
				switch ( ( int ) ( g_random() * 4 ) )
				{
				case 0:
					deathstring = "%s mows down teammate %s\n";
					break;
				case 1:
					deathstring = "%s checks his glasses after killing %s\n";
					break;
				case 2:
					deathstring = "%s gets a frag for the other team with %s's death\n";
					break;
				default:
					deathstring = "%s killed his supposed friend %s\n";
					break;
				}
				if ( tf_data.deathmsg == 23 )
				{
					G_bprint( 1, "%s didn't survive the operation by %s\n", targ->s.v.netname,
						  attacker->s.v.netname );
					return;
				}
				G_bprint( 1, deathstring, attacker->s.v.netname, targ->s.v.netname );
				CheckAutoKick( attacker );
				return;
			}	//end of teamkills
			else
			{
				TF_AddFrags( attacker, 1 );
				logfrag( attacker, targ );
				if ( tf_data.birthday == 1 && g_random() < 0.5 )
				{
					switch ( tf_data.deathmsg )
					{
					case 9:
						deathstring = "%s chews on %s's nails!\n";
						break;
					case 10:
						deathstring = "%s gathers the darling buds of %s's Mirv grenade\n";
						break;
					case 30:
						deathstring = "%s's presents go up in %s's EMP presence.\n";
						break;
					case 35:
						G_bprint( 1, "%s goes POP!\n", targ->s.v.netname );
						return;
					case 31:
						deathstring = "%s stands to near %s's birthday surprise\n";
						break;
					case 12:
						deathstring = "%s eats %s's good cheer!\n";
						if ( g_random() < 0.1 )
							G_bprint( 1, "Damn that jokey smurf!\n" );
						break;
					case 16:
						deathstring = "%s cut the red ribbon of %s's detpack\n";
						break;
					case 5:
						deathstring = "%s recieves a gift from %s\n";
						if ( targ->s.v.health < -40 )
							deathstring = "%s plays pass-the-parcel with %s\n";
						break;
					case 6:
						deathstring = "%s rides %s's firecracker\n";
						if ( targ->s.v.health < -40 )
						{
							G_bprint( 1,
								  "%s gets turned into little %s's by %s's firecracker",
								  targ->s.v.netname, targ->s.v.netname,
								  attacker->s.v.netname );
							return;
						}
						break;
					case 15:
						if ( ( int ) ( g_random() * 2 ) )
							deathstring = "%s gets too close to %s's kitchen\n";
						else
							deathstring = "%s plays with %s's fire\n";
						break;
					case 18:
						if ( ( int ) ( g_random() * 3 ) )
							deathstring = "%s blocks %s's birthday bullet with his chest\n";
						else
							deathstring = "%s gets party popped by %s\n";
						break;
					case 29:
						if ( ( int ) ( g_random() * 2 ) )
							deathstring =
							    "%s ALMOST catches %s's bullet between his teeth!\n";
						else
							deathstring = "%s loves snipers like %s\n";
						break;
					case 28:
						deathstring = "%s won't run crying to %s anymore\n";
						break;
					case 19:
						deathstring = "%s collects %s's highspeed gifts.\n";
						break;
					case 20:
						deathstring = "%s died. I blame %s\n";
						break;
					case 22:
						deathstring = "%s gets a gift in the back from %s\n";
						break;
					case 2:
						deathstring = "%s gets a double of %s's buck\n";
						break;
					case 25:
						deathstring = "%s is all partied out by %s\n";
						break;
					case 26:
						deathstring = "%s gets derailed by %s\n";
						break;
					case 3:
						deathstring = "%s gets no say in it, no say in it at all! sings %s\n";
						break;
					case 33:
						deathstring = "%s gets all fired up by %s\n";
						break;
					default:	//death msg 8??? 
						if ( ( int ) ( g_random() * 2 ) )
							deathstring = "%s recieves a gift from %s\n";
						else
							deathstring = "%s has a happy birthday, thanks to %s\n";
						break;

					}
					G_bprint( 1, deathstring, targ->s.v.netname, attacker->s.v.netname );
					return;
				}	//end of player vs player Birthday messages
//normal player vs player messages
				switch ( tf_data.deathmsg )
				{
				case 8:
					switch ( attacker->playerclass )
					{
					case 2:
						deathstring = "%s got up-close and personal with %s's grenade\n";
						break;
					case 3:
						deathstring = "%s played catch with %s's grenade\n";
						break;
					case 4:
						deathstring = "%s received a pineapple enema from %s\n";
						break;
					case 5:
						deathstring = "%s fetched %s's pineapple\n";
						break;
					case 6:
						deathstring = "%s caught too much shrapnel from %s's grenade\n";
						break;
					case 7:
						deathstring = "%s tried to pick up %s's hot potato\n";
						break;
					case 8:
						deathstring = "%s thought %s was tossing him a spare grenade\n";
						break;
					case 9:
						deathstring =
						    "%s stops to ponder the technical details of %s's grenade\n";
						break;
					default:
						deathstring = "%s surfs on a grenade from %s\n";
						break;
					}
					break;
				case 9:
					deathstring = "%s gets flayed by %s's nail grenade\n";
					break;
				case 10:
					if ( attacker->playerclass == 4 )
						deathstring = "%s does a dance on %s's Mirv grenade\n";
					else
						deathstring = "%s gets spammed by %s's Mirv grenade\n";
					break;
				case 11:
					deathstring = "%s is caught by %s's pipebomb trap\n";
					break;
				case 40:
					deathstring = "%s fell victim to %s's fireworks\n";
					break;
				case 24:
					deathstring = "%s gags on %s's noxious gasses\n";
					break;
				case 30:
					deathstring = "%s's ammo detonates him as %s's EMP fries it\n";
					break;
				case 41:
					deathstring = "%s stepped on too many of %s's caltrops\n";
					break;
				case 35:
					deathstring = "%s is charred by %s's flash grenade\n";
					break;
				case 31:
					deathstring = "%s stands near some ammo as %s's EMP nukes it\n";
					break;
				case 12:
					deathstring = "%s reaches orbit via %s's detpack\n";
					break;
				case 16:
					deathstring = "%s cut the red wire of %s's detpack\n";
					break;
				case 13:
					deathstring = "%s dies from %s's mysterious tropical disease\n";
					break;
				case 14:
					deathstring = "%s escapes infection from %s by dying first\n";
					break;
				case 5:
					deathstring = "%s eats %s's pineapple\n";
					if ( targ->s.v.health < -40 )
						deathstring = "%s was gibbed by %s's grenade\n";
					break;
				case 6:
					deathstring = "%s rides %s's rocket\n";
					if ( targ->s.v.health < -40 )
						deathstring = "%s was gibbed by %s's rocket\n";
					break;
				case 15:
					switch ( ( int ) ( rnum * 5 ) )
					{
					case 0:
						deathstring = "%s is burnt up by %s's flame\n";
						break;
					case 1:
						deathstring = "%s is fried by %s's fire\n";
						break;
					case 2:
						deathstring = "%s feels %s's fire of wrath\n";
						break;
					case 3:
						deathstring = "%s is reduced to ashes by %s\n";
						break;
					default:
						deathstring = "%s is grilled by %s's flame\n";
						break;
					}
					break;
				case 17:
					switch ( attacker->playerclass )
					{
					case 8:
						deathstring = "%s was knife-murdered by %s\n";
						break;
					case 1:
						deathstring = "%s's mellon was split by %s\n";
						break;
					case 2:
						deathstring = "%s was put on the chop block by %s\n";
						break;
					case 3:
						deathstring = "%s was sliced and diced by %s's blade\n";
						break;
					case 4:
						deathstring = "%s is split from crotch to sternum by %s's axe swing\n";
						break;
					case 6:
						deathstring = "%s is split in two with a powerful axe blow from %s\n";
						break;
					case 7:
						deathstring = "%s's death put another notch on %s's axe\n";
						break;
					default:
						deathstring = "%s's death put another notch on %s's axe\n";
						break;
					}
					break;

				case 32:
					deathstring = "%s was spanner-murdered by %s\n";
					break;
				case 1:
					switch ( attacker->playerclass )
					{
					case 1:
						deathstring = "%s got too close to %s's muzzleflash\n";
						break;
					case 3:
						deathstring = "%s practices being %s's clay pigeon\n";
						break;
					case 4:
						deathstring = "%s was on the receiving end of %s's shotgun barrel\n";
						break;
					case 5:
						deathstring = "%s was fed a lead diet by %s\n";
						break;
					case 6:
						deathstring = "%s got blasted by %s's last resort\n";
						break;
					case 7:
						deathstring = "%s got more than a powderburn from %s's shotgun blast\n";
						break;
					default:
						deathstring = "%s chewed on %s's boomstick\n";
						break;
					}
					break;
				case 2:
					switch ( attacker->playerclass )
					{

					case 3:
						deathstring = "%s was turned into swiss cheese by %s's buckshot\n";
						break;
					case 5:
						deathstring = "%s got a double-dose of %s's buckshot\n";
						break;
					case 6:
						deathstring = "%s unfortunately forgot %s carried a super-shotgun\n";
						break;
					case 8:
						deathstring = "%s gets ventilated by %s's super-shotgun blast\n";
						break;
					case 9:
						deathstring = "%s's body got chuck full of %s's lead pellets\n";
						break;
					default:
						deathstring = "%s ate 2 loads of %s's buckshot\n";
						break;
					}
					break;
				case 3:
					switch ( attacker->playerclass )
					{
					case 1:
						deathstring = "%s caught one too many nails from %s\n";
						break;
					case 2:
						deathstring = "%s ran into %s's nails\n";
						break;
					case 8:
						deathstring = "%s was turned into %s's pin-cushion\n";
						break;
					default:
						deathstring = "%s was nailed by %s\n";
						break;
					}
					break;
				case 4:
					deathstring = "%s was punctured by %s\n";
					break;
				case 7:
					if ( attacker->s.v.waterlevel > 1 )
						deathstring = "%s accepts %s's discharge\n";
					else
						deathstring = "%s accepts %s's shaft\n";
					break;
				case 21:
					deathstring = "%s grappled with %s\n";
					break;
				case 18:
					if ( rnum <= 0.3 )
					{
						deathstring = "%s takes a bullet in the chest from %s\n";
					} else
					{
						deathstring = "%s succumbs to sniperfire from %s\n";
					}
					break;
				case 29:
					if ( rnum <= 0.5 )
					{
						deathstring = "%s gets a third eye from %s\n";
					} else
					{
						deathstring = "%s gets his head blown off by %s\n";
					}
					break;
				case 28:
					if ( rnum <= 0.5 )
					{
						deathstring = "%s is made legless by %s\n";
					} else
					{
						deathstring = "%s gets his legs blown off by %s\n";
					}
					break;
				case 19:
					deathstring = "%s collects %s's bullet spray.\n";
					break;
				case 20:
					deathstring = "%s gets sawn in half by %s\n";
					break;
				case 22:
					deathstring = "%s gets knifed from behind by %s\n";
					break;
				case 25:
					deathstring = "%s is put to sleep by %s\n";
					break;
				case 26:
					deathstring = "%s gets a hole in his heart from %s's railgun\n";
					break;
				case 33:
					deathstring = "%s gets well done by %s's incendiary rocket\n";
					break;
				case 38:
					deathstring = "%s gets destroyed by %s's exploding sentrygun\n";
					break;
				case 39:
					deathstring = "%s didn't insert the correct change into %s's dispenser.\n";
					break;

				}
				G_bprint( 1, deathstring, targ->s.v.netname, attacker->s.v.netname );
			}
		}
		return;
	}			//end of player vs player
	else
	{
		if ( streq( attacker->s.v.classname, "building_sentrygun" ) )
		{
			if ( targ == attacker->real_owner )
			{
				if ( tf_data.deathmsg == DMSG_SENTRYGUN_ROCKET )
					deathstring = "%s intercepts his sentry gun's rocket\n";
				else
				{
					if ( tf_data.deathmsg == 27 )
						deathstring = "%s crossed his sentry gun's line of fire\n";
				}
				G_bprint( 1, deathstring, targ->s.v.netname );
			} else
			{
				if ( teamplay && attacker->team_no == targ->team_no && attacker->team_no > 0 )
				{
					G_bprint( 1, "%s obstructs his team's sentry gun\n", targ->s.v.netname );
				} else
				{
					TF_AddFrags( attacker->real_owner, 1 );
					logfrag( attacker->real_owner, targ );
					if ( tf_data.deathmsg == DMSG_SENTRYGUN_ROCKET )
					{
						deathstring = "%s hates %s's sentry gun\n";
					} else
					{
						if ( tf_data.deathmsg == 27 )
						{
							deathstring = "%s is mown down by %s's sentry gun\n";
						}
					}
					G_bprint( 1, deathstring, targ->s.v.netname,
						  attacker->real_owner->s.v.netname );
				}
			}
		} else
		{
			logfrag( targ, targ );
			TF_AddFrags( targ, -1 );
			rnum = targ->s.v.watertype;
			switch ( ( int ) targ->s.v.watertype )
			{
			case -3:
				if ( tf_data.birthday == 1 )
				{
					if ( g_random() < 0.5 )
						deathstring = "%s bobs for apples\n";
					else
						deathstring = "%s drowns in the punch\n";
				} else
				{
					if ( g_random() < 0.5 )
						deathstring = "%s sleeps with the fishes\n";
					else
						deathstring = "%s sucks it down\n";
				}
				break;
			case -4:
				if ( g_random() < 0.5 )
					deathstring = "%s gulped a load of slime\n";
				else
					deathstring = "%s can't exist on slime alone\n";
				break;
			case -5:
				if ( targ->s.v.health < -15 )
					deathstring = "%s burst into flames\n";
				else
				{
					if ( g_random() < 0.5 )
						deathstring = "%s turned into hot slag\n";
					else
						deathstring = "%s visits the Volcano God\n";
				}
				break;
			default:
				if ( streq( attacker->s.v.classname, "explo_box" ) )
				{
					deathstring = "%s blew up\n";
					break;
				}
				if ( attacker->s.v.solid == 4 && attacker != world )
				{
					deathstring = "%s was squished\n";
					break;
				}
				if ( streq( targ->deathtype, "falling" ) )
				{
					targ->deathtype = "";
					deathstring = "%s fell to his death\n";
					break;
				}
				if ( streq( attacker->s.v.classname, "trap_shooter" )
				     || streq( attacker->s.v.classname, "trap_spikeshooter" ) )
				{
					deathstring = "%s was spiked\n";
					break;
				}
				if ( streq( attacker->s.v.classname, "fireball" ) )
				{
					deathstring = "%s ate a lavaball\n";
					break;
				}
				if ( streq( attacker->s.v.classname, "trigger_changelevel" ) )
					deathstring = "%s tried to leave\n";
				else
					deathstring = "%s died\n";

				break;

			}
			G_bprint( 1, deathstring, targ->s.v.netname );
		}
	}
}
