/*
 *  QWProgs-TF2003
 *  Copyright (C) 2004  [sd] angel
 *
 *  This code is based on Q3 VM code by Id Software, Inc.
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
 *  $Id: g_spawn.c,v 1.8 2005-05-28 22:35:46 AngelD Exp $
 */

#include "g_local.h"

#define	MAX_SPAWN_VARS			64
#define	MAX_SPAWN_VARS_CHARS	4096

static int             numSpawnVars;
static char           *spawnVars[MAX_SPAWN_VARS][2];	// key / value pairs
static int             numSpawnVarChars;
static char            spawnVarChars[MAX_SPAWN_VARS_CHARS];

qboolean G_SpawnString( const char *key, const char *defaultString, char **out )
{
	int             i;

/*	if ( !level.spawning ) {
		*out = (char *)defaultString;
//		G_Error( "G_SpawnString() called while not spawning" );
	}*/

	for ( i = 0; i < numSpawnVars; i++ )
	{
		if ( !Q_stricmp( key, spawnVars[i][0] ) )
		{
			*out = spawnVars[i][1];
			return true;
		}
	}

	*out = ( char * ) defaultString;
	return false;
}

qboolean G_SpawnFloat( const char *key, const char *defaultString, float *out )
{
	char           *s;
	qboolean        present;

	present = G_SpawnString( key, defaultString, &s );
	*out = atof( s );
	return present;
}

qboolean G_SpawnInt( const char *key, const char *defaultString, int *out )
{
	char           *s;
	qboolean        present;

	present = G_SpawnString( key, defaultString, &s );
	*out = atoi( s );
	return present;
}

qboolean G_SpawnVector( const char *key, const char *defaultString, float *out )
{
	char           *s;
	qboolean        present;

	present = G_SpawnString( key, defaultString, &s );
	sscanf( s, "%f %f %f", &out[0], &out[1], &out[2] );
	return present;
}

//
// fields are needed for spawning from the entity string
//

