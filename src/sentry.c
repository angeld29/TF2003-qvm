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
 *  $Id: sentry.c,v 1.26 2005-11-29 14:22:43 AngelD Exp $
 */
#include "g_local.h"
#include "sentry.h"

void    FireSentryLighting( gedict_t * targ )  ;
void    FireBullets( float shotcount, vec3_t dir, float spread_x, float spread_y, float spread_z );


void ai_face(  )
{
	vec3_t  vtemp;

	VectorSubtract( PROG_TO_EDICT( self->s.v.enemy )->s.v.origin, self->s.v.origin, vtemp );
	self->s.v.ideal_yaw = vectoyaw( vtemp );
	self->s.v.ideal_yaw = anglemod( self->s.v.ideal_yaw );
	changeyaw( self );
}

//===========================
// Level 1 Sentry Gun Frames
void lvl1_sentry_stand(  )
{
	self->s.v.frame = 0;
	self->s.v.think = ( func_t ) lvl1_sentry_stand;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	Sentry_Rotate(  );
}

void lvl1_sentry_atk1(  )
{
	gedict_t *enemy = PROG_TO_EDICT( self->s.v.enemy );

	self->s.v.frame = 1;
	self->s.v.think = ( func_t ) lvl1_sentry_atk3;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	ai_face(  );
	if ( enemy == world || enemy->is_feigning || enemy->s.v.health <= 0 || !visible( enemy )
	     || enemy->has_disconnected == 1 )
		lvl1_sentry_stand(  );
	else
	{
		if ( self->s.v.ammo_shells <= 0 )
			lvl1_sentry_stand(  );
		else
		{
			if ( !Sentry_Fire(  ) )
				lvl1_sentry_atk3(  );
		}
	}
}

void lvl1_sentry_atk2(  )
{
	self->s.v.frame = 2;
	self->s.v.think = ( func_t ) lvl1_sentry_atk3;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	ai_face(  );
	Sentry_Fire(  );
}

void lvl1_sentry_atk3(  )
{
	self->s.v.frame = 0;
	self->s.v.think = ( func_t ) lvl1_sentry_atk1;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	ai_face(  );
}


//===========================
// Level 2 Sentry Gun Frames
void lvl2_sentry_stand(  )
{
	self->s.v.frame = 3;
	self->s.v.think = ( func_t ) lvl2_sentry_stand;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	Sentry_Rotate(  );
}

void lvl2_sentry_atk1(  )
{
	gedict_t *enemy = PROG_TO_EDICT( self->s.v.enemy );

	self->s.v.frame = 4;
	self->s.v.think = ( func_t ) lvl2_sentry_atk2;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	ai_face(  );
	if ( enemy == world || enemy->is_feigning || enemy->s.v.health <= 0 || !visible( enemy )
	     || enemy->has_disconnected == 1 )
		lvl2_sentry_stand(  );
	else
	{
		if ( self->s.v.ammo_shells <= 0 )
			lvl2_sentry_stand(  );
		else
		{
			if ( !Sentry_Fire(  ) )
				lvl2_sentry_atk3(  );
		}
	}
}

void lvl2_sentry_atk2(  )
{
	self->s.v.frame = 5;
	self->s.v.think = ( func_t ) lvl2_sentry_atk3;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	ai_face(  );
	Sentry_Fire(  );
}

void lvl2_sentry_atk3(  )
{
	self->s.v.frame = 3;
	self->s.v.think = ( func_t ) lvl2_sentry_atk1;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	ai_face(  );
	Sentry_Fire(  );
}


//===========================
// Level 3 Sentry Gun Frames
void lvl3_sentry_stand(  )
{
	self->s.v.frame = 6;
	self->s.v.think = ( func_t ) lvl3_sentry_stand;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	Sentry_Rotate(  );
}

void lvl3_sentry_atk1(  )
{
	gedict_t *enemy = PROG_TO_EDICT( self->s.v.enemy );

	self->s.v.frame = 7;
	self->s.v.think = ( func_t ) lvl3_sentry_atk2;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	ai_face(  );
	if ( enemy == world || enemy->is_feigning || enemy->s.v.health <= 0 || !visible( enemy )
	     || enemy->has_disconnected == 1 )
		lvl3_sentry_stand(  );
	else
	{
		if ( self->s.v.ammo_shells <= 0 && self->s.v.ammo_rockets <= 0 )
			lvl3_sentry_stand(  );
		else
		{
			if ( !Sentry_Fire(  ) )
				lvl3_sentry_atk3(  );
		}
	}
}
void lvl3_sentry_atk2(  )
{
	self->s.v.frame = 8;
	self->s.v.think = ( func_t ) lvl3_sentry_atk3;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	ai_face(  );
	Sentry_Fire(  );
}

