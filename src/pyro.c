#include "g_local.h"

void    NapalmGrenadeTouch(  );
void    NapalmGrenadeLaunch( vec3_t org, gedict_t * shooter );
void    Napalm_touch(  );
int     RemoveFlameFromQueue( int id_flame );
int     num_world_flames = 0;

gedict_t *FlameSpawn( int type, gedict_t * p_owner )
{
	if ( tf_data.cb_prematch_time > g_globalvars.time )
		return world;
#ifndef LAN_SERVER
	if ( type != 1 )
		return world;
#endif
	num_world_flames = num_world_flames + 1;
	while ( num_world_flames > FLAME_MAXWORLDNUM )
	{
		if ( !RemoveFlameFromQueue( type ) )
			return world;
	}
	newmis = spawn(  );
	switch ( type )
	{
	case 1:
		newmis->s.v.movetype = MOVETYPE_FLYMISSILE;
		newmis->s.v.solid = SOLID_BBOX;
		newmis->s.v.effects = 8;
		newmis->flame_id = "1";
		setmodel( newmis, "progs/flame2.mdl" );
		setsize( newmis, 0, 0, 0, 0, 0, 0 );
		break;
	case 2:
		newmis->s.v.movetype = MOVETYPE_BOUNCE;
		newmis->s.v.solid = SOLID_BBOX;
		newmis->flame_id = "2";
		setmodel( newmis, "progs/flame2.mdl" );
		newmis->s.v.frame = 1;
		setsize( newmis, 0, 0, 0, 0, 0, 0 );
		break;
	case 3:
		newmis->s.v.movetype = MOVETYPE_FLYMISSILE;
		newmis->s.v.solid = SOLID_BBOX;
		newmis->flame_id = "3";
		setmodel( newmis, "progs/flame2.mdl" );
		setsize( newmis, 0, 0, 0, 0, 0, 0 );
		break;
	case 4:
		newmis->s.v.movetype = MOVETYPE_FLYMISSILE;
		newmis->flame_id = "4";
		newmis->s.v.frame = 1;
		newmis->s.v.solid = SOLID_BBOX;
		setmodel( newmis, "progs/flame2.mdl" );
		setsize( newmis, 0, 0, 0, 0, 0, 0 );
		break;
	}
	newmis->s.v.owner = EDICT_TO_PROG( p_owner );
	return newmis;
}

void FlameDestroy( gedict_t * this )
{
	num_world_flames = num_world_flames - 1;
	dremove( this );
}

int RemoveFlameFromQueue( int id_flame )
{
	gedict_t *tmp = NULL;

	if ( num_world_flames < FLAME_MAXWORLDNUM )
	{
		G_dprint( "ERROR in RemoveFlameFromQueue\n" );
		return 0;
	}


	switch ( id_flame )
	{
	case 1:
		tmp = find( world, FOFS( flame_id ), "1" );
		break;
	case 2:
		tmp = find( world, FOFS( flame_id ), "2" );
		break;
	case 3:
		tmp = find( world, FOFS( flame_id ), "3" );
		break;
	case 4:
		tmp = find( world, FOFS( flame_id ), "4" );
		break;
	}
	if ( !tmp )
	{
		G_dprint( "\n\nRemoveFlameFromQueue():BOOM!\n" );
		return 0;
	}
	num_world_flames = num_world_flames - 1;
	ent_remove( tmp );
	return 1;
}

void Remove(  )
{
	FlameDestroy( self );
}

void NapalmGrenadeFollow(  )
{
	traceline( PASSVEC3( self->s.v.origin ), PASSVEC3( self->s.v.origin ), 1, self );
	if ( g_globalvars.trace_inwater == 1 )
	{
		sound( self, 2, "misc/vapeur2.wav", 1, 1 );
		FlameDestroy( self );
	}
	if ( VectorCompareF( self->s.v.velocity, 0, 0, 0 ) )
		FlameDestroy( self );
	self->s.v.nextthink = g_globalvars.time + 0.1;
}

void NapalmGrenadeTouch(  )
{
	sound( self, 1, "weapons/bounce.wav", 1, 1 );
	if ( VectorCompareF( self->s.v.velocity, 0, 0, 0 ) )
		SetVector( self->s.v.avelocity, 0, 0, 0 );
}