static const field_t         fields[] = {
	{"classname",	FOFS( s.v.classname ),	F_LSTRING},
	{"origin", 	FOFS( s.v.origin ),	F_VECTOR},
	{"model", 	FOFS( s.v.model ),	F_LSTRING},
	{"message", 	FOFS( s.v.message ),	F_LSTRING},
	{"target", 	FOFS( s.v.target ),	F_LSTRING},
	{"map", 	FOFS( map ),	 	F_LSTRING},
	{"killtarget", 	FOFS( killtarget ),	F_LSTRING},
	{"count", 	FOFS( count ),	 	F_FLOAT},
	{"targetname", 	FOFS( s.v.targetname ),	F_LSTRING},
	{"wait", 	FOFS( wait ),	 	F_FLOAT},
	{"skin", 	FOFS( s.v.skin ),	F_FLOAT},
	{"effects", 	FOFS( s.v.effects ),	F_FLOAT},
	{"speed", 	FOFS( speed ),	 	F_FLOAT},
	{"spawnflags", 	FOFS( s.v.spawnflags ),	F_FLOAT},
	{"health", 	FOFS( s.v.health ),	F_FLOAT},
	{"takedamage", 	FOFS( s.v.takedamage ),	F_FLOAT},
	{"dmg", 	FOFS( dmg ),	 	F_FLOAT},
	{"delay", 	FOFS( delay ),	 	F_FLOAT},
	{"worldtype", 	FOFS( worldtype ),	F_INT},
	{"lip", 	FOFS( lip ),		F_FLOAT},
	{"height", 	FOFS( height ),		F_FLOAT},
	{"sounds", 	FOFS( s.v.sounds ),	F_FLOAT},
	{"angles", 	FOFS( s.v.angles ),	F_VECTOR},
	{"mangle", 	FOFS( mangle ),		F_VECTOR},
	{"style", 	FOFS( style ),		F_INT},
	{"angle", 	FOFS( s.v.angles ),	F_ANGLEHACK},
	{"light", 	0,			F_IGNORE},
	{"wad", 	0, 			F_IGNORE},
	{"_color", 	0, 			F_IGNORE},
//////Team Fortress
	{"netname",		FOFS( s.v.netname ),		F_LSTRING},
	{"team_no", 		FOFS( team_no ),		F_INT},
	{"frags", 		FOFS( s.v.frags ),	 	F_FLOAT},
	{"noise",		FOFS( s.v.noise ),		F_LSTRING},
	{"noise1",		FOFS( s.v.noise1 ),		F_LSTRING},
	{"noise2",		FOFS( s.v.noise2 ),		F_LSTRING},
	{"noise3",		FOFS( s.v.noise3 ),		F_LSTRING},
	{"goal_activation",	FOFS( goal_activation ),	F_INT},
	{"g_a",			FOFS( goal_activation ),	F_INT},
	{"goal_effects",	FOFS( goal_effects ),		F_INT},
	{"g_e",			FOFS( goal_effects ),		F_INT},
//	{"items_allowed",	FOFS( items_allowed ),		F_INT},
	
        {"mdl",			FOFS( mdl ),			 F_LSTRING},
        {"killtarget",		FOFS( killtarget ),		 F_LSTRING},
        {"map",			FOFS( map ),	 F_LSTRING},
        {"noise4",		FOFS( noise4 ),	 F_LSTRING},
        {"deathtype",		FOFS( deathtype	 ),	 F_LSTRING},
        {"team_str_home",	FOFS( team_str_home ),	 F_LSTRING},
        {"t_s_h",		FOFS( team_str_home ),	 F_LSTRING},
        {"team_str_moved",	FOFS( team_str_moved ),	 F_LSTRING},
        {"t_s_m",		FOFS( team_str_moved ),	 F_LSTRING},
        {"team_str_carried",	FOFS( team_str_carried ),	 F_LSTRING},
        {"t_s_c",		FOFS( team_str_carried ),	 F_LSTRING},
        {"non_team_str_home",	FOFS( non_team_str_home ),	 F_LSTRING},
        {"n_s_h",		FOFS( non_team_str_home ),	 F_LSTRING},
        {"non_team_str_moved",	FOFS( non_team_str_moved ),	 F_LSTRING},
        {"n_s_m",		FOFS( non_team_str_moved ),	 F_LSTRING},
        {"non_team_str_carried",FOFS( non_team_str_carried ),	 F_LSTRING},
        {"n_s_c",		FOFS( non_team_str_carried ),	 F_LSTRING},
        {"broadcast",		FOFS( broadcast ),	 F_LSTRING},
        {"b_b",			FOFS( broadcast ),	 F_LSTRING},
        {"team_broadcast",	FOFS( team_broadcast ),	 F_LSTRING},
        {"b_t",			FOFS( team_broadcast ),	 F_LSTRING},
        {"non_team_broadcast",	FOFS( non_team_broadcast ),	 F_LSTRING},
        {"b_n",			FOFS( non_team_broadcast ),	 F_LSTRING},
        {"owners_team_broadcast",FOFS( owners_team_broadcast ),	 F_LSTRING},
        {"b_o",			FOFS( owners_team_broadcast ),	 F_LSTRING},
        {"netname_broadcast",	FOFS( netname_broadcast ),	 F_LSTRING},
        {"n_b",				FOFS( netname_broadcast ),	     	F_LSTRING},
        {"netname_team_broadcast",	FOFS( netname_team_broadcast ),	     	F_LSTRING},
        {"n_t",				FOFS( netname_team_broadcast ),	     	F_LSTRING},
        {"netname_non_team_broadcast",	FOFS( netname_non_team_broadcast ),	F_LSTRING},
        {"n_n",				FOFS( netname_non_team_broadcast ),	F_LSTRING},
        {"netname_owners_team_broadcast",FOFS( netname_owners_team_broadcast ),	F_LSTRING},
        {"n_o",				FOFS( netname_owners_team_broadcast ),	F_LSTRING},
        {"team_drop",			FOFS( team_drop ),	                F_LSTRING},
        {"d_t",				FOFS( team_drop ),	                F_LSTRING},
        {"non_team_drop",		FOFS( non_team_drop ),	                F_LSTRING},
        {"d_n",				FOFS( non_team_drop ),	                F_LSTRING},
        {"netname_team_drop",		FOFS( netname_team_drop ),	        F_LSTRING},
        {"d_n_t",			FOFS( netname_team_drop ),	        F_LSTRING},
        {"netname_non_team_drop",	FOFS( netname_non_team_drop ),	        F_LSTRING},
        {"d_n_n",			FOFS( netname_non_team_drop ),	        F_LSTRING},

        {"aflag",			FOFS( aflag ),			 	F_INT},
        {"last_impulse",		FOFS( last_impulse ),			F_INT},
        {"playerclass",			FOFS( playerclass ),			F_INT},
        {"nextpc",			FOFS( nextpc ),			 	F_INT},
        {"armorclass",			FOFS( armorclass ),			F_INT},
        {"tf_items",			FOFS( tf_items ),			F_INT},
        {"tf_items_flags",		FOFS( tf_items_flags ),			F_INT},
        {"no_grenades_1",		FOFS( no_grenades_1 ),			F_INT},
        {"no_grenades_2",		FOFS( no_grenades_2 ),			F_INT},
        {"tp_grenades_1",		FOFS( tp_grenades_1 ),			F_INT},
        {"tp_grenades_2",		FOFS( tp_grenades_2 ),			F_INT},
        {"got_aliases",			FOFS( got_aliases ),			F_INT},
        {"cheat_check",			FOFS( cheat_check ),			F_INT},
        {"is_removed",			FOFS( is_removed ),			F_INT},
        {"is_undercover",		FOFS( is_undercover ),			F_INT},
        {"is_building",			FOFS( is_building ),			F_INT},
        {"is_detpacking",		FOFS( is_detpacking ),			F_INT},
        {"is_feigning",			FOFS( is_feigning ),			F_INT},
        {"is_unabletospy",		FOFS( is_unabletospy ),			F_INT},
        {"has_disconnected",		FOFS( has_disconnected ),		F_INT},
        {"tfstate",			FOFS( tfstate ),			F_INT},
        {"maxammo_shells",		FOFS( maxammo_shells ),			F_INT},
        {"maxammo_nails",		FOFS( maxammo_nails ),			F_INT},
        {"maxammo_cells",		FOFS( maxammo_cells ),			F_INT},
        {"maxammo_rockets",		FOFS( maxammo_rockets ),		F_INT},
        {"items_allowed",		FOFS( items_allowed ),			F_INT},
        {"armor_allowed",		FOFS( armor_allowed ),			F_FLOAT},
        {"maxarmor",			FOFS( maxarmor ),			F_INT},
        {"maxfbspeed",			FOFS( maxfbspeed ),			F_INT},
        {"maxstrafespeed",		FOFS( maxstrafespeed ),			F_INT},
        {"weaponmode",			FOFS( weaponmode ),			F_INT},
        {"motd",			FOFS( motd ),			 	F_INT},
        {"current_menu",		FOFS( current_menu ),			F_INT},
        {"menu_count",			FOFS( menu_count ),			F_INT},
        {"menu_displaytime",		FOFS( menu_displaytime ),		F_INT},
        {"team_no",			FOFS( team_no ),			F_INT},
        {"lives",			FOFS( lives ),			 	F_INT},
        {"infection_team_no",		FOFS( infection_team_no ),		F_INT},
        {"has_dispenser",		FOFS( has_dispenser ),			F_INT},
        {"has_sentry",			FOFS( has_sentry ),			F_INT},
        {"real_frags",			FOFS( real_frags ),			F_INT},
        {"weapons_carried",		FOFS( weapons_carried ),		F_INT},
        {"current_weapon",		FOFS( current_weapon ),			F_INT},
        {"last_weapon",			FOFS( last_weapon ),			F_INT},
        {"last_weaponmode",		FOFS( last_weaponmode ),		F_INT},
        {"ammo_medikit",		FOFS( ammo_medikit ),			F_INT},
        {"maxammo_medikit",		FOFS( maxammo_medikit ),		F_INT},
        {"ammo_detpack",		FOFS( ammo_detpack ),			F_INT},
        {"maxammo_detpack",		FOFS( maxammo_detpack ),		F_INT},
        {"reload_shotgun",		FOFS( reload_shotgun ),			F_INT},
        {"reload_super_shotgun",	FOFS( reload_super_shotgun ),		F_INT},
        {"reload_grenade_launcher",	FOFS( reload_grenade_launcher ),	F_INT},
        {"reload_rocket_launcher",	FOFS( reload_rocket_launcher ),		F_INT},
        {"no_active_nail_grens",	FOFS( no_active_nail_grens ),		F_INT},
        {"distance",			FOFS( distance ),			F_INT},
        {"goal_no",			FOFS( goal_no ),			F_INT},
        {"group_no",			FOFS( group_no ),			F_INT},
        {"goal_state",			FOFS( goal_state ),			F_INT},
        {"owned_by",			FOFS( owned_by ),			F_INT},
        {"goal_activation",		FOFS( goal_activation ),		F_INT},
        {"goal_effects",		FOFS( goal_effects ),			F_INT},
        {"goal_result",			FOFS( goal_result ),			F_INT},
        {"goal_group",			FOFS( goal_group ),			F_INT},
        {"else_goal",			FOFS( else_goal ),			F_INT},
        {"if_goal_is_active",		FOFS( if_goal_is_active ),		F_INT},
        {"if_goal_is_inactive",		FOFS( if_goal_is_inactive ),		F_INT},
        {"if_goal_is_removed",		FOFS( if_goal_is_removed ),		F_INT},
        {"if_group_is_active",		FOFS( if_group_is_active ),		F_INT},
        {"if_group_is_inactive",	FOFS( if_group_is_inactive ),		F_INT},
        {"if_group_is_removed",		FOFS( if_group_is_removed ),		F_INT},
        {"activate_goal_no",		FOFS( activate_goal_no ),		F_INT},
        {"inactivate_goal_no",		FOFS( inactivate_goal_no ),		F_INT},
        {"remove_goal_no",		FOFS( remove_goal_no ),			F_INT},
        {"rv_g",			FOFS( remove_goal_no ),			F_INT},
        {"restore_goal_no",		FOFS( restore_goal_no ),		F_INT},
        {"rs_g",			FOFS( restore_goal_no ),		F_INT},
        {"activate_group_no",		FOFS( activate_group_no ),		F_INT},
        {"inactivate_group_no",		FOFS( inactivate_group_no ),		F_INT},
        {"remove_group_no",		FOFS( remove_group_no ),		F_INT},
        {"rv_gr",			FOFS( remove_group_no ),		F_INT},
        {"restore_group_no",		FOFS( restore_group_no ),		F_INT},
        {"rs_gr",			FOFS( restore_group_no ),		F_INT},
        {"has_item_from_group",		FOFS( has_item_from_group ),		F_INT},
        {"h_i_g",			FOFS( has_item_from_group ),		F_INT},
        {"hasnt_item_from_group",	FOFS( hasnt_item_from_group ),		F_INT},
        {"hn_i_g",			FOFS( hasnt_item_from_group ),		F_INT},
        {"remove_item_group",		FOFS( remove_item_group ),		F_INT},
        {"r_i_g",			FOFS( remove_item_group ),		F_INT},
        {"return_item_no",		FOFS( return_item_no ),			F_INT},
        {"if_item_has_moved",		FOFS( if_item_has_moved ),		F_INT},
        {"if_item_hasnt_moved",		FOFS( if_item_hasnt_moved ),		F_INT},
        {"remove_spawnpoint",		FOFS( remove_spawnpoint ),		F_INT},
        {"restore_spawnpoint",		FOFS( restore_spawnpoint ),		F_INT},
        {"remove_spawngroup",		FOFS( remove_spawngroup ),		F_INT},
        {"rv_s_h",			FOFS( remove_spawngroup ),		F_INT},
        {"restore_spawngroup",		FOFS( restore_spawngroup ),		F_INT},
        {"rs_s_h",			FOFS( restore_spawngroup ),		F_INT},
        {"display_item_status1",	FOFS( display_item_status1 ),		F_INT},
        {"display_item_status2",	FOFS( display_item_status2 ),		F_INT},
        {"display_item_status3",	FOFS( display_item_status3 ),		F_INT},
        {"display_item_status4",	FOFS( display_item_status4 ),		F_INT},
        {"ex_skill_min",		FOFS( ex_skill_min ),			F_INT},
        {"ex_skill_max",		FOFS( ex_skill_max ),			F_INT},
        {"increase_team1",		FOFS( increase_team1 ),			F_INT},
        {"increase_team2",		FOFS( increase_team2 ),			F_INT},
        {"increase_team3",		FOFS( increase_team3 ),			F_INT},
        {"increase_team4",		FOFS( increase_team4 ),			F_INT},
        {"all_active",			FOFS( all_active ),			F_INT},
        {"item_list",			FOFS( item_list ),			F_INT},
        {"delay_time",			FOFS( delay_time ),			F_FLOAT},
        {"dont_do_triggerwork",		FOFS( dont_do_triggerwork ),		F_INT},
//float
        {"healamount",			FOFS( healamount ),			F_FLOAT},
        {"healtype",			FOFS( healtype ),			F_FLOAT},
        {"t_length",			FOFS( t_length ),			F_FLOAT},
        {"t_width",			FOFS( t_width ),			F_FLOAT},
        {"pausetime",			FOFS( pausetime ),			F_FLOAT},
        {"cnt",				FOFS( cnt ),				F_FLOAT},
        {"attack_finished",		FOFS( attack_finished ),		F_FLOAT},
        {"pain_finished",		FOFS( pain_finished ),			F_FLOAT},
        {"invincible_finished",		FOFS( invincible_finished ),		F_FLOAT},
        {"invisible_finished",		FOFS( invisible_finished ),		F_FLOAT},
        {"super_damage_finished",	FOFS( super_damage_finished ),		F_FLOAT},
        {"radsuit_finished",		FOFS( radsuit_finished ),		F_FLOAT},
        {"invincible_time",		FOFS( invincible_time ),		F_FLOAT},
        {"invincible_sound",		FOFS( invincible_sound ),		F_FLOAT},
        {"invisible_time",		FOFS( invisible_time ),			F_FLOAT},
        {"invisible_sound",		FOFS( invisible_sound ),		F_FLOAT},
        {"super_time",			FOFS( super_time ),			F_FLOAT},
        {"super_sound",			FOFS( super_sound ),			F_FLOAT},
        {"rad_time",			FOFS( rad_time ),			F_FLOAT},
        {"fly_sound",			FOFS( fly_sound ),			F_FLOAT},
        {"axhitme",			FOFS( axhitme ),			F_FLOAT},
        {"show_hostile",		FOFS( show_hostile ),			F_FLOAT},
        {"jump_flag",			FOFS( jump_flag ),			F_FLOAT},
        {"swim_flag",			FOFS( swim_flag ),			F_FLOAT},
        {"air_finished",		FOFS( air_finished ),			F_FLOAT},
        {"bubble_count",		FOFS( bubble_count ),			F_FLOAT},
        {"dmgtime",			FOFS( dmgtime ),			F_FLOAT},
        {"building_wait",		FOFS( building_wait ),			F_FLOAT},
        {"respawn_time",		FOFS( respawn_time ),			F_FLOAT},
        {"suicide_time",		FOFS( suicide_time ),			F_FLOAT},
        {"heat",			FOFS( heat ),				F_FLOAT},
        {"immune_to_check",		FOFS( immune_to_check ),		F_FLOAT},
        {"last_saveme_sound",		FOFS( last_saveme_sound ),		F_FLOAT},
        {"waitmin",			FOFS( waitmin ),			F_FLOAT},
        {"waitmax",			FOFS( waitmax ),			F_FLOAT},
        {"search_time",			FOFS( search_time ),			F_FLOAT},
        {"camdist",			FOFS( camdist ),			F_FLOAT},
//vectors
        {"dest",			FOFS( dest ),				F_VECTOR},
        {"dest1",			FOFS( dest1 ),				F_VECTOR},
        {"dest2",			FOFS( dest2 ),				F_VECTOR},
        {"pos1",			FOFS( pos1 ),				F_VECTOR},
        {"pos2",			FOFS( pos2 ),				F_VECTOR},
        {"finaldest",			FOFS( finaldest ),			F_VECTOR},
        {"goal_min",			FOFS( goal_min ),			F_VECTOR},
        {"goal_max",			FOFS( goal_max ),			F_VECTOR},
        {"camangle",			FOFS( camangle ),			F_VECTOR},
//float2
        {"ltime",			FOFS( s.v.ltime ),			F_FLOAT},
        {"lastruntime",			FOFS( s.v.lastruntime ),		F_FLOAT},
        {"solid",			FOFS( s.v.solid ),			F_FLOAT},
        {"frame",			FOFS( s.v.frame ),			F_FLOAT},
        {"nextthink",			FOFS( s.v.nextthink ),			F_FLOAT},
        {"frags",			FOFS( s.v.frags ),			F_FLOAT},
        {"weapon",			FOFS( s.v.weapon ),			F_FLOAT},
        {"weaponframe",			FOFS( s.v.weaponframe ),		F_FLOAT},
        {"currentammo",			FOFS( s.v.currentammo ),		F_FLOAT},
        {"ammo_shells",			FOFS( s.v.ammo_shells ),		F_FLOAT},
        {"ammo_nails",			FOFS( s.v.ammo_nails ),			F_FLOAT},
        {"ammo_rockets",		FOFS( s.v.ammo_rockets ),		F_FLOAT},
        {"ammo_cells",			FOFS( s.v.ammo_cells ),			F_FLOAT},
        {"a_s",				FOFS( s.v.ammo_shells ),		F_FLOAT},
        {"a_n",				FOFS( s.v.ammo_nails ),			F_FLOAT},
        {"a_r",				FOFS( s.v.ammo_rockets ),		F_FLOAT},
        {"a_c",				FOFS( s.v.ammo_cells ),			F_FLOAT},

        {"items",			FOFS( s.v.items ),			F_FLOAT},
        {"deadflag",			FOFS( s.v.deadflag ),			F_FLOAT},
        {"button0",			FOFS( s.v.button0 ),			F_FLOAT},
        {"button1",			FOFS( s.v.button1 ),			F_FLOAT},
        {"button2",			FOFS( s.v.button2 ),			F_FLOAT},
        {"impulse",			FOFS( s.v.impulse ),			F_FLOAT},
        {"fixangle",			FOFS( s.v.fixangle ),			F_FLOAT},
        {"flags",			FOFS( s.v.flags ),			F_FLOAT},
        {"colormap",			FOFS( s.v.colormap ),			F_FLOAT},
        {"team",			FOFS( s.v.team ),			F_FLOAT},
        {"max_health",			FOFS( s.v.max_health ),			F_FLOAT},
        {"teleport_time",		FOFS( s.v.teleport_time ),		F_FLOAT},
        {"armortype",			FOFS( s.v.armortype ),			F_FLOAT},
        {"armorvalue",			FOFS( s.v.armorvalue ),			F_FLOAT},
        {"waterlevel",			FOFS( s.v.waterlevel ),			F_FLOAT},
        {"watertype",			FOFS( s.v.watertype ),			F_FLOAT},
        {"ideal_yaw",			FOFS( s.v.ideal_yaw ),			F_FLOAT},
        {"yaw_speed",			FOFS( s.v.yaw_speed ),			F_FLOAT},
        {"dmg_take",			FOFS( s.v.dmg_take ),			F_FLOAT},
        {"dmg_save",			FOFS( s.v.dmg_save ),			F_FLOAT},
        {"hook_out",			FOFS( hook_out ),			F_INT},
        {NULL}
};
typedef struct {
	char           *name;
	void            ( *spawn ) ();
} spawn_t;

