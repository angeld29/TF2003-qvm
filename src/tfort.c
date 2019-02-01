/*
 *  QWProgs-TF2003
 *  Copyright (C) 2004  [sd] angel
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
 *  $Id: tfort.c,v 1.50 2006-11-29 23:19:23 AngelD Exp $
 */
#include "g_local.h"

tf_server_data_t tf_data;
int FLAME_MAXWORLDNUM, MAX_WORLD_PIPEBOMBS, MAX_WORLD_AMMOBOXES, GR_TYPE_MIRV_NO, GR_TYPE_NAPALM_NO;


static int     num_team_ammoboxes[5] = { 0, 0, 0, 0, 0 };

typedef struct class_settings_s{
	int     bitmask;
	int     maxhealth, maxspeed, maxstrafespeed;
	int     weapons_carried, current_weapon;
	int     ammo_shells, ammo_nails, ammo_rockets, ammo_cells, ammo_detpack, ammo_medikit;
	int     maxammo_shells, maxammo_nails, maxammo_rockets, 
		maxammo_cells, maxammo_detpack, maxammo_medikit;
	int     no_grenades_1, no_grenades_2;
	int     tp_grenades_1, tp_grenades_2;
	int     og_tp_grenades_1, og_tp_grenades_2;
	int     tf_items, tf_items_flags;
	int     armorclass, armorvalue, maxarmor;
	float   armortype, armor_allowed;
	int     items_allowed, items;
	const   char   *name;
	const   char   *defaultskin;
	const   char   *infokey4skin[4];
}class_settings_t;


const class_settings_t class_set[] = {
	{
	 0,
	 1, 0, 0,
	 0, 0,
	 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0,
	 0, 0,
	 0, 0, 0, 0, 0,
	 0, 0,
	 "Observer", "base", /*"base",*/ {NULL, NULL, NULL, NULL}
	 },
	{
	 1,			//bitmask
	 75, 450, 450,		//maxhelth ,speeds
	 WEAP_AXE | WEAP_SHOTGUN | WEAP_NAILGUN, WEAP_NAILGUN,	//weapon: carried, start
	 25, 100, 0, 50, 0, 0,	// ammo
	 50, 200, 25, 100, 0, 0,	//maxammo
	 2, 3,			//num grens
	 GR_TYPE_CALTROPS, GR_TYPE_CONCUSSION,	//newgren types
	 GR_TYPE_FLASH, GR_TYPE_CONCUSSION,	//oldgren types
	 NIT_SCANNER, NIT_SCANNER_ENEMY,	//tf_items,tf_items_flags
	 0, 25, 50,		//AC, val,max
	 0.3, 0.3,		//AT ,Aallow
	 WEAP_AXE | WEAP_SHOTGUN | WEAP_NAILGUN,	//items allowed
	 IT_SHOTGUN | IT_NAILGUN,	//items
	 "Scout", "tf_scout", //"tf_scout",
	 {"sk_t1_scout", "sk_t2_scout", "sk_t3_scout", "sk_t4_scout"}
	 },
	{
	 2,			//bitmask
	 90, 300, 300,		//maxhelth ,speeds
	 WEAP_SNIPER_RIFLE | WEAP_AUTO_RIFLE | WEAP_AXE | WEAP_NAILGUN,
	 WEAP_SNIPER_RIFLE,	//weapon: carried, start
	 60, 50, 0, 0, 0, 0,	// ammo
	 75, 100, 25, 50, 0, 0,	//maxammo
	 2, 3,			//num grens
	 GR_TYPE_NORMAL, GR_TYPE_FLARE,	//newgren types
	 GR_TYPE_NORMAL, GR_TYPE_FLARE,	//oldgren types
	 0, 0,			//tf_items,tf_items_flags
	 0, 0, 50,		//AC, val,max
	 0.3, 0.3,		//AT ,Aallow
	 WEAP_SNIPER_RIFLE | WEAP_AUTO_RIFLE | WEAP_AXE | WEAP_NAILGUN,	//items allowed
	 IT_SHOTGUN | IT_SUPER_SHOTGUN | IT_NAILGUN,	//items
	 "Sniper", "tf_snipe", //"tf_snipe",
	 {"sk_t1_sniper", "sk_t2_sniper", "sk_t3_sniper", "sk_t4_sniper"}
	 },
	{
	 4,			//bitmask
	 100, 240, 240,		//maxhelth ,speeds
	 WEAP_AXE | WEAP_SHOTGUN | WEAP_SUPER_SHOTGUN | WEAP_ROCKET_LAUNCHER,
	 WEAP_ROCKET_LAUNCHER,	//weapon: carried, start
	 50, 0, 10, 0, 0, 0,	// ammo
	 100, 100, 50, 50, 0, 0,	//maxammo
	 4, 1,			//num grens
	 GR_TYPE_NORMAL, GR_TYPE_NAIL,	//newgren types
	 GR_TYPE_NORMAL, GR_TYPE_NAIL,	//oldgren types
	 0, 0,			//tf_items,tf_items_flags
	 0, 100, 200,		//AC, val,max
	 0.8, 0.8,		//AT ,Aallow
	 WEAP_AXE | WEAP_SHOTGUN | WEAP_SUPER_SHOTGUN | WEAP_ROCKET_LAUNCHER,	//items allowed
	 IT_SHOTGUN | IT_SUPER_SHOTGUN | IT_ROCKET_LAUNCHER,	//items
	 "Soldier", "tf_sold", //"tf_sold",
	 {"sk_t1_soldier", "sk_t2_soldier", "sk_t3_soldier", "sk_t4_soldier"}
	 },
	{
	 8,			//bitmask
	 90, 280, 280,		//maxhelth ,speeds
	 WEAP_AXE | WEAP_SHOTGUN | WEAP_GRENADE_LAUNCHER | WEAP_DETPACK,
	 WEAP_GRENADE_LAUNCHER,	//weapon: carried, start
	 30, 0, 20, 0, 1, 0,	// ammo
	 75, 50, 50, 50, 1, 0,	//maxammo
	 4, 4,			//num grens
	 GR_TYPE_NORMAL, GR_TYPE_MIRV,	//newgren types
	 GR_TYPE_NORMAL, GR_TYPE_MIRV,	//oldgren types
	 0, 0,			//tf_items,tf_items_flags
	 0, 50, 120,		//AC, val,max
	 0.6, 0.6,		//AT ,Aallow
	 WEAP_AXE | WEAP_SHOTGUN | WEAP_GRENADE_LAUNCHER | WEAP_DETPACK,	//items allowed
	 IT_SHOTGUN | IT_GRENADE_LAUNCHER,	//items
	 "Demolitions Man", "tf_demo", //"tf_demo",
	 {"sk_t1_demoman", "sk_t2_demoman", "sk_t3_demoman", "sk_t4_demoman"}
	 },
	{
	 16,			//bitmask
	 90, 320, 320,		//maxhelth ,speeds
	 WEAP_BIOWEAPON | WEAP_MEDIKIT | WEAP_SHOTGUN | WEAP_SUPER_SHOTGUN | WEAP_SUPER_NAILGUN,
	 WEAP_SUPER_NAILGUN,	//weapon: carried, start
	 50, 50, 0, 0, 0, 50,	// ammo
	 75, 150, 25, 50, 0, 100,	//maxammo
	 3, 2,			//num grens
	 GR_TYPE_NORMAL, GR_TYPE_CONCUSSION,	//newgren types
	 GR_TYPE_NORMAL, GR_TYPE_CONCUSSION,	//oldgren types
	 0, 0,			//tf_items,tf_items_flags
	 0, 50, 100,		//AC, val,max
	 0.3, 0.6,		//AT ,Aallow
	 WEAP_BIOWEAPON | WEAP_MEDIKIT | WEAP_SHOTGUN | WEAP_SUPER_SHOTGUN | WEAP_SUPER_NAILGUN,	//items allowed
	 IT_SHOTGUN | IT_SUPER_SHOTGUN | IT_SUPER_NAILGUN,	//items
	 "Combat Medic", "tf_medic", //"tf_medic",
	 {"sk_t1_medic", "sk_t2_medic", "sk_t3_medic", "sk_t4_medic"}
	 },
	{
	 32,			//bitmask
	 100, 230, 230,		//maxhelth ,speeds
	 WEAP_ASSAULT_CANNON | WEAP_AXE | WEAP_SHOTGUN | WEAP_SUPER_SHOTGUN,
	 WEAP_ASSAULT_CANNON,	//weapon: carried, start
	 200, 0, 0, 24, 0, 0,	// ammo
	 200, 200, 25, 50, 0, 0,	//maxammo
	 4, 1,			//num grens
	 GR_TYPE_NORMAL, GR_TYPE_MIRV,	//newgren types
	 GR_TYPE_NORMAL, GR_TYPE_MIRV,	//oldgren types
	 0, 0,			//tf_items,tf_items_flags
	 0, 150, 300,		//AC, val,max
	 0.8, 0.8,		//AT ,Aallow
	 WEAP_ASSAULT_CANNON | WEAP_AXE | WEAP_SHOTGUN | WEAP_SUPER_SHOTGUN,	//items allowed
	 IT_SHOTGUN | IT_SUPER_SHOTGUN | IT_ROCKET_LAUNCHER,	//items
	 "Heavy Weapons Guy", "tf_hwguy", //"tf_hwguy",
	 {"sk_t1_hwguy", "sk_t2_hwguy", "sk_t3_hwguy", "sk_t4_hwguy"}
	 },
	{
	 64,			//bitmask
	 100, 300, 300,		//maxhelth ,speeds
	 WEAP_INCENDIARY | WEAP_FLAMETHROWER | WEAP_AXE | WEAP_SHOTGUN,
	 WEAP_INCENDIARY,	//weapon: carried, start
	 20, 0, 15, 120, 0, 0,	// ammo
	 40, 50, 60, 200, 0, 0,	//maxammo
	 1, 4,			//num grens
	 GR_TYPE_NORMAL, GR_TYPE_NAPALM,	//newgren types
	 GR_TYPE_NORMAL, GR_TYPE_NAPALM,	//oldgren types
	 0, 0,			//tf_items,tf_items_flags
	 AT_SAVEFIRE, 50, 150,	//AC, val,max
	 0.6, 0.6,		//AT ,Aallow
	 WEAP_INCENDIARY | WEAP_FLAMETHROWER | WEAP_AXE | WEAP_SHOTGUN,	//items allowed
	 IT_SHOTGUN | IT_GRENADE_LAUNCHER | IT_ROCKET_LAUNCHER,	//items
	 "Pyro", "tf_pyro", //"tf_pyro",
	 {"sk_t1_pyro", "sk_t2_pyro", "sk_t3_pyro", "sk_t4_pyro"}
	 },
	{
	 256,			//bitmask
	 90, 300, 300,		//maxhelth ,speeds
	 WEAP_AXE | WEAP_TRANQ | WEAP_SUPER_SHOTGUN | WEAP_NAILGUN,
	 WEAP_TRANQ,		//weapon: carried, start
	 40, 50, 0, 10, 0, 0,	// ammo
	 40, 100, 15, 30, 0, 0,	//maxammo
	 2, 2,			//num grens
	 GR_TYPE_NORMAL, GR_TYPE_GAS,	//newgren types
	 GR_TYPE_NORMAL, GR_TYPE_GAS,	//oldgren types
	 0, 0,			//tf_items,tf_items_flags
	 0, 25, 70,		//AC, val,max
	 0.3, 0.6,		//AT ,Aallow
	 WEAP_AXE | WEAP_TRANQ | WEAP_SUPER_SHOTGUN | WEAP_NAILGUN,	//items allowed
	 IT_SHOTGUN | IT_SUPER_SHOTGUN | IT_NAILGUN,	//items
	 "Spy", "tf_spy", //"tf_spy",
	 {"sk_t1_spy", "sk_t2_spy", "sk_t3_spy", "sk_t4_spy"}
	 },
	{
	 512,			//bitmask
	 80, 300, 300,		//maxhelth ,speeds
	 WEAP_SPANNER | WEAP_LASER | WEAP_SUPER_SHOTGUN,
	 WEAP_LASER,		//weapon: carried, start
	 20, 25, 0, 100, 0, 0,	// ammo
	 50, 50, 30, 200, 0, 0,	//maxammo
	 2, 2,			//num grens
	 GR_TYPE_NORMAL, GR_TYPE_EMP,	//newgren types
	 GR_TYPE_NORMAL, GR_TYPE_EMP,	//oldgren types
	 0, 0,			//tf_items,tf_items_flags
	 0, 25, 50,		//AC, val,max
	 0.3, 0.6,		//AT ,Aallow
	 WEAP_SPANNER | WEAP_LASER | WEAP_SUPER_SHOTGUN,	//items allowed
	 IT_SHOTGUN | IT_SUPER_SHOTGUN,	//items
	 "Engineer", "tf_eng", //"tf_eng",
	 {"sk_t1_engineer", "sk_t2_engineer", "sk_t3_engineer", "sk_t4_engineer"}
	 },
	{
	 128,			//bitmask
	 0, 0, 0,		//maxhelth ,speeds
	 0,
	 0,			//weapon: carried, start
	 0, 0, 0, 0, 0, 0,	// ammo
	 0, 0, 0, 0, 0, 0,	//maxammo
	 0, 0,			//num grens
	 0, 0,			//newgren types
	 0, 0,			//oldgren types
	 0, 0,			//tf_items,tf_items_flags
	 0, 0, 0,		//AC, val,max
	 0, 0,			//AT ,Aallow
	 0,			//items allowed
	 0,			//items
	 "Random Playerclass", "",// "",
	 {NULL, NULL, NULL, NULL}
	 },
	{
	 0,			//bitmask
	 50, 240, 240,		//maxhelth ,speeds
	 WEAP_AXE,
	 WEAP_AXE,		//weapon: carried, start
	 0, 0, 0, 0, 0, 0,	// ammo
	 0, 0, 0, 0, 0, 0,	//maxammo
	 0, 0,			//num grens
	 0, 0,			//newgren types
	 0, 0,			//oldgren types
	 0, 0,			//tf_items,tf_items_flags
	 0, 0, 0,		//AC, val,max
	 0, 0,			//AT ,Aallow
	 WEAP_AXE,		//items allowed
	 0,			//items
	 "Civilian", "base", //"base",
	 {NULL, NULL, NULL, NULL}
	 }

};

