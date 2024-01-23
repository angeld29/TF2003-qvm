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
 *  $Id: actions.c,v 1.26 2006-11-29 23:19:23 AngelD Exp $
 */
#include "g_local.h"

void	W_Reload_shotgun(  );
void	W_Reload_super_shotgun(  );
void 	W_Reload_grenade_launcher(  );
void 	W_Reload_rocket_launcher(  );

static const int     default_class_discard[10][4] = {
	{0, 0, 0, 0},
	{0, 0, 1, 1},
	{0, 0, 1, 1},
	{0, 1, 0, 1},
	{0, 1, 0, 1},
	{0, 0, 1, 1},
	{0, 1, 1, 0},
	{0, 1, 0, 0},
	{0, 0, 1, 1},
	{0, 0, 1, 0}
};

void doDiscard( gedict_t* newmis, gedict_t *self)
{
	vec3_t  vtemp;
	if ( !( newmis->s.v.ammo_shells + newmis->s.v.ammo_nails + newmis->s.v.ammo_rockets + newmis->s.v.ammo_cells ) )
	{
		dremove( newmis );
		return;
	}
	g_globalvars.newmis = EDICT_TO_PROG( newmis );

	if ( newmis->s.v.ammo_shells )
		self->s.v.ammo_shells = self->s.v.ammo_shells - newmis->s.v.ammo_shells;
	if ( newmis->s.v.ammo_nails )
		self->s.v.ammo_nails = self->s.v.ammo_nails - newmis->s.v.ammo_nails;
	if ( newmis->s.v.ammo_rockets )
		self->s.v.ammo_rockets = self->s.v.ammo_rockets - newmis->s.v.ammo_rockets;
	if ( newmis->s.v.ammo_cells )
		self->s.v.ammo_cells = self->s.v.ammo_cells - newmis->s.v.ammo_cells;
	W_SetCurrentAmmo();
	sound( self, CHAN_ITEM, "weapons/lock4.wav", 1, 1 );
	increment_team_ammoboxes( self->team_no );

	newmis->s.v.enemy = EDICT_TO_PROG( self );
	newmis->s.v.health = g_globalvars.time;
	newmis->s.v.weapon = 0;
	newmis->s.v.movetype = MOVETYPE_TOSS;
	newmis->s.v.solid = SOLID_TRIGGER;
	newmis->s.v.classname = "ammobox";
	newmis->team_no = self->team_no;
	trap_makevectors( self->s.v.v_angle );
	if ( self->s.v.v_angle[0] )
	{
		VectorScale( g_globalvars.v_forward, 400, newmis->s.v.velocity );
		VectorScale( g_globalvars.v_up, 200, vtemp );
		VectorAdd( vtemp, newmis->s.v.velocity, newmis->s.v.velocity );
	} else
	{
		aim( newmis->s.v.velocity );
		VectorScale( newmis->s.v.velocity, 400, newmis->s.v.velocity );
		newmis->s.v.velocity[2] = 200;
	}
	SetVector( newmis->s.v.avelocity, 0, 300, 0 );
	setsize( newmis, 0, 0, 0, 0, 0, 0 );
	setorigin( newmis, PASSVEC3( self->s.v.origin ) );
	newmis->s.v.nextthink = g_globalvars.time + 30;
	newmis->s.v.think = ( func_t ) TeamFortress_AmmoboxRemove;//	SUB_Remove;
	newmis->s.v.touch = ( func_t ) TeamFortress_AmmoboxTouch;
	setmodel( newmis, "progs/backpack.mdl" );
}

static int bound_discard_ammo( int ammo4drop, int have_ammo )
{
    if( ammo4drop > 0 )
    {
        if( ammo4drop > have_ammo )
            ammo4drop = have_ammo;
    }else {
        if( -ammo4drop > have_ammo )
            ammo4drop = 0;
        else
            ammo4drop += have_ammo;
    }
    return ammo4drop;
}