void SUB_Remove()
{
	ent_remove( self );
}

void SUB_Null()
{
}

void            SP_light();
void            SP_light_fluoro();
void            SP_light_fluorospark();
void            SP_light_globe();
void            SP_light_torch_small_walltorch();
void            SP_light_flame_large_yellow();
void            SP_light_flame_small_yellow();
void            SP_light_flame_small_white();

void            SP_ambient_suck_wind();
void            SP_ambient_drone();
void            SP_ambient_flouro_buzz();
void            SP_ambient_drip();
void            SP_ambient_comp_hum();
void            SP_ambient_thunder();
void            SP_ambient_light_buzz();
void            SP_ambient_swamp1();
void            SP_ambient_swamp2();
void            SP_misc_noisemaker();
void            SP_misc_explobox();
void            SP_misc_explobox2();
void            SP_air_bubbles();

void            SP_trap_spikeshooter();
void            SP_trap_shooter();
void            SP_func_wall();
void            SP_func_illusionary();
void            SP_func_episodegate();
void            SP_func_bossgate();

void            SP_func_door();
void            SP_func_door_secret();
void            SP_func_plat();
void            SP_func_train();
void            SP_misc_teleporttrain();
void            SP_func_button();

void            SP_trigger_multiple();
void            SP_trigger_once();
void            SP_trigger_relay();
void            SP_trigger_secret();
void            SP_trigger_counter();
void            SP_info_teleport_destination();
void            SP_trigger_teleport();
void            SP_trigger_setskill();
void            SP_trigger_onlyregistered();
void            SP_trigger_hurt();
void            SP_trigger_push();
void            SP_trigger_monsterjump();
void            SP_trigger_changelevel();
void            SP_path_corner();