void lvl3_sentry_atk3(  )
{
	self->s.v.frame = 6;
	self->s.v.think = ( func_t ) lvl3_sentry_atk1;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	ai_face(  );
	Sentry_Fire(  );
}

//=============
void Sentry_Rotate(  )
{
	float   ay;

	self->s.v.effects = ( int ) self->s.v.effects - ( ( int ) self->s.v.effects & EF_DIMLIGHT );
	CheckSentry( self );
	if ( tf_data.sg_newfind)
	{
		if ( Sentry_FindTarget_Angel(  ) )
			return;
	} else
	{
		if ( Sentry_FindTarget(  ) )
			return;
	}
	if ( !self->heat )
	{
		self->s.v.ideal_yaw = self->waitmin;
		changeyaw( self );
		ay = ( int ) anglemod( self->s.v.angles[1] );
		if ( ay == ( int ) self->waitmin )
		{
			CheckBelowBuilding( self->trigger_field );
			self->heat = 1;
			if ( g_random(  ) < 0.1 )
				sound( self, 3, "weapons/turridle.wav", 1, 1 );
		}
	} else
	{
		self->s.v.ideal_yaw = self->waitmax;
		changeyaw( self );
		ay = ( int ) anglemod( self->s.v.angles[1] );
		if ( ay == ( int ) self->waitmax )
		{
			CheckBelowBuilding( self->trigger_field );
			self->heat = 0;
		}
	}
}

//////////////////////////////////////////////////////////////////////
int CheckTarget( gedict_t * Target )
{
	int     r;

	if ( tg_data.sg_allow_find == TG_SG_FIND_IGNORE_ALL )
		return 0;
	if ( !Target )
		return 0;
	if ( Target == self )
		return 0;
	if ( Target->has_disconnected == 1 )
		return 0;
	if ( !Target->s.v.takedamage )
		return 0;
	if ( Target->is_feigning )
		return 0;
	if ( ( int ) Target->s.v.flags & FL_NOTARGET )
		return 0;
	if ( ( int ) Target->s.v.items & IT_INVISIBILITY )
		return 0;
	if ( !visible( Target ) )
		return 0;
	r = range( Target );
	if ( r == 3 )
		return 0;
	else
	{
		if ( r == 2 && !infront( Target ) )
			return 0;
	}

	if ( tg_data.sg_allow_find == TG_SG_FIND_IGNORE_OFF )
		return 1;

	if ( teamplay )
	{
		if (  self->team_no && TeamFortress_isTeamsAllied (Target->team_no , self->team_no) )
		{
			if ( tg_data.sg_allow_find == TG_SG_FIND_IGNORE_TEAM )
				return 0;
		}
		if ( self->team_no && TeamFortress_isTeamsAllied(Target->undercover_team , self->team_no))
		{
			if ( tg_data.sg_allow_find == TG_SG_FIND_IGNORE_TEAM )
				return 0;
		}
	}
	if ( Target == self->real_owner )
	{
		if ( tg_data.sg_allow_find == TG_SG_FIND_IGNORE_OWNER )
			return 0;
	}


	return 1;
}

//////////////////////////////////////////////////////////////////////
int Sentry_FindTarget_Angel(  )
{
	gedict_t *client;
	gedict_t *enemy = PROG_TO_EDICT( self->s.v.enemy );

	if ( enemy != world )
	{
		if ( CheckTarget( enemy ) )
		{
			if ( g_globalvars.time > self->height )
			{
				Sentry_FoundTarget(  );
				return 1;
			} else
				return 0;
		}
	}
	for ( client = world; (client = trap_find( client, FOFS( s.v.classname ), "player" )); )
	{
		if ( CheckTarget( client ) )
		{
			self->s.v.enemy = EDICT_TO_PROG( client );
			self->height = g_globalvars.time + 0.1 * ( int ) ( g_random(  ) * tf_data.sgppl );
//     self.height = g_globalvars.time + 0.1 * rint(sgppl);
			if ( self->height == g_globalvars.time )
			{
				Sentry_FoundTarget(  );
				return 1;
			}
			return 0;
		}
	}
	self->s.v.enemy = EDICT_TO_PROG( world );
	return 0;
}

