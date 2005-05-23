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
 *  $Id: tfortmap.c,v 1.18 2005-05-23 18:54:02 AngelD Exp $
 */
#include "g_local.h"

static int     item_list_bit;
char   *team_menu_string = NULL;
int     CTF_Map;
void UpdateAbbreviations( gedict_t * Goal )
{

	if ( !Goal->has_abbreviated )
	{
/*
//REMOVE !!!
  if (Goal->g_a && !Goal->goal_activation) 
   Goal->goal_activation = Goal->g_a;
  if (Goal->g_e && !Goal->goal_effects) 
   Goal->goal_effects = Goal->g_e;
  if (Goal->g_e && !Goal->goal_effects) 
   Goal->goal_effects = Goal->g_e;
  if (Goal->h_i_g && !Goal->has_item_from_group) 
   Goal->has_item_from_group = Goal->h_i_g;
  if (Goal->hn_i_g && !Goal->hasnt_item_from_group) 
   Goal->hasnt_item_from_group = Goal->hn_i_g;
  if (Goal->r_i_g && !Goal->remove_item_group) 
   Goal->remove_item_group = Goal->r_i_g;
  if (Goal->a_s && !Goal->s.v.ammo_shells) 
   Goal->s.v.ammo_shells = Goal->a_s;
  if (Goal->a_n && !Goal->s.v.ammo_nails) 
   Goal->s.v.ammo_nails = Goal->a_n;
  if (Goal->a_r && !Goal->s.v.ammo_rockets) 
   Goal->s.v.ammo_rockets = Goal->a_r;
  if (Goal->a_c && !Goal->s.v.ammo_cells) 
   Goal->s.v.ammo_cells = Goal->a_c;
  if (Goal->rv_s_h && !Goal->remove_spawngroup) 
   Goal->remove_spawngroup = Goal->rv_s_h;
  if (Goal->rs_s_h && !Goal->restore_spawngroup) 
   Goal->restore_spawngroup = Goal->rs_s_h;
  if (Goal->rv_gr && !Goal->remove_group_no) 
   Goal->remove_group_no = Goal->rv_gr;
  if (Goal->rs_gr && !Goal->restore_group_no) 
   Goal->restore_group_no = Goal->rs_gr;
  if (Goal->rv_g && !Goal->remove_goal_no) 
   Goal->remove_goal_no = Goal->rv_g;
  if (Goal->rs_g && !Goal->restore_goal_no) 
   Goal->restore_goal_no = Goal->rs_g;
  if (Goal->t_s_h ) 
   Goal->team_str_home = Goal->t_s_h;
  if (Goal->t_s_m ) 
   Goal->team_str_moved = Goal->t_s_m;
  if (Goal->t_s_c ) 
   Goal->team_str_carried = Goal->t_s_c;
  if (Goal->n_s_h ) 
   Goal->non_team_str_home = Goal->n_s_h;
  if (Goal->n_s_m ) 
   Goal->non_team_str_moved = Goal->n_s_m;
  if (Goal->n_s_c ) 
   Goal->non_team_str_carried = Goal->n_s_c;
  if (Goal->b_b ) 
   Goal->broadcast = Goal->b_b;
  if (Goal->b_t ) 
   Goal->team_broadcast = Goal->b_t;
  if (Goal->b_n ) 
   Goal->non_team_broadcast = Goal->b_n;
  if (Goal->b_o ) 
   Goal->owners_team_broadcast = Goal->b_o;
  if (Goal->n_b ) 
   Goal->netname_broadcast = Goal->n_b;
  if (Goal->n_t ) 
   Goal->netname_team_broadcast = Goal->n_t;
  if (Goal->n_n ) 
   Goal->netname_non_team_broadcast = Goal->n_n;
  if (Goal->n_o ) 
   Goal->netname_owners_team_broadcast = Goal->n_o;
  if (Goal->d_t ) 
   Goal->team_drop = Goal->d_t;
  if (Goal->d_n ) 
   Goal->non_team_drop = Goal->d_n;
  if (Goal->d_n_t ) 
   Goal->netname_team_drop = Goal->d_n_t;
  if (Goal->d_n_n ) 
   Goal->netname_non_team_drop = Goal->d_n_n;
   */
		char    st[10];

		if ( !tf_data.flagem_checked )
		{

			GetSVInfokeyString( "fe", "flag_emu", st, sizeof( st ), "off" );
			if ( !strcmp( st, "on" ) )
				tf_data.toggleflags |= TFLAG_FLAG_EMULATION;
			GetSVInfokeyString( "ws", "use_standard", st, sizeof( st ), "off" );

			if ( !strcmp( st, "on" ) )
				tf_data.toggleflags |= TFLAG_USE_WAR_STD;
			tf_data.flagem_checked = 1;
		}
		if ( ( tf_data.toggleflags & TFLAG_FLAG_EMULATION ) && !( tf_data.toggleflags & TFLAG_USE_WAR_STD ) )
		{
			if ( streq( Goal->mdl, "progs/b_s_key.mdl" ) || streq( Goal->mdl, "progs/m_s_key.mdl" )
			     || streq( Goal->mdl, "progs/w_s_key.mdl" ) )
			{
				Goal->mdl = "progs/tf_flag.mdl";
				Goal->s.v.skin = 1;
			} else
			{
				if ( streq( Goal->mdl, "progs/b_g_key.mdl" ) || streq( Goal->mdl, "progs/m_g_key.mdl" )
				     || streq( Goal->mdl, "progs/w_g_key.mdl" ) )
				{
					Goal->mdl = "progs/tf_flag.mdl";
					Goal->s.v.skin = 2;
				}
			}
		}
		if ( tf_data.toggleflags & TFLAG_USE_WAR_STD )
		{
			if ( streq( Goal->mdl, "progs/b_s_key.mdl" ) || streq( Goal->mdl, "progs/m_s_key.mdl" )
			     || streq( Goal->mdl, "progs/w_s_key.mdl" ) )
			{
				Goal->mdl = "progs/tf_stan.mdl";
				Goal->s.v.skin = 1;
			} else
			{
				if ( streq( Goal->mdl, "progs/b_g_key.mdl" ) || streq( Goal->mdl, "progs/m_g_key.mdl" )
				     || streq( Goal->mdl, "progs/w_g_key.mdl" ) )
				{
					Goal->mdl = "progs/tf_stan.mdl";
					Goal->s.v.skin = 2;
				} else
				{
					if ( streq( Goal->mdl, "progs/tf_flag.mdl" ) )
						Goal->mdl = "progs/tf_stan.mdl";
				}
			}
		}
		Goal->has_abbreviated = 1;
	}
}


void TF_PlaceItem(  )
{
	float   oldz;

	self->s.v.flags = FL_ITEM;
	SetVector( self->s.v.velocity, 0, 0, 0 );
	if ( self->goal_activation & TFGI_DROP_TO_FLOOR )
	{
		self->s.v.movetype = MOVETYPE_TOSS;
		self->s.v.origin[2] = self->s.v.origin[2] + 6;
		oldz = self->s.v.origin[2];
		if ( !droptofloor( self ) )
		{
			G_dprintf( "GoalItem fell out of level at '%f %f %f'\n",
				  self->s.v.origin[0], self->s.v.origin[1], self->s.v.origin[2] );
			dremove( self );
			return;
		}
	}
	self->s.v.movetype = MOVETYPE_NONE;
	VectorCopy( self->s.v.origin, self->s.v.oldorigin );
	if ( self->goal_activation & TFGI_ITEMGLOWS )
		self->s.v.effects = ( int ) self->s.v.effects | EF_DIMLIGHT;
	if ( !item_list_bit )
		item_list_bit = 1;
	self->item_list = item_list_bit;
	item_list_bit *= 2;
}

void TF_StartItem(  )
{
	UpdateAbbreviations( self );
	self->s.v.nextthink = g_globalvars.time + 0.2;
	self->s.v.think = ( func_t ) TF_PlaceItem;
	if ( self->goal_state == TFGS_REMOVED )
		RemoveGoal( self );
}

void TF_PlaceGoal(  )
{
	float   oldz;

	if ( strneq( self->s.v.classname, "info_tfgoal_timer" ) )
	{
		if ( self->goal_activation & TFGA_TOUCH )
			self->s.v.touch = ( func_t ) tfgoal_touch;
	} else
	{
		self->s.v.think = ( func_t ) tfgoal_timer_tick;
		self->s.v.nextthink = g_globalvars.time + self->search_time;
		self->s.v.classname = "info_tfgoal";
	}
	if ( self->goal_activation & TFGI_DROP_TO_FLOOR )
	{
		self->s.v.movetype = MOVETYPE_TOSS;
		self->s.v.origin[2] = self->s.v.origin[2] + 6;
		oldz = self->s.v.origin[2];
		if ( !droptofloor( self ) )
		{
			G_dprintf( "Goal fell out of level at '%f %f %f'\n",
				  self->s.v.origin[0], self->s.v.origin[1], self->s.v.origin[2] );

			dremove( self );
			return;
		}
	}
	self->s.v.flags = FL_ITEM;
	self->s.v.movetype = MOVETYPE_NONE;
	SetVector( self->s.v.velocity, 0, 0, 0 );
	VectorCopy( self->s.v.origin, self->s.v.oldorigin );
}

void TF_StartGoal(  )
{
	UpdateAbbreviations( self );
	self->s.v.nextthink = g_globalvars.time + 0.2;
	self->s.v.think = ( func_t ) TF_PlaceGoal;
	self->s.v.use = ( func_t ) info_tfgoal_use;
	if ( self->goal_state == TFGS_REMOVED )
		RemoveGoal( self );
}

int CheckExistence(  )
{
	UpdateAbbreviations( self );
	skill = trap_cvar( "skill" );
	if ( self->ex_skill_min == -1 && skill < 0 )
		return 0;
	else
	{
		if ( self->ex_skill_max == -1 && skill > 0 )
			return 0;
	}
	if ( self->ex_skill_min && self->ex_skill_min != -1 && skill < self->ex_skill_min )
		return 0;
	else
	{
		if ( self->ex_skill_max && self->ex_skill_max != -1 && skill > self->ex_skill_max )
			return 0;
	}
	return 1;
}

void SP_info_tfdetect(  )
{
	UpdateAbbreviations( self );
}

extern const char *team_spawn_str[5];
void SP_info_player_teamspawn(  )
{

	if ( !CheckExistence(  ) )
	{
		dremove( self );
		return;
	}
	if ( self->team_no <= 0 || self->team_no >= 5 )
	{
		G_Error( "error: bad team_no associated with info_player_teamspawn\n" );
		dremove( self );
		return;
	}
	if ( number_of_teams < self->team_no )
		number_of_teams = self->team_no;
	self->team_str_home = (char*)team_spawn_str[self->team_no];
}

void SP_i_p_t(  )
{
	self->s.v.classname = "info_player_teamspawn";
	SP_info_player_teamspawn(  );
}

void SP_info_tfgoal(  )
{
	if ( !CheckExistence(  ) )
	{
		dremove( self );
		return;
	}
	if ( self->mdl )
	{
		trap_precache_model( self->mdl );
		setmodel( self, self->mdl );
	}
	if ( self->s.v.noise )
	{
		trap_precache_sound( self->s.v.noise );
	}
	trap_precache_sound( "items/protect.wav" );
	trap_precache_sound( "items/protect2.wav" );
	trap_precache_sound( "items/protect3.wav" );
	trap_precache_sound( "items/suit.wav" );
	trap_precache_sound( "items/suit2.wav" );
	trap_precache_sound( "items/inv1.wav" );
	trap_precache_sound( "items/inv2.wav" );
	trap_precache_sound( "items/inv3.wav" );
	trap_precache_sound( "items/damage.wav" );
	trap_precache_sound( "items/damage2.wav" );
	trap_precache_sound( "items/damage3.wav" );
	self->s.v.solid = SOLID_TRIGGER;
	if ( !self->goal_state )
		self->goal_state = TFGS_INACTIVE;
	if ( VectorCompareF( self->goal_min, 0, 0, 0 ) )
		SetVector( self->goal_min, -16, -16, -24 );
	if ( VectorCompareF( self->goal_max, 0, 0, 0 ) )
		SetVector( self->goal_max, 16, 16, 32 );
	setsize( self, PASSVEC3( self->goal_min ), PASSVEC3( self->goal_max ) );
	TF_StartGoal(  );
}

