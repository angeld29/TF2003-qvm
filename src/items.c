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
 *  $Id: items.c,v 1.12 2005-05-23 18:54:02 AngelD Exp $
 */
#include "g_local.h"

void SUB_regen(  )
{
	self->s.v.model = self->mdl;	// restore original model
	self->s.v.solid = SOLID_TRIGGER;	// allow it to be touched again
	sound( self, CHAN_VOICE, "items/itembk2.wav", 1, ATTN_NORM );	// play respawn sound
	setorigin( self, PASSVEC3( self->s.v.origin ) );
}

void SP_noclass(  )
{
	G_dprintf( "noclass spawned at %f %f %f\n", self->s.v.origin[0], self->s.v.origin[1], self->s.v.origin[2] );
	ent_remove( self );
}

void q_touch(  )
{

	if ( strneq( other->s.v.classname, "player" ) )
		return;
	if ( other->s.v.health <= 0 )
		return;

	self->mdl = self->s.v.model;

	sound( other, CHAN_VOICE, self->s.v.noise, 1, ATTN_NORM );
	stuffcmd( other, "bf\n" );

	self->s.v.solid = SOLID_NOT;
	other->s.v.items = ( ( int ) other->s.v.items ) | IT_QUAD;
	self->s.v.model = "";

// do the apropriate action
	other->super_time = 1;
	other->super_damage_finished = self->cnt;

	G_bprint( PRINT_LOW, "%s recovered a Quad with %d seconds remaining!\n",
		  other->s.v.netname, ( int ) ( other->super_damage_finished - g_globalvars.time ) );


	activator = other;
	SUB_UseTargets(  );	// fire all targets / killtargets
}

void DropQuad( float timeleft )
{
	gedict_t *item;

	item = spawn(  );
	VectorCopy( self->s.v.origin, item->s.v.origin );
	item->s.v.origin[2] -= 24;

	item->s.v.velocity[2] = 300;
	item->s.v.velocity[0] = -100 + ( g_random(  ) * 200 );
	item->s.v.velocity[1] = -100 + ( g_random(  ) * 200 );

	item->s.v.flags = FL_ITEM;
	item->s.v.solid = SOLID_TRIGGER;
	item->s.v.movetype = MOVETYPE_TOSS;
	item->s.v.noise = "items/damage.wav";

	setmodel( item, "progs/quaddama.mdl" );
	setsize( item, -16, -16, -24, 16, 16, 32 );

	item->cnt = g_globalvars.time + timeleft;
	item->s.v.touch = ( func_t ) q_touch;
	item->s.v.nextthink = g_globalvars.time + timeleft;	// remove it with the time left on it
	item->s.v.think = ( func_t ) SUB_Remove;
}

void r_touch(  )
{

	if ( strneq( other->s.v.classname, "player" ) )
		return;
	if ( other->s.v.health <= 0 )
		return;

	self->mdl = self->s.v.model;

	sound( other, CHAN_VOICE, self->s.v.noise, 1, ATTN_NORM );
	stuffcmd( other, "bf\n" );

	self->s.v.solid = SOLID_NOT;
	other->s.v.items = ( ( int ) other->s.v.items ) | IT_INVISIBILITY;
	self->s.v.model = "";

// do the apropriate action
	other->invisible_time = 1;
	other->invisible_finished = self->cnt;

	G_bprint( PRINT_LOW, "%s recovered a Ring with %d seconds remaining!\n",
		  other->s.v.netname, ( int ) ( other->invisible_finished - g_globalvars.time ) );

	activator = other;
	SUB_UseTargets(  );	// fire all targets / killtargets
}

void DropRing( float timeleft )
{
	gedict_t *item;

	item = spawn(  );
	VectorCopy( self->s.v.origin, item->s.v.origin );
	item->s.v.origin[2] -= 24;

	item->s.v.velocity[2] = 300;
	item->s.v.velocity[0] = -100 + ( g_random(  ) * 200 );
	item->s.v.velocity[1] = -100 + ( g_random(  ) * 200 );

	item->s.v.flags = FL_ITEM;
	item->s.v.solid = SOLID_TRIGGER;
	item->s.v.movetype = MOVETYPE_TOSS;
	item->s.v.noise = "items/inv1.wav";

	setmodel( item, "progs/invisibl.mdl" );
	setsize( item, -16, -16, -24, 16, 16, 32 );

	item->cnt = g_globalvars.time + timeleft;
	item->s.v.touch = ( func_t ) r_touch;
	item->s.v.nextthink = g_globalvars.time + timeleft;	// remove after 30 seconds
	item->s.v.think = ( func_t ) SUB_Remove;
}

void PlaceItem(  )
{
	self->s.v.solid = SOLID_TRIGGER;
	self->s.v.movetype = MOVETYPE_TOSS;
	self->s.v.flags = FL_ITEM;
	self->mdl = self->s.v.model;

	SetVector( self->s.v.velocity, 0, 0, 0 );
	self->s.v.origin[2] += 6;

	if ( !droptofloor( self ) )
	{
		G_dprintf( "Bonus item fell out of level at  '%f %f %f'\n",
			  self->s.v.origin[0], self->s.v.origin[1], self->s.v.origin[2] );
		ent_remove( self );
	}
}

/*
============
StartItem

Sets the clipping size and plants the object on the floor
============
*/

void StartItem(  )
{
	self->s.v.nextthink = g_globalvars.time + 0.2;	// items start after other solids
	self->s.v.think = ( func_t ) PlaceItem;
}

/*
=========================================================================

HEALTH BOX

=========================================================================
*/
//
// T_Heal: add health to an entity, limiting health to max_health
// "ignore" will ignore max_health limit
//
int T_Heal( gedict_t * e, float healamount, float ignore )
{
	if ( e->s.v.health <= 0 )
		return 0;

	if ( ( !ignore ) && ( e->s.v.health >= e->s.v.max_health ) )
	{
		if ( e->leg_damage )
		{
			e->leg_damage -= ceil( e->s.v.health / 20 );
			if ( e->leg_damage < 1 )
				e->leg_damage = 0;
			TeamFortress_SetSpeed( e );
			return 1;
		} else
			return 0;
	}

	healamount = ceil( healamount );

	e->s.v.health = e->s.v.health + healamount;
	if ( ( !ignore ) && ( e->s.v.health >= e->s.v.max_health ) )
		e->s.v.health = e->s.v.max_health;

	if ( e->s.v.health > 250 )
		e->s.v.health = 250;
	if ( e->leg_damage )
	{
		if ( e->s.v.health > 95 )
			e->leg_damage = 0;
		else
			e->leg_damage -= ceil( e->s.v.health / 20 );
		if ( e->leg_damage < 1 )
			e->leg_damage = 0;
		TeamFortress_SetSpeed( e );
	}
	return 1;
}


#define   H_ROTTEN  1
#define   H_MEGA 2
void    health_touch(  );
void    item_megahealth_rot(  );

/*QUAKED item_health (.3 .3 1) (0 0 0) (32 32 32) rotten megahealth
Health box. Normally gives 25 points.
Rotten box heals 5-10 points,
megahealth will add 100 health, then 
rot you down to your maximum health limit, 
one point per second.
*/

