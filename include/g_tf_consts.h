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
 *  $Id: g_tf_consts.h,v 1.11 2005-05-17 03:56:00 AngelD Exp $
 */
// TeamFortress State Flags
#define TFSTATE_GRENPRIMED  1  // Whether the player has a primed grenade
#define TFSTATE_RELOADING  2  // Whether the player is reloading
#define TFSTATE_ALTKILL   4   // #TRUE if killed with a weapon not in self.weapon: NOT USED ANYMORE
#define TFSTATE_RANDOMPC  8   // Whether Playerclass is random, new one each respawn
#define TFSTATE_INFECTED  16  // set when player is infected by the bioweapon
#define TFSTATE_INVINCIBLE  32  // Player has permanent Invincibility (Usually by GoalItem)
#define TFSTATE_INVISIBLE  64  // Player has permanent Invisibility (Usually by GoalItem)
#define TFSTATE_QUAD   128 // Player has permanent Quad Damage (Usually by GoalItem)
#define TFSTATE_RADSUIT   256 // Player has permanent Radsuit (Usually by GoalItem)
#define TFSTATE_BURNING   512 // Is on fire
#define TFSTATE_GRENTHROWING 1024  // is throwing a grenade
#define TFSTATE_AIMING   2048  // is using the laser sight
#define TFSTATE_ZOOMOFF   4096  // doesn't want the FOV changed when zooming
#define TFSTATE_RESPAWN_READY 8192  // is waiting for respawn, and has pressed fire
#define TFSTATE_HALLUCINATING  16384  // set when player is hallucinating
#define TFSTATE_TRANQUILISED   32768  // set when player is tranquilised
#define TFSTATE_CANT_MOVE    65536  // set when player is setting a detpack

// Defines used by TF_T_Damage (see combat.qc)
#define TF_TD_IGNOREARMOUR 1  // Bypasses the armour of the target
#define TF_TD_NOTTEAM  2  // Doesn't damage a team member (indicates direct fire weapon)
#define TF_TD_NOTSELF  4  // Doesn't damage self

#define TF_TD_OTHER   0  // Ignore armorclass
#define TF_TD_SHOT   1  // Bullet damage
#define TF_TD_NAIL   2  // Nail damage
#define TF_TD_EXPLOSION  4  // Explosion damage
#define TF_TD_ELECTRICITY 8  // Electric damage
#define TF_TD_FIRE   16  // Fire damage
#define TF_TD_NOSOUND  256 // Special damage. Makes no sound/painframe, etc


/*==================================================*/
/* Toggleable Game Settings       */
/*==================================================*/

#define TF_RESPAWNDELAY1 5  // seconds of waiting before player can respawn
#define TF_RESPAWNDELAY2 10  // seconds of waiting before player can respawn
#define TF_RESPAWNDELAY3 20  // seconds of waiting before player can respawn

#define TEAMPLAY_NORMAL		 1
#define TEAMPLAY_HALFDIRECT	 2
#define TEAMPLAY_NODIRECT	 4
#define TEAMPLAY_HALFEXPLOSIVE	 8
#define TEAMPLAY_NOEXPLOSIVE	 16
#define TEAMPLAY_LESSPLAYERSHELP 32
#define TEAMPLAY_LESSSCOREHELP	 64

// FortressMap stuff
#define TEAM1_CIVILIANS 1 
#define TEAM2_CIVILIANS 2
#define TEAM3_CIVILIANS 4 
#define TEAM4_CIVILIANS 8 

// Defines for the playerclass
#define PC_UNDEFINED 0 

#define PC_SCOUT  1 
#define PC_SNIPER  2 
#define PC_SOLDIER  3 
#define PC_DEMOMAN  4 
#define PC_MEDIC  5 
#define PC_HVYWEAP  6 
#define PC_PYRO   7
#define PC_SPY   8
#define PC_ENGINEER  9

// Insert new class definitions here

// PC_RANDOM _MUST_ be the third last class
#define PC_RANDOM  10   // Random playerclass
#define PC_CIVILIAN  11  // Civilians are a special class. They cannot
        // be chosen by players, only enforced by maps
#define PC_LASTCLASS 12   // Use this as the high-boundary for any loops
        // through the playerclass.