const char   *GrenadeNames[] = { "BUG",
	"Normal",
	"Concussion",
	"Nail",
	"Mirv",
	"Napalm",
	"Flare",
	"Hallucinogenic",
	"EMP",
	"Flash",
	"Caltrop",
	"Flash"
};

const char   *GrenadePrimeName[] = { "Grenade",
	"Grenade",
	"Concussion grenade",
	"Nail grenade",
	"Mirv grenade",
	"Napalm grenade",
	"Flare",
	"Gas grenade",
	"EMP grenade",
	"Flash grenade",
	"Caltrop canister",
	"Flash grenade"
};

const int     classbits[] = { 0, 1, 2, 4, 8, 16, 32, 64, 256, 512, 128 };

//=========================================================================
// IMPULSE FUNCTIONS
//=========================================================================
// Uses a class dependant special skill
void UseSpecialSkill(  )
{
	vec3_t  src;
	gedict_t *ent;

	self->s.v.impulse = 0;

	switch ( self->playerclass )
	{
	case PC_SCOUT:
		self->s.v.impulse = TF_SCAN_30;
		break;

	case PC_SNIPER:
		self->s.v.impulse = TF_AUTOZOOM;
		break;

	case PC_SOLDIER:
		if ( self->current_weapon != WEAP_ROCKET_LAUNCHER && self->s.v.ammo_rockets > 0 )
			self->s.v.impulse = 7;	//RL
		else
			self->s.v.impulse = 3;	//SSG
		break;

	case PC_DEMOMAN:
		self->s.v.impulse = TF_PB_DETONATE;
		break;

	case PC_MEDIC:
		if ( self->current_weapon != WEAP_MEDIKIT )
			self->s.v.impulse = TF_MEDIKIT;
		else if ( self->s.v.ammo_nails > 0 )
			self->s.v.impulse = 5;
		else
			self->s.v.impulse = 3;
		break;

	case PC_HVYWEAP:
		if ( self->current_weapon != WEAP_SUPER_SHOTGUN )
			self->s.v.impulse = 7;	//ASSAULT
		else
			self->s.v.impulse = 3;	//SSG
		break;

	case PC_PYRO:
		if ( (self->current_weapon != WEAP_INCENDIARY) && (self->s.v.ammo_rockets != 0) )
			self->s.v.impulse = 7;
		else
			self->s.v.impulse = 6;
		break;
	case PC_SPY:
		self->s.v.impulse = TF_SPY_SPY;
		break;

	case PC_ENGINEER:
		self->s.v.impulse = TF_ENGINEER_BUILD;
		break;
	default:
		if ( !self->s.v.enemy || self->s.v.enemy == EDICT_TO_PROG( world ) )
		{
			src[0] = self->s.v.origin[0] + g_globalvars.v_forward[0] * 10;
			src[1] = self->s.v.origin[0] + g_globalvars.v_forward[0] * 10;
			src[2] = self->s.v.absmin[2] + self->s.v.size[2] * 0.7;
			traceline( PASSVEC3( src ),
				   src[0] + g_globalvars.v_forward[0] * 2048,
				   src[1] + g_globalvars.v_forward[1] * 2048,
				   src[2] + g_globalvars.v_forward[2] * 2048, 0, self );
			ent = PROG_TO_EDICT( g_globalvars.trace_ent );
			if ( ent != world && !VectorCompare( ent->s.v.origin, world->s.v.origin ) )
			{
				G_sprint( self, 2, "Locked onto %s\n", ent->s.v.classname );
				self->s.v.enemy = g_globalvars.trace_ent;
				VectorSubtract( PROG_TO_EDICT( self->s.v.enemy )->s.v.origin, self->s.v.origin, src );
				self->camdist = vlen( src );
				VectorSubtract( self->s.v.origin, PROG_TO_EDICT( self->s.v.enemy )->s.v.origin,
						self->camangle );
				self->camangle[2] = -self->camangle[2];
				vectoangles( self->camangle, self->camangle );
			}
		} else
		{
			G_sprint( self, 2, "Removed Lock\n" );
			self->s.v.enemy = EDICT_TO_PROG( world );
		}
		break;
	}

}

void TeamFortress_ChangeClass(  )
{
	gedict_t *spot;
	gedict_t *te;
	vec3_t  v;

	if ( self->playerclass )
	{
		if ( deathmatch != 3 && tf_data.cb_prematch_time < g_globalvars.time )
			return;
		if ( TeamFortress_TeamIsCivilian( self->team_no ) )
		{
			G_sprint( self, 2, "You cannot change class.\n" );
			return;
		}
		if ( !IsLegalClass( self->s.v.impulse - TF_CHANGEPC ) )
		{
			G_sprint( self, 2, "Your team cannot play that class.\n" );
			TeamFortress_DisplayLegalClasses(  );
			return;
		}
		if ( tf_data.spy_off == 1 && self->s.v.impulse - TF_CHANGEPC == 8 )
		{
			G_sprint( self, 2, "The spy class has been disabled on the server by the administrator.\n" );
			return;
		}
		if ( ClassIsRestricted( self->team_no, self->s.v.impulse - TF_CHANGEPC ) )
		{
			G_sprint( self, 2, "Your team already has enough of that class.\n" );
			return;
		}
		self->nextpc = self->s.v.impulse - TF_CHANGEPC;
		G_sprint( self, 2, "After dying, you will return as a " );
		TeamFortress_PrintClassName( self, self->nextpc, self->tfstate & 8 );
		if ( self->s.v.deadflag )
			self->immune_to_check = g_globalvars.time + 10;
		return;
	}
	if ( teamplay && !self->team_no )
	{
		if ( tf_data.toggleflags & TFLAG_AUTOTEAM )
		{
			if ( !TeamFortress_TeamPutPlayerInTeam(  ) )
				return;
		} else
		{
			G_sprint( self, 2, "You must join a team first. \n" );
			return;
		}
	}
	if ( !self->lives )
	{
		G_sprint( self, 2, "You have no lives left.\n" );
		return;
	}
	if ( !IsLegalClass( self->s.v.impulse - TF_CHANGEPC ) && self->s.v.impulse != 1 )
	{
		G_sprint( self, 2, "You cannot play that playerclass on this map. \n" );
		TeamFortress_DisplayLegalClasses(  );
		return;
	}
	if ( tf_data.spy_off == 1 && self->s.v.impulse - TF_CHANGEPC == 8 )
	{
		G_sprint( self, 2, "The spy class has been disabled on the server by the administrator.\n" );
		return;
	}
	if ( ClassIsRestricted( self->team_no, self->s.v.impulse - TF_CHANGEPC ) )
	{
		G_sprint( self, 2, "Your team has enough of that class.\n" );
		return;
	}
	TeamFortress_ExecClassScript( self );

	self->playerclass = ( self->s.v.impulse != 1 ) ? ( self->s.v.impulse - TF_CHANGEPC ) : PC_CIVILIAN;

	self->nextpc = 0;
	self->s.v.takedamage = DAMAGE_AIM;
	self->s.v.movetype = MOVETYPE_WALK;
	self->s.v.flags = FL_CLIENT | FL_ONGROUND;
	self->s.v.waterlevel = 0;
	self->air_finished = g_globalvars.time + 12;
	self->s.v.solid = SOLID_SLIDEBOX;
	self->pausetime = 0;
	spot = SelectSpawnPoint(  );
	VectorCopy( spot->s.v.origin, self->s.v.origin );
	self->s.v.origin[2]++;

	VectorCopy( spot->s.v.angles, self->s.v.angles );
	self->s.v.fixangle = 1;

	setmodel( self, "" );

	modelindex_null = self->s.v.modelindex;

	setmodel( self, "progs/eyes.mdl" );
	modelindex_eyes = self->s.v.modelindex;
	setmodel( self, "progs/player.mdl" );
	modelindex_player = self->s.v.modelindex;
	setsize( self, -16, -16, -24, 16, 16, 32 );
	self->s.v.view_ofs[0] = self->s.v.view_ofs[1] = 0;
	self->s.v.view_ofs[2] = 22;

	player_stand1(  );
	if ( deathmatch || coop )
	{
		trap_makevectors( self->s.v.angles );
		SetVector( v, self->s.v.origin[0] + g_globalvars.v_forward[0] * 20,
			   self->s.v.origin[1] + g_globalvars.v_forward[1] * 20,
			   self->s.v.origin[2] + g_globalvars.v_forward[2] * 20 );
		spawn_tfog( v );
	}
	if ( self->playerclass == PC_RANDOM )
	{
		G_sprint( self, 2, "Random Playerclass.\n" );
		self->tfstate |= 8;
		self->playerclass = 1 + (int)( g_random(  ) * ( 10 - 1 ) );
	}
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
			CenterPrint( self, "%s", spot->s.v.message );
			if ( !( spot->goal_activation & TFSP_MULTIPLEMSGS ) )
				spot->s.v.message = 0;
		}
		if ( spot->activate_goal_no )
		{
			te = Findgoal( spot->activate_goal_no );
			if ( te )
				AttemptToActivate( te, self, spot );
		}
		if ( spot->goal_effects == TFGE_AP )
		{
			spot->s.v.classname = "deadpoint";
			spot->team_str_home = "";
			spot->s.v.nextthink = g_globalvars.time + 1;
			spot->s.v.think = ( func_t ) SUB_Remove;
		}
	}
	
	for( spot = world;( spot = trap_find( spot, FOFS( s.v.classname ), "player" ) ) ;)
	{
		if ( spot->team_no == self->team_no && spot != self )
		{
			G_sprint( spot, 2, "%s is playing as a ", self->s.v.netname );
			TeamFortress_PrintClassName( spot, self->playerclass, self->tfstate & 8 );
		}
	}
	TeamFortress_PrintClassName( self, self->playerclass, self->tfstate & 8 );
	TeamFortress_SetEquipment(  );
	TeamFortress_SetHealth(  );
	TeamFortress_PrepareForArenaRespawn();
	TeamFortress_SetSpeed( self );
	TeamFortress_SetSkin( self );
	TeamFortress_ExecClassScript( self );
	if ( tf_data.cease_fire )
	{
		G_sprint( self, 2, "\n\nCEASE FIRE MODE\n" );
		self->tfstate |= TFSTATE_CANT_MOVE;
		TeamFortress_SetSpeed( self );
	}
}

void TeamFortress_DisplayLegalClasses(  )
{
	int     gotone;
	int     ill;

	G_sprint( self, 2, "Legal Classes for your team are:\n" );
	gotone = 0;
	ill = TeamFortress_TeamGetIllegalClasses( self->team_no );
	if ( !( illegalclasses[0] & 1 ) && !( ill & 1 ) )
	{
		if ( gotone )
			G_sprint( self, 2, ", " );
		gotone = 1;
		G_sprint( self, 2, "Scout" );
	}
	if ( !( illegalclasses[0] & 2 ) && !( ill & 2 ) )
	{
		if ( gotone )
			G_sprint( self, 2, ", " );
		gotone = 1;
		G_sprint( self, 2, "Sniper" );
	}
	if ( !( illegalclasses[0] & 4 ) && !( ill & 4 ) )
	{
		if ( gotone )
			G_sprint( self, 2, ", " );
		gotone = 1;
		G_sprint( self, 2, "Soldier" );
	}
	if ( !( illegalclasses[0] & 8 ) && !( ill & 8 ) )
	{
		if ( gotone )
			G_sprint( self, 2, ", " );
		gotone = 1;
		G_sprint( self, 2, "Demolitions Man" );
	}
	if ( !( illegalclasses[0] & 16 ) && !( ill & 16 ) )
	{
		if ( gotone )
			G_sprint( self, 2, ", " );
		gotone = 1;
		G_sprint( self, 2, "Combat Medic" );
	}
	if ( !( illegalclasses[0] & 32 ) && !( ill & 32 ) )
	{
		if ( gotone )
			G_sprint( self, 2, ", " );
		gotone = 1;
		G_sprint( self, 2, "Heavy Weapons Guy" );
	}
	if ( !( illegalclasses[0] & 64 ) && !( ill & 64 ) )
	{
		if ( gotone )
			G_sprint( self, 2, ", " );
		gotone = 1;
		G_sprint( self, 2, "Pyro" );
	}
	if ( !( illegalclasses[0] & 256 ) && !( ill & 256 ) )
	{
		if ( gotone )
			G_sprint( self, 2, ", " );
		gotone = 1;
		G_sprint( self, 2, "Spy" );
	}
	if ( !( illegalclasses[0] & 512 ) && !( ill & 512 ) )
	{
		if ( gotone )
			G_sprint( self, 2, ", " );
		gotone = 1;
		G_sprint( self, 2, "Engineer" );
	}
	if ( !( illegalclasses[0] & 128 ) && !( ill & 128 ) )
	{
		if ( gotone )
			G_sprint( self, 2, ", " );
		gotone = 1;
		G_sprint( self, 2, "RandomPC" );
	}
	G_sprint( self, 2, "\n" );
}