void NapalmGrenadeNetThink(  )
{
	gedict_t *head;

	self->s.v.nextthink = g_globalvars.time + 1;
	for ( head = world; head = findradius( head, self->s.v.origin, 180 ); )
	{
		if ( head->s.v.takedamage )
		{
			tf_data.deathmsg = 15;
			TF_T_Damage( head, self, PROG_TO_EDICT( self->s.v.owner ), 20, 2, 16 );
			other = head;
			Napalm_touch(  );
			if ( streq( other->s.v.classname, "player" ) )
				stuffcmd( other, "bf\nbf\n" );
		}
	}
	trap_WriteByte( 4, 23 );
	trap_WriteByte( 4, 3 );
	trap_WriteCoord( 4, self->s.v.origin[0] );
	trap_WriteCoord( 4, self->s.v.origin[1] );
	trap_WriteCoord( 4, self->s.v.origin[2] );
	trap_multicast( PASSVEC3( self->s.v.origin ), 1 );
	self->heat = self->heat + 1;
	if ( self->heat > 7 )
	{
		dremove( PROG_TO_EDICT( self->s.v.enemy ) );
		dremove( self );
	}
}

#ifndef LAN_SERVER
void NapalmGrenadeExplode(  )
{
//      float   i;
	gedict_t *head;

	sound( self, 0, "weapons/flmgrexp.wav", 1, 1 );
	traceline( PASSVEC3( self->s.v.origin ), PASSVEC3( self->s.v.origin ), 1, self );
	if ( g_globalvars.trace_inwater == 1 )
	{
		dremove( self );
		return;
	}
	self->s.v.effects = ( int ) self->s.v.effects | 8;
	for ( head = world; head = findradius( head, self->s.v.origin, 180 ); )
	{
		if ( head->s.v.takedamage )
		{
			tf_data.deathmsg = 15;
			TF_T_Damage( head, self, PROG_TO_EDICT( self->s.v.owner ), 40, 2, 16 );
			other = head;
			Napalm_touch(  );
			if ( streq( other->s.v.classname, "player" ) )
				stuffcmd( other, "bf\nbf\n" );
		}
	}
	trap_WriteByte( 4, 23 );
	trap_WriteByte( 4, 3 );
	trap_WriteCoord( 4, self->s.v.origin[0] );
	trap_WriteCoord( 4, self->s.v.origin[1] );
	trap_WriteCoord( 4, self->s.v.origin[2] );
	trap_multicast( PASSVEC3( self->s.v.origin ), 1 );
	head = spawn(  );
	head->s.v.think = ( func_t ) NapalmGrenadeNetThink;
	head->s.v.nextthink = g_globalvars.time + 1;
	head->heat = 0;
	VectorCopy( self->s.v.origin, head->s.v.origin );
	head->s.v.owner = self->s.v.owner;
	head->team_no = PROG_TO_EDICT( self->s.v.owner )->team_no;
	head->s.v.enemy = EDICT_TO_PROG( self );
///Napalm fix
	self->s.v.movetype = MOVETYPE_NONE;
	SetVector( self->s.v.velocity, 0, 0, 0 );

}
#else
void NapalmGrenadeExplode(  )
{
	float   i;
	vec3_t  tmp;

	sound( self, 0, "weapons/flmgrexp.wav", 1, 1 );
	traceline( PASSVEC3( self->s.v.origin ), PASSVEC3( self->s.v.origin ), 1, self );
	if ( g_globalvars.trace_inwater == 1 )
	{
		dremove( self );
		return;
	}
	deathmsg = 15;
	T_RadiusDamage( self, PROG_TO_EDICT( self->s.v.owner ), 20, world );
	i = 0;
	VectorCopy( self->s.v.origin, tmp );
	tmp[2] += 5;
	while ( i < 15 )
	{
		NapalmGrenadeLaunch( tmp, PROG_TO_EDICT( self->s.v.owner ) );
		i = i + 1;
	}
	self->s.v.solid = SOLID_NOT;
	BecomeExplosion(  );
}
#endif
void NapalmGrenadeLaunch( vec3_t org, gedict_t * shooter )
{
	float   xdir;
	float   ydir;
	float   zdir;
	float   spin;

	xdir = 150 * g_random(  ) - 75;
	ydir = 150 * g_random(  ) - 75;
	zdir = 40 * g_random(  );
	newmis = FlameSpawn( 2, shooter );
	g_globalvars.newmis = EDICT_TO_PROG( newmis );

	if ( newmis == world )
		return;
	self->s.v.touch = ( func_t ) SUB_Null;
	newmis->s.v.classname = "fire";
	newmis->s.v.touch = ( func_t ) Napalm_touch;
	newmis->s.v.think = ( func_t ) NapalmGrenadeFollow;
	newmis->s.v.nextthink = g_globalvars.time + 0.1;
	newmis->s.v.enemy = shooter->s.v.owner;
	newmis->s.v.velocity[0] = xdir * 2;
	newmis->s.v.velocity[1] = ydir * 2;
	newmis->s.v.velocity[2] = zdir * 15;
	spin = ( int ) ( g_random(  ) * 10 / 2 );
	if ( spin <= 0 )
		SetVector( newmis->s.v.avelocity, 250, 300, 400 );
	if ( spin == 1 )
		SetVector( newmis->s.v.avelocity, 400, 250, 300 );
	if ( spin == 2 )
		SetVector( newmis->s.v.avelocity, 300, 400, 250 );
	if ( spin == 3 )
		SetVector( newmis->s.v.avelocity, 300, 300, 300 );
	if ( spin >= 4 )
		SetVector( newmis->s.v.avelocity, 400, 250, 400 );
	setorigin( newmis, PASSVEC3( org ) );
	setsize( newmis, 0, 0, 0, 0, 0, 0 );
}