void SP_i_t_g(  )
{
	self->s.v.classname = "info_tfgoal";
	SP_info_tfgoal(  );
}

void SP_info_tfgoal_timer(  )
{
	if ( !CheckExistence(  ) )
	{
		dremove( self );
		return;
	}
	if ( self->mdl )
	{
		trap_precache_model( self->mdl );
		setmodel( self, self->mdl );
	}
	if ( self->s.v.noise )
	{
		trap_precache_sound( self->s.v.noise );
	}
	if ( self->search_time <= 0 )
	{
		G_dprintf( "Timer Goal created with no specified time.\n" );
		dremove( self );
	}
	self->s.v.solid = SOLID_NOT;
	if ( !self->goal_state )
		self->goal_state = TFGS_INACTIVE;
	if ( VectorCompareF( self->goal_min, 0, 0, 0 ) )
		SetVector( self->goal_min, -16, -16, -24 );
	if ( VectorCompareF( self->goal_max, 0, 0, 0 ) )
		SetVector( self->goal_max, 16, 16, 32 );
	setsize( self, PASSVEC3( self->goal_min ), PASSVEC3( self->goal_max ) );
	TF_StartGoal(  );
}

void SP_i_t_t(  )
{
	self->s.v.classname = "info_tfgoal_timer";
	SP_info_tfgoal_timer(  );
}

void SP_item_tfgoal(  )
{
	if ( !CheckExistence(  ) )
	{
		dremove( self );
		return;
	}
	if ( self->mdl )
	{
		trap_precache_model( self->mdl );
		setmodel( self, self->mdl );
	} else
	{
		self->mdl = "";
		setmodel( self, "" );
	}
	trap_precache_sound( "items/itembk2.wav" );
	if ( self->s.v.noise )
	{
		trap_precache_sound( self->s.v.noise );
	}
	self->s.v.touch = ( func_t ) item_tfgoal_touch;
	if ( !self->goal_state )
		self->goal_state = TFGS_INACTIVE;
	if ( self->goal_activation & TFGI_SOLID )
		self->s.v.solid = SOLID_BBOX;
	else
		self->s.v.solid = SOLID_TRIGGER;
	setorigin( self, PASSVEC3( self->s.v.origin ) );
	if ( !self->s.v.netname )
		self->s.v.netname = "goalitem";
	if ( self->pausetime <= 0 )
		self->pausetime = 60;
	if ( self->delay && !self->pausetime )
		self->pausetime = self->delay;
	if ( VectorCompareF( self->goal_min, 0, 0, 0 ) )
		SetVector( self->goal_min, -16, -16, -24 );
	if ( VectorCompareF( self->goal_max, 0, 0, 0 ) )
		SetVector( self->goal_max, 16, 16, 32 );
	setsize( self, PASSVEC3( self->goal_min ), PASSVEC3( self->goal_max ) );
	TF_StartItem(  );
}

void ParseTFDetect( gedict_t * AD )
{
	if ( AD->team_broadcast )
		team_menu_string = AD->team_broadcast;
	if ( AD->s.v.message )
		localcmd( AD->s.v.message );
	trap_cvar_set( "sv_maxspeed", "500" );

	teamlives[1] = AD->s.v.ammo_shells;
	teamlives[2] = AD->s.v.ammo_nails;
	teamlives[3] = AD->s.v.ammo_rockets;
	teamlives[4] = AD->s.v.ammo_cells;

	if ( !teamlives[1] )
		teamlives[1] = -1;
	if ( !teamlives[2] )
		teamlives[2] = -1;
	if ( !teamlives[3] )
		teamlives[3] = -1;
	if ( !teamlives[4] )
		teamlives[4] = -1;
	if ( AD->hook_out == 1 )
		tf_data.allow_hook = 0;
	teammaxplayers[1] = AD->ammo_medikit;
	teammaxplayers[2] = AD->ammo_detpack;
	teammaxplayers[3] = AD->maxammo_medikit;
	teammaxplayers[4] = AD->maxammo_detpack;
	if ( !teammaxplayers[1] )
		teammaxplayers[1] = 100;
	if ( !teammaxplayers[2] )
		teammaxplayers[2] = 100;
	if ( !teammaxplayers[3] )
		teammaxplayers[3] = 100;
	if ( !teammaxplayers[4] )
		teammaxplayers[4] = 100;

	illegalclasses[0] = AD->playerclass;
	illegalclasses[1] = AD->maxammo_shells;
	illegalclasses[2] = AD->maxammo_nails;
	illegalclasses[3] = AD->maxammo_rockets;
	illegalclasses[4] = AD->maxammo_cells;
	civilianteams = 0;
	if ( illegalclasses[1] == -1 )
	{
		illegalclasses[1] = 0;
		civilianteams = civilianteams | 1;
	}
	if ( illegalclasses[2] == -1 )
	{
		illegalclasses[2] = 0;
		civilianteams = civilianteams | 2;
	}
	if ( illegalclasses[3] == -1 )
	{
		illegalclasses[3] = 0;
		civilianteams = civilianteams | 4;
	}
	if ( illegalclasses[4] == -1 )
	{
		illegalclasses[4] = 0;
		civilianteams = civilianteams | 8;
	}

	teams_allied = AD->team_no;
}

gedict_t *Finditem( int ino )
{
	gedict_t *tg;

	tg = trap_find( world, FOFS( s.v.classname ), "item_tfgoal" );
	while ( tg )
	{
		if ( tg->goal_no == ino )
			return tg;
		tg = trap_find( tg, FOFS( s.v.classname ), "item_tfgoal" );
	}
	G_dprintf( "Could not find an item with a goal_no of %d.\n", ino );
	return world;
}

gedict_t *Findgoal( int gno )
{
	gedict_t *tg;

	tg = trap_find( world, FOFS( s.v.classname ), "info_tfgoal" );
	while ( tg )
	{
		if ( tg->goal_no == gno )
			return tg;
		tg = trap_find( tg, FOFS( s.v.classname ), "info_tfgoal" );
	}
	G_dprintf( "Could not find a goal with a goal_no of %d.\n", gno );
	return NULL;
}

gedict_t *Findteamspawn( int gno )
{
	gedict_t *tg;

	tg = trap_find( world, FOFS( s.v.classname ), "info_player_teamspawn" );
	while ( tg )
	{
		if ( tg->goal_no == gno )
			return tg;
		tg = trap_find( tg, FOFS( s.v.classname ), "info_player_teamspawn" );
	}
	G_dprintf( "Could not find a Teamspawn with a goal_no of %d.\n", gno );
	return NULL;
}

void InactivateGoal( gedict_t * Goal )
{
	if ( Goal->goal_state == TFGS_ACTIVE )
	{
		if ( !Goal->search_time )
		{
			if ( ( Goal->goal_activation & TFGI_SOLID ) && streq( Goal->s.v.classname, "item_tfgoal" ) )
				Goal->s.v.solid = SOLID_BBOX;
			else
				Goal->s.v.solid = SOLID_TRIGGER;
		}
		Goal->goal_state = TFGS_INACTIVE;
		if ( Goal->mdl )
			setmodel( Goal, Goal->mdl );
	}
}

void RestoreGoal( gedict_t * Goal )
{
	if ( Goal->goal_state == TFGS_REMOVED )
	{
		if ( !Goal->search_time )
		{
			if ( ( Goal->goal_activation & TFGI_SOLID ) && streq( Goal->s.v.classname, "item_tfgoal" ) )
				Goal->s.v.solid = SOLID_BBOX;
			else
				Goal->s.v.solid = SOLID_TRIGGER;
		} else
			Goal->s.v.nextthink = g_globalvars.time + Goal->search_time;
		Goal->goal_state = TFGS_INACTIVE;
		if ( Goal->mdl )
			setmodel( Goal, Goal->mdl );
	}
}

void RemoveGoal( gedict_t * Goal )
{
	Goal->s.v.solid = SOLID_NOT;
	Goal->goal_state = TFGS_REMOVED;
	if ( Goal->mdl )
		setmodel( Goal, "" );
}

int IsAffectedBy( gedict_t * Goal, gedict_t * Player, gedict_t * AP )
{
	float   genv;
	vec3_t  tmp;

	if ( !Player->playerclass )
		return 0;
	if ( Goal->goal_effects & TFGE_SAME_ENVIRONMENT )
	{
		genv = trap_pointcontents( PASSVEC3( Goal->s.v.origin ) );
		if ( trap_pointcontents( PASSVEC3( Player->s.v.origin ) ) != genv )
			return 0;
	}
	if ( Goal->t_length )
	{
		VectorSubtract( Goal->s.v.origin, Player->s.v.origin, tmp );
		if ( vlen( tmp ) <= Goal->t_length )
		{
			if ( Goal->goal_effects & TFGE_WALL )
			{
				traceline( PASSVEC3( Goal->s.v.origin ), PASSVEC3( Player->s.v.origin ), 1, Goal );
				if ( g_globalvars.trace_fraction == 1 )
					return 1;
			} else
				return 1;
		}
	}
	if ( strneq( Goal->s.v.classname, "info_tfgoal_timer" ) )
	{
		if ( ( Goal->goal_effects & TFGE_AP ) && Player == AP )
			return 1;
		if ( ( Goal->goal_effects & TFGE_AP_TEAM ) && AP->team_no == Player->team_no )
			return 1;
		if ( ( Goal->goal_effects & TFGE_NOT_AP_TEAM ) && AP->team_no != Player->team_no )
			return 1;
		if ( ( Goal->goal_effects & TFGE_NOT_AP ) && Player != AP )
			return 1;
	}
	if ( Goal->maxammo_shells && Player->team_no == Goal->maxammo_shells )
		return 1;
	if ( Goal->maxammo_nails && Player->team_no != Goal->maxammo_shells )
		return 1;
	return 0;
}