static gedict_t* defaultDiscard( gedict_t*self)
{
    gedict_t*newmis;
	newmis = spawn(  );

	if ( default_class_discard[self->playerclass][0] )
		newmis->s.v.ammo_shells = self->s.v.ammo_shells;
	if ( default_class_discard[self->playerclass][1] )
		newmis->s.v.ammo_nails = self->s.v.ammo_nails;
	if ( default_class_discard[self->playerclass][2] )
		newmis->s.v.ammo_rockets = self->s.v.ammo_rockets;
	if ( default_class_discard[self->playerclass][3] )
		newmis->s.v.ammo_cells = self->s.v.ammo_cells;
    return newmis;
}

void TeamFortress_Cmd_Discard( void )
{
	int     ammo_tmp;
	int argc,i;
	char    type[10];
	char    value[10];

	argc  = trap_CmdArgc();
	newmis = defaultDiscard( self );

	for (  i = 1 ; i < argc-1 ; i++ )
	{
		trap_CmdArgv( i, type, sizeof( type ) );

        trap_CmdArgv( ++i, value, sizeof( value ) );
        ammo_tmp = atoi(value);
		switch( type[0] )
		{
			case 's':
				newmis->s.v.ammo_shells = bound_discard_ammo( ammo_tmp, self->s.v.ammo_shells);
				break;
			case 'n':
				newmis->s.v.ammo_nails = bound_discard_ammo( ammo_tmp, self->s.v.ammo_nails);
				break;
				
			case 'r':
				newmis->s.v.ammo_rockets = bound_discard_ammo( ammo_tmp, self->s.v.ammo_rockets);
				break;
			case 'c':
				newmis->s.v.ammo_cells = bound_discard_ammo( ammo_tmp, self->s.v.ammo_cells);
				break;
		}
		
	}
    doDiscard( newmis, self );
}

static int bound_discard_ammo_classic( int ammo4drop, int have_ammo )
{
    if ( ammo4drop <= have_ammo )
        return have_ammo -  ammo4drop;
    else
        return 0; 
}

void TeamFortress_Discard(  )
{
	int     ammo_tmp;

	newmis = defaultDiscard( self );


    if( self->discard_shells >= 0 )
			newmis->s.v.ammo_shells = bound_discard_ammo_classic( self->discard_shells, self->s.v.ammo_shells );

    if( self->discard_nails >= 0 )
			newmis->s.v.ammo_nails = bound_discard_ammo_classic( self->discard_nails, self->s.v.ammo_nails );

    if( self->discard_rockets >= 0 )
			newmis->s.v.ammo_rockets = bound_discard_ammo_classic( self->discard_rockets, self->s.v.ammo_rockets );

    if( self->discard_cells >= 0 )
			newmis->s.v.ammo_cells = bound_discard_ammo_classic( self->discard_cells, self->s.v.ammo_cells );

    doDiscard( newmis, self );
}

void TeamFortress_SaveMe(  )
{
	gedict_t *te;
	gedict_t *tl;

	if ( self->last_saveme_sound < g_globalvars.time )
	{
		if ( g_random(  ) < 0.8 )
			sound( self, CHAN_WEAPON, "speech/saveme1.wav", 1, 1 );
		else
			sound( self, CHAN_WEAPON, "speech/saveme2.wav", 1, 1 );
		self->last_saveme_sound = g_globalvars.time + 4;
	}
	for ( te = world; ( te = trap_find( te, FOFS( s.v.classname ) , "player" )); )
	{
		if ( self == te || te->playerclass == PC_MEDIC || te->playerclass == PC_ENGINEER
		     || te->playerclass == PC_SPY )
		{
			if ( ( self->team_no && TeamFortress_isTeamsAllied(te->team_no , self->team_no) ) || te->playerclass == PC_SPY )
			{
				if ( visible( te ) )
				{
					g_globalvars.msg_entity = EDICT_TO_PROG( te );
					tl = spawn(  );	
					VectorCopy( self->s.v.origin, tl->s.v.origin );
					tl->s.v.origin[2] += 32;
					trap_WriteByte( MSG_ONE, SVC_TEMPENTITY );
					trap_WriteByte( MSG_ONE, TE_LIGHTNING3 );
					WriteEntity( MSG_ONE, tl );
					trap_WriteCoord( MSG_ONE, tl->s.v.origin[0] );
					trap_WriteCoord( MSG_ONE, tl->s.v.origin[1] );
					trap_WriteCoord( MSG_ONE, tl->s.v.origin[2] + 24 );
					trap_WriteCoord( MSG_ONE, self->s.v.origin[0] );
					trap_WriteCoord( MSG_ONE, self->s.v.origin[1] );
					trap_WriteCoord( MSG_ONE, self->s.v.origin[2] );
					dremove( tl );
				}
			}
		}
	}
}