void    OnPlayerFlame_touch(  );

void FlameFollow(  )
{
	vec3_t  dir;
	vec3_t  vtemp;
	vec3_t  boundsize;
	float   damage;
	gedict_t *enemy = PROG_TO_EDICT( self->s.v.enemy );

	VectorCopy( enemy->s.v.absmin, vtemp );
	VectorCopy( enemy->s.v.size, boundsize );
	self->s.v.solid = SOLID_NOT;
	self->s.v.movetype = MOVETYPE_NONE;
	if ( !enemy->numflames )
	{
		FlameDestroy( self );
		return;
	}
	if ( enemy->s.v.health < 1 )
	{
		tf_data.deathmsg = 15;
		T_RadiusDamage( self, self, 10, self );
		enemy->numflames = 0;
		FlameDestroy( self );
		return;
	}
	if ( ( enemy->armorclass & 16 ) && enemy->s.v.armorvalue > 0 )
		self->s.v.health = 0;
	if ( enemy->tfstate & 131072 )
	{
		self->s.v.health = 45;
		enemy->tfstate = enemy->tfstate - ( enemy->tfstate & 131072 );
	}
	if ( self->s.v.health < 1 )
	{
		if ( self->s.v.effects != 8 || enemy->numflames <= 1 )
		{
			enemy->numflames = enemy->numflames - 1;
			enemy->numflames = 0;
			FlameDestroy( self );
			return;
		}
	}
	self->s.v.health = self->s.v.health - 1;
	if ( vlen( enemy->s.v.velocity ) < 50 )
	{
		dir[0] = g_random(  ) * boundsize[0] / 2 + boundsize[0] / 4;
		dir[1] = g_random(  ) * boundsize[1] / 2 + boundsize[1] / 4;
		dir[2] = g_random(  ) * boundsize[2] / 3 + boundsize[2] / 2;
		VectorAdd( vtemp, dir, vtemp );
		setorigin( self, PASSVEC3( vtemp ) );
		if ( strneq( self->s.v.model, "progs/flame2.mdl" ) )
		{
			self->s.v.model = "progs/flame2.mdl";
			setmodel( self, self->s.v.model );
		}
	} else
	{
		if ( streq( self->s.v.model, "progs/flame2.mdl" ) )
		{
			self->s.v.model = "";
			setmodel( self, self->s.v.model );
		}
	}
	if ( enemy->s.v.waterlevel > 1 )
	{
		sound( self, 2, "misc/vapeur2.wav", 1, 1 );
		enemy->numflames = enemy->numflames - 1;
		FlameDestroy( self );
		return;
	}
	self->s.v.nextthink = g_globalvars.time + 0.1;
	if ( self->s.v.effects == 8 && self->heat >= 3 )
	{
		damage = enemy->numflames * 0.3 * 3;
		if ( damage < 1 )
			damage = 1;
		self->heat = 1;
		tf_data.deathmsg = 15;
		TF_T_Damage( enemy, self, PROG_TO_EDICT( self->s.v.owner ), damage, 2, 16 );
	} else
	{
		if ( self->s.v.effects == 8 )
			self->heat = self->heat + 1;
	}
}