void            SP_item_health();
void            SP_item_armor1();
void            SP_item_armor2();
void            SP_item_armorInv();
void            SP_weapon_supershotgun();
void            SP_weapon_nailgun();
void            SP_weapon_supernailgun();
void            SP_weapon_grenadelauncher();
void            SP_weapon_rocketlauncher();
void            SP_weapon_lightning();
void            SP_item_shells();
void            SP_item_spikes();
void            SP_item_rockets();
void            SP_item_cells();
void            SP_item_weapon();

void            SP_item_artifact_invulnerability();
void            SP_item_artifact_envirosuit();
void            SP_item_artifact_invisibility();
void            SP_item_artifact_super_damage();
void            SP_item_sigil();
void            SP_item_key1();
void            SP_item_key2();

void            SP_misc_fireball();
void            SP_info_intermission();
void		SP_Null_tf_spawn();
//TF entites
void 		SP_noclass();
void 		SP_info_tfdetect();

void SP_info_tfdetect();
void SP_info_player_teamspawn();
void SP_i_p_t();
void SP_info_tfgoal();
void SP_i_t_g();
void SP_info_tfgoal_timer();
void SP_i_t_t();
void SP_item_tfgoal();
void SP_info_player_team1();
void SP_info_player_team2();
void SP_item_flag_team2();
void SP_item_flag_team1();