void SP_item_health(  )
{
	if ( !CheckExistence(  ) )
	{
		dremove( self );
		return;
	}
	self->s.v.touch = ( func_t ) health_touch;
	if ( ( int ) self->s.v.spawnflags & H_ROTTEN )
	{
		trap_precache_model( "maps/b_bh10.bsp" );
		trap_precache_sound( "items/r_item1.wav" );
		setmodel( self, "maps/b_bh10.bsp" );
		self->s.v.noise = "items/r_item1.wav";
		self->healamount = 15;
		self->healtype = 0;
	} else
	{
		if ( ( int ) self->s.v.spawnflags & H_MEGA )
		{
			trap_precache_model( "maps/b_bh100.bsp" );
			trap_precache_sound( "items/r_item2.wav" );
			setmodel( self, "maps/b_bh100.bsp" );
			self->s.v.noise = "items/r_item2.wav";
			self->healamount = 100;
			self->healtype = 2;
		} else
		{
			trap_precache_model( "maps/b_bh25.bsp" );
			trap_precache_sound( "items/health1.wav" );
			setmodel( self, "maps/b_bh25.bsp" );
			self->s.v.noise = "items/health1.wav";
			self->healamount = 25;
			self->healtype = 1;
		}
	}

	setsize( self, 0, 0, 0, 32, 32, 56 );
	StartItem( );
}

void health_touch(  )
{
//      float   amount;
	float   medi;

	if ( strneq( other->s.v.classname, "player" ) )
		return;


	if ( other->is_feigning )
		return;
	if ( ( other->tfstate & TFSTATE_CANT_MOVE ) || ( other->tfstate & TFSTATE_AIMING ) )
		return;
	if ( tf_data.cb_prematch_time > g_globalvars.time )
		return;
	medi = 0;
	if ( self->healtype == 2 )	// Megahealth?  Ignore max_health...
	{

		if ( !( other->tfstate & TFSTATE_INFECTED ) )
		{
			if ( other->s.v.health >= 250 )
				return;
			if ( !T_Heal( other, self->healamount, 1 ) )
				return;
		}
	} else
	{
		if ( !T_Heal( other, self->healamount, 0 ) )
		{
			if ( other->weapons_carried & WEAP_MEDIKIT )
			{
				if ( other->ammo_medikit < other->maxammo_medikit )
				{
					other->ammo_medikit += self->healamount;
					if ( other->ammo_medikit > other->maxammo_medikit )
						other->ammo_medikit = other->maxammo_medikit;
					G_sprint( other, 0, "You gather %.0f medikit ammo\n", self->healamount );
// health touch sound
					sound( other, CHAN_ITEM, self->s.v.noise, 1, ATTN_NORM );

					stuffcmd( other, "bf\n" );

					self->s.v.model = "";
					self->s.v.solid = SOLID_NOT;

					if ( deathmatch != 2 )
					{
						if ( deathmatch )
							self->s.v.nextthink = g_globalvars.time + 20;
						self->s.v.think = ( func_t ) SUB_regen;
					}
					activator = other;
					SUB_UseTargets(  );
				}
			}
			return;
		}
	}
	if ( ( other->tfstate & TFSTATE_INFECTED ) && self->healamount > 80 )
	{
		G_sprint( other, 1, "You have been healed of your infection!" );
		other->tfstate -= ( other->tfstate & TFSTATE_INFECTED );
	} else
	{
		G_sprint( other, PRINT_LOW, "You receive %.0f health\n", self->healamount );
	}
// health touch sound
	sound( other, CHAN_ITEM, self->s.v.noise, 1, ATTN_NORM );

	stuffcmd( other, "bf\n" );

	self->s.v.model = "";
	self->s.v.solid = SOLID_NOT;

	// Megahealth = rot down the player's super health
	if ( self->healtype == 2 )
	{
		other->s.v.items = ( int ) other->s.v.items | IT_SUPERHEALTH;
		self->s.v.nextthink = g_globalvars.time + 5;
		self->s.v.think = ( func_t ) item_megahealth_rot;
		self->s.v.owner = EDICT_TO_PROG( other );
	} else
	{
		if ( deathmatch != 2 )	// deathmatch 2 is the silly old rules
		{
			if ( deathmatch )
				self->s.v.nextthink = g_globalvars.time + 20;
			self->s.v.think = ( func_t ) SUB_regen;
		}
	}
	activator = other;
	SUB_UseTargets(  );	// fire all targets / killtargets
}

void item_megahealth_rot(  )
{
	other = PROG_TO_EDICT( self->s.v.owner );
	if ( other->s.v.health > other->s.v.max_health )
	{
		other->s.v.health -= 1;
		self->s.v.nextthink = g_globalvars.time + 1;
		return;
	}
// it is possible for a player to die and respawn between rots, so don't
// just blindly subtract the flag off
	other->s.v.items -= ( int ) other->s.v.items & IT_SUPERHEALTH;
	if ( streq( self->s.v.classname, "medikit_rot" ) )
	{
		dremove( self );
		return;
	}
	if ( deathmatch != 2 )	// deathmatch 2 is silly old rules
	{
		self->s.v.nextthink = g_globalvars.time + 20;
		self->s.v.think = ( func_t ) SUB_regen;
	}
}

/*
===============================================================================

ARMOR

===============================================================================
*/
void armor_touch(  )
{
	float   type = 0, value = 0;
	int     bit = 0;
	gedict_t *oldself;

	if ( other->s.v.health <= 0 )
		return;
	if ( strneq( other->s.v.classname, "player" ) )
		return;
	if ( other->is_feigning )
		return;
	if ( ( other->tfstate & TFSTATE_CANT_MOVE ) || ( other->tfstate & TFSTATE_AIMING ) )
		return;
	if ( tf_data.cb_prematch_time > g_globalvars.time )
		return;
	if ( !strcmp( self->s.v.classname, "item_armor1" ) )
	{
		type = 0.3;
		value = 100;
		bit = IT_ARMOR1;
	}
	if ( !strcmp( self->s.v.classname, "item_armor2" ) )
	{
		type = 0.6;
		value = 150;
		bit = IT_ARMOR2;
	}
	if ( !strcmp( self->s.v.classname, "item_armorInv" ) )
	{
		type = 0.8;
		value = 300;
		bit = IT_ARMOR3;
	}
	if ( other->s.v.armortype * other->s.v.armorvalue >= type * value )
	{
		if ( other->playerclass == PC_ENGINEER )
		{
			if ( other->s.v.ammo_cells >= other->maxammo_cells )
				return;
		} else
			return;
	}
	if ( other->armor_allowed * other->maxarmor <= type * value )
	{
		if ( other->armor_allowed == other->s.v.armortype )
		{
			if ( other->maxarmor == other->s.v.armorvalue )
			{
				if ( other->playerclass == PC_ENGINEER )
				{
					if ( other->s.v.ammo_cells >= other->maxammo_cells )
						return;
				} else
					return;
			}
		}
	}
	if ( type > other->armor_allowed )
	{
		type = other->armor_allowed;
		if ( type == (float)0.8 )
			bit = IT_ARMOR3;
		else
		{
			if ( type == (float)0.6 )
				bit = IT_ARMOR2;
			else
			{
				if ( type == (float)0.3 )
					bit = IT_ARMOR1;
			}
		}
	}
	G_sprint( other, PRINT_LOW, "You got armor\n" );
	if ( value > other->maxarmor )
	{
		if ( other->playerclass == PC_ENGINEER && other->s.v.ammo_cells < other->maxammo_cells )
		{
			G_sprint( other, 0, "%.0f metal\n", value - other->maxarmor );
			other->s.v.ammo_cells += value - other->maxarmor;
			if ( other->s.v.ammo_cells > other->maxammo_cells )
				other->s.v.ammo_cells = other->maxammo_cells;
			oldself = self;
			self = other;
			W_SetCurrentAmmo(  );
			self = oldself;
		}
		value = other->maxarmor;
	}

	if ( other->s.v.armortype * other->s.v.armorvalue < type * value )
	{
		other->s.v.armortype = type;
		other->s.v.armorvalue = value;
		other->s.v.items = (int) other->s.v.items - ( ( int ) other->s.v.items & ( IT_ARMOR1 | IT_ARMOR2 | IT_ARMOR3 ) ) + bit;
	}
	if ( self->armorclass > 0 )
		other->armorclass = self->armorclass;
	self->s.v.solid = SOLID_NOT;
	self->s.v.model = "";
	if ( deathmatch != 2 )
		self->s.v.nextthink = g_globalvars.time + 20;
	if ( coop )
		self->s.v.nextthink = g_globalvars.time + 40;
	self->s.v.think = ( func_t ) SUB_regen;
	sound( other, 3, "items/armor1.wav", 1, 1 );
	stuffcmd( other, "bf\n" );
	activator = other;
	SUB_UseTargets(  );

}

