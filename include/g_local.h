//
// g_local.h -- local definitions for game module

#ifndef __G_LOCAL
#define __G_LOCAL
#include "q_shared.h"
#include "mathlib.h"
#include "progs.h"
#include "g_public.h"
#include "g_consts.h"
#include "g_tf_consts.h"
#include "g_syscalls.h"
#include "player.h"

#include "menu.h"
#define NEWHWGUY
#ifdef TG
#include "tg.h"
#endif

#ifdef DEBUG
#define DebugTrap(x) *(char*)0=x
#else
#define DebugTrap(x) G_Error(x)
#endif


#define	MAX_STRING_CHARS	1024	// max length of a string passed to Cmd_TokenizeString
#define	MAX_STRING_TOKENS	1024	// max tokens resulting from Cmd_TokenizeString
#define	MAX_TOKEN_CHARS		1024	// max length of an individual token

#define	FOFS(x) ((int)&(((gedict_t *)0)->x))
int             NUM_FOR_EDICT( gedict_t * e );

extern tf_server_data_t tf_data;

extern char     mapname[];
extern gedict_t g_edicts[];
extern globalvars_t g_globalvars;
extern gedict_t *world;
extern gedict_t *self, *other;
extern gedict_t *newmis;
extern int      timelimit, fraglimit, teamplay, deathmatch, framecount, coop;

#define	EDICT_TO_PROG(e) ((byte *)(e) - (byte *)g_edicts)
#define PROG_TO_EDICT(e) ((gedict_t *)((byte *)g_edicts + (e)))

void            G_Printf( const char *fmt, ... );
void            G_Error( const char *fmt, ... );

#define PASSVEC3(x) (x[0]),(x[1]),(x[2])
#define SetVector(v,x,y,z) (v[0]=x,v[1]=y,v[2]=z)
//g_utils.c
float           g_random( void );
float           crandom( void );
gedict_t       *spawn( void );
void            ent_remove( gedict_t * t );

gedict_t       *nextent( gedict_t * ent );
gedict_t       *find( gedict_t * start, int fieldoff, char *str );
gedict_t       *findradius( gedict_t * start, vec3_t org, float rad );
void            normalize( vec3_t value, vec3_t newvalue );
float           vlen( vec3_t value1 );
float           vectoyaw( vec3_t value1 );
void            vectoangles( vec3_t value1, vec3_t ret );
void            changeyaw( gedict_t * ent );
void            makevectors( vec3_t vector );
void            G_sprint( gedict_t * ed, int level, const char *fmt, ... );
void            G_bprint( int level, const char *fmt, ... );
void            G_centerprint( gedict_t * ed, const char *fmt, ... );
void            G_dprint( const char *fmt, ... );
void 		localcmd( const char *fmt, ... );
void		stuffcmd( gedict_t * ed, const char *fmt, ... );
int             streq( char *s1, char *s2 );
int             strneq( char *s1, char *s2 );
void            aim( vec3_t ret );
void    	setorigin( gedict_t * ed, float origin_x, float origin_y, float origin_z );
void    	setsize( gedict_t * ed, float min_x, float min_y, float min_z, float max_x,
		 float max_y, float max_z );
void    	setmodel( gedict_t * ed, char *model );
void    	sound( gedict_t * ed, int channel, char *samp, int vol, float att );
gedict_t 	*checkclient(  );
void    	traceline( float v1_x, float v1_y, float v1_z, float v2_x, float v2_y, float v2_z,
		   int nomonst, gedict_t * ed );
void    	stuffcmd( gedict_t * ed, const char *fmt , ...);
int     	droptofloor( gedict_t * ed );
int     	walkmove( gedict_t * ed, float yaw, float dist );
int     	checkbottom( gedict_t * ed );
void    	makestatic( gedict_t * ed );
void    	setspawnparam( gedict_t * ed );
void    	logfrag( gedict_t * killer, gedict_t * killee );
void    	infokey( gedict_t * ed, char *key, char *valbuff, int sizebuff );
void    	WriteEntity( int to, gedict_t * ed );
void    	disableupdates( gedict_t * ed, float time );

//
//  subs.c
//
void            SUB_CalcMove( vec3_t tdest, float tspeed, void ( *func ) () );
void            SUB_CalcMoveEnt( gedict_t * ent, vec3_t tdest, float tspeed,
				 void ( *func ) () );
