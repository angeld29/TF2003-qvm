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

void Sentry_Rotate(  )
{
	float   ay;

//      gedict_t *gunhead;

	self->s.v.effects = ( int ) self->s.v.effects - ( ( int ) self->s.v.effects & 8 );
	CheckSentry( self );
	if ( tf_data.sentry_type == SENTRY_NEW || tf_data.sentry_type == SENTRY_MTFL_NEWFIND
	     || tf_data.sentry_type == SENTRY_OLD_NEWFIND )
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

#ifdef TG
	if ( tg_data.sg_allow_find == TG_SG_FIND_IGNORE_ALL )
		return 0;
#endif
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

#ifdef TG
	if ( tg_data.sg_allow_find == TG_SG_FIND_IGNORE_OFF )
		return 1;
#endif

	if ( teamplay )
	{
		if ( Target->team_no == self->team_no && self->team_no )
		{
#ifdef TG
			if ( tg_data.sg_allow_find == TG_SG_FIND_IGNORE_TEAM )
#endif
				return 0;
		}
		if ( Target->undercover_team == self->team_no && self->team_no )
		{
#ifdef TG
			if ( tg_data.sg_allow_find == TG_SG_FIND_IGNORE_TEAM )
#endif
				return 0;
		}
	}
	if ( Target == self->real_owner )
	{
#ifdef TG
		if ( tg_data.sg_allow_find == TG_SG_FIND_IGNORE_OWNER )
#endif
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
	for ( client = world; client = find( client, FOFS( s.v.classname ), "player" ); )
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
	SUB_AttackFinished( 1 );
}

void Sentry_Pain( struct gedict_s *attacker, float take )
{
	self->real_owner->StatusRefreshTime = g_globalvars.time + 0.2;
}

void Sentry_Explode(  )
{
	ThrowGib( "progs/tgib1.mdl", -70 );
	ThrowGib( "progs/tgib2.mdl", -70 );
	ThrowGib( "progs/tgib3.mdl", -70 );
	if ( self->real_owner->has_disconnected != 1 )
	{
		tf_data.deathmsg = 38;
		T_RadiusDamage( self, self->real_owner, 75 + self->s.v.ammo_rockets * 8, self );
	}
	if ( streq( self->s.v.classname, "building_sentrygun_base" ) )
	{
		if ( self->oldenemy )
			dremove( self->oldenemy );
	} else
		dremove( self->trigger_field );
	trap_WriteByte( 4, 23 );
	trap_WriteByte( 4, 3 );
	trap_WriteCoord( 4, self->s.v.origin[0] );
	trap_WriteCoord( 4, self->s.v.origin[1] );
	trap_WriteCoord( 4, self->s.v.origin[2] );
	trap_multicast( PASSVEC3( self->s.v.origin ), 1 );
	BecomeExplosion(  );
}

#ifndef TG
void Sentry_Die(  )
{
	G_sprint( self->real_owner, 2, "Your sentry gun was destroyed.\n" );
	self->real_owner->has_sentry = 0;
	self->s.v.think = ( func_t ) Sentry_Explode;
	self->s.v.nextthink = g_globalvars.time + 0.1;
}
#else
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
	if ( self->has_sentry )
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
		tf_data.deathmsg = 38;
		T_RadiusDamage( self, self->real_owner, 75 + self->s.v.ammo_rockets * 8, self );
		trap_WriteByte( 4, 23 );
		trap_WriteByte( 4, 3 );
		trap_WriteCoord( 4, self->s.v.origin[0] );
		trap_WriteCoord( 4, self->s.v.origin[1] );
		trap_WriteCoord( 4, self->s.v.origin[2] );
		trap_multicast( PASSVEC3( self->s.v.origin ), 1 );
	} else
	{
		self->real_owner->has_sentry = self->real_owner->has_sentry - 1;
		self->s.v.think = ( func_t ) Sentry_Explode;
		self->s.v.nextthink = g_globalvars.time + 0.1;
	}
}
#endif

