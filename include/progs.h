#include "progdefs.h"

#define	MAX_ENT_LEAFS	16

typedef struct shared_edict_s {
	qboolean        free;
	link_t          area;	// linked to a division node or leaf

	int             num_leafs;
	short           leafnums[MAX_ENT_LEAFS];

	entity_state_t  baseline;

	float           freetime;	// sv.time when the object was freed
	//double                lastruntime;            // sv.time when SV_RunEntity was last
	float           lastruntime1, lastruntime2;	//VM not support double // called for this edict (Tonik)

	entvars_t       v;	// C exported fields from progs
// other fields from progs come immediately after
} edict_t;

struct gedict_s;
typedef void (*th_die_func_t)();
typedef void (*th_pain_func_t)(struct gedict_s *, float);


typedef struct gedict_s {
	edict_t         s;
	//custom fields
	float           maxspeed, gravity;
	char           *mdl;
	char           *killtarget;
	int            worldtype;	// 0=medieval 1=metal 2=base
	char           *map;
//player
	int             walkframe;

//
// quakeed fields
//
	int             style;
	float           speed;
	int             state;
	int             voided;
//items.c
	float           healamount, healtype;
//ammo
	int             aflag;
//
// doors, etc
//
	vec3_t          dest, dest1, dest2;
	vec3_t          pos1, pos2;
	vec3_t          mangle;
	float           t_length, t_width, height;
	float           wait;	// time from firing to restarting
	float           delay;	// time from activation to firing
	struct gedict_s *trigger_field;	// door's trigger entity
	struct gedict_s *movetarget;
	float           pausetime;
	char           *noise4;
	float           count;
	int		last_impulse;
//
// misc
//
	float           cnt;	// misc flag

	void            ( *think1 ) ();	//calcmove
	vec3_t          finaldest;
//hook  
	int	lefty;
//combat
	float           dmg;
	float           lip;
	float           attack_finished;
	float           pain_finished;

	float           invincible_finished;
	float           invisible_finished;
	float           super_damage_finished;
	float           radsuit_finished;


	float           invincible_time, invincible_sound;
	float           invisible_time, invisible_sound;
	float           super_time, super_sound;
	float           rad_time;
	float           fly_sound;

	float           axhitme;
	float           show_hostile;	// set to time+0.2 whenever a client fires a
	// weapon or takes damage.  Used to alert
	// monsters that otherwise would let the player go
	float           jump_flag;	// player jump flag
	float           swim_flag;	// player swimming sound flag
	float           air_finished;	// when time > air_finished, start drowning
	float           bubble_count;	// keeps track of the number of bubbles
	char           *deathtype;	// keeps track of how the player died
	float           dmgtime;

	void            ( *th_die ) ();
	void            ( *th_pain ) ( struct gedict_s *, float );

///team fortess
	int             playerclass, nextpc;
	int             armorclass;
	int             tf_items, tf_items_flags;
	int             no_grenades_1;
	int             no_grenades_2;
	int             tp_grenades_1;
	int             tp_grenades_2;
	int             got_aliases;
	int             cheat_check;
	int             is_removed;
	int             is_undercover;
	int             is_building;
	int             is_detpacking;
	int             is_feigning;
	int             is_unabletospy;
	int             has_disconnected;
	int             tfstate;
	struct gedict_s *linked_list, *observer_list;

	int             maxammo_shells;
	int             maxammo_nails;
	int             maxammo_cells;
	int             maxammo_rockets;
	int             items_allowed;
	float           armor_allowed;
	int             maxarmor;
	int             maxfbspeed;
	int             maxstrafespeed;
	int             weaponmode;
	int             motd;
	int             current_menu;
	int             menu_count;
	int             menu_displaytime;

	int             team_no, lives, infection_team_no;
	struct gedict_s *building, *real_owner,*oldenemy;

	float           building_wait, respawn_time, suicide_time;
	int             has_dispenser;
	int             has_sentry;
	int             real_frags;
	int             weapons_carried;
	int             current_weapon;
	int             last_weapon;
	int             last_weaponmode;
	int             ammo_medikit;
	int             maxammo_medikit;
	int             ammo_detpack;
	int             maxammo_detpack;
	int             reload_shotgun;
	int             reload_super_shotgun;
	int             reload_grenade_launcher;
	int             reload_rocket_launcher;
	float           heat;
	float           immune_to_check;
	float           last_saveme_sound;
	int             no_active_nail_grens;
	float		waitmin,waitmax;

//goal fields
        int		has_abbreviated;
        int		distance;
        float		search_time;
	int             goal_no;
	int             group_no;
	int             goal_state;
	int             owned_by;
	int             goal_activation;
	int             goal_effects;
	int             goal_result;
	int             goal_group;
	int             else_goal;
	int             if_goal_is_active;
	int             if_goal_is_inactive;
	int             if_goal_is_removed;
	int             if_group_is_active;
	int             if_group_is_inactive;
	int             if_group_is_removed;
	int             activate_goal_no;
	int             inactivate_goal_no;
	int             remove_goal_no;
	int             restore_goal_no;
	int             activate_group_no;
	int             inactivate_group_no;
	int             remove_group_no;
	int             restore_group_no;
	vec3_t          goal_min, goal_max;
	int             has_item_from_group;
	int             hasnt_item_from_group;
	int             remove_item_group;
	int             return_item_no;
	int             if_item_has_moved;
	int             if_item_hasnt_moved;
	int             remove_spawnpoint;
	int             restore_spawnpoint;
	int             remove_spawngroup;
	int             restore_spawngroup;
	int             display_item_status1;
	int             display_item_status2;
	int             display_item_status3;
	int             display_item_status4;
	char           *team_str_home;
	char           *team_str_moved;
	char           *team_str_carried;
	char           *non_team_str_home;
	char           *non_team_str_moved;
	char           *non_team_str_carried;
	int             ex_skill_min;
	int             ex_skill_max;
	int             increase_team1;
	int             increase_team2;
	int             increase_team3;
	int             increase_team4;
	char           *broadcast;
	char           *team_broadcast;
	char           *non_team_broadcast;
	char           *owners_team_broadcast;
	char           *netname_broadcast;
	char           *netname_team_broadcast;
	char           *netname_non_team_broadcast;
	char           *netname_owners_team_broadcast;
	char           *team_drop;
	char           *non_team_drop;
	char           *netname_team_drop;
	char           *netname_non_team_drop;
	int             all_active;
	int             item_list;
	int             delay_time;
	int             dont_do_triggerwork;

	int             numflames;
	char*		flame_id;

//spy
	int             undercover_team;
	int             undercover_skin;
	char           *undercover_name;



	float           camdist;
	vec3_t          camangle;
	struct gedict_s *camera_list;

	int             hook_out;
	struct gedict_s *hook;
	int             on_hook;
	int             fire_held_down;

	vec3_t          head_shot_vector;
	int             leg_damage;
	int             cheat_level;
	float          	FlashTime;
	int             nailpos;
	float           StatusRefreshTime;
	int             StatusBarSize;
	int             StatusBarRes;
	int             ScannerOn;
	int             ScanRange;
	int		assault_min_shells;

	int             tf_id;
	int             teamkills;
	int 		tf_impulse,imp1,imp2,imp3,imp4;
//admin
	int		is_admin;
	int		admin_mode;
	int		autokick_kills,autokick_time;
	struct gedict_s*	admin_use;

	float		allow_snip_time;
	int		has_fake_gas_setinfo;
#ifdef TG
	float		gren_eff_time;
#endif
} gedict_t;
extern  int 		skill;
extern  int 		number_of_teams; 
extern  int             illegalclasses[5];
extern  int             civilianteams;		
//extern  int		team_col[5];            //
extern  int             teamscores[5];
extern  int             teamfrags[5];
extern  int             teamlives[5];
extern	int             teammaxplayers[5];
extern  float		teamadvantage[5];       