static const spawn_t         spawns[] = {
	// info entities don't do anything at all, but provide positional
	// information for things controlled by other processes
	{"info_player_start",		SP_Null_tf_spawn},
	{"info_player_start2",		SP_Null_tf_spawn},
	{"info_player_deathmatch", 	SP_Null_tf_spawn},
	{"info_player_coop", 		SP_Null_tf_spawn},
	{"testplayerstart", 		SP_Null_tf_spawn},
	{"info_intermission", 		SP_info_intermission},
	{"trigger_changelevel", 	SP_trigger_changelevel},
/*QUAKED info_notnull (0 0.5 0) (-4 -4 -4) (4 4 4)
Used as a positional target for lightning.
*/
	{"info_notnull", 		SUB_Null},

	{"light", 			SP_light},
	{"light_fluoro", 		SP_light_fluoro},
	{"light_fluorospark", 		SP_light_fluorospark},
	{"light_globe", 		SP_light_globe},
	{"light_torch_small_walltorch", SP_light_torch_small_walltorch},
	{"light_flame_large_yellow", 	SP_light_flame_large_yellow},
	{"light_flame_small_yellow", 	SP_light_flame_small_yellow},
	{"light_flame_small_white", 	SP_light_flame_small_white},
	{"air_bubbles", 		SP_air_bubbles},

	{"misc_fireball",		SP_misc_fireball},

	{"ambient_suck_wind",		SP_ambient_suck_wind},
	{"ambient_drone",		SP_ambient_drone},
	{"ambient_flouro_buzz",		SP_ambient_flouro_buzz},
	{"ambient_drip",		SP_ambient_drip},
	{"ambient_comp_hum",		SP_ambient_comp_hum},
	{"ambient_thunder",		SP_ambient_thunder},
	{"ambient_light_buzz",		SP_ambient_light_buzz},
	{"ambient_swamp1",		SP_ambient_swamp1},
	{"ambient_swamp2",		SP_ambient_swamp2},
	{"misc_noisemaker",		SP_misc_noisemaker},
	{"misc_explobox",		SP_misc_explobox},
	{"misc_explobox2",		SP_misc_explobox2},
	{"trap_spikeshooter",		SP_trap_spikeshooter},
	{"trap_shooter",		SP_trap_shooter},
	{"func_wall",			SP_func_wall},
	{"func_illusionary",		SP_func_illusionary},
	{"func_episodegate",		SP_func_episodegate},
	{"func_bossgate",		SP_func_bossgate},


	{"func_door",			SP_func_door},
	{"func_door_secret",		SP_func_door_secret},
	{"func_plat",			SP_func_plat},
	{"func_train",			SP_func_train},
	{"misc_teleporttrain",		SP_misc_teleporttrain},
	{"func_button",			SP_func_button},

	{"trigger_multiple",		SP_trigger_multiple},
	{"trigger_once",		SP_trigger_once},
	{"trigger_relay",		SP_trigger_relay},
	{"trigger_secret",		SP_trigger_secret},
	{"trigger_counter",		SP_trigger_counter},
	{"info_teleport_destination",	SP_info_teleport_destination},
	{"trigger_teleport",		SP_trigger_teleport},
	{"trigger_setskill",		SP_trigger_setskill},
	{"trigger_onlyregistered",	SP_trigger_onlyregistered},
	{"trigger_hurt",		SP_trigger_hurt},
	{"trigger_push",		SP_trigger_push},
	{"trigger_monsterjump",		SP_trigger_monsterjump},
	{"path_corner",			SP_path_corner},

	{"item_health",			SP_item_health},
	{"item_armor1",			SP_item_armor1},
	{"item_armor2",			SP_item_armor2},
	{"item_armorInv",		SP_item_armorInv},
	{"weapon_supershotgun",		SP_weapon_supershotgun},
	{"weapon_nailgun",		SP_weapon_nailgun},
	{"weapon_supernailgun",		SP_weapon_supernailgun},
	{"weapon_grenadelauncher",	SP_weapon_grenadelauncher},
	{"weapon_rocketlauncher",	SP_weapon_rocketlauncher},
	{"weapon_lightning",		SP_weapon_lightning},
	{"item_shells",			SP_item_shells},
	{"item_spikes",			SP_item_spikes},
	{"item_rockets",		SP_item_rockets},
	{"item_cells",			SP_item_cells},
	{"item_weapon",			SP_item_weapon},
	{"item_artifact_invulnerability",	SP_item_artifact_invulnerability},
	{"item_artifact_envirosuit",		SP_item_artifact_envirosuit},
	{"item_artifact_invisibility",		SP_item_artifact_invisibility},
	{"item_artifact_super_damage",		SP_item_artifact_super_damage},
	{"item_sigil",			SP_item_sigil},
	{"item_key1",			SP_item_key1},
	{"item_key2",			SP_item_key2},

//not used ents
/*QUAKED info_null (0 0.5 0) (-4 -4 -4) (4 4 4)
Used as a positional target for spotlights, etc.
*/
	{"info_null",			SUB_Remove},
	{"monster_ogre",		SUB_Remove},
	{"monster_demon1",		SUB_Remove},
	{"monster_shambler",		SUB_Remove},
	{"monster_knight",		SUB_Remove},
	{"monster_army",		SUB_Remove},
	{"monster_wizard",		SUB_Remove},
	{"monster_dog",			SUB_Remove},
	{"monster_zombie",		SUB_Remove},
	{"monster_boss",		SUB_Remove},
	{"monster_tarbaby",		SUB_Remove},
	{"monster_hell_knight",		SUB_Remove},
	{"monster_fish",		SUB_Remove},
	{"monster_shalrath",		SUB_Remove},
	{"monster_enforcer",		SUB_Remove},
	{"monster_oldone",		SUB_Remove},
	{"monster_dragon",		SUB_Remove},
	{"monster_vomit",		SUB_Remove},
	{"event_lightning",		SUB_Remove},
//TF entites
	{"noclass",			SP_noclass},
	{"info_tfdetect",         	SP_info_tfdetect},
	{"info_player_teamspawn", 	SP_info_player_teamspawn},
	{"i_p_t",         	  	SP_i_p_t},
	{"info_tfgoal",         	SP_info_tfgoal},
	{"i_t_g",         		SP_i_t_g},
	{"info_tfgoal_timer",         	SP_info_tfgoal_timer},
	{"i_t_t",	         	SP_i_t_t},
	{"item_tfgoal",         	SP_item_tfgoal},
	{"info_player_team1",         	SP_info_player_team1},
	{"info_player_team2",         	SP_info_player_team2},
	{"item_flag_team2",         	SP_item_flag_team2},
	{"item_flag_team1",         	SP_item_flag_team1},
	{0, 0}
};

