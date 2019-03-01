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
 *  $Id: weapons.c,v 1.36 2006-11-29 23:19:23 AngelD Exp $
 */

#include "g_local.h"

typedef struct {
    int w, bit_item, bit_ammo, wnum; 
	int			ammo_ofs;
	fieldtype_t	ammo_type;
    int ammo_shells, ammo_nails, ammo_rockets, ammo_cells;
    int clip, reload_time;
    void (* fire_func)();
    void (* can_use_f)();
    float attack_finished;
    char* model,*model_mode,*sound;
    int   have_mode;
}weapon_info_t;

const weapon_info_t weapons_info[]= {
{  0 },
{  WEAP_HOOK             , IT_AXE,              0,          0, 0,                     0,0,0,0,0, 0,0,         0,0, 0.1, "progs/v_grap.mdl", "", "", 0},
{  WEAP_BIOWEAPON        , IT_AXE,              0,          0, 0,                     0,0,0,0,0, 0,0,         0,0, 0.5, "progs/v_bio.mdl", "", "weapons/ax1.wav", 0},
{  WEAP_MEDIKIT          , IT_AXE,              0,          0, FOFS(ammo_medikit),    F_INT,0,0,0,0, 0,0,     0,0, 0.5, "progs/v_medi.mdl", "", "weapons/ax1.wav", 0},
{  WEAP_SPANNER          , IT_AXE,              0,          0, 0,                     0,0,0,0,0, 0,0,         0,0, 0.5, "progs/v_span.mdl", "", "weapons/ax1.wav", 0},
{  WEAP_AXE              , IT_AXE,              0,          0, 0,                     0,0,0,0,0, 0,0,         0,0, 0.5, "progs/v_axe.mdl", "", "weapons/ax1.wav", 0},
{  WEAP_SNIPER_RIFLE     , IT_SHOTGUN,          IT_SHELLS,  1, FOFS(s.v.ammo_shells), F_FLOAT,1,0,0,0, 0,0,   0,0, 1.5, "progs/v_srifle.mdl", "", "weapons/sniper.wav", 0},
{  WEAP_AUTO_RIFLE       , IT_SUPER_SHOTGUN,    IT_SHELLS,  2, FOFS(s.v.ammo_shells), F_FLOAT,1,0,0,0, 0,0,   0,0, 0.1, "progs/v_srifle.mdl", "", "weapons/sniper.wav", 0},
{  WEAP_SHOTGUN          , IT_SHOTGUN,          IT_SHELLS,  1, FOFS(s.v.ammo_shells), F_FLOAT,1,0,0,0, 8,2,   0,0, 0.5, "progs/v_shot.mdl", "", "weapons/guncock.wav", 0},
{  WEAP_SUPER_SHOTGUN    , IT_SUPER_SHOTGUN,    IT_SHELLS,  2, FOFS(s.v.ammo_shells), F_FLOAT,2,0,0,0, 16,2,  0,0, 0.7, "progs/v_shot2.mdl", "", "weapons/shotgn2.wav", 0},
{  WEAP_NAILGUN          , IT_NAILGUN,          IT_NAILS,   4, FOFS(s.v.ammo_nails),  F_FLOAT,0,1,0,0, 0,0,   0,0, 0.0, "progs/v_nail.mdl", "", "", 0},
{  WEAP_SUPER_NAILGUN    , IT_SUPER_NAILGUN,    IT_NAILS,   8, FOFS(s.v.ammo_nails),  F_FLOAT,0,4,0,0, 0,0,   0,0, 0.0, "progs/v_nail2.mdl", "", "", 0},
{  WEAP_GRENADE_LAUNCHER , IT_GRENADE_LAUNCHER, IT_ROCKETS, 16, FOFS(s.v.ammo_rockets),F_FLOAT,0,0,1,0, 6,4,   0,0, 0.6, "progs/v_rock.mdl", "progs/v_pipe.mdl", "weapons/grenade.wav", 1},
{  WEAP_FLAMETHROWER     , IT_GRENADE_LAUNCHER, IT_CELLS,   16, FOFS(s.v.ammo_cells),  F_FLOAT,0,0,0,1, 0,0,   0,0, 0.0, "progs/v_flame.mdl", "", "", 0},
{  WEAP_ROCKET_LAUNCHER  , IT_ROCKET_LAUNCHER,  IT_ROCKETS, 32, FOFS(s.v.ammo_rockets),F_FLOAT,0,0,1,0, 4,5,   0,0, 0.8, "progs/v_rock2.mdl", "", "weapons/sgun1.wav", 0},
{  WEAP_INCENDIARY       , IT_ROCKET_LAUNCHER,  IT_ROCKETS, 32, FOFS(s.v.ammo_rockets),F_FLOAT,0,0,3,0, 0,0,   0,0, 1.2, "progs/v_rock2.mdl", "", "weapons/sgun1.wav", 0},
{  WEAP_ASSAULT_CANNON   , IT_ROCKET_LAUNCHER,  IT_SHELLS,  32, FOFS(s.v.ammo_shells), F_FLOAT,0,0,0,6, 0,0,   0,0, 0.0, "progs/v_asscan.mdl", "", 0},
{  WEAP_LIGHTNING        , IT_LIGHTNING,        IT_CELLS,   64, FOFS(s.v.ammo_cells),  F_FLOAT,0,0,0,1, 0,0,   0,0, 0.1, "progs/v_light.mdl", "", "weapons/lstart.wav", 0},
{  WEAP_DETPACK          , 0,                   },
{  WEAP_TRANQ            , IT_SHOTGUN,          IT_SHELLS,  1, FOFS(s.v.ammo_shells), F_FLOAT,1,0,0,0, 0,0,   0,0, 1.5, "progs/v_tgun.mdl", "", "weapons/dartgun.wav", 0},
{  WEAP_LASER            , IT_SHOTGUN,          IT_NAILS,   1, FOFS(s.v.ammo_nails),  F_FLOAT,0,1,0,0, 0,0,   0,0, 0.4, "progs/v_rail.mdl", "", "weapons/railgun.wav", 0},
};

#define WEAPON_BY_BIT(bit) &weapons_info[log2powerof2(bit) + 1]

void    item_megahealth_rot(  );

float   button_touch(  );
void    button_fire(  );

void W_Precache(  )
{
	trap_precache_sound( "weapons/r_exp3.wav" );           // new rocket explosion               
	trap_precache_sound( "weapons/rocket1i.wav" );         // spike gun                          
	trap_precache_sound( "weapons/sgun1.wav" );                                                  
	trap_precache_sound( "weapons/guncock.wav" );          // player shotgun                     
	trap_precache_sound( "weapons/ric1.wav" );             // ricochet (used in c code)          
	trap_precache_sound( "weapons/ric2.wav" );             // ricochet (used in c code)          
	trap_precache_sound( "weapons/ric3.wav" );             // ricochet (used in c code)          
	trap_precache_sound( "weapons/spike2.wav" );           // super spikes                       
	trap_precache_sound( "weapons/tink1.wav" );            // spikes tink (used in c code)       
	trap_precache_sound( "weapons/grenade.wav" );          // grenade launcher                   
	trap_precache_sound( "weapons/bounce.wav" );           // grenade bounce                     
	trap_precache_sound( "weapons/shotgn2.wav" );          // super shotgun                      
	trap_precache_sound( "wizard/wattack.wav" );           // sniper rifle                       
	trap_precache_sound( "items/r_item1.wav" );            // Medikit                            
	trap_precache_sound( "items/r_item2.wav" );            // Medikit                            
	trap_precache_model( "progs/flame2.mdl" );             // Flamethrower                       
	trap_precache_sound( "ambience/fire1.wav" );                                                 
	trap_precache_sound( "blob/land1.wav" );               // Hook                               
	trap_precache_model( "progs/v_spike.mdl" );            // Hook                               
	trap_precache_sound( "hknight/hit.wav" );              	// Hook                       
	                                                                                             
	trap_precache_sound( "weapons/detpack.wav" );           
	trap_precache_sound( "weapons/turrset.wav" );            // Sentry Gun Setup                 
	trap_precache_sound( "weapons/turrspot.wav" );           // Sentry Gun Spot                  
	trap_precache_sound( "weapons/turridle.wav" );           // Sentry Gun Idle                  
	trap_precache_sound( "weapons/sniper.wav" );             // sniper rifle                       
	trap_precache_sound( "weapons/flmfire2.wav" );           //flamethrower                         
	trap_precache_sound( "weapons/flmgrexp.wav" );           //flamethrower                       
	trap_precache_sound( "misc/vapeur2.wav" );               // flamethrower                     
	trap_precache_sound( "weapons/asscan1.wav" );            // Assault Cannon Powerup           
	trap_precache_sound( "weapons/asscan2.wav" );            // Assault Cannon Churning          
	trap_precache_sound( "weapons/asscan3.wav" );            // Assault Cannon Powerdown         
	trap_precache_sound( "weapons/asscan4.wav" );            
	trap_precache_sound( "weapons/railgun.wav" );            // Railgun                          
	trap_precache_sound( "weapons/dartgun.wav" );            // Spy's dart gun                   
	trap_precache_sound( "count.wav" );                      // grenade prime sound

	trap_precache_model( "progs/v_flame.mdl" );              
	trap_precache_model( "progs/v_tgun.mdl" );

}


//======================================================================
// Calculate the attack_finished time
void Attack_Finished( float att_delay )
{
	if ( self->tfstate & TFSTATE_TRANQUILISED )
		self->attack_finished = g_globalvars.time + att_delay * 2;
	else
		self->attack_finished = g_globalvars.time + att_delay;
}

/*
================
W_FireAxe
================
*/

void W_FireAxe(  )
{
	vec3_t  source, dest;
	vec3_t  org, def;

	trap_makevectors( self->s.v.v_angle );

	VectorCopy( self->s.v.origin, source );
	source[2] += 16;
	VectorScale( g_globalvars.v_forward, 64, dest );
	VectorAdd( dest, source, dest )
        //source = self->s.v.origin + '0 0 16';
        traceline( PASSVEC3( source ), PASSVEC3( dest ), false, self );
	if ( g_globalvars.trace_fraction == 1.0 )
		return;

	VectorScale( g_globalvars.v_forward, 4, org );
	VectorSubtract( g_globalvars.trace_endpos, org, org );
// org = trace_endpos - g_globalvars.v_forward*4;
	if ( PROG_TO_EDICT( g_globalvars.trace_ent )->s.v.takedamage )
	{
		PROG_TO_EDICT( g_globalvars.trace_ent )->axhitme = 1;
		SpawnBlood( org, 20 );
		if ( self->playerclass != PC_SPY
		     || strneq( PROG_TO_EDICT( g_globalvars.trace_ent )->s.v.classname, "player" ) )
		{
			tf_data.deathmsg = DMSG_AXE;
			TF_T_Damage( PROG_TO_EDICT( g_globalvars.trace_ent ), self, self, 20, TF_TD_NOTTEAM, 0 );
		} else
		{
			self->weaponmode = 1;
			self->s.v.weaponmodel = "progs/v_knife2.mdl";
			trap_makevectors( PROG_TO_EDICT( g_globalvars.trace_ent )->s.v.v_angle );
			VectorCopy( g_globalvars.v_right, def );

			trap_makevectors( self->s.v.v_angle );
			if ( crossproduct( def, g_globalvars.v_forward ) > 0 )
			{
				tf_data.deathmsg = DMSG_BACKSTAB;
				TF_T_Damage( PROG_TO_EDICT( g_globalvars.trace_ent ), self, self, 120, TF_TD_NOTTEAM, 0 );
			} else
			{
				tf_data.deathmsg = DMSG_AXE;
				TF_T_Damage( PROG_TO_EDICT( g_globalvars.trace_ent ), self, self, 40, TF_TD_NOTTEAM, 0 );
			}
		}
	} else
	{
		sound( self, CHAN_WEAPON, "player/axhit2.wav", 1, ATTN_NORM );

		trap_WriteByte( MSG_MULTICAST, SVC_TEMPENTITY );
		trap_WriteByte( MSG_MULTICAST, TE_GUNSHOT );
		trap_WriteByte( MSG_MULTICAST, 3 );
		trap_WriteCoord( MSG_MULTICAST, org[0] );
		trap_WriteCoord( MSG_MULTICAST, org[1] );
		trap_WriteCoord( MSG_MULTICAST, org[2] );
		trap_multicast( PASSVEC3( org ), MULTICAST_PVS );
	}
}

void W_FireSpanner(  )
{
	vec3_t  source;
	vec3_t  org;
	vec3_t  dest;
	float   healam;
	gedict_t *te, *trace_ent;

	trap_makevectors( self->s.v.v_angle );
	VectorCopy( self->s.v.origin, source );
	source[2] += 16;
	VectorScale( g_globalvars.v_forward, 64, dest );
	VectorAdd( dest, source, dest ) traceline( PASSVEC3( source ), PASSVEC3( dest ), false, self );
	if ( g_globalvars.trace_fraction == 1.0 )
		return;

	VectorScale( g_globalvars.v_forward, 4, org );
	VectorSubtract( g_globalvars.trace_endpos, org, org );
	trace_ent = PROG_TO_EDICT( g_globalvars.trace_ent );

	if ( trace_ent->goal_activation & TFGA_SPANNER )
	{
		if ( Activated( trace_ent, self ) )
		{
			DoResults( trace_ent, self, 1 );
			if ( streq( trace_ent->s.v.classname, "func_button" ) )
			{
				trace_ent->s.v.enemy = EDICT_TO_PROG( self );
				other = self;
				self = trace_ent;
				self->dont_do_triggerwork = 1;
				button_fire(  );
				self = other;
			}
		} else
		{
			if ( trace_ent->else_goal )
			{
				te = Findgoal( trace_ent->else_goal );
				if ( te )
					AttemptToActivate( te, self, trace_ent );
			} else
			{
				sound( self, CHAN_WEAPON, "player/axhit2.wav", 1, ATTN_NORM );

				trap_WriteByte( MSG_MULTICAST, SVC_TEMPENTITY );
				trap_WriteByte( MSG_MULTICAST, TE_GUNSHOT );
				trap_WriteByte( MSG_MULTICAST, 3 );
				trap_WriteCoord( MSG_MULTICAST, org[0] );
				trap_WriteCoord( MSG_MULTICAST, org[1] );
				trap_WriteCoord( MSG_MULTICAST, org[2] );
				trap_multicast( PASSVEC3( org ), MULTICAST_PVS );
			}
		}
		return;
	}
	if ( trace_ent->s.v.takedamage )
	{
		if ( streq( trace_ent->s.v.classname, "building_dispenser" ) )
		{
			Engineer_UseDispenser( trace_ent );
			return;
		} else
		{
			if ( streq( trace_ent->s.v.classname, "building_sentrygun" ) )
			{
				Engineer_UseSentryGun( trace_ent );
				return;
			} else
			{
				if ( streq( trace_ent->s.v.classname, "building_sentrygun_base" ) )
				{
					if ( trace_ent->oldenemy )
						Engineer_UseSentryGun( trace_ent->oldenemy );
					return;
				} else
				{
					if ( streq( trace_ent->s.v.classname, "player" ) )
					{
						if ( ( TeamFortress_isTeamsAllied(trace_ent->team_no ,self->team_no)
						       && self->team_no && teamplay ) || coop )
						{
							healam = WEAP_SPANNER_REPAIR;
							if ( self->s.v.ammo_cells < healam )
								healam = self->s.v.ammo_cells;
							if ( !trace_ent->s.v.armortype )
								return;
							if ( trace_ent->maxarmor -
							     trace_ent->s.v.armorvalue < healam * 4 )
								healam =
								    ceil( ( trace_ent->maxarmor - trace_ent->s.v.armorvalue ) / 4 );
							if ( healam > 0 )
							{
								trace_ent->s.v.armorvalue = trace_ent->s.v.armorvalue + healam * 4;
								if ( trace_ent->s.v.armorvalue > trace_ent->maxarmor )
									trace_ent->s.v.armorvalue = trace_ent->maxarmor;
								if ( !tg_data.unlimit_ammo )
									self->s.v.ammo_cells = self->s.v.ammo_cells - healam;
								sound( trace_ent,
								       CHAN_WEAPON, "items/r_item1.wav", 1, ATTN_NORM );
								trap_WriteByte( MSG_MULTICAST, SVC_TEMPENTITY );
								trap_WriteByte( MSG_MULTICAST, TE_GUNSHOT );
								trap_WriteByte( MSG_MULTICAST, 3 );
								trap_WriteCoord( MSG_MULTICAST, org[0] );
								trap_WriteCoord( MSG_MULTICAST, org[1] );
								trap_WriteCoord( MSG_MULTICAST, org[2] );
								trap_multicast( PASSVEC3( org ), MULTICAST_PVS );
								W_SetCurrentAmmo(  );
							}
							return;
						}
					}
					trace_ent->axhitme = 1;
					SpawnBlood( org, 20 );
					tf_data.deathmsg = DMSG_SPANNER;
					TF_T_Damage( trace_ent, self, self, 20, TF_TD_NOTTEAM, 0 );
				}
			}
		}
	} else
	{
		sound( self, 1, "player/axhit2.wav", 1, 1 );
		trap_WriteByte( MSG_MULTICAST, SVC_TEMPENTITY );
		trap_WriteByte( MSG_MULTICAST, TE_GUNSHOT );
		trap_WriteByte( MSG_MULTICAST, 3 );
		trap_WriteCoord( MSG_MULTICAST, org[0] );
		trap_WriteCoord( MSG_MULTICAST, org[1] );
		trap_WriteCoord( MSG_MULTICAST, org[2] );
		trap_multicast( PASSVEC3( org ), MULTICAST_PVS );
	}
}