/*QUAKED item_armor1 (0 .5 .8) (-16 -16 0) (16 16 32)
*/

void SP_item_armor1(  )
{
	if ( !CheckExistence(  ) )
	{
		dremove( self );
		return;
	}
	self->s.v.touch = ( func_t ) armor_touch;
	trap_precache_model( "progs/armor.mdl" );
	setmodel( self, "progs/armor.mdl" );
	self->s.v.skin = 0;
	setsize( self, -16, -16, 0, 16, 16, 56 );
	StartItem( );
}


/*QUAKED item_armor2 (0 .5 .8) (-16 -16 0) (16 16 32)
*/

void SP_item_armor2(  )
{
	if ( !CheckExistence(  ) )
	{
		dremove( self );
		return;
	}
	self->s.v.touch = ( func_t ) armor_touch;
	trap_precache_model( "progs/armor.mdl" );
	setmodel( self, "progs/armor.mdl" );
	self->s.v.skin = 1;
	setsize( self, -16, -16, 0, 16, 16, 56 );
	StartItem( );
}

/*QUAKED item_armorInv (0 .5 .8) (-16 -16 0) (16 16 32)
*/

void SP_item_armorInv(  )
{
	if ( !CheckExistence(  ) )
	{
		dremove( self );
		return;
	}
	self->s.v.touch = ( func_t ) armor_touch;
	trap_precache_model( "progs/armor.mdl" );
	setmodel( self, "progs/armor.mdl" );
	self->s.v.skin = 2;
	setsize( self, -16, -16, 0, 16, 16, 56 );
	StartItem( );
}

/*
===============================================================================

WEAPONS

===============================================================================
*/

void bound_other_ammo( gedict_t * p )
{
	if ( p->s.v.ammo_shells > TeamFortress_GetMaxAmmo( p, IT_SHELLS ) )
		p->s.v.ammo_shells = TeamFortress_GetMaxAmmo( p, IT_SHELLS );
	if ( p->s.v.ammo_nails > TeamFortress_GetMaxAmmo( p, IT_NAILS ) )
		p->s.v.ammo_nails = TeamFortress_GetMaxAmmo( p, IT_NAILS );
	if ( p->s.v.ammo_rockets > TeamFortress_GetMaxAmmo( p, IT_ROCKETS ) )
		p->s.v.ammo_rockets = TeamFortress_GetMaxAmmo( p, IT_ROCKETS );
	if ( p->s.v.ammo_cells > TeamFortress_GetMaxAmmo( p, IT_CELLS ) )
		p->s.v.ammo_cells = TeamFortress_GetMaxAmmo( p, IT_CELLS );
	if ( p->ammo_medikit > p->maxammo_medikit )
		p->ammo_medikit = p->maxammo_medikit;
	if ( p->s.v.armorvalue > p->maxarmor )
		p->s.v.armorvalue = p->maxarmor;
	if ( p->ammo_detpack > p->maxammo_detpack )
		p->ammo_detpack = p->maxammo_detpack;

	if ( p->no_grenades_1 > 4 )
		p->no_grenades_1 = 4;
	if ( p->no_grenades_2 > 4 )
		p->no_grenades_2 = 4;
	if ( p->tp_grenades_1 == GR_TYPE_NAIL && p->no_grenades_1 > 2 )
		p->no_grenades_1 = 2;
	if ( p->tp_grenades_2 == GR_TYPE_NAIL && p->no_grenades_2 > 2 )
		p->no_grenades_2 = 2;
	if ( p->tp_grenades_1 == GR_TYPE_CONCUSSION && p->no_grenades_1 > 3 )
		p->no_grenades_1 = 3;
	if ( p->tp_grenades_2 == GR_TYPE_CONCUSSION && p->no_grenades_2 > 3 )
		p->no_grenades_2 = 3;
	if ( p->tp_grenades_1 == GR_TYPE_CALTROPS && p->no_grenades_1 > 3 )
		p->no_grenades_1 = 3;
	if ( p->tp_grenades_2 == GR_TYPE_CALTROPS && p->no_grenades_2 > 3 )
		p->no_grenades_2 = 3;
}

float RankForWeapon( float w )
{
	if ( w == WEAP_LIGHTNING )
		return 1;
	if ( w == WEAP_ROCKET_LAUNCHER )
		return 2;
	if ( w == WEAP_SUPER_NAILGUN )
		return 3;
	if ( w == WEAP_GRENADE_LAUNCHER )
		return 4;
	if ( w == WEAP_SUPER_SHOTGUN )
		return 5;
	if ( w == WEAP_NAILGUN )
		return 6;
	return 7;
}

void Deathmatch_Weapon( float old, float new )
{
}


void    ammo_touch(  );

void Respawn_Item( gedict_t * ritem, gedict_t * act )
{
	gedict_t *oldself;

	oldself = self;
	self = ritem;
	self->s.v.model = "";
	self->s.v.solid = SOLID_NOT;
	if ( deathmatch != 2 )
		self->s.v.nextthink = g_globalvars.time + 30;
	else
	{
		if ( coop && ritem->s.v.touch == ( func_t ) ammo_touch )
			self->s.v.nextthink = g_globalvars.time + 45;
	}
	self->s.v.think = ( func_t ) SUB_regen;
	activator = act;
	SUB_UseTargets(  );
	self = oldself;
}


/*
=============
weapon_touch
=============
*/