// Flare skin numbers
//float TF_FLARE_LIT  = 0;
//float TF_FLARE_OFF  = 1;

/*==================================================*/
/* Impulse Defines                           */
/*==================================================*/
// Alias check to see whether they already have the aliases
#define TF_ALIAS_CHECK  13 

// CTF Support Impulses
#define HOOK_IMP1  22
#define FLAG_INFO  23
#define HOOK_IMP2  39

// Axe
#define AXE_IMP   40

// Camera Impulse
#define TF_CAM_TARGET   50
#define TF_CAM_ZOOM    51
#define TF_CAM_ANGLE   52
#define TF_CAM_VEC    53
#define TF_CAM_PROJECTILE  54
#define TF_CAM_PROJECTILE_Z  55
#define TF_CAM_REVANGLE   56
#define TF_CAM_OFFSET   57
#define TF_CAM_DROP    58 
#define TF_CAM_FADETOBLACK  59
#define TF_CAM_FADEFROMBLACK 60
#define TF_CAM_FADETOWHITE  61
#define TF_CAM_FADEFROMWHITE 62

// Status Bar Resolution Settings.  Same as CTF to maintain ease of use.
#define TF_STATUSBAR_RES_START 71
#define TF_STATUSBAR_RES_END 81

// Added to PC_??? to get impulse to use if this clashes with your 
// own impulses, just change this value, not the PC_??
#define TF_CHANGEPC   100 
// The next few impulses are all the class selections
//PC_SCOUT  101 
//PC_SNIPER  102 
//PC_SOLDIER 103 
//PC_DEMOMAN 104 
//PC_MEDIC  105 
//PC_HVYWEAP 106 
//PC_PYRO  107 
//PC_RANDOM  108
//PC_CIVILIAN 109  // Cannot be used
//PC_SPY  110
//PC_ENGINEER 111

// Help impulses
#define TF_DISPLAYLOCATION  118
#define TF_STATUS_QUERY  119

#define TF_HELP_MAP   131

// Information impulses
#define TF_INVENTORY  135
#define TF_SHOWTF   136 
#define TF_SHOWLEGALCLASSES 137

// Team Impulses
#define TF_TEAM_1   140   // Join Team 1
#define TF_TEAM_2   141   // Join Team 2
#define TF_TEAM_3   142   // Join Team 3
#define TF_TEAM_4   143   // Join Team 4
#define TF_TEAM_CLASSES  144   // Impulse to display team classes
#define TF_TEAM_SCORES  145   // Impulse to display team scores
#define TF_TEAM_LIST  146   // Impulse to display the players in each team.

// Grenade Impulses
#define TF_GRENADE_1  150   // Prime grenade type 1
#define TF_GRENADE_2  151   // Prime grenade type 2
#define TF_GRENADE_T  152   // Throw primed grenade

// Impulses for new items
#define TF_SCAN    159  // Scanner Pre-Impulse
#define TF_SCAN_ENEMY  160  // Impulses to toggle scanning of enemies
#define TF_SCAN_FRIENDLY 161  // Impulses to toggle scanning of friendlies 
#define TF_SCAN_10   162  // Scan using 10 enery (1 cell)
#define TF_SCAN_30   163  // Scan using 30 energy (2 cells)
#define TF_SCAN_100   164  // Scan using 100 energy (5 cells)

#define TF_POST_SCANF_OFF 162   //"Friendly Scanning disabled.\n"
#define TF_POST_SCANF_ON  163   //"Friendly Scanning enabled.\n"
#define TF_POST_SCANE_OFF 164   //"Enemy Scanning disabled.\n"
#define TF_POST_SCANE_ON  165   //"Enemy Scanning enabled.\n"
#define TF_POST_AUTOSCAN_OFF  166   //"Scanner off.\n"
#define TF_POST_AUTOSCAN_ON   167   //"Scanner on.\n"

#define TF_DETPACK_5  165  // Detpack set to 5 seconds
#define TF_DETPACK_20  166  // Detpack set to 20 seconds
#define TF_DETPACK_50  167  // Detpack set to 50 seconds
#define TF_DETPACK   168  // Detpack Pre-Impulse
#define TF_DETPACK_STOP  169  // Impulse to stop setting detpack
#define TF_PB_DETONATE  170  // Detonate Pipebombs