void W_FireMedikit(  )
{
	vec3_t  source, dest;
	vec3_t  org;
	float   healam;
	gedict_t *te;
	gedict_t *BioInfection, *trace_ent;

	trap_makevectors( self->s.v.v_angle );

	VectorCopy( self->s.v.origin, source );
	source[2] += 16;
	VectorScale( g_globalvars.v_forward, 64, dest );
	VectorAdd( dest, source, dest )
	    //source = self->s.v.origin + '0 0 16';
	    traceline( PASSVEC3( source ), PASSVEC3( dest ), false, self );
	if ( g_globalvars.trace_fraction == 1.0 )
		return;

	VectorScale( g_globalvars.v_forward, 4, org );
	VectorSubtract( g_globalvars.trace_endpos, org, org );
	trace_ent = PROG_TO_EDICT( g_globalvars.trace_ent );
	if ( trace_ent->s.v.takedamage )
	{
		if ( streq( trace_ent->s.v.classname, "player" ) )
		{
			if ( ( TeamFortress_isTeamsAllied(trace_ent->team_no , self->team_no) && self->team_no ) || coop )
			{
				healam = WEAP_MEDIKIT_HEAL;
				for ( te=world ; (te = trap_find( te, FOFS( s.v.classname ), "timer" ));)
				{
					if ( te->s.v.owner != g_globalvars.trace_ent )
						continue;
					if ( te->s.v.think != ( func_t ) ConcussionGrenadeTimer &&
						te->s.v.think != ( func_t ) OldConcussionGrenadeTimer )
						continue;
					if ( tf_data.old_grens == 1 )
                    {
						stuffcmd( trace_ent, "v_idlescale 0\nfov 90\n" );
                        trace_ent->eff_info.conc_idle = 0;
                    }
					SpawnBlood( org, 20 );
					G_bprint( 1, "%s cured %s's concussion\n", self->s.v.netname, trace_ent->s.v.netname );
					if ( !TeamFortress_isTeamsAllied(te->team_no , self->team_no) )
						TF_AddFrags( self, 1 );
					dremove( te );
					break;
				}
				if ( trace_ent->tfstate & TFSTATE_HALLUCINATING )
				{
				    for ( te=world ; (te = trap_find( te, FOFS( s.v.classname ), "timer" ));)
					{
						if ( te->s.v.owner != g_globalvars.trace_ent )
							continue;
						if ( te->s.v.think != ( func_t ) HallucinationTimer )
							continue;

						trace_ent->tfstate -= ( trace_ent->tfstate & TFSTATE_HALLUCINATING );
						SpawnBlood( org, 20 );
						G_bprint( 1, "%s healed %s of his hallucinations\n",
							  self->s.v.netname, trace_ent->s.v.netname );
						
						ResetGasSkins(trace_ent);
						if ( tf_data.new_gas & GAS_MASK_PALETTE) 
							stuffcmd( trace_ent, "v_cshift; wait; bf\n" );
						if ( !TeamFortress_isTeamsAllied(te->team_no , self->team_no) )
							TF_AddFrags( self, 1 );
						dremove( te );
						break;
					}
					if ( !te )
						G_conprintf( "Warning: Error in Hallucination Timer logic.\n" );
				}
				if ( trace_ent->tfstate & TFSTATE_TRANQUILISED )
				{
				    for ( te=world ; (te = trap_find( te, FOFS( s.v.classname ), "timer" ));)
					{
						if ( te->s.v.owner != g_globalvars.trace_ent )
							continue;
						if ( te->s.v.think != ( func_t ) TranquiliserTimer )
							continue;

						trace_ent->tfstate -= ( trace_ent->tfstate & TFSTATE_TRANQUILISED );
						TeamFortress_SetSpeed( trace_ent );
						SpawnBlood( org, 20 );
						G_bprint( 1, "%s healed %s's tranquilisation\n",
							  self->s.v.netname, trace_ent->s.v.netname );
						if ( !TeamFortress_isTeamsAllied(te->team_no , self->team_no) )
							TF_AddFrags( self, 1 );
						dremove( te );
						break;
					}
					if ( !te )
						G_conprintf( "Warning: Error in Tranquilisation Timer logic.\n" );
				}
				if ( trace_ent->FlashTime > 0 )
				{
				    for ( te=world ; (te = trap_find( te, FOFS( s.v.netname ), "flashtimer" ));)
					{
						if ( te->s.v.owner != g_globalvars.trace_ent )
							continue;
						if ( strneq( te->s.v.classname, "timer" ) )
							continue;

						trace_ent->FlashTime = 0;
						SpawnBlood( org, 20 );
						G_bprint( 1, "%s cured %s's blindness\n",
							  self->s.v.netname, trace_ent->s.v.netname );
						if ( !TeamFortress_isTeamsAllied(te->team_no , self->team_no) )
							TF_AddFrags( self, 1 );
						if ( tf_data.new_flash )
							disableupdates( trace_ent, -1 );	/* server-side flash */
						break;
					}
					if ( !te )
					{
						G_conprintf( "Warning: Error in Flash Timer logic.\n" );
						trace_ent->FlashTime = 0;
					}
				}
				if ( trace_ent->tfstate & TFSTATE_INFECTED )
				{
					healam = Q_rint( trace_ent->s.v.health / 2 );
					trace_ent->tfstate -= ( trace_ent->tfstate & TFSTATE_INFECTED );
					tf_data.deathmsg = DMSG_MEDIKIT;
					T_Damage( trace_ent, self, self, healam );
					SpawnBlood( org, 30 );
					if ( streq( self->s.v.classname, "player" ) )
					{
						G_bprint( 1, "%s cured %s's infection\n",
							  self->s.v.netname, trace_ent->s.v.netname );
						if ( !TeamFortress_isTeamsAllied(trace_ent->infection_team_no , self->team_no) )
							TF_AddFrags( self, 1 );
					}
					return;
				}
				if ( trace_ent->numflames > 0 )
				{
					sound( trace_ent, 1, "items/r_item1.wav", 1, 1 );
					trace_ent->numflames = 0;
					if ( streq( self->s.v.classname, "player" ) )
					{
						G_bprint( 1, "%s put out %s's fire.\n",
							  self->s.v.netname, trace_ent->s.v.netname );
					}
					return;
				}
				
				if ( healam > 0 && trace_ent->s.v.health < trace_ent->s.v.max_health )
				{
					sound( trace_ent, 1, "items/r_item1.wav", 1, 1 );
					trace_ent->axhitme = 1;
					SpawnBlood( org, 20 );
					T_Heal( trace_ent, healam, 0 );
				} else
				{
					if ( trace_ent->s.v.health >=
					     trace_ent->s.v.max_health
					     && trace_ent->s.v.health <
					     trace_ent->s.v.max_health + WEAP_MEDIKIT_OVERHEAL )
					{
						healam = 5;
						if ( healam > self->ammo_medikit * 5 )
							healam = self->ammo_medikit * 5;
						if ( healam > 0 )
						{
							sound( trace_ent, 3, "items/r_item2.wav", 1, 1 );
							T_Heal( trace_ent, healam, 1 );
							self->ammo_medikit -= healam / 5;
							if ( !( ( int ) trace_ent->s.v.items & IT_SUPERHEALTH ) )
							{
								trace_ent->s.v.items =
								    ( int ) trace_ent->s.v.items | IT_SUPERHEALTH;
								newmis = spawn(  );
								newmis->s.v.nextthink = g_globalvars.time + 5;
								newmis->s.v.think = ( func_t ) item_megahealth_rot;
								newmis->s.v.owner = EDICT_TO_PROG( trace_ent );
							}
							self->s.v.currentammo = self->ammo_medikit;
						}
					}
				}
			} else
			{
//not teammate
				trace_ent->axhitme = 1;
				SpawnBlood( org, 20 );
				tf_data.deathmsg = DMSG_BIOWEAPON_ATT;
				T_Damage( trace_ent, self, self, 10 );
				if ( trace_ent->playerclass == PC_MEDIC )
					return;
				if ( tf_data.cb_prematch_time > g_globalvars.time )
					return;
				if ( trace_ent->tfstate & TFSTATE_INFECTED )
					return;
				trace_ent->tfstate |= TFSTATE_INFECTED;
				BioInfection = spawn(  );
				BioInfection->s.v.classname = "timer";
				BioInfection->s.v.owner = EDICT_TO_PROG( trace_ent );
				BioInfection->s.v.nextthink = g_globalvars.time + 2;
				BioInfection->s.v.think = ( func_t ) BioInfection_Decay;
				BioInfection->s.v.enemy = EDICT_TO_PROG( self );
				trace_ent->infection_team_no = self->team_no;
			}
		}
	} else
	{
		sound( self, CHAN_WEAPON, "player/axhit2.wav", 1, ATTN_NORM );

		trap_WriteByte( MSG_MULTICAST, SVC_TEMPENTITY );
		trap_WriteByte( MSG_MULTICAST, TE_GUNSHOT );
		trap_WriteByte( MSG_MULTICAST, 3 );
		trap_WriteCoord( MSG_MULTICAST, org[0] );
		trap_WriteCoord( MSG_MULTICAST, org[1] );
		trap_WriteCoord( MSG_MULTICAST, org[2] );
		trap_multicast( PASSVEC3( org ), MULTICAST_PVS );
	}
}

void W_FireBioweapon(  )
{
	vec3_t  source, dest;

	vec3_t  org;
	gedict_t *BioInfection, *trace_ent;

	trap_makevectors( self->s.v.v_angle );

	VectorCopy( self->s.v.origin, source );
	source[2] += 16;
	VectorScale( g_globalvars.v_forward, 64, dest );
	VectorAdd( dest, source, dest )
	    //source = self->s.v.origin + '0 0 16';
	    traceline( PASSVEC3( source ), PASSVEC3( dest ), false, self );
	if ( g_globalvars.trace_fraction == 1.0 )
		return;

	VectorScale( g_globalvars.v_forward, 4, org );
	VectorSubtract( g_globalvars.trace_endpos, org, org );
	trace_ent = PROG_TO_EDICT( g_globalvars.trace_ent );

	if ( trace_ent->s.v.takedamage )
	{
		if ( streq( trace_ent->s.v.classname, "player" ) )
		{
			if ( ( !TeamFortress_isTeamsAllied(trace_ent->team_no , self->team_no) && self->team_no && teamplay ) || !teamplay )
			{
				trace_ent->axhitme = 1;
				SpawnBlood( org, 20 );
				tf_data.deathmsg = DMSG_BIOWEAPON_ATT;
				T_Damage( trace_ent, self, self, 10 );
				if ( trace_ent->playerclass == PC_MEDIC )
					return;
				if ( tf_data.cb_prematch_time > g_globalvars.time )
					return;
				trace_ent->tfstate |= TFSTATE_INFECTED;
				BioInfection = spawn(  );
				BioInfection->s.v.classname = "timer";
				BioInfection->s.v.owner = g_globalvars.trace_ent;
				BioInfection->s.v.nextthink = g_globalvars.time + 2;
				BioInfection->s.v.think = ( func_t ) BioInfection_Decay;
				BioInfection->s.v.enemy = EDICT_TO_PROG( self );
				trace_ent->infection_team_no = self->team_no;
			}
		} else
		{
			if ( ( int ) trace_ent->s.v.flags & FL_MONSTER )
			{
				if ( streq( trace_ent->s.v.classname, "monster_zombie" ) )
					T_Damage( trace_ent, self, self, 200 );
				trace_ent->axhitme = 1;
				SpawnBlood( org, 20 );
				T_Damage( trace_ent, self, self, 10 );
				BioInfection = spawn(  );
				BioInfection->s.v.classname = "timer";
				BioInfection->s.v.nextthink = g_globalvars.time + 2;
				BioInfection->s.v.think = ( func_t ) BioInfection_MonsterDecay;
				BioInfection->s.v.owner = EDICT_TO_PROG( self );
				BioInfection->s.v.enemy = g_globalvars.trace_ent;
			} else
			{
				trace_ent->axhitme = 1;
				SpawnBlood( org, 30 );
				T_Damage( trace_ent, self, self, 40 );
			}
		}
	} else
	{
		sound( self, CHAN_WEAPON, "player/axhit2.wav", 1, ATTN_NORM );

		trap_WriteByte( MSG_MULTICAST, SVC_TEMPENTITY );
		trap_WriteByte( MSG_MULTICAST, TE_GUNSHOT );
		trap_WriteByte( MSG_MULTICAST, 3 );
		trap_WriteCoord( MSG_MULTICAST, org[0] );
		trap_WriteCoord( MSG_MULTICAST, org[1] );
		trap_WriteCoord( MSG_MULTICAST, org[2] );
		trap_multicast( PASSVEC3( org ), MULTICAST_PVS );
	}
}

//============================================================================


//============================================================================

void wall_velocity( vec3_t v )
{
	vec3_t  vel;

	normalize( self->s.v.velocity, vel );

	vel[0] += g_globalvars.v_up[0] * ( g_random(  ) - 0.5 ) + g_globalvars.v_right[0] * ( g_random(  ) - 0.5 );

	vel[1] += g_globalvars.v_up[1] * ( g_random(  ) - 0.5 ) + g_globalvars.v_right[1] * ( g_random(  ) - 0.5 );
	vel[2] += g_globalvars.v_up[2] * ( g_random(  ) - 0.5 ) + g_globalvars.v_right[2] * ( g_random(  ) - 0.5 );

	VectorNormalize( vel );

	vel[0] += 2 * g_globalvars.trace_plane_normal[0];
	vel[1] += 2 * g_globalvars.trace_plane_normal[1];
	vel[2] += 2 * g_globalvars.trace_plane_normal[2];

	VectorScale( vel, 200, v );

	//return vel;
}