//////////////////////////////////////////////////////////////////////
int Sentry_FindTarget(  )
{
	gedict_t *client;
	float   r;
	float   gotone;
	float   loopc;

	r = 0;
	loopc = 0;
	gotone = 0;
	while ( loopc < 6 && !gotone )
	{
		client = checkclient(  );
		gotone = CheckTarget( client );
		loopc = loopc + 1;
	}
	if ( !gotone )
		return 0;
	self->s.v.enemy = EDICT_TO_PROG( client );
	if ( strneq( PROG_TO_EDICT( self->s.v.enemy )->s.v.classname, "player" ) )
	{
		self->s.v.enemy = PROG_TO_EDICT( self->s.v.enemy )->s.v.enemy;
		if ( strneq( PROG_TO_EDICT( self->s.v.enemy )->s.v.classname, "player" ) )
		{
			self->s.v.enemy = EDICT_TO_PROG( world );
			return 0;
		}
	}
	Sentry_FoundTarget(  );
	return 1;
}

void Sentry_FoundTarget(  )
{
	if ( self->s.v.ammo_shells > 0 || ( self->s.v.ammo_rockets > 0 && self->s.v.weapon == 3 ) )
		sound( self, 2, "weapons/turrspot.wav", 1, 1 );
	Sentry_HuntTarget(  );
	if ( self->super_damage_finished < g_globalvars.time )
		self->super_damage_finished = g_globalvars.time + 0.5;
}

void Sentry_HuntTarget(  )
{
	vec3_t  vtemp;

	self->s.v.goalentity = self->s.v.enemy;
	if ( self->s.v.weapon == 1 )
		self->s.v.think = ( func_t ) lvl1_sentry_atk1;
	else
	{
		if ( self->s.v.weapon == 2 )
			self->s.v.think = ( func_t ) lvl2_sentry_atk1;
		else
			self->s.v.think = ( func_t ) lvl3_sentry_atk1;
	}

	VectorSubtract( PROG_TO_EDICT( self->s.v.enemy )->s.v.origin, self->s.v.origin, vtemp );
	self->s.v.ideal_yaw = vectoyaw( vtemp );
	self->s.v.nextthink = g_globalvars.time + 0.1;
	SUB_AttackFinished( 1 ); // wait a while before first attack
}

void Sentry_Pain( struct gedict_s *attacker, float take )
{
        // Update the owner's status bar
	self->real_owner->StatusRefreshTime = g_globalvars.time + 0.2;
}

void Sentry_Explode(  )
{
	ThrowGib( "progs/tgib1.mdl", -70 );
	ThrowGib( "progs/tgib2.mdl", -70 );
	ThrowGib( "progs/tgib3.mdl", -70 );
	if ( self->real_owner->has_disconnected != 1 )
	{
		tf_data.deathmsg = DMSG_SG_EXPLODION;
		T_RadiusDamage( self, self->real_owner, 75 + self->s.v.ammo_rockets * 8, self );
	}
	if ( streq( self->s.v.classname, "building_sentrygun_base" ) )
	{
		if ( self->oldenemy )
			dremove( self->oldenemy );
	} else
		dremove( self->trigger_field );
	trap_WriteByte( MSG_MULTICAST, SVC_TEMPENTITY );
	trap_WriteByte( MSG_MULTICAST, TE_EXPLOSION );
	trap_WriteCoord( MSG_MULTICAST, self->s.v.origin[0] );
	trap_WriteCoord( MSG_MULTICAST, self->s.v.origin[1] );
	trap_WriteCoord( MSG_MULTICAST, self->s.v.origin[2] );
	trap_multicast( PASSVEC3( self->s.v.origin ), 1 );
	BecomeExplosion(  );
}

void SG_Static(  )
{
	if ( self->s.v.health == 0 )
	{
		setmodel( self, "progs/turrgun.mdl" );
		self->s.v.health = -1;
	} else
	{
		setmodel( self, "" );
		self->s.v.health = 0;
	}
	self->s.v.nextthink = g_globalvars.time + 0.5;
}