// Special skill
#define TF_SPECIAL_SKILL 171

// Ammo Drop impulse
#define TF_DROP_AMMO        172

// Reload impulse
#define TF_RELOAD   173

// auto-zoom toggle
#define TF_AUTOZOOM   174

// drop/pass commands
#define TF_DROPKEY   175

// Select Medikit  
#define TF_MEDIKIT   176

// Spy Impulses
#define TF_SPY_SPY   177  // On net, go invisible, on LAN, change skin/color
#define TF_SPY_DIE   178  // Feign Death

// Engineer Impulses
#define TF_ENGINEER_BUILD 179
#define TF_ENGINEER_SANDBAG 180

// Medic!! 
#define TF_MEDIC_HELPME  181

// Status bar
#define TF_STATUSBAR_ON  182
#define TF_STATUSBAR_OFF 183

// Discard impulse
#define TF_DISCARD      184

// ID Player impulse
#define TF_ID       185

/*==================================================*/
/* Colors           */
/*==================================================*/
#define WHITE   1
#define BROWN   2
#define BLUE   3
#define GREEN    4
#define RED   5
#define TAN      6
#define PINK     7
#define ORANGE   8
#define PURPLE   9
#define DARKPURPLE  10
#define GREY     11
#define DARKGREEN   12
#define YELLOW   13
#define DARKBLUE    14

/*==================================================*/
/* Defines for the ENGINEER's Building ability  */
/*==================================================*/
// Ammo costs
#define AMMO_COST_SHELLS  3  // Metal needed to make 1 shell
#define AMMO_COST_NAILS   2
#define AMMO_COST_ROCKETS  5
#define AMMO_COST_CELLS   5

// Building types
#define BUILD_DISPENSER   1
#define BUILD_SENTRYGUN   2
//#define BUILD_MORTAR   3

// Building metal costs
#define BUILD_COST_DISPENSER 100  // Built thru the menu
#define BUILD_COST_SENTRYGUN 130  // Built thru the menu
//#define BUILD_COST_MORTAR  150  // Built thru the menu

//#define BUILD_COST_SANDBAG  20  // Built with a separate alias

// Building times
#define BUILD_TIME_DISPENSER 2  // 2 seconds to build
#define BUILD_TIME_SENTRYGUN 5  // 5 seconds to build
//#define BUILD_TIME_MORTAR  5  // 5 seconds to build

// Building health levels
#define BUILD_HEALTH_DISPENSER 150  // Built thru the menu
#define BUILD_HEALTH_SENTRYGUN 150  // Built thru the menu
#define BUILD_HEALTH_MORTAR  200  // Built thru the menu

// Dispenser's maximum carrying capability
#define BUILD_DISPENSER_MAX_SHELLS  400
#define BUILD_DISPENSER_MAX_NAILS   600
#define BUILD_DISPENSER_MAX_ROCKETS 300
#define BUILD_DISPENSER_MAX_CELLS   400
#define BUILD_DISPENSER_MAX_ARMOR   500

/*==================================================*/
/* Ammo quantities for dropping               */
/*==================================================*/
#define DROP_SHELLS   20
#define DROP_NAILS    20
#define DROP_ROCKETS  10
#define DROP_CELLS    10
#define DROP_ARMOR   40

/*==================================================*/
/* Team Defines                   */
/*==================================================*/
#define TM_MAX_NO 4    // Max number of teams. Simply changing this value isn't enough.
        // A new global to hold new team colors is needed, and more flags
        // in the spawnpoint spawnflags may need to be used.
        // Basically, don't change this unless you know what you're doing :)

/*==================================================*/
/* New Weapon Defines                          */
/*==================================================*/

#define WEAP_HOOK    1
#define WEAP_BIOWEAPON   2
#define WEAP_MEDIKIT   4
#define WEAP_SPANNER   8
#define WEAP_AXE    16
#define WEAP_SNIPER_RIFLE  32
#define WEAP_AUTO_RIFLE   64
#define WEAP_SHOTGUN   128
#define WEAP_SUPER_SHOTGUN  256
#define WEAP_NAILGUN   512
#define WEAP_SUPER_NAILGUN  1024
#define WEAP_GRENADE_LAUNCHER 2048
#define WEAP_FLAMETHROWER  4096
#define WEAP_ROCKET_LAUNCHER 8192
#define WEAP_INCENDIARY   16384
#define WEAP_ASSAULT_CANNON  32768
#define WEAP_LIGHTNING   65536
#define WEAP_DETPACK   131072
#define WEAP_TRANQ    262144
#define WEAP_LASER    524288
// still room for 12 more weapons
// but we can remove some by giving the weapons
// a weapon mode (like the rifle)

