#include "g_local.h"

void    increment_team_ammoboxes( int tno );
void    TeamFortress_AmmoboxTouch(  );
void	W_Reload_shotgun(  );
void	W_Reload_super_shotgun(  );
void W_Reload_grenade_launcher(  );
void W_Reload_rocket_launcher(  );

int     default_class_discard[10][4] = {
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
void TeamFortress_Discard(  )
{
	int     ammo_tmp;
	vec3_t  vtemp;

	newmis = spawn(  );

	if ( default_class_discard[self->playerclass][0] )
		newmis->s.v.ammo_shells = self->s.v.ammo_shells;
	if ( default_class_discard[self->playerclass][1] )
		newmis->s.v.ammo_nails = self->s.v.ammo_nails;
	if ( default_class_discard[self->playerclass][2] )
		newmis->s.v.ammo_rockets = self->s.v.ammo_rockets;
	if ( default_class_discard[self->playerclass][3] )
		newmis->s.v.ammo_cells = self->s.v.ammo_cells;

	ammo_tmp = GetInfokeyInt( self, "mxs", "1", -1 );
	if ( ammo_tmp >= 0 )
	{
		if ( ammo_tmp <= self->s.v.ammo_shells )
			newmis->s.v.ammo_shells = self->s.v.ammo_shells -  ammo_tmp;
		else
			newmis->s.v.ammo_shells = 0;
	}
	ammo_tmp = GetInfokeyInt( self, "mxn", "2", -1 );
	if ( ammo_tmp >= 0 )
	{
		if ( ammo_tmp <= self->s.v.ammo_nails )
			newmis->s.v.ammo_nails = self->s.v.ammo_nails - ammo_tmp;
		else
			newmis->s.v.ammo_nails = 0;
	}
	ammo_tmp = GetInfokeyInt( self, "mxr", "3", -1 );
	if ( ammo_tmp >= 0 )
	{
		if ( ammo_tmp <= self->s.v.ammo_rockets )
			newmis->s.v.ammo_rockets = self->s.v.ammo_rockets - ammo_tmp;
		else
			newmis->s.v.ammo_rockets = 0;
	}
	ammo_tmp = GetInfokeyInt( self, "mxc", "4", -1 );
	if ( ammo_tmp >= 0 )
	{
		if ( ammo_tmp <= self->s.v.ammo_cells )
			newmis->s.v.ammo_cells = self->s.v.ammo_cells - ammo_tmp;
		else
			newmis->s.v.ammo_cells = 0;
	}


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
	sound( self, 3, "weapons/lock4.wav", 1, 1 );
	increment_team_ammoboxes( self->team_no );

	newmis->s.v.enemy = EDICT_TO_PROG( self );
	newmis->s.v.health = g_globalvars.time;
	newmis->s.v.weapon = 0;
	newmis->s.v.movetype = MOVETYPE_TOSS;
	newmis->s.v.solid = SOLID_TRIGGER;
	newmis->s.v.classname = "ammobox";
	newmis->team_no = self->team_no;
	makevectors( self->s.v.v_angle );
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
	newmis->s.v.think = ( func_t ) SUB_Remove;
	newmis->s.v.touch = ( func_t ) TeamFortress_AmmoboxTouch;
	setmodel( newmis, "progs/backpack.mdl" );
}

void TeamFortress_SaveMe(  )
{
	gedict_t *te;
	gedict_t *tl;

	if ( self->last_saveme_sound < g_globalvars.time )
	{
		if ( g_random(  ) < 0.8 )
			sound( self, 1, "speech/saveme1.wav", 1, 1 );
		else
			sound( self, 1, "speech/saveme2.wav", 1, 1 );
		self->last_saveme_sound = g_globalvars.time + 4;
	}
	for ( te = world; te = find( te, FOFS( s.v.classname ), "player" ); )
	{
		if ( self == te || te->playerclass == PC_MEDIC || te->playerclass == PC_ENGINEER
		     || te->playerclass == PC_SPY )
		{
			if ( ( te->team_no == self->team_no && self->team_no ) || te->playerclass == PC_SPY )
			{
				if ( visible( te ) )
				{
					g_globalvars.msg_entity = EDICT_TO_PROG( te );
					tl = spawn(  );
					VectorCopy( self->s.v.origin, tl->s.v.origin );
					tl->s.v.origin[2] += 32;
					trap_WriteByte( 1, 23 );
					trap_WriteByte( 1, 9 );
					WriteEntity( 1, tl );
					trap_WriteCoord( 1, tl->s.v.origin[0] );
					trap_WriteCoord( 1, tl->s.v.origin[1] );
					trap_WriteCoord( 1, tl->s.v.origin[2] + 24 );
					trap_WriteCoord( 1, self->s.v.origin[0] );
					trap_WriteCoord( 1, self->s.v.origin[1] );
					trap_WriteCoord( 1, self->s.v.origin[2] );
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
	char   *cls;

	if ( self->team_no && self->team_no == trace_ent->team_no )
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
	if ( self->team_no && self->team_no == trace_ent->undercover_team )
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
			G_centerprint( self, "\n\n\n\n%s\nFriendly %s\n", trace_ent->undercover_name, cls );
		else
			G_centerprint( self, "\n\n\n\n%s\nFriendly Spy\n", trace_ent->undercover_name );
	} else
	{
		if ( trace_ent->undercover_skin )
			G_centerprint( self, "\n\n\n\n%s\nFriendly %s\n", trace_ent->s.v.netname, cls );
		else
			G_centerprint( self, "\n\n\n\n%s\nFriendly Spy\n", trace_ent->s.v.netname );
	}
}
void TeamFortress_ID_Sentry( gedict_t * te )
{

	if ( self == te->real_owner )
	{
		if ( self->playerclass == PC_ENGINEER )
		{
			if ( te->s.v.weapon != 3 )
				G_centerprint( self, "\n\n\nYour SentryGun Level %.0f\nHealth: %.0f\nShells: %.0f\n",
					       te->s.v.weapon, te->s.v.health, te->s.v.ammo_shells );
			else
				G_centerprint( self,
					       "\n\n\nYour SentryGun Level %.0f\nHealth: %.0f\nShells: %.0f Rockets: %.0f\n",
					       te->s.v.weapon, te->s.v.health, te->s.v.ammo_shells,
					       te->s.v.ammo_rockets );
		} else
			G_centerprint( self, "\n\n\n\nYour SentryGun\n" );
	} else
	{
		if ( !teamplay )
			G_centerprint( self, "\n\n\n\nSentrygun made by\n%s\n", te->real_owner->s.v.netname );
		else
		{
			if ( self->team_no && self->team_no == te->team_no )
			{
				if ( self->playerclass == PC_ENGINEER )
				{
					if ( te->s.v.weapon != 3 )
						G_centerprint( self,
							       "\n\n\nFriendly Sentrygun Level %.0f made by\n%s\nHealth: %.0f\nShells: %.0f\n",
							       te->s.v.weapon, te->real_owner->s.v.netname,
							       te->s.v.health, te->s.v.ammo_shells );
					else
					{
						G_centerprint( self,
							       "\n\n\nFriendly Sentrygun Level %.0f made by\n%s\nHealth: %.0f\nShells: %.0f Rockets: %.0f\n",
							       te->s.v.weapon, te->real_owner->s.v.netname,
							       te->s.v.health, te->s.v.ammo_shells,
							       te->s.v.ammo_rockets );
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

	if ( tf_data.new_flash && ( self->FlashTime >= ( 24 - NEW_FLASH_START_TIME ) ) )
		return;
	if((self->tfstate & TFSTATE_HALLUCINATING) && (tf_data.new_gas & GAS_MASK_DISABLE_ID) )
		return;

	if( self->current_menu > MENU_DEFAULT)
		return;
	makevectors( self->s.v.v_angle );
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

	if ( streq( trace_ent->s.v.classname, "player" ) && trace_ent->s.v.health > 0 )
	{
		if ( trace_ent->is_feigning && self->team_no && self->team_no != trace_ent->team_no )
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
					       "\n\n\n\nYour Dispenser\nHealth: %.0f Armor %.0f\nShells %.0f Nails %.0f\nRockets %.0f Cells %.0f\n",
					       trace_ent->s.v.health, trace_ent->s.v.armorvalue,
					       trace_ent->s.v.ammo_shells, trace_ent->s.v.ammo_nails,
					       trace_ent->s.v.ammo_rockets, trace_ent->s.v.ammo_cells );
				return;
			} else
			{
				if ( self->team_no && self->team_no == trace_ent->team_no
				     && self->playerclass == PC_ENGINEER )
				{
					G_centerprint( self,
						       "\n\n\n\nDispenser made by %s\nHealth: %.0f Armor %.0f\nShells %.0f Nails %.0f\nRockets %.0f Cells %.0f\n",
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
				for ( te = world; te = find( te, FOFS( s.v.classname ), "building_sentrygun" ); )
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