void            SUB_UseTargets();
void            SetMovedir();
void            InitTrigger();
void SUB_AttackFinished( float normal );
extern gedict_t *activator;

//
// g_mem.c
//
void           *G_Alloc( int size );
void            G_InitMemory( void );

//
// g_spawn.c
//

void            G_SpawnEntitiesFromString( void );
qboolean        G_SpawnString( const char *key, const char *defaultString, char **out );
qboolean        G_SpawnFloat( const char *key, const char *defaultString, float *out );
qboolean        G_SpawnInt( const char *key, const char *defaultString, int *out );
qboolean        G_SpawnVector( const char *key, const char *defaultString, float *out );
void            SUB_Remove();
void            SUB_Null();

//world.c
void            CopyToBodyQue( gedict_t * ent );

// client.c
extern float    intermission_running;
extern float    intermission_exittime;
extern int     modelindex_eyes, modelindex_player, modelindex_null;

void            SetChangeParms();
void            SetNewParms();
void            ClientConnect();
void            PutClientInServer();
void            ClientDisconnect();
void            PlayerPreThink();
void            PlayerPostThink();
void            SuperDamageSound();

//spectate.c
void            SpectatorConnect();
void            SpectatorDisconnect();
void            SpectatorThink();

// weapons.c
extern int      impulse;
extern gedict_t *multi_ent;
extern float   multi_damage;

extern vec3_t  blood_org;
extern float   blood_count;

extern vec3_t  puff_org;
extern float   puff_count;

int  	        W_BestWeapon();
void            W_Precache();
void            W_SetCurrentAmmo();
void            SpawnBlood( vec3_t, float );
void    	PreMatchImpulses(  );
void    	DeadImpulses(  );
void 		W_WeaponFrame();
void W_FireAxe();
void W_FireSpanner();
void W_FireMedikit();
void W_FireBioweapon();
void W_FireShotgun();
void W_FireSuperShotgun();
void W_FireSniperRifle();
void W_FireAutoRifle();
void W_FireAssaultCannon();
void W_FireRocket();
void W_FireLightning();
void W_FireGrenade();
void W_FireSuperSpikes();
void W_FireSpikes( float ox );

void BecomeExplosion();
void ClearMultiDamage();
void TraceAttack( float damage, vec3_t dir );
void ApplyMultiDamage(  );
void AddMultiDamage( gedict_t * hit, float damage );
void Multi_Finish(  );
void Attack_Finished( float att_delay );
void    W_PrintWeaponMessage();
void T_MissileTouch();
void    launch_spike( vec3_t org, vec3_t dir );
//combat 
extern gedict_t *damage_attacker, *damage_inflictor;
void            T_Damage( gedict_t * targ, gedict_t * inflictor, gedict_t * attacker,
			  float damage );
void            T_RadiusDamage( gedict_t * inflictor, gedict_t * attacker, float damage,
				gedict_t * ignore/*, char *dtype */);
void            T_BeamDamage( gedict_t * attacker, float damage );
void TF_T_Damage( gedict_t * targ, gedict_t * inflictor, gedict_t * attacker, float damage, int T_flags, int T_AttackType );
qboolean CanDamage( gedict_t * targ, gedict_t * inflictor );
//items
int T_Heal( gedict_t * e, float healamount, float ignore );
void            DropQuad( float timeleft );
void            DropRing( float timeleft );
void            DropBackpack();
void 		bound_other_ammo( gedict_t * p );
//triggers.c
void            spawn_tfog( vec3_t org );
void            spawn_tdeath( vec3_t org, gedict_t * death_owner );

//debug.c
void 		dremove( gedict_t* te );
void 		display_location();

//g_tf_utils.c
qboolean 	GetSVInfokeyString( char*key, char* key2, char*value, int size, char*defaultval);
int		GetSVInfokeyInt   ( char*key, char* key2, int defalutval);
float 		GetSVInfokeyFloat ( char*key, char* key2, float defalutval);

qboolean 	GetInfokeyString( gedict_t*pl, char*key, char* key2, char*value, int size, char*defaultval);
int		GetInfokeyInt   ( gedict_t*pl, char*key, char* key2, int defalutval);

void  		KickPlayer( int psize, gedict_t* p);
void  		muzzleflash();
int 		visible(gedict_t* targ);
int		range(gedict_t* targ);
int		infront(gedict_t* targ);