void weapon_touch(  )
{
	int     hadammo, best, new = 0, old;
	gedict_t *stemp;
	int     leave;

//      char    s[20];


	if ( !( ( int ) other->s.v.flags & FL_CLIENT ) )
		return;
	if ( other->is_feigning )
		return;
	if ( ( other->tfstate & TFSTATE_CANT_MOVE ) || ( other->tfstate & TFSTATE_AIMING ) )
		return;
	if ( tf_data.cb_prematch_time > g_globalvars.time )
		return;
	stemp = self;
	self = other;
	best = W_BestWeapon(  );
	self = stemp;
	if ( deathmatch == 2 || coop )
		leave = 1;
	else
		leave = 0;
	if ( !strcmp( self->s.v.classname, "weapon_nailgun" ) )
	{
		if ( leave && ( other->weapons_carried & WEAP_NAILGUN ) )
			return;
		if ( !TeamFortress_CanGetWeapon( other, WEAP_NAILGUN ) )
			return;
		hadammo = other->s.v.ammo_nails;
		new = WEAP_NAILGUN;
		other->s.v.ammo_nails = other->s.v.ammo_nails + 30;
	} else
	{
		if ( streq( self->s.v.classname, "weapon_supernailgun" ) )
		{
			if ( leave && ( other->weapons_carried & WEAP_SUPER_NAILGUN ) )
				return;
			if ( !TeamFortress_CanGetWeapon( other, WEAP_SUPER_NAILGUN ) )
				return;
			hadammo = other->s.v.ammo_rockets;
			new = WEAP_SUPER_NAILGUN;
			other->s.v.ammo_nails = other->s.v.ammo_nails + 30;
		} else
		{
			if ( streq( self->s.v.classname, "weapon_supershotgun" ) )
			{
				if ( leave && ( other->weapons_carried & WEAP_SUPER_SHOTGUN ) )
					return;
				if ( !TeamFortress_CanGetWeapon( other, WEAP_SUPER_SHOTGUN ) )
					return;
				hadammo = other->s.v.ammo_rockets;
				new = WEAP_SUPER_SHOTGUN;
				other->s.v.ammo_shells = other->s.v.ammo_shells + 5;
			} else
			{
				if ( streq( self->s.v.classname, "weapon_rocketlauncher" ) )
				{
					if ( leave && ( other->weapons_carried & WEAP_ROCKET_LAUNCHER ) )
						return;
					if ( !TeamFortress_CanGetWeapon( other, WEAP_ROCKET_LAUNCHER ) )
						return;
					hadammo = other->s.v.ammo_rockets;
					new = WEAP_ROCKET_LAUNCHER;
					other->s.v.ammo_rockets = other->s.v.ammo_rockets + 5;
				} else
				{
					if ( streq( self->s.v.classname, "weapon_grenadelauncher" ) )
					{
						if ( leave && ( other->weapons_carried & WEAP_GRENADE_LAUNCHER ) )
							return;
						if ( !TeamFortress_CanGetWeapon( other, WEAP_GRENADE_LAUNCHER ) )
							return;
						hadammo = other->s.v.ammo_rockets;
						new = WEAP_GRENADE_LAUNCHER;
						other->s.v.ammo_rockets = other->s.v.ammo_rockets + 5;
					} else
					{
						if ( streq( self->s.v.classname, "weapon_lightning" ) )
						{
							if ( leave && ( other->weapons_carried & WEAP_LIGHTNING ) )
								return;
							if ( !TeamFortress_CanGetWeapon( other, WEAP_LIGHTNING ) )
								return;
							hadammo = other->s.v.ammo_rockets;
							new = WEAP_LIGHTNING;
							other->s.v.ammo_cells = other->s.v.ammo_cells + 15;
						} else
							G_Error( "weapon_touch: unknown classname" );
					}
				}
			}
		}
	}
	G_sprint( other, PRINT_LOW, "You got the %s\n", self->s.v.netname );
// weapon touch sound
	sound( other, CHAN_ITEM, "weapons/pkup.wav", 1, ATTN_NORM );
	stuffcmd( other, "bf\n" );

	bound_other_ammo( other );

// change to the weapon
	old = other->weapons_carried;

	other->weapons_carried = other->weapons_carried | new;
	stemp = self;
	self = other;
	Deathmatch_Weapon( old, new );
	W_SetCurrentAmmo(  );
	self = stemp;
	if ( leave )
		return;
	Respawn_Item( self, other );
}

/*QUAKED weapon_supershotgun (0 .5 .8) (-16 -16 0) (16 16 32)
*/

void SP_weapon_supershotgun(  )
{
	if ( !CheckExistence(  ) )
	{
		dremove( self );
		return;
	}
	trap_precache_model( "progs/g_shot.mdl" );
	setmodel( self, "progs/g_shot.mdl" );

	self->s.v.weapon = IT_SUPER_SHOTGUN;
	self->s.v.netname = "Double-barrelled Shotgun";
	self->s.v.touch = ( func_t ) weapon_touch;

	setsize( self, -16, -16, 0, 16, 16, 56 );

	StartItem(  );
}


/*QUAKED weapon_nailgun (0 .5 .8) (-16 -16 0) (16 16 32)
*/

void SP_weapon_nailgun(  )
{
	if ( !CheckExistence(  ) )
	{
		dremove( self );
		return;
	}
	trap_precache_model( "progs/g_nail.mdl" );
	setmodel( self, "progs/g_nail.mdl" );
	self->s.v.weapon = IT_NAILGUN;
	self->s.v.netname = "nailgun";
	self->s.v.touch = ( func_t ) weapon_touch;
	setsize( self, -16, -16, 0, 16, 16, 56 );
	StartItem(  );
}

/*QUAKED weapon_supernailgun (0 .5 .8) (-16 -16 0) (16 16 32)
*/

void SP_weapon_supernailgun(  )
{
	if ( !CheckExistence(  ) )
	{
		dremove( self );
		return;
	}
	trap_precache_model( "progs/g_nail2.mdl" );
	setmodel( self, "progs/g_nail2.mdl" );
	self->s.v.weapon = IT_SUPER_NAILGUN;
	self->s.v.netname = "Super Nailgun";
	self->s.v.touch = ( func_t ) weapon_touch;
	setsize( self, -16, -16, 0, 16, 16, 56 );
	StartItem(  );
}

/*QUAKED weapon_grenadelauncher (0 .5 .8) (-16 -16 0) (16 16 32)
*/

void SP_weapon_grenadelauncher(  )
{
	if ( !CheckExistence(  ) )
	{
		dremove( self );
		return;
	}
	trap_precache_model( "progs/g_rock.mdl" );
	setmodel( self, "progs/g_rock.mdl" );
	self->s.v.weapon = 3;
	self->s.v.netname = "Grenade Launcher";
	self->s.v.touch = ( func_t ) weapon_touch;
	setsize( self, -16, -16, 0, 16, 16, 56 );
	StartItem(  );
}


/*QUAKED weapon_rocketlauncher (0 .5 .8) (-16 -16 0) (16 16 32)
*/

void SP_weapon_rocketlauncher(  )
{
	if ( !CheckExistence(  ) )
	{
		dremove( self );
		return;
	}
	trap_precache_model( "progs/g_rock2.mdl" );
	setmodel( self, "progs/g_rock2.mdl" );
	self->s.v.weapon = 3;
	self->s.v.netname = "Rocket Launcher";
	self->s.v.touch = ( func_t ) weapon_touch;
	setsize( self, -16, -16, 0, 16, 16, 56 );
	StartItem(  );
}

/*QUAKED weapon_lightning (0 .5 .8) (-16 -16 0) (16 16 32)
*/

void SP_weapon_lightning(  )
{
	if ( !CheckExistence(  ) )
	{
		dremove( self );
		return;
	}
	trap_precache_model( "progs/g_light.mdl" );
	setmodel( self, "progs/g_light.mdl" );
	self->s.v.weapon = 3;
	self->s.v.netname = "Thunderbolt";
	self->s.v.touch = ( func_t ) weapon_touch;
	setsize( self, -16, -16, 0, 16, 16, 56 );
	StartItem(  );
}
extern char *GrenadeNames[];