void OnPlayerFlame_touch(  )
{
	gedict_t *flame;

//      vec3_t  vtemp;

	if ( other != world && other->s.v.health > 0 && other != PROG_TO_EDICT( self->s.v.enemy ) )
	{
		if ( tf_data.cb_prematch_time > g_globalvars.time )
			return;
		if ( other->numflames >= 4 )
		{
			other->tfstate = other->tfstate | 131072;
			return;
		}
		if ( streq( other->s.v.classname, "player" ) )
		{
			if ( ( teamplay & 16 ) && other->team_no > 0
			     && other->team_no == PROG_TO_EDICT( self->s.v.owner )->team_no )
				return;
			CenterPrint( other, "You are on fire!\n" );
			stuffcmd( other, "bf\n" );
		}
		if ( other->numflames < 1 )
		{
			flame = FlameSpawn( 1, other );
			sound( flame, 2, "ambience/fire1.wav", 1, 1 );
		} else
		{
			flame = FlameSpawn( 3, other );
			if ( flame == world )
				return;
		}
		flame->s.v.classname = "fire";
		flame->s.v.health = 45;
		other->numflames = other->numflames + 1;
		VectorCopy( other->s.v.velocity, flame->s.v.velocity );
		flame->s.v.enemy = EDICT_TO_PROG( other );
		flame->s.v.touch = ( func_t ) OnPlayerFlame_touch;
		flame->s.v.owner = self->s.v.owner;
		setorigin( flame, PASSVEC3( self->s.v.origin ) );
		flame->s.v.nextthink = g_globalvars.time + 0.1;
		flame->s.v.think = ( func_t ) FlameFollow;
	}
}


void    WorldFlame_touch(  );

void QW_Flame_ResetTouch(  )
{
	self->s.v.touch = ( func_t ) WorldFlame_touch;
	self->s.v.think = ( func_t ) Remove;
	self->s.v.nextthink = self->heat;
}

void WorldFlame_touch(  )
{
	gedict_t *flame;

//      vec3_t  dir;
	vec3_t  vtemp;

	tf_data.deathmsg = 15;
	TF_T_Damage( other, self, PROG_TO_EDICT( self->s.v.enemy ), 10, 2, 16 );
	self->s.v.touch = ( func_t ) SUB_Null;
	if ( self->heat > g_globalvars.time + 1 )
	{
		self->s.v.think = ( func_t ) QW_Flame_ResetTouch;
		self->s.v.nextthink = g_globalvars.time + 1;
	}
	if ( other != world && other->s.v.solid != 1 && other->s.v.health > 0 )
	{
		if ( tf_data.cb_prematch_time > g_globalvars.time )
			return;
		if ( other->numflames >= 4 )
		{
			other->tfstate = other->tfstate | 131072;
			return;
		}
		if ( streq( other->s.v.classname, "player" ) )
		{
			if ( ( teamplay & 16 ) && other->team_no > 0
			     && other->team_no == PROG_TO_EDICT( self->s.v.owner )->team_no )
				return;
			CenterPrint( other, "You are on fire!\n" );
			stuffcmd( other, "bf\n" );
		}
		if ( other->numflames < 1 )
		{
			flame = FlameSpawn( 1, other );
			sound( flame, 2, "ambience/fire1.wav", 1, 1 );
		} else
		{
			flame = FlameSpawn( 3, other );
			if ( flame == world )
				return;
		}
		flame->s.v.classname = "fire";
		flame->s.v.health = 0;
		other->numflames = other->numflames + 1;
		VectorCopy( other->s.v.velocity, flame->s.v.velocity );
		flame->s.v.enemy = EDICT_TO_PROG( other );
		flame->s.v.touch = ( func_t ) OnPlayerFlame_touch;
		flame->s.v.owner = self->s.v.owner;
		VectorCopy( self->s.v.origin, vtemp );
		vtemp[2] += 10;
		setorigin( flame, PASSVEC3( vtemp ) );
		flame->s.v.nextthink = g_globalvars.time + 0.15;
		flame->s.v.think = ( func_t ) FlameFollow;
	}
}