void Apply_Results( gedict_t * Goal, gedict_t * Player, gedict_t * AP, float addb )
{
	gedict_t *oldself;
	gedict_t *te;
	gedict_t *oldte;

//      float   tc;
	stuffcmd( Player, "bf\n" );
	if ( streq( Goal->s.v.classname, "item_tfgoal" ) )
		Player->item_list = Player->item_list | Goal->item_list;
	if ( Player == AP )
	{
		if ( Goal->count > 0 )
		{
			if ( Player->team_no > 0 )
			{
				TeamFortress_TeamIncreaseScore( Player->team_no, Goal->count );
				TeamFortress_TeamShowScores( 2 );
			}
		}
	}
	if ( addb )
	{
		if ( Player->s.v.health > 0 )
		{
			if ( Goal->s.v.health > 0 )
				T_Heal( Player, Goal->s.v.health, 0 );
			if ( Goal->s.v.health < 0 )
			{
				if ( 0 - Player->s.v.health > Goal->s.v.health )
					TF_T_Damage( Player, Goal, Goal, Player->s.v.health + 1, 1, 0 );
				else
					TF_T_Damage( Player, Goal, Goal, 0 - Goal->s.v.health, 1, 0 );
			}
		}
		if ( Player->s.v.health > 0 )
		{
			if ( Goal->s.v.armortype > 0 )
				Player->s.v.armortype = Goal->s.v.armortype;
			else
			{
				if ( Goal->s.v.armorvalue > 0 )
					Player->s.v.armortype = Player->armor_allowed;
			}

			Player->s.v.armorvalue = Player->s.v.armorvalue + Goal->s.v.armorvalue;
			if ( Goal->armorclass > 0 )
				Player->armorclass = Goal->armorclass;
			Player->s.v.ammo_shells = Player->s.v.ammo_shells + Goal->s.v.ammo_shells;
			Player->s.v.ammo_nails = Player->s.v.ammo_nails + Goal->s.v.ammo_nails;
			Player->s.v.ammo_rockets = Player->s.v.ammo_rockets + Goal->s.v.ammo_rockets;
			Player->s.v.ammo_cells = Player->s.v.ammo_cells + Goal->s.v.ammo_cells;
			Player->ammo_medikit = Player->ammo_medikit + Goal->ammo_medikit;
			Player->ammo_detpack = Player->ammo_detpack + Goal->ammo_detpack;
			Player->no_grenades_1 = Player->no_grenades_1 + Goal->no_grenades_1;
			Player->no_grenades_2 = Player->no_grenades_2 + Goal->no_grenades_2;
			bound_other_ammo( Player );
			if ( Player->tfstate & TFSTATE_GRENPRIMED )
			{
				te = trap_find( world, FOFS( s.v.classname ), "primer" );
				while ( te )
				{
					if ( te->s.v.owner == EDICT_TO_PROG( Player ) )
					{
						if ( te->s.v.impulse == 151 && Goal->no_grenades_2 < 0 )
						{
							Player->tfstate =
							    Player->tfstate - ( Player->tfstate & TFSTATE_GRENPRIMED );
							Player->tfstate =
							    Player->tfstate -
							    ( Player->tfstate & TFSTATE_GRENTHROWING );
							dremove( te );
						} else
						{
							if ( te->s.v.impulse == 150 && Goal->no_grenades_1 < 0 )
							{
								Player->tfstate =
								    Player->tfstate -
								    ( Player->tfstate & TFSTATE_GRENPRIMED );
								Player->tfstate =
								    Player->tfstate -
								    ( Player->tfstate & TFSTATE_GRENTHROWING );
								dremove( te );
							}
						}
						te = NULL;
					} else
						te = trap_find( te, FOFS( s.v.classname ), "primer" );
				}
			}
			if ( !tf_data.disable_powerups && ( Goal->invincible_finished > 0 ) )
			{
				Player->s.v.items = ( int ) Player->s.v.items | IT_INVULNERABILITY;
				Player->invincible_time = 1;
				Player->invincible_finished = g_globalvars.time + Goal->invincible_finished;
				if ( streq( Goal->s.v.classname, "item_tfgoal" ) )
				{
					Player->tfstate = Player->tfstate | TFSTATE_INVINCIBLE;
					Player->invincible_finished = g_globalvars.time + 666;
				}
			}
			if ( !tf_data.disable_powerups && ( Goal->invisible_finished > 0 ) )
			{
				Player->s.v.items = ( int ) Player->s.v.items | IT_INVISIBILITY;
				Player->invisible_time = 1;
				Player->invisible_finished = g_globalvars.time + Goal->invisible_finished;
				if ( streq( Goal->s.v.classname, "item_tfgoal" ) )
				{
					Player->tfstate = Player->tfstate | TFSTATE_INVISIBLE;
					Player->invisible_finished = g_globalvars.time + 666;
				}
			}
			if ( !tf_data.disable_powerups && ( Goal->super_damage_finished > 0 ) )
			{
				Player->s.v.items = ( int ) Player->s.v.items | IT_QUAD;
				Player->super_time = 1;
				Player->super_damage_finished = g_globalvars.time + Goal->super_damage_finished;
				if ( streq( Goal->s.v.classname, "item_tfgoal" ) )
				{
					Player->tfstate = Player->tfstate | TFSTATE_QUAD;
					Player->super_damage_finished = g_globalvars.time + 666;
				}
			}
			if ( !tf_data.disable_powerups && ( Goal->radsuit_finished > 0 ) )
			{
				Player->s.v.items = ( int ) Player->s.v.items | IT_SUIT;
				Player->rad_time = 1;
				Player->radsuit_finished = g_globalvars.time + Goal->radsuit_finished;
				if ( streq( Goal->s.v.classname, "item_tfgoal" ) )
				{
					Player->tfstate = Player->tfstate | TFSTATE_RADSUIT;
					Player->radsuit_finished = g_globalvars.time + 666;
				}
			}
		}
		Player->lives = Player->lives + Goal->lives;
		if ( Goal->s.v.frags )
		{
			if ( Goal->goal_effects == TFGE_AP || !( tf_data.toggleflags & TFLAG_FULLTEAMSCORE ) )
				TF_AddFrags( Player, Goal->s.v.frags );
		}
		oldself = self;
		self = Player;
		TeamFortress_CheckClassStats(  );
		W_SetCurrentAmmo(  );
		self = oldself;
	}
	if ( Player->playerclass == PC_SPY && ( Goal->goal_result & TFGR_REMOVE_DISGUISE ) )
	{
		self->immune_to_check = g_globalvars.time + tf_data.cheat_pause;	//10;
		Spy_RemoveDisguise( Player );
	}
	if ( Goal->s.v.items && strneq( Goal->s.v.classname, "item_tfgoal" ) )
	{
		te = Finditem( Goal->s.v.items );
		if ( te != world && te != Goal )
			tfgoalitem_GiveToPlayer( te, Player, Goal );
	}
	if ( Goal->axhitme )
	{
		te = Finditem( Goal->axhitme );
		if ( te->s.v.owner == EDICT_TO_PROG( Player ) )
			tfgoalitem_RemoveFromPlayer( te, Player, 1 );
	}
	if ( Goal->remove_item_group )
	{
		te = trap_find( world, FOFS( s.v.classname ), "item_tfgoal" );
		while ( te )
		{
			if ( te->group_no == Goal->remove_item_group && te->s.v.owner == EDICT_TO_PROG( AP ) )
			{
				oldte = te;
				te = trap_find( te, FOFS( s.v.classname ), "item_tfgoal" );
				tfgoalitem_RemoveFromPlayer( oldte, Player, 1 );
			} else
				te = trap_find( te, FOFS( s.v.classname ), "item_tfgoal" );
		}
	}
	if ( Goal->display_item_status1 )
	{
		te = Finditem( Goal->display_item_status1 );
		if ( te != world )
			DisplayItemStatus( Goal, Player, te );
		else
			G_sprint( Player, 2, "Item is missing.\n" );
	}
	if ( Goal->display_item_status2 )
	{
		te = Finditem( Goal->display_item_status2 );
		if ( te != world )
			DisplayItemStatus( Goal, Player, te );
		else
			G_sprint( Player, 2, "Item is missing.\n" );
	}
	if ( Goal->display_item_status3 )
	{
		te = Finditem( Goal->display_item_status3 );
		if ( te != world )
			DisplayItemStatus( Goal, Player, te );
		else
			G_sprint( Player, 2, "Item is missing.\n" );
	}
	if ( Goal->display_item_status4 )
	{
		te = Finditem( Goal->display_item_status4 );
		if ( te != world )
			DisplayItemStatus( Goal, Player, te );
		else
			G_sprint( Player, 2, "Item is missing.\n" );
	}
	if ( Goal->goal_result & TFGR_FORCE_RESPAWN )
		ForceRespawn( Player );
}

void RemoveResults( gedict_t * Goal, gedict_t * Player )
{
	gedict_t *oldself;
	gedict_t *te;
	float   puinvin;
	float   puinvis;
	float   puquad;
	float   purad;

	if ( streq( Goal->s.v.classname, "item_tfgoal" ) )
	{
		if ( !( Player->item_list & Goal->item_list ) )
			return;
		if ( Goal->goal_activation & TFGI_DONTREMOVERES )
			return;
		Player->item_list = Player->item_list - ( Player->item_list & Goal->item_list );
	}
	if ( Goal->s.v.health > 0 )
		TF_T_Damage( Player, Goal, Goal, Goal->s.v.health, 1, 0 );
	if ( Goal->s.v.health < 0 )
		T_Heal( Player, 0 - Goal->s.v.health, 0 );
	Player->lives = Player->lives - Goal->lives;
	Player->s.v.armortype = Player->s.v.armortype - Goal->s.v.armortype;
	Player->s.v.armorvalue = Player->s.v.armorvalue - Goal->s.v.armorvalue;
	Player->armorclass = Player->armorclass - ( Player->armorclass & Goal->armorclass );
	if ( Goal->s.v.frags )
	{
		if ( Goal->goal_effects == TFGE_AP || !( tf_data.toggleflags & 2048 ) )
			TF_AddFrags( Player, Goal->s.v.frags );
	}
	Player->s.v.ammo_shells = Player->s.v.ammo_shells - Goal->s.v.ammo_shells;
	Player->s.v.ammo_nails = Player->s.v.ammo_nails - Goal->s.v.ammo_nails;
	Player->s.v.ammo_rockets = Player->s.v.ammo_rockets - Goal->s.v.ammo_rockets;
	Player->s.v.ammo_cells = Player->s.v.ammo_cells - Goal->s.v.ammo_cells;
	Player->ammo_medikit = Player->ammo_medikit - Goal->ammo_medikit;
	Player->ammo_detpack = Player->ammo_detpack - Goal->ammo_detpack;
	Player->no_grenades_1 = Player->no_grenades_1 - Goal->no_grenades_1;
	Player->no_grenades_2 = Player->no_grenades_2 - Goal->no_grenades_2;
	bound_other_ammo( Player );
	if ( Player->tfstate & TFSTATE_GRENPRIMED )
	{
		te = trap_find( world, FOFS( s.v.classname ), "primer" );
		while ( te )
		{
			if ( te->s.v.owner == EDICT_TO_PROG( Player ) )
			{
				if ( te->s.v.impulse == 151 && Goal->no_grenades_2 < 0 )
				{
					Player->tfstate = Player->tfstate - ( Player->tfstate & TFSTATE_GRENPRIMED );
					Player->tfstate = Player->tfstate - ( Player->tfstate & TFSTATE_GRENTHROWING );
					dremove( te );
				} else
				{
					if ( te->s.v.impulse == 150 && Goal->no_grenades_1 < 0 )
					{
						Player->tfstate =
						    Player->tfstate - ( Player->tfstate & TFSTATE_GRENPRIMED );
						Player->tfstate =
						    Player->tfstate - ( Player->tfstate & TFSTATE_GRENTHROWING );
						dremove( te );
					}
				}
				te = NULL;
			} else
				te = trap_find( te, FOFS( s.v.classname ), "primer" );
		}
	}
	puinvin = 0;
	puinvis = 0;
	puquad = 0;
	purad = 0;
	te = trap_find( world, FOFS( s.v.classname ), "item_tfgoal" );
	while ( te )
	{
		if ( te->s.v.owner == EDICT_TO_PROG( Player ) && te != Goal )
		{
			if ( te->invincible_finished > 0 )
				puinvin = 1;
			if ( te->invisible_finished > 0 )
				puinvis = 1;
			if ( te->super_damage_finished > 0 )
				puquad = 1;
			if ( te->radsuit_finished > 0 )
				purad = 1;
		}
		te = trap_find( te, FOFS( s.v.classname ), "item_tfgoal" );
	}
	if ( !tf_data.disable_powerups && Goal->invincible_finished > 0 && !puinvin )
	{
		Player->tfstate = Player->tfstate - ( Player->tfstate & TFSTATE_INVINCIBLE );
		Player->s.v.items = ( int ) Player->s.v.items | IT_INVULNERABILITY;
		Player->invincible_time = 1;
		Player->invincible_finished = g_globalvars.time + Goal->invincible_finished;
	}
	if ( !tf_data.disable_powerups && Goal->invisible_finished > 0 && !puinvis )
	{
		Player->tfstate = Player->tfstate - ( Player->tfstate & TFSTATE_INVISIBLE );
		Player->s.v.items = ( int ) Player->s.v.items | IT_INVISIBILITY;
		Player->invisible_time = 1;
		Player->invisible_finished = g_globalvars.time + Goal->invisible_finished;
	}
	if ( !tf_data.disable_powerups && Goal->super_damage_finished > 0 && !puquad )
	{
		Player->tfstate = Player->tfstate - ( Player->tfstate & TFSTATE_QUAD );
		Player->s.v.items = ( int ) Player->s.v.items | IT_QUAD;
		Player->super_time = 1;
		Player->super_damage_finished = g_globalvars.time + Goal->super_damage_finished;
	}
	if ( !tf_data.disable_powerups && Goal->radsuit_finished > 0 && !purad )
	{
		Player->tfstate = Player->tfstate - ( Player->tfstate & TFSTATE_RADSUIT );
		Player->s.v.items = ( int ) Player->s.v.items | IT_SUIT;
		Player->rad_time = 1;
		Player->radsuit_finished = g_globalvars.time + Goal->radsuit_finished;
	}
	oldself = self;
	self = Player;
	TeamFortress_CheckClassStats(  );
	W_SetCurrentAmmo(  );
	self = oldself;
}