void FireSentryBullets( int shotcount, gedict_t * targ, float spread_x, float spread_y, float spread_z )
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
#ifdef TG
void    FireSentryLighting( gedict_t * targ )  
{
	vec3_t  src;
	vec3_t  dst;
	vec3_t  dir, end,direction;
	gedict_t*trace_ent;
	float   rs;

	if ( tf_data.sentry_type == SENTRY_FIX || tf_data.sentry_type == SENTRY_NEW )
	{
		VectorCopy( self->s.v.angles, self->s.v.v_angle );
		makevectors( self->s.v.v_angle );
		VectorAdd( self->s.v.origin, self->s.v.view_ofs, src );
		VectorAdd( targ->s.v.origin, targ->s.v.view_ofs, dst );
		VectorSubtract( dst, src, dir );
		VectorNormalize( direction );
	} else
	{
		if ( tf_data.sentry_type == SENTRY_MTFL || tf_data.sentry_type == SENTRY_MTFL_NEWFIND )
		{
			VectorCopy( self->s.v.angles, self->s.v.v_angle );
		}
		makevectors( self->s.v.v_angle );
		VectorScale( g_globalvars.v_forward, 10, src );
		VectorAdd( self->s.v.origin, src, src );
		src[2] = self->s.v.absmin[2] + self->s.v.size[2] * 0.7;
		VectorSubtract( targ->s.v.origin, self->s.v.origin, dir );
	}
	g_globalvars.trace_ent = 0;

	VectorScale( dir, 2048, end );
	VectorAdd( end, src, end );
	traceline( PASSVEC3( src ), PASSVEC3( end ), 0, self );

	trap_WriteByte( 4, 23 );
	trap_WriteByte( 4, 5 );
	WriteEntity( 4, self );
	trap_WriteCoord( 4, src[0] );
	trap_WriteCoord( 4, src[1] );
	trap_WriteCoord( 4, src[2] );
	trap_WriteCoord( 4, g_globalvars.trace_endpos[0] );
	trap_WriteCoord( 4, g_globalvars.trace_endpos[1] );
	trap_WriteCoord( 4, g_globalvars.trace_endpos[2] );
	trap_multicast( PASSVEC3( src ), 1 );
	if ( g_globalvars.trace_ent )
	{
		trace_ent = PROG_TO_EDICT(g_globalvars.trace_ent);
		if ( streq( trace_ent->s.v.classname, "player" ) )
		{
			rs = (int)( g_random(  ) * 15) + 1 ;
			trace_ent->s.v.noise = "";
			if ( rs == 1 )
				trace_ent->s.v.noise = "player/pain1.wav";
			else
			{
				if ( rs == 2 )
					trace_ent->s.v.noise = "player/pain2.wav";
				else
				{
					if ( rs == 3 )
						trace_ent->s.v.noise = "player/pain3.wav";
					else
					{
						if ( rs == 4 )
							trace_ent->s.v.noise = "player/pain4.wav";
						else
						{
							if ( rs == 5 )
								trace_ent->s.v.noise = "player/pain5.wav";
							else
								trace_ent->s.v.noise = "player/pain6.wav";
						}
					}
				}
			}
			if ( rs <= 6 )
				sound( trace_ent, 2, trace_ent->s.v.noise, 1, 1 );
		}
	}
}
#endif
//////////////////////////////////////////////////////////////////////////////
int Sentry_Fire(  )
{
	vec3_t  dir, vtemp;

	gedict_t *enemy = PROG_TO_EDICT( self->s.v.enemy );

#ifdef TG
	if ( tg_data.sg_disable_fire || !CheckTarget(enemy) )
		return 0;
#endif
	self->s.v.effects = ( int ) self->s.v.effects - ( ( int ) self->s.v.effects & 8 );
	VectorSubtract( enemy->s.v.origin, self->s.v.origin, dir );
	if ( self->s.v.ideal_yaw - anglemod( self->s.v.angles[1] ) < -10
	     || self->s.v.ideal_yaw - anglemod( self->s.v.angles[1] ) > 10 )
		return 0;
	if ( enemy->is_feigning == 1 )
		return 0;
	if ( self->s.v.weapon == 3 && self->s.v.ammo_rockets > 0 && self->super_damage_finished < g_globalvars.time
#ifdef TG
	     && ( tg_data.sg_fire_type == TG_SG_FIRE_NORMAL )
#endif
	     )
	{
		Sentry_MuzzleFlash(  );
		sound( self, 1, "weapons/rocket1i.wav", 1, 1 );
		newmis = spawn(  );
		g_globalvars.newmis = EDICT_TO_PROG( newmis );
		newmis->s.v.owner = EDICT_TO_PROG( self );
		newmis->s.v.movetype = MOVETYPE_FLYMISSILE;
		newmis->s.v.solid = SOLID_BBOX;
		VectorScale( dir, 800, newmis->s.v.velocity );
		VectorNormalize( newmis->s.v.velocity );
		vectoangles( newmis->s.v.velocity, newmis->s.v.angles );
		newmis->s.v.weapon = 34;
		newmis->s.v.touch = ( func_t ) T_MissileTouch;
		newmis->s.v.nextthink = g_globalvars.time + 5;
		newmis->s.v.think = ( func_t ) SUB_Remove;
		setmodel( newmis, "progs/missile.mdl" );
		setsize( newmis, 0, 0, 0, 0, 0, 0 );
		VectorScale( g_globalvars.v_forward, 8, vtemp );
		VectorAdd( vtemp, self->s.v.origin, vtemp );
		vtemp[2] += 16;
		setorigin( newmis, PASSVEC3( vtemp ) );
		self->super_damage_finished = g_globalvars.time + 3;
		self->s.v.ammo_rockets = self->s.v.ammo_rockets - 1;
		if ( self->s.v.ammo_rockets == 10 )
			G_sprint( self->real_owner, 2, "Sentry Gun is low on rockets.\n" );
	}
#ifdef TG
	if ( tg_data.sg_fire_type != TG_SG_FIRE_LIGHTING )
	{
#endif
		self->s.v.ammo_shells -= 1;
		if ( self->s.v.ammo_shells < 0 )
		{
			self->s.v.ammo_shells = 0;
			return 0;
		}
		Sentry_MuzzleFlash(  );
		sound( self, 1, "weapons/sniper.wav", 1, 1 );
#ifdef TG
	}
#endif
	tf_data.deathmsg = 27;
#ifdef TG
	if ( tg_data.sg_fire_type == TG_SG_FIRE_LIGHTING )
	{
		FireSentryLighting( PROG_TO_EDICT(self->s.v.enemy) );
	} else
	{
#endif
		if ( tf_data.sentry_type == SENTRY_MTFL || tf_data.sentry_type == SENTRY_MTFL_NEWFIND )
		{		//mtfl sentry
			VectorCopy( self->s.v.angles, self->s.v.v_angle );
			FireBullets( 4, dir, 0.1, 0.1, 0 );
		} else
		{
			if ( tf_data.sentry_type == SENTRY_FIX || tf_data.sentry_type == SENTRY_NEW )
			{
				VectorCopy( self->s.v.angles, self->s.v.v_angle );
				FireSentryBullets( 4, enemy, 0.1, 0.1, 0 );
			} else
				FireBullets( 4, dir, 0.1, 0.1, 0 );
		}
#ifdef TG
	}
#endif
//////
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

#ifdef TG
void Eng_StaticSG_Activate(  )
{
	gedict_t *ent;
	gedict_t *pl;
	float   failed;

	for ( ent = world; ent = find( ent, FOFS( s.v.classname ), "building_sentrygun" ); )
	{
		failed = 0;
		if ( ent->has_sentry && !ent->s.v.takedamage )
		{
			for ( pl = world; pl = findradius( pl, ent->s.v.origin, 64 ); )
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

				ent->trigger_field->s.v.origin[2] += 8;
				setorigin( ent->trigger_field, PASSVEC3(ent->trigger_field->s.v.origin));
				SetVector( ent->trigger_field->s.v.velocity, 0, 0, -8 );

				G_sprint( ent->real_owner, 2, "Your Static Sentry Gun rebuilded.\n" );
			}
		}
	}
}
#endif