/*==================================================*/
/* New Weapon Related Defines                  */
/*==================================================*/
// shots per reload 
#define RE_SHOTGUN   8 
#define RE_SUPER_SHOTGUN 16 // 8 shots
#define RE_GRENADE_LAUNCHER 6 
#define RE_ROCKET_LAUNCHER 4 

// reload times
#define RE_SHOTGUN_TIME    2 
#define RE_SUPER_SHOTGUN_TIME  3 
#define RE_GRENADE_LAUNCHER_TIME 4 
#define RE_ROCKET_LAUNCHER_TIME  5 

// Maximum velocity you can move and fire the Sniper Rifle
#define WEAP_SNIPER_RIFLE_MAX_MOVE 50 

// Medikit
#define WEAP_MEDIKIT_HEAL 200  // Amount medikit heals per hit
#define WEAP_MEDIKIT_OVERHEAL 50 // Amount of superhealth over max_health the medikit will dispense

// Spanner
#define WEAP_SPANNER_REPAIR 10

// Detpack
#define WEAP_DETPACK_DISARMTIME 3    //Time it takes to disarm a Detpack
#define WEAP_DETPACK_SETTIME  3    // Time it takes to set a Detpack
#define WEAP_DETPACK_SIZE  1500 
#define WEAP_DETPACK_BITS_NO  12   // Bits that detpack explodes into

// Tranquiliser Gun
#define TRANQ_TIME   15

// Grenades
#define GR_PRIMETIME  3 
#define GR_TYPE_NONE  0 
#define GR_TYPE_NORMAL  1 
#define GR_TYPE_CONCUSSION 2 
#define GR_TYPE_NAIL  3 
#define GR_TYPE_MIRV  4 
#define GR_TYPE_NAPALM  5 
#define GR_TYPE_FLARE  6 
#define GR_TYPE_GAS   7
#define GR_TYPE_EMP   8
#define GR_TYPE_FLASH  9
#define GR_TYPE_CALTROPS  10

// Defines for WeaponMode
#define GL_NORMAL 0 
#define GL_PIPEBOMB 1 

// Defines for Concussion Grenade
//#define GR_CONCUSS_TIME 5 
//#define GR_CONCUSS_DEC 20 

// Defines for the Gas Grenade
//#define GR_HALLU_TIME 0.5
//#define GR_HALLU_DEC 2.5

/*==================================================*/
/* New Items                            */
/*==================================================*/
#define NIT_SCANNER    1 

#define NIT_SILVER_DOOR_OPENED  IT_KEY1 // 131072 
#define NIT_GOLD_DOOR_OPENED  IT_KEY2 // 262144

/*==================================================*/
/* New Item Flags                           */
/*==================================================*/
#define NIT_SCANNER_ENEMY  1  // Detect enemies
#define NIT_SCANNER_FRIENDLY 2  // Detect friendlies (team members)
#define NIT_SCANNER_MOVEMENT 4  // Motion detection. Only report moving entities.

/*==================================================*/
/* New Item Related Defines                      */
/*==================================================*/
#define NIT_SCANNER_POWER   100 // The amount of power spent on a scan with the scanner
          // is multiplied by this to get the scanrange.
#define NIT_SCANNER_MAXCELL   50  // The maximum number of cells than can be used in one scan
#define NIT_SCANNER_MIN_MOVEMENT 50  // The minimum velocity an entity must have to be detected
          // by scanners that only detect movement

/*==================================================*/
/* Variables used for New Weapons and Reloading     */
/*==================================================*/
// Armor Classes : Bitfields. Use the "armorclass" of armor for the Armor Type.
#define AT_SAVESHOT   1   // Kevlar    : Reduces bullet damage by 15%
#define AT_SAVENAIL   2   // Wood :)   : Reduces nail damage by 15%
#define AT_SAVEEXPLOSION 4   // Blast     : Reduces explosion damage by 15%
#define AT_SAVEELECTRICITY 8  // Shock  : Reduces electricity damage by 15%
#define AT_SAVEFIRE   16  // Asbestos  : Reduces fire damage by 15%