int APMeetsCriteria( gedict_t * Goal, gedict_t * AP )
{
	float   gotone;
	gedict_t *te;

	if ( AP && AP != world )
	{
		if ( Goal->team_no )
		{
			if ( Goal->team_no != AP->team_no )
				return 0;
		}
		if ( Goal->playerclass )
		{
			if ( Goal->playerclass != AP->playerclass )
				return 0;
		}
		if ( Goal->items_allowed )
		{
			te = Finditem( Goal->items_allowed );
			if ( te->s.v.owner != EDICT_TO_PROG( AP ) )
				return 0;
		}
	}
	if ( Goal->if_goal_is_active )
	{
		te = Findgoal( Goal->if_goal_is_active );
		if ( te && te->goal_state != 1 )
			return 0;
	}
	if ( Goal->if_goal_is_inactive )
	{
		te = Findgoal( Goal->if_goal_is_inactive );
		if ( te && te->goal_state != 2 )
			return 0;
	}
	if ( Goal->if_goal_is_removed )
	{
		te = Findgoal( Goal->if_goal_is_removed );
		if ( te && te->goal_state != 3 )
			return 0;
	}
	if ( Goal->if_group_is_active )
	{
		te = trap_find( world, FOFS( s.v.classname ), "info_tfgoal" );
		while ( te )
		{
			if ( te->group_no == Goal->if_group_is_active )
			{
				if ( te->goal_state != 1 )
					return 0;
			}
			te = trap_find( te, FOFS( s.v.classname ), "info_tfgoal" );
		}
	}
	if ( Goal->if_group_is_inactive )
	{
		te = trap_find( world, FOFS( s.v.classname ), "info_tfgoal" );
		while ( te )
		{
			if ( te->group_no == Goal->if_group_is_inactive )
			{
				if ( te->goal_state != 2 )
					return 0;
			}
			te = trap_find( te, FOFS( s.v.classname ), "info_tfgoal" );
		}
	}
	if ( Goal->if_group_is_removed )
	{
		te = trap_find( world, FOFS( s.v.classname ), "info_tfgoal" );
		while ( te )
		{
			if ( te->group_no == Goal->if_group_is_removed )
			{
				if ( te->goal_state != 3 )
					return 0;
			}
			te = trap_find( te, FOFS( s.v.classname ), "info_tfgoal" );
		}
	}
	if ( Goal->if_item_has_moved )
	{
		te = Finditem( Goal->if_item_has_moved );
		if ( te != world )
		{
			if ( te->goal_state != 1 && VectorCompare( te->s.v.origin, te->s.v.oldorigin ) )
				return 0;
		}
	}
	if ( Goal->if_item_hasnt_moved )
	{
		te = Finditem( Goal->if_item_hasnt_moved );
		if ( te != world )
		{
			if ( te->goal_state == 1 || !VectorCompare( te->s.v.origin, te->s.v.oldorigin ) )
				return 0;
		}
	}
	if ( AP && AP != world )
	{
		gotone = 0;
		if ( Goal->has_item_from_group )
		{
			te = trap_find( world, FOFS( s.v.classname ), "item_tfgoal" );
			while ( te && !gotone )
			{
				if ( te->group_no == Goal->has_item_from_group && te->s.v.owner == EDICT_TO_PROG( AP ) )
					gotone = 1;
				te = trap_find( te, FOFS( s.v.classname ), "item_tfgoal" );
			}
			if ( !gotone )
				return 0;
		}
		if ( Goal->hasnt_item_from_group )
		{
			te = trap_find( world, FOFS( s.v.classname ), "item_tfgoal" );
			while ( te && !gotone )
			{
				if ( te->group_no == Goal->hasnt_item_from_group
				     && te->s.v.owner == EDICT_TO_PROG( AP ) )
					return 0;
				te = trap_find( te, FOFS( s.v.classname ), "item_tfgoal" );
			}
		}
	}
	return 1;
}

void SetupRespawn( gedict_t * Goal )
{
	if ( Goal->search_time )
	{
		InactivateGoal( Goal );
		Goal->s.v.think = ( func_t ) tfgoal_timer_tick;
		Goal->s.v.nextthink = g_globalvars.time + Goal->search_time;
		return;
	}
	if ( Goal->goal_result & TFGR_SINGLE )
	{
		RemoveGoal( Goal );
		return;
	}
	if ( Goal->wait > 0 )
	{
		Goal->s.v.nextthink = g_globalvars.time + Goal->wait;
		Goal->s.v.think = ( func_t ) DoRespawn;
		return;
	} else
	{
		if ( Goal->wait == -1 )
			return;
	}
	InactivateGoal( Goal );
}

void DoRespawn(  )
{
	RestoreGoal( self );
	InactivateGoal( self );
}

int Activated( gedict_t * Goal, gedict_t * AP )
{
	float   APMet;
	float   RevAct;
	float   Act;

	if ( Goal->goal_state == 1 )
		return 0;
	if ( Goal->goal_state == 3 )
		return 0;
	if ( Goal->goal_state == 4 )
		return 0;
	APMet = APMeetsCriteria( Goal, AP );
	if ( streq( Goal->s.v.classname, "item_tfgoal" ) )
		RevAct = Goal->goal_activation & TFGI_REVERSE_AP;
	else
		RevAct = Goal->goal_activation & TFGA_REVERSE_AP;
	Act = 0;
	if ( APMet )
	{
		if ( !RevAct )
			Act = 1;
	} else
	{
		if ( RevAct )
			Act = 1;
	}
	return Act;
}

void AttemptToActivate( gedict_t * Goal, gedict_t * AP, gedict_t * ActivatingGoal )
{
	gedict_t *te;

	if ( tf_data.cb_prematch_time > g_globalvars.time )
		return;
	if ( Activated( Goal, AP ) )
	{
		if ( ActivatingGoal == Goal )
			DoResults( Goal, AP, 1 );
		else
		{
			if ( ActivatingGoal != world )
				DoResults( Goal, AP, ActivatingGoal->goal_result & TFGR_ADD_BONUSES );
			else
				DoResults( Goal, AP, 0 );
		}
	} else
	{
		if ( Goal->else_goal )
		{
			te = Findgoal( Goal->else_goal );
			if ( te )
				AttemptToActivate( te, AP, Goal );
		}
	}
}

void DoGoalWork( gedict_t * Goal, gedict_t * AP )
{
	gedict_t *te;
	gedict_t *RI;

	if ( Goal->activate_goal_no )
	{
		te = Findgoal( Goal->activate_goal_no );
		if ( te )
			AttemptToActivate( te, AP, Goal );
	}
	if ( Goal->inactivate_goal_no )
	{
		te = Findgoal( Goal->inactivate_goal_no );
		if ( te )
			InactivateGoal( te );
	}
	if ( Goal->restore_goal_no )
	{
		te = Findgoal( Goal->restore_goal_no );
		if ( te )
			RestoreGoal( te );
	}
	if ( Goal->remove_goal_no )
	{
		te = Findgoal( Goal->remove_goal_no );
		if ( te )
			RemoveGoal( te );
	}
	if ( Goal->return_item_no )
	{
		te = Finditem( Goal->return_item_no );
		if ( te != world )
		{
			if ( te->goal_state == 1 )
				tfgoalitem_RemoveFromPlayer( te, PROG_TO_EDICT( te->s.v.owner ), 1 );
			RI = spawn(  );
			RI->s.v.enemy = EDICT_TO_PROG( te );
			RI->s.v.weapon = 2;
			RI->s.v.think = ( func_t ) ReturnItem;
			RI->s.v.nextthink = g_globalvars.time + 0.1;
			te->s.v.solid = SOLID_NOT;
		}
	}
	if ( Goal->remove_spawnpoint )
	{
		te = Findteamspawn( Goal->remove_spawnpoint );
		if ( te )
		{
			te->goal_state = 3;
			te->team_str_home = "";
		}
	}
	if ( Goal->restore_spawnpoint )
	{
		te = Findteamspawn( Goal->restore_spawnpoint );
		if ( te )
		{
			if ( te->goal_state == 3 )
			{
				te->goal_state = 2;
				te->team_str_home = (char*)team_spawn_str[te->team_no];
			}
		}
	}
}

void DoGroupWork( gedict_t * Goal, gedict_t * AP )
{
	gedict_t *tg;
	float   allset;

	if ( Goal->all_active )
	{
		if ( !Goal->last_impulse )
		{
			G_dprintf( "Goal %d has a .all_active specified, but no .last_impulse\n", Goal->goal_no );
		} else
		{
			allset = 1;
			tg = trap_find( world, FOFS( s.v.classname ), "info_tfgoal" );
			while ( tg )
			{
				if ( tg->group_no == Goal->all_active )
				{
					if ( tg->goal_state != 1 )
						allset = 0;
				}
				tg = trap_find( tg, FOFS( s.v.classname ), "info_tfgoal" );
			}
			if ( allset )
			{
				tg = Findgoal( Goal->last_impulse );
				if ( tg )
					DoResults( tg, AP, Goal->goal_result & TFGR_ADD_BONUSES );
			}
		}
	}
	if ( Goal->activate_group_no )
	{
		tg = trap_find( world, FOFS( s.v.classname ), "info_tfgoal" );
		while ( tg )
		{
			if ( tg->group_no == Goal->activate_group_no )
				DoResults( tg, AP, 0 );
			tg = trap_find( tg, FOFS( s.v.classname ), "info_tfgoal" );
		}
	}
	if ( Goal->inactivate_group_no )
	{
		tg = trap_find( world, FOFS( s.v.classname ), "info_tfgoal" );
		while ( tg )
		{
			if ( tg->group_no == Goal->inactivate_group_no )
				InactivateGoal( tg );
			tg = trap_find( tg, FOFS( s.v.classname ), "info_tfgoal" );
		}
	}
	if ( Goal->remove_group_no )
	{
		tg = trap_find( world, FOFS( s.v.classname ), "info_tfgoal" );
		while ( tg )
		{
			if ( tg->group_no == Goal->remove_group_no )
				RemoveGoal( tg );
			tg = trap_find( tg, FOFS( s.v.classname ), "info_tfgoal" );
		}
	}
	if ( Goal->restore_group_no )
	{
		tg = trap_find( world, FOFS( s.v.classname ), "info_tfgoal" );
		while ( tg )
		{
			if ( tg->group_no == Goal->restore_group_no )
				RestoreGoal( tg );
			tg = trap_find( tg, FOFS( s.v.classname ), "info_tfgoal" );
		}
	}
	if ( Goal->remove_spawngroup )
	{
		tg = trap_find( world, FOFS( s.v.classname ), "info_player_teamspawn" );
		while ( tg )
		{
			if ( tg->group_no == Goal->remove_spawngroup )
			{
				tg->goal_state = 3;
				tg->team_str_home = "";
			}
			tg = trap_find( tg, FOFS( s.v.classname ), "info_player_teamspawn" );
		}
	}
	if ( Goal->restore_spawngroup )
	{
		tg = trap_find( world, FOFS( s.v.classname ), "info_player_teamspawn" );
		while ( tg )
		{
			if ( tg->group_no == Goal->restore_spawngroup )
			{
				tg->goal_state = 2;
				tg->team_str_home = (char*)team_spawn_str[tg->team_no];
			}
			tg = trap_find( tg, FOFS( s.v.classname ), "info_player_teamspawn" );
		}
	}
}