int GetGrenadePossibility(  )
{
	float   maxg;

	if ( g_random(  ) < 0.5 )
		return 0;
	maxg = 4;
	if ( g_random(  ) < 0.5 )
	{
		if ( other->tp_grenades_1 == GR_TYPE_NAIL )
			maxg = 2;
		if ( other->tp_grenades_1 == GR_TYPE_CALTROPS )
			maxg = 3;
		if ( other->tp_grenades_1 && other->no_grenades_1 < maxg )
		{
			other->no_grenades_1 += 1;
			G_sprint( other, 2, "You found a %s", GrenadeNames[other->tp_grenades_1] );
			if ( other->tp_grenades_1 == GR_TYPE_CALTROPS )
				G_sprint( other, 2, " canister\n" );
			else
				G_sprint( other, 2, " grenade\n" );
			return 1;
		}
	} else
	{
		if ( other->tp_grenades_2 == GR_TYPE_NAIL )
			maxg = 2;
		if ( other->tp_grenades_2 == GR_TYPE_CONCUSSION )
			maxg = 3;
		if ( other->tp_grenades_2 && other->no_grenades_2 < maxg )
		{
			other->no_grenades_2 += 1;
			G_sprint( other, 2, "You found a %s grenade\n", GrenadeNames[other->tp_grenades_2] );
			return 1;
		}
	}
	return 0;
}

/*
===============================================================================

AMMO

===============================================================================
*/

void ammo_touch(  )
{
	gedict_t *stemp;
	float   best;
	int     gotgren;
	int     gotbox;


	if ( strneq( other->s.v.classname, "player" ) )
		return;

	if ( other->s.v.health <= 0 )
		return;

	if ( ( other->tfstate & TFSTATE_CANT_MOVE ) || ( other->tfstate & TFSTATE_AIMING ) )
		return;
	if ( tf_data.cb_prematch_time > g_globalvars.time )
		return;
	gotgren = 0;
	gotbox = 0;
	stemp = self;
	self = other;
	best = W_BestWeapon(  );
	self = stemp;
	if ( self->s.v.weapon == 1 )
	{
		if ( other->s.v.ammo_shells >= TeamFortress_GetMaxAmmo( other, IT_SHELLS ) )
			return;
		other->s.v.ammo_shells += self->aflag;
		gotbox = 1;
	}
	if ( self->s.v.weapon == 2 )
	{
		if ( other->s.v.ammo_nails >= TeamFortress_GetMaxAmmo( other, IT_NAILS ) )
			return;
		other->s.v.ammo_nails += self->aflag;
		gotbox = 1;
	} else
	{
		if ( self->s.v.weapon == 3 )
		{
			gotgren = GetGrenadePossibility(  );
			if ( other->s.v.ammo_rockets >= TeamFortress_GetMaxAmmo( other, IT_ROCKETS ) )
				gotbox = 0;
			else
			{
				gotbox = 1;
				other->s.v.ammo_rockets += self->aflag;
			}
		} else
		{
			if ( self->s.v.weapon == 4 )
			{
				if ( other->s.v.ammo_cells >= TeamFortress_GetMaxAmmo( other, IT_CELLS ) )
					return;
				other->s.v.ammo_cells += self->aflag;
				gotbox = 1;
			}
		}
	}
	if ( !gotbox && !gotgren )
		return;
// ammo touch sound
	sound( other, CHAN_ITEM, "weapons/lock4.wav", 1, ATTN_NORM );
	stuffcmd( other, "bf\n" );
	if ( gotbox )
	{
		bound_other_ammo( other );

		G_sprint( other, PRINT_LOW, "You got the %s\n", self->s.v.netname );
		stemp = self;
		self = other;
		W_SetCurrentAmmo(  );
		self = stemp;
	}
	Respawn_Item( self, other );
}

#define  WEAPON_BIG2  1

/*QUAKED item_shells (0 .5 .8) (0 0 0) (32 32 32) big
*/

void SP_item_shells(  )
{
	if ( !CheckExistence(  ) )
	{
		dremove( self );
		return;
	}
	self->s.v.touch = ( func_t ) ammo_touch;

	if ( ( int ) ( self->s.v.spawnflags ) & WEAPON_BIG2 )
	{
		trap_precache_model( "maps/b_shell1.bsp" );
		setmodel( self, "maps/b_shell1.bsp" );
		self->aflag = 40;
	} else
	{
		trap_precache_model( "maps/b_shell0.bsp" );
		setmodel( self, "maps/b_shell0.bsp" );
		self->aflag = 20;
	}

	self->s.v.weapon = 1;
	self->s.v.netname = "shells";

	setsize( self, 0, 0, 0, 32, 32, 56 );
	StartItem(  );
}

/*QUAKED item_spikes (0 .5 .8) (0 0 0) (32 32 32) big
*/

void SP_item_spikes(  )
{
	if ( !CheckExistence(  ) )
	{
		dremove( self );
		return;
	}
	self->s.v.touch = ( func_t ) ammo_touch;

	if ( ( int ) ( self->s.v.spawnflags ) & WEAPON_BIG2 )
	{
		trap_precache_model( "maps/b_nail1.bsp" );
		setmodel( self, "maps/b_nail1.bsp" );
		self->aflag = 50;
	} else
	{
		trap_precache_model( "maps/b_nail0.bsp" );
		setmodel( self, "maps/b_nail0.bsp" );
		self->aflag = 25;
	}
	self->s.v.weapon = 2;
	self->s.v.netname = "nails";

	setsize( self, 0, 0, 0, 32, 32, 56 );

	StartItem(  );

}

/*QUAKED item_rockets (0 .5 .8) (0 0 0) (32 32 32) big
*/

void SP_item_rockets(  )
{
	if ( !CheckExistence(  ) )
	{
		dremove( self );
		return;
	}
	self->s.v.touch = ( func_t ) ammo_touch;

	if ( ( int ) ( self->s.v.spawnflags ) & WEAPON_BIG2 )
	{
		trap_precache_model( "maps/b_rock1.bsp" );
		setmodel( self, "maps/b_rock1.bsp" );
		self->aflag = 10;
	} else
	{
		trap_precache_model( "maps/b_rock0.bsp" );
		setmodel( self, "maps/b_rock0.bsp" );
		self->aflag = 5;
	}
	self->s.v.weapon = 3;
	self->s.v.netname = "rockets";

	setsize( self, 0, 0, 0, 32, 32, 56 );

	StartItem(  );

}

/*QUAKED item_cells (0 .5 .8) (0 0 0) (32 32 32) big
*/

void SP_item_cells(  )
{
	if ( !CheckExistence(  ) )
	{
		dremove( self );
		return;
	}
	self->s.v.touch = ( func_t ) ammo_touch;

	if ( ( int ) ( self->s.v.spawnflags ) & WEAPON_BIG2 )
	{
		trap_precache_model( "maps/b_batt1.bsp" );
		setmodel( self, "maps/b_batt1.bsp" );
		self->aflag = 12;
	} else
	{
		trap_precache_model( "maps/b_batt0.bsp" );
		setmodel( self, "maps/b_batt0.bsp" );
		self->aflag = 6;
	}

	self->s.v.weapon = 4;
	self->s.v.netname = "cells";

	setsize( self, 0, 0, 0, 32, 32, 56 );

	StartItem(  );

}