void Flamer_stream_touch(  )
{
	gedict_t *flame;

//      vec3_t  dir;
	vec3_t  vtemp;
	int pos;
    
	VectorCopy( self->s.v.origin , vtemp);
	vtemp[2]++;
	pos = trap_pointcontents( PASSVEC3( vtemp));

	if( pos == CONTENT_WATER || pos == CONTENT_SLIME )
	{
		dremove(self);
		return;
	}

	if ( streq( other->s.v.classname, "fire" ) )
		return;
	if ( other != world )
	{
		if ( other->s.v.takedamage == 2 && other->s.v.health > 0 )
		{
			tf_data.deathmsg = 15;
			TF_T_Damage( other, self, PROG_TO_EDICT( self->s.v.owner ), 10, 2, 16 );
			if ( tf_data.cb_prematch_time > g_globalvars.time )
				return;
			if ( other->numflames >= 4 )
			{
				other->tfstate = other->tfstate | 131072;
				return;
			}
			if ( ( other->armorclass & 16 ) && other->s.v.armorvalue > 0 )
				return;
			if ( streq( other->s.v.classname, "player" ) )
			{
				if ( ( teamplay & 16 ) && other->team_no > 0
				     && other->team_no == PROG_TO_EDICT( self->s.v.owner )->team_no )
					return;
				CenterPrint( other, "You are on fire!\n" );
				stuffcmd( other, "bf\n" );
			}
			if ( other->numflames < 1 )
			{
				flame = FlameSpawn( 1, other );
				sound( flame, 2, "ambience/fire1.wav", 1, 1 );
			} else
			{
				flame = FlameSpawn( 3, other );
				if ( flame == world )
					return;
			}
			flame->s.v.classname = "fire";
			flame->s.v.health = 45;
			other->numflames = other->numflames + 1;
			VectorCopy( other->s.v.velocity, flame->s.v.velocity );
			flame->s.v.enemy = EDICT_TO_PROG( other );
			flame->s.v.touch = ( func_t ) OnPlayerFlame_touch;
			flame->s.v.owner = self->s.v.owner;
			VectorCopy( self->s.v.origin, vtemp );
			setorigin( flame, PASSVEC3( vtemp ) );
			flame->s.v.nextthink = g_globalvars.time + 0.1;
			flame->s.v.think = ( func_t ) FlameFollow;
		}
	} else
	{
		if ( g_random(  ) < 0.3 || trap_pointcontents( PASSVEC3( self->s.v.origin ) + 1 ) != -1 )
		{
			ent_remove( self );
			return;
		}
		flame = FlameSpawn( 4, other );
		if ( flame != world )
		{
			flame->s.v.touch = ( func_t ) WorldFlame_touch;
			flame->s.v.classname = "fire";
			VectorCopy( self->s.v.origin, vtemp );
			vtemp[2] += 10;
			setorigin( flame, PASSVEC3( vtemp ) );
			flame->s.v.nextthink = g_globalvars.time + 8;
			flame->heat = flame->s.v.nextthink;
			flame->s.v.think = ( func_t ) Remove;
			flame->s.v.enemy = self->s.v.owner;
		}
		ent_remove( self );
	}
}

void Napalm_touch(  )
{
	gedict_t *flame;

//      vec3_t  dir;
	vec3_t  vtemp;

	if ( streq( other->s.v.classname, "fire" ) )
		return;
	if ( other != world )
	{
		if ( other->s.v.takedamage == 2 && other->s.v.health > 0 )
		{
			tf_data.deathmsg = 15;
			TF_T_Damage( other, self, PROG_TO_EDICT( self->s.v.owner ), 6, 2, 16 );
			if ( tf_data.cb_prematch_time > g_globalvars.time )
				return;
			if ( other->numflames >= 4 )
			{
				other->tfstate = other->tfstate | 131072;
				return;
			}
			if ( ( other->armorclass & 16 ) && other->s.v.armorvalue > 0 )
				return;
			if ( streq( other->s.v.classname, "player" ) )
			{
				if ( ( teamplay & 16 ) && other->team_no > 0
				     && other->team_no == PROG_TO_EDICT( self->s.v.owner )->team_no )
					return;
				CenterPrint( other, "You are on fire!\n" );
				stuffcmd( other, "bf\n" );
			}
			if ( other->numflames < 1 )
			{
				flame = FlameSpawn( 1, other );
				sound( flame, 2, "ambience/fire1.wav", 1, 1 );
			} else
			{
				flame = FlameSpawn( 3, other );
				if ( flame == world )
					return;
			}
			flame->s.v.classname = "fire";
			flame->s.v.health = 45;
			other->numflames = other->numflames + 1;
			VectorCopy( other->s.v.velocity, flame->s.v.velocity );
			flame->s.v.enemy = EDICT_TO_PROG( other );
			flame->s.v.touch = ( func_t ) OnPlayerFlame_touch;
			flame->s.v.owner = self->s.v.owner;
			VectorCopy( self->s.v.origin, vtemp );
			setorigin( flame, PASSVEC3( vtemp ) );
			flame->s.v.nextthink = g_globalvars.time + 0.1;
			flame->s.v.think = ( func_t ) FlameFollow;
		}
	} else
	{
		if ( trap_pointcontents( PASSVEC3( self->s.v.origin ) + 1 ) != -1 )
		{
			FlameDestroy( self );
			return;
		}
		flame = FlameSpawn( 4, other );
		if ( flame != world )
		{
			flame->s.v.touch = ( func_t ) WorldFlame_touch;
			flame->s.v.classname = "fire";
			VectorCopy( self->s.v.origin, vtemp );
			vtemp[2] += 10;
			setorigin( flame, PASSVEC3( vtemp ) );
			flame->s.v.nextthink = g_globalvars.time + 20;
			flame->heat = flame->s.v.nextthink;
			flame->s.v.think = ( func_t ) Remove;
			flame->s.v.enemy = self->s.v.owner;
		}
		FlameDestroy( self );
	}
}