void TeamFortress_Inventory(  )
{
	gedict_t *tg;
	int     col, grentype;
	gedict_t *te;

	col = TeamFortress_TeamGetColor( self->team_no );
	G_sprint( self, 2, "You're in team %d, color %d.\n", self->team_no, col );
	if ( self->lives != -1 )
	{
		if ( self->lives == 1 )
			G_sprint( self, 2, "You've got %d life.\n", self->lives );
		else
			G_sprint( self, 2, "You've got %d lifes.\n", self->lives );
	}

	if ( self->no_grenades_1 > 0 )
	{
		grentype = self->tp_grenades_1;
		if ( grentype < 0 || grentype > 10 )
			grentype = 0;
		G_sprint( self, 2, "Gren.Type 1 :  %s(%d)\n", GrenadeNames[grentype], self->no_grenades_1 );
	}
	if ( self->no_grenades_2 > 0 )
	{
		grentype = self->tp_grenades_2;
		if ( grentype < 0 || grentype > 10 )
			grentype = 0;
		G_sprint( self, 2, "Gren.Type 2 :  %s(%d)\n", GrenadeNames[grentype], self->no_grenades_2 );
	}

	if ( self->tf_items & 1 )
		G_sprint( self, 2, "Scanner. " );

	if ( self->weapons_carried & WEAP_MEDIKIT )
		G_sprint( self, 2, "Medikit (%d) ", self->ammo_medikit );

	if ( ( self->weapons_carried & WEAP_DETPACK ) && self->ammo_detpack > 0 )
	{
		if ( self->ammo_detpack > 1 )
			G_sprint( self, 2, "%d Detpacks. ", self->ammo_detpack );
		else
			G_sprint( self, 2, "%d Detpack. ", self->ammo_detpack );
	}
	
	for ( tg = world;( tg = trap_find( tg, FOFS( s.v.classname ), "item_tfgoal" ) ) ; )
	{
		if ( tg->s.v.owner == EDICT_TO_PROG( self ) )
		{
			G_sprint( self, 2, "%s. ", tg->s.v.netname );
		}
	}
	if ( self->s.v.armorvalue > 0 )
		TeamFortress_DescribeArmor( self, self->armorclass );

	if ( !tf_data.invis_only )
	{
		if ( self->playerclass == PC_SPY && !tf_data.invis_only )
		{
			G_sprint( self, 2, "Skin : " );
			if ( self->undercover_skin )
				TeamFortress_PrintClassName( self, self->undercover_skin, 0 );
			else
				G_sprint( self, 2, "Spy\n" );

			G_sprint( self, 2, "Colors : Team %d",
				  ( self->undercover_team ) ? self->undercover_team : self->team_no );
		}
	}
	G_sprint( self, 2, "\n" );
	for ( te = world;( te = trap_find( te, FOFS( s.v.classname ), "detpack" ) ) ; )
	{
		if ( te->s.v.owner == EDICT_TO_PROG( self ) )
		{
			G_sprint( self, 2, "Detpack armed: %3d\n", ( int ) ( te->s.v.nextthink - g_globalvars.time ) );
		}
	}

}

void TeamFortress_ShowTF(  )
{
	float fl;
	int	it,i;

	if ( tf_data.toggleflags & TFLAG_CLASS_PERSIST )
		G_sprint( self, 2, "Class Persistence On.\n" );
	else
		G_sprint( self, 2, "Class Persistence Off.\n" );

	if ( tf_data.toggleflags & TFLAG_CHEATCHECK )
		G_sprint( self, 2, "Cheat Checking On.\n" );
	else
		G_sprint( self, 2, "Cheat Checking Off.\n" );

	if ( tf_data.toggleflags & TFLAG_AUTOTEAM )
		G_sprint( self, 2, "AutoTeam On.\n" );
	else
		G_sprint( self, 2, "AutoTeam Off.\n" );

	if ( tf_data.toggleflags & TFLAG_RESPAWNDELAY )
		G_sprint( self, 2, "%.0f second Respawn Delay.\n", tf_data.respawn_delay_time );
	else
		G_sprint( self, 2, "No Respawn Delay.\n" );

	if ( tf_data.toggleflags & TFLAG_TEAMFRAGS )
		G_sprint( self, 2, "TeamFrags On.\n" );
	else
		G_sprint( self, 2, "TeamFrags Off.\n" );

	if ( tf_data.allow_hook )
		G_sprint( self, 2, "Grapple On.\n" );
	else
		G_sprint( self, 2, "Grapple Off.\n" );

	if ( tf_data.toggleflags & TFLAG_FULLTEAMSCORE )
		G_sprint( self, 2, "Full TeamScore On.\n" );
	else
		G_sprint( self, 2, "Full TeamScore Off.\n" );

	if ( tf_data.clanbattle == 1 )
	{
		fl = GetSVInfokeyFloat( "pm", "prematch", 0 ) * 60;
		G_sprint( self, 2, "Clan battle: ÏÎ\nPrematch: %d seconds\n", (int)fl );
		fl =  GetSVInfokeyFloat( "cft", "ceasefire_time", 0 ) * 60;
		G_sprint( self, 2, "Cease fire: %d seconds\n", (int)fl );
	} else
		G_sprint( self, 2, "Clan battle: ÏÆÆ\n" );
	G_sprint( self, 2, "Cheat pause: %d\n", ( int ) tf_data.cheat_pause );

	if ( tf_data.old_grens )
		G_sprint( self, 2, "ÏÌÄ grenades\n" );
	else
		G_sprint( self, 2, "ÎÅ× grenades\n" );

	if( tf_data.topcolor_check )
	{
		G_sprint( self, 2, "Topcolor checking enabled\n" );
	        for ( i = 1 ; i <= number_of_teams  ; i++ )
	        {
	        	it = TeamFortress_TeamGetTopColor(i);
	        	G_sprint( self, 2, " Team %d topcolor: %2d %s\n",i , it, TeamFortress_TeamGetColorString( it - 1 ));
	        }
	}else
		G_sprint( self, 2, "Topcolor checking disabled\n" );

	if( tf_data.sg_newfind )
		G_sprint( self, 2, "Sentry New Find Target: %d ppl\n", tf_data.sgppl );
	else
		G_sprint( self, 2, "Sentry New Find Target: ÏÆÆ\n" );
	
	switch ( tf_data.sg_sfire )
	{
	case SG_SFIRE_281:
		G_sprint( self, 2, "Sentry Shells Fire: 2.8.1\n" );
		break;
	case SG_SFIRE_MTFL1:
		G_sprint( self, 2, "Sentry Shells Fire: 2.8.1 fixed(mtfl)\n" );
		break;
	case SG_SFIRE_MTFL2:
		G_sprint( self, 2, "Sentry Shells Fire: MTFL2\n" );
		break;
	case SG_SFIRE_NEW:
		G_sprint( self, 2, "Sentry Shells Fire: NEW\n" );
		break;
	default:
		break;
			
	}
	if(tf_data.sg_rfire)
		G_sprint( self, 2, "Sentry Rockets Fire: NEW\n" );
	else
		G_sprint( self, 2, "Sentry Rockets Fire: OLD\n" );
	
	G_sprint( self, 2, "New Gas Options %d:\n" ,tf_data.new_gas );
	if( (tf_data.new_gas & GAS_MASK_COLOR) && !(tf_data.new_gas & GAS_MASK_ALLSPYS))
	{
	 if( tf_data.new_gas & GAS_MASK_ALLCOLORS)
	 	it = 16;
	 else
	 {
	 	if( tf_data.new_gas & GAS_MASK_4COLORS)
	 		it = 4;
	 	else
	 		it = number_of_teams;
	 }
	 G_sprint( self, 2, " Players Colors: %d\n", it );
	}
	if( tf_data.new_gas & GAS_MASK_ALLSPYS)
		G_sprint( self, 2, " Players Skins: Spys\n" );
	else
	{
		if( tf_data.new_gas & GAS_MASK_SKIN)
			G_sprint( self, 2, " Players Skins: Random\n" );
	}
	
	if( tf_data.new_gas & GAS_MASK_PALETTE)
		G_sprint( self, 2, " Palette: ÏÎ\n" );
	else
		G_sprint( self, 2, " Palette: ÏÆÆ\n" );
	
	if( tf_data.new_gas & GAS_MASK_NEWGREN_EFFECTS)
		G_sprint( self, 2, " New Gren Effects: ÏÎ\n" );
	else
		G_sprint( self, 2, " New Gren Effects: ÏÆÆ\n" );
	
	if( tf_data.new_gas & GAS_MASK_NEWGREN_TIMES)
		G_sprint( self, 2, " New Gren Times: ÏÎ\n" );
	else
		G_sprint( self, 2, " New Gren Times: ÏÆÆ\n" );
	
	if( tf_data.new_gas & GAS_MASK_NEWGREN_DMG)
		G_sprint( self, 2, " New Gren DMG: ÏÎ\n" );
	else
		G_sprint( self, 2, " New Gren DMG: ÏÆÆ\n" );
	
	if( !(tf_data.new_gas & GAS_MASK_DISABLE_ID))
		G_sprint( self, 2, " ID: ÏÎ\n" );
	else
		G_sprint( self, 2, " ID: ÏÆÆ\n" );
	
	if ( tf_data.new_flash )
		G_sprint( self, 2, "New Flash: ÏÎ\n" );
	else
		G_sprint( self, 2, "New Flash: ÏÆÆ\n" );

	if ( !tf_data.disable_powerups )
		G_sprint( self, 2, "Powerups: ÏÎ\n" );
	else
		G_sprint( self, 2, "Powerups: ÏÆÆ\n" );

	if ( tf_data.flag_timer )
		G_sprint( self, 2, "Flag Timer: ÏÎ\n" );
	else
		G_sprint( self, 2, "Flag Timer: ÏÆÆ\n" );

	if ( tf_data.snip_fps )

		G_sprint( self, 2, "Sniper FPS: %d\n", ( int ) tf_data.snip_fps );
	else
		G_sprint( self, 2, "Sniper FPS: ÏÆÆ\n" );

	if ( tf_data.snip_range_fix )
		G_sprint( self, 2, "Sniper Range Fix: ÏÎ\n" );
	else
		G_sprint( self, 2, "Sniper Range Fix: ÏÆÆ\n" );

	
	G_sprint( self, 2, "Sniper ammo cost: %d\n", ( int ) tf_data.snip_ammo );
	G_sprint( self, 2, "Sniper Reload Time: %.1f\n", tf_data.snip_time );

	if ( tf_data.random_tf_spawn )
		G_sprint( self, 2, "Random Team Spawn: ÏÎ\n" );
	else
		G_sprint( self, 2, "Random Team Spawn: ÏÆÆ\n" );

	if ( tf_data.allow_drop_goal )
		G_sprint( self, 2, "Drop Goals: ÏÎ\n" );
	else
		G_sprint( self, 2, "Drop Goals: ÏÆÆ\n" );

	if ( tf_data.add_pipe )
		G_sprint( self, 2, "Additional pipebomb: ÏÎ\n" );
	else
		G_sprint( self, 2, "Additional pipebomb: ÏÆÆ\n" );

	if ( tf_data.gren2box )
	{
		G_sprint( self, 2, "Extended Backpack:\n" );
		if ( tf_data.gren2box & BP_TYPE_HEALTH )
			G_sprint( self, 2, " Drop Health   : ÏÎ\n" );
		else
			G_sprint( self, 2, " Drop Health   : ÏÆÆ\n" );

		if ( tf_data.gren2box & BP_TYPE_ARMOR )
			G_sprint( self, 2, " Drop Armor    : ÏÎ\n" );
		else
			G_sprint( self, 2, " Drop Armor    : ÏÆÆ\n" );

		if ( tf_data.gren2box & BP_GREN_BYTYPE )
			G_sprint( self, 2, " Drop Grenades : Type-depend\n" );
		else
		{
			if ( tf_data.gren2box & BP_GREN_BYTYPE )
				G_sprint( self, 2, " Drop Grenades : ÏÎ\n" );

			G_sprint( self, 2, " Drop Grenades : ÏÆÆ\n" );
		}
		if ( tf_data.gren2box & BP_TYPE_DETPACK )
			G_sprint( self, 2, " Drop Detpack  : ÏÎ\n" );
		else
			G_sprint( self, 2, " Drop Detpack  : ÏÆÆ\n" );
	} else
	{
		G_sprint( self, 2, "Extended Backpack: ÏÆÆ\n" );
	}
	if ( tf_data.disable_grens )
	{
		G_sprint( self, 2, "Disabled Grenade Types:\n" );
		if ( tf_data.disable_grens & DG_TYPE_NORMAL )
			G_sprint( self, 2, " Normal\n" );
		if ( tf_data.disable_grens & DG_TYPE_CONCUSSION )
			G_sprint( self, 2, " Concussion\n" );
		if ( tf_data.disable_grens & DG_TYPE_NAIL )
			G_sprint( self, 2, " Nail\n" );
		if ( tf_data.disable_grens & DG_TYPE_MIRV )
			G_sprint( self, 2, " Mirv\n" );
		if ( tf_data.disable_grens & DG_TYPE_NAPALM )
			G_sprint( self, 2, " Napalm\n" );
		if ( tf_data.disable_grens & DG_TYPE_FLARE )
			G_sprint( self, 2, " Flare\n" );
		if ( tf_data.disable_grens & DG_TYPE_GAS )
			G_sprint( self, 2, " Gas\n" );
		if ( tf_data.disable_grens & DG_TYPE_EMP )
			G_sprint( self, 2, " Emp\n" );
		if ( tf_data.disable_grens & DG_TYPE_FLASH )
			G_sprint( self, 2, " Flash\n" );
		if ( tf_data.disable_grens & DG_TYPE_CALTROPS )
			G_sprint( self, 2, " Caltrops\n" );
		if ( tf_data.disable_grens & DG_TYPE_DETPACK )
			G_sprint( self, 2, " Detpack\n" );
	} else
		G_sprint( self, 2, "All Grenades Enabled\n" );
		
	switch( tf_data.arena_mode )
	{
	       case ARENA_MODE_NONE:
	                       G_sprint( self, 2, "Arena Mode: Off\n" );
	                       break;
	       case ARENA_MODE_FFA:
                               G_sprint( self, 2, "Arena Mode: FFA\n" );
	                       break;
	       case ARENA_MODE_DUEL:
	                       G_sprint( self, 2, "Arena Mode: Duel\n" );
	                       break;
	}

	if ( tf_data.mtfl )
		G_sprint( self, 2, "ÍÔÆÌ Settings ÏÎ\n" );

	G_sprint( self, 2, "TF2003 (build %d) by sd‘ angel for ÍÔÆÌ\n" ,build_number());
	if( tf_data.enable_bot )
	        G_sprint( self, 2, "Bot enabled\n" );
	        

        if( tg_data.tg_enabled )
        	G_sprint( self, 2, "Training Ground enabled\n" );

        if( tf_data.lan_mode )
        	G_sprint( self, 2, "Lan mode enabled\n" );

	G_sprint( self, 2,  "%s\n", ANGEL_VERSION);
	
}


