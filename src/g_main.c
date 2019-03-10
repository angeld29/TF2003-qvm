/*
 *  QWProgs-TF2003
 *  Copyright (C) 2004  [sd] angel
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
 *  $Id: g_main.c,v 1.27 2006-11-26 21:33:27 AngelD Exp $
 */

#include "g_local.h"

gedict_t        g_edicts[MAX_EDICTS];	//768
gedict_t       *world = g_edicts;
gedict_t       *self, *other;

globalvars_t    g_globalvars;
static field_t         expfields[] = {
    {"maxspeed", FOFS( maxspeed ), F_FLOAT}	,
    {"gravity", FOFS( gravity ), F_FLOAT},
    {"isBot", FOFS( isBot ), F_INT},
	{"items2",      FOFS( items2 ),      F_FLOAT},

	{"vw_index",    FOFS( vw_index ),    F_FLOAT},
	{"movement",    FOFS( movement ),    F_VECTOR},
	{"brokenankle", FOFS( brokenankle ), F_FLOAT},
	{"mod_admin",   FOFS( k_admin ),     F_INT},
	{"hideentity",  FOFS( hideentity ),  F_INT},
	{"trackent",	FOFS( trackent ),	 F_INT},
	{"hideplayers", FOFS( hideplayers ), F_INT},
	{"visclients",  FOFS( visclients ),  F_INT},
	{"teleported",  FOFS( teleported ),  F_INT},
    {NULL}
};

char     mapname[64];
static char     worldmodel[64] = "worldmodel";
static char     netnames[MAX_CLIENTS][32];
int             api_ver;

//#define MIN_API_VERSION 12
#define MIN_API_VERSION GAME_API_VERSION

static gameData_t      gamedata =
{ ( edict_t * ) g_edicts, sizeof( gedict_t ), &g_globalvars, expfields , MIN_API_VERSION, MAX_EDICTS};
float           starttime;
void            G_InitGame( int levelTime, int randomSeed );
void            StartFrame( int time );
qboolean        ClientCommand();
qboolean        ClientUserInfoChanged();
qboolean        ClientUserInfoChanged_after();
void            G_EdictTouch();
void            G_EdictThink();
void            G_EdictBlocked();
void            ClearGlobals();
void initialise_spawned_ent(gedict_t* ent);

/*
   ================
   vmMain

   This is the only way control passes into the module.
   This must be the very first function compiled into the .q3vm file
   ================
   */

void   TF_LocalinfoSettings( );
void   TF_FinalizeSettings( );
#define RestoreGlobals()  \
    damage_attacker = damage_attacker;\
    damage_inflictor = damage_inflictor_; \
    activator = activator_; \
    self = self_; \
    other = other_; \
    newmis = newmis_; 