/*==========================================================================*/
/* TEAMFORTRESS CLASS DETAILS            */
/*==========================================================================*/
#define PC_MEDIC_REGEN_TIME   3   // Number of seconds between each regen.
#define PC_MEDIC_REGEN_AMOUNT  2  // Amount of health regenerated each regen.

#define PC_SPY_CELL_REGEN_TIME  5 
#define PC_SPY_CELL_REGEN_AMOUNT 1
#define PC_SPY_CELL_USAGE   3 // Amount of cells spent while invisible
#define PC_SPY_GO_UNDERCOVER_TIME 4 // Time it takes to go undercover

/*==========================================================================*/
/* TEAMFORTRESS GOALS              */
/*==========================================================================*/
// For all these defines, see the tfortmap.txt that came with the zip
// for complete descriptions.
// Defines for Goal Activation types : goal_activation (in goals)
#define TFGA_TOUCH   1  // Activated when touched
#define TFGA_TOUCH_DETPACK 2  // Activated when touched by a detpack explosion
#define TFGA_REVERSE_AP  4  // Activated when AP details are _not_ met
#define TFGA_SPANNER  8  // Activated when hit by an engineer's spanner

// Defines for Goal Effects types : goal_effect
#define TFGE_AP      1  // AP is affected. Default.
#define TFGE_AP_TEAM    2  // All of the AP's team.
#define TFGE_NOT_AP_TEAM   4  // All except AP's team.
#define TFGE_NOT_AP     8  // All except AP.
#define TFGE_WALL     16 // If set, walls stop the Radius effects
#define TFGE_SAME_ENVIRONMENT 32 // If set, players in a different environment to the Goal are not affected
#define TFGE_TIMER_CHECK_AP   64 // If set, Timer Goals check their critera for all players fitting their effects

// Defines for Goal Result types : goal_result
#define TFGR_SINGLE    1  // Goal can only be activated once
#define TFGR_ADD_BONUSES  2  // Any Goals activated by this one give their bonuses
#define TFGR_ENDGAME   4  // Goal fires Intermission, displays scores, and ends level
#define TFGR_NO_ITEM_RESULTS 8 // GoalItems given by this Goal don't do results
#define TFGR_REMOVE_DISGUISE 16 // Prevent/Remove undercover from any Spy
#define TFGR_FORCE_RESPAWN 32 // forces anyone affected by the goal to simply respawn. The player doesn't die... just respawns.

// Defines for Goal Group Result types : goal_group
// None!
// But I'm leaving this variable in there, since it's fairly likely
// that some will show up sometime.

// Defines for Goal Item types, : goal_activation (in items)
#define TFGI_GLOW   1   // Players carrying this GoalItem will glow
#define TFGI_SLOW   2   // Players carrying this GoalItem will move at half-speed
#define TFGI_DROP   4   // Players dying with this item will drop it
#define TFGI_RETURN_DROP 8   // Return if a player with it dies
#define TFGI_RETURN_GOAL 16  // Return if a player with it has it removed by a goal's activation
#define TFGI_RETURN_REMOVE 32  // Return if it is removed by TFGI_REMOVE
#define TFGI_REVERSE_AP  64  // Only pickup if the player _doesn't_ match AP Details
#define TFGI_REMOVE   128 // Remove if left untouched for 2 minutes after being dropped
#define TFGI_KEEP   256 // Players keep this item even when they die
#define TFGI_ITEMGLOWS  512 // Item glows when on the ground
#define TFGI_DONTREMOVERES 1024 // Don't remove results when the item is removed
#define TFGI_DROP_TO_FLOOR  2048 // If this bit is set, the Goal/Item drops to the ground when it first spawns.
#define TFGI_ALLOW_DROP     4096 // If this bit is set, any player carrying this item can drop it using the "dropitems" command.
#define TFGI_SOLID          8192 // If this bit is set, the GoalItem is Solid while not being carried by a player. This means it blocks bullets, grenades.