void    make_bubbles(  );
void    bubble_remove(  );
void    bubble_bob(  );


// common tf
void CenterPrint( gedict_t * pl, const char *fmt, ... );
float crossproduct( vec3_t veca, vec3_t vecb );
void TeamFortress_MOTD();
void    BioInfection_Decay();
void TeamFortress_Discard();
void TeamFortress_ID();
void TeamFortress_ReloadCurrentWeapon();


//tforttm
char   *GetTeamName( int tno );
int TeamFortress_TeamGetWinner(  );
int TeamFortress_TeamGetScore( int tno );
int TeamFortress_TeamGetSecond(  );
void TeamFortress_CheckTeamCheats(  );
void    TeamFortress_DetpackExplode();
int TeamFortress_TeamGetColor( int tno );
int TeamFortress_TeamGetTopColor( int tno );
int TeamFortress_GetNoPlayers();
qboolean TeamFortress_TeamIsCivilian( int tno );
int ClassIsRestricted( int tno, int pc );
void SetTeamName( gedict_t * p );
int TeamFortress_TeamSet( int tno );
void TeamFortress_TeamShowScores( int all );
int TeamFortress_TeamGetNoPlayers( int tno );
void teamsprint( int tno, gedict_t * ignore, char *st );
//tfort.c
char   *TeamFortress_GetClassName( int pc );
void TF_AddFrags( gedict_t * pl, int fr );
void TeamFortress_RemoveTimers();
void TeamFortress_SetupRespawn( int Suicided );
void TeamFortress_ChangeClass();
void TeamFortress_CheckClassStats();
int IsLegalClass( int pc );
void    TeamFortress_DescribeArmor( gedict_t * Player, int Armorclass );
void    TeamFortress_PrintClassName( gedict_t *, int, int );
void    TeamFortress_SetEquipment();
void    TeamFortress_SetHealth();
void    TeamFortress_SetSpeed( gedict_t * );
void    TeamFortress_SetSkin( gedict_t * );
void    TeamFortress_ExecClassScript( gedict_t * );
void 	TeamFortress_ExecMapScript( gedict_t * p );
int TeamFortress_TeamPutPlayerInTeam();
void KickCheater( gedict_t * p );
void    TeamFortress_Alias( char *halias, int himpulse1, int himpulse2 );
//tfortmap.c
void DisplayItemStatus( gedict_t * Goal, gedict_t * Player, gedict_t * Item );
gedict_t *Finditem( int );
gedict_t *Findgoal( int );
int Activated( gedict_t * Goal, gedict_t * AP );
void AttemptToActivate( gedict_t * Goal, gedict_t * AP, gedict_t * ActivatingGoal );
int CheckExistence();
void ParseTFDetect( gedict_t * AD );
void SetupTeamEqualiser();
void tfgoalitem_GiveToPlayer( gedict_t * Item, gedict_t * AP, gedict_t * Goal );
void tfgoalitem_RemoveFromPlayer( gedict_t * Item, gedict_t * AP, int method );
void DoResults( gedict_t * Goal, gedict_t * AP, float addb );

///clan
void PreMatch_Think();
void DumpClanScores();

//classes
void 	Engineer_RemoveBuildings();
void 	DestroyBuilding( gedict_t * eng, char *bld );
void    TeamFortress_EngineerBuild(  );
void    TeamFortress_Build( int objtobuild );
void TeamFortress_SpyChangeSkin( int class );
void TeamFortress_SpyChangeColor( int teamno );

void TF_zoom( int zoom_level );
void Spy_RemoveDisguise( gedict_t * spy );
void CheckBelowBuilding( gedict_t * bld );
void TeamFortress_DetonatePipebombs(  );
void TeamFortress_SpyFeignDeath( int issilent );
void ScannerSwitch();

//hook
void  Reset_Grapple(gedict_t* rhook);
void Service_Grapple();
//status
void CenterPrint( gedict_t * pl, const char *fmt, ... );
void StatusPrint( gedict_t * pl, float fTime, const char *fmt, ... );
void RefreshStatusBar( gedict_t * pl );
void StatusRes( int res );
//menu
void Player_Menu();
void Menu_Input(int impulse);
void Menu_Team_Input(int impulse);
void Menu_Class_Input(int impulse);

#endif
