#include "g_local.h"
#define MAX_BODYQUE 4
gedict_t *bodyque[MAX_BODYQUE];
int     bodyque_head;
void InitBodyQue(  )
{
	int     i;

	bodyque[0] = spawn(  );
	bodyque[0]->s.v.classname = "bodyque";
	for ( i = 1; i < MAX_BODYQUE; i++ )
	{
		bodyque[i] = spawn(  );
		bodyque[i]->s.v.classname = "bodyque";
		bodyque[i - 1]->s.v.owner = EDICT_TO_PROG( bodyque[i] );
	}
	bodyque[MAX_BODYQUE - 1]->s.v.owner = EDICT_TO_PROG( bodyque[0] );
	bodyque_head = 0;
}

// make a body que entry for the given ent so the ent can be
// respawned elsewhere
void CopyToBodyQue( gedict_t * ent )
{
	VectorCopy( ent->s.v.angles, bodyque[bodyque_head]->s.v.angles );
	VectorCopy( ent->s.v.velocity, bodyque[bodyque_head]->s.v.velocity );
	bodyque[bodyque_head]->s.v.model = ent->s.v.model;
	bodyque[bodyque_head]->s.v.modelindex = ent->s.v.modelindex;
	bodyque[bodyque_head]->s.v.frame = ent->s.v.frame;
	bodyque[bodyque_head]->s.v.colormap = ent->s.v.colormap;
	bodyque[bodyque_head]->s.v.movetype = ent->s.v.movetype;
	bodyque[bodyque_head]->s.v.flags = 0;

	setorigin( bodyque[bodyque_head], PASSVEC3( ent->s.v.origin ) );
	setsize( bodyque[bodyque_head], PASSVEC3( ent->s.v.mins ), PASSVEC3( ent->s.v.maxs ) );

	if ( ++bodyque_head >= MAX_BODYQUE )
		bodyque_head = 0;
}


extern gedict_t *lastspawn;
int     number_of_teams;