void TeamFortress_ReloadCurrentWeapon(  )
{
	float   rt;
	gedict_t *tWeapon;

	if ( self->tfstate & TFSTATE_RELOADING )
		return;
	if ( self->current_weapon == WEAP_SHOTGUN )
	{
		if ( !self->s.v.ammo_shells )
		{
			G_sprint( self, 2, "out of shells.\n" );
			return;
		}
		if ( !self->reload_shotgun )
		{
			G_sprint( self, 2, "clip full.\n" );
			return;
		}
		if ( 8 - self->reload_shotgun == self->s.v.ammo_shells )
		{
			G_sprint( self, 2, "all shells are in the clip.\n" );
			return;
		}
		if ( self->reload_shotgun < self->s.v.ammo_shells )
		{
			Attack_Finished( 0.4 );
			rt = ( 8 - (float)self->reload_shotgun ) / 8;
			rt = 2 - 2 * rt;
			self->reload_shotgun = 0;
			if ( self->s.v.ammo_shells < 8 )
				self->reload_shotgun = 8 - self->s.v.ammo_shells;
			G_sprint( self, 2, "reloading...\n" );
			self->tfstate = self->tfstate | TFSTATE_RELOADING;
			tWeapon = spawn(  );
			tWeapon->s.v.owner = EDICT_TO_PROG(self);
			tWeapon->s.v.classname = "timer";
			tWeapon->s.v.nextthink = g_globalvars.time + rt;
			tWeapon->s.v.think = ( func_t ) W_Reload_shotgun;
			self->s.v.weaponmodel = "";
			self->s.v.weaponframe = 0;
		} else
			G_sprint( self, 2, "not enough ammo to reload\n" );
	}
	if ( self->current_weapon == WEAP_SUPER_SHOTGUN )
	{
		if ( !self->s.v.ammo_shells )
		{
			G_sprint( self, 2, "out of shells.\n" );
			return;
		}
		if ( !self->reload_super_shotgun )
		{
			G_sprint( self, 2, "clip full.\n" );
			return;
		}
		if ( 16 - self->reload_super_shotgun == self->s.v.ammo_shells )
		{
			G_sprint( self, 2, "all shells are in the clip.\n" );
			return;
		}
		if ( self->reload_super_shotgun < self->s.v.ammo_shells )
		{
			Attack_Finished( 0.7 );
			rt = ( 16 - (float)self->reload_super_shotgun ) / 16;
			rt = 3 - 3 * rt;
			self->reload_super_shotgun = 0;
			if ( self->s.v.ammo_shells < 16 )
				self->reload_super_shotgun = 16 - self->s.v.ammo_shells;
			G_sprint( self, 2, "reloading...\n" );
			self->tfstate = self->tfstate | TFSTATE_RELOADING;
			tWeapon = spawn(  );
			tWeapon->s.v.owner = EDICT_TO_PROG(self);
			tWeapon->s.v.classname = "timer";
			tWeapon->s.v.nextthink = g_globalvars.time + rt;
			tWeapon->s.v.think = ( func_t ) W_Reload_super_shotgun;
			self->s.v.weaponmodel = "";
			self->s.v.weaponframe = 0;
		} else
			G_sprint( self, 2, "not enough ammo to reload\n" );
	}
	if ( self->current_weapon == WEAP_GRENADE_LAUNCHER )
	{
		if ( !self->s.v.ammo_rockets )
		{
			G_sprint( self, 2, "out of grenades.\n" );
			return;
		}
		if ( !self->reload_grenade_launcher )
		{
			G_sprint( self, 2, "clip full.\n" );
			return;
		}
		if ( 6 - self->reload_grenade_launcher == self->s.v.ammo_rockets )
		{
			G_sprint( self, 2, "all grenades are in the clip.\n" );
			return;
		}
		if ( self->reload_grenade_launcher < self->s.v.ammo_rockets )
		{
			Attack_Finished( 0.6 );
			rt = ( 6 - (float)self->reload_grenade_launcher ) / 6;
			rt = 4 - 4 * rt;
			self->reload_grenade_launcher = 0;
			if ( self->s.v.ammo_rockets < 6 )
				self->reload_grenade_launcher = 6 - self->s.v.ammo_rockets;
			G_sprint( self, 2, "reloading...\n" );
			self->tfstate = self->tfstate | TFSTATE_RELOADING;
			tWeapon = spawn(  );
			tWeapon->s.v.owner = EDICT_TO_PROG(self);
			tWeapon->s.v.classname = "timer";
			tWeapon->s.v.nextthink = g_globalvars.time + rt;
			tWeapon->s.v.think = ( func_t ) W_Reload_grenade_launcher;
			self->s.v.weaponmodel = "";
			self->s.v.weaponframe = 0;
		} else
			G_sprint( self, 2, "not enough ammo to reload\n" );
	}
	if ( self->current_weapon == WEAP_ROCKET_LAUNCHER )
	{
		if ( !self->s.v.ammo_rockets )
		{
			G_sprint( self, 2, "out of rockets.\n" );
			return;
		}
		if ( !self->reload_rocket_launcher )
		{
			G_sprint( self, 2, "clip full.\n" );
			return;
		}
		if ( 4 - self->reload_rocket_launcher == self->s.v.ammo_rockets )
		{
			G_sprint( self, 2, "all rockets are in the clip.\n" );
			return;
		}
		if ( self->reload_rocket_launcher < self->s.v.ammo_rockets )
		{
			Attack_Finished( 0.8 );
			rt = ( 4 - (float)self->reload_rocket_launcher ) / 4;
			rt = 5 - 5 * rt;
			self->reload_rocket_launcher = 0;
			if ( self->s.v.ammo_rockets < 4 )
				self->reload_rocket_launcher = 4 - self->s.v.ammo_rockets;
			G_sprint( self, 2, "reloading...\n" );
			self->tfstate = self->tfstate | TFSTATE_RELOADING;
			tWeapon = spawn(  );
			tWeapon->s.v.owner = EDICT_TO_PROG(self);
			tWeapon->s.v.classname = "timer";
			tWeapon->s.v.nextthink = g_globalvars.time + rt;
			tWeapon->s.v.think = ( func_t ) W_Reload_rocket_launcher;
			self->s.v.weaponmodel = "";
			self->s.v.weaponframe = 0;
		}
	}
}