/*
================
SpawnMeatSpray
================
*/
void SpawnMeatSpray( vec3_t org, vec3_t vel )
{
	gedict_t *missile;

	missile = spawn(  );
	missile->s.v.owner = EDICT_TO_PROG( self );
	missile->s.v.movetype = MOVETYPE_BOUNCE;
	missile->s.v.solid = SOLID_NOT;

	trap_makevectors( self->s.v.angles );

	VectorCopy( vel, missile->s.v.velocity );
	missile->s.v.velocity[2] = missile->s.v.velocity[2] + 250 + 50 * g_random(  );

	SetVector( missile->s.v.avelocity, 3000, 1000, 2000 );

// set missile duration
	missile->s.v.nextthink = g_globalvars.time + 1;
	missile->s.v.think = ( func_t ) SUB_Remove;

	setmodel( missile, "progs/zom_gib.mdl" );
	setsize( missile, 0, 0, 0, 0, 0, 0 );
	setorigin( missile, PASSVEC3( org ) );
}

/*
================
SpawnBlood
================
*/
void SpawnBlood( vec3_t org, float damage )
{
	trap_WriteByte( MSG_MULTICAST, SVC_TEMPENTITY );
	trap_WriteByte( MSG_MULTICAST, TE_BLOOD );
	trap_WriteByte( MSG_MULTICAST, 1 );
	trap_WriteCoord( MSG_MULTICAST, org[0] );
	trap_WriteCoord( MSG_MULTICAST, org[1] );
	trap_WriteCoord( MSG_MULTICAST, org[2] );
	trap_multicast( PASSVEC3( org ), MULTICAST_PVS );
}

/*
================
spawn_touchblood
================
*/

void spawn_touchblood( float damage )
{
	vec3_t  vel;

	wall_velocity( vel );
	VectorScale( vel, 0.2 * 0.01, vel );
	VectorAdd( self->s.v.origin, vel, vel );
	SpawnBlood( vel, damage );
}

void SpawnChunk( vec3_t org, vec3_t vel )
{
	//particle(org, vel * 0.02, 0, 10); //not exist in qw server
}

/*
==============================================================================

MULTI-DAMAGE

Collects multiple small damages into a single damage

==============================================================================
*/
static gedict_t *multi_ent;
static float   multi_damage;

static vec3_t  blood_org;
static float   blood_count;

vec3_t  puff_org;
static float   puff_count;

void ClearMultiDamage(  )
{
	multi_ent = world;
	multi_damage = 0;
	blood_count = 0;
	puff_count = 0;
}

void ApplyMultiDamage(  )
{
	if ( multi_ent == world || !multi_ent )
		return;
	TF_T_Damage( multi_ent, self, self, multi_damage, TF_TD_NOTTEAM, TF_TD_SHOT );
}

void AddMultiDamage( gedict_t * hit, float damage )
{
	if ( !hit )
		return;
	if ( hit != multi_ent )
	{
		ApplyMultiDamage(  );
		multi_damage = damage;
		multi_ent = hit;
	} else
		multi_damage = multi_damage + damage;
}

void Multi_Finish(  )
{
	if ( puff_count )
	{
		trap_WriteByte( MSG_MULTICAST, SVC_TEMPENTITY );
		trap_WriteByte( MSG_MULTICAST, TE_GUNSHOT );
		trap_WriteByte( MSG_MULTICAST, puff_count );
		trap_WriteCoord( MSG_MULTICAST, puff_org[0] );
		trap_WriteCoord( MSG_MULTICAST, puff_org[1] );
		trap_WriteCoord( MSG_MULTICAST, puff_org[2] );
		trap_multicast( PASSVEC3( puff_org ), MULTICAST_PVS );
	}

	if ( blood_count )
	{
		trap_WriteByte( MSG_MULTICAST, SVC_TEMPENTITY );
		trap_WriteByte( MSG_MULTICAST, TE_BLOOD );
		trap_WriteByte( MSG_MULTICAST, blood_count );
		trap_WriteCoord( MSG_MULTICAST, blood_org[0] );
		trap_WriteCoord( MSG_MULTICAST, blood_org[1] );
		trap_WriteCoord( MSG_MULTICAST, blood_org[2] );
		trap_multicast( PASSVEC3( puff_org ), MULTICAST_PVS );
	}
}

/*
==============================================================================
BULLETS
==============================================================================
*/
/*
================
TraceAttack
================
*/
// dir must be normalized
void TraceAttack( float damage, vec3_t dir )
{
	vec3_t  	org, tmp;

	// org = trace_endpos - dir*4;
	VectorScale( dir, 4, tmp );
	VectorSubtract( g_globalvars.trace_endpos, tmp, org );
	
	if ( PROG_TO_EDICT( g_globalvars.trace_ent )->s.v.takedamage )
	{
		blood_count = blood_count + 1;
		VectorCopy( org, blood_org );	//  blood_org = org;
		AddMultiDamage( PROG_TO_EDICT( g_globalvars.trace_ent ), damage );
	} else
	{
		puff_count = puff_count + 1;
	}
}

/*
================
FireBullets

Used by shotgun, super shotgun, and enemy soldier firing
Go to the trouble of combining multiple pellets into a single damage call.
================
*/
void FireBullets( float shotcount, vec3_t dir, float spread_x, float spread_y, float spread_z )
{
    vec3_t  direction;
    vec3_t  src, tmp,end;

    trap_makevectors( self->s.v.v_angle );
    VectorScale( g_globalvars.v_forward, 10, tmp );
    VectorAdd( self->s.v.origin, tmp, src );
    //src = self->s.v.origin + g_globalvars.v_forward*10;
    src[2] = self->s.v.absmin[2] + self->s.v.size[2] * 0.7;

    ClearMultiDamage(  );
#ifdef NEWHWGUY
    if (self->current_weapon != WEAP_ASSAULT_CANNON ) {
#endif
        VectorScale( dir, 2048 ,end);
        VectorAdd( src, end ,end );
        traceline( PASSVEC3( src ), PASSVEC3( end ), false, self );
        VectorScale( dir, 4, tmp );
        VectorSubtract( g_globalvars.trace_endpos, tmp, puff_org );	// puff_org = trace_endpos - dir*4;
#ifdef NEWHWGUY
    }else{

        VectorScale( g_globalvars.v_right, crandom(  ) * spread_x / 1.5 , tmp );
        VectorAdd( dir, tmp, direction );
        VectorScale( g_globalvars.v_up, crandom(  ) * spread_y /1.5 , tmp );
        VectorAdd( direction, tmp, direction );

        VectorScale( direction, 2048 ,end);
        VectorAdd( src, end ,end );

        traceline( PASSVEC3( src ), PASSVEC3( end ), false, self );

        normalize( dir, tmp );
        VectorScale( tmp, 4, tmp );
        VectorSubtract( g_globalvars.trace_endpos, tmp, puff_org );
    }
#endif

    while ( shotcount > 0 )
    {
        VectorScale( g_globalvars.v_right, crandom(  ) * spread_x, tmp );
        VectorAdd( dir, tmp, direction );
        VectorScale( g_globalvars.v_up, crandom(  ) * spread_y, tmp );
        VectorAdd( direction, tmp, direction );

        VectorScale( direction, 2048, tmp );
        VectorAdd( src, tmp, tmp );
        traceline( PASSVEC3( src ), PASSVEC3( tmp ), false, self );
        if ( g_globalvars.trace_fraction != 1.0 )
        {
#ifndef NEWHWGUYold
            if ( self->s.v.weapon != WEAP_ASSAULT_CANNON )
#else
                if ( self->current_weapon != WEAP_ASSAULT_CANNON )
#endif
                    TraceAttack( 4, direction );
                else
                    TraceAttack( 12, direction );
        }
        shotcount = shotcount - 1;
    }

    ApplyMultiDamage(  );
    Multi_Finish(  );
}

/*
================
W_FireShotgun
================
*/
void W_FireShotgun(  )
{
	vec3_t  dir;

	sound( self, CHAN_WEAPON, "weapons/guncock.wav", 1, ATTN_NORM );
	KickPlayer( -2, self );
	if ( !tg_data.unlimit_ammo )
		self->s.v.currentammo = --( self->s.v.ammo_shells );
	aim( dir );
	tf_data.deathmsg = DMSG_SHOTGUN;
	FireBullets( 6, dir, 0.04, 0.04, 0 );
}

/*
================
W_FireSuperShotgun
================
*/
void W_FireSuperShotgun(  )
{
	vec3_t  dir;

	if ( self->s.v.currentammo == 1 )
	{
		W_FireShotgun(  );
		return;
	}
	sound( self, CHAN_WEAPON, "weapons/shotgn2.wav", 1, ATTN_NORM );
	KickPlayer( -4, self );
	if ( !tg_data.unlimit_ammo )
		self->s.v.currentammo = self->s.v.ammo_shells = self->s.v.ammo_shells - 2;
	aim( dir );
	tf_data.deathmsg = DMSG_SSHOTGUN;
	FireBullets( 14, dir, 0.14, 0.08, 0 );
}

/*
================
FireSniperBullet
================
*/

void FireSniperBullet( vec3_t direction, float damage )
{
	vec3_t  src;

	trap_makevectors( self->s.v.v_angle );

	src[0] = self->s.v.origin[0] + g_globalvars.v_forward[0] * 10;
	src[1] = self->s.v.origin[1] + g_globalvars.v_forward[1] * 10;
	src[2] = self->s.v.absmin[2] + self->s.v.size[2] * 0.7;
	ClearMultiDamage(  );
	traceline( PASSVEC3( src ), src[0] + direction[0] * 4096, src[1] + direction[1] * 4096,
		   src[2] + direction[2] * 4096, false, self );

	if ( g_globalvars.trace_fraction != 1 )
		TraceAttack( damage, direction );
	if ( !g_globalvars.trace_ent )
	{
		trap_WriteByte( MSG_BROADCAST, SVC_TEMPENTITY );
		trap_WriteByte( MSG_BROADCAST, TE_SPIKE );
		trap_WriteCoord( MSG_BROADCAST, g_globalvars.trace_endpos[0] );
		trap_WriteCoord( MSG_BROADCAST, g_globalvars.trace_endpos[1] );
		trap_WriteCoord( MSG_BROADCAST, g_globalvars.trace_endpos[2] );
		trap_multicast( PASSVEC3( g_globalvars.trace_endpos ), MULTICAST_PHS );
	}

	ApplyMultiDamage(  );
}

void W_FireSniperRifle(  )
{
	vec3_t  dir, src;
	float   dam_mult;
	float   zdif;
	//float   use_this;
	vec3_t  f;
	vec3_t  g;
	vec3_t  h, tmp;
	gedict_t *trace_ent;

	if ( tf_data.snip_fps )
	{
		self->heat = ( g_globalvars.time - self->heat ) * tf_data.snip_fps * 3 + SR_INITDAMAGE;

		if ( self->heat > SR_MAXDAMAGE )
			self->heat = SR_MAXDAMAGE;
	}

	sound( self, 1, "weapons/sniper.wav", 1, 1 );
	KickPlayer( -2, self );
	if ( !tg_data.unlimit_ammo )
		self->s.v.currentammo = ( self->s.v.ammo_shells -= tf_data.snip_ammo);
	trap_makevectors( self->s.v.v_angle );
	VectorCopy( g_globalvars.v_forward, dir );

	src[0] = self->s.v.origin[0] + g_globalvars.v_forward[0] * 10;
	src[1] = self->s.v.origin[1] + g_globalvars.v_forward[1] * 10;
	src[2] = self->s.v.absmin[2] + self->s.v.size[2] * 0.7;

	if ( tf_data.snip_range_fix )
	       traceline( PASSVEC3( src ), src[0] + dir[0] * 8092, src[1] + dir[1] * 8092,
	       	       src[2] + dir[2] * 8092, false, self );
	else
		traceline( PASSVEC3( src ), src[0] + dir[0] * 3072, src[1] + dir[1] * 3072,
			   src[2] + dir[2] * 3072, false, self );
	        
	        
	trace_ent = PROG_TO_EDICT( g_globalvars.trace_ent );

	KickPlayer( -4, self );

	tf_data.deathmsg = DMSG_SNIPERRIFLE;
	dam_mult = 1;
	if ( g_globalvars.trace_ent )
	{
		if ( streq( trace_ent->s.v.classname, "player" ) )
		{
			VectorSubtract( g_globalvars.trace_endpos, src, f );
			g[0] = g_globalvars.trace_endpos[0];
			g[1] = g_globalvars.trace_endpos[1];
			g[2] = 0;
			h[0] = trace_ent->s.v.origin[0];
			h[1] = trace_ent->s.v.origin[1];
			h[2] = 0;
			VectorSubtract( g, h, tmp );
			VectorNormalize( f );
			VectorScale( f, vlen( tmp ), f );
			VectorAdd( f, g_globalvars.trace_endpos, f );
			//f = normalize(f) * x + g_globalvars.trace_endpos;
			zdif = f[2] - trace_ent->s.v.origin[2];
			tf_data.deathmsg = DMSG_SNIPERRIFLE;
			SetVector( trace_ent->head_shot_vector, 0, 0, 0 );
			if ( zdif < 0 )
			{
				dam_mult = 0.5;
				trace_ent->leg_damage = trace_ent->leg_damage + 1;
				TeamFortress_SetSpeed( trace_ent );
				tf_data.deathmsg = DMSG_SNIPERLEGSHOT;
				TF_T_Damage( trace_ent, self, self, self->heat * dam_mult, TF_TD_NOTTEAM, TF_TD_SHOT );
				if ( trace_ent->s.v.health > 0 )
				{
					G_sprint( trace_ent, 0, "Leg injury!\n" );
					G_sprint( self, 1, "Leg shot - that'll slow him down!\n" );
				}
				return;
			} else
			{
				if ( zdif > 20 )
				{
					dam_mult = 2;
					stuffcmd( trace_ent, "bf\n" );
					VectorSubtract( trace_ent->s.v.origin, self->s.v.origin,
							trace_ent->head_shot_vector );
					//trace_ent->head_shot_vector = trace_ent->s.v.origin - self->s.v.origin;
					tf_data.deathmsg = DMSG_SNIPERHEADSHOT;
					TF_T_Damage( trace_ent, self, self, self->heat * dam_mult, TF_TD_NOTTEAM, TF_TD_SHOT );
					if ( trace_ent->s.v.health > 0 )
					{
						G_sprint( trace_ent, 0, "Head injury!\n" );
						G_sprint( self, 1, "Head shot - that's gotta hurt!\n" );
					}
					return;
				} else
					tf_data.deathmsg = DMSG_SNIPERRIFLE;
			}
		}
	}else
	{
		trap_WriteByte( MSG_BROADCAST, SVC_TEMPENTITY );
		trap_WriteByte( MSG_BROADCAST, TE_SPIKE );
		trap_WriteCoord( MSG_BROADCAST, g_globalvars.trace_endpos[0] );
		trap_WriteCoord( MSG_BROADCAST, g_globalvars.trace_endpos[1] );
		trap_WriteCoord( MSG_BROADCAST, g_globalvars.trace_endpos[2] );
		trap_multicast( PASSVEC3( g_globalvars.trace_endpos ), MULTICAST_PHS );
	}
	ClearMultiDamage(  );
	if ( g_globalvars.trace_fraction != 1 )
	{
		TraceAttack( self->heat * dam_mult, dir );
	}
	ApplyMultiDamage(  );
}

void W_FireAutoRifle(  )
{
	vec3_t  dir;

	sound( self, 1, "weapons/sniper.wav", 1, 1 );
	KickPlayer( -1, self );
	if ( !tg_data.unlimit_ammo )
		self->s.v.currentammo = --( self->s.v.ammo_shells );
	trap_makevectors( self->s.v.v_angle );
	VectorCopy( g_globalvars.v_forward, dir );
	tf_data.deathmsg = DMSG_AUTORIFLE;
	FireSniperBullet( dir, 8 );
}