intptr_t vmMain( int command, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5,
        int arg6, int arg7, int arg8, int arg9, int arg10, int arg11 )
{
    gedict_t* damage_attacker_= damage_attacker;
    gedict_t* damage_inflictor_= damage_inflictor;
    gedict_t* activator_ = activator ;
    gedict_t* self_ = self;
    gedict_t* other_ = other;
    gedict_t* newmis_ = newmis;
    ClearGlobals();
    switch ( command )
    {
        case GAME_INIT:
            api_ver = trap_GetApiVersion();
            if ( api_ver < MIN_API_VERSION ) 
            {
                G_conprintf("Mod requried API_VERSION %d or higher, server have %d\n", MIN_API_VERSION, api_ver);
                RestoreGlobals();
                return 0;
            }
            if( api_ver >= MIN_API_VERSION && api_ver <= GAME_API_VERSION )
            {
                gamedata.APIversion = api_ver;
            }
            G_InitGame( arg0, arg1 );
            RestoreGlobals();
            return ( intptr_t ) ( &gamedata );

        case GAME_LOADENTS:
            infokey( world, "mapname", mapname, sizeof(mapname) );
            TF_LocalinfoSettings();
            G_SpawnEntitiesFromString();
            TF_FinalizeSettings();
            RestoreGlobals();
            return 1;

        case GAME_START_FRAME:
            StartFrame( arg0 );
            if( arg1 ){
                botFrame();
            }
            RestoreGlobals();
            return 1;

        case GAME_CLIENT_CONNECT:
            self = PROG_TO_EDICT( g_globalvars.self );
            self->s.v.netname = netnames[NUM_FOR_EDICT(self)-1]; //Init client names
            infokey( self, "netname", self->s.v.netname,  32);
            self->auth_time = g_globalvars.time + 10.0;
            self->isSpectator = arg0?1:0;
            if ( arg0 )
                SpectatorConnect();
            else
                ClientConnect();
            RestoreGlobals();
            return 1;

        case GAME_PUT_CLIENT_IN_SERVER:
            self = PROG_TO_EDICT( g_globalvars.self );
            if ( !arg0 )
                PutClientInServer();
            RestoreGlobals();
            return 1;

        case GAME_CLIENT_DISCONNECT:
            self = PROG_TO_EDICT( g_globalvars.self );
            if ( arg0 )
                SpectatorDisconnect();
            else
                ClientDisconnect();
            RestoreGlobals();
            return 1;

        case GAME_SETNEWPARMS:
            SetNewParms();
            RestoreGlobals();
            return 1;

        case GAME_CLIENT_PRETHINK:
            self = PROG_TO_EDICT( g_globalvars.self );
            if ( !arg0 )
                PlayerPreThink();
            RestoreGlobals();
            return 1;

        case GAME_CLIENT_POSTTHINK:
            self = PROG_TO_EDICT( g_globalvars.self );
            if ( !arg0 )
                PlayerPostThink();
            else
                SpectatorThink();
            RestoreGlobals();
            return 1;

        case GAME_EDICT_TOUCH:
            G_EdictTouch();
            RestoreGlobals();
            return 1;

        case GAME_EDICT_THINK:
            G_EdictThink();
            RestoreGlobals();
            return 1;

        case GAME_EDICT_BLOCKED:
            G_EdictBlocked();
            RestoreGlobals();
            return 1;

        case GAME_SETCHANGEPARMS: //called before spawn new server for save client params
            self = PROG_TO_EDICT( g_globalvars.self );
            SetChangeParms();
            RestoreGlobals();
            return 1;

        case GAME_CLIENT_COMMAND:
            self = PROG_TO_EDICT( g_globalvars.self );
            RestoreGlobals();
            return ClientCommand();

        case GAME_CLIENT_USERINFO_CHANGED:
            // called on user /cmd setinfo	if value changed
            // return not zero dont allow change
            // params like GAME_CLIENT_COMMAND, but argv(0) always "setinfo" and argc always 3
            self = PROG_TO_EDICT( g_globalvars.self );
            {
                int ret = arg0?ClientUserInfoChanged():ClientUserInfoChanged_after();
                RestoreGlobals();
                return ret;
            }
        case GAME_SHUTDOWN:
            RestoreGlobals();
            return 0;

        case GAME_CONSOLE_COMMAND:

            // called on server console command "mod"
            // params like GAME_CLIENT_COMMAND, but argv(0) always "mod"
            // self - rconner if can detect else world
            // other 
            //SV_CMD_CONSOLE		0          
            //SV_CMD_RCON			1  
            //SV_CMD_MASTER		2          
            //SV_CMD_BOT			3  
            self = PROG_TO_EDICT( g_globalvars.self );
            ModCommand();
            RestoreGlobals();
            return 0;
        case GAME_CLIENT_SAY:
            // called on user /say or /say_team
            // arg0 non zero if say_team
            // return non zero if say/say_team handled by mod
            // params like GAME_CLIENT_COMMAND

            RestoreGlobals();
            return 0;//ClientSay( arg0 );

        case GAME_PAUSED_TIC:
            // called every frame when the game is paused
            //PausedTic( arg0 );
            RestoreGlobals();
            return 0;

        case GAME_CLEAR_EDICT:
            // Don't ClearGlobals() as this will be called during spawn()
            initialise_spawned_ent(PROG_TO_EDICT( g_globalvars.self ));
            RestoreGlobals();
            return 0;

    }

    return 0;
}