void DoItemGroupWork( gedict_t * Item, gedict_t * AP )
{
	gedict_t *tg;
	gedict_t *carrier;
	float   allcarried;

	allcarried = 1;
	if ( Item->distance )
	{
		if ( !Item->pain_finished )
		{
			G_dprintf( "GoalItem %d has a .distance specified, but no .pain_finished\n", Item->goal_no );
		}
		tg = trap_find( world, FOFS( s.v.classname ), "item_tfgoal" );
		while ( tg )
		{
			if ( tg->group_no == Item->distance )
			{
				if ( tg->goal_state != 1 )
					allcarried = 0;
			}
			tg = trap_find( tg, FOFS( s.v.classname ), "item_tfgoal" );
		}
		if ( allcarried == 1 )
		{
			tg = Findgoal( Item->pain_finished );
			if ( tg )
				DoResults( tg, AP, Item->goal_result & TFGR_ADD_BONUSES );
		}
	}
	allcarried = 1;
	if ( Item->speed )
	{
		if ( !Item->attack_finished )
		{
			G_dprintf( "GoalItem %d has a ->speed  specified, but no .attack_finished\n", Item->goal_no );
		}
		carrier = world;
		tg = trap_find( world, FOFS( s.v.classname ), "item_tfgoal" );
		while ( tg )
		{
			if ( tg->group_no == Item->speed )
			{
				if ( tg->goal_state != 1 )
					allcarried = 0;
				else
				{
					if ( carrier == world )
						carrier = PROG_TO_EDICT( tg->s.v.owner );
					else
					{
						if ( carrier != PROG_TO_EDICT( tg->s.v.owner ) )
							allcarried = 0;
					}
				}
			}
			tg = trap_find( tg, FOFS( s.v.classname ), "item_tfgoal" );
		}
		if ( allcarried == 1 )
		{
			tg = Findgoal( Item->attack_finished );
			if ( tg )
				DoResults( tg, AP, Item->goal_result & TFGR_ADD_BONUSES );
		}
	}
}

void DoTriggerWork( gedict_t * Goal, gedict_t * AP )
{
	gedict_t *t, *stemp, *otemp;

	if ( Goal->killtarget )
	{
		t = world;
		do
		{
			t = trap_find( t, FOFS( s.v.targetname ), Goal->killtarget );
			if ( t )
				ent_remove( t );
		}
		while ( t );
	}
	if ( Goal->s.v.target )
	{
		t = world;
		activator = AP;
		do
		{
			t = trap_find( t, FOFS( s.v.targetname ), Goal->s.v.target );
			if ( !t )
				return;
			stemp = self;
			otemp = other;
			self = t;
			other = stemp;
			if ( self->s.v.use )
				( ( void ( * )(  ) ) ( self->s.v.use ) ) (  );
			self = stemp;
			other = otemp;
			activator = AP;
		}
		while ( t );
	}
}

void DelayedResult(  )
{
	if ( PROG_TO_EDICT( self->s.v.enemy )->goal_state == 4 )
		DoResults( PROG_TO_EDICT( self->s.v.enemy ), PROG_TO_EDICT( self->s.v.owner ), self->s.v.weapon );
	dremove( self );
}

void DoResults( gedict_t * Goal, gedict_t * AP, float addb )
{
	gedict_t *te;
	float   winners;
	float   gotone;

	if ( tf_data.cb_prematch_time > g_globalvars.time )
		return;
	if ( Goal->goal_state == 1 )
		return;
	if ( Goal->delay_time > 0 && Goal->goal_state != 4 )
	{
		Goal->goal_state = 4;
		te = spawn(  );
		te->s.v.think = ( func_t ) DelayedResult;
		te->s.v.nextthink = g_globalvars.time + Goal->delay_time;
		te->s.v.owner = EDICT_TO_PROG( AP );
		te->s.v.enemy = EDICT_TO_PROG( Goal );
		te->s.v.weapon = addb;
		return;
	}
	UpdateAbbreviations( Goal );
	Goal->goal_state = 2;
	if ( streq( Goal->s.v.classname, "info_tfgoal" ) && Goal->mdl )
		setmodel( Goal, "" );
	if ( Goal->s.v.noise )
		sound( other, 3, Goal->s.v.noise, 1, 1 );
	winners = 0;
	if ( Goal->increase_team1 )
	{
		TeamFortress_TeamIncreaseScore( 1, Goal->increase_team1 );
		winners = 1;
	}
	if ( Goal->increase_team2 )
	{
		TeamFortress_TeamIncreaseScore( 2, Goal->increase_team2 );
		winners = 1;
	}
	if ( Goal->increase_team3 )
	{
		TeamFortress_TeamIncreaseScore( 3, Goal->increase_team3 );
		winners = 1;
	}
	if ( Goal->increase_team4 )
	{
		TeamFortress_TeamIncreaseScore( 4, Goal->increase_team4 );
		winners = 1;
	}
	if ( winners == 1 )
		TeamFortress_TeamShowScores( 2 );
	if ( CTF_Map == 1 )
	{
		if ( AP && AP != world )
		{
			if ( Goal->goal_no == 1 )
			{
				te = trap_find( world, FOFS( s.v.classname ), "player" );
				while ( te )
				{
					if ( te->team_no == 2 )
					{
						if ( te == AP )
						{
							winners = g_random(  );
							if ( winners < 0.1 )
								CenterPrint( te,
									     "\n\n\nYou got the enemy flag!\n\nFlee!" );
							else
							{
								if ( winners < 0.2 )
									CenterPrint( te,
										     "\n\n\nYou got the enemy flag!\n\nHead for home!" );
								else
								{
									if ( winners < 0.6 )
										CenterPrint( te,
											     "\n\n\nYou got the enemy flag!\n\nReturn to base!" );
									else
									{
										if ( winners < 0.7 )
											CenterPrint( te,
												     "\n\n\nYou got the enemy flag!\n\n<Insert witty comment here>" );
										else
										{
											if ( winners < 0.8 )
												CenterPrint( te,
													     "\n\n\nYou got the enemy flag!\n\n" );
											else
											{
												if ( winners < 0.95 )
													CenterPrint( te,
														     "\n\n\nYou got the enemy flag!\n\n" );
												else
													CenterPrint( te,
														     "\n\n\nIs that a flag in your pocket\nor a you just happy to see me?" );
											}
										}
									}
								}
							}
						} else
							CenterPrint( te, "\n\n\nYour team гот the енемы flag!!" );
					} else
						CenterPrint( te, "\n\n\nYour flag has been такен!!" );
					te = trap_find( te, FOFS( s.v.classname ), "player" );
				}
				G_bprint( 2, "%s гот the блуе flag!\n", AP->s.v.netname );
				AP->s.v.items = ( int ) AP->s.v.items | 131072;
			} else
			{
				if ( Goal->goal_no == 2 )
				{
					te = trap_find( world, FOFS( s.v.classname ), "player" );
					while ( te )
					{
						if ( te->team_no == 1 )
						{
							if ( te == AP )
							{
								winners = g_random(  );
								if ( winners < 0.1 )
									CenterPrint( te,
										     "\n\n\nYou got the enemy flag!\n\nFlee!" );
								else
								{
									if ( winners < 0.2 )
										CenterPrint( te,
											     "\n\n\nYou got the enemy flag!\n\nHead for home!" );
									else
									{
										if ( winners < 0.6 )
											CenterPrint( te,
												     "\n\n\nYou got the enemy flag!\n\nReturn to base!" );
										else
										{
											if ( winners < 0.7 )
												CenterPrint( te,
													     "\n\n\nYou got the enemy flag!\n\n<Insert witty comment here>" );
											else
											{
												if ( winners < 0.8 )
													CenterPrint( te,
														     "\n\n\nYou got the enemy flag!\n\nRed's dead baby, Red's dead..." );
												else
												{
													if ( winners <
													     0.95 )
														CenterPrint
														    ( te,
														      "\n\n\nYou got the enemy flag!\n\n" );
													else
														CenterPrint
														    ( te,
														      "\n\n\nIs that a flag in your pocket\nor a you just happy to see me?" );
												}
											}
										}
									}
								}
							} else
								CenterPrint( te,
									     "\n\n\nYour team гот the енемы flag!!" );
						} else
							CenterPrint( te, "\n\n\nYour flag has been такен!!" );
						te = trap_find( te, FOFS( s.v.classname ), "player" );
					}
					G_bprint( 2, "%s гот the ред flag!\n", AP->s.v.netname );
					AP->s.v.items = ( int ) AP->s.v.items | 262144;
				} else
				{
					if ( Goal->goal_no == 3 )
					{
						te = trap_find( world, FOFS( s.v.classname ), "player" );
						while ( te )
						{
							if ( te->team_no == 2 )
							{
								if ( te == AP )
									CenterPrint( te,
										     "\n\n\nYou цаптуред the flag!!" );
								else
									CenterPrint( te,
										     "\n\n\nYour flag was цаптуред!!" );
							} else
								CenterPrint( te,
									     "\n\n\nYour team цаптуред the flag!!" );
							te = trap_find( te, FOFS( s.v.classname ), "player" );
						}
						G_bprint( 2, "%s цаптуред the ред flag!\n", AP->s.v.netname );
						AP->s.v.items = AP->s.v.items - ( ( int ) AP->s.v.items & 262144 );
					} else
					{
						if ( Goal->goal_no == 4 )
						{
							te = trap_find( world, FOFS( s.v.classname ), "player" );
							while ( te )
							{
								if ( te->team_no == 1 )
								{
									if ( te == AP )
										CenterPrint( te,
											     "\n\n\nYou цаптуред the flag!!" );
									else
										CenterPrint( te,
											     "\n\n\nYour flag was цаптуред!!" );
								} else
									CenterPrint( te,
										     "\n\n\nYour team цаптуред the flag!!" );
								te = trap_find( te, FOFS( s.v.classname ), "player" );
							}
							G_bprint( 2, "%s цаптуред the блуе flag!\n", AP->s.v.netname );
							AP->s.v.items =
							    AP->s.v.items - ( ( int ) AP->s.v.items & 131072 );
						}
					}
				}
			}
		}
	}
	gotone = 0;
	te = trap_find( world, FOFS( s.v.classname ), "player" );
	while ( te )
	{
		if ( Goal->broadcast && !CTF_Map )
			CenterPrint( te, "\n\n\n%s", Goal->broadcast );
		if ( Goal->netname_broadcast && !CTF_Map )
		{
			G_sprint( te, 2, AP->s.v.netname );
			G_sprint( te, 2, Goal->netname_broadcast );
		}
		if ( AP == te )
		{
			if ( Goal->s.v.message )
				CenterPrint( te, "\n\n\n%s", Goal->s.v.message );
		} else
		{
			if ( AP->team_no == te->team_no )
			{
				if ( Goal->owners_team_broadcast && te->team_no == Goal->owned_by )
					CenterPrint( te, "\n\n\n%s", Goal->owners_team_broadcast );
				else
				{
					if ( Goal->team_broadcast )
						CenterPrint( te, "\n\n\n%s", Goal->team_broadcast );
				}
				if ( Goal->netname_owners_team_broadcast && te->team_no == Goal->owned_by )
				{
					G_sprint( te, 2, "%s%s", AP->s.v.netname, Goal->netname_owners_team_broadcast );
				} else
				{
					if ( Goal->netname_team_broadcast )
					{
						G_sprint( te, 2, "%s%s", AP->s.v.netname,
							  Goal->netname_team_broadcast );
					}
				}
			} else
			{
				if ( Goal->owners_team_broadcast && te->team_no == Goal->owned_by )
					CenterPrint( te, "\n\n\n%s", Goal->owners_team_broadcast );
				else
				{
					if ( Goal->non_team_broadcast )
						CenterPrint( te, "\n\n\n%s", Goal->non_team_broadcast );
				}
				if ( Goal->netname_owners_team_broadcast && te->team_no == Goal->owned_by )
				{
					G_sprint( te, 2, "%s%s", AP->s.v.netname, Goal->netname_owners_team_broadcast );
				} else
				{
					if ( Goal->netname_non_team_broadcast )
					{
						G_sprint( te, 2, "%s%s", AP->s.v.netname,
							  Goal->netname_non_team_broadcast );
					}
				}
			}
		}
		if ( IsAffectedBy( Goal, te, AP ) )
		{
			if ( Goal->search_time && ( Goal->goal_effects & TFGE_TIMER_CHECK_AP ) )
			{
				if ( APMeetsCriteria( Goal, te ) )
				{
					Apply_Results( Goal, te, AP, addb );
					gotone = 1;
				}
			} else
			{
				Apply_Results( Goal, te, AP, addb );
				gotone = 1;
			}
		}
		te = trap_find( te, FOFS( s.v.classname ), "player" );
	}
	if ( strneq( Goal->s.v.classname, "item_tfgoal" ) )
		Goal->goal_state = 1;
	if ( Goal->goal_result & TFGR_ENDGAME )
	{
		TeamFortress_TeamShowScores( 1 );
		winners = TeamFortress_TeamGetWinner(  );
		te = trap_find( world, FOFS( s.v.classname ), "player" );
		while ( te )
		{
			te->s.v.takedamage = 0;
			te->s.v.movetype = MOVETYPE_NONE;
			SetVector( te->s.v.velocity, 0, 0, 0 );
			SetVector( te->s.v.avelocity, 0, 0, 0 );
			te = trap_find( te, FOFS( s.v.classname ), "player" );
		}
		te = spawn(  );
		te->s.v.nextthink = g_globalvars.time + 5;
		te->s.v.think = ( func_t ) execute_changelevel;
		dremove( Goal );
		return;
	}
	DoGroupWork( Goal, AP );
	DoGoalWork( Goal, AP );
	DoTriggerWork( Goal, AP );
	if ( streq( Goal->s.v.classname, "info_tfgoal" ) )
		SetupRespawn( Goal );
}