// Defines for TeamSpawnpoints : goal_activation (in teamspawns)
#define TFSP_MULTIPLEITEMS 1  // Give out the GoalItem multiple times
#define TFSP_MULTIPLEMSGS 2  // Display the message multiple times

// Defines for TeamSpawnpoints : goal_effects (in teamspawns)
#define TFSP_REMOVESELF  1  // Remove itself after being spawned on

// Defines for Goal States
#define TFGS_ACTIVE  1 
#define TFGS_INACTIVE 2 
#define TFGS_REMOVED 3 
#define TFGS_DELAYED 4


// Legal Playerclass Handling
#define TF_ILL_SCOUT   1
#define TF_ILL_SNIPER  2
#define TF_ILL_SOLDIER  4
#define TF_ILL_DEMOMAN  8
#define TF_ILL_MEDIC  16
#define TF_ILL_HVYWEP  32
#define TF_ILL_PYRO   64
#define TF_ILL_RANDOMPC  128
#define TF_ILL_SPY   256
#define TF_ILL_ENGINEER  512
    
/*==========================================================================*/
/* Flamethrower                */
/*==========================================================================*/
#define FLAME_PLYRMAXTIME 45  // lifetime in 0.1 sec of a flame on a player
#define FLAME_MAXBURNTIME 8   // lifetime in seconds of a flame on the world (big ones)
#define NAPALM_MAXBURNTIME 20  // lifetime in seconds of flame from a napalm grenade
#define FLAME_MAXPLYRFLAMES 4  // maximum number of flames on a player
#define FLAME_NUMLIGHTS  1  // maximum number of light flame 
#define FLAME_BURNRATIO  0.3 // the chance of a flame not 'sticking'
#define GR_TYPE_FLAMES_NO 15  // number of flames spawned when a grenade explode

/*==================================================*/
/* CTF Support defines         */
/*==================================================*/
#define CTF_FLAG1   1
#define CTF_FLAG2   2
#define CTF_DROPOFF1  3
#define CTF_DROPOFF2  4
#define CTF_SCORE1    5
#define CTF_SCORE2    6


/*==================================================*/
/* Death Message defines       */
/*==================================================*/
#define DMSG_SHOTGUN   1
#define DMSG_SSHOTGUN   2
#define DMSG_NAILGUN   3
#define DMSG_SNAILGUN   4
#define DMSG_GRENADEL   5
#define DMSG_ROCKETL   6
#define DMSG_LIGHTNING   7
#define DMSG_GREN_HAND   8
#define DMSG_GREN_NAIL   9
#define DMSG_GREN_MIRV   10
#define DMSG_GREN_PIPE   11
#define DMSG_DETPACK   12
#define DMSG_BIOWEAPON   13
#define DMSG_BIOWEAPON_ATT  14
#define DMSG_FLAME    15
#define DMSG_DETPACK_DIS  16
#define DMSG_AXE    17
#define DMSG_SNIPERRIFLE  18
#define DMSG_AUTORIFLE   19
#define DMSG_ASSAULTCANNON  20
#define DMSG_HOOK    21
#define DMSG_BACKSTAB   22
#define DMSG_MEDIKIT   23
#define DMSG_GREN_GAS   24
#define DMSG_TRANQ    25
#define DMSG_LASERBOLT   26
#define DMSG_SENTRYGUN_BULLET  27
#define DMSG_SNIPERLEGSHOT  28
#define DMSG_SNIPERHEADSHOT  29
#define DMSG_GREN_EMP   30
#define DMSG_GREN_EMP_AMMO  31
#define DMSG_SPANNER   32
#define DMSG_INCENDIARY   33
#define DMSG_SENTRYGUN_ROCKET 34
#define DMSG_GREN_FLASH   35
#define DMSG_TRIGGER   36
#define DMSG_TEAMKILL   37
#define DMSG_SG_EXPLODION   38
#define DMSG_DISP_EXPLODION   39
#define DMSG_PIPEBOMB   40
#define DMSG_CALTROP   41