/*
===============
G_CallSpawn

Finds the spawn function for the entity and calls it,
returning qfalse if not found
===============
*/
qboolean G_CallSpawn( gedict_t * ent )
{
	const spawn_t        *s;

//      gitem_t *item;

	if ( !ent->s.v.classname )
	{
		G_dprintf( "G_CallSpawn: NULL classname\n" );
		return false;
	}

/*	// check item spawn functions
	for ( item=bg_itemlist+1 ; item->classname ; item++ ) {
		if ( !strcmp(item->classname, ent->classname) ) {
			G_SpawnItem( ent, item );
			return qtrue;
		}
	}*/

	// check normal spawn functions
	for ( s = spawns; s->name; s++ )
	{
		if ( !strcmp( s->name, ent->s.v.classname ) )
		{
			// found it
			self = ent;
			//G_Printf("%8i %s\n",ent->s.v.classname,ent->s.v.classname);
			s->spawn();
			return true;
		}
	}
	G_dprintf( "%s doesn't have a spawn function\n", ent->s.v.classname );
	return false;
}

/*
=============
G_NewString

Builds a copy of the string, translating \n to real linefeeds
so message texts can be multi-line
=============
*/
char           *G_NewString( const char *string )
{
	char           *newb, *new_p;
	int             i, l;

	l = strlen( string ) + 1;

	newb = G_Alloc( l );
	//newb = malloc( l );

	new_p = newb;

	// turn \n into a real linefeed
	for ( i = 0; i < l; i++ )
	{
		if ( string[i] == '\\' && i < l - 1 )
		{
			i++;
			if ( string[i] == 'n' )
			{
				*new_p++ = '\n';
			} else
			{
				*new_p++ = '\\';
			}
		} else
		{
			*new_p++ = string[i];
		}
	}

	return newb;
}