void tfgoal_touch(  )
{
	gedict_t *te;

	if( tf_data.arena_mode )
	        return;

	if ( !( self->goal_activation & TFGA_TOUCH ) )
		return;
	if ( strneq( other->s.v.classname, "player" ) )
		return;
	if ( tf_data.cb_prematch_time > g_globalvars.time )
		return;
	if ( self->goal_state == 1 )
		return;
	if ( CTF_Map == 1 )
	{
		if ( self->goal_no == 3 && other->team_no == 1 )
		{
			te = Finditem( 1 );
			if ( te->goal_state == 1 || !VectorCompare( te->s.v.origin, te->s.v.oldorigin ) )
				return;
		}
		if ( self->goal_no == 4 && other->team_no == 2 )
		{
			te = Finditem( 2 );
			if ( te->goal_state == 1 || !VectorCompare( te->s.v.origin, te->s.v.oldorigin ) )
				return;
		}
	}
	AttemptToActivate( self, other, self );
}

void info_tfgoal_use(  )
{
	AttemptToActivate( self, activator, self );
}

void tfgoal_timer_tick(  )
{
	if ( self->goal_state != 3 )
	{
		if ( APMeetsCriteria( self, world ) )
			DoResults( self, world, 1 );
		else
		{
			InactivateGoal( self );
			self->s.v.think = ( func_t ) tfgoal_timer_tick;
			self->s.v.nextthink = g_globalvars.time + self->search_time;
		}
	}
}

void item_tfgoal_touch(  )
{
	gedict_t *te;

	if( tf_data.arena_mode )
	        return;

	if ( strneq( other->s.v.classname, "player" ) )
		return;
	if ( other->s.v.health <= 0 )
		return;
	if ( tf_data.cb_prematch_time > g_globalvars.time )
		return;
	if ( other == PROG_TO_EDICT( self->s.v.owner ) )
		return;
	if ( other->is_feigning )
		return;

	if ( CTF_Map == 1 )
	{
		if ( self->goal_no == 1 )
		{
			if ( !VectorCompare( self->s.v.origin, self->s.v.oldorigin ) )
			{
				if ( other->team_no == 1 )
				{
					G_bprint( 2, "%s ретурнед the блуе flag!\n", other->s.v.netname );
					te = trap_find( world, FOFS( s.v.classname ), "player" );
					while ( te )
					{
						if ( te->team_no == 1 )
							CenterPrint( te, "\n\n\nYour flag was ретурнед!!" );
						else
							CenterPrint( te, "\n\n\nThe енемы flag was ретурнед!!" );
						te = trap_find( te, FOFS( s.v.classname ), "player" );
					}
					self->goal_state = 2;
					self->s.v.solid = SOLID_TRIGGER;
					self->s.v.touch = ( func_t ) item_tfgoal_touch;
					VectorCopy( self->s.v.oldorigin, self->s.v.origin );
//     self->s.v.origin = self->s.v.oldorigin;
					setmodel( self, self->mdl );
					setorigin( self, PASSVEC3( self->s.v.origin ) );
					sound( self, 2, "items/itembk2.wav", 1, 1 );
					return;
				}
			} else
			{
				if ( other->team_no == 1 )
					return;
			}
		} else
		{
			if ( self->goal_no == 2 )
			{
				if ( !VectorCompare( self->s.v.origin, self->s.v.oldorigin ) )
				{
					if ( other->team_no == 2 )
					{
						G_bprint( 2, "%s ретурнед the ред flag!\n", other->s.v.netname );
						te = trap_find( world, FOFS( s.v.classname ), "player" );
						while ( te )
						{
							if ( te->team_no == 2 )
								CenterPrint( te, "\n\n\n Your flag was ретурнед!!" );
							else
								CenterPrint( te,
									     "\n\n\n The енемы flag was ретурнед!!" );
							te = trap_find( te, FOFS( s.v.classname ), "player" );
						}
						self->goal_state = 2;
						self->s.v.solid = SOLID_TRIGGER;
						self->s.v.touch = ( func_t ) item_tfgoal_touch;
						VectorCopy( self->s.v.oldorigin, self->s.v.origin );
//      self->s.v.origin = self->s.v.oldorigin;
						setmodel( self, self->mdl );
						setorigin( self, PASSVEC3( self->s.v.origin ) );
						sound( self, 2, "items/itembk2.wav", 1, 1 );
						return;
					}
				} else
				{
					if ( other->team_no == 2 )
						return;
				}
			}
		}
	}
	if ( Activated( self, other ) )
	{
		tfgoalitem_GiveToPlayer( self, other, self );
		if ( other->s.v.health > 0 )
			self->goal_state = 1;
	} else
	{
		if ( self->else_goal )
		{
			te = Findgoal( self->else_goal );
			if ( te )
				AttemptToActivate( te, other, self );
		}
	}
}

void tfgoalitem_GiveToPlayer( gedict_t * Item, gedict_t * AP, gedict_t * Goal )
{

        //REMOVE ME !!!!
        //G_sprint(AP,2,"tfgoalitem_GiveToPlayer %s %s\n",Item->s.v.netname,Goal->s.v.netname);

	Item->s.v.owner = EDICT_TO_PROG( AP );
	if ( Item->mdl )
		setmodel( Item, "" );
	Item->s.v.solid = SOLID_NOT;
	if ( Item->goal_activation & TFGI_GLOW )
		AP->s.v.effects = ( int ) AP->s.v.effects | 8;
	if ( Item->goal_activation & TFGI_SLOW )
		TeamFortress_SetSpeed( AP );
	if ( Item->goal_activation & TFGI_ITEMGLOWS )
		Item->s.v.effects = Item->s.v.effects - ( ( int ) Item->s.v.effects | 8 );
	if ( ( int ) Item->s.v.items & IT_KEY1 )
		AP->s.v.items = ( int ) AP->s.v.items | IT_KEY1;
	if ( ( int ) Item->s.v.items & IT_KEY2 )
		AP->s.v.items = ( int ) AP->s.v.items | IT_KEY2;
	if ( Goal != Item )
	{
		if ( Goal->goal_result & TFGR_NO_ITEM_RESULTS )
		{
			Item->goal_state = 1;
			return;
		}
	}
	if ( AP->playerclass == PC_SPY && ( Item->goal_result & TFGR_REMOVE_DISGUISE ) )
		AP->is_unabletospy = 1;
	DoResults( Item, AP, 1 );
	DoItemGroupWork( Item, AP );
}

void ReturnItem(  )
{
	gedict_t *te;
	gedict_t *enemy = PROG_TO_EDICT( self->s.v.enemy );

	enemy->goal_state = 2;
	if ( ( enemy->goal_activation & TFGI_SOLID ) && streq( enemy->s.v.classname, "item_tfgoal" ) )
		enemy->s.v.solid = SOLID_BBOX;
	else
		enemy->s.v.solid = SOLID_TRIGGER;
	enemy->s.v.movetype = MOVETYPE_NONE;
	enemy->s.v.touch = ( func_t ) item_tfgoal_touch;
	VectorCopy( enemy->s.v.oldorigin, enemy->s.v.origin );
	if ( enemy->mdl )
		setmodel( enemy, enemy->mdl );
	setorigin( enemy, PASSVEC3( enemy->s.v.origin ) );
	sound( enemy, 2, "items/itembk2.wav", 1, 1 );
	tfgoalitem_checkgoalreturn( enemy );
	if ( self->s.v.weapon != 2 )
	{
		if ( enemy->s.v.noise3 || enemy->noise4 )
		{
			te = trap_find( world, FOFS( s.v.classname ), "player" );
			while ( te )
			{
				if ( te->team_no == enemy->owned_by )
				{
				    if ( enemy->s.v.noise3 )
					CenterPrint( te, "\n\n\n%s", enemy->s.v.noise3 );
				}
				else
				{
				   if ( enemy->noise4 )
					CenterPrint( te, "\n\n\n%s", enemy->noise4 );
				}
				te = trap_find( te, FOFS( s.v.classname ), "player" );
			}
		}
	}
	dremove( self );
}