// Some of the toggleflags aren't used anymore, but the bits are still
// there to provide compatability with old maps
#define TFLAG_CLASS_PERSIST	1  		// Persistent Classes Bit
#define TFLAG_CHEATCHECK	2 		// Cheatchecking Bit
#define TFLAG_RESPAWNDELAY	4 		// RespawnDelay bit
#define TFLAG_UN			8		// NOT USED ANYMORE
#define TFLAG_UN2			16		// NOT USED ANYMORE
#define TFLAG_UN3			32		// NOT USED ANYMORE
#define TFLAG_AUTOTEAM		64		// sets whether players are automatically placed in teams
#define TFLAG_TEAMFRAGS		128		// Individual Frags, or Frags = TeamScore
#define TFLAG_FIRSTENTRY	256		// Used to determine the first time toggleflags is set
									// In a map. Cannot be toggled by players.
#define TFLAG_SPYINVIS		512		// Spy invisible only
#define TFLAG_GRAPPLE		1024	// Grapple on/off
#define TFLAG_FULLTEAMSCORE	2048		
#define TFLAG_FLAG_EMULATION	4096		
#define TFLAG_USE_WAR_STD	8192


///Angel TF consts
//Sentry types
/*typedef enum {
 SENTRY_OLD,
 SENTRY_MTFL,
 SENTRY_FIX,
 SENTRY_NEW,
 SENTRY_MTFL_NEWFIND,
 SENTRY_OLD_NEWFIND,
 MAX_SENTRY_TYPES
}sentry_types_t;*/

#define TF_AUTOIDTIME 0.5
#define TF_AUTOSCAN_TIME 0.5

#define TF_SPY_SFEIGN_IMPULSE 199

#define TF_EXEC_MAP_MASK  1
#define TF_EXEC_CLASS_MASK  2
#define TF_CLASS_HELP_MASK  4
#define TF_AUTOID_MASK  8
#define TF_MULTISCAN_MASK  16
#define TF_STATUS_STATS_MASK  32
#define TF_STATUS_COORD_MASK  64
#define TF_STATUS_SPY_MASK  128
#define TF_STATUS_DETPACK_MASK  256
#define TF_STATUS_SENTRY_MASK  512


#define DG_TYPE_NORMAL  2 
#define DG_TYPE_CONCUSSION 4 
#define DG_TYPE_NAIL  8 
#define DG_TYPE_MIRV  16
#define DG_TYPE_NAPALM  32
#define DG_TYPE_FLARE  64
#define DG_TYPE_GAS   128
#define DG_TYPE_EMP   256
#define DG_TYPE_FLASH  512
#define DG_TYPE_CALTROPS  1024
#define DG_TYPE_DETPACK  2048

#define BP_GREN  1  
#define BP_GREN_BYTYPE  2  
#define BP_TYPE_ARMOR  4
#define BP_TYPE_HEALTH  8
#define BP_TYPE_DETPACK  16

// sniper rifle control
#define SR_INITDAMAGE 50
#define SR_INCDAMAGE 216 // by second
#define SR_MAXDAMAGE 398

#define NEW_FLASH_START_TIME 1.6
#define AUTOSCAN_IMPULSE 210
#define DEFAULT_AUTOSCAN_RANGE 30

#define DEFAULT_ASSAULT_MIN_SHELLS 20
#define ASSAULT_SPREAD_MULTIPLY 3
#define ASSAULT_SPREAD_TIME 3


#define GAS_MASK_COLOR             1
#define GAS_MASK_SKIN              2
#define GAS_MASK_4COLORS           4
#define GAS_MASK_ALLCOLORS         8
#define GAS_MASK_PALETTE          16
#define GAS_MASK_NEWGREN_EFFECTS  32
#define GAS_MASK_ALLSPYS          64
#define GAS_MASK_DISABLE_ID      128
#define GAS_MASK_NEWGREN_TIMES   256
#define GAS_MASK_NEWGREN_DMG     512

#define GAS_OLG_GREN GAS_MASK_PALETTE 
//16
#define GAS_NEW_GREN (GAS_MASK_NEWGREN_EFFECTS | GAS_MASK_NEWGREN_TIMES | GAS_MASK_NEWGREN_DMG)
//800
#define GAS_DEFAULT (GAS_MASK_DISABLE_ID | GAS_MASK_COLOR | GAS_MASK_SKIN)
//131

#define SETGAS_IN_GAS_TIME 0.3
#define SETGAS_TIMER_THINK 0.5
#define SETGAS_SETCOLOR    0.4
#define SETGAS_SETSKIN     0.2