/*QUAKED item_weapon (0 .5 .8) (0 0 0) (32 32 32) shotgun rocket spikes big
DO NOT USE THIS!!!! IT WILL BE REMOVED!
*/

#define WEAPON_SHOTGUN  1
#define WEAPON_ROCKET  2
#define WEAPON_SPIKES  4
#define WEAPON_BIG  8
void SP_item_weapon(  )
{
	if ( !CheckExistence(  ) )
	{
		dremove( self );
		return;
	}
	self->s.v.touch = ( func_t ) ammo_touch;

	if ( ( int ) ( self->s.v.spawnflags ) & WEAPON_SHOTGUN )
	{
		if ( ( int ) ( self->s.v.spawnflags ) & WEAPON_BIG )
		{
			trap_precache_model( "maps/b_shell1.bsp" );
			setmodel( self, "maps/b_shell1.bsp" );
			self->aflag = 40;
		} else
		{
			trap_precache_model( "maps/b_shell0.bsp" );
			setmodel( self, "maps/b_shell0.bsp" );
			self->aflag = 20;
		}
		self->s.v.weapon = 1;
		self->s.v.netname = "shells";
	}

	if ( ( int ) ( self->s.v.spawnflags ) & WEAPON_SPIKES )
	{
		if ( ( int ) ( self->s.v.spawnflags ) & WEAPON_BIG )
		{
			trap_precache_model( "maps/b_nail1.bsp" );
			setmodel( self, "maps/b_nail1.bsp" );
			self->aflag = 40;
		} else
		{
			trap_precache_model( "maps/b_nail0.bsp" );
			setmodel( self, "maps/b_nail0.bsp" );
			self->aflag = 20;
		}
		self->s.v.weapon = 2;
		self->s.v.netname = "spikes";
	}

	if ( ( int ) ( self->s.v.spawnflags ) & WEAPON_ROCKET )
	{
		if ( ( int ) ( self->s.v.spawnflags ) & WEAPON_BIG )
		{
			trap_precache_model( "maps/b_rock1.bsp" );
			setmodel( self, "maps/b_rock1.bsp" );
			self->aflag = 10;
		} else
		{
			trap_precache_model( "maps/b_rock0.bsp" );
			setmodel( self, "maps/b_rock0.bsp" );
			self->aflag = 5;
		}
		self->s.v.weapon = 3;
		self->s.v.netname = "rockets";
	}

	setsize( self, 0, 0, 0, 32, 32, 56 );

	StartItem(  );
}

/*
===============================================================================

KEYS

===============================================================================
*/
void key_touch(  )
{

	if ( strneq( other->s.v.classname, "player" ) )
		return;

	if ( other->s.v.health <= 0 )
		return;

	if ( ( int ) other->s.v.items & ( int ) self->s.v.items )
		return;
	if ( other->is_feigning )
		return;
	if ( ( other->tfstate & TFSTATE_CANT_MOVE ) || ( other->tfstate & TFSTATE_AIMING ) )
		return;
	if ( tf_data.cb_prematch_time > g_globalvars.time )
		return;

	G_sprint( other, PRINT_LOW, "You got the %s\n", self->s.v.netname );

	sound( other, CHAN_ITEM, self->s.v.noise, 1, ATTN_NORM );
	stuffcmd( other, "bf\n" );
	other->s.v.items = ( int ) other->s.v.items | ( int ) self->s.v.items;
	if ( !coop )
	{
		self->s.v.solid = SOLID_NOT;
		self->s.v.model = "";
	} else
	{
		if ( coop && self->s.v.deadflag )
		{
			dremove( self );
			return;
		}
	}
	activator = other;
	SUB_UseTargets(  );	// fire all targets / killtargets
}

extern char *key_models1[3];
extern char *key_names1[3];
extern char *key_sound1[3];

extern char *key_models2[3];
extern char *key_names2[3];
extern char *key_sound2[3];


void key_setsounds(  )
{
	if ( world->worldtype > 2 )
		return;
	self->s.v.noise = key_sound1[world->worldtype];
	trap_precache_sound( key_sound1[world->worldtype] );
}


/*QUAKED item_key1 (0 .5 .8) (-16 -16 -24) (16 16 32)
SILVER key
In order for keys to work
you MUST set your maps
worldtype to one of the
following:
0: medieval
1: metal
2: base
*/

void SP_item_key1(  )
{
	if ( !CheckExistence(  ) )
	{
		dremove( self );
		return;
	}
	setmodel( self, key_models1[world->worldtype] );
	self->s.v.netname = key_names1[world->worldtype];
	self->s.v.noise = key_sound1[world->worldtype];


	key_setsounds(  );
	self->s.v.touch = ( func_t ) key_touch;
	self->s.v.items = IT_KEY1;
	setsize( self, -16, -16, -24, 16, 16, 32 );
	StartItem(  );
}

/*QUAKED item_key2 (0 .5 .8) (-16 -16 -24) (16 16 32)
GOLD key
In order for keys to work
you MUST set your maps
worldtype to one of the
following:
0: medieval
1: metal
2: base
*/

void SP_item_key2(  )
{
	if ( !CheckExistence(  ) )
	{
		dremove( self );
		return;
	}
	setmodel( self, key_models2[world->worldtype] );
	self->s.v.netname = key_names2[world->worldtype];
	self->s.v.noise = key_sound2[world->worldtype];

	key_setsounds(  );
	self->s.v.touch = ( func_t ) key_touch;
	self->s.v.items = IT_KEY2;
	setsize( self, -16, -16, -24, 16, 16, 32 );
	StartItem(  );
}

/*
===============================================================================

END OF LEVEL RUNES

===============================================================================
*/

void sigil_touch(  )
{
//      gedict_t *stemp;
//      float   best;

	if ( strneq( other->s.v.classname, "player" ) )
		return;
	if ( other->s.v.health <= 0 )
		return;

	if ( other->is_feigning )
		return;
	if ( ( other->tfstate & TFSTATE_CANT_MOVE ) || ( other->tfstate & TFSTATE_AIMING ) )
		return;
	if ( tf_data.cb_prematch_time > g_globalvars.time )
		return;

        if( tf_data.lan_mode )
		G_centerprint( other, "You got the rune!" );

	sound( other, CHAN_ITEM, self->s.v.noise, 1, ATTN_NORM );
	stuffcmd( other, "bf\n" );
	self->s.v.solid = SOLID_NOT;
	self->s.v.model = "";
	g_globalvars.serverflags = ( int ) ( g_globalvars.serverflags ) | ( ( int ) ( self->s.v.spawnflags ) & 15 );
	self->s.v.classname = "";	// so rune doors won't find it

	activator = other;
	SUB_UseTargets(  );	// fire all targets / killtargets
}

/*QUAKED item_sigil (0 .5 .8) (-16 -16 -24) (16 16 32) E1 E2 E3 E4
End of level sigil, pick up to end episode and return to jrstart.
*/