void tfgoalitem_RemoveFromPlayer( gedict_t * Item, gedict_t * AP, int method )
{
	gedict_t *te;
	float   lighton;
	float   slowon;
	float   key1on;
	float   key2on;
	float   spyoff;
	gedict_t *DelayReturn;

        //REMOVE ME !!!!
        //G_sprint(AP,2,"tfgoalitem_RemoveFromPlayer %s\n",Item->s.v.netname);
	if ( !Item || Item == world )
	{
		G_Error( "error: tfgoalitem_RemoveFromPlayer(): Item == world" );
		return;
	}
	lighton = 0;
	slowon = 0;
	key1on = 0;
	key2on = 0;
	spyoff = 0;
	te = trap_find( world, FOFS( s.v.classname ), "item_tfgoal" );
	while ( te )
	{
		if ( te->s.v.owner == EDICT_TO_PROG( AP ) && te != Item )
		{
			if ( te->goal_activation & TFGI_GLOW )
				lighton = 1;
			if ( te->goal_activation & TFGI_SLOW )
				slowon = 1;
			if ( ( int ) te->s.v.items & IT_KEY1 )
				key1on = 1;
			if ( ( int ) te->s.v.items & IT_KEY2 )
				key2on = 1;
			if ( te->goal_result & TFGR_REMOVE_DISGUISE )
				spyoff = 1;
		}
		te = trap_find( te, FOFS( s.v.classname ), "item_tfgoal" );
	}
	if ( !lighton )
	{
		if ( AP->invincible_finished > g_globalvars.time + 3 )
			lighton = 1;
		else
		{
			if ( AP->super_damage_finished > g_globalvars.time + 3 )
				lighton = 1;
		}
	}
	if ( !lighton )
	{
		AP->s.v.effects = AP->s.v.effects - ( ( int ) AP->s.v.effects & 8 );
		AP->s.v.effects = AP->s.v.effects - ( ( int ) AP->s.v.effects & 64 );
		AP->s.v.effects = AP->s.v.effects - ( ( int ) AP->s.v.effects & 128 );
	}
	if ( Item->goal_activation & TFGI_ITEMGLOWS )
		Item->s.v.effects = ( int ) Item->s.v.effects | 8;
	if ( !spyoff )
		AP->is_unabletospy = 0;
	if ( !key1on )
		AP->s.v.items = AP->s.v.items - ( ( int ) AP->s.v.items & IT_KEY1 );
	if ( !key2on )
		AP->s.v.items = AP->s.v.items - ( ( int ) AP->s.v.items & IT_KEY2 );
	te = trap_find( world, FOFS( s.v.classname ), "player" );
	while ( te )
	{
		if ( IsAffectedBy( Item, te, AP ) )
			RemoveResults( Item, te );
		te = trap_find( te, FOFS( s.v.classname ), "player" );
	}
	if ( !method || method == 2 )
	{
		te = trap_find( world, FOFS( s.v.classname ), "player" );
		while ( te )
		{
			if ( te->team_no == Item->owned_by )
			{
				if ( Item->team_drop )
					CenterPrint( te, "\n\n\n%s", Item->team_drop );
				if ( Item->netname_team_drop )
				{
					G_sprint( te, 2, "%s%s", AP->s.v.netname, Item->netname_team_drop );
				}
			} else
			{
				if ( Item->non_team_drop )
					CenterPrint( te, "\n\n\n%s", Item->non_team_drop );
				if ( Item->netname_non_team_drop )
				{
					G_sprint( te, 2, "%s%s", AP->s.v.netname, Item->netname_non_team_drop );
				}
			}
			te = trap_find( te, FOFS( s.v.classname ), "player" );
		}
		if ( Item->goal_activation & TFGI_RETURN_DROP )
		{
			DelayReturn = spawn(  );
			DelayReturn->s.v.enemy = EDICT_TO_PROG( Item );
			if ( !method )
				DelayReturn->s.v.weapon = 0;
			else
				DelayReturn->s.v.weapon = 1;
			DelayReturn->s.v.think = ( func_t ) ReturnItem;
			DelayReturn->s.v.nextthink = g_globalvars.time + 0.5;
		} else
		{
			if ( Item->goal_activation & TFGI_DROP )
			{
				if ( method == 2 && ( ( Item->goal_activation & TFGI_ALLOW_DROP ) || tf_data.allow_drop_goal ) )
					tfgoalitem_drop( Item, 1, AP );
				else
					tfgoalitem_drop( Item, 0, AP );
			} else
			{
				Item->s.v.owner = EDICT_TO_PROG( world );
				dremove( Item );
				TeamFortress_SetSpeed( AP );
				return;
			}
		}
		Item->s.v.owner = EDICT_TO_PROG( world );
		Item->s.v.flags = ( int ) Item->s.v.flags - ( ( int ) Item->s.v.flags & 512 );
		setsize( Item, PASSVEC3( Item->goal_min ), PASSVEC3( Item->goal_max ) );
		TeamFortress_SetSpeed( AP );
		return;
	} else
	{
		if ( method == 1 )
		{
			if ( Item->goal_activation & TFGI_RETURN_GOAL )
			{
				DelayReturn = spawn(  );
				DelayReturn->s.v.enemy = EDICT_TO_PROG( Item );
				DelayReturn->s.v.weapon = 2;
				DelayReturn->s.v.think = ( func_t ) ReturnItem;
				DelayReturn->s.v.nextthink = g_globalvars.time + 0.5;
				Item->s.v.owner = EDICT_TO_PROG( world );
				TeamFortress_SetSpeed( AP );
				return;
			}
			Item->s.v.solid = SOLID_NOT;
			Item->s.v.owner = EDICT_TO_PROG( world );
			TeamFortress_SetSpeed( AP );
			return;
		}
	}
	G_Error( "Invalid method passed into tfgoalitem_RemoveFromPlayer\n" );
}

void tfgoalitem_dropthink(  )
{
	float   pos;

	self->s.v.movetype = MOVETYPE_TOSS;
	if ( self->pausetime )
	{
		pos = trap_pointcontents( PASSVEC3( self->s.v.origin ) );
		if ( pos == -4 )
			self->s.v.nextthink = g_globalvars.time + self->pausetime / 4;
		else
		{
			if ( pos == -5 )
				self->s.v.nextthink = g_globalvars.time + 5;
			else
			{
				if ( pos == -2 || pos == -6 )
				{
					if ( self->camdist < 3 )
					{
						VectorCopy( self->camangle, self->s.v.origin );
						setorigin( self, PASSVEC3( self->s.v.origin ) );
						self->s.v.velocity[2] = 400;
						self->s.v.velocity[0] = -50 + g_random(  ) * 100;
						self->s.v.velocity[1] = -50 + g_random(  ) * 100;
						self->goal_state = 2;
						self->s.v.movetype = MOVETYPE_TOSS;
						if ( self->goal_activation & TFGI_SOLID )
							self->s.v.solid = SOLID_BBOX;
						else
							self->s.v.solid = SOLID_TRIGGER;
						if ( self->mdl )
							setmodel( self, self->mdl );
						setsize( self, PASSVEC3( self->goal_min ), PASSVEC3( self->goal_max ) );
						self->camdist = self->camdist + 1;
						self->s.v.nextthink = g_globalvars.time + 5;
						self->s.v.think = ( func_t ) tfgoalitem_dropthink;
						return;
					} else
						self->s.v.nextthink = g_globalvars.time + 2;
				} else
					self->s.v.nextthink = g_globalvars.time + self->pausetime;
			}
		}
		self->s.v.think = ( func_t ) tfgoalitem_remove;
	}
}

void tfgoalitem_droptouch(  )
{
	self->s.v.touch = ( func_t ) item_tfgoal_touch;
	self->s.v.nextthink = g_globalvars.time + 4.25;
	self->s.v.think = ( func_t ) tfgoalitem_dropthink;
}

void tfgoalitem_drop( gedict_t * Item, float PAlive, gedict_t * P )
{
	VectorCopy( PROG_TO_EDICT( Item->s.v.owner )->s.v.origin, Item->s.v.origin );
	setorigin( Item, PASSVEC3( Item->s.v.origin ) );
	VectorCopy( PROG_TO_EDICT( Item->s.v.owner )->s.v.origin, Item->camangle );

	Item->camangle[2] -= 8;	//Item->s.v.owner->s.v.origin - '0 0 8';
	Item->camdist = 0;
	Item->s.v.velocity[2] = 400;
	Item->s.v.velocity[0] = -50 + g_random(  ) * 100;
	Item->s.v.velocity[1] = -50 + g_random(  ) * 100;
	Item->goal_state = 2;
	Item->s.v.movetype = MOVETYPE_TOSS;
	if ( Item->goal_activation & TFGI_SOLID )
		Item->s.v.solid = SOLID_BBOX;
	else
		Item->s.v.solid = SOLID_TRIGGER;
	if ( Item->mdl )
		setmodel( Item, Item->mdl );
	setsize( Item, PASSVEC3( Item->goal_min ), PASSVEC3( Item->goal_max ) );
	Item->s.v.owner = EDICT_TO_PROG( P );
	if ( PAlive == 1 )
	{
		makevectors( P->s.v.v_angle );
		if ( P->s.v.v_angle[0] )
		{
			Item->s.v.velocity[0] = g_globalvars.v_forward[0] * 400 + g_globalvars.v_up[0] * 200;
			Item->s.v.velocity[1] = g_globalvars.v_forward[1] * 400 + g_globalvars.v_up[1] * 200;
			Item->s.v.velocity[2] = g_globalvars.v_forward[2] * 400 + g_globalvars.v_up[2] * 200;
		} else
		{
			aim( Item->s.v.velocity );
			VectorScale( Item->s.v.velocity, 400, Item->s.v.velocity );
			Item->s.v.velocity[2] = 200;
		}
		Item->s.v.touch = ( func_t ) SUB_Null;
		Item->s.v.nextthink = g_globalvars.time + 0.75;
		Item->s.v.think = ( func_t ) tfgoalitem_droptouch;
	} else
	{
		Item->s.v.touch = ( func_t ) item_tfgoal_touch;
		Item->s.v.nextthink = g_globalvars.time + 5;
		Item->s.v.think = ( func_t ) tfgoalitem_dropthink;
	}
}

void tfgoalitem_remove(  )
{
	gedict_t *te;
	gedict_t *oldself;

	if ( self->goal_state == 1 )
		return;
	if ( self->goal_activation & TFGI_RETURN_REMOVE )
	{
		te = spawn(  );
		te->s.v.enemy = EDICT_TO_PROG( self );
		te->s.v.weapon = 3;
		oldself = self;
		self = te;
		ReturnItem(  );
		self = oldself;
		return;
	}
	dremove( self );
}

void tfgoalitem_checkgoalreturn( gedict_t * Item )
{
	gedict_t *te;

	if ( Item->s.v.impulse )
	{
		te = Findgoal( Item->s.v.impulse );
		if ( te )
		{
			te = Findgoal( Item->s.v.impulse );
			if ( te )
				AttemptToActivate( te, world, Item );
		}
	}
}

void DisplayItemStatus( gedict_t * Goal, gedict_t * Player, gedict_t * Item )
{
	int     flag_time = 0;

	if ( Item->goal_state == 1 )
	{
		if ( Goal->team_str_carried || Goal->non_team_str_carried )
		{
			if ( Player->team_no == Item->owned_by )
				G_sprint( Player, 2, "%s ", Goal->team_str_carried );
			else
				G_sprint( Player, 2, "%s ", Goal->non_team_str_carried );

	        	if( !(Item->s.v.owner) ) //FIXME !!!!!
	        	{
	        	        G_sprint( Player, 2, ".\n");
	        		return;
	        	}

			if ( Player == PROG_TO_EDICT( Item->s.v.owner ) )
				G_sprint( Player, 2, "You.\n" );
			else
				G_sprint( Player, 2, "%s.\n", PROG_TO_EDICT( Item->s.v.owner )->s.v.netname );
		}
	} else
	{
		if ( !VectorCompare( Item->s.v.origin, Item->s.v.oldorigin ) )
		{
			if ( Goal->team_str_moved || Goal->non_team_str_moved )
			{

				if ( Player->team_no == Item->owned_by )
					G_sprint( Player, 2, Goal->team_str_moved );
				else
					G_sprint( Player, 2, Goal->non_team_str_moved );

				if ( tf_data.flag_timer )
				{
					if ( Item->s.v.think == ( func_t ) tfgoalitem_remove )
					{
						flag_time = floor( Item->s.v.nextthink - g_globalvars.time );
					}

					if ( Item->s.v.think == ( func_t ) tfgoalitem_dropthink )
					{
						flag_time =
						    ( Item->s.v.nextthink - g_globalvars.time + Item->pausetime );
					}
					G_sprint( Player, 2, " :%3d\n", flag_time );

				}
			}
		} else
		{
			if ( Goal->team_str_home || Goal->non_team_str_home )
			{
				if ( Player->team_no == Item->owned_by )
					G_sprint( Player, 2, "%s\n", Goal->team_str_home );
				else
					G_sprint( Player, 2, "%s\n", Goal->non_team_str_home );
			}
		}
	}
}
void SP_info_player_team1(  )
{
	CTF_Map = 1;
	self->s.v.classname = "info_player_teamspawn";
	self->team_no = 2;
	self->goal_effects = TFGE_AP;
	self->team_str_home = "ts2";
}