void NewBubbles( float num_bubbles, vec3_t bub_origin )
{
	gedict_t *bubble_spawner;

	bubble_spawner = spawn(  );
	setorigin( bubble_spawner, PASSVEC3( bub_origin ) );
	bubble_spawner->s.v.movetype = MOVETYPE_NONE;
	bubble_spawner->s.v.solid = SOLID_NOT;
	bubble_spawner->s.v.nextthink = g_globalvars.time + 0.1;
	if ( streq( self->s.v.classname, "player" ) )
		bubble_spawner->s.v.owner = EDICT_TO_PROG( self );
	else
		bubble_spawner->s.v.owner = self->s.v.enemy;
	bubble_spawner->s.v.think = ( func_t ) DeathBubblesSpawn;
	bubble_spawner->bubble_count = num_bubbles;
	return;
}

void    s_explode1(  );
void    s_explode2(  );
void    s_explode3(  );
void    s_explode4(  );
void    s_explode5(  );
void    s_explode6(  );

void W_FireFlame(  )
{
	gedict_t *flame;
	float   rn;
	vec3_t  vtemp;

	if ( self->s.v.waterlevel > 2 )
	{
		makevectors( self->s.v.v_angle );
		VectorScale( g_globalvars.v_forward, 64, vtemp );
		VectorAdd( self->s.v.origin, vtemp, vtemp );
		NewBubbles( 2, vtemp );
		rn = g_random(  );
		if ( rn < 0.5 )
			sound( self, 1, "misc/water1.wav", 1, 1 );
		else
			sound( self, 1, "misc/water2.wav", 1, 1 );
		return;
	}
#ifdef TG
	if ( !tg_data.unlimit_ammo )
#endif
		self->s.v.currentammo = --( self->s.v.ammo_cells );
	sound( self, 0, "weapons/flmfire2.wav", 1, 1 );
	flame = spawn(  );
	flame->s.v.owner = EDICT_TO_PROG( self );
	flame->s.v.movetype = MOVETYPE_FLYMISSILE;
	flame->s.v.solid = SOLID_BBOX;
	flame->s.v.classname = "flamerflame";
	makevectors( self->s.v.v_angle );
	aim( flame->s.v.velocity );
	VectorScale( flame->s.v.velocity, 600, flame->s.v.velocity );
	flame->s.v.touch = ( func_t ) Flamer_stream_touch;
	flame->s.v.think = ( func_t ) s_explode1;
	flame->s.v.effects = 8;
	flame->s.v.nextthink = g_globalvars.time + 0.15;
	setmodel( flame, "progs/s_explod.spr" );
	setsize( flame, 0, 0, 0, 0, 0, 0 );
	VectorScale( g_globalvars.v_forward, 16, vtemp );
	VectorAdd( self->s.v.origin, vtemp, vtemp );
	vtemp[2] += 16;
	setorigin( flame, PASSVEC3( vtemp ) );
}