/*
===============
G_ParseField

Takes a key/value pair and sets the binary values
in a gentity
===============
*/
void G_ParseField( const char *key, const char *value, gedict_t * ent )
{
	const field_t        *f;
	byte           *b;
	float           v;
	vec3_t          vec;

	for ( f = fields; f->name; f++ )
	{
		if ( !Q_stricmp( f->name, key ) )
		{
			// found it
			b = ( byte * ) ent;

			switch ( f->type )
			{
			case F_LSTRING:
				*( char ** ) ( b + f->ofs ) = G_NewString( value );
				break;
			case F_VECTOR:
				sscanf( value, "%f %f %f", &vec[0], &vec[1], &vec[2] );
				( ( float * ) ( b + f->ofs ) )[0] = vec[0];
				( ( float * ) ( b + f->ofs ) )[1] = vec[1];
				( ( float * ) ( b + f->ofs ) )[2] = vec[2];
				break;
			case F_INT:
				*( int * ) ( b + f->ofs ) = atoi( value );
				break;
			case F_FLOAT:
				*( float * ) ( b + f->ofs ) = atof( value );
				break;
			case F_ANGLEHACK:
				v = atof( value );
				( ( float * ) ( b + f->ofs ) )[0] = 0;
				( ( float * ) ( b + f->ofs ) )[1] = v;
				( ( float * ) ( b + f->ofs ) )[2] = 0;
				break;
			default:
			case F_IGNORE:
				break;
			}
			return;
		}
	}
	G_dprintf( "unknown field: %s\n", key );
}