void SP_worldspawn(  )
{
	char   *s;
	char    gamedir[64], cycledir[64];
	char    exec_maps[10];

	G_SpawnString( "classname", "", &s );
	if ( Q_stricmp( s, "worldspawn" ) )
	{
		G_Error( "SP_worldspawn: The first entity isn't 'worldspawn'" );
	}
	world->s.v.classname = "worldspawn";
	lastspawn = world;
	number_of_teams = 0;
	InitBodyQue(  );

	if ( !Q_stricmp( self->s.v.model, "maps/e1m8.bsp" ) )
		trap_cvar_set( "sv_gravity", "100" );
	else
		trap_cvar_set( "sv_gravity", "800" );
	infokey( world, "*sv_gamedir", gamedir, sizeof( gamedir ) );
	if ( gamedir[0] != 0 && strcmp( gamedir, "fortress" ) )
	{
		G_Error( "QW TF must be run with a sv_gamedir of \"fortress\".\n" );
	} else
	{
		if ( gamedir[0] == 0 )
		{
			infokey( world, "*gamedir", gamedir, sizeof( gamedir ) );
			if ( strcmp( gamedir, "fortress" ) )
				G_Error( "QW TF must be run with a sv_gamedir of \"fortress\".\n" );
		}
	}

	infokey( world, "exec_map_cfgs", exec_maps, sizeof( exec_maps ) );

	if ( !strcmp( exec_maps, "on" ) )
	{
		infokey( world, "cycledir", cycledir, sizeof( cycledir ) );
		if ( cycledir[0] )
		{
			localcmd( "exec %s/mapdefault.cfg\n", cycledir );
			localcmd( "exec %s/%s.cfg\n", cycledir, g_globalvars.mapname );
		} else
		{
			localcmd( "exec qwmcycle/mapdefault.cfg\n" );
			localcmd( "exec qwmcycle/%s.cfg\n", g_globalvars.mapname );
		}
	}
// the area based ambient sounds MUST be the first precache_sounds

// player precaches     
	W_Precache(  );		// get weapon precaches

// sounds used from C physics code
	trap_precache_sound( "demon/dland2.wav" );	// landing thud
	trap_precache_sound( "misc/h2ohit1.wav" );	// landing splash

// setup precaches allways needed
	trap_precache_sound( "items/itembk2.wav" );	// item respawn sound
	trap_precache_sound( "player/plyrjmp8.wav" );	// player jump
	trap_precache_sound( "player/land.wav" );	// player landing
	trap_precache_sound( "player/land2.wav" );	// player hurt landing
	trap_precache_sound( "player/drown1.wav" );	// drowning pain
	trap_precache_sound( "player/drown2.wav" );	// drowning pain
	trap_precache_sound( "player/gasp1.wav" );	// gasping for air
	trap_precache_sound( "player/gasp2.wav" );	// taking breath
	trap_precache_sound( "player/h2odeath.wav" );	// drowning death

	trap_precache_sound( "misc/talk.wav" );	// talk
	trap_precache_sound( "player/teledth1.wav" );	// telefrag
	trap_precache_sound( "misc/r_tele1.wav" );	// teleport sounds
	trap_precache_sound( "misc/r_tele2.wav" );
	trap_precache_sound( "misc/r_tele3.wav" );
	trap_precache_sound( "misc/r_tele4.wav" );
	trap_precache_sound( "misc/r_tele5.wav" );
	trap_precache_sound( "weapons/lock4.wav" );	// ammo pick up
	trap_precache_sound( "weapons/pkup.wav" );	// weapon up
	trap_precache_sound( "items/armor1.wav" );	// armor up
	trap_precache_sound( "weapons/lhit.wav" );	//lightning
	trap_precache_sound( "weapons/lstart.wav" );	//lightning start
	trap_precache_sound( "items/damage3.wav" );

	trap_precache_sound( "misc/power.wav" );	//lightning for boss

// player gib sounds
	trap_precache_sound( "player/gib.wav" );	// player gib sound
	trap_precache_sound( "player/udeath.wav" );	// player gib sound
	trap_precache_sound( "player/tornoff2.wav" );	// gib sound

// player pain sounds

	trap_precache_sound( "player/pain1.wav" );
	trap_precache_sound( "player/pain2.wav" );
	trap_precache_sound( "player/pain3.wav" );
	trap_precache_sound( "player/pain4.wav" );
	trap_precache_sound( "player/pain5.wav" );
	trap_precache_sound( "player/pain6.wav" );

// player death sounds
	trap_precache_sound( "player/death1.wav" );
	trap_precache_sound( "player/death2.wav" );
	trap_precache_sound( "player/death3.wav" );
	trap_precache_sound( "player/death4.wav" );
	trap_precache_sound( "player/death5.wav" );

	trap_precache_sound( "boss1/sight1.wav" );

// ax sounds    
	trap_precache_sound( "weapons/ax1.wav" );	// ax swoosh
	trap_precache_sound( "player/axhit1.wav" );	// ax hit meat
	trap_precache_sound( "player/axhit2.wav" );	// ax hit world

	trap_precache_sound( "player/h2ojump.wav" );	// player jumping into water
	trap_precache_sound( "player/slimbrn2.wav" );	// player enter slime
	trap_precache_sound( "player/inh2o.wav" );	// player enter water
	trap_precache_sound( "player/inlava.wav" );	// player enter lava
	trap_precache_sound( "misc/outwater.wav" );	// leaving water sound

	trap_precache_sound( "player/lburn1.wav" );	// lava burn
	trap_precache_sound( "player/lburn2.wav" );	// lava burn

	trap_precache_sound( "misc/water1.wav" );	// swimming
	trap_precache_sound( "misc/water2.wav" );	// swimming

// Invulnerability sounds
	trap_precache_sound( "items/protect.wav" );
	trap_precache_sound( "items/protect2.wav" );
	trap_precache_sound( "items/protect3.wav" );


	trap_precache_model( "progs/player.mdl" );
	trap_precache_model( "progs/eyes.mdl" );
	trap_precache_model( "progs/h_player.mdl" );
	trap_precache_model( "progs/gib1.mdl" );
	trap_precache_model( "progs/gib2.mdl" );
	trap_precache_model( "progs/gib3.mdl" );

	trap_precache_model( "progs/s_bubble.spr" );	// drowning bubbles
	trap_precache_model( "progs/s_explod.spr" );	// sprite explosion

	trap_precache_model( "progs/v_axe.mdl" );
	trap_precache_model( "progs/v_shot.mdl" );
	trap_precache_model( "progs/v_nail.mdl" );
	trap_precache_model( "progs/v_rock.mdl" );
	trap_precache_model( "progs/v_shot2.mdl" );
	trap_precache_model( "progs/v_nail2.mdl" );
	trap_precache_model( "progs/v_rock2.mdl" );

	trap_precache_model( "progs/bolt.mdl" );	// for lightning gun
	trap_precache_model( "progs/bolt2.mdl" );	// for lightning gun
	trap_precache_model( "progs/bolt3.mdl" );	// for boss shock
	trap_precache_model( "progs/lavaball.mdl" );	// for testing

	trap_precache_model( "progs/missile.mdl" );
	trap_precache_model( "progs/grenade.mdl" );
	trap_precache_model( "progs/spike.mdl" );
	trap_precache_model( "progs/s_spike.mdl" );

	trap_precache_model( "progs/backpack.mdl" );

	trap_precache_model( "progs/zom_gib.mdl" );

	trap_precache_model( "progs/v_light.mdl" );

/////////////////////////TF///////////////////
	trap_precache_model( "progs/laser.mdl" );
	trap_precache_sound( "enforcer/enfire.wav" );
	trap_precache_sound( "enforcer/enfstop.wav" );
	trap_precache_sound( "hknight/attack1.wav" );
	trap_precache_model( "progs/sight.spr" );
	trap_precache_model( "progs/caltrop.mdl" );
	trap_precache_model( "progs/cross1.mdl" );
	trap_precache_model( "progs/tf_flag.mdl" );
	trap_precache_model( "progs/tf_stan.mdl" );
	trap_precache_model( "progs/v_medi.mdl" );
	trap_precache_model( "progs/v_bio.mdl" );
	trap_precache_model( "progs/v_pipe.mdl" );
	trap_precache_model( "progs/hgren2.mdl" );
	trap_precache_model( "progs/biggren.mdl" );
	trap_precache_model( "progs/flare.mdl" );
	trap_precache_model( "progs/v_srifle.mdl" );
	trap_precache_model( "progs/v_asscan.mdl" );
	trap_precache_model( "progs/detpack.mdl" );
	trap_precache_model( "progs/ammobox.mdl" );
	trap_precache_model( "progs/v_knife.mdl" );
	trap_precache_model( "progs/v_knife2.mdl" );
	trap_precache_model( "progs/v_span.mdl" );
	trap_precache_model( "progs/e_spike1.mdl" );
	trap_precache_model( "progs/e_spike2.mdl" );
	trap_precache_model( "progs/v_rail.mdl" );
	trap_precache_model( "progs/turrgun.mdl" );
	trap_precache_model( "progs/turrbase.mdl" );
	trap_precache_model( "progs/tgib1.mdl" );
	trap_precache_model( "progs/tgib2.mdl" );
	trap_precache_model( "progs/tgib3.mdl" );
	trap_precache_model( "progs/disp.mdl" );
	trap_precache_model( "progs/dgib1.mdl" );
	trap_precache_model( "progs/dgib2.mdl" );
	trap_precache_model( "progs/dgib3.mdl" );
	trap_precache_model( "progs/grenade2.mdl" );
	trap_precache_model( "progs/v_grap.mdl" );
	trap_precache_model( "progs/hook.mdl" );
	trap_precache_model( "progs/caltrop.mdl" );
	trap_precache_sound( "doors/baseuse.wav" );
	trap_precache_sound( "doors/medtry.wav" );
	trap_precache_sound( "speech/saveme1.wav" );
	trap_precache_sound( "speech/saveme2.wav" );
	trap_precache_model( "progs/detpack2.mdl" );
	trap_precache_model( "progs/grenade3.mdl" );
	trap_precache_sound( "misc/b1.wav" );
	trap_precache_sound( "misc/b2.wav" );
	trap_precache_sound( "misc/b3.wav" );
	trap_precache_sound( "misc/b4.wav" );
	trap_precache_model( "progs/w_s_key.mdl" );
	trap_precache_model( "progs/m_s_key.mdl" );
	trap_precache_model( "progs/b_s_key.mdl" );
	trap_precache_model( "progs/w_g_key.mdl" );
	trap_precache_model( "progs/m_g_key.mdl" );
	trap_precache_model( "progs/b_g_key.mdl" );
//
// Setup light animation tables. 'a' is total darkness, 'z' is maxbright.
//

	// 0 normal
	trap_lightstyle( 0, "m" );

	// 1 FLICKER (first variety)
	trap_lightstyle( 1, "mmnmmommommnonmmonqnmmo" );

	// 2 SLOW STRONG PULSE
	trap_lightstyle( 2, "abcdefghijklmnopqrstuvwxyzyxwvutsrqponmlkjihgfedcba" );

	// 3 CANDLE (first variety)
	trap_lightstyle( 3, "mmmmmaaaaammmmmaaaaaabcdefgabcdefg" );

	// 4 FAST STROBE
	trap_lightstyle( 4, "mamamamamama" );

	// 5 GENTLE PULSE 1
	trap_lightstyle( 5, "jklmnopqrstuvwxyzyxwvutsrqponmlkj" );

	// 6 FLICKER (second variety)
	trap_lightstyle( 6, "nmonqnmomnmomomno" );

	// 7 CANDLE (second variety)
	trap_lightstyle( 7, "mmmaaaabcdefgmmmmaaaammmaamm" );

	// 8 CANDLE (third variety)
	trap_lightstyle( 8, "mmmaaammmaaammmabcdefaaaammmmabcdefmmmaaaa" );

	// 9 SLOW STROBE (fourth variety)
	trap_lightstyle( 9, "aaaaaaaazzzzzzzz" );

	// 10 FLUORESCENT FLICKER
	trap_lightstyle( 10, "mmamammmmammamamaaamammma" );

	// 11 SLOW PULSE NOT FADE TO BLACK
	trap_lightstyle( 11, "abcdefghijklmnopqrrqponmlkjihgfedcba" );

	// styles 32-62 are assigned by the light program for switchable lights

	// 63 testing
	trap_lightstyle( 63, "a" );

}

int     timelimit, fraglimit, teamplay, deathmatch, framecount, coop;


void StartFrame( int time )
{
	timelimit = trap_cvar( "timelimit" ) * 60;
	fraglimit = trap_cvar( "fraglimit" );
	teamplay = trap_cvar( "teamplay" );
	deathmatch = trap_cvar( "deathmatch" );

	framecount = framecount + 1;
}