void Sentry_Die(  )
{
	G_sprint( self->real_owner, 2, "Your sentry gun was destroyed.\n" );
	if ( self->has_sentry && tg_data.tg_enabled )
	{
		self->s.v.health = 0;
		setmodel( self, "" );
		setmodel( self->trigger_field, "" );
		self->s.v.think = ( func_t ) SG_Static;
		self->s.v.nextthink = g_globalvars.time + 0.5;
		self->s.v.effects = self->s.v.effects - ( (int)self->s.v.effects & 8 );
		self->s.v.movetype = MOVETYPE_NONE;
		self->s.v.solid = SOLID_NOT;
		self->trigger_field->s.v.movetype = MOVETYPE_NONE;
		self->trigger_field->s.v.solid = SOLID_NOT;
		ThrowGib( "progs/tgib1.mdl", -70 );
		ThrowGib( "progs/tgib2.mdl", -70 );
		ThrowGib( "progs/tgib3.mdl", -70 );
		tf_data.deathmsg = DMSG_SG_EXPLODION;
		T_RadiusDamage( self, self->real_owner, 75 + self->s.v.ammo_rockets * 8, self );
		trap_WriteByte( MSG_MULTICAST, SVC_TEMPENTITY );
		trap_WriteByte( MSG_MULTICAST, TE_EXPLOSION );
		trap_WriteCoord( MSG_MULTICAST, self->s.v.origin[0] );
		trap_WriteCoord( MSG_MULTICAST, self->s.v.origin[1] );
		trap_WriteCoord( MSG_MULTICAST, self->s.v.origin[2] );
		trap_multicast( PASSVEC3( self->s.v.origin ), 1 );
	} else
	{
		self->real_owner->has_sentry -= 1;
#ifdef HAS_SENTRY_CHECK
		if(!tg_data.tg_enabled && self->real_owner->has_sentry)
		{
			G_dprintf("Error SentryDie owner->has_sentry !=0 %d\n",self->real_owner->has_sentry);
			G_sprint(self->real_owner,2,"Error SentryDie owner->has_sentry !=0 %d\n",self->real_owner->has_sentry);
			self->real_owner->has_sentry = 0;
		}
#endif
		self->s.v.think = ( func_t ) Sentry_Explode;
		self->s.v.nextthink = g_globalvars.time + 0.1;
	}
}

void FireSentryBulletsNEW( int shotcount, gedict_t * targ, float spread_x, float spread_y, float spread_z )
{
	vec3_t  src;
	vec3_t  dst;
	vec3_t  dir,  tmp, norm_dir;

	makevectors( self->s.v.v_angle );

	VectorAdd( self->s.v.origin, self->s.v.view_ofs, src );
	VectorAdd( targ->s.v.origin, targ->s.v.view_ofs, dst );
	VectorSubtract( dst, src, dir );

	normalize(dir,norm_dir);
	
	//чтобы не попадать в подставку
	traceline( PASSVEC3( src ), PASSVEC3( dst ), 0, self );
	
	if( (PROG_TO_EDICT(g_globalvars.trace_ent) == self->trigger_field) && vlen(dir) > 100 )
	{
		 VectorScale(norm_dir, 60, tmp);
		 VectorAdd(src,tmp,src);
	}

	ClearMultiDamage(  );
	traceline( PASSVEC3( src ), PASSVEC3( dst ), 0, self );

	VectorScale( norm_dir, 4, puff_org );
	VectorSubtract( g_globalvars.trace_endpos, puff_org, puff_org );

	for (  ; shotcount > 0  ; shotcount-- )
	{
	        // для каждого выстрела определяем trace_ent trace_endpos
	        // т.к.выстрелы могут убрать препятствия
	        traceline( PASSVEC3( src ), PASSVEC3( dst ), 0, self );

	        // TraceAttack требует нормализованый вектор для корректного определения blood_org
	        if(g_globalvars.trace_fraction != 1)
	        	TraceAttack( 4, norm_dir ); 
	}
	ApplyMultiDamage(  );
	Multi_Finish(  );
}
/*
====================================
FireSentryBulletsMTFL2
====================================
*/
void FireSentryBulletsMTFL2( int shotcount, gedict_t * targ, float spread_x, float spread_y, float spread_z )
{
	vec3_t  direction;
	vec3_t  src;
	vec3_t  dst;
	vec3_t  dir, end, tmp;

	makevectors( self->s.v.v_angle );

	VectorAdd( self->s.v.origin, self->s.v.view_ofs, src );
	VectorAdd( targ->s.v.origin, targ->s.v.view_ofs, dst );
	VectorSubtract( dst, src, dir );

	ClearMultiDamage(  );
	VectorScale( dir, 2048, end );
	VectorAdd( end, src, end );
	traceline( PASSVEC3( src ), PASSVEC3( end ), 0, self );

	VectorScale( dir, 4, puff_org );
	VectorSubtract( g_globalvars.trace_endpos, puff_org, puff_org );
	while ( shotcount > 0 )
	{
		VectorScale( g_globalvars.v_right, crandom(  ) * spread_x, tmp );
		VectorScale( g_globalvars.v_up, crandom(  ) * spread_y, direction );
		VectorAdd( direction, tmp, direction );
		VectorAdd( direction, dir, direction );
		VectorNormalize( direction );

		VectorScale( direction, 2048, end );
		VectorAdd( end, src, end );
		traceline( PASSVEC3( src ), PASSVEC3( end ), 0, self );


		if ( g_globalvars.trace_fraction != 1 )
		{
			TraceAttack( 4, direction );
		}
		shotcount = shotcount - 1;
	}
	ApplyMultiDamage(  );
	Multi_Finish(  );
}