void W_FireAssaultCannon(  )
{
    vec3_t  dir;

#ifdef 	NEWHWGUY
    vec3_t  spread = { 0.04, 0.04, 0 };

    if ( self->heat < ( ( ASSAULT_SPREAD_TIME / 0.2 ) * 0.1 ) )
    {
        VectorScale( spread,
                ( 1 +
                  ( self->heat / ( ( ASSAULT_SPREAD_TIME / 0.2 ) * 0.1 ) ) * ( ASSAULT_SPREAD_MULTIPLY -
                      1 ) ), spread );
        //spread=spread*(1+(self.heat/((ASSAULT_SPREAD_TIME/0.2)*0.1))*( ASSAULT_SPREAD_MULTIPLY - 1));
    } else
        VectorScale( spread, ASSAULT_SPREAD_MULTIPLY, spread );
#endif
    KickPlayer( -4, self );

    if ( !tg_data.unlimit_ammo )
        self->s.v.currentammo = --( self->s.v.ammo_shells );

    trap_makevectors( self->s.v.v_angle);
    aim( dir );
    tf_data.deathmsg = DMSG_ASSAULTCANNON;

    FireBullets( 5, dir, 0.04, 0.04, 0 );
#ifdef NEWHWGUY
    FireBullets( 5, dir, PASSVEC3( spread ) );
#endif
}

/*
==============================================================================

ROCKETS

==============================================================================
*/

void BecomeExplosion(  )
{
	dremove( self );
}

void T_MissileTouch(  )
{
	float   damg;
	vec3_t  tmp;

	if ( self->voided )
	{
		return;
	}
	self->voided = 1;
	if ( trap_pointcontents( PASSVEC3( self->s.v.origin ) ) == CONTENT_SKY )
	{
		ent_remove( self );
		return;
	}
	damg = 92 + g_random(  ) * 20;
	tf_data.deathmsg = self->s.v.weapon;
	if ( other->s.v.health )
		TF_T_Damage( other, self, PROG_TO_EDICT( self->s.v.owner ), damg, TF_TD_NOTTEAM, 0 );
	if ( streq( PROG_TO_EDICT( self->s.v.owner )->s.v.classname, "building_sentrygun" ) )
		T_RadiusDamage( self, PROG_TO_EDICT( self->s.v.owner ), 150, other );
	else
		T_RadiusDamage( self, PROG_TO_EDICT( self->s.v.owner ), 92, other );
	normalize( self->s.v.velocity, tmp );
	VectorScale( tmp, -8, tmp );
	VectorAdd( self->s.v.origin, tmp, self->s.v.origin ) trap_WriteByte( MSG_MULTICAST, SVC_TEMPENTITY );
	trap_WriteByte( MSG_MULTICAST, TE_EXPLOSION );
	trap_WriteCoord( MSG_MULTICAST, self->s.v.origin[0] );
	trap_WriteCoord( MSG_MULTICAST, self->s.v.origin[1] );
	trap_WriteCoord( MSG_MULTICAST, self->s.v.origin[2] );
	trap_multicast( PASSVEC3( self->s.v.origin ), MULTICAST_PHS );
	dremove( self );
}

/*
================
W_FireRocket
================
*/
void W_FireRocket(  )
{
	if ( !tg_data.unlimit_ammo )
		self->s.v.currentammo = --( self->s.v.ammo_rockets );
	sound( self, CHAN_WEAPON, "weapons/sgun1.wav", 1, ATTN_NORM );
	KickPlayer( -2, self );
	newmis = spawn(  );
	g_globalvars.newmis = EDICT_TO_PROG( newmis );
	newmis->s.v.owner = EDICT_TO_PROG( self );
	newmis->s.v.movetype = MOVETYPE_FLYMISSILE;
	newmis->s.v.solid = SOLID_BBOX;

// set newmis speed     
	trap_makevectors( self->s.v.v_angle );
	VectorScale( g_globalvars.v_forward, 900, newmis->s.v.velocity );

	vectoangles( newmis->s.v.velocity, newmis->s.v.angles );

	newmis->s.v.classname = "rocket";
	newmis->s.v.touch = ( func_t ) T_MissileTouch;
	newmis->voided = 0;
// set newmis duration
	newmis->s.v.nextthink = g_globalvars.time + 5;
	newmis->s.v.think = ( func_t ) SUB_Remove;
	newmis->s.v.weapon = 6;
	setmodel( newmis, "progs/missile.mdl" );
	setsize( newmis, 0, 0, 0, 0, 0, 0 );
	setorigin( newmis, self->s.v.origin[0] + g_globalvars.v_forward[0] * 8,
		   self->s.v.origin[1] + g_globalvars.v_forward[1] * 8,
		   self->s.v.origin[2] + g_globalvars.v_forward[2] * 8 + 16 );
}

/*
===============================================================================
LIGHTNING
===============================================================================
*/
void LightningHit( gedict_t * from, float damage )
{
	trap_WriteByte( MSG_MULTICAST, SVC_TEMPENTITY );
	trap_WriteByte( MSG_MULTICAST, TE_LIGHTNINGBLOOD );
	trap_WriteCoord( MSG_MULTICAST, g_globalvars.trace_endpos[0] );
	trap_WriteCoord( MSG_MULTICAST, g_globalvars.trace_endpos[1] );
	trap_WriteCoord( MSG_MULTICAST, g_globalvars.trace_endpos[2] );
	trap_multicast( PASSVEC3( g_globalvars.trace_endpos ), MULTICAST_PVS );

	TF_T_Damage( PROG_TO_EDICT( g_globalvars.trace_ent ), from, from, damage, TF_TD_NOTTEAM, TF_TD_ELECTRICITY );
}

/*
=================
LightningDamage
=================
*/

void LightningDamage( vec3_t p1, vec3_t p2, gedict_t * from, float damage )
{
	gedict_t *e1;
	gedict_t *e2;
	vec3_t  f;

	VectorSubtract( p2, p1, f );	// f = p2 - p1;
	VectorNormalize( f );	// normalize (f);

	f[0] = 0 - f[1];
	f[1] = f[0];
	f[2] = 0;
	VectorScale( f, 16, f );	//f = f*16;

	e1 = e2 = world;
	traceline( PASSVEC3( p1 ), PASSVEC3( p2 ), false, self );

	if ( PROG_TO_EDICT( g_globalvars.trace_ent )->s.v.takedamage )
	{
		LightningHit( from, damage );
		if ( streq( self->s.v.classname, "player" ) )
		{
			if ( streq( other->s.v.classname, "player" ) )
				PROG_TO_EDICT( g_globalvars.trace_ent )->s.v.velocity[2] += 400;
		}
	}
	e1 = PROG_TO_EDICT( g_globalvars.trace_ent );

	traceline( p1[0] + f[0], p1[1] + f[1], p1[2] + f[2], p2[0] + f[0], p2[1] + f[1], p2[2] + f[2], false, self );
	if ( PROG_TO_EDICT( g_globalvars.trace_ent ) != e1 && PROG_TO_EDICT( g_globalvars.trace_ent )->s.v.takedamage )
	{
		LightningHit( from, damage );
	}
	e2 = PROG_TO_EDICT( g_globalvars.trace_ent );

	traceline( p1[0] - f[0], p1[1] - f[1], p1[2] - f[2], p2[0] - f[0], p2[1] - f[1], p2[2] - f[2], false, self );
	if ( PROG_TO_EDICT( g_globalvars.trace_ent ) != e1
	     && PROG_TO_EDICT( g_globalvars.trace_ent ) != e2
	     && PROG_TO_EDICT( g_globalvars.trace_ent )->s.v.takedamage )
	{
		LightningHit( from, damage );
	}
}

void W_FireLightning(  )
{
	vec3_t  org;
	float   cells;
	vec3_t  tmp;

	if ( self->s.v.ammo_cells < 1 )
	{
		self->last_weapon = self->current_weapon;
		self->last_weaponmode = self->weaponmode;
		self->current_weapon = W_BestWeapon(  );
		W_SetCurrentAmmo(  );
		W_PrintWeaponMessage(  );
		return;
	}
	if ( self->s.v.waterlevel > 1 )
	{
		cells = self->s.v.ammo_cells;
		self->s.v.ammo_cells = 0;
		W_SetCurrentAmmo(  );
		tf_data.deathmsg = DMSG_LIGHTNING;
		T_RadiusDamage( self, self, 35 * cells, world );
		return;
	}
	if ( self->t_width < g_globalvars.time )
	{
		sound( self, 1, "weapons/lhit.wav", 1, 1 );
		self->t_width = g_globalvars.time + 0.6;
	}
	KickPlayer( -2, self );
	if ( !tg_data.unlimit_ammo )
		self->s.v.currentammo = --( self->s.v.ammo_cells );

	VectorCopy( self->s.v.origin, org );	//org = self->s.v.origin + '0 0 16';
	org[2] += 16;
	traceline( PASSVEC3( org ), org[0] + g_globalvars.v_forward[0] * 600,
		   org[1] + g_globalvars.v_forward[1] * 600, org[2] + g_globalvars.v_forward[2] * 600, true, self );
	trap_WriteByte( MSG_MULTICAST, SVC_TEMPENTITY );
	trap_WriteByte( MSG_MULTICAST, TE_LIGHTNING2 );
	WriteEntity( MSG_MULTICAST, self );
	trap_WriteCoord( MSG_MULTICAST, org[0] );
	trap_WriteCoord( MSG_MULTICAST, org[1] );
	trap_WriteCoord( MSG_MULTICAST, org[2] );
	trap_WriteCoord( MSG_MULTICAST, g_globalvars.trace_endpos[0] );
	trap_WriteCoord( MSG_MULTICAST, g_globalvars.trace_endpos[1] );
	trap_WriteCoord( MSG_MULTICAST, g_globalvars.trace_endpos[2] );

	trap_multicast( PASSVEC3( org ), MULTICAST_PHS );

	VectorScale( g_globalvars.v_forward, 4, tmp );
	VectorAdd( g_globalvars.trace_endpos, tmp, tmp );
	LightningDamage( self->s.v.origin, tmp, self, 30 );
}

int CountPlayerPipebombs( gedict_t * player )
{
	gedict_t *old;
	int     num_pipes;

	num_pipes = 0;
	old = trap_find( world, FOFS( s.v.classname ), "pipebomb" );
	while ( old )
	{
		if ( old->s.v.owner == EDICT_TO_PROG( player ) )
			num_pipes++;
		old = trap_find( old, FOFS( s.v.classname ), "pipebomb" );
	}
	return num_pipes;
}

static int     num_team_pipebombs[5];

void ExplodeOldPipebomb( int tno, int extra )
{
	gedict_t *old;
	int     index;

	if ( tno )
	{
		index = num_team_pipebombs[tno];
		index = index - MAX_WORLD_PIPEBOMBS / number_of_teams;	//- 1 ;//extra pipepomb
		if ( extra )
			index = index - 1;
	} else
		index = num_team_pipebombs[0] - MAX_WORLD_PIPEBOMBS;
	old = trap_find( world, FOFS( s.v.classname ), "pipebomb" );
	while ( index > 0 )
	{
		if ( !old )
		{
			num_team_pipebombs[0] = 0;
			num_team_pipebombs[1] = 0;
			num_team_pipebombs[2] = 0;
			num_team_pipebombs[3] = 0;
			num_team_pipebombs[4] = 0;
			return;
		}
		if ( PROG_TO_EDICT( old->s.v.owner )->team_no == tno || !tno )
		{
			old->s.v.nextthink = g_globalvars.time + 0.5;
			index = index - 1;
		}
		old = trap_find( old, FOFS( s.v.classname ), "pipebomb" );
	}
}

void increment_team_pipebombs( int tno )
{
	if ( tno < 0 || tno > 4 )
		return;
	num_team_pipebombs[tno]++;
}

void decrement_team_pipebombs( int tno )
{
	if ( tno < 0 || tno > 4 )
		return;
	num_team_pipebombs[tno]--;

}

//=============================================================================

void GrenadeExplode(  )
{
	gedict_t *te;

	if ( self->voided )
	{
		return;
	}
	self->voided = 1;

	if ( streq( self->s.v.classname, "pipebomb" ) )
	{
		decrement_team_pipebombs( self->team_no );

		if ( !( ( int ) self->s.v.flags & FL_ONGROUND ) )
			self->s.v.weapon = DMSG_PIPEBOMB;	// TRAP
	}
	if ( PROG_TO_EDICT( self->s.v.owner )->has_disconnected != 1 )
	{
		tf_data.deathmsg = self->s.v.weapon;
		T_RadiusDamage( self, PROG_TO_EDICT( self->s.v.owner ), 120, world );
	}
	if ( self->no_active_nail_grens )
	{
		self->no_active_nail_grens = 0;
		PROG_TO_EDICT( self->s.v.owner )->no_active_nail_grens -= 1;
		te = trap_find( world, FOFS( s.v.classname ), "grenade" );
		while ( te )
		{
			if ( te->s.v.owner == self->s.v.owner && te->no_active_nail_grens )
				te->no_active_nail_grens -= 1;
			te = trap_find( te, FOFS( s.v.classname ), "grenade" );
		}
	}
	trap_WriteByte( MSG_MULTICAST, SVC_TEMPENTITY );
	trap_WriteByte( MSG_MULTICAST, TE_EXPLOSION );
	trap_WriteCoord( MSG_MULTICAST, self->s.v.origin[0] );
	trap_WriteCoord( MSG_MULTICAST, self->s.v.origin[1] );
	trap_WriteCoord( MSG_MULTICAST, self->s.v.origin[2] );
	trap_multicast( PASSVEC3( self->s.v.origin ), MULTICAST_PHS );
	BecomeExplosion(  );
}

void GrenadeTouch(  )
{
	if ( other == PROG_TO_EDICT( self->s.v.owner ) )
		return;		// don't explode on owner
	if ( other->s.v.takedamage == DAMAGE_AIM )
	{
		GrenadeExplode(  );
		return;
	}
	sound( self, CHAN_WEAPON, "weapons/bounce.wav", 1, ATTN_NORM );	// bounce sound
	if ( VectorCompareF( self->s.v.velocity, 0, 0, 0 ) )
		SetVector( self->s.v.avelocity, 0, 0, 0 );
}

