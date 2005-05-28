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
 *  $Id: g_local.h,v 1.20 2005-05-28 19:03:44 AngelD Exp $
 */
//
// g_local.h -- local definitions for game module

#ifndef __G_LOCAL
#define __G_LOCAL
#include "q_shared.h"
#include "mathlib.h"
#include "qmd5.h"
#include "g_consts.h"
#include "g_tf_consts.h"
#include "progs.h"
#include "g_public.h"
#include "g_syscalls.h"
#include "player.h"
#include "tfort.h"
#include "spy.h"
#include "bot.h"

#include "menu.h"
#include "vote.h"

#define NEWHWGUY
#define HAS_SENTRY_CHECK

#include "tg.h"

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

extern gedict_t g_edicts[];
extern globalvars_t g_globalvars;
extern gedict_t *world;
extern gedict_t *self, *other;
extern gedict_t *newmis;
extern int      timelimit, fraglimit, teamplay, deathmatch, framecount, coop;

#define	EDICT_TO_PROG(e) ((byte *)(e) - (byte *)g_edicts)
#define PROG_TO_EDICT(e) ((gedict_t *)((byte *)g_edicts + (e)))

void            G_dprintf( const char *fmt, ... );
void            G_Error( const char *fmt, ... );
void            G_conprintf( const char *fmt, ... );

#define PASSVEC3(x) (x[0]),(x[1]),(x[2])
#define SetVector(v,x,y,z) (v[0]=x,v[1]=y,v[2]=z)
//g_utils.c
float           g_random( void );
float           crandom( void );
int 		rint(float f);
gedict_t       *spawn( void );
void            ent_remove( gedict_t * t );

gedict_t       *nextent( gedict_t * ent );
//gedict_t       *find( gedict_t * start, int fieldoff, char *str );
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
void 		localcmd( const char *fmt, ... );
void		stuffcmd( gedict_t * ed, const char *fmt, ... );
int             streq( const char *s1, const char *s2 );
int             strneq( const char *s1, const char *s2 );
void            aim( vec3_t ret );
void    	setorigin( gedict_t * ed, float origin_x, float origin_y, float origin_z );
void    	setsize( gedict_t * ed, float min_x, float min_y, float min_z, float max_x,
		 float max_y, float max_z );
void    	setmodel( gedict_t * ed, char *model );
void    	sound( gedict_t * ed, int channel, char *samp, int vol, float att );
gedict_t 	*checkclient(  );
void    	traceline( float v1_x, float v1_y, float v1_z, float v2_x, float v2_y, float v2_z,
		   int nomonst, gedict_t * ed );
void 		TraceCapsule( float v1_x, float v1_y, float v1_z, float v2_x, float v2_y, float v2_z, int nomonst, gedict_t * ed ,
			float min_x, float min_y, float min_z, 
			float max_x, float max_y, float max_z);
void    	stuffcmd( gedict_t * ed, const char *fmt , ...);
int     	droptofloor( gedict_t * ed );
int     	walkmove( gedict_t * ed, float yaw, float dist );
int     	checkbottom( gedict_t * ed );
void    	makestatic( gedict_t * ed );
void    	setspawnparam( gedict_t * ed );
void    	logfrag( gedict_t * killer, gedict_t * killee );
void    	infokey( gedict_t * ed, const char *key, char *valbuff, int sizebuff );
void    	WriteEntity( int to, gedict_t * ed );
void    	disableupdates( gedict_t * ed, float time );
int		walkmove( gedict_t * ed, float yaw, float dist );

//
//  subs.c
//
void            SUB_CalcMove( vec3_t tdest, float tspeed, void ( *func ) () );
void            SUB_CalcMoveEnt( gedict_t * ent, vec3_t tdest, float tspeed,
				 void ( *func ) () );
void            SUB_UseTargets();
void            SetMovedir();
void            InitTrigger();
void 		SUB_AttackFinished( float normal );
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
/*extern int      impulse;
extern gedict_t *multi_ent;
extern float   multi_damage;*/

extern vec3_t  blood_org;
//extern float   blood_count;

extern vec3_t  puff_org;
//extern float   puff_count;

int  	        W_BestWeapon();
void            W_Precache();
void            W_SetCurrentAmmo();
void            SpawnBlood( vec3_t, float );
void    	PreMatchImpulses(  );
void    	DeadImpulses(  );
void 		W_WeaponFrame();
void 		W_FireAxe();
void            W_FireSpanner();
void            W_FireMedikit();
void            W_FireBioweapon();
void    	W_FireShotgun();
void    	W_FireSuperShotgun();
void 		W_FireSniperRifle();
void 		W_FireAutoRifle();
void            W_FireAssaultCannon();
void            W_FireRocket();
void            W_FireLightning();
void    	W_FireGrenade();
void    	W_FireSuperSpikes();
void 		W_FireSpikes( float ox );
void    	W_FireFlame(  );
void    	W_FireIncendiaryCannon(  );
void    	W_FireTranq(  );
void    	W_FireLaser(  );
     		

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
int 		T_Heal( gedict_t * e, float healamount, float ignore );
void            DropQuad( float timeleft );
void            DropRing( float timeleft );
void            DropBackpack();
void 		bound_other_ammo( gedict_t * p );
void 		Respawn_Item( gedict_t * ritem, gedict_t * act );
//triggers.c
void            spawn_tfog( vec3_t org );
void            spawn_tdeath( vec3_t org, gedict_t * death_owner );

//debug.c
void 		dremove( gedict_t* te );
void 		display_location();

//version.c
extern char * ANGEL_VERSION;
int build_number (void);
//g_tf_utils.c

qboolean 	GetSVInfokeyString( const char*key, const char* key2, char*value, int size, const char*defaultval);
int		GetSVInfokeyInt   ( const char*key, const char* key2, int defalutval);
float 		GetSVInfokeyFloat ( const char*key, const char* key2, float defalutval);

qboolean 	GetInfokeyString( gedict_t*pl, const char*key, const char* key2, char*value, int size, const char*defaultval);
int		GetInfokeyInt   ( gedict_t*pl, const char*key, const char* key2, int defalutval);

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

///clan

//hook
void  Reset_Grapple(gedict_t* rhook);
void Service_Grapple();
void    Throw_Grapple(  );
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
void CheckAutoKick( gedict_t * p );

#endif