//===================================================================

void G_InitGame( int levelTime, int randomSeed )
{
    int             i;
    /*	int num;
        char		dirlist[1024];
        char*		dirptr;
        int dirlen;*/
    //Common Initialization
    srand( randomSeed );
    framecount = 0;
    starttime = levelTime * 0.001;
    G_dprintf( "Init Game\n" );
#ifdef idx64
            // We set references
            cvar_fset("sv_pr2references", 1);
#endif
    //G_InitMemory();
    memset( g_edicts, 0, sizeof( gedict_t ) * MAX_EDICTS );


    world->s.v.model = worldmodel;
    //g_globalvars.mapname = mapname;
    for ( i = 0; i < MAX_CLIENTS; i++ )
    {
        g_edicts[i + 1].s.v.netname = netnames[i]; //Init client names
    }
    //TF Intialization
    memset( &tf_data, 0, sizeof(tf_data));
    memset( &tg_data, 0, sizeof(tg_data));
    localcmd("serverinfo status Standby\n");
    //test
    /*        num = trap_FS_GetFileList( "SKINS" , ".pcx" , dirlist, sizeof(dirlist));
              dirptr=dirlist;
              for (i = 0; i < num; i++, dirptr += dirlen+1) {
              dirlen = strlen(dirptr);
              G_Printf("%s\n",dirptr);
              }*/
}




//===========================================================================
// Physics
// 
//===========================================================================
////////////////
// GlobalParams:
// time
// self
// other
///////////////
void G_EdictTouch()
{
    self = PROG_TO_EDICT( g_globalvars.self );
    other = PROG_TO_EDICT( g_globalvars.other );
    if ( self->s.v.touch )
    {
        /*#ifdef DEBUG
          if(self->s.v.classname && other->s.v.classname)
          if(!strcmp(self->s.v.classname,"player")||!strcmp(other->s.v.classname,"player"))
          G_dprintf( "touch %s <-> %s\n", self->s.v.classname,other->s.v.classname);
#endif*/
        ( ( void ( * )() ) ( self->s.v.touch ) ) ();
    } else
    {
        G_dprintf( "Null touch func" );
    }
}

////////////////
// GlobalParams:
// time
// self
// other=world
///////////////
void ClientThink();
void G_EdictThink()
{
    self = PROG_TO_EDICT( g_globalvars.self );
    other = PROG_TO_EDICT( g_globalvars.other );
    if( (int)self->s.v.flags & FL_CLIENT ){
        ClientThink();
    }
    if ( self->s.v.think )
    {
        ( ( void ( * )() ) ( self->s.v.think ) ) ();
    } else
    {
        G_dprintf( "Null think func" );
    }
}

////////////////
// GlobalParams:
// time
// self=pusher
// other=check
// if the pusher has a "blocked" function, call it
// otherwise, just stay in place until the obstacle is gone
///////////////
void G_EdictBlocked()
{
    self = PROG_TO_EDICT( g_globalvars.self );
    other = PROG_TO_EDICT( g_globalvars.other );

    if ( self->s.v.blocked )
    {
        ( ( void ( * )() ) ( self->s.v.blocked ) ) ();
    } else
    {
#ifdef PARANOID
        G_dprintf("Null blocked func");
#endif
    }

}

float sv_gravity;
void ClearGlobals()
{
    damage_attacker = damage_inflictor = activator = self = other = newmis = world;
    sv_gravity = trap_cvar("sv_gravity");
}