/*
================
W_FireGrenade
================
*/
void W_FireGrenade(  )
{
	int     num_pipes;
	int     num_team_pipes;

	if ( !tg_data.unlimit_ammo  )
		self->s.v.currentammo = --( self->s.v.ammo_rockets );

	sound( self, 1, "weapons/grenade.wav", 1, 1 );
	KickPlayer( -2, self );
	newmis = spawn(  );
	g_globalvars.newmis = EDICT_TO_PROG( newmis );
	newmis->voided = 0;
	newmis->s.v.owner = EDICT_TO_PROG( self );
	newmis->s.v.movetype = MOVETYPE_BOUNCE;
	newmis->s.v.solid = SOLID_BBOX;
	if ( !self->weaponmode || ( tf_data.cb_prematch_time > g_globalvars.time ) )
	{
		newmis->s.v.weapon = 5;
		newmis->s.v.classname = "grenade";
		newmis->s.v.skin = 1;
		newmis->s.v.touch = ( func_t ) GrenadeTouch;
		newmis->s.v.nextthink = g_globalvars.time + 2.5;
	} else
	{
		num_pipes = CountPlayerPipebombs( self );
		if ( self->team_no )
		{
			increment_team_pipebombs( self->team_no );
			num_team_pipes = num_team_pipebombs[self->team_no];

			if ( num_pipes == 0  && tf_data.add_pipe == 1)
			{
				if ( num_team_pipes > MAX_WORLD_PIPEBOMBS / number_of_teams + 1 )
					ExplodeOldPipebomb( self->team_no, 1 );
			} else
			{
				if ( num_team_pipes > ( MAX_WORLD_PIPEBOMBS / number_of_teams ) )
					ExplodeOldPipebomb( self->team_no, 0 );
			}
		} else
		{
			increment_team_pipebombs( self->team_no );
			if ( num_pipes == 0 && tf_data.add_pipe == 1)
			{
				if ( num_team_pipebombs[0] > MAX_WORLD_PIPEBOMBS + 1 )
					ExplodeOldPipebomb( 0, 1 );
			} else
			{
				if ( num_team_pipebombs[0] > MAX_WORLD_PIPEBOMBS )
					ExplodeOldPipebomb( 0, 0 );
			}
		}
		newmis->s.v.classname = "pipebomb";
		newmis->s.v.skin = 2;
		newmis->s.v.touch = ( func_t ) PipebombTouch;
		newmis->s.v.nextthink = g_globalvars.time + 120;
		newmis->s.v.weapon = 11;
		newmis->team_no = self->team_no;
	}
	trap_makevectors( self->s.v.v_angle );
	if ( self->s.v.v_angle[0] )
	{
		newmis->s.v.velocity[0] =
		    g_globalvars.v_forward[0] * 600 + g_globalvars.v_up[0] * 200 +
		    crandom(  ) * g_globalvars.v_right[0] * 10 + crandom(  ) * g_globalvars.v_up[0] * 10;
		newmis->s.v.velocity[1] =
		    g_globalvars.v_forward[1] * 600 + g_globalvars.v_up[1] * 200 +
		    crandom(  ) * g_globalvars.v_right[1] * 10 + crandom(  ) * g_globalvars.v_up[1] * 10;
		newmis->s.v.velocity[2] =
		    g_globalvars.v_forward[2] * 600 + g_globalvars.v_up[2] * 200 +
		    crandom(  ) * g_globalvars.v_right[2] * 10 + crandom(  ) * g_globalvars.v_up[0] * 10;
	} else
	{
		aim( newmis->s.v.velocity );	// = aim(self, 10000);
		VectorScale( newmis->s.v.velocity, 600, newmis->s.v.velocity );	// * 600;
		newmis->s.v.velocity[2] = 200;
	}
	SetVector( newmis->s.v.avelocity, 300, 300, 300 );
	vectoangles( newmis->s.v.velocity, newmis->s.v.angles );
	newmis->s.v.think = ( func_t ) GrenadeExplode;
	if ( tf_data.birthday == 1 && g_random(  ) < 0.2 )
		setmodel( newmis, "progs/grenade3.mdl" );
	else
		setmodel( newmis, "progs/grenade2.mdl" );
	setsize( newmis, 0, 0, 0, 0, 0, 0 );
	setorigin( newmis, PASSVEC3( self->s.v.origin ) );
}


//=============================================================================

void    spike_touch(  );
void    superspike_touch(  );

/*
===============
launch_spike

Used for both the player and the traps
===============
*/
void launch_spike( vec3_t org, vec3_t dir )
{
	newmis = spawn(  );
	g_globalvars.newmis = EDICT_TO_PROG( newmis );
	newmis->voided = 0;
	newmis->s.v.owner = EDICT_TO_PROG( self );
	newmis->s.v.movetype = MOVETYPE_FLYMISSILE;
	newmis->s.v.solid = SOLID_BBOX;

	vectoangles( dir, newmis->s.v.angles );

	newmis->s.v.touch = ( func_t ) spike_touch;
	newmis->s.v.weapon = 3;
	newmis->s.v.classname = "spike";
	newmis->s.v.think = ( func_t ) SUB_Remove;
	newmis->s.v.nextthink = g_globalvars.time + 6;
	setmodel( newmis, "progs/spike.mdl" );
	setsize( newmis, 0, 0, 0, 0, 0, 0 );
	setorigin( newmis, PASSVEC3( org ) );
	VectorScale( dir, 1000, newmis->s.v.velocity );
}

void W_FireSuperSpikes(  )
{
	vec3_t  dir, tmp;

	sound( self, CHAN_WEAPON, "weapons/spike2.wav", 1, ATTN_NORM );
	if ( !tg_data.unlimit_ammo  )
		self->s.v.currentammo = self->s.v.ammo_nails = self->s.v.ammo_nails - 4;
	aim( dir );		//dir = aim (self, 1000);

	VectorCopy( self->s.v.origin, tmp );
	tmp[2] += 16;
	launch_spike( tmp, dir );
	newmis->s.v.touch = ( func_t ) superspike_touch;
	newmis->s.v.weapon = 4;
	setmodel( newmis, "progs/s_spike.mdl" );
	setsize( newmis, 0, 0, 0, 0, 0, 0 );
	KickPlayer( -2, self );
}

void W_FireSpikes( float ox )
{
	vec3_t  dir, tmp;

	trap_makevectors( self->s.v.v_angle );
	if ( self->s.v.ammo_nails >= 4 && self->current_weapon == WEAP_SUPER_NAILGUN )
	{
		W_FireSuperSpikes(  );
		return;
	}
	if ( self->s.v.ammo_nails < 1 )
	{
		self->last_weapon = self->current_weapon;
		self->last_weaponmode = self->weaponmode;
		self->current_weapon = W_BestWeapon(  );
		W_SetCurrentAmmo(  );
		W_PrintWeaponMessage(  );
		return;
	}
	sound( self, 1, "weapons/rocket1i.wav", 1, 1 );
	if ( self->s.v.ammo_nails == 1 )
	{
		if ( !tg_data.unlimit_ammo  )
			self->s.v.currentammo = self->s.v.ammo_nails = self->s.v.ammo_nails - 1;
	} else
	{
		if ( !tg_data.unlimit_ammo  )
			self->s.v.currentammo = self->s.v.ammo_nails = self->s.v.ammo_nails - 2;
	}
	aim( dir );		// dir = aim (self, 1000);
	VectorScale( g_globalvars.v_right, ox, tmp );
	VectorAdd( tmp, self->s.v.origin, tmp );
	tmp[2] += 16;
	launch_spike( tmp, dir );
	KickPlayer( -2, self );
}



void spike_touch(  )
{
	if ( self->voided )
	{
		return;
	}
	self->voided = 1;

	if ( other->s.v.solid == SOLID_TRIGGER )
		return;		// trigger field, do nothing

	if ( trap_pointcontents( PASSVEC3( self->s.v.origin ) ) == CONTENT_SKY )
	{
		dremove( self );
		return;
	}
// hit something that bleeds
	if ( other->s.v.takedamage )
	{
		spawn_touchblood( 9 );
		tf_data.deathmsg = self->s.v.weapon;
		if ( streq( PROG_TO_EDICT( self->s.v.owner )->s.v.classname, "grenade" ) )
			TF_T_Damage( other, self, PROG_TO_EDICT( PROG_TO_EDICT( self->s.v.owner )->s.v.owner ), 9, TF_TD_NOTTEAM, TF_TD_NAIL );
		else
			TF_T_Damage( other, self, PROG_TO_EDICT( self->s.v.owner ), 18, TF_TD_NOTTEAM, TF_TD_NAIL );
	} else
	{
		trap_WriteByte( MSG_MULTICAST, SVC_TEMPENTITY );
		if ( !strcmp( self->s.v.classname, "wizspike" ) )
			trap_WriteByte( MSG_MULTICAST, TE_WIZSPIKE );
		else if ( !strcmp( self->s.v.classname, "knightspike" ) )
			trap_WriteByte( MSG_MULTICAST, TE_KNIGHTSPIKE );
		else
			trap_WriteByte( MSG_MULTICAST, TE_SPIKE );
		trap_WriteCoord( MSG_MULTICAST, self->s.v.origin[0] );
		trap_WriteCoord( MSG_MULTICAST, self->s.v.origin[1] );
		trap_WriteCoord( MSG_MULTICAST, self->s.v.origin[2] );
		trap_multicast( PASSVEC3( self->s.v.origin ), MULTICAST_PHS );
	}
	dremove( self );
}

void superspike_touch(  )
{
	float   ndmg;

	if ( self->voided )
	{
		return;
	}
	self->voided = 1;


	if ( other->s.v.solid == SOLID_TRIGGER )
		return;		// trigger field, do nothing

	if ( trap_pointcontents( PASSVEC3( self->s.v.origin ) ) == CONTENT_SKY )
	{
		ent_remove( self );
		return;
	}
// hit something that bleeds
	if ( other->s.v.takedamage )
	{
		spawn_touchblood( 18 );
		tf_data.deathmsg = self->s.v.weapon;
		if ( tf_data.deathmsg == DMSG_GREN_NAIL )
			ndmg = 40;
		else
			ndmg = 26;
		if ( streq( PROG_TO_EDICT( self->s.v.owner )->s.v.classname, "grenade" ) )
			TF_T_Damage( other, self, PROG_TO_EDICT( PROG_TO_EDICT( self->s.v.owner )->s.v.owner ), ndmg, TF_TD_NOTTEAM, TF_TD_NAIL );
		else
			TF_T_Damage( other, self, PROG_TO_EDICT( self->s.v.owner ), ndmg, TF_TD_NOTTEAM, TF_TD_NAIL );
	} else
	{
		trap_WriteByte( MSG_MULTICAST, SVC_TEMPENTITY );
		trap_WriteByte( MSG_MULTICAST, TE_SUPERSPIKE );
		trap_WriteCoord( MSG_MULTICAST, self->s.v.origin[0] );
		trap_WriteCoord( MSG_MULTICAST, self->s.v.origin[1] );
		trap_WriteCoord( MSG_MULTICAST, self->s.v.origin[2] );
		trap_multicast( PASSVEC3( self->s.v.origin ), MULTICAST_PHS );
	}
	dremove( self );
}

/*
===============================================================================

PLAYER WEAPON USE

===============================================================================
*/

/*void W_SetCurrentAmmo_old(  )
{
	int     items;

	if ( self->s.v.health <= 0 || !self->current_weapon )
		return;
	if ( self->current_weapon == WEAP_ASSAULT_CANNON && ( self->tfstate & TFSTATE_AIMING ) )
		return;
	player_run(  );
	items = self->s.v.items;
	items -= items & ( IT_SHELLS | IT_NAILS | IT_ROCKETS | IT_CELLS );
	self->s.v.weapon = 0;
	switch ( self->current_weapon )
	{
	case WEAP_AXE:
		self->s.v.currentammo = 0;
		if ( self->playerclass == PC_SPY )
		{
			if ( !self->weaponmode )
				self->s.v.weaponmodel = "progs/v_knife.mdl";
			else
				self->s.v.weaponmodel = "progs/v_knife2.mdl";
		} else
			self->s.v.weaponmodel = "progs/v_axe.mdl";
		self->s.v.weaponframe = 0;

		break;
	case WEAP_HOOK:
		self->s.v.currentammo = 0;
		self->s.v.weaponmodel = "progs/v_grap.mdl";
		self->s.v.weaponframe = 0;
		break;
	case WEAP_SPANNER:
		self->s.v.currentammo = self->s.v.ammo_cells;
		self->s.v.weaponmodel = "progs/v_span.mdl";
		self->s.v.weaponframe = 0;
		break;
	case WEAP_SHOTGUN:
		self->s.v.currentammo = self->s.v.ammo_shells;
		items |= 2048;
		if ( !( self->tfstate & TFSTATE_RELOADING ) )
		{
			self->s.v.weaponmodel = "progs/v_shot.mdl";
			self->s.v.weaponframe = 0;
		}
		items |= 256;
		self->s.v.weapon = 1;
		break;
	case WEAP_SUPER_SHOTGUN:
		self->s.v.currentammo = self->s.v.ammo_shells;
		if ( !( self->tfstate & TFSTATE_RELOADING ) )
		{
			self->s.v.weaponmodel = "progs/v_shot2.mdl";
			self->s.v.weaponframe = 0;
		}
		items |= 256;
		self->s.v.weapon = 2;
		break;
	case WEAP_NAILGUN:
		self->s.v.currentammo = self->s.v.ammo_nails;
		if ( !( self->tfstate & TFSTATE_RELOADING ) )
		{
			self->s.v.weaponmodel = "progs/v_nail.mdl";
			self->s.v.weaponframe = 0;
		}
		items |= 512;
		self->s.v.weapon = 4;
		break;
	case WEAP_SUPER_NAILGUN:
		self->s.v.currentammo = self->s.v.ammo_nails;
		if ( !( self->tfstate & TFSTATE_RELOADING ) )
		{
			self->s.v.weaponmodel = "progs/v_nail2.mdl";
			self->s.v.weaponframe = 0;
		}
		items |= 512;
		self->s.v.weapon = 8;
		break;
	case WEAP_GRENADE_LAUNCHER:
		self->s.v.currentammo = self->s.v.ammo_rockets;
		if ( !( self->tfstate & TFSTATE_RELOADING ) )
		{
			if ( self->weaponmode == GL_NORMAL )
				self->s.v.weaponmodel = "progs/v_rock.mdl";
			else
				self->s.v.weaponmodel = "progs/v_pipe.mdl";
			self->s.v.weaponframe = 0;
		}
		self->s.v.weapon = 16;
		items |= 1024;
		break;
	case WEAP_ROCKET_LAUNCHER:
		self->s.v.currentammo = self->s.v.ammo_rockets;
		if ( !( self->tfstate & TFSTATE_RELOADING ) )
		{
			self->s.v.weaponmodel = "progs/v_rock2.mdl";
			self->s.v.weaponframe = 0;
		}
		items |= 1024;
		self->s.v.weapon = 32;
		break;
	case WEAP_LIGHTNING:
		self->s.v.currentammo = self->s.v.ammo_cells;
		if ( !( self->tfstate & TFSTATE_RELOADING ) )
		{
			self->s.v.weaponmodel = "progs/v_light.mdl";
			self->s.v.weaponframe = 0;
		}
		items |= 2048;
		self->s.v.weapon = 64;
		break;
	case WEAP_SNIPER_RIFLE:
		self->s.v.currentammo = self->s.v.ammo_shells;
		if ( !( self->tfstate & TFSTATE_RELOADING ) )
		{
			self->s.v.weaponmodel = "progs/v_srifle.mdl";
			self->s.v.weaponframe = 0;
		}
		items |= 256;
		self->s.v.weapon = 1;
		break;
	case WEAP_AUTO_RIFLE:
		self->s.v.currentammo = self->s.v.ammo_shells;
		if ( !( self->tfstate & TFSTATE_RELOADING ) )
		{
			self->s.v.weaponmodel = "progs/v_srifle.mdl";
			self->s.v.weaponframe = 0;
		}
		items |= 256;
		self->s.v.weapon = 2;
		break;
	case WEAP_ASSAULT_CANNON:
		self->s.v.currentammo = self->s.v.ammo_shells;
		if ( !( self->tfstate & TFSTATE_RELOADING ) )
		{
			self->s.v.weaponmodel = "progs/v_asscan.mdl";
			self->s.v.weaponframe = 0;
		}
		items |= 256;
		self->s.v.weapon = 32;
		break;
	case WEAP_FLAMETHROWER:
		self->s.v.currentammo = self->s.v.ammo_cells;
		if ( !( self->tfstate & TFSTATE_RELOADING ) )
		{
			self->s.v.weaponmodel = "progs/v_flame.mdl";
			self->s.v.weaponframe = 0;
		}
		items |= 2048;
		self->s.v.weapon = 16;
		break;
	case WEAP_INCENDIARY:
		self->s.v.currentammo = self->s.v.ammo_rockets;
		if ( !( self->tfstate & TFSTATE_RELOADING ) )
		{
			self->s.v.weaponmodel = "progs/v_rock2.mdl";
			self->s.v.weaponframe = 0;
		}
		items |= 1024;
		self->s.v.weapon = 32;
		break;
	case WEAP_MEDIKIT:
//                 self->s.v.currentammo = 0;
		self->s.v.currentammo = self->ammo_medikit;
		self->s.v.weaponmodel = "progs/v_medi.mdl";
		self->s.v.weaponframe = 0;
		break;
	case WEAP_BIOWEAPON:
		self->s.v.currentammo = 0;
		self->s.v.weaponmodel = "progs/v_bio.mdl";
		self->s.v.weaponframe = 0;
		break;
	case WEAP_TRANQ:
		self->s.v.currentammo = self->s.v.ammo_shells;
		if ( !( self->tfstate & TFSTATE_RELOADING ) )
		{
			self->s.v.weaponmodel = "progs/v_tgun.mdl";
			self->s.v.weaponframe = 0;
		}
		items |= 256;
		self->s.v.weapon = 1;
		break;
	case WEAP_LASER:
		self->s.v.currentammo = self->s.v.ammo_nails;
		if ( !( self->tfstate & TFSTATE_RELOADING ) )
		{
			self->s.v.weaponmodel = "progs/v_rail.mdl";
			self->s.v.weaponframe = 0;
		}
		items |= 512;
		self->s.v.weapon = 1;
		break;
	default:
		self->s.v.currentammo = 0;
		self->s.v.weaponmodel = "";
		self->s.v.weaponframe = 0;
		break;
	}
	self->s.v.items = items;
}*/
void W_SetCurrentAmmo(  )
{
	int     items;
    const weapon_info_t* wi;

	if ( self->s.v.health <= 0 || !self->current_weapon )
		return;
	if ( self->current_weapon == WEAP_ASSAULT_CANNON && ( self->tfstate & TFSTATE_AIMING ) )
		return;
	player_run(  );
	items = self->s.v.items;
	items -= items & ( IT_SHELLS | IT_NAILS | IT_ROCKETS | IT_CELLS );
	self->s.v.weapon = 0;
    if( self->current_weapon == WEAP_AXE && self->playerclass == PC_SPY ){
        self->s.v.currentammo = 0;
        if ( !self->weaponmode )
            self->s.v.weaponmodel = "progs/v_knife.mdl";
        else
            self->s.v.weaponmodel = "progs/v_knife2.mdl";
        self->s.v.weaponframe = 0;
        return;
    }else{
        wi = WEAPON_BY_BIT( self->current_weapon );
        if( wi->w == self->current_weapon ){
            self->s.v.currentammo = 0;

            if ( !( self->tfstate & TFSTATE_RELOADING ) ){
                if( wi->have_mode && self->weaponmode == 1 ){
                    self->s.v.weaponmodel = wi->model_mode;
                }else{
                    self->s.v.weaponmodel = wi->model;
                }
            }
            if( wi->ammo_ofs){
                if(wi->ammo_type == F_INT ){
                    self->s.v.currentammo = *(int*)((byte*)self + wi->ammo_ofs );
                }else{
                    self->s.v.currentammo = *(float*)((byte*)self + wi->ammo_ofs );
                }
            }
            items |= wi->bit_item;
            items |= wi->bit_ammo;
            self->s.v.items = items;
            self->s.v.weaponframe = 0;
        }else{
            self->s.v.currentammo = 0;
            self->s.v.weaponmodel = "";
            self->s.v.weaponframe = 0;
        }
    }
}