/*
===================
G_SpawnGEntityFromSpawnVars

Spawn an entity and fill in all of the level fields from
level.spawnVars[], then call the class specfic spawn function
===================
*/
void G_SpawnGEntityFromSpawnVars( void )
{
	int             i;
	gedict_t       *ent;

	// get the next free entity
	ent = spawn();

	for ( i = 0; i < numSpawnVars; i++ )
	{
		G_ParseField( spawnVars[i][0], spawnVars[i][1], ent );
	}

	if ( ( ( int ) ent->s.v.spawnflags & SPAWNFLAG_NOT_DEATHMATCH ) )
	{
		ent_remove( ent );
		return;
	}

	// if we didn't get a classname, don't bother spawning anything
	if ( !G_CallSpawn( ent ) )
	{
		ent_remove( ent );
		return;
	}
//      if(!ent->s.v.classname)ent->s.v.classname="";
}


/*
====================
G_AddSpawnVarToken
====================
*/
char           *G_AddSpawnVarToken( const char *string )
{
	int             l;
	char           *dest;

	l = strlen( string );
	if ( numSpawnVarChars + l + 1 > MAX_SPAWN_VARS_CHARS )
	{
		G_Error( "G_AddSpawnVarToken: MAX_SPAWN_CHARS" );
	}

	dest = spawnVarChars + numSpawnVarChars;
	memcpy( dest, string, l + 1 );

	numSpawnVarChars += l + 1;

	return dest;
}

/*
====================
G_ParseSpawnVars

Parses a brace bounded set of key / value pairs out of the
level's entity strings into level.spawnVars[]

This does not actually spawn an entity.
====================
*/
qboolean G_ParseSpawnVars( void )
{
	char            keyname[MAX_TOKEN_CHARS];
	char            com_token[MAX_TOKEN_CHARS];

	numSpawnVars = 0;
	numSpawnVarChars = 0;

	// parse the opening brace
	if ( !trap_GetEntityToken( com_token, sizeof( com_token ) ) )
	{
		// end of spawn string
		return false;
	}
	if ( com_token[0] != '{' )
	{
		G_Error( "G_ParseSpawnVars: found %s when expecting {", com_token );
	}
	// go through all the key / value pairs
	while ( 1 )
	{
		// parse key
		if ( !trap_GetEntityToken( keyname, sizeof( keyname ) ) )
		{
			G_Error( "G_ParseSpawnVars: EOF without closing brace" );
		}

		if ( keyname[0] == '}' )
		{
			break;
		}
		// parse value  
		if ( !trap_GetEntityToken( com_token, sizeof( com_token ) ) )
		{
			G_Error( "G_ParseSpawnVars: EOF without closing brace" );
		}
//              G_Printf("%s\t%s\n",keyname,com_token);
		if ( com_token[0] == '}' )
		{
			G_Error( "G_ParseSpawnVars: closing brace without data" );
		}
		if ( numSpawnVars == MAX_SPAWN_VARS )
		{
			G_Error( "G_ParseSpawnVars: MAX_SPAWN_VARS" );
		}
		spawnVars[numSpawnVars][0] = G_AddSpawnVarToken( keyname );
		spawnVars[numSpawnVars][1] = G_AddSpawnVarToken( com_token );
		numSpawnVars++;
	}

	return true;
}

void            SP_worldspawn();

/*
==============
G_SpawnEntitiesFromString

Parses textual entity definitions out of an entstring and spawns gentities.
==============
*/

void G_SpawnEntitiesFromString( void )
{
	int             i;

	// the worldspawn is not an actual entity, but it still
	// has a "spawn" function to perform any global setup
	// needed by a level (setting configstrings or cvars, etc)
	if ( !G_ParseSpawnVars() )
	{
		G_Error( "SpawnEntities: no entities" );
	}
	self = world;
	for ( i = 0; i < numSpawnVars; i++ )
	{
		G_ParseField( spawnVars[i][0], spawnVars[i][1], world );
	}

	SP_worldspawn( world );

	// parse ents

	while ( G_ParseSpawnVars() )
	{
		G_SpawnGEntityFromSpawnVars();
		trap_FlushSignon();
	}
}