void TeamFortress_ID_Player( gedict_t * trace_ent )
{
	const char   *cls;

	if ( self->team_no && TeamFortress_isTeamsAllied(self->team_no , trace_ent->team_no) )
	{
		cls = TeamFortress_GetClassName( trace_ent->playerclass );
		switch ( self->playerclass )
		{
		case PC_MEDIC:
			G_centerprint( self, "\n\n\n\n%s\nFriendly %s\n%.0f health\n", trace_ent->s.v.netname, cls,
				       trace_ent->s.v.health );
			break;
		case PC_ENGINEER:
			G_centerprint( self, "\n\n\n\n%s\nFriendly %s\n%.0f armor\n", trace_ent->s.v.netname, cls,
				       trace_ent->s.v.armorvalue );
			break;
		default:
			G_centerprint( self, "\n\n\n\n%s\nFriendly %s\n", trace_ent->s.v.netname, cls );
			break;
		}
		return;
	}
//enemy
	if ( trace_ent->playerclass != PC_SPY )
	{
		cls = TeamFortress_GetClassName( trace_ent->playerclass );
		G_centerprint( self, "\n\n\n\n%s\nEnemy %s\n\n", trace_ent->s.v.netname, cls );
		return;
	}
//enemy spy
	cls = TeamFortress_GetClassName( trace_ent->undercover_skin );
	if ( self->team_no && TeamFortress_isTeamsAllied( self->team_no , trace_ent->undercover_team) )
	{
		switch ( self->playerclass )
		{
		case PC_MEDIC:
			if ( trace_ent->undercover_skin )
				G_centerprint( self, "\n\n\n\n%s\nFriendly %s\n%.0f health\n",
					       trace_ent->undercover_name, cls, trace_ent->s.v.health );
			else
				G_centerprint( self, "\n\n\n\n%s\nFriendly Spy\n%.0f health\n",
					       trace_ent->undercover_name, trace_ent->s.v.health );
			break;
		case PC_ENGINEER:
			if ( trace_ent->undercover_skin )
				G_centerprint( self, "\n\n\n\n%s\nFriendly %s\n%.0f armor\n",
					       trace_ent->undercover_name, cls, trace_ent->s.v.armorvalue );
			else
				G_centerprint( self, "\n\n\n\n%s\nFriendly Spy\n%.0f armor\n",
					       trace_ent->undercover_name, trace_ent->s.v.armorvalue );
			break;
		default:
			if ( trace_ent->undercover_skin )
				G_centerprint( self, "\n\n\n\n%s\nFriendly %s\n", trace_ent->undercover_name, cls );
			else
				G_centerprint( self, "\n\n\n\n%s\nFriendly Spy\n", trace_ent->undercover_name );
			break;
		}
		return;
	}
	if ( trace_ent->undercover_name )
	{
		if ( trace_ent->undercover_skin )
			G_centerprint( self, "\n\n\n\n%s\nEnemy %s\n", trace_ent->undercover_name, cls );
		else
			G_centerprint( self, "\n\n\n\n%s\nEnemy Spy\n", trace_ent->undercover_name );
	} else
	{
		if ( trace_ent->undercover_skin )
			G_centerprint( self, "\n\n\n\n%s\nEnemy %s\n", trace_ent->s.v.netname, cls );
		else
			G_centerprint( self, "\n\n\n\n%s\nEnemy Spy\n", trace_ent->s.v.netname );
	}
}
void TeamFortress_ID_Sentry( gedict_t * te )
{

    if ( self == te->real_owner )
    {
        if ( self->playerclass == PC_ENGINEER || tfset(tg_enabled) )
        {
            if ( (int)te->s.v.weapon != 3 )
                G_centerprint( self, "\n\n\nYour SentryGun Level %.0f\nHealth: %.0f\nShells: %.0f\n",
                        te->s.v.weapon, te->s.v.health, te->s.v.ammo_shells );
            else
                G_centerprint( self, "\n\n\nYour SentryGun Level %.0f\nHealth: %.0f\nShells: %.0f\nRockets: %.0f\n",
                        te->s.v.weapon, te->s.v.health, te->s.v.ammo_shells, te->s.v.ammo_rockets );
        } else
            G_centerprint( self, "\n\n\n\nYour SentryGun\n" );
    } else
    {
        if ( !teamplay )
            G_centerprint( self, "\n\n\n\nSentrygun made by\n%s\n", te->real_owner->s.v.netname );
        else
        {
            if ( self->team_no && TeamFortress_isTeamsAllied(self->team_no , te->team_no) )
            {
                if ( self->playerclass == PC_ENGINEER )
                {
                    if ( (int)te->s.v.weapon != 3 )
                        G_centerprint( self, "\n\n\nFriendly Sentrygun Level %.0f made by\n%s\nHealth: %.0f\nShells: %.0f\n",
                                te->s.v.weapon, te->real_owner->s.v.netname, te->s.v.health, te->s.v.ammo_shells );
                    else
                    {
                        G_centerprint( self, "\n\n\nFriendly Sentrygun Level %.0f made by\n%s\nHealth: %.0f\nShells: %.0f\nRockets: %.0f\n",
                                te->s.v.weapon, te->real_owner->s.v.netname, te->s.v.health, te->s.v.ammo_shells, te->s.v.ammo_rockets );
                    }
                } else
                    G_centerprint( self, "\n\n\n\nFriendly Sentrygun made by\n%s\n",
                            te->real_owner->s.v.netname );
            } else
                G_centerprint( self, "\n\n\n\nEnemy Sentrygun made by\n%s\n",
                        te->real_owner->s.v.netname );
        }
    }

}
void TeamFortress_ID(  )
{
	vec3_t  src, end;
	gedict_t *te, *trace_ent;
	if ( tfset(new_flash) && ( self->FlashTime >= ( 24 - NEW_FLASH_START_TIME ) ) )
		return;

	if((self->tfstate & TFSTATE_HALLUCINATING) && (tfset_new_gas & GAS_MASK_DISABLE_ID) )
		return;

	if( self->current_menu > MENU_DEFAULT)
		return;

	trap_makevectors( self->s.v.v_angle );
	VectorScale( g_globalvars.v_forward, 10, src );
	VectorAdd( src, self->s.v.origin, src );
	src[2] = self->s.v.absmin[2] + self->s.v.size[2] * 0.7;
	VectorScale( g_globalvars.v_forward, 2048, end );
	VectorAdd( end, src, end );
	traceline( PASSVEC3( src ), PASSVEC3( end ), 0, self );
	trace_ent = PROG_TO_EDICT( g_globalvars.trace_ent );

	if ( trace_ent == world )
		return;

	if ( VectorCompare( trace_ent->s.v.origin, world->s.v.origin ) )
		return;

    	if( GetSVInfokeyInt( "watervis", NULL, 0) == 0)
	{
	 traceline( PASSVEC3( src ), PASSVEC3( g_globalvars.trace_endpos ), 1, self );
	 //inwater inopen startsolid set only if NOMONSTERS = 1
	 if ( g_globalvars.trace_inopen && g_globalvars.trace_inwater )
	 	return;
	}
	if ( streq( trace_ent->s.v.classname, "player" ) && trace_ent->s.v.health > 0 )
	{
		if ( trace_ent->is_feigning && self->team_no && !TeamFortress_isTeamsAllied(self->team_no, trace_ent->team_no) )
			return;
		self->StatusRefreshTime = g_globalvars.time + 1.5;
		TeamFortress_ID_Player( trace_ent );

	} else
	{
		if ( streq( trace_ent->s.v.classname, "building_dispenser" ) )
		{
			self->StatusRefreshTime = g_globalvars.time + 1.5;

			if ( self == trace_ent->real_owner )
			{
				G_centerprint( self,
					       "\n\n\n\nYour Dispenser\nHealth: %.0f Armor: %.0f\nShells: %.0f Nails: %.0f\nRockets: %.0f Cells: %.0f\n",
					       trace_ent->s.v.health, trace_ent->s.v.armorvalue,
					       trace_ent->s.v.ammo_shells, trace_ent->s.v.ammo_nails,
					       trace_ent->s.v.ammo_rockets, trace_ent->s.v.ammo_cells );
				return;
			} else
			{
				if ( self->team_no && TeamFortress_isTeamsAllied( self->team_no , trace_ent->team_no)
				     && self->playerclass == PC_ENGINEER )
				{
					G_centerprint( self,
						       "\n\n\n\nDispenser made by %s\nHealth: %.0f Armor: %.0f\nShells: %.0f Nails: %.0f\nRockets: %.0f Cells: %.0f\n",
						       trace_ent->real_owner->s.v.netname, trace_ent->s.v.health,
						       trace_ent->s.v.armorvalue, trace_ent->s.v.ammo_shells,
						       trace_ent->s.v.ammo_nails, trace_ent->s.v.ammo_rockets,
						       trace_ent->s.v.ammo_cells );
				} else
					G_centerprint( self, "\n\n\n\nDispenser made by %s\n",
						       trace_ent->real_owner->s.v.netname );

			}
		} else
		{
			if ( streq( trace_ent->s.v.classname, "building_sentrygun_base" ) )
			{
				for ( te = world; ( te = trap_find( te, FOFS( s.v.classname ) , "building_sentrygun" )); )
				{
					if ( te->trigger_field == trace_ent )
					{
						trace_ent = te;
						break;
					}
				}
			}
			if ( streq( trace_ent->s.v.classname, "building_sentrygun" ) )
			{
				self->StatusRefreshTime = g_globalvars.time + 1.5;
				TeamFortress_ID_Sentry( trace_ent );
			}
		}
	}
}