//////////////////////////////////////////////////////////////////////////////
void    FireSentryLighting( gedict_t * targ )  
{
	vec3_t  src;
	vec3_t  dst;
	vec3_t  dir, end,direction, tmp,norm_dir;
	gedict_t*trace_ent;

	switch( tf_data.sg_sfire )
	{
		case SG_SFIRE_NEW:
			VectorCopy( self->s.v.angles, self->s.v.v_angle );
			makevectors( self->s.v.v_angle );

                	VectorAdd( self->s.v.origin, self->s.v.view_ofs, src );
                	VectorAdd( targ->s.v.origin, targ->s.v.view_ofs, dst );
                	VectorSubtract( dst, src, dir );

                	normalize(dir,norm_dir);
                	traceline( PASSVEC3( src ), PASSVEC3( dst ), 0, self );

               		if( (PROG_TO_EDICT(g_globalvars.trace_ent) == self->trigger_field) && vlen(dir) > 100 )
                	{
                		 VectorScale(norm_dir, 60, tmp);
                		 VectorAdd(src,tmp,src);
                	}
                	VectorCopy(dst,end);
			break;
		case SG_SFIRE_MTFL2:
        		VectorCopy( self->s.v.angles, self->s.v.v_angle );
        		makevectors( self->s.v.v_angle );
        		VectorAdd( self->s.v.origin, self->s.v.view_ofs, src );
        		VectorAdd( targ->s.v.origin, targ->s.v.view_ofs, dst );

        		VectorSubtract( dst, src, dir );
        		VectorNormalize( direction );
        		VectorScale( dir, 2048, end );
        		VectorAdd( end, src, end );
        		break;

		case SG_SFIRE_MTFL1:
			VectorCopy( self->s.v.angles, self->s.v.v_angle );
		case SG_SFIRE_281:
        		makevectors( self->s.v.v_angle );
        		VectorScale( g_globalvars.v_forward, 10, src );
        		VectorAdd( self->s.v.origin, src, src );
        		src[2] = self->s.v.absmin[2] + self->s.v.size[2] * 0.7;
        		VectorSubtract( targ->s.v.origin, self->s.v.origin, dir );
        		VectorScale( dir, 2048, end );
        		VectorAdd( end, src, end );
        		break;
		default:
			return;
	}
	g_globalvars.trace_ent = 0;

	traceline( PASSVEC3( src ), PASSVEC3( end ), 0, self );
	

	trap_WriteByte( MSG_MULTICAST, SVC_TEMPENTITY );
	trap_WriteByte( MSG_MULTICAST, TE_LIGHTNING1 );
	WriteEntity( MSG_MULTICAST, self );
	trap_WriteCoord( MSG_MULTICAST, src[0] );
	trap_WriteCoord( MSG_MULTICAST, src[1] );
	trap_WriteCoord( MSG_MULTICAST, src[2] );
	trap_WriteCoord( MSG_MULTICAST, g_globalvars.trace_endpos[0] );
	trap_WriteCoord( MSG_MULTICAST, g_globalvars.trace_endpos[1] );
	trap_WriteCoord( MSG_MULTICAST, g_globalvars.trace_endpos[2] );
	trap_multicast( PASSVEC3( src ), 1 );
	if ( g_globalvars.trace_ent )
	{
		trace_ent = PROG_TO_EDICT(g_globalvars.trace_ent);
		if ( streq( trace_ent->s.v.classname, "player" ) )
		{
            switch((int)( g_random(  ) * 30) )  
			{
				case 0:
					sound( trace_ent, 2, "player/pain1.wav" , 1, 1 );
					break;
				case 1:
					sound( trace_ent, 2, "player/pain1.wav" , 1, 1 );
					break;
				case 2:
					sound( trace_ent, 2, "player/pain2.wav" , 1, 1 );
					break;
				case 3:
					sound( trace_ent, 2, "player/pain3.wav" , 1, 1 );
					break;
				case 4:
					sound( trace_ent, 2, "player/pain4.wav" , 1, 1 );
					break;
				case 5:
					sound( trace_ent, 2, "player/pain5.wav" , 1, 1 );
					break;
				case 6:
					sound( trace_ent, 2, "player/pain6.wav" , 1, 1 );
					break;
				default:break;
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////////
void LaunchSGRocketOLD(gedict_t*targ)
{
		vec3_t   vtemp;

       	newmis = spawn(  );
       	g_globalvars.newmis = EDICT_TO_PROG( newmis );
       	newmis->s.v.owner = EDICT_TO_PROG( self );
       	newmis->s.v.movetype = MOVETYPE_FLYMISSILE;
       	newmis->s.v.solid = SOLID_BBOX;


       	newmis->s.v.weapon = DMSG_SENTRYGUN_ROCKET;
       	newmis->s.v.touch = ( func_t ) T_MissileTouch;
       	newmis->s.v.nextthink = g_globalvars.time + 5;
       	newmis->s.v.think = ( func_t ) SUB_Remove;
       	
       	setmodel( newmis, "progs/missile.mdl" );
       	setsize( newmis, 0, 0, 0, 0, 0, 0 );

       	VectorSubtract( targ->s.v.origin, self->s.v.origin, newmis->s.v.velocity );
       	VectorNormalize( newmis->s.v.velocity );
       	VectorScale( newmis->s.v.velocity, 800, newmis->s.v.velocity );
       	
       	vectoangles( newmis->s.v.velocity, newmis->s.v.angles );

       	makevectors( self->s.v.angles);
       	VectorScale( g_globalvars.v_forward, 8, vtemp );
       	VectorAdd( vtemp, self->s.v.origin, vtemp );
       	vtemp[2] += 16;
       	setorigin( newmis, PASSVEC3( vtemp ) );


}
//////////////////////////////////////////////////////////////////////////////
int CheckAim(vec3_t src, gedict_t*targ, float vel, float time,vec3_t vTarget)
{
	vec3_t  vtemp;
	float dist;

	
        VectorScale(targ->s.v.velocity,time ,vTarget);
        VectorAdd(vTarget, targ->s.v.origin, vTarget);

//        VectorScale(self->accel,time*time*0.5,vtemp);
//        VectorAdd(vTarget, vtemp, vTarget);

	if(!((int)targ->s.v.flags & FL_ONGROUND ))
	{
		
		vTarget[2] -= 0.5*time*time*sv_gravity;
	}

	VectorSubtract(vTarget, src, vtemp);
	dist = vlen(vtemp);
	if( fabs(vel*(time+0.05) - dist) < 40)
		return 1;
	else 
		return 0;

}
void LaunchSGRocketNEW(gedict_t*targ)
{
	vec3_t   vtemp,dir,norm_dir;
	vec3_t  src;
	vec3_t  dst;
	float time;

       	newmis = spawn(  );
       	g_globalvars.newmis = EDICT_TO_PROG( newmis );
       	newmis->s.v.owner = EDICT_TO_PROG( self );
       	newmis->s.v.movetype = MOVETYPE_FLYMISSILE;
       	newmis->s.v.solid = SOLID_BBOX;


       	newmis->s.v.weapon = DMSG_SENTRYGUN_ROCKET;
       	newmis->s.v.touch = ( func_t ) T_MissileTouch;
       	newmis->s.v.nextthink = g_globalvars.time + 5;
       	newmis->s.v.think = ( func_t ) SUB_Remove;
       	
       	setmodel( newmis, "progs/missile.mdl" );
       	setsize( newmis, 0, 0, 0, 0, 0, 0 );

//try fire to targ origin

       	makevectors( self->s.v.angles);
       	VectorScale( g_globalvars.v_forward, 8, vtemp );
       	VectorAdd( vtemp, self->s.v.origin, src );
       	src[2] += 16;

	
	VectorCopy( targ->s.v.origin, dst );
	VectorSubtract( dst, src, dir );

	normalize(dir,norm_dir);

	//чтобы не попадать в подставку
	traceline( PASSVEC3( src ), PASSVEC3( dst ), 0, self );
	
	if( (PROG_TO_EDICT(g_globalvars.trace_ent) == self->trigger_field) && vlen(dir) > 100 )
	{
		 VectorScale(norm_dir, 60, vtemp);
		 VectorAdd(src,vtemp,src);
	}

	traceline( PASSVEC3( src ), PASSVEC3( dst ), 1, self );

	if( g_globalvars.trace_fraction != 1 && vlen(dir) > 100 ) //FIX ME can shot trown wall??
	{
		 VectorScale(norm_dir, 60, vtemp);
		 VectorAdd(src,vtemp,src);
	}


       	setorigin( newmis, PASSVEC3( src ) );
//prediction
        for ( time = 0 ; time < 3 ; time+=0.05 )
        {
        	if(CheckAim(src,targ,800,time,vtemp))
        	{
        	        traceline( PASSVEC3( src ), PASSVEC3( vtemp ), 1, self );
        	        if( g_globalvars.trace_fraction != 1 )
        	        	continue;
        	        VectorCopy(vtemp,dst);
        		break;
        	}
        }
        if((int)targ->s.v.flags & FL_ONGROUND )
        {
 	        VectorAdd(dst,targ->s.v.mins,vtemp);

 	        traceline( PASSVEC3( src ), PASSVEC3( vtemp ), 1, self );
 	        if(g_globalvars.trace_fraction == 1)
 	        	VectorCopy(vtemp,dst);
        }else
        {
#define MAX_BOTTOM_DISTANCE  100
                VectorCopy(dst,vtemp);
                vtemp[2] -= MAX_BOTTOM_DISTANCE;
                traceline( PASSVEC3( dst ), PASSVEC3( vtemp ), 1, self );
                if(g_globalvars.trace_fraction != 1)
                {
                	vtemp[2] = dst[2] - MAX_BOTTOM_DISTANCE*g_globalvars.trace_fraction;
                	traceline( PASSVEC3( dst ), PASSVEC3( vtemp ), 1, self );
 	        	if(g_globalvars.trace_fraction == 1)
 	        		VectorCopy(vtemp,dst);
                }
        }

#ifdef DEBUG_SG
	trap_WriteByte(MULTICAST_PHS_R, SVC_TEMPENTITY);
	trap_WriteByte(MULTICAST_PHS_R, TE_TAREXPLOSION);
	trap_WriteCoord(MULTICAST_PHS_R, dst[0]);
	trap_WriteCoord(MULTICAST_PHS_R, dst[1]);
	trap_WriteCoord(MULTICAST_PHS_R, dst[2]);
	trap_multicast(PASSVEC3(dst), 1);
#endif

       	VectorSubtract( dst, src, dir );
       	normalize(dir,norm_dir);
       	VectorScale( norm_dir, 800, newmis->s.v.velocity );

       	vectoangles( newmis->s.v.velocity, newmis->s.v.angles );
}

//////////////////////////////////////////////////////////////////////////////
int Sentry_Fire(  )
{
	vec3_t  dir;

	gedict_t *enemy = PROG_TO_EDICT( self->s.v.enemy );

	if ( tg_data.sg_disable_fire )
		return 0;

	self->s.v.effects = ( int ) self->s.v.effects - ( ( int ) self->s.v.effects & 8 );
	VectorSubtract( enemy->s.v.origin, self->s.v.origin, dir );
	if ( self->s.v.ideal_yaw - anglemod( self->s.v.angles[1] ) < -10
	     || self->s.v.ideal_yaw - anglemod( self->s.v.angles[1] ) > 10 )
		return 0;
	if ( enemy->is_feigning == 1 )
		return 0;
	
	// Level 3 Turrets fire rockets every 3 seconds
	if ( self->s.v.weapon == 3 && self->s.v.ammo_rockets > 0 
		&& self->super_damage_finished < g_globalvars.time
	     	&& ( tg_data.sg_fire_type == TG_SG_FIRE_NORMAL )  )
	{
		Sentry_MuzzleFlash(  );
		sound( self, 1, "weapons/rocket1i.wav", 1, 1 );

	        if(tf_data.sg_rfire)
	        	LaunchSGRocketNEW(enemy);
	        else
	        	LaunchSGRocketOLD(enemy); 

		self->super_damage_finished = g_globalvars.time + 3;
#ifndef DEBUG_SG
		self->s.v.ammo_rockets = self->s.v.ammo_rockets - 1;
#endif		
		if ( self->s.v.ammo_rockets == 10 )
			G_sprint( self->real_owner, 2, "Sentry Gun is low on rockets.\n" );
	}
	if ( tg_data.sg_fire_type == TG_SG_FIRE_LIGHTING )
	{
		FireSentryLighting( PROG_TO_EDICT(self->s.v.enemy) );
		return 1;
	}
	tf_data.deathmsg = DMSG_SENTRYGUN_BULLET;

	self->s.v.ammo_shells -= 1;
	if ( self->s.v.ammo_shells < 0 )
	{
		self->s.v.ammo_shells = 0;
		return 0;
	}
	Sentry_MuzzleFlash(  );
	sound( self, 1, "weapons/sniper.wav", 1, 1 );

        switch(tf_data.sg_sfire)
        {
        	
        	case SG_SFIRE_MTFL1:
			VectorCopy( self->s.v.angles, self->s.v.v_angle );
		case SG_SFIRE_281:
			FireBullets( 4, dir, 0.1, 0.1, 0 );
        		break;
        	case SG_SFIRE_MTFL2:
			VectorCopy( self->s.v.angles, self->s.v.v_angle );
			FireSentryBulletsMTFL2( 4, enemy, 0.1, 0.1, 0 );
			break;
        	case SG_SFIRE_NEW:
			VectorCopy( self->s.v.angles, self->s.v.v_angle );
			FireSentryBulletsNEW( 4, enemy, 0.1, 0.1, 0 );
			break;
        	default:
        		G_Error("Unknown SG TYPE %d\n",tf_data.sg_sfire);
        
        }

        // Warn owner that it's low on ammo
	if ( !self->s.v.ammo_shells && g_random(  ) < 0.1 )
		G_sprint( self->real_owner, 2, "Sentry Gun is out of shells.\n" );
	else
	{
		if ( self->s.v.ammo_shells == 20 )
			G_sprint( self->real_owner, 2, "Sentry Gun is low on shells.\n" );
	}
	if ( !self->s.v.ammo_rockets && self->s.v.weapon == 3 && g_random(  ) < 0.1 )
		G_sprint( self->real_owner, 2, "Sentry Gun is out of rockets.\n" );
	return 1;
}

void Sentry_MuzzleFlash(  )
{
	self->s.v.effects = ( int ) self->s.v.effects | 8;
}

void Eng_StaticSG_Activate(  )
{
	gedict_t *ent;
	gedict_t *pl;
	float   failed;

	for ( ent = world; (ent = trap_find( ent, FOFS( s.v.classname ), "building_sentrygun" )); )
	{
		failed = 0;
		if ( ent->has_sentry && !ent->s.v.takedamage )
		{
			for ( pl = world; (pl = findradius( pl, ent->s.v.origin, 64 )); )
			{
				if ( streq( pl->s.v.classname, "player" ) )
				{
					G_bprint( 2, "Static sentry too close to player\n" );
					failed = 1;
				}
			}
			if ( trap_pointcontents( PASSVEC3(ent->s.v.origin) ) == -6 )
			{
				G_bprint( 2, "ERROR: Sentry gun felt off the level\n" );
				failed = 1;
				ent->real_owner->has_sentry = ent->real_owner->has_sentry - 1;
				dremove( ent->trigger_field );
				dremove( ent );
			}
			if ( !failed )
			{
				setmodel( ent, "progs/turrgun.mdl" );
				setmodel( ent->trigger_field, "progs/turrbase.mdl" );
				if ( ent->s.v.weapon == 1 )
					ent->s.v.think = ( func_t ) lvl1_sentry_stand;
				else
				{
					if ( ent->s.v.weapon == 2 )
						ent->s.v.think = ( func_t ) lvl2_sentry_stand;
					else
						ent->s.v.think = ( func_t ) lvl3_sentry_stand;
				}
				ent->s.v.nextthink = g_globalvars.time + 0.1;
				ent->s.v.movetype = MOVETYPE_STEP;
				ent->s.v.solid = SOLID_BBOX;
				ent->trigger_field->s.v.movetype = MOVETYPE_TOSS;
				ent->trigger_field->s.v.solid = SOLID_BBOX;
				ent->s.v.takedamage = 2;
				ent->s.v.health = ent->s.v.max_health;

				//ent->trigger_field->s.v.origin[2] += 1;
				//ent->s.v.origin[2] += 1;
				setorigin( ent->trigger_field, PASSVEC3(ent->trigger_field->s.v.origin));
				setorigin( ent, PASSVEC3(ent->s.v.origin));
				//SetVector( ent->trigger_field->s.v.velocity, 0, 0, -8 );

				G_sprint( ent->real_owner, 2, "Your Static Sentry Gun rebuilded.\n" );
			}
		}
	}
}