void    TeamFortress_GrenadePrimed(  );


void TeamFortress_PrimeGrenade(  )
{
	int     gtype = -1;

	gedict_t *tGrenade;

	if ( ( self->tfstate & 1 ) || ( self->tfstate & 1024 ) )
		return;
	if ( self->s.v.impulse == TF_GRENADE_1 )
	{

		gtype = self->tp_grenades_1;
		if ( gtype < 0 || gtype > 10 )
			gtype = 0;

		if ( tf_data.disable_grens & ( 1 << gtype ) )
			return;

		if ( self->no_grenades_1 > 0 || tg_data.unlimit_grens )
		{
			if ( !tg_data.unlimit_grens )
				self->no_grenades_1--;

			switch ( gtype )
			{

			case GR_TYPE_FLARE:
				newmis = spawn(  );
				g_globalvars.newmis = EDICT_TO_PROG( newmis );
				newmis->s.v.owner = EDICT_TO_PROG( self );
				newmis->s.v.movetype = MOVETYPE_TOSS;
				newmis->s.v.solid = SOLID_BBOX;
				newmis->s.v.classname = "grenade";
				trap_makevectors( self->s.v.v_angle );
				newmis->s.v.velocity[0] =
				    ( g_globalvars.v_forward[0] * 600 + g_globalvars.v_up[0] * 25 ) * 700;
				newmis->s.v.velocity[1] =
				    ( g_globalvars.v_forward[1] * 600 + g_globalvars.v_up[1] * 25 ) * 700;
				newmis->s.v.velocity[2] =
				    ( g_globalvars.v_forward[2] * 600 + g_globalvars.v_up[2] * 25 ) * 700;
				vectoangles( newmis->s.v.velocity, newmis->s.v.angles );
				newmis->s.v.weapon = self->team_no;
				newmis->s.v.think = ( func_t ) FlareGrenadeExplode;
				newmis->s.v.nextthink = g_globalvars.time + 0.8;
				newmis->s.v.touch = ( func_t ) FlareGrenadeTouch;
				newmis->s.v.skin = 1;
				newmis->mdl = "flare";
				setmodel( newmis, "progs/flare.mdl" );
				setsize( newmis, 0, 0, 0, 0, 0, 0 );
				setorigin( newmis, PASSVEC3( self->s.v.origin ) );
				return;
				break;
			case GR_TYPE_CALTROPS:
				G_sprint( self, 2, "Opening Caltrop canister...\n" );
				break;
			default:
				G_sprint( self, 2, "%s primed, 3 seconds...\n", GrenadePrimeName[gtype] );
				if ( self->internal_settings_bits & TF_INTERNAL_GRENSOUND )
				        stuffcmd( self, "play count.wav\n" );
				break;
			}

		} else
		{
			G_sprint( self, 2, "No %ss left.\n", GrenadePrimeName[gtype] );
			return;
		}
	}
	if ( self->s.v.impulse == TF_GRENADE_2 )
	{

		gtype = self->tp_grenades_2;
		if ( gtype < 0 || gtype > 10 )
			gtype = 0;

		if ( tf_data.disable_grens & ( 1 << gtype ) )
			return;

		if ( self->no_grenades_2 > 0 || tg_data.unlimit_grens )
		{
			if ( !tg_data.unlimit_grens )
				self->no_grenades_2--;
			switch ( gtype )
			{

			case GR_TYPE_FLARE:
				newmis = spawn(  );
				newmis->s.v.owner = EDICT_TO_PROG( self );
				newmis->s.v.movetype = MOVETYPE_TOSS;
				newmis->s.v.solid = SOLID_BBOX;
				newmis->s.v.classname = "grenade";
				trap_makevectors( self->s.v.v_angle );
				if ( self->s.v.v_angle[0] )
				{
					newmis->s.v.velocity[0] =
					    g_globalvars.v_forward[0] * 1200 + g_globalvars.v_up[0] * 200;
					newmis->s.v.velocity[1] =
					    g_globalvars.v_forward[1] * 1200 + g_globalvars.v_up[1] * 200;
					newmis->s.v.velocity[2] =
					    g_globalvars.v_forward[2] * 1200 + g_globalvars.v_up[2] * 200;
				} else
				{
					aim( newmis->s.v.velocity );
					VectorScale( newmis->s.v.velocity, 1200, newmis->s.v.velocity );
					newmis->s.v.velocity[2] = 75;
				}
				vectoangles( newmis->s.v.velocity, newmis->s.v.angles );
				newmis->s.v.weapon = self->team_no;
				newmis->s.v.think = ( func_t ) FlareGrenadeExplode;
				newmis->s.v.nextthink = g_globalvars.time + 0.8;
				newmis->s.v.touch = ( func_t ) FlareGrenadeTouch;
				newmis->s.v.skin = 1;
				newmis->mdl = "flare";
				setmodel( newmis, "progs/flare.mdl" );
				setsize( newmis, 0, 0, 0, 0, 0, 0 );
				setorigin( newmis, PASSVEC3( self->s.v.origin ) );
				return;
				break;
			case GR_TYPE_CALTROPS:
				G_sprint( self, 2, "Opening Caltrop canister...\n" );
				break;
			default:
				G_sprint( self, 2, "%s primed, 3 seconds...\n", GrenadePrimeName[gtype] );
				if ( self->internal_settings_bits & TF_INTERNAL_GRENSOUND )
				        stuffcmd( self, "play count.wav\n" );
				break;
			}

		} else
		{
			G_sprint( self, 2, "No %ss left.\n", GrenadePrimeName[gtype] );
			return;
		}
	}
	if( gtype < 0 )
		return;

	self->tfstate |= 1;
	tGrenade = spawn(  );
	tGrenade->s.v.owner = EDICT_TO_PROG( self );
	tGrenade->s.v.weapon = gtype;
	tGrenade->s.v.classname = "primer";
	tGrenade->s.v.impulse = self->s.v.impulse;
	tGrenade->s.v.nextthink = g_globalvars.time + 0.8;
	tGrenade->respawn_time = g_globalvars.time + 0.8;
	if ( gtype == GR_TYPE_CALTROPS )
		tGrenade->heat = g_globalvars.time + 0.5 + 0.5;
	else
		tGrenade->heat = g_globalvars.time + 3 + 0.8;
	tGrenade->s.v.think = ( func_t ) TeamFortress_GrenadePrimed;
}

void TeamFortress_GrenadePrimed(  )
{
	gedict_t *user;
	qboolean printthrowmsg = true;

	user = PROG_TO_EDICT( self->s.v.owner );
	if ( !( user->tfstate & TFSTATE_GRENTHROWING ) && !user->s.v.deadflag )
	{
		self->s.v.nextthink = g_globalvars.time + 0.1;
		if ( !self->s.v.think )
			dremove( self );
		if ( g_globalvars.time > self->heat )
			TeamFortress_ExplodePerson(  );
		return;
	}
	if ( !( user->tfstate & TFSTATE_GRENPRIMED ) )
		G_conprintf( "GrenadePrimed logic error\n" );
	user->tfstate -= ( user->tfstate & TFSTATE_GRENPRIMED );
	user->tfstate -= ( user->tfstate & TFSTATE_GRENTHROWING );
	sound( user, 1, "weapons/grenade.wav", 1, 1 );
// sound (user, 1, "weapons/ax1.wav", 1, 1);
	KickPlayer( -1, user );
	newmis = spawn(  );
	g_globalvars.newmis = EDICT_TO_PROG( newmis );
	newmis->s.v.owner = EDICT_TO_PROG( user );
	newmis->s.v.movetype = MOVETYPE_BOUNCE;
	newmis->s.v.solid = SOLID_BBOX;
	newmis->s.v.classname = "grenade";
	trap_makevectors( user->s.v.v_angle );
	if ( user->s.v.deadflag )
		SetVector( newmis->s.v.velocity, 0, 0, 200 );
	else
	{
		if ( user->s.v.v_angle[0] )
		{
			newmis->s.v.velocity[0] =
			    g_globalvars.v_forward[0] * 600 + g_globalvars.v_up[0] * 200 +
			    crandom(  ) * g_globalvars.v_right[0] * 10 + crandom(  ) * g_globalvars.v_up[0] * 10;
			newmis->s.v.velocity[1] =
			    g_globalvars.v_forward[1] * 600 + g_globalvars.v_up[1] * 200 +
			    crandom(  ) * g_globalvars.v_right[1] * 10 + crandom(  ) * g_globalvars.v_up[1] * 10;
			newmis->s.v.velocity[2] =
			    g_globalvars.v_forward[2] * 600 + g_globalvars.v_up[2] * 200 +
			    crandom(  ) * g_globalvars.v_right[2] * 10 + crandom(  ) * g_globalvars.v_up[2] * 10;
		} else
		{
			//newmis->s.v.velocity = aim(user, 10000);
			aim( newmis->s.v.velocity );
			VectorScale( newmis->s.v.velocity, 600, newmis->s.v.velocity );
			newmis->s.v.velocity[2] = 200;
		}
	}
	vectoangles( newmis->s.v.velocity, newmis->s.v.angles );
	newmis->s.v.think = ( func_t ) SUB_Null;
	newmis->s.v.nextthink = self->heat;
	switch ( ( int ) self->s.v.weapon )
	{
	case GR_TYPE_NORMAL:
		newmis->s.v.touch = ( func_t ) NormalGrenadeTouch;
		newmis->s.v.think = ( func_t ) NormalGrenadeExplode;
		newmis->s.v.skin = 0;
		SetVector( newmis->s.v.avelocity, 300, 300, 300 );
		if ( tf_data.birthday == 1 && g_random(  ) < 0.6 )
			setmodel( newmis, "progs/grenade3.mdl" );
		else
			setmodel( newmis, "progs/hgren2.mdl" );
		break;
	case GR_TYPE_CONCUSSION:
		newmis->s.v.touch = ( func_t ) ConcussionGrenadeTouch;
		newmis->s.v.think = ( func_t ) ConcussionGrenadeExplode;
		newmis->s.v.skin = 1;
		SetVector( newmis->s.v.avelocity, 300, 300, 300 );
		if ( tf_data.birthday == 1 && g_random(  ) < 0.6 )
			setmodel( newmis, "progs/grenade3.mdl" );
		else
			setmodel( newmis, "progs/hgren2.mdl" );
		break;

	case GR_TYPE_NAIL:
		newmis->s.v.touch = ( func_t ) NailGrenadeTouch;
		newmis->s.v.think = ( func_t ) NailGrenadeExplode;
		newmis->s.v.skin = 1;
		SetVector( newmis->s.v.avelocity, 0, 300, 0 );
		if ( tf_data.birthday == 1 && g_random(  ) < 0.6 )
			setmodel( newmis, "progs/grenade3.mdl" );
		else
			setmodel( newmis, "progs/biggren.mdl" );
		break;

	case GR_TYPE_MIRV:
		newmis->s.v.touch = ( func_t ) MirvGrenadeTouch;
		newmis->s.v.think = ( func_t ) MirvGrenadeExplode;
		newmis->s.v.skin = 0;
		SetVector( newmis->s.v.avelocity, 0, 300, 0 );
		if ( tf_data.birthday == 1 && g_random(  ) < 0.6 )
			setmodel( newmis, "progs/grenade3.mdl" );
		else
			setmodel( newmis, "progs/biggren.mdl" );
		break;

	case GR_TYPE_NAPALM:
		newmis->s.v.touch = ( func_t ) NapalmGrenadeTouch;
		newmis->s.v.think = ( func_t ) NapalmGrenadeExplode;
		newmis->s.v.skin = 2;
		SetVector( newmis->s.v.avelocity, 0, 300, 0 );
		if ( tf_data.birthday == 1 && g_random(  ) < 0.6 )
			setmodel( newmis, "progs/grenade3.mdl" );
		else
			setmodel( newmis, "progs/biggren.mdl" );
		break;

	case GR_TYPE_FLARE:
		newmis->s.v.touch = ( func_t ) FlareGrenadeTouch;
		newmis->s.v.weapon = self->team_no;
		newmis->s.v.think = ( func_t ) FlareGrenadeExplode;
		newmis->s.v.skin = 1;
		SetVector( newmis->s.v.avelocity, 300, 300, 300 );
		if ( tf_data.birthday == 1 && g_random(  ) < 0.6 )
			setmodel( newmis, "progs/grenade3.mdl" );
		else
			newmis->mdl = "flare";
		setmodel( newmis, "progs/flare.mdl" );
		printthrowmsg = false;
		break;

	case GR_TYPE_GAS:
		newmis->s.v.touch = ( func_t ) GasGrenadeTouch;
		newmis->s.v.think = ( func_t ) GasGrenadeExplode;
		newmis->s.v.skin = 3;
		SetVector( newmis->s.v.avelocity, 300, 300, 300 );
		if ( tf_data.birthday == 1 && g_random(  ) < 0.6 )
			setmodel( newmis, "progs/grenade3.mdl" );
		else
			setmodel( newmis, "progs/grenade2.mdl" );
		break;

	case GR_TYPE_EMP:
		newmis->s.v.touch = ( func_t ) EMPGrenadeTouch;
		newmis->s.v.think = ( func_t ) EMPGrenadeExplode;
		newmis->s.v.skin = 4;
		SetVector( newmis->s.v.avelocity, 300, 300, 300 );
		if ( tf_data.birthday == 1 && g_random(  ) < 0.6 )
			setmodel( newmis, "progs/grenade3.mdl" );
		else
			setmodel( newmis, "progs/grenade2.mdl" );
		break;

	case GR_TYPE_FLASH:
		newmis->s.v.touch = ( func_t ) FlashGrenadeTouch;
		newmis->s.v.think = ( func_t ) FlashGrenadeExplode;
		newmis->s.v.skin = 2;
		SetVector( newmis->s.v.avelocity, 300, 300, 300 );
		if ( tf_data.birthday == 1 && g_random(  ) < 0.6 )
			setmodel( newmis, "progs/grenade3.mdl" );
		else
			setmodel( newmis, "progs/hgren2.mdl" );
		break;

	case GR_TYPE_CALTROPS:
		newmis->s.v.touch = ( func_t ) CanisterTouch;
		newmis->s.v.think = ( func_t ) ScatterCaltrops;
		newmis->s.v.skin = 0;
		SetVector( newmis->s.v.avelocity, 0, 0, 0 );
		printthrowmsg = false;
		break;
	}
	if( printthrowmsg )
	        G_sprint( user, 0, "%s thrown.\n", GrenadePrimeName[(int)self->s.v.weapon] );
	        
	setsize( newmis, 0, 0, 0, 0, 0, 0 );
	setorigin( newmis, PASSVEC3( user->s.v.origin ) );
	dremove( self );
}