void SP_item_sigil(  )
{

	if ( !CheckExistence(  ) )
	{
		dremove( self );
		return;
	}
	if ( !( int ) ( self->s.v.spawnflags ) )
		G_Error( "item_sigil no spawnflags" );

	trap_precache_sound( "misc/runekey.wav" );
	self->s.v.noise = "misc/runekey.wav";

	if ( ( int ) ( self->s.v.spawnflags ) & 1 )
	{
		trap_precache_model( "progs/end1.mdl" );
		setmodel( self, "progs/end1.mdl" );
	}
	if ( ( int ) ( self->s.v.spawnflags ) & 2 )
	{
		trap_precache_model( "progs/end2.mdl" );
		setmodel( self, "progs/end2.mdl" );
	}
	if ( ( int ) ( self->s.v.spawnflags ) & 4 )
	{
		trap_precache_model( "progs/end3.mdl" );
		setmodel( self, "progs/end3.mdl" );
	}
	if ( ( int ) ( self->s.v.spawnflags ) & 8 )
	{
		trap_precache_model( "progs/end4.mdl" );
		setmodel( self, "progs/end4.mdl" );
	}

	self->s.v.touch = ( func_t ) sigil_touch;
	setsize( self, -16, -16, -24, 16, 16, 32 );
	StartItem(  );
}

/*
===============================================================================

POWERUPS

===============================================================================
*/

void powerup_touch(  )
{

	if ( strneq( other->s.v.classname, "player" ) )
		return;
	if ( other->s.v.health <= 0 )
		return;
	if ( other->is_feigning )
		return;
	if ( ( other->tfstate & TFSTATE_CANT_MOVE ) || ( other->tfstate & TFSTATE_AIMING ) )
		return;
	if ( tf_data.cb_prematch_time > g_globalvars.time )
		return;
	if ( tf_data.disable_powerups )
		return;
	G_sprint( other, PRINT_LOW, "You got the %s\n", self->s.v.netname );

	if ( deathmatch )
	{
		self->mdl = self->s.v.model;
		if ( !strcmp( self->s.v.classname, "item_artifact_invulnerability" ) ||
		     !strcmp( self->s.v.classname, "item_artifact_invisibility" ) )
			self->s.v.nextthink = g_globalvars.time + 60 * 5;
		else
			self->s.v.nextthink = g_globalvars.time + 60;
		self->s.v.think = ( func_t ) SUB_regen;
	} else
	{
		if ( coop )
		{
			self->mdl = self->s.v.model;
			if ( !strcmp
			     ( self->s.v.classname, "item_artifact_invulnerability" )
			     || !strcmp( self->s.v.classname, "item_artifact_invisibility" ) )
				self->s.v.nextthink = g_globalvars.time + 120 * 5;
			else
				self->s.v.nextthink = g_globalvars.time + 120;
			self->s.v.think = ( func_t ) SUB_regen;
		}
	}
	sound( other, CHAN_VOICE, self->s.v.noise, 1, ATTN_NORM );
	stuffcmd( other, "bf\n" );
	self->s.v.solid = SOLID_NOT;
	other->s.v.items = ( ( int ) other->s.v.items ) | ( ( int ) self->s.v.items );
	self->s.v.model = "";

// do the apropriate action
	if ( !strcmp( self->s.v.classname, "item_artifact_envirosuit" ) )
	{
		other->rad_time = 1;
		other->radsuit_finished = g_globalvars.time + 30;
	}

	if ( !strcmp( self->s.v.classname, "item_artifact_invulnerability" ) )
	{
		other->invincible_time = 1;
		other->invincible_finished = g_globalvars.time + 30;
	}

	if ( streq( self->s.v.classname, "item_artifact_invisibility" ) )
	{
		other->invisible_time = 1;
		other->invisible_finished = g_globalvars.time + 30;
	}

	if ( streq( self->s.v.classname, "item_artifact_super_damage" ) )
	{
		other->super_time = 1;
		other->super_damage_finished = g_globalvars.time + 30;
	}
	activator = other;
	SUB_UseTargets(  );	// fire all targets / killtargets
}

/*QUAKED item_artifact_invulnerability (0 .5 .8) (-16 -16 -24) (16 16 32)
Player is invulnerable for 30 seconds
*/
void SP_item_artifact_invulnerability(  )
{
	if ( !CheckExistence(  ) )
	{
		dremove( self );
		return;
	}
	self->s.v.touch = ( func_t ) powerup_touch;

	trap_precache_model( "progs/invulner.mdl" );
	trap_precache_sound( "items/protect.wav" );
	trap_precache_sound( "items/protect2.wav" );
	trap_precache_sound( "items/protect3.wav" );
	self->s.v.noise = "items/protect.wav";
	setmodel( self, "progs/invulner.mdl" );
	self->s.v.netname = "Pentagram of Protection";
	self->s.v.effects = ( int ) self->s.v.effects | EF_RED;
	self->s.v.items = IT_INVULNERABILITY;
	setsize( self, -16, -16, -24, 16, 16, 32 );
	StartItem(  );
}

/*QUAKED item_artifact_envirosuit (0 .5 .8) (-16 -16 -24) (16 16 32)
Player takes no damage from water or slime for 30 seconds
*/
void SP_item_artifact_envirosuit(  )
{
	if ( !CheckExistence(  ) )
	{
		dremove( self );
		return;
	}
	self->s.v.touch = ( func_t ) powerup_touch;

	trap_precache_model( "progs/suit.mdl" );
	trap_precache_sound( "items/suit.wav" );
	trap_precache_sound( "items/suit2.wav" );
	self->s.v.noise = "items/suit.wav";
	setmodel( self, "progs/suit.mdl" );
	self->s.v.netname = "Biosuit";
	self->s.v.items = IT_SUIT;
	setsize( self, -16, -16, -24, 16, 16, 32 );
	StartItem(  );
}

/*QUAKED item_artifact_invisibility (0 .5 .8) (-16 -16 -24) (16 16 32)
Player is invisible for 30 seconds
*/
void SP_item_artifact_invisibility(  )
{
	if ( !CheckExistence(  ) )
	{
		dremove( self );
		return;
	}
	self->s.v.touch = ( func_t ) powerup_touch;

	trap_precache_model( "progs/invisibl.mdl" );
	trap_precache_sound( "items/inv1.wav" );
	trap_precache_sound( "items/inv2.wav" );
	trap_precache_sound( "items/inv3.wav" );
	self->s.v.noise = "items/inv1.wav";
	setmodel( self, "progs/invisibl.mdl" );
	self->s.v.netname = "Ring of Shadows";
	self->s.v.items = IT_INVISIBILITY;
	setsize( self, -16, -16, -24, 16, 16, 32 );
	StartItem(  );
}

/*QUAKED item_artifact_super_damage (0 .5 .8) (-16 -16 -24) (16 16 32)
The next attack from the player will do 4x damage
*/
void SP_item_artifact_super_damage(  )
{
	if ( !CheckExistence(  ) )
	{
		dremove( self );
		return;
	}
	self->s.v.touch = ( func_t ) powerup_touch;

	trap_precache_model( "progs/quaddama.mdl" );
	trap_precache_sound( "items/damage.wav" );
	trap_precache_sound( "items/damage2.wav" );
	trap_precache_sound( "items/damage3.wav" );
	self->s.v.noise = "items/damage.wav";
	setmodel( self, "progs/quaddama.mdl" );
	if ( deathmatch == 4 )
		self->s.v.netname = "OctaPower";
	else
		self->s.v.netname = "Quad Damage";
	self->s.v.items = IT_QUAD;
	self->s.v.effects = ( int ) self->s.v.effects | EF_BLUE;
	setsize( self, -16, -16, -24, 16, 16, 32 );
	StartItem(  );
}