void T_IncendiaryTouch(  )
{
	float   damg;

//      float   points;
	gedict_t *head, *owner;
	vec3_t  vtemp;

	owner = PROG_TO_EDICT( self->s.v.owner );
	if ( other == owner )
		return;

	if ( trap_pointcontents( PASSVEC3( self->s.v.origin ) ) == -6 )
	{
		ent_remove( self );
		return;
	}
	self->s.v.effects = ( int ) self->s.v.effects | 4;
	damg = 30 + g_random(  ) * 20;
	if ( other->s.v.health )
	{
		tf_data.deathmsg = 15;
		TF_T_Damage( other, self, owner, damg, 2, 16 );
	}
	for ( head = world; head = findradius( head, self->s.v.origin, 180 ); )
	{
		if ( head->s.v.takedamage )
		{
			traceline( PASSVEC3( self->s.v.origin ), PASSVEC3( head->s.v.origin ), 1, self );
			VectorSubtract( self->s.v.origin, head->s.v.origin, vtemp );
			if ( g_globalvars.trace_fraction == 1
			     || ( g_globalvars.trace_fraction != 1 && vlen( vtemp ) <= 64 ) )
			{
				tf_data.deathmsg = 15;
				TF_T_Damage( head, self, owner, 10, 2, 16 );
				other = head;
				if( !g_globalvars.trace_inwater)
					Napalm_touch(  );

				if ( streq( other->s.v.classname, "player" ) )
					stuffcmd( other, "bf\nbf\n" );
			}
		}
	}
	normalize( self->s.v.velocity, vtemp );
	VectorScale( vtemp, 8, vtemp );
	VectorSubtract( self->s.v.origin, vtemp, self->s.v.origin );
	trap_WriteByte( 4, 23 );
	trap_WriteByte( 4, 3 );
	trap_WriteCoord( 4, self->s.v.origin[0] );
	trap_WriteCoord( 4, self->s.v.origin[1] );
	trap_WriteCoord( 4, self->s.v.origin[2] );
	trap_multicast( PASSVEC3( self->s.v.origin ), 1 );
	dremove( self );
}

void W_FireIncendiaryCannon(  )
{
	vec3_t  vtemp;

	if ( self->s.v.ammo_rockets < 3 )
		return;
#ifdef TG
	if ( !tg_data.unlimit_ammo )
#endif
		self->s.v.currentammo = self->s.v.ammo_rockets = self->s.v.ammo_rockets - 3;

	sound( self, 1, "weapons/sgun1.wav", 1, 1 );
	KickPlayer( -3, self );
	newmis = spawn(  );
	g_globalvars.newmis = EDICT_TO_PROG( newmis );
	newmis->s.v.owner = EDICT_TO_PROG( self );
	newmis->s.v.movetype = MOVETYPE_FLYMISSILE;
	newmis->s.v.solid = SOLID_BBOX;
	makevectors( self->s.v.v_angle );
	aim( newmis->s.v.velocity );
	VectorScale( newmis->s.v.velocity, 600, newmis->s.v.velocity );
	vectoangles( newmis->s.v.velocity, newmis->s.v.angles );
	newmis->s.v.touch = ( func_t ) T_IncendiaryTouch;
	newmis->s.v.nextthink = g_globalvars.time + 5;
	newmis->s.v.think = ( func_t ) SUB_Remove;
	newmis->s.v.weapon = 33;
	setmodel( newmis, "progs/missile.mdl" );
	setsize( newmis, 0, 0, 0, 0, 0, 0 );
	VectorScale( g_globalvars.v_forward, 8, vtemp );
	VectorAdd( vtemp, self->s.v.origin, vtemp );
	vtemp[2] += 16;
	setorigin( newmis, PASSVEC3( vtemp ) );
}

void TeamFortress_IncendiaryCannon(  )
{
	if ( !( self->weapons_carried & WEAP_INCENDIARY ) )
		return;
	if ( self->s.v.ammo_rockets < 3 )
	{
		G_sprint( self, 2, "not enough ammo.\n" );
		return;
	}
	self->current_weapon = WEAP_INCENDIARY;
	W_SetCurrentAmmo(  );
}

void TeamFortress_FlameThrower(  )
{
	if ( !( self->weapons_carried & WEAP_FLAMETHROWER ) )
		return;
	if ( self->s.v.ammo_cells < 1 )
	{
		G_sprint( self, 2, "not enough ammo.\n" );
		return;
	}
	self->current_weapon = WEAP_FLAMETHROWER;
	W_SetCurrentAmmo(  );
}