const int best_weapon_list[] = { WEAP_LIGHTNING, WEAP_ASSAULT_CANNON, WEAP_FLAMETHROWER,
    WEAP_SUPER_NAILGUN, WEAP_SUPER_SHOTGUN, WEAP_LASER, WEAP_NAILGUN,
    WEAP_SHOTGUN, WEAP_TRANQ, WEAP_MEDIKIT, WEAP_SPANNER, WEAP_AXE,
    0,
};
int W_BestWeapon(  )
{
	int     it;
    const weapon_info_t * wi;
    const int* w = &best_weapon_list[0];

	it = self->weapons_carried;
    do{
        wi = WEAPON_BY_BIT( *w );

        if( !( wi->w & it ) ) continue;
        
        if( wi->w == WEAP_ASSAULT_CANNON && 
                (self->s.v.ammo_cells >= 6 && self->s.v.ammo_shells > self->assault_min_shells ) ){
			return WEAP_ASSAULT_CANNON;
        }
        if( wi->ammo_shells && self->s.v.ammo_shells < wi->ammo_shells ) continue;
        if( wi->ammo_nails && self->s.v.ammo_nails < wi->ammo_nails ) continue;
        if( wi->ammo_rockets && self->s.v.ammo_rockets < wi->ammo_rockets ) continue;
        if( wi->ammo_cells && self->s.v.ammo_cells < wi->ammo_cells ) continue;
        return *w;
    } while( *(++w) );
	return 0;
}

int W_CheckNoAmmo(  )
{
    int it;
    it = self->current_weapon;
    if( it & (WEAP_MEDIKIT | WEAP_SPANNER| WEAP_AXE | WEAP_HOOK ) )
        return 1;
    if ( it == WEAP_INCENDIARY && ( self->s.v.currentammo >= 3 ))
        return 1;
    if( it == WEAP_SNIPER_RIFLE && ( self->s.v.currentammo >= tf_data.snip_ammo ))
        return 1;
    if ( self->s.v.currentammo > 0 )
        return 1;

    self->current_weapon = W_BestWeapon();
    W_SetCurrentAmmo(  );
    W_PrintWeaponMessage(  );
    return 0;
}

void W_Reload_shotgun(  )
{
	gedict_t *owner = PROG_TO_EDICT( self->s.v.owner );

	owner->tfstate -= ( owner->tfstate & TFSTATE_RELOADING );
	owner->s.v.weaponmodel = "progs/v_shot.mdl";
	G_sprint( owner, 0, "finished reloading\n" );
	dremove( self );
	owner->StatusRefreshTime = g_globalvars.time + 0.1;
}

void W_Reload_super_shotgun(  )
{
	gedict_t *owner = PROG_TO_EDICT( self->s.v.owner );

	owner->tfstate -= ( owner->tfstate & TFSTATE_RELOADING );
	owner->s.v.weaponmodel = "progs/v_shot2.mdl";
	G_sprint( owner, 0, "finished reloading\n" );
	dremove( self );
	owner->StatusRefreshTime = g_globalvars.time + 0.1;
}

void W_Reload_grenade_launcher(  )
{
	gedict_t *owner = PROG_TO_EDICT( self->s.v.owner );

	owner->tfstate -= ( owner->tfstate & TFSTATE_RELOADING );
	if ( owner->weaponmode == GL_NORMAL )
		owner->s.v.weaponmodel = "progs/v_rock.mdl";
	else
		owner->s.v.weaponmodel = "progs/v_pipe.mdl";
	G_sprint( owner, 0, "finished reloading\n" );
	dremove( self );
	owner->StatusRefreshTime = g_globalvars.time + 0.1;
}

void W_Reload_rocket_launcher(  )
{
	gedict_t *owner = PROG_TO_EDICT( self->s.v.owner );

	owner->tfstate -= ( owner->tfstate & TFSTATE_RELOADING );
	owner->s.v.weaponmodel = "progs/v_rock2.mdl";
	G_sprint( owner, 0, "finished reloading\n" );
	dremove( self );
	owner->StatusRefreshTime = g_globalvars.time + 0.1;
}

int _weapon_reload( void (* f)(), int tm )
{
    gedict_t *tWeapon;
    G_sprint( self, 2, "reloading...\n" );
    self->tfstate = self->tfstate | TFSTATE_RELOADING;
    tWeapon = spawn(  );
    tWeapon->s.v.owner = EDICT_TO_PROG( self );
    tWeapon->s.v.classname = "timer";
    tWeapon->s.v.nextthink = g_globalvars.time + tm;
    tWeapon->s.v.think = (func_t) f;
    self->s.v.weaponmodel = "";
    self->s.v.weaponframe = 0;
    return 1;
}

float CheckForReload(  )
{
 	if( tg_data.disable_reload )
	{
		self->reload_shotgun = 0;
		self->reload_super_shotgun = 0;
		self->reload_grenade_launcher = 0;
		self->reload_rocket_launcher = 0;
		return 0;
	}

	switch ( self->current_weapon )
	{
	case WEAP_SHOTGUN:
		if ( self->reload_shotgun >= 8 && self->s.v.ammo_shells > 0 )
		{
			self->reload_shotgun = 0;
			if ( self->s.v.ammo_shells < 8 )
				self->reload_shotgun = 8 - self->s.v.ammo_shells;
            return _weapon_reload( W_Reload_shotgun, 2 );
		}
		break;
	case WEAP_SUPER_SHOTGUN:
		if ( self->reload_super_shotgun > 16 )
			self->reload_super_shotgun = 16;
		if ( self->reload_super_shotgun >= 16 && self->s.v.ammo_shells > 0 )
		{
			self->reload_super_shotgun = 0;
			if ( self->s.v.ammo_shells < 16 )
				self->reload_super_shotgun = 16 - self->s.v.ammo_shells;
            return _weapon_reload( W_Reload_super_shotgun, 2 );
		}
		break;
	case WEAP_GRENADE_LAUNCHER:
		if ( self->reload_grenade_launcher >= 6 && self->s.v.ammo_rockets > 0 )
		{
			self->reload_grenade_launcher = 0;
			if ( self->s.v.ammo_rockets < 6 )
				self->reload_grenade_launcher = 6 - self->s.v.ammo_rockets;
            return _weapon_reload( W_Reload_grenade_launcher, 4 );
		}
		break;
	case WEAP_ROCKET_LAUNCHER:
		if ( self->reload_rocket_launcher >= 4 && self->s.v.ammo_rockets > 0 )
		{
			self->reload_rocket_launcher = 0;
			if ( self->s.v.ammo_rockets < 4 )
				self->reload_rocket_launcher = 4 - self->s.v.ammo_rockets;
            return _weapon_reload( W_Reload_rocket_launcher, 5 );
		}
		break;
	}
	return 0;
}

void W_Attack(  )
{

    if ( !W_CheckNoAmmo(  ) )
        return;
    if ( self->has_disconnected == 1 )
        return;
    if ( self->tfstate & TFSTATE_RELOADING )
        return;
    if ( self->is_undercover || self->undercover_team || self->undercover_skin )
        Spy_RemoveDisguise( self );
    trap_makevectors( self->s.v.v_angle );	// calculate forward angle for velocity
    self->show_hostile = g_globalvars.time + 1;	// wake monsters up

    switch ( self->current_weapon )
    {
        case WEAP_AXE:
            Attack_Finished( 0.5 );
            sound( self, 1, "weapons/ax1.wav", 1, 1 );
            switch ( ( int ) ( g_random(  ) * 4 ) )
            {
                case 0:
                    player_naxe(119,1, W_FireAxe);
                    break;
                case 1:
                    player_naxe(125,5, W_FireAxe);
                    break;
                case 2:
                    player_naxe(131,1, W_FireAxe);
                    break;
                default:
                    player_naxe(137,5, W_FireAxe);
                    break;
            }
            break;
        case WEAP_SPANNER:
            Attack_Finished( 0.5 );
            sound( self, 1, "weapons/ax1.wav", 1, 1 );
            player_naxe(119,1, W_FireSpanner);
            break;
        case WEAP_HOOK:
            if ( !self->hook_out )
                player_chain1(  );
            Attack_Finished( 0.1 );
            break;
        case WEAP_SHOTGUN:
            if ( CheckForReload(  ) == 1 )
                return;
            player_shot(113);
            W_FireShotgun(  );
            self->reload_shotgun += 1;
            self->StatusRefreshTime = g_globalvars.time + 0.1;
            CheckForReload(  );
            Attack_Finished( 0.5 );
            break;
        case WEAP_SUPER_SHOTGUN:
            if ( CheckForReload(  ) == 1 )
                return;
            player_shot(113);
            W_FireSuperShotgun(  );
            self->reload_super_shotgun += 2;
            self->StatusRefreshTime = g_globalvars.time + 0.1;
            CheckForReload(  );
            Attack_Finished( 0.7 );
            break;
        case WEAP_NAILGUN:
        case WEAP_SUPER_NAILGUN:
            player_nail1(  );
            break;
        case WEAP_GRENADE_LAUNCHER:
            if ( CheckForReload(  ) == 1 )
                return;
            player_shot(107);
            W_FireGrenade(  );
            self->reload_grenade_launcher += 1;
            self->StatusRefreshTime = g_globalvars.time + 0.1;
            CheckForReload(  );
            Attack_Finished( 0.6 );
            break;
        case WEAP_ROCKET_LAUNCHER:
            if ( CheckForReload(  ) == 1 )
                return;
            player_shot(107);
            W_FireRocket(  );
            self->reload_rocket_launcher += 1;
            self->StatusRefreshTime = g_globalvars.time + 0.1;
            CheckForReload(  );
            Attack_Finished( 0.8 );
            break;
        case WEAP_LIGHTNING:
            player_light1(  );
            Attack_Finished( 0.1 );
            sound( self, 0, "weapons/lstart.wav", 1, 1 );
            break;
        case WEAP_SNIPER_RIFLE:
            if ( ( ( int ) self->s.v.flags & FL_ONGROUND ) || self->hook_out )
            {
                player_shot(113);
                W_FireSniperRifle(  );
                self->allow_snip_time = g_globalvars.time + tf_data.snip_time;
                Attack_Finished( 1.5 );
            }
            break;
        case WEAP_AUTO_RIFLE:
            player_autorifle1(  );
            W_FireAutoRifle(  );
            Attack_Finished( 0.1 );
            break;
        case WEAP_ASSAULT_CANNON:
            if ( self->s.v.ammo_cells < 4 )
                G_sprint( self, 1, "Insufficient cells to power up the Assault Cannon.\n" );
            else
            {
                if( !tg_data.unlimit_ammo )
                    self->s.v.ammo_cells -= 4;

                self->heat = 1;
                self->tfstate |= TFSTATE_AIMING;
                TeamFortress_SetSpeed( self );
                player_assaultcannonup1(  );
            }
            break;
        case WEAP_FLAMETHROWER:
            player_shot(113);
            W_FireFlame(  );
            if ( self->s.v.waterlevel > 2 )
                Attack_Finished( 1 );
            else
            {
                Attack_Finished( 0.15 );
            }
            break;
        case WEAP_INCENDIARY:
            player_shot(107);
            W_FireIncendiaryCannon(  );
            Attack_Finished( 1.2 );
            break;
        case WEAP_MEDIKIT:
            sound( self, 1, "weapons/ax1.wav", 1, 1 );
            switch ( ( int ) ( g_random(  ) * 4 ) )
            {
                case 0:
                    player_naxe(119,1, W_FireMedikit);
                    break;
                case 1:
                    player_naxe(125,5, W_FireMedikit);
                    break;
                case 2:
                    player_naxe(131,1, W_FireMedikit);
                    break;
                default:
                    player_naxe(137,5, W_FireMedikit);
                    break;
            }
            Attack_Finished( 0.5 );
            break;
        case WEAP_BIOWEAPON:
            sound( self, 1, "weapons/ax1.wav", 1, 1 );
            switch ( ( int ) ( g_random(  ) * 4 ) )
            {
                case 0:
                    player_naxe(119,1, W_FireBioweapon);
                    break;
                case 1:
                    player_naxe(125,5, W_FireBioweapon);
                    break;
                case 2:
                    player_naxe(131,1, W_FireBioweapon);
                    break;
                default:
                    player_naxe(137,5, W_FireBioweapon);
                    break;
            }
            Attack_Finished( 0.5 );
            break;
        case WEAP_TRANQ:
            sound( self, 1, "weapons/dartgun.wav", 1, 1 );
            player_shot(113);
            W_FireTranq(  );
            Attack_Finished( 1.5 );
            break;
        case WEAP_LASER:
            sound( self, 1, "weapons/railgun.wav", 1, 1 );
            player_shot(113);
            W_FireLaser(  );
            Attack_Finished( 0.4 );
            break;
    }
}