/*
===============================================================================

PLAYER BACKPACKS

===============================================================================
*/
extern char *GrenadePrimeName[];
void BackpackTouch(  )
{
	float   best;
	int		gotgren1 = 0;
	int		gotgren2	= 0;

	if ( strneq( other->s.v.classname, "player" ) )
		return;
	if ( other->s.v.health <= 0 )
		return;

	if ( other->s.v.button0 )
		return;
	other->s.v.ammo_shells = other->s.v.ammo_shells + self->s.v.ammo_shells;
	other->s.v.ammo_nails = other->s.v.ammo_nails + self->s.v.ammo_nails;
	other->s.v.ammo_rockets = other->s.v.ammo_rockets + self->s.v.ammo_rockets;
	other->s.v.ammo_cells = other->s.v.ammo_cells + self->s.v.ammo_cells;
	if ( tf_data.gren2box & BP_TYPE_HEALTH )
		if ( self->ammo_medikit > 0 )
		{

			best = other->s.v.max_health - other->s.v.health;
			if ( best > 0 )
			{
				if ( best >= self->ammo_medikit )
				{
					T_Heal( other, self->ammo_medikit, 0 );
					self->ammo_medikit = 0;
				} else
				{
					T_Heal( other, best, 0 );
					self->ammo_medikit = self->ammo_medikit - best;
				}

			}
			if ( self->ammo_medikit > 0 && other->playerclass == PC_MEDIC )
				other->ammo_medikit = other->ammo_medikit + self->ammo_medikit;
		}
	if ( tf_data.gren2box & BP_TYPE_ARMOR )
		if ( other->s.v.armorvalue * other->s.v.armortype < self->s.v.armorvalue * self->s.v.armortype )
		{
			other->s.v.armorvalue = self->s.v.armorvalue;
			other->s.v.armortype = self->s.v.armortype;
		}
	if ( tf_data.gren2box & BP_GREN_BYTYPE )
	{
		if ( self->tp_grenades_1 == other->tp_grenades_1 )
		{
			other->no_grenades_1 = other->no_grenades_1 + self->no_grenades_1;
			gotgren1 = 1;
		}
		if ( self->tp_grenades_2 == other->tp_grenades_2 )
		{
			other->no_grenades_2 = other->no_grenades_2 + self->no_grenades_2;
			gotgren2 = 1;
		}
	} else if ( tf_data.gren2box & BP_GREN )
	{
		other->no_grenades_1 = other->no_grenades_1 + self->no_grenades_1;
		other->no_grenades_2 = other->no_grenades_2 + self->no_grenades_2;
		gotgren1 = 1;
		gotgren2 = 1;
	}
	if ( tf_data.gren2box & BP_TYPE_DETPACK )
	{
		if ( self->ammo_detpack > 0 && other->playerclass == PC_DEMOMAN )
			other->ammo_detpack += self->ammo_detpack;
	}


	bound_other_ammo( other );
	G_sprint( other, 0, "You get " );
	if ( self->s.v.ammo_shells )
	{
		G_sprint( other, 0, "%.0f shells ", self->s.v.ammo_shells );
	}
	if ( self->s.v.ammo_nails )
	{
		G_sprint( other, 0, "%.0f nails ", self->s.v.ammo_nails );
	}
	if ( self->s.v.ammo_rockets )
	{
		G_sprint( other, 0, "%.0f rockets ", self->s.v.ammo_rockets );
	}
	if ( self->s.v.ammo_cells )
	{
		G_sprint( other, 0, "%.0f cells ", self->s.v.ammo_cells );
	}
	if ( self->s.v.armorvalue && other->playerclass == PC_ENGINEER && other->s.v.ammo_cells < other->maxammo_cells )
	{
		G_sprint( other, 0, "%.0f metal ", self->s.v.armorvalue );
		other->s.v.ammo_cells += self->s.v.armorvalue;
		if ( other->s.v.ammo_cells > other->maxammo_cells )
			other->s.v.ammo_cells = other->maxammo_cells;
	}


	if ( self->no_grenades_1 && gotgren1 )
	{
		G_sprint( other, 0, "%d %s", self->no_grenades_1,
			  GrenadePrimeName[( self->tp_grenades_1 > 11 ) ? 0 : self->tp_grenades_1] );
		if ( self->no_grenades_1 != 1 )
			G_sprint( other, 0, "s" );
		G_sprint( other, 0, "  " );
	}
	if ( self->no_grenades_2 && gotgren2 )
	{
		G_sprint( other, 0, "%d %s", self->no_grenades_2,
			  GrenadePrimeName[( self->tp_grenades_2 > 11 ) ? 0 : self->tp_grenades_2] );

		if ( self->no_grenades_2 != 1 )
			G_sprint( other, 0, "s" );
		G_sprint( other, 0, "  " );
	}
	G_sprint( other, PRINT_LOW, "\n" );
// backpack touch sound
	sound( other, CHAN_ITEM, "weapons/lock4.wav", 1, ATTN_NORM );
	stuffcmd( other, "bf\n" );

	ent_remove( self );
	self = other;
	W_SetCurrentAmmo(  );
}

void DropBackpack(  )
{
	if ( tf_data.cb_prematch_time + 3 > g_globalvars.time )
		return;
	if ( !( self->s.v.ammo_shells + self->s.v.ammo_nails + self->s.v.ammo_rockets + self->s.v.ammo_cells ) )
		return;
	newmis = spawn(  );
	g_globalvars.newmis = EDICT_TO_PROG( newmis );
	VectorCopy( self->s.v.origin, newmis->s.v.origin );
	newmis->s.v.origin[2] -= 24;

	newmis->s.v.classname = "item_backpack";
	newmis->s.v.ammo_shells = self->s.v.ammo_shells;
	newmis->s.v.ammo_nails = self->s.v.ammo_nails;
	newmis->s.v.ammo_rockets = self->s.v.ammo_rockets;
	newmis->s.v.ammo_cells = self->s.v.ammo_cells;
	newmis->s.v.armorvalue = self->s.v.armorvalue;

	newmis->s.v.armortype = self->s.v.armortype;
	newmis->ammo_detpack = self->ammo_detpack;
	newmis->ammo_medikit = self->ammo_medikit;
	newmis->no_grenades_1 = self->no_grenades_1;
	newmis->no_grenades_2 = self->no_grenades_2;
	newmis->tp_grenades_1 = self->tp_grenades_1;
	newmis->tp_grenades_2 = self->tp_grenades_2;

	newmis->s.v.velocity[2] = 300;
	newmis->s.v.velocity[0] = -100 + ( g_random(  ) * 200 );
	newmis->s.v.velocity[1] = -100 + ( g_random(  ) * 200 );
	newmis->s.v.flags = FL_ITEM;
	newmis->s.v.solid = SOLID_TRIGGER;
	newmis->s.v.movetype = MOVETYPE_TOSS;
	setmodel( newmis, "progs/backpack.mdl" );
	setsize( newmis, -16, -16, 0, 16, 16, 56 );
	newmis->s.v.touch = ( func_t ) BackpackTouch;

	newmis->s.v.nextthink = g_globalvars.time + 120;	// remove after 2 minutes
	newmis->s.v.think = ( func_t ) SUB_Remove;
}