void TeamFortress_ThrowGrenade(  )
{
	gedict_t *te;

	if ( !( self->tfstate & 1 ) )
		return;
	self->tfstate |= TFSTATE_GRENTHROWING;
	for ( te = world ; ( te = trap_find( te, FOFS( s.v.classname ), "primer" ) ) ; )
	{
		if ( te->s.v.owner == EDICT_TO_PROG( self ) )
			if ( te->respawn_time <= g_globalvars.time )
			{
				te->s.v.nextthink = g_globalvars.time;
			}
//              else te->s.v.nextthink=te.respawn_time;
	}

}




int IsLegalClass( int pc )
{
	int     bit;


	if ( tf_data.spy_off == 1 && pc == 8 )
		return 0;
	bit = classbits[pc];
	if ( ( illegalclasses[0] & bit ) || ( TeamFortress_TeamGetIllegalClasses( self->team_no ) & bit ) )
		return 0;
	return 1;
}

void TeamFortress_SetSpeed( gedict_t * p )
{
	int     tf = 0;
	gedict_t *te = world;
	int     pc;

	stuffcmd( p, "cl_movespeedkey 1\n" );
	if ( p->tfstate & TFSTATE_CANT_MOVE )
	{
		SetVector( p->s.v.velocity, 0, 0, 0 );
		stuffcmd( p, "cl_backspeed 0\n" );
		stuffcmd( p, "cl_forwardspeed 0\n" );
		stuffcmd( p, "cl_sidespeed 0\n" );
		p->maxspeed = 0;
		return;
	}
	pc = p->playerclass;
	if ( pc <= 0 || pc > 11 )
	{
		pc = 0;
		p->maxspeed = 0;
	}
	p->maxfbspeed = class_set[pc].maxspeed;
	p->maxstrafespeed = class_set[pc].maxstrafespeed;

	if ( !pc )
	{
		return;
	}

	while ( ( te = trap_find( te, FOFS( s.v.classname ), "item_tfgoal" ) ) && !tf )
	{
		if ( te->s.v.owner == EDICT_TO_PROG( p ) )
		{
			if ( te->goal_activation & TFGI_SLOW )
			{
				tf = 1;
				p->maxfbspeed /= 2;
				p->maxstrafespeed /= 2;
			}
		}
	}
	if ( p->tfstate & TFSTATE_TRANQUILISED )
	{
		p->maxfbspeed /= 2;
		p->maxstrafespeed /= 2;
	}
	if ( p->leg_damage )
	{
		if ( p->leg_damage > 6 )
			p->leg_damage = 6;
		p->maxfbspeed *= ( 10 - (float)p->leg_damage ) / 10;
		p->maxstrafespeed *= ( 10 - (float)p->leg_damage ) / 10;
	}
	if ( p->tfstate & TFSTATE_AIMING )
	{
		if ( p->maxfbspeed > 80 )
			p->maxfbspeed = 80;
		if ( p->maxstrafespeed > 80 )
			p->maxstrafespeed = 80;
	}

	stuffcmd( p, "cl_backspeed %d\n", p->maxfbspeed );
	stuffcmd( p, "cl_forwardspeed %d\n", p->maxfbspeed );
	stuffcmd( p, "cl_sidespeed %d\n", p->maxstrafespeed );
	p->maxspeed = p->maxfbspeed;
}

void TeamFortress_SetHealth(  )
{
	int     pc ;
	pc = self->playerclass;
	//if ( (pc <= 0 || pc >= 10)  && (pc != 11 ))
	if ( (pc <= 0 || pc > 11) )
	{
		pc = 0;
		self->s.v.takedamage = 0;
	}
	self->s.v.health = self->s.v.max_health = class_set[pc].maxhealth;
}

const char* TeamFortress_GetSkinByTeamClass( int tn, int pc )
{
        static char skin[20];

	GetSVInfokeyString( class_set[pc].infokey4skin[tn - 1], NULL,
			    skin, sizeof( skin ), class_set[pc].defaultskin );

	return skin;
}
const char* TeamFortress_GetSkin( gedict_t * p )
{
	int     tn;
	int     pc;

	if ( p->playerclass == PC_CIVILIAN || !p->team_no )
		return "base";
	tn = p->team_no;
	pc = p->playerclass;
	if ( p->playerclass == 8 )
	{
		if ( p->undercover_team )
			tn = p->undercover_team;
		if ( p->undercover_skin )
			pc = p->undercover_skin;
	}

	return TeamFortress_GetSkinByTeamClass( tn, pc );
}

void TeamFortress_SetSkin( gedict_t * p )
{

	p->immune_to_check = g_globalvars.time + tf_data.cheat_pause;	//10;
	if ( p->playerclass == PC_SPY && p->undercover_skin )
		p->s.v.skin = p->undercover_skin;
	else
		p->s.v.skin = p->playerclass;
	if( p->isBot )
	{
	        if ( p->s.v.skin )
	                trap_SetBotUserInfo(NUM_FOR_EDICT( p ),"skin",TeamFortress_GetSkin( p ),0);
	        else
	                trap_SetBotUserInfo(NUM_FOR_EDICT( p ),"skin","base",0);

	}else
	{
        	if ( p->s.v.skin )
        		stuffcmd( p, "skin %s\n", TeamFortress_GetSkin( p ) );
        	else
        		stuffcmd( p, "skin base\n" );
	}
}

void TeamFortress_SetColor( gedict_t * p, int top, int bottom )
{
        char tmp[0x20];
        if( p->isBot )
        {
                _snprintf(tmp, sizeof(tmp), "%d",top);
                trap_SetBotUserInfo(NUM_FOR_EDICT( p ),"topcolor",tmp,0);
                _snprintf(tmp, sizeof(tmp),"%d",bottom);
                trap_SetBotUserInfo(NUM_FOR_EDICT( p ),"bottomcolor",tmp,0);
        }else
        {
                stuffcmd( p, "color %d %d\n",top,bottom);
        }
}


void TeamFortress_SetEquipment(  )
{
	int     kept_items;
	int     pc;
	gedict_t *te;
//	char	st[10];
//	int		opt;

	if ( strneq( self->s.v.classname, "player" ) )
		return;
	kept_items = self->tf_items & ( IT_KEY1 | IT_KEY2 );
	self->s.v.items = 0;
	self->last_weaponmode = 0;
	self->last_weapon = 0;
	self->current_weapon = 0;
	self->weapons_carried = 0;
	self->tf_items = 0;
	if ( self->playerclass != PC_SCOUT )
		self->tf_items_flags = 0;
	self->armorclass = 0;
	self->s.v.impulse = 0;
	self->undercover_skin = 0;
	if ( self->undercover_team )
	{
		self->immune_to_check = g_globalvars.time + tf_data.cheat_pause;	//10;
		self->undercover_team = 0;
		TeamFortress_SetColor( self, TeamFortress_TeamGetTopColor( self->team_no ),
			  TeamFortress_TeamGetColor( self->team_no ) - 1 );
	}
	self->is_building = 0;
	self->is_detpacking = 0;
	self->is_undercover = 0;
	self->is_feigning = 0;
	self->is_unabletospy = 0;
	self->ammo_medikit = 0;
	self->maxammo_medikit = 0;
	self->ammo_detpack = 0;
	self->maxammo_detpack = 0;
	self->items_allowed = 0;
	self->armor_allowed = 0;
	self->maxarmor = 0;
	self->weaponmode = 0;
	self->respawn_time = 0;
	self->heat = 0;
	self->tfstate -= self->tfstate & TFSTATE_RELOADING;
	if ( !self->team_no )
		self->lives = -1;
	self->s.v.items = ( int ) self->s.v.items | kept_items;
    
	self->assault_min_shells = GetInfokeyInt( self, "sb", NULL, DEFAULT_ASSAULT_MIN_SHELLS );

	if( self->settings_bits & TF_AUTOID_MASK)
	{
         te = spawn();
         te->s.v.nextthink = g_globalvars.time + TF_AUTOIDTIME;
         te->s.v.think = (func_t) TeamFortress_AutoID;
         te->s.v.owner = EDICT_TO_PROG(self);
         te->s.v.classname  = "timer";
	}
	self->allow_snip_time = 0;
	pc = self->playerclass;
	if ( pc > 11 )
		pc = 0;
	self->weapons_carried |= class_set[pc].weapons_carried;

        if( tg_data.tg_enabled )
        {
		self->weapons_carried |= WEAP_SPANNER;
		self->weapons_carried -= self->weapons_carried & WEAP_AXE;
        }

	self->s.v.ammo_rockets = class_set[pc].ammo_rockets;
	self->s.v.ammo_nails = class_set[pc].ammo_nails;
	self->s.v.ammo_shells = class_set[pc].ammo_shells;
	self->s.v.ammo_cells = class_set[pc].ammo_cells;
	self->ammo_detpack = class_set[pc].ammo_detpack;
	self->ammo_medikit = class_set[pc].ammo_medikit;

	self->maxammo_rockets = class_set[pc].maxammo_rockets;
	self->maxammo_nails = class_set[pc].maxammo_nails;
	self->maxammo_shells = class_set[pc].maxammo_shells;
	self->maxammo_cells = class_set[pc].maxammo_cells;
	self->maxammo_detpack = class_set[pc].maxammo_detpack;
	self->maxammo_medikit = class_set[pc].maxammo_medikit;

	self->no_grenades_1 = class_set[pc].no_grenades_1;
	self->no_grenades_2 = class_set[pc].no_grenades_2;
	if ( tf_data.old_grens == 1 )
	{
		self->tp_grenades_1 = class_set[pc].og_tp_grenades_1;
		self->tp_grenades_2 = class_set[pc].og_tp_grenades_2;
	} else
	{
		self->tp_grenades_1 = class_set[pc].tp_grenades_1;
		self->tp_grenades_2 = class_set[pc].tp_grenades_2;
	}
	self->tf_items = class_set[pc].tf_items;
	self->tf_items_flags |= class_set[pc].tf_items_flags;

	self->armorclass |= class_set[pc].armorclass;
	self->s.v.armortype = class_set[pc].armortype;
	self->s.v.armorvalue = class_set[pc].armorvalue;
	self->armor_allowed = class_set[pc].armor_allowed;
	self->maxarmor = class_set[pc].maxarmor;
	self->current_weapon = class_set[pc].current_weapon;
	self->items_allowed = class_set[pc].items_allowed;
	self->s.v.items = ( int ) self->s.v.items | class_set[pc].items;

	if ( pc == 1 )
	{
		if ( self->ScannerOn )
		{
			self->ScannerOn = 0;
			ScannerSwitch(  );
		}
		if ( self->ScanRange == 0 )
			self->ScanRange = DEFAULT_AUTOSCAN_RANGE;
	}
	if ( pc == 5 )
	{
		te = spawn(  );
		te->s.v.nextthink = g_globalvars.time + PC_MEDIC_REGEN_TIME;
		te->s.v.think = ( func_t ) TeamFortress_Regenerate;
		te->s.v.owner = EDICT_TO_PROG( self );
		te->s.v.classname = "timer";
	}
	if ( pc == 5 && tf_data.invis_only == 1 )
	{
		te = spawn(  );
		te->s.v.nextthink = g_globalvars.time + PC_SPY_CELL_REGEN_TIME;
		te->s.v.think = ( func_t ) TeamFortress_RegenerateCells;
		te->s.v.owner = EDICT_TO_PROG( self );
		te->s.v.classname = "timer";
	}

	if ( !pc )
	{
		self->s.v.flags = FL_CLIENT | FL_NOTARGET;
		self->s.v.waterlevel = 3;
		self->s.v.takedamage = 0;
		self->s.v.solid = SOLID_NOT;
		self->s.v.movetype = MOVETYPE_NONE;//!!!REMOVE ME MOVETYPE_NOCLIP;
		self->s.v.model = "";
		self->mdl = "";
		self->s.v.modelindex = 0;
		self->s.v.weaponmodel = "";
		modelindex_player = 0;
		self->tfstate = self->tfstate | TFSTATE_RELOADING;	//?????
		setmodel( self, "" );
	}

	if ( self->s.v.armortype >= 0.8 )
		self->s.v.items = ( int ) self->s.v.items | IT_ARMOR3;
	else
	{
		if ( self->s.v.armortype >= 0.6 )
			self->s.v.items = ( int ) self->s.v.items | IT_ARMOR2;
		else
		{
			if ( self->s.v.armortype >= 0.3 )
				self->s.v.items = ( int ) self->s.v.items | IT_ARMOR1;
		}
	}
	if ( tf_data.allow_hook && self->playerclass )
		self->weapons_carried |= WEAP_HOOK;
	W_SetCurrentAmmo(  );
}