extern  int             CTF_Map;                
extern  float           rj;                     
extern  int             num_world_flames;       
extern  int             num_team_pipebombs[5];  
extern  int             num_team_flares[5];     
extern  int             num_team_ammoboxes[5];  
extern	int             item_list_bit;
extern char           *team_menu_string;  

extern int FLAME_MAXWORLDNUM, MAX_WORLD_PIPEBOMBS, MAX_WORLD_AMMOBOXES, GR_TYPE_MIRV_NO, GR_TYPE_NAPALM_NO;

typedef struct {
	int             toggleflags;
	float           respawn_delay_time;
	int             live_camera;
	int             already_chosen_map;

	int             clanbattle;
	float           clan_scores_dumped;
	float           cb_prematch_time;
	float           cb_ceasefire_time;

	int             game_locked;
	int             last_id;

	int             spy_off;
	int             old_grens;
	int             new_flash;
	int             invis_only;
	int             allow_hook;
	int             flagem_checked;
	int             cease_fire;
	int             allow_drop_goal;

	float           autokick_time;
	int             autokick_kills;
	int             deathmsg;
	int             birthday;

	int		cheat_pause;
	int		disable_grens;
	int		topcolor_check;
	sentry_types_t	sentry_type;
	int		sgppl;
	int		snip_range_fix,snip_fps,snip_ammo;
	float		snip_time;
	int		add_pipe;
	int		new_gas;
	int		gren2box,random_tf_spawn,disable_powerups,flag_timer,detpack_block;
	int		mtfl;
	int 		lan_mode;
} tf_server_data_t;