void W_PrintWeaponMessage(  )
{
	switch ( self->current_weapon )
	{
	case WEAP_AXE:
		if ( tf_data.allow_hook )
			if ( self->playerclass != PC_SPY )
				G_sprint( self, 1, "Axe selected\n" );
		break;
	case WEAP_HOOK:
		G_sprint( self, 1, "grappling hook selected\n" );
		break;
	case WEAP_GRENADE_LAUNCHER:
		if ( self->weaponmode == GL_NORMAL )
			G_sprint( self, 1, "Normal grenade mode\n" );
		else
		{
			if ( tf_data.cb_prematch_time > g_globalvars.time )
				G_sprint( self, 1, "Pipebomb mode not available in PreMatch\n" );
			else
			{
				if ( self->weaponmode == GL_PIPEBOMB )
					G_sprint( self, 1, "Pipebomb mode\n" );
			}
		}
		break;
	case WEAP_SNIPER_RIFLE:
		G_sprint( self, 1, "Sniper rifle ready\n" );
		break;
	case WEAP_AUTO_RIFLE:
		G_sprint( self, 1, "rifle on fully auto\n" );
		break;
	case WEAP_TRANQ:
		G_sprint( self, 1, "Tranquiliser gun selected\n" );
		break;
	case WEAP_BIOWEAPON:
		G_sprint( self, 1, "BioWeapon readied\n" );
		break;
	case WEAP_MEDIKIT:
		G_sprint( self, 1, "Medikit/Bioweapon readied\n" );
		break;
	}
}
int W_CanUseWeapon( int weapon )
    // 1 no ammo
    // 2 no ammo assault
    // 3 no weapon
    // 0 ok 
{
    if( !( weapon & self->weapons_carried )) return 3;
    switch( weapon )
    {
        case WEAP_HOOK:
            if ( !tf_data.allow_hook ) return 0;
            if ( self->hook_out )
                Reset_Grapple( self->hook );
            break;
        case WEAP_SNIPER_RIFLE: 
            if ( self->s.v.ammo_shells < tf_data.snip_ammo ) return 1;
            break;
        case WEAP_SHOTGUN://   128
        case WEAP_TRANQ://    262144
        case WEAP_AUTO_RIFLE://   64
            if ( self->s.v.ammo_shells < 1 ) return 1;
            break;
        case WEAP_SUPER_SHOTGUN://  256
            if ( self->s.v.ammo_shells < 2 ) return 1;
            break;
        case WEAP_NAILGUN://   512
        case WEAP_LASER://    524288
            if ( self->s.v.ammo_nails < 1 ) return 1;
            break;
        case WEAP_SUPER_NAILGUN://  1024
            if ( self->s.v.ammo_nails < 4 ) return 1;
            break;
        case WEAP_GRENADE_LAUNCHER:// 2048
        case WEAP_ROCKET_LAUNCHER:// 8192
            if ( self->s.v.ammo_rockets < 1 ) return 1;
            break;
        case WEAP_INCENDIARY://   16384
            if ( self->s.v.ammo_rockets < 3 ) return 1;
            break;
        case WEAP_FLAMETHROWER://  4096
        case WEAP_LIGHTNING://   65536
            if ( self->s.v.ammo_cells < 1 ) return 1;
            break;
        case WEAP_ASSAULT_CANNON://  32768
            if ( self->s.v.ammo_shells < self->assault_min_shells ) return 1;
            if ( self->s.v.ammo_cells < 4 ) return 2;
            break;
        default:
            break;
    };
    return 0;
}

struct w_impulse_s{
    int impulse;
    int mask;
    int weapons[10];
};

static struct w_impulse_s w_impulses[]={
    {1, 0, { WEAP_SPANNER, /*WEAP_BIOWEAPON, */WEAP_MEDIKIT, WEAP_HOOK, WEAP_AXE, 0 }},
    {2, 0, { WEAP_SNIPER_RIFLE, WEAP_SHOTGUN, WEAP_TRANQ, WEAP_LASER, 0 }},
    {3, 0, { WEAP_AUTO_RIFLE, WEAP_SUPER_SHOTGUN, 0}},
    {4, 0, { WEAP_NAILGUN, 0}},
    {5, 0, { WEAP_SUPER_NAILGUN, 0}},
    {6, 0, { WEAP_FLAMETHROWER, WEAP_GRENADE_LAUNCHER, 0}},
    {7, 0, { WEAP_INCENDIARY, WEAP_ROCKET_LAUNCHER, WEAP_ASSAULT_CANNON, WEAP_GRENADE_LAUNCHER, 0}},
    {8, 0, { WEAP_LIGHTNING, 0 }},
    {TF_MEDIKIT, 0, { WEAP_MEDIKIT , 0}},
    {AXE_IMP, 0, { WEAP_SPANNER, /*WEAP_BIOWEAPON, */WEAP_MEDIKIT, WEAP_HOOK, WEAP_AXE, 0 }},
    {TF_HOOK_IMP1, 0, { WEAP_HOOK, 0 }},
    {TF_HOOK_IMP2, 0, { WEAP_HOOK, 0 }},
    {0},
};

void W_SetWeapon( int fl, int wm, int am )
{
	int it = self->weapons_carried;
	if ( !fl ||  !( it & fl ) )
	{
		G_sprint( self, 2, "no weapon.\n" );
		return;
	}
	if ( am == 1 )
	{
		G_sprint( self, 2, "not enough ammo.\n" );
		return;
	}
	if ( am == 2 )
	{
		G_sprint( self, 2, "not enough cells to power assault cannon.\n" );
		return;
	}
	self->last_weaponmode = self->weaponmode;
	self->last_weapon = self->current_weapon;
	self->current_weapon = fl;
	self->weaponmode = wm;
	W_SetCurrentAmmo(  );
	W_PrintWeaponMessage(  );
	self->StatusRefreshTime = g_globalvars.time + 0.1;
}

void W_ChangeWeapon(  )
{
	int     it, am, fl, wm = 0, have_weapon, usable;
    int *weapons;
    struct w_impulse_s* wi = &w_impulses[0];

	if ( self->tfstate & TFSTATE_RELOADING )
		return;
	it = self->weapons_carried;
	fl = self->current_weapon;
	am = 0;
	usable = 0;
	have_weapon = 1;
    if ( self->s.v.impulse == TF_WEAPLAST )
    {
        self->s.v.impulse = 0;
        if( !self->last_weapon ) return;
        am = W_CanUseWeapon( self->last_weapon );
        W_SetWeapon( self->last_weapon, self->last_weaponmode, am );
        return; 
    }else{
        while( wi->impulse && wi->impulse != self->s.v.impulse ) wi++;
    }
    self->s.v.impulse = 0;
    if( !wi->impulse){ return; }

    if( wi->mask == 0 ){//init mask
        int* w = wi->weapons;
        while(*w){
            wi->mask |= *w++;
        };
    }
    if( wi->mask & it ){
        int* w = wi->weapons;
        while( *w && *w++ != fl );
        if( !*w) w = wi->weapons;
        fl = *w;
        do{
            am = W_CanUseWeapon(*w);
            if(!am){
                fl = *w; 
                break;
            }
            w++;
            if( !*w) w = wi->weapons;
            if( *w == fl ){ return;}
        }while(*w != fl );
        if( am == 3 || fl == 0 ){
            have_weapon = 0;
            am = 0;
        }

    }else{
        have_weapon = 0;
    }
    if( wi->impulse == 7 && fl == WEAP_GRENADE_LAUNCHER ) wm = 1;
    
    W_SetWeapon( fl, wm, am );
}

struct weapon_s{
    int w;
    int mode;
};

const struct weapon_s weapon_cycle[] = { 
    { WEAP_AXE             , 0},
    { WEAP_SPANNER         , 0},
    { WEAP_SHOTGUN         , 0},
    { WEAP_LASER           , 0},
    { WEAP_TRANQ           , 0},
    { WEAP_SNIPER_RIFLE    , 0},
    { WEAP_AUTO_RIFLE      , 0},
    { WEAP_SUPER_SHOTGUN   , 0},
    { WEAP_NAILGUN         , 0},
    { WEAP_SUPER_NAILGUN   , 0},
    { WEAP_GRENADE_LAUNCHER, 0},
    { WEAP_GRENADE_LAUNCHER, 1},
    { WEAP_ROCKET_LAUNCHER , 0},
    { WEAP_LIGHTNING       , 0},
    { WEAP_FLAMETHROWER    , 0},
    { WEAP_INCENDIARY      , 0},
    { WEAP_ASSAULT_CANNON  , 0},
    { WEAP_HOOK            , 0},
    { WEAP_MEDIKIT         , 0},
    { WEAP_BIOWEAPON       , 0},
    { WEAP_MEDIKIT         , 0},
    { 0,0                    }
};                          
const struct weapon_s weapon_cycle_rev[] = { 
    { WEAP_MEDIKIT         , 0},
    { WEAP_BIOWEAPON       , 0},
    { WEAP_MEDIKIT         , 0},
    { WEAP_HOOK            , 0},
    { WEAP_ASSAULT_CANNON  , 0},
    { WEAP_INCENDIARY      , 0},
    { WEAP_FLAMETHROWER    , 0},
    { WEAP_LIGHTNING       , 0},
    { WEAP_ROCKET_LAUNCHER , 0},
    { WEAP_GRENADE_LAUNCHER, 1},
    { WEAP_GRENADE_LAUNCHER, 0},
    { WEAP_SUPER_NAILGUN   , 0},
    { WEAP_NAILGUN         , 0},
    { WEAP_SUPER_SHOTGUN   , 0},
    { WEAP_AUTO_RIFLE      , 0},
    { WEAP_SNIPER_RIFLE    , 0},
    { WEAP_TRANQ           , 0},
    { WEAP_LASER           , 0},
    { WEAP_SHOTGUN         , 0},
    { WEAP_SPANNER         , 0},
    { WEAP_AXE             , 0},
    { 0,0                    }
};                          
void CycleWeaponCommand( int prev )
{
	float   it;
	int   lw;
    int wm;
    int am;
    const struct weapon_s *wp;

    wp = !prev ?&weapon_cycle[0] : &weapon_cycle_rev[0];

	if ( self->s.v.weaponmodel[0] == 0 || !self->current_weapon )
		return;
	if ( self->tfstate & TFSTATE_RELOADING )
		return;
	it = self->weapons_carried;
	self->s.v.impulse = 0;
	lw = self->current_weapon;
    wm = ( lw == WEAP_GRENADE_LAUNCHER) ? self->weaponmode : 0;

    while( wp->w && (wp->w != lw || wp->mode != wm) ) wp++;
    if( !wp->w ) return;

    do{
        wp++;
        if( !wp->w ) wp = !prev ?&weapon_cycle[0] : &weapon_cycle_rev[0];
        am = W_CanUseWeapon( wp->w );
        if(!am){
            self->weaponmode = wp->mode;
            self->current_weapon = wp->w;
			self->last_weapon = lw;
            self->last_weaponmode = wm;
			W_SetCurrentAmmo(  );
			W_PrintWeaponMessage(  );
			self->StatusRefreshTime = g_globalvars.time + 0.1;
            return;
        }
    }while( wp->w != lw || wp->mode != wm );
}



void ImpulseCommands(  )
{
        char st[20];

	if ( self->last_impulse == TF_DETPACK && self->s.v.impulse )
		TeamFortress_SetDetpack( self->s.v.impulse );
	else
	{
		if ( self->last_impulse == TF_SCAN && self->s.v.impulse )
			TeamFortress_Scan_Angel( self->s.v.impulse, 0 );
		if ( self->s.v.impulse == TF_SCAN_ENEMY || self->s.v.impulse == TF_SCAN_FRIENDLY )
			TeamFortress_Scan_Angel( self->s.v.impulse, 0 );
	}

	if ( self->s.v.impulse == 8 && self->current_menu != MENU_CLASSHELP )
	{
		self->current_menu = MENU_CLASSHELP;
		self->menu_count = MENU_REFRESH_RATE;
		self->menu_displaytime = 0;
	}

	if ( self->s.v.impulse == 242 && !tf_data.birthday )
	{
		GetSVInfokeyString( "bd", "birthday", st, sizeof( st ), "" );
		if ( strneq( st, "off" ) )
		{
			self->current_menu = MENU_BIRTHDAY1;
			self->menu_count = MENU_REFRESH_RATE;
			self->menu_displaytime = 0;
			self->s.v.impulse = 0;
			return;
		}
	}
	if ( ( tf_data.cb_prematch_time > g_globalvars.time ) || tf_data.cease_fire )
	{
		PreMatchImpulses(  );
		DeadImpulses(  );
		self->s.v.impulse = 0;
		return;
	}
	if ( self->s.v.impulse == TF_SPECIAL_SKILL )
		UseSpecialSkill(  );

        if( tg_data.tg_enabled )
        {
         	switch ( ( int ) self->s.v.impulse )
         	{
         	case TG_MAINMENU_IMPULSE:
         		self->current_menu = TG_MENU_MAIN;
         		self->s.v.impulse = 0;
         		break;
         	case TG_SG_REBUILD_IMPULSE:
         		Eng_StaticSG_Activate(  );
         		self->s.v.impulse = 0;
         		break;
         	case TG_SG_RELOAD_IMPULSE:
         		Eng_SGReload(  );
         		self->s.v.impulse = 0;
         		break;
         	case TG_SG_UPGRADE_IMPULSE:
         		Eng_SGUp(  );
         		self->s.v.impulse = 0;
         		break;
         	case TG_DISP_LOAD_IMPULSE:
         		Eng_DispLoad(  );
         		self->s.v.impulse = 0;
         		break;
         	case TG_DISP_UNLOAD_IMPULSE:
         		Eng_DispUnload(  );
         		self->s.v.impulse = 0;
         		break;
         	case TG_CONC_IMPULSE:
         		TG_Eff_Conc( self );
         		self->s.v.impulse = 0;
         		break;
         	case TG_FLASH_IMPULSE:
         		TG_Eff_Flash( self );
         		self->s.v.impulse = 0;
         		break;
         	case TG_EFF_REMOVE_IMPULSE:
         		TG_Eff_Remove(self);
         		self->s.v.impulse = 0;
         		break;
         	}
        }

	if ( !self->is_building && !self->is_detpacking && !self->is_feigning )
	{
		switch ( ( int ) self->s.v.impulse )
		{
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
        case TF_MEDIKIT:
		case AXE_IMP:
			W_ChangeWeapon(  );
			break;
		case TF_HOOK_IMP1:
		case TF_HOOK_IMP2:
			if ( tf_data.allow_hook )
				W_ChangeWeapon(  );
			break;
		case TF_WEAPLAST:
			if ( self->last_weapon )
				W_ChangeWeapon(  );
			break;
		case TF_WEAPNEXT:
			CycleWeaponCommand( 0 );
            break;
		case TF_WEAPPREV:
			CycleWeaponCommand( 1 );
			break;
		case TF_GRENADE_1:
		case TF_GRENADE_2:
			TeamFortress_PrimeGrenade(  );
			break;
		case TF_SCAN_10:
			TeamFortress_Scan_Angel( 10, 0 );
			break;
		case TF_SCAN_30:
			TeamFortress_Scan_Angel( 30, 0 );
			break;
		case TF_SCAN_100:
			TeamFortress_Scan_Angel( 100, 0 );
			break;
		case TF_DETPACK_5:
			TeamFortress_SetDetpack( 5 );
			break;
		case TF_DETPACK_20:
			TeamFortress_SetDetpack( 20 );
			break;
        case TF_DETPACK_50:
			TeamFortress_SetDetpack( 50 );
			break;
		case TF_DROP_AMMO:
			self->current_menu = MENU_DROP;
			self->menu_count = MENU_REFRESH_RATE - 5;
			break;
		case TF_RELOAD:
			TeamFortress_ReloadCurrentWeapon(  );
			break;
		case TF_DISCARD:
			TeamFortress_Discard(  );
			break;
		}
	}
	switch ( ( int ) self->s.v.impulse )
	{
	case TF_INVENTORY:
		TeamFortress_Inventory(  );
		break;
	case TF_MEDIC_HELPME:
		if ( self->playerclass )
			TeamFortress_SaveMe(  );
		break;
	case TF_ID:
		TeamFortress_ID(  );
		break;
	case TF_SHOWIDS:
		TeamFortress_ShowIDs(  );
		break;
	case TF_GRENADE_T:
		TeamFortress_ThrowGrenade(  );
		break;
	case TF_DROPGOAL:
		if ( self->playerclass )
			DropGoalItems(  );
		break;
	case TF_PB_DETONATE:
		TeamFortress_DetonatePipebombs(  );
		break;
	case TF_DETPACK_STOP:
		TeamFortress_DetpackStop(  );
		break;
	case TF_ENGINEER_DETSENTRY:
		if ( self->playerclass == PC_ENGINEER || tg_data.tg_enabled )			
			DestroyBuilding( self, "building_sentrygun" );
		break;
	case TF_ENGINEER_DETDISP:
		if ( self->playerclass == PC_ENGINEER || tg_data.tg_enabled )			
			DestroyBuilding( self, "building_dispenser" );
		break;
	/*case 196:
		if ( self->playerclass == PC_ENGINEER || tg_data.tg_enabled )			
			DestroyBuilding( self, "building_teleporter_exit" );
		break;
	case 197:
		if ( self->playerclass == PC_ENGINEER || tg_data.tg_enabled )			
			DestroyBuilding( self, "building_teleporter_entrance" );
		break;*/
	case TF_SPY_SPY:
		if ( self->playerclass == PC_SPY )
			TeamFortress_SpyGoUndercover(  );
		break;
	case TF_SPY_DIE:
		if ( self->playerclass == PC_SPY )
			TeamFortress_SpyFeignDeath( 0 );
		break;
	case TF_SPY_SFEIGN_IMPULSE:
		if ( self->playerclass == PC_SPY )
			TeamFortress_SpyFeignDeath( 1 );
		break;
	case TF_ENGINEER_BUILD:
		if ( self->playerclass == PC_ENGINEER || tg_data.tg_enabled )			
			TeamFortress_EngineerBuild(  );
		break;
	case AUTOSCAN_IMPULSE:
		if ( self->playerclass == PC_SCOUT )
			ScannerSwitch(  );
		break;
	case TF_FLAG_INFO:
		if ( CTF_Map == 1 )
			TeamFortress_CTF_FlagInfo(  );
		else
			TeamFortress_DisplayDetectionItems(  );
		break;
	case TF_DISPLAYLOCATION:
		display_location(  );
		break;
	default:
		DeadImpulses(  );
		break;
	}
	if ( self->s.v.impulse == TF_DETPACK )
	{
		self->last_impulse = self->s.v.impulse;
	}
	if ( self->s.v.impulse == TF_SCAN )
		self->last_impulse = self->s.v.impulse;
	self->s.v.impulse = 0;
}