int TeamFortress_GetMaxAmmo( gedict_t * Retriever, int AmmoType )
{
	if ( AmmoType == IT_SHELLS )
		return Retriever->maxammo_shells;
	else
	{
		if ( AmmoType == IT_NAILS )
			return Retriever->maxammo_nails;
		else
		{
			if ( AmmoType == IT_CELLS )
				return Retriever->maxammo_cells;
			else
			{
				if ( AmmoType == IT_ROCKETS )
					return Retriever->maxammo_rockets;
				else
				{
					if ( AmmoType == WEAP_MEDIKIT )
						return Retriever->maxammo_medikit;
					else
					{
						if ( AmmoType == WEAP_DETPACK )
							return Retriever->maxammo_detpack;
					}
				}
			}
		}
	}
	G_conprintf( "Error in TeamFortress_GetMaxAmmo()\n" );
	G_conprintf( "Invalid ammo type passed.\n" );
	return 0;
}

int TeamFortress_CanGetWeapon( gedict_t * Retriever, int WeaponType )
{
	if ( Retriever->items_allowed & WeaponType )
		return 1;
	return 0;
}


void TeamFortress_DescribeArmor( gedict_t * Player, int Armorclass )
{

	if ( !Armorclass )
		return;
	if ( Armorclass & AT_SAVEFIRE )
		G_sprint( Player, 2, "Asbestos " );
	if ( Armorclass & AT_SAVENAIL )
		G_sprint( Player, 2, "Wooden " );
	if ( Armorclass & AT_SAVEEXPLOSION )
		G_sprint( Player, 2, "Blast " );
	if ( Armorclass & AT_SAVEELECTRICITY )
		G_sprint( Player, 2, "Shockproof " );
	if ( Armorclass & AT_SAVESHOT )
		G_sprint( Player, 2, "Kevlar " );
	G_sprint( Player, 2, "armor\n" );
}

void TeamFortress_AddBackpackItems( gedict_t * Retriever, gedict_t * Items )
{
	return;
}

const char   *TeamFortress_GetClassName( int pc )
{
	return class_set[pc].name;
}


void TeamFortress_PrintClassName( gedict_t * Viewer, int pc, int rpc )
{
	if ( rpc )
		G_sprint( Viewer, 2, "%s (Random)\n", TeamFortress_GetClassName( pc ) );
	else
		G_sprint( Viewer, 2, "%s\n", TeamFortress_GetClassName( pc ) );
}


void TeamFortress_RemoveTimers(  )
{
	gedict_t *te;

	self->leg_damage = 0;
	self->is_undercover = 0;
	self->is_building = 0;
	self->building = world;
	if ( self->tfstate & TFSTATE_AIMING )
	{
		self->tfstate -= TFSTATE_AIMING;
		TeamFortress_SetSpeed( self );
		self->heat = 0;
	}
	if ( self->tfstate & TFSTATE_INFECTED )
		self->tfstate -= TFSTATE_INFECTED;

	if ( self->tfstate & TFSTATE_HALLUCINATING )
		self->tfstate -= TFSTATE_HALLUCINATING;

	ResetGasSkins(self);

	for( te = world; ( te = trap_find( te, FOFS( s.v.classname ), "timer" ) );)
	{
		if ( te->s.v.owner == EDICT_TO_PROG( self ) )
		{
			dremove( te );
			te = world;
		}
	}

	for( te = world; (te = trap_find( te, FOFS( s.v.classname ), "item_tfgoal" )); )
	{
		if ( te->s.v.owner == EDICT_TO_PROG( self ) )
		{
			if ( !( te->goal_activation & TFGI_KEEP ) || self->has_disconnected == 1 )
				tfgoalitem_RemoveFromPlayer( te, self, 0 );
			if ( CTF_Map == 1 && te->goal_no == 1 )
			{
				G_bprint( 2, "%s ÌÏÓÔ the ÂÌÕÅ flag!\n", self->s.v.netname );
			} else
			{
				if ( CTF_Map == 1 && te->goal_no == 2 )
				{
					G_bprint( 2, "%s ÌÏÓÔ the ÒÅÄ flag!\n", self->s.v.netname );
				}
			}
		}
	}
	for( te = world; (te = trap_find( te, FOFS( s.v.classname ), "detpack" ));)
	{
		if ( te->weaponmode == 1 && te->s.v.enemy == EDICT_TO_PROG( self ) )
			te->weaponmode = 0;
	}

	TeamFortress_DetonatePipebombs(  );

	if ( self->has_disconnected == 1 )
	{
		for(te = world; (te = trap_find( te, FOFS( s.v.classname ), "grenade" ));)
		{
			if ( te->s.v.owner == EDICT_TO_PROG( self ) && streq(te->s.v.model , "progs/caltrop.mdl") )
			{
				dremove( te );
				te = world;
			}
		}
	}
	if ( (tf_data.old_grens == 1) || (tf_data.new_gas & GAS_MASK_PALETTE) )
	{
		stuffcmd( self, "v_idlescale 0\n" );
		stuffcmd( self, "v_cshift; wait; bf\n" );
		self->FlashTime = 0;
		if ( tf_data.new_flash )
			disableupdates( self, -1 );	/* server-side flash */
	}
	self->item_list = 0;
	CenterPrint( self, "\n" );
	self->menu_count = MENU_REFRESH_RATE;
	self->current_menu = MENU_DEFAULT;
	self->s.v.impulse = 0;
}

void TeamFortress_SetupRespawn( int Suicided )
{
	float   restime;

	if ( self->respawn_time > g_globalvars.time )
		return;
	if ( tf_data.toggleflags & TFLAG_RESPAWNDELAY )
		restime = tf_data.respawn_delay_time;
	else
		restime = 0;

	if ( tf_data.cb_prematch_time < g_globalvars.time )
	{
		if ( Suicided )
		{
			if ( self->lives > 0 )
				self->lives--;
			restime += 7;
		}
	}
	if ( tf_data.cb_prematch_time > g_globalvars.time )
	{
		if ( self->lives > 0 )
			self->lives--;

		if ( self->lives != -1 )
		{
			if ( !self->lives )
			{
				G_sprint( self, 2, "NO lives left, returning to Observer mode.\n" );
				self->playerclass = 0;
				self->tfstate -= ( self->tfstate & TFSTATE_RANDOMPC );
				self->s.v.movetype = MOVETYPE_NONE;//!!!REMOVE ME MOVETYPE_NOCLIP;
				self->s.v.solid = SOLID_NOT;
				self->s.v.model = "";
				self->mdl = "";
				SetVector( self->s.v.velocity, 0, 0, 0 );
				SetVector( self->s.v.avelocity, 0, 0, 0 );
				self->s.v.enemy = EDICT_TO_PROG( world );
				setmodel( self, "" );
				return;
			}
			if ( self->lives == 1 )
				G_sprint( self, 2, "LAST life.\n" );
			else
			{
				G_sprint( self, 2, "%d lives left.\n", self->lives );
			}
		}
	}
	self->respawn_time = g_globalvars.time + restime;
	if ( restime > 3 )
	{
		G_sprint( self, 2, "%d seconds till respawn.\n", ( int ) restime );
	}
}


void TeamFortress_CheckClassStats(  )
{
	if ( self->s.v.armortype > self->armor_allowed )
		self->s.v.armortype = self->armor_allowed;

	if ( self->s.v.armorvalue > self->maxarmor )
		self->s.v.armorvalue = self->maxarmor;
	if ( self->s.v.armortype < 0 )
		self->s.v.armortype = 0;
	if ( self->s.v.armorvalue < 0 )
		self->s.v.armorvalue = 0;
	if ( self->s.v.ammo_shells > TeamFortress_GetMaxAmmo( self, 256 ) )
		self->s.v.ammo_shells = TeamFortress_GetMaxAmmo( self, 256 );
	if ( self->s.v.ammo_shells < 0 )
		self->s.v.ammo_shells = 0;
	if ( self->s.v.ammo_nails > TeamFortress_GetMaxAmmo( self, 512 ) )
		self->s.v.ammo_nails = TeamFortress_GetMaxAmmo( self, 512 );
	if ( self->s.v.ammo_nails < 0 )
		self->s.v.ammo_nails = 0;
	if ( self->s.v.ammo_rockets > TeamFortress_GetMaxAmmo( self, 1024 ) )
		self->s.v.ammo_rockets = TeamFortress_GetMaxAmmo( self, 1024 );
	if ( self->s.v.ammo_rockets < 0 )
		self->s.v.ammo_rockets = 0;
	if ( self->s.v.ammo_cells > TeamFortress_GetMaxAmmo( self, 2048 ) )
		self->s.v.ammo_cells = TeamFortress_GetMaxAmmo( self, 2048 );
	if ( self->s.v.ammo_cells < 0 )
		self->s.v.ammo_cells = 0;
	if ( self->ammo_medikit > TeamFortress_GetMaxAmmo( self, 4 ) )
		self->ammo_medikit = TeamFortress_GetMaxAmmo( self, 4 );
	if ( self->ammo_medikit < 0 )
		self->ammo_medikit = 0;
	if ( self->ammo_detpack > TeamFortress_GetMaxAmmo( self, 131072 ) )
		self->ammo_detpack = TeamFortress_GetMaxAmmo( self, 131072 );
	if ( self->ammo_detpack < 0 )
		self->ammo_detpack = 0;
	if ( self->no_grenades_1 < 0 )
		self->no_grenades_1 = 0;
	if ( self->no_grenades_2 < 0 )
		self->no_grenades_2 = 0;

	if ( self->s.v.health > self->s.v.max_health && !( ( int ) ( self->s.v.items ) & IT_SUPERHEALTH ) )
		TF_T_Damage( self, world, world, self->s.v.max_health, 0, TF_TD_NOSOUND );
//		TF_T_Damage( self, world, world, self->s.v.max_health - self->s.v.health, 0, TF_TD_NOSOUND ); //old qqq

	if ( self->s.v.health < 0 )
		T_Heal( self, self->s.v.health - self->s.v.health, 0 ); //????? not worked

	self->s.v.items = ( int ) self->s.v.items - ( ( int ) self->s.v.items & ( IT_ARMOR1 | IT_ARMOR2 | IT_ARMOR3 ) );

	if ( self->s.v.armortype >= 0.8 )
		self->s.v.items = ( int ) self->s.v.items | IT_ARMOR3;
	else
	{
		if ( self->s.v.armortype >= 0.6 )
			self->s.v.items = ( int ) self->s.v.items | IT_ARMOR2;
		else
		{
			if ( self->s.v.armortype >= 0.3 )
				self->s.v.items = ( int ) self->s.v.items | IT_ARMOR1;
		}
	}
}


void TeamFortress_DropAmmo( int type )
{
	float   ammo ;

	switch( type )
	{
		case 1:
          		ammo = DROP_SHELLS;
          		if ( self->s.v.ammo_shells < ammo )
          		{
          			if ( self->playerclass == PC_ENGINEER )
          			{
          				if ( self->s.v.ammo_cells / AMMO_COST_SHELLS > ammo - self->s.v.ammo_shells )
          				{
          					G_sprint( self, 2, "you make some shells.\n" );
          					self->s.v.ammo_cells =
          					    self->s.v.ammo_cells - ( ammo - self->s.v.ammo_shells ) * AMMO_COST_SHELLS;
          					self->s.v.ammo_shells = ammo;
          				}
          			}
             			if (self->s.v.ammo_shells < ammo) 
          				return;
          		}
          		self->s.v.ammo_shells = self->s.v.ammo_shells - ammo;
          		break;
		case 2:
			ammo = DROP_NAILS;
			if ( self->s.v.ammo_nails < ammo )
			{
				if ( self->playerclass == PC_ENGINEER )
				{
					if ( self->s.v.ammo_cells / AMMO_COST_NAILS > ammo - self->s.v.ammo_nails )
					{
						G_sprint( self, 2, "you make some nails.\n" );
						self->s.v.ammo_cells =
						    self->s.v.ammo_cells - ( ammo -
									     self->s.v.ammo_nails ) * AMMO_COST_NAILS;
						self->s.v.ammo_nails = ammo;
					}
				}
				if ( self->s.v.ammo_nails < ammo )
					return;
			}
			self->s.v.ammo_nails = self->s.v.ammo_nails - ammo;
			break;
		case 3:
       			ammo = DROP_ROCKETS;
       			if ( self->s.v.ammo_rockets < ammo )
       			{
       				if ( self->playerclass == PC_ENGINEER )
       				{
       					if ( self->s.v.ammo_cells /
       					     AMMO_COST_ROCKETS > ammo - self->s.v.ammo_rockets )
       					{
       						G_sprint( self, 2, "you make some rockets.\n" );
       						self->s.v.ammo_cells =
       						    self->s.v.ammo_cells -
       						    ( ammo - self->s.v.ammo_rockets ) * AMMO_COST_ROCKETS;
       						self->s.v.ammo_rockets = ammo;
       					}
       				}
       				if ( self->s.v.ammo_rockets < ammo )
       					return;
       			}
       			self->s.v.ammo_rockets = self->s.v.ammo_rockets - ammo;
			break;
		case 4:
      			ammo = DROP_CELLS;
      			if ( self->s.v.ammo_cells < ammo )
      			{
      				return;
      			}
      			self->s.v.ammo_cells = self->s.v.ammo_cells - ammo;
      			break;
		default:
			return;
	}
	W_SetCurrentAmmo(  );
	increment_team_ammoboxes( self->team_no );

	newmis = spawn(  );
	g_globalvars.newmis = EDICT_TO_PROG( newmis );
	newmis->aflag = ammo;
	newmis->s.v.weapon = type;
	switch ( ( int ) newmis->s.v.weapon )
	{
	case 1:
		newmis->s.v.ammo_shells = ammo;
		break;
	case 2:
		newmis->s.v.ammo_nails = ammo;
		break;
	case 3:
		newmis->s.v.ammo_rockets = ammo;
		break;
	case 4:
		newmis->s.v.ammo_cells = ammo;
		break;
	}

	newmis->s.v.enemy = EDICT_TO_PROG( self );
	newmis->s.v.health = g_globalvars.time;
	newmis->s.v.movetype = MOVETYPE_TOSS;
	newmis->s.v.solid = SOLID_TRIGGER;
	newmis->s.v.classname = "ammobox";
	newmis->team_no = self->team_no;

	trap_makevectors( self->s.v.v_angle );
	if ( self->s.v.v_angle[0] )
	{
		newmis->s.v.velocity[0] = g_globalvars.v_forward[0] * 400 + g_globalvars.v_up[0] * 200;
		newmis->s.v.velocity[1] = g_globalvars.v_forward[1] * 400 + g_globalvars.v_up[1] * 200;
		newmis->s.v.velocity[2] = g_globalvars.v_forward[2] * 400 + g_globalvars.v_up[2] * 200;
	} else
	{
		//newmis->s.v.velocity = aim(self, 10000);
		aim( newmis->s.v.velocity );
		VectorScale( newmis->s.v.velocity, 400, newmis->s.v.velocity );
		newmis->s.v.velocity[2] = 200;
	}
	newmis->s.v.avelocity[0] = 0;
	newmis->s.v.avelocity[1] = 300;
	newmis->s.v.avelocity[2] = 0;
	setsize( newmis, 0, 0, 0, 0, 0, 0 );
	setorigin( newmis, PASSVEC3( self->s.v.origin ) );
	newmis->s.v.nextthink = g_globalvars.time + 30;
	newmis->s.v.think = ( func_t ) TeamFortress_AmmoboxRemove;//SUB_Remove;
	newmis->s.v.touch = ( func_t ) TeamFortress_AmmoboxTouch;
	newmis->s.v.skin = type - 1;
	if ( tf_data.birthday == 1 && g_random(  ) < 0.6 )
		setmodel( newmis, "progs/grenade3.mdl" );
	else
		setmodel( newmis, "progs/ammobox.mdl" );
}