void SP_info_player_team2(  )
{
	CTF_Map = 1;
	self->s.v.classname = "info_player_teamspawn";
	self->team_no = 1;
	self->goal_effects = TFGE_AP;
	self->team_str_home = "ts1";
}

void SP_item_flag_team2(  )
{
	gedict_t *dp;

	CTF_Map = 1;
	UpdateAbbreviations( self );
	trap_precache_sound( "ogre/ogwake.wav" );
	trap_precache_sound( "boss2/pop2.wav" );
	self->s.v.classname = "item_tfgoal";
	self->s.v.netname = "Team 1 Flag";
	self->broadcast = " гот the enemy team's flag!\n";
	self->deathtype = "You've got the enemy flag!\n";
	self->s.v.noise = "ogre/ogwake.wav";
	if ( tf_data.toggleflags & 8192 )
		self->mdl = "progs/tf_stan.mdl";
	else
		self->mdl = "progs/tf_flag.mdl";
	self->s.v.skin = 0;
	setmodel( self, self->mdl );
	self->goal_no = 1;
	self->goal_activation = TFGI_GLOW | TFGI_DROP | TFGI_REMOVE | TFGI_RETURN_REMOVE | TFGI_RETURN_GOAL | TFGI_ITEMGLOWS;
	self->goal_effects = TFGE_AP;
	self->pausetime = 128;
	SetVector( self->goal_min, -16, -16, -24 );
	SetVector( self->goal_max, 16, 16, 32 );
	setsize( self, PASSVEC3( self->goal_min ), PASSVEC3( self->goal_max ) );
	self->s.v.touch = ( func_t ) item_tfgoal_touch;
	self->goal_state = 2;
	self->s.v.solid = SOLID_TRIGGER;
	setorigin( self, PASSVEC3( self->s.v.origin ) );
	self->s.v.nextthink = g_globalvars.time + 0.2;
	self->s.v.think = ( func_t ) TF_PlaceItem;
	dp = spawn(  );
	VectorCopy( self->s.v.origin, dp->s.v.origin );
	dp->s.v.classname = "info_tfgoal";
	dp->goal_activation = TFGA_TOUCH;
	dp->team_no = 1;
	dp->items_allowed = 2;
	dp->goal_no = 3;
	dp->goal_effects = TFGE_AP | TFGE_AP_TEAM;
	dp->broadcast = " цаптуред the enemy flag!\n";
	dp->s.v.message = "You цаптуред the enemy flag!\n";
	dp->s.v.noise = "boss2/pop2.wav";
	dp->goal_result = TFGR_ADD_BONUSES;
	dp->activate_goal_no = 5;
	dp->axhitme = 2;
	dp->count = 10;
	dp->s.v.frags = 10;
	dp->s.v.solid = SOLID_TRIGGER;
	dp->goal_state = 2;
	SetVector( dp->goal_min, -16, -16, -24 );
	SetVector( dp->goal_max, 16, 16, 32 );
	setsize( dp, PASSVEC3( dp->goal_min ), PASSVEC3( dp->goal_max ) );
	dp->s.v.nextthink = g_globalvars.time + 0.2;
	dp->s.v.think = ( func_t ) TF_PlaceGoal;
	dp = spawn(  );
	VectorCopy( dp->s.v.origin, dp->s.v.origin );
	dp->s.v.classname = "info_tfgoal";
	dp->goal_effects = TFGE_AP;
	dp->s.v.frags = 5;
	dp->goal_activation = 0;
	dp->goal_no = 5;
	dp->s.v.solid = SOLID_NOT;
	dp->goal_state = 2;
	SetVector( dp->goal_min, -16, -16, -24 );
	SetVector( dp->goal_max, 16, 16, 32 );
	setsize( dp, PASSVEC3( dp->goal_min ), PASSVEC3( dp->goal_max ) );
	dp->s.v.nextthink = g_globalvars.time + 0.2;
	dp->s.v.think = ( func_t ) TF_PlaceGoal;
}

void SP_item_flag_team1(  )
{
	gedict_t *dp;

	CTF_Map = 1;
	UpdateAbbreviations( self );
	trap_precache_sound( "ogre/ogwake.wav" );
	trap_precache_sound( "boss2/pop2.wav" );
	self->s.v.classname = "item_tfgoal";
	self->s.v.netname = "Team 2 Flag";
	self->broadcast = " гот the enemy team's flag!\n";
	self->deathtype = "You've got the enemy flag!\n";
	self->s.v.noise = "ogre/ogwake.wav";
	if ( tf_data.toggleflags & 8192 )
		self->mdl = "progs/tf_stan.mdl";
	else
		self->mdl = "progs/tf_flag.mdl";
	setmodel( self, self->mdl );
	self->s.v.skin = 1;
	self->goal_no = 2;
	self->goal_activation = TFGI_GLOW | TFGI_DROP | TFGI_REMOVE | TFGI_RETURN_REMOVE | TFGI_RETURN_GOAL | TFGI_ITEMGLOWS;
	self->goal_effects = TFGE_AP;
	self->pausetime = 128;
	SetVector( self->goal_min, -16, -16, -24 );
	SetVector( self->goal_max, 16, 16, 32 );
	setsize( self, PASSVEC3( self->goal_min ), PASSVEC3( self->goal_max ) );
	self->s.v.touch = ( func_t ) item_tfgoal_touch;
	self->goal_state = 2;
	self->s.v.solid = SOLID_TRIGGER;
	setorigin( self, PASSVEC3( self->s.v.origin ) );
	self->s.v.nextthink = g_globalvars.time + 0.2;
	self->s.v.think = ( func_t ) TF_PlaceItem;
	dp = spawn(  );
	VectorCopy( self->s.v.origin, dp->s.v.origin );
	dp->s.v.classname = "info_tfgoal";
	dp->goal_activation = TFGA_TOUCH;
	dp->team_no = 2;
	dp->items_allowed = 1;
	dp->goal_no = 4;
	dp->goal_effects = TFGE_AP | TFGE_AP_TEAM;
	dp->broadcast = " цаптуред the enemy flag!\n";
	dp->s.v.message = "You цаптуред the enemy flag!\n";
	dp->s.v.noise = "boss2/pop2.wav";
	dp->goal_result = TFGR_ADD_BONUSES;
	dp->activate_goal_no = 6;
	dp->axhitme = 1;
	dp->count = 10;
	dp->s.v.frags = 10;
	dp->s.v.solid = SOLID_TRIGGER;
	dp->goal_state = 2;
	SetVector( dp->goal_min, -16, -16, -24 );
	SetVector( dp->goal_max, 16, 16, 32 );
	setsize( dp, PASSVEC3( dp->goal_min ), PASSVEC3( dp->goal_max ) );
	dp->s.v.nextthink = g_globalvars.time + 0.2;
	dp->s.v.think = ( func_t ) TF_PlaceGoal;
	dp = spawn(  );
	VectorCopy( dp->s.v.origin, dp->s.v.origin );
	dp->s.v.classname = "info_tfgoal";
	dp->goal_effects = TFGE_AP;
	dp->s.v.frags = 5;
	dp->goal_activation = 0;
	dp->goal_no = 6;
	dp->s.v.solid = SOLID_NOT;
	dp->goal_state = 2;
	SetVector( dp->goal_min, -16, -16, -24 );
	SetVector( dp->goal_max, 16, 16, 32 );
	setsize( dp, PASSVEC3( dp->goal_min ), PASSVEC3( dp->goal_max ) );
	dp->s.v.nextthink = g_globalvars.time + 0.2;
	dp->s.v.think = ( func_t ) TF_PlaceGoal;
}

void CTF_FlagCheck(  )
{
	gedict_t *te;
	float   flagcount;
	float   pos;

	flagcount = 0;
	te = trap_find( world, FOFS( s.v.classname ), "item_tfgoal" );
	while ( te )
	{
		if ( te->goal_no == 1 )
		{
			pos = trap_pointcontents( PASSVEC3( te->s.v.origin ) );
			if ( pos == -2 || pos == -6 )
			{
				G_conprintf( "*****BUG*****\nFlag(s) outside world.\nPlease report this.\n" );
				te->s.v.nextthink = g_globalvars.time + 0.2;
				te->s.v.think = ( func_t ) tfgoalitem_remove;
			}
			flagcount = flagcount + 1;
		} else
		{
			if ( te->goal_no == 2 )
			{
				pos = trap_pointcontents( PASSVEC3( te->s.v.origin ) );
				if ( pos == -2 || pos == -6 )
				{
					G_conprintf( "*****BUG*****\nFlag(s) outside world.\nPlease report this.\n" );
					te->s.v.nextthink = g_globalvars.time + 0.2;
					te->s.v.think = ( func_t ) tfgoalitem_remove;
				}
				flagcount = flagcount + 1;
			}
		}
		te = trap_find( te, FOFS( s.v.classname ), "item_tfgoal" );
	}
	if ( flagcount != 2 )
		G_conprintf( "*****BUG*****\nFlag(s) missing.\nPlease report this.\n" );
	self->s.v.nextthink = g_globalvars.time + 30;
}

void ForceRespawn( gedict_t * P )
{
	gedict_t *spot;
	gedict_t *te;
	gedict_t *oldself;
	vec3_t  tmp;

	oldself = self;
	self = P;
	spot = SelectSpawnPoint(  );
	if ( self->playerclass )
		spawn_tdeath( spot->s.v.origin, self );
	self->observer_list = spot;
	VectorCopy( spot->s.v.origin, self->s.v.origin );
	self->s.v.origin[2] += 1;
	VectorCopy( spot->s.v.angles, self->s.v.angles );
	self->s.v.fixangle = 1;
	if ( streq( spot->s.v.classname, "info_player_teamspawn" ) && tf_data.cb_prematch_time < g_globalvars.time )
	{
		if ( spot->s.v.items )
		{
			te = Finditem( spot->s.v.items );
			if ( te != world )
				tfgoalitem_GiveToPlayer( te, self, self );
			if ( !( spot->goal_activation & TFSP_MULTIPLEITEMS ) )
				spot->s.v.items = 0;
		}
		if ( spot->s.v.message )
		{
			CenterPrint( self, spot->s.v.message );
			if ( !( spot->goal_activation & TFSP_MULTIPLEMSGS ) )
				spot->s.v.message = NULL;
		}
		if ( spot->activate_goal_no )
		{
			te = Findgoal( spot->activate_goal_no );
			if ( te )
				AttemptToActivate( te, self, spot );
		}
		if ( spot->goal_effects == TFGE_AP )
		{
			spot->s.v.classname = "deadpoint";
			spot->team_str_home = "";
			spot->s.v.nextthink = g_globalvars.time + 1;
			spot->s.v.think = ( func_t ) SUB_Remove;
		}
	}
	if ( deathmatch || coop )
	{
		makevectors( self->s.v.angles );
		if ( self->playerclass )
		{
			VectorScale( g_globalvars.v_forward, 20, tmp );
			VectorAdd( tmp, self->s.v.origin, tmp );
			spawn_tfog( tmp );
		}
	}
	self = oldself;
}

void DropGoalItems(  )
{
	gedict_t *te;

	newmis = spawn(); //FIX ME??? no need this?
	g_globalvars.newmis = EDICT_TO_PROG( newmis );
	makevectors( self->s.v.v_angle );
	VectorNormalize( g_globalvars.v_forward );
	VectorScale( g_globalvars.v_forward, 64, g_globalvars.v_forward );

	VectorAdd( g_globalvars.v_forward, self->s.v.origin, newmis->s.v.origin );
	te = trap_find( world, FOFS( s.v.classname ), "item_tfgoal" );
	while ( te )
	{
		if ( te->s.v.owner == EDICT_TO_PROG( self ) )
		{
			if ( ( te->goal_activation & TFGI_ALLOW_DROP ) || tf_data.allow_drop_goal )
				tfgoalitem_RemoveFromPlayer( te, self, 2 );
		}
		te = trap_find( te, FOFS( s.v.classname ), "item_tfgoal" );
	}
	dremove( newmis );
}