void PreMatchImpulses(  )
{
	switch ( ( int ) self->s.v.impulse )
	{
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case TF_MEDIKIT:
	case AXE_IMP:
		W_ChangeWeapon(  );
		break;
	case TF_HOOK_IMP1:
	case TF_HOOK_IMP2:
		if ( tf_data.allow_hook )
			W_ChangeWeapon(  );
		break;
    case TF_WEAPLAST:
        if ( self->last_weapon )
            W_ChangeWeapon(  );
        break;
    case TF_WEAPNEXT:
        CycleWeaponCommand( 0 );
        break;
    case TF_WEAPPREV:
        CycleWeaponCommand( 1 );
        break;
	}
	switch ( ( int ) self->s.v.impulse )
	{
	case TF_INVENTORY:
		TeamFortress_Inventory(  );
		break;
	case TF_ID:
		TeamFortress_ID(  );
		break;
	case TF_FLAG_INFO:
		if ( CTF_Map == 1 )
			TeamFortress_CTF_FlagInfo(  );
		else
			TeamFortress_DisplayDetectionItems(  );
		break;
	case TF_DISPLAYLOCATION:
		display_location(  );
		break;
	}
}

void DeadImpulses(  )
{
	switch ( ( int ) self->s.v.impulse )
	{
	case TF_SHOWTF:
		TeamFortress_ShowTF(  );
		break;
	case TF_AUTOZOOM:
		TeamFortress_AutoZoomToggle(  );
		break;
	case TF_SHOWLEGALCLASSES:
		TeamFortress_DisplayLegalClasses(  );
		break;
	case TF_CHANGEPC:
	case TF_CHANGEPC + 1:
	case TF_CHANGEPC + 2:
	case TF_CHANGEPC + 3:
	case TF_CHANGEPC + 4:
	case TF_CHANGEPC + 5:
	case TF_CHANGEPC + 6:
	case TF_CHANGEPC + 7:
	case TF_CHANGEPC + 8:
	case TF_CHANGEPC + 9:
	case TF_CHANGEPC + 10:
		TeamFortress_ChangeClass(  );
		break;
	case TF_CHANGEPC_MENU:
		if ( self->playerclass && deathmatch == 3 && ( tf_data.cb_prematch_time < g_globalvars.time ) )
		{
			self->current_menu = MENU_CHANGECLASS;
			self->menu_count = MENU_REFRESH_RATE;
		}
		break;
	case TF_HELP_MAP:
		TeamFortress_HelpMap(  );
		break;
	case TF_STATUS_QUERY:
		TeamFortress_StatusQuery(  );
		break;
	case TF_TEAM_1:
	case TF_TEAM_2:
	case TF_TEAM_3:
	case TF_TEAM_4:
		TeamFortress_TeamSet( (int)self->s.v.impulse - TF_TEAM_1 + 1);
		break;
	case TF_TEAM_SCORES:
		TeamFortress_TeamShowScores( 0 );
		break;
	case TF_TEAM_CLASSES:
		TeamFortress_TeamShowMemberClasses_New( self );
		break;
	case TF_STATUSBAR_ON:
		self->StatusRefreshTime = g_globalvars.time + 0.2;
		self->StatusBarSize = self->StatusBarSize + 1;
		if ( self->StatusBarSize > 2 )
			self->StatusBarSize = 1;
		break;
	case TF_STATUSBAR_OFF:
		self->StatusRefreshTime = g_globalvars.time + 60;
		self->StatusBarSize = 0;
		break;
	case TF_STATUSBAR_RES_START:
	case TF_STATUSBAR_RES_START +  1:
	case TF_STATUSBAR_RES_START +  2:
	case TF_STATUSBAR_RES_START +  3:
	case TF_STATUSBAR_RES_START +  4:
	case TF_STATUSBAR_RES_START +  5:
	case TF_STATUSBAR_RES_START +  6:
	case TF_STATUSBAR_RES_START +  7:
	case TF_STATUSBAR_RES_START +  8:
	case TF_STATUSBAR_RES_START +  9:
	case TF_STATUSBAR_RES_START +  10:
		StatusRes( self->s.v.impulse - TF_STATUSBAR_RES_START );
		break;
	case TF_FLAG_INFO:
		if ( CTF_Map == 1 )
			TeamFortress_CTF_FlagInfo(  );
		else
			TeamFortress_DisplayDetectionItems(  );
		break;
	case TF_ALIAS_CHECK:
		G_sprint( self, 2, "Aliases checked.\n" );
		self->got_aliases = 1;
		self->s.v.impulse = 0;
		break;
	default:
	        return;
	}
	self->s.v.impulse = 0;
}
#define SI_DETPIPE 1
#define SI_THROWGREN 2
#define SI_PRIMEONE 4
#define SI_PRIMETWO 8
#define SI_DISCARD 16		// fix me: do not save discard?
#define SI_INV 32
#define SI_ID 64
#define SI_SPECIAL 128
#define SI_RELOAD 256

void Angel_SaveImpulse( int impulse )
{
	if ( self->s.v.impulse <= 9 )
	{
		if ( self->current_weapon == WEAP_SPANNER )
			return;
		self->imp1 = self->s.v.impulse;
		self->s.v.impulse = 0;
		return;
	}
	if ( self->s.v.impulse == TF_GRENADE_T && ( self->tfstate & TFSTATE_GRENPRIMED ) )
		self->tf_impulse = self->tf_impulse | SI_THROWGREN;
	else if ( self->s.v.impulse == TF_PB_DETONATE )
		self->tf_impulse = self->tf_impulse | SI_DETPIPE;
	else if ( self->s.v.impulse == TF_SPECIAL_SKILL )
		self->tf_impulse = self->tf_impulse | SI_SPECIAL;
	else if ( self->s.v.impulse == TF_RELOAD )
		self->tf_impulse = self->tf_impulse | SI_RELOAD;
	else if ( self->s.v.impulse == TF_DISCARD )
		self->tf_impulse = self->tf_impulse | SI_DISCARD;
	else if ( self->s.v.impulse == TF_GRENADE_1 && !( self->tfstate & TFSTATE_GRENPRIMED ) )
	{
		self->tf_impulse = self->tf_impulse - ( self->tf_impulse & SI_PRIMETWO );
		self->tf_impulse = self->tf_impulse | SI_PRIMEONE;
	} else if ( self->s.v.impulse == TF_GRENADE_2 && !( self->tfstate & TFSTATE_GRENPRIMED ) )
	{
		self->tf_impulse = self->tf_impulse - ( self->tf_impulse & SI_PRIMEONE );
		self->tf_impulse = self->tf_impulse | SI_PRIMETWO;
	} else
		return;		// do not save impulse

	self->s.v.impulse = 0;	// reset impulse, chto by ne bylo gemora
}

int Angel_GetSavedImpulse(  )
{
	int     saved;

	// throwgren MUST be after priming;
	// special and reload MUST BE LAST!!!
	if ( self->tf_impulse & SI_PRIMEONE )
	{
		self->tf_impulse = self->tf_impulse - ( self->tf_impulse & SI_PRIMEONE );
		return TF_GRENADE_1;
	} else if ( self->tf_impulse & SI_PRIMETWO )
	{
		self->tf_impulse = self->tf_impulse - ( self->tf_impulse & SI_PRIMETWO );
		return TF_GRENADE_2;
	} else if ( self->tf_impulse & SI_THROWGREN )
	{
		self->tf_impulse = self->tf_impulse - ( self->tf_impulse & SI_THROWGREN );
		return TF_GRENADE_T;
	} else if ( self->tf_impulse & SI_DETPIPE )
	{
		self->tf_impulse = self->tf_impulse - ( self->tf_impulse & SI_DETPIPE );
		return TF_PB_DETONATE;
	} else if ( self->imp1 )
	{
		saved = self->imp1;
		self->imp1 = 0;
		return saved;
	} else if ( self->tf_impulse & SI_DISCARD )
	{
		self->tf_impulse = self->tf_impulse - ( self->tf_impulse & SI_DISCARD );
		return TF_DISCARD;
	} else if ( self->tf_impulse & SI_SPECIAL )
	{
		self->tf_impulse = self->tf_impulse - ( self->tf_impulse & SI_SPECIAL );
		return TF_SPECIAL_SKILL;
	} else if ( self->tf_impulse & SI_RELOAD )
	{
		self->tf_impulse = self->tf_impulse - ( self->tf_impulse & SI_RELOAD );
		return TF_RELOAD;
	}
/* REMOVE!!!
if (self.imp1){
   saved = self.imp1;
   self.imp1 = 0;
   return saved;
  }*/

	return 0;
}

void W_WeaponFrame(  )
{
	vec3_t  tv;

	if ( !( self->tfstate & TFSTATE_AIMING ) )
	{
		if ( self->height > 29 && self->height < 90 )
		{
			self->height = self->height + 8;
			if ( self->height > 90 )
				self->height = 90;
			TF_zoom( self->height );
		}
	}
	if ( self->current_menu > 0 )
	{
		Player_Menu(  );
		if ( self->s.v.impulse > 0
		     && ( self->current_menu == 23 || self->current_menu == 24 || self->current_menu == 25 ) )
			Menu_Input( self->s.v.impulse );
		else
		{
			if ( self->s.v.impulse > 0 && self->s.v.impulse < 11 )
			{
				Menu_Input( self->s.v.impulse );
				if ( self->s.v.impulse )
				{
					if ( !self->team_no && teamplay && self->lives )
						Menu_Team_Input( self->s.v.impulse );
					else
					{
						if ( !self->playerclass && self->lives )
							Menu_Class_Input( self->s.v.impulse );
					}
				}
			}
		}
	}
	if ( g_globalvars.time < self->attack_finished )
	{
	        DeadImpulses();
		if ( self->s.v.impulse )
			Angel_SaveImpulse( self->s.v.impulse );
		return;
	}
	if ( self->s.v.impulse && !self->has_disconnected )
		ImpulseCommands(  );
	self->s.v.impulse = 0;

	while ( ( self->s.v.impulse = Angel_GetSavedImpulse(  ) ) && !self->has_disconnected )
		ImpulseCommands(  );

	if ( self->s.v.impulse && !self->has_disconnected )
		ImpulseCommands(  );
	if ( tf_data.cease_fire )
		return;
	if ( self->is_building || self->is_detpacking || self->is_feigning )
		return;
	if ( !self->s.v.button0 && self->fire_held_down && self->current_weapon == WEAP_ASSAULT_CANNON )
	{
		self->fire_held_down = 0;
		self->tfstate -= ( self->tfstate & TFSTATE_AIMING );
		TeamFortress_SetSpeed( self );
		player_run(  );
	}
	if ( self->s.v.button0 && !self->fire_held_down )
	{
		if ( self->current_menu == MENU_CLASSHELP || self->current_menu == MENU_CLASSHELP2 )
		{
			self->current_menu = MENU_REPEATHELP;
			self->menu_count = MENU_REFRESH_RATE;
			Attack_Finished( 0.2 );
		} else
		{
			switch ( self->current_weapon )
			{
			case WEAP_SNIPER_RIFLE:
				if ( self->tfstate & TFSTATE_AIMING )
				{
					if ( !tf_data.snip_fps && self->heat < 400 )
						self->heat += 3;
					if ( self->height > 30 )
					{
						self->height -= 5;
						TF_zoom( self->height );
					}
				} else
				{
				        if( g_globalvars.time > self->allow_snip_time)
				        {
						VectorCopy( self->s.v.velocity, tv );
						tv[2] = 0;
						if ( vlen( tv ) <= WEAP_SNIPER_RIFLE_MAX_MOVE )
						{
							SniperSight_Create(  );
							if ( tf_data.snip_fps )
								self->heat = g_globalvars.time;
							else
								self->heat = 50;
        
							self->height = 90;
							self->tfstate = self->tfstate | TFSTATE_AIMING;
							TeamFortress_SetSpeed( self );
						}
				        }
				}

				break;
			case WEAP_ASSAULT_CANNON:
				if ( ( int ) self->s.v.flags & FL_ONGROUND )
				{
					SuperDamageSound(  );
					W_Attack(  );
				} else
				{
					//G_sprint( self, 1,
					//	  "You cannot fire the assault cannon without\nyour feet on the ground...\n" );
				}
				break;
			default:
				SuperDamageSound(  );
				W_Attack(  );
				break;
			}

		}
	} else
	{
		if ( !self->playerclass )
			self->weaponmode = 0;
		else
		{
			if ( self->tfstate & TFSTATE_AIMING )
			{
				W_Attack(  );
				self->tfstate = self->tfstate - TFSTATE_AIMING;
				TeamFortress_SetSpeed( self );
				self->heat = 0;
			}
		}
	}
}

/*
========
SuperDamageSound

Plays sound if needed
========
*/
void SuperDamageSound(  )
{
	if ( self->super_damage_finished > g_globalvars.time )
	{
		if ( self->super_sound < g_globalvars.time )
		{
			self->super_sound = g_globalvars.time + 1;
			sound( self, CHAN_BODY, "items/damage3.wav", 1, ATTN_NORM );
		}
	}
	return;
}