void    TeamFortress_AmmoboxRemove(  )
{
	decrement_team_ammoboxes( self->team_no );
	SUB_Remove();
}

void TeamFortress_AmmoboxTouch(  )
{

	if ( g_globalvars.other == self->s.v.enemy && g_globalvars.time < self->s.v.health + 2 )
		return;
	if ( ( other->tfstate & TFSTATE_CANT_MOVE ) || ( other->tfstate & TFSTATE_AIMING ) )
		return;
	if ( strneq( other->s.v.classname, "player" ) )
		return;
	if ( other->s.v.health <= 0 )
		return;
	//num_world_ammoboxes = num_world_ammoboxes - 1;
	decrement_team_ammoboxes( self->team_no );
	if ( self->s.v.weapon == 0 )
	{
		G_sprint( other, 2, "You got " );
		if ( self->s.v.ammo_shells > 0 )
		{
			other->s.v.ammo_shells += self->s.v.ammo_shells;
			G_sprint( other, 2, "%d shells  ", ( int ) self->s.v.ammo_shells );
		}
		if ( self->s.v.ammo_nails > 0 )
		{
			other->s.v.ammo_nails += self->s.v.ammo_nails;
			G_sprint( other, 2, "%d nails  ", ( int ) self->s.v.ammo_nails );
		}
		if ( self->s.v.ammo_rockets > 0 )
		{
			other->s.v.ammo_rockets += self->s.v.ammo_rockets;
			G_sprint( other, 2, "%d rockets  ", ( int ) self->s.v.ammo_rockets );
		}
		if ( self->s.v.ammo_cells > 0 )
		{
			other->s.v.ammo_cells += self->s.v.ammo_cells;
			G_sprint( other, 2, "%d cells  ", ( int ) self->s.v.ammo_cells );
		}
		G_sprint( other, 2, " from %s's discarded pack.\n", PROG_TO_EDICT( self->s.v.enemy )->s.v.netname );
	} else
	{
		switch ( ( int ) self->s.v.weapon )
		{
		case 1:
			if ( other->s.v.ammo_shells >= TeamFortress_GetMaxAmmo( other, 256 ) )
				return;
			other->s.v.ammo_shells += self->aflag;
			G_sprint( other, 0, "You picked up %d shells\n", ( int ) self->aflag );
			break;
		case 2:
			if ( other->s.v.ammo_nails >= TeamFortress_GetMaxAmmo( other, 512 ) )
				return;
			other->s.v.ammo_nails += self->aflag;
			G_sprint( other, 0, "You picked up %d nails\n", ( int ) self->aflag );
			break;
		case 3:
			if ( other->s.v.ammo_rockets >= TeamFortress_GetMaxAmmo( other, 1024 ) )
				return;
			other->s.v.ammo_rockets += self->aflag;
			G_sprint( other, 0, "You picked up %d rockets\n", ( int ) self->aflag );
			break;
		case 4:
			if ( other->s.v.ammo_cells >= TeamFortress_GetMaxAmmo( other, 2048 ) )
				return;
			other->s.v.ammo_cells += self->aflag;
			G_sprint( other, 0, "You picked up %d cells\n", ( int ) self->aflag );
			break;


		}
	}
	bound_other_ammo( other );
	sound( other, 3, "weapons/lock4.wav", 1, 1 );
	stuffcmd( other, "bf\n" );
	dremove( self );
	self = other;
	W_SetCurrentAmmo(  );
}


void RemoveOldAmmoboxOld( int tno )
{
	int     index;
	gedict_t *old;

	index = num_team_ammoboxes[tno];
	if ( tno )
	{
		index = index - 20 / number_of_teams;
	} else
		index = index - 20;

	old = trap_find( world, FOFS( s.v.classname ), "ammobox" );
	while ( index > 0 )
	{
		if ( !old || old == world )
			return;
		if ( old->team_no == tno || !tno )
		{
			old->s.v.think = ( func_t ) TeamFortress_AmmoboxRemove;//SUB_Remove;
			old->s.v.nextthink = g_globalvars.time + 0.1;
			index--;
//			decrement_team_ammoboxes( old->team_no );
		}
		old = trap_find( old, FOFS( s.v.classname ), "ammobox" );
	}
}

void RemoveOldAmmobox( int tno )
{
	float time;
	gedict_t *old,*lastgood=NULL;

	time = g_globalvars.time + 35;

	for(old = world; (old = trap_find( old, FOFS( s.v.classname ), "ammobox" ));)
	{
	        if( old->team_no != tno && tno )
	        	continue;
		if( old->s.v.nextthink > time)
			continue;

		if( old->s.v.nextthink <= g_globalvars.time + 0.1 )
			continue;
		if( old->s.v.think != ( func_t ) TeamFortress_AmmoboxRemove )
		{
			G_conprintf("Warning: ammobox ent with bad remove func\n");
		}
		time = old->s.v.nextthink;
		lastgood = old;
	}
	if(lastgood)
	{
     		lastgood->s.v.think = ( func_t ) TeamFortress_AmmoboxRemove;
     		lastgood->s.v.nextthink = g_globalvars.time + 0.1;
	}

}

void increment_team_ammoboxes( int tno )
{
	if ( tno )
	{
		if ( ++( num_team_ammoboxes[tno] ) > 20 / number_of_teams )
			RemoveOldAmmobox( tno );

	} else
	{
		if ( ++( num_team_ammoboxes[tno] ) > 20 )
			RemoveOldAmmobox( tno );
	}
}

void decrement_team_ammoboxes( int tno )
{
	if ( num_team_ammoboxes[tno] )
		num_team_ammoboxes[tno]--;
}

void TeamFortress_AssaultWeapon(  )
{
	self->s.v.impulse = 0;
	if ( self->tfstate & TFSTATE_RELOADING )
		return;
	if ( !( self->weapons_carried & WEAP_ASSAULT_CANNON ) )
		return;
	if ( self->heat > 0 )
	{
		G_sprint( self, 2, "the assault cannon is still overheated.\n" );
		return;
	}
	if ( self->s.v.ammo_shells < 1 )
	{
		G_sprint( self, 2, "not enough ammo.\n" );
		return;
	}
	if ( self->s.v.ammo_cells < 6 )
	{
		G_sprint( self, 2, "not enough cells to power the assault cannon.\n" );
		return;
	}
	self->current_weapon = WEAP_ASSAULT_CANNON;
	W_SetCurrentAmmo(  );
}

const char   *explode_msgs[] = {
	"%s wouldn't let go of his gifts!\n",
	"No %s swallowing the grenade isn't very effective!\n",
	"Well %s don't quit your day job!\n",
	"Ummm, %s you're supposed to THROW the grenade!\n",
	"Ack! %s! The grenade is your friend for another reason!\n",
	"No %s! Assist your own suicide some other time!\n",
	"Hey %s you're not THAT heavy!\n",
	"Yes %s the grenade does explode on '3'!\n",
	"You do realize %s you can blow your cover in easier ways!\n",
	"Hey %s study grenade dynamics on your own time!\n",
	"No %s throw the grenade, not the pin!\n",
	""
};
void TeamFortress_ExplodePerson(  )
{
	gedict_t *te;
	gedict_t *owner = PROG_TO_EDICT( self->s.v.owner );

	owner->tfstate = owner->tfstate - ( ( int ) owner->tfstate & TFSTATE_GRENPRIMED );
	KickPlayer( -2, owner );
	newmis = spawn(  );
	g_globalvars.newmis = EDICT_TO_PROG( newmis );
	newmis->s.v.movetype = MOVETYPE_BOUNCE;
	newmis->s.v.solid = SOLID_BBOX;
	newmis->s.v.classname = "grenade";
	newmis->team_no = owner->team_no;
	newmis->s.v.owner = self->s.v.owner;
	SetVector( newmis->s.v.velocity, 0, 0, 0 );

	vectoangles( newmis->s.v.velocity, newmis->s.v.angles );
	newmis->s.v.think = ( func_t ) SUB_Null;
	newmis->s.v.nextthink = g_globalvars.time + 0.1;
	switch ( ( int ) self->s.v.weapon )
	{
	case GR_TYPE_NORMAL:
		newmis->s.v.touch = ( func_t ) NormalGrenadeTouch;
		newmis->s.v.think = ( func_t ) NormalGrenadeExplode;
		newmis->s.v.skin = 0;
		SetVector( newmis->s.v.avelocity, 300, 300, 300 );
		setmodel( newmis, "progs/hgren2.mdl" );
		break;

	case GR_TYPE_CONCUSSION:
		newmis->s.v.touch = ( func_t ) ConcussionGrenadeTouch;
		newmis->s.v.think = ( func_t ) ConcussionGrenadeExplode;
		newmis->s.v.skin = 1;
		SetVector( newmis->s.v.avelocity, 300, 300, 300 );
		setmodel( newmis, "progs/hgren2.mdl" );
		break;
	case GR_TYPE_NAIL:
		newmis->s.v.touch = ( func_t ) NailGrenadeTouch;
		newmis->s.v.think = ( func_t ) NailGrenadeExplode;
		newmis->s.v.skin = 1;
		SetVector( newmis->s.v.avelocity, 0, 300, 0 );
		setmodel( newmis, "progs/biggren.mdl" );
		break;
	case GR_TYPE_MIRV:
		newmis->s.v.touch = ( func_t ) MirvGrenadeTouch;
		newmis->s.v.think = ( func_t ) MirvGrenadeExplode;
		newmis->s.v.skin = 0;
		SetVector( newmis->s.v.avelocity, 0, 300, 0 );
		setmodel( newmis, "progs/biggren.mdl" );
		break;
	case GR_TYPE_NAPALM:
		newmis->s.v.touch = ( func_t ) NapalmGrenadeTouch;
		newmis->s.v.think = ( func_t ) NapalmGrenadeExplode;
		newmis->s.v.skin = 2;
		SetVector( newmis->s.v.avelocity, 0, 300, 0 );
		setmodel( newmis, "progs/biggren.mdl" );
		break;
	case GR_TYPE_FLARE:
		G_sprint( owner, 2, "Flare lit.\n" );
		te = spawn(  );
		te->s.v.touch = ( func_t ) SUB_Null;
		te->s.v.think = ( func_t ) RemoveFlare;
		te->s.v.nextthink = g_globalvars.time + 25;
		te->s.v.owner = self->s.v.owner;
		te->s.v.solid = 0;
		owner->s.v.effects = ( int ) owner->s.v.effects | EF_BRIGHTLIGHT;
		dremove( self );
		dremove( newmis );
		g_globalvars.newmis = EDICT_TO_PROG( world );
		break;
	case GR_TYPE_GAS:
		newmis->s.v.touch = ( func_t ) GasGrenadeTouch;
		newmis->s.v.think = ( func_t ) GasGrenadeExplode;
		newmis->s.v.skin = 2;
		SetVector( newmis->s.v.avelocity, 300, 300, 300 );
		setmodel( newmis, "progs/grenade2.mdl" );
		break;
	case GR_TYPE_EMP:
		newmis->s.v.touch = ( func_t ) EMPGrenadeTouch;
		newmis->s.v.think = ( func_t ) EMPGrenadeExplode;
		newmis->s.v.skin = 4;
		SetVector( newmis->s.v.avelocity, 300, 300, 300 );
		setmodel( newmis, "progs/grenade2.mdl" );
		break;
	case GR_TYPE_FLASH:
		newmis->s.v.touch = ( func_t ) FlashGrenadeTouch;
		newmis->s.v.think = ( func_t ) FlashGrenadeExplode;
		newmis->s.v.skin = 1;
		SetVector( newmis->s.v.avelocity, 300, 300, 300 );
		setmodel( newmis, "progs/grenade2.mdl" );
		break;
	case GR_TYPE_CALTROPS:
		newmis->s.v.touch = ( func_t ) CaltropTouch;
		newmis->s.v.think = ( func_t ) ScatterCaltrops;
		break;

	}
	setsize( newmis, 0, 0, 0, 0, 0, 0 );
	setorigin( newmis, PASSVEC3( owner->s.v.origin ) );
	if ( tf_data.birthday == 1 )
		G_bprint( 1, explode_msgs[0], owner->s.v.netname );
	else
	{
		switch ( owner->playerclass )
		{
		case PC_SCOUT:
			if ( self->s.v.weapon != GR_TYPE_CALTROPS )
				G_bprint( 1, explode_msgs[1], owner->s.v.netname );
			break;
		case PC_SNIPER:
		case PC_SOLDIER:
		case PC_DEMOMAN:
		case PC_MEDIC:
		case PC_HVYWEAP:
		case PC_PYRO:
		case PC_SPY:
		case PC_ENGINEER:
			G_bprint( 1, explode_msgs[owner->playerclass], owner->s.v.netname );
			break;
		default:
			G_bprint( 1, explode_msgs[10], owner->s.v.netname );
			break;
		}
	}
	dremove( self );
}

void NormalGrenadeTouch(  )
{
	if ( other == PROG_TO_EDICT( self->s.v.owner ) )
		return;
	sound( self, 1, "weapons/bounce.wav", 1, 1 );
	if ( ( self->s.v.velocity[0] == 0 ) && ( self->s.v.velocity[1] == 0 ) && ( self->s.v.velocity[2] == 0 ) )
		SetVector( self->s.v.avelocity, 0, 0, 0 );
}

void NormalGrenadeExplode(  )
{
	tf_data.deathmsg = DMSG_GREN_HAND;
	T_RadiusDamage( self, PROG_TO_EDICT( self->s.v.owner ), 180, world );
	trap_WriteByte( MSG_MULTICAST, SVC_TEMPENTITY );
	trap_WriteByte( MSG_MULTICAST, TE_EXPLOSION );
	trap_WriteCoord( MSG_MULTICAST, self->s.v.origin[0] );
	trap_WriteCoord( MSG_MULTICAST, self->s.v.origin[1] );
	trap_WriteCoord( MSG_MULTICAST, self->s.v.origin[2] );
	trap_multicast( PASSVEC3( self->s.v.origin ), 1 );
	dremove( self );
}
void TeamFortress_DisplayDetectionItems(  )
{
	gedict_t *Goal;
	gedict_t *te;

	Goal = trap_find( world, FOFS( s.v.classname ), "info_tfdetect" );

	if ( !Goal )
		return;
	if ( Goal->display_item_status1 )
	{
		te = Finditem( Goal->display_item_status1 );
		if ( te != world )
			DisplayItemStatus( Goal, self, te );
		else
			G_sprint( self, 2, "Item is missing.\n" );
	} else
		return;

	if ( Goal->display_item_status2 )
	{
		te = Finditem( Goal->display_item_status2 );
		if ( te != world )
			DisplayItemStatus( Goal, self, te );
		else
			G_sprint( self, 2, "Item is missing.\n" );
	} else
		return;

	if ( Goal->display_item_status3 )
	{
		te = Finditem( Goal->display_item_status3 );
		if ( te != world )
			DisplayItemStatus( Goal, self, te );
		else
			G_sprint( self, 2, "Item is missing.\n" );
	} else
		return;
	if ( Goal->display_item_status4 )
	{
		te = Finditem( Goal->display_item_status4 );
		if ( te != world )
			DisplayItemStatus( Goal, self, te );
		else
			G_sprint( self, 2, "Item is missing.\n" );
	} else
		return;

}


void BioInfection_Decay(  )
{
	gedict_t *te;
	gedict_t *Bio;
	gedict_t *owner = PROG_TO_EDICT( self->s.v.owner );
	gedict_t *enemy = PROG_TO_EDICT( self->s.v.enemy );

	if ( ( teamplay & TEAMPLAY_NOEXPLOSIVE ) && owner->team_no == enemy->team_no && owner->team_no )
	{
		owner->tfstate = ( int ) owner->tfstate - ( ( int ) owner->tfstate & TFSTATE_INFECTED );
		dremove( self );
		return;
	} else
	{
		if ( self->invincible_finished > g_globalvars.time )
		{
			owner->tfstate = ( int ) owner->tfstate - ( ( int ) owner->tfstate & TFSTATE_INFECTED );
			dremove( self );
			return;
		}
	}
	if ( !( owner->tfstate & TFSTATE_INFECTED ) || owner->playerclass == PC_MEDIC )
	{
		dremove( self );
		return;
	}

	for ( te = trap_findradius( world, owner->s.v.origin, 80 ); te; te = trap_findradius( te, owner->s.v.origin, 80 ) )
	{
		if ( te == owner )
			continue;
		if ( strneq( te->s.v.classname, "player" ) )
			continue;
		if ( te->s.v.deadflag || !te->playerclass )
			continue;
		if ( te->tfstate & TFSTATE_INFECTED )
			continue;
		if ( te->playerclass == PC_MEDIC )
			continue;
		if ( ( teamplay & 16 ) && owner->team_no == enemy->team_no && owner->team_no )
			continue;
		Bio = spawn(  );
		Bio->s.v.nextthink = 2;
		Bio->s.v.think = ( func_t ) BioInfection_Decay;
		Bio->s.v.owner = EDICT_TO_PROG( te );
		Bio->s.v.classname = "timer";
		Bio->s.v.enemy = self->s.v.enemy;
		te->tfstate |= TFSTATE_INFECTED;
		te->infection_team_no = owner->infection_team_no;
		G_sprint( te, 1, "You have been infected by %s!\n", owner->s.v.netname );
		G_sprint( owner, 1, "You have infected %s!\n", te->s.v.netname );
	}
	self->s.v.nextthink = g_globalvars.time + 3;
	tf_data.deathmsg = DMSG_BIOWEAPON;
	TF_T_Damage( owner, self, enemy, 8, 1, 0 );
	SpawnBlood( owner->s.v.origin, 30 );
}


void BioInfection_MonsterDecay(  )
{
	self->s.v.nextthink = g_globalvars.time + 2;
	T_Damage( PROG_TO_EDICT( self->s.v.enemy ), self, PROG_TO_EDICT( self->s.v.owner ), 5 );
	SpawnBlood( PROG_TO_EDICT( self->s.v.enemy )->s.v.origin, 20 );
	if ( PROG_TO_EDICT( self->s.v.enemy )->s.v.health < 1 )
		dremove( self );
}

void TeamFortress_Alias( char *halias, int himpulse1, int himpulse2 )
{

	stuffcmd( self, "alias %s \"impulse %d", halias, himpulse1 );
	if ( himpulse2 )
	{
		stuffcmd( self, ";wait; impulse %d", himpulse2 );
	}
	stuffcmd( self, "\"\n" );
}

void TeamFortress_Regenerate(  )
{
	gedict_t *owner = PROG_TO_EDICT( self->s.v.owner );

	if ( owner->playerclass != PC_MEDIC )
		return;
	self->s.v.nextthink = g_globalvars.time + PC_MEDIC_REGEN_TIME;

	if ( owner->has_disconnected == 1 )
	{
		dremove( self );
		return;
	}
	if ( owner->s.v.health >= owner->s.v.max_health )
		return;
	if ( !owner->ammo_medikit )
		return;
	if ( owner->ammo_medikit < PC_MEDIC_REGEN_AMOUNT )
	{
		owner->s.v.health += owner->ammo_medikit;
		owner->ammo_medikit = 0;
	} else
	{
		owner->s.v.health += PC_MEDIC_REGEN_AMOUNT;
		owner->ammo_medikit -= PC_MEDIC_REGEN_AMOUNT;
	}
	if ( owner->s.v.health > owner->s.v.max_health )
		owner->s.v.health = owner->s.v.max_health;

	if( owner->current_weapon == WEAP_MEDIKIT )
		owner->s.v.currentammo = owner->ammo_medikit;
}

void TeamFortress_RegenerateCells(  )
{
	gedict_t *owner = PROG_TO_EDICT( self->s.v.owner );

	if ( owner->playerclass != PC_SPY )
		return;

	self->s.v.nextthink = g_globalvars.time + PC_SPY_CELL_REGEN_TIME;
	if ( owner->is_undercover == 1 )
	{
		if ( !owner->s.v.ammo_cells )
		{
			owner->is_undercover = 0;
			owner->s.v.modelindex = modelindex_player;
			owner->s.v.items = ( int ) owner->s.v.items - ( ( int ) owner->s.v.items & IT_INVISIBILITY );
		} else
		{
			owner->s.v.ammo_cells -= PC_SPY_CELL_USAGE;
			if ( owner->s.v.ammo_cells < 0 )
				owner->s.v.ammo_cells = 0;
		}
	} else
	{
		if ( owner->s.v.ammo_cells >= owner->maxammo_cells )
			return;
		owner->s.v.ammo_cells += PC_SPY_CELL_REGEN_AMOUNT;
		if ( owner->s.v.ammo_cells > owner->maxammo_cells )
			owner->s.v.ammo_cells = owner->maxammo_cells;
	}
}


void TeamFortress_AutoID(  )
{
	gedict_t *oldself;

	self->s.v.nextthink = g_globalvars.time + TF_AUTOIDTIME;
	oldself = self;
	self = PROG_TO_EDICT( self->s.v.owner );
	TeamFortress_ID(  );
	self = oldself;
}


void PlayerObserverMode(  )
{
	self->current_menu = MENU_DEFAULT;
	self->s.v.impulse = 0;
	self->playerclass = 0;
	self->lives = 0;
	self->team_no = -1;
	self->s.v.flags = FL_CLIENT | FL_NOTARGET | FL_ONGROUND;
	self->s.v.waterlevel = 3;
	self->s.v.takedamage = 0;
	self->s.v.solid = SOLID_NOT;
	self->s.v.movetype = MOVETYPE_NONE;//!!!REMOVE ME MOVETYPE_NOCLIP;
	G_sprint( self, 2, "Observer mode\n" );
	CenterPrint( self, "\n" );
	stuffcmd( self, "cl_rollangle 0\n" );
}


float crossproduct( vec3_t veca, vec3_t vecb )
{
	float   result;

	result = veca[0] * vecb[1] - vecb[0] * veca[1];
	return result;
}

void TF_AddFrags( gedict_t * pl, int fr )
{
	gedict_t *e;

	if ( intermission_running || intermission_exittime > g_globalvars.time )
		return;
	pl->real_frags += fr;
	if ( !pl->team_no || pl->team_no > 4 )
		return;

	if ( tf_data.toggleflags & TFLAG_FULLTEAMSCORE )
	{
		teamscores[pl->team_no] += fr;
	}
	teamfrags[pl->team_no] += fr;

	if ( tf_data.toggleflags & TFLAG_FULLTEAMSCORE )
	{
		e = trap_find( world, FOFS( s.v.classname ), "player" );
		while ( e )
		{
			if ( e->team_no == pl->team_no )
				e->s.v.frags = TeamFortress_TeamGetScore( e->team_no );
			e = trap_find( e, FOFS( s.v.classname ), "player" );
		}
	} else
	{
		if ( !( tf_data.toggleflags & TFLAG_TEAMFRAGS ) )
			pl->s.v.frags = pl->real_frags;
	}
}

const char   *ec_strings[] = {
	"",
	"exec scout.cfg\n",
	"exec sniper.cfg\n",
	"exec soldier.cfg\n",
	"exec demoman.cfg\n",
	"exec medic.cfg\n",
	"exec hwguy.cfg\n",
	"exec pyro.cfg\n",
	"exec spy.cfg\n",
	"exec engineer.cfg\n",
	""
};

void TeamFortress_ExecClassScript( gedict_t * p )
{

	if( ! (self->settings_bits  & TF_EXEC_CLASS_MASK) )
	        return;
	if ( !p->playerclass || p->playerclass > 9 )
		return;
	stuffcmd( p, ec_strings[p->playerclass] );
}

void TeamFortress_ExecMapScript( gedict_t * p )
{

	if( ! (self->settings_bits  & TF_EXEC_MAP_MASK) )
	        return;

	stuffcmd( p, "exec mapdefault.cfg\n" );
	stuffcmd( p, "exec %s.cfg", g_globalvars.mapname );
}

void KickCheater( gedict_t * p )
{
	stuffcmd( p, "disconnect\n" );
	stuffcmd( p, "cmd drop\n" );

	g_globalvars.msg_entity = EDICT_TO_PROG( p );
	trap_WriteByte( MSG_ONE, SVC_TEMPENTITY );
	trap_WriteByte( MSG_ONE, TE_LIGHTNING1 );
	trap_WriteCoord( MSG_ONE, self->s.v.origin[0] );
	p->has_disconnected = 1;
	p->s.v.touch = ( func_t ) SUB_Null;
	p->s.v.health = 0;
	p->s.v.solid = 0;
	p->tfstate = p->tfstate | TFSTATE_CANT_MOVE;
	TeamFortress_SetSpeed( p );
	TeamFortress_RemoveTimers(  );
}


void TeamFortress_PrepareForArenaRespawn(  )
{
        if( !tf_data.arena_mode )
                return;

	self->s.v.ammo_rockets = self->maxammo_rockets;
	self->s.v.ammo_nails  = self->maxammo_nails  ;
	self->s.v.ammo_shells = self->maxammo_shells ;
	self->s.v.ammo_cells  = self->maxammo_cells  ;
	self->ammo_detpack    = self->maxammo_detpack;
	self->ammo_medikit    = self->maxammo_medikit;

	//self->armorclass |= class_set[pc].armorclass;
	self->s.v.armortype = self->armor_allowed;
	self->s.v.armorvalue =self->maxarmor;

	self->no_grenades_1 = 4;
	self->no_grenades_2 = 4;
	bound_other_ammo( self );
	W_SetCurrentAmmo(  );
}
