#include "g_local.h"

void    NormalGrenadeTouch(  );
void    NormalGrenadeExplode(  );
void    MirvGrenadeTouch(  );
void    MirvGrenadeExplode(  );
void    MirvGrenadeLaunch( vec3_t org, gedict_t * shooter );
void    TeamFortress_DetpackSet(  );
void    TeamFortress_DetpackExplode(  );
void    TeamFortress_DetpackTouch(  );
void    TeamFortress_DetpackDisarm(  );
void    TeamFortress_DetpackCountDown(  );
int     num_team_pipebombs[5];

void TeamFortress_DetonatePipebombs(  )
{
	gedict_t *e;

	for ( e = world; e = find( e, FOFS( s.v.classname ), "pipebomb" ); )
	{
		if ( e->s.v.owner == EDICT_TO_PROG( self ) )
			e->s.v.nextthink = g_globalvars.time;
	}
}

void PipebombTouch(  )
{
	sound( self, 1, "weapons/bounce.wav", 1, 1 );
	if ( VectorCompareF( self->s.v.velocity, 0, 0, 0 ) )
		SetVector( self->s.v.avelocity, 0, 0, 0 );
}

void MirvGrenadeTouch(  )
{
	sound( self, 1, "weapons/bounce.wav", 1, 1 );
	if ( VectorCompareF( self->s.v.velocity, 0, 0, 0 ) )
		SetVector( self->s.v.avelocity, 0, 0, 0 );
}

void MirvGrenadeExplode(  )
{
	float   i;

	tf_data.deathmsg = 10;
	T_RadiusDamage( self, PROG_TO_EDICT( self->s.v.owner ), 100, world );
	trap_WriteByte( 4, 23 );
	trap_WriteByte( 4, 3 );
	trap_WriteCoord( 4, self->s.v.origin[0] );
	trap_WriteCoord( 4, self->s.v.origin[1] );
	trap_WriteCoord( 4, self->s.v.origin[2] );
	trap_multicast( PASSVEC3( self->s.v.origin ), 1 );
	self->s.v.solid = SOLID_NOT;
	i = 0;
	while ( i < GR_TYPE_MIRV_NO )
	{
		MirvGrenadeLaunch( self->s.v.origin, PROG_TO_EDICT( self->s.v.owner ) );
		i = i + 1;
	}
	BecomeExplosion(  );
}

void MirvGrenadeLaunch( vec3_t org, gedict_t * shooter )
{
	float   xdir;
	float   ydir;
	float   zdir;

	xdir = 150 * g_random(  ) - 75;
	ydir = 150 * g_random(  ) - 75;
	zdir = 40 * g_random(  );
	newmis = spawn(  );
	g_globalvars.newmis = EDICT_TO_PROG( newmis );
	newmis->s.v.owner = EDICT_TO_PROG( shooter );
	newmis->s.v.movetype = MOVETYPE_BOUNCE;
	newmis->s.v.solid = SOLID_BBOX;
	newmis->s.v.classname = "grenade";
	newmis->s.v.weapon = 10;
#ifndef LAN_SERVER
	newmis->s.v.touch = ( func_t ) NormalGrenadeTouch;
	newmis->s.v.think = ( func_t ) NormalGrenadeExplode;
#else
	newmis->s.v.touch = ( func_t ) GrenadeTouch;
	newmis->s.v.think = ( func_t ) GrenadeExplode;
#endif
	newmis->s.v.nextthink = g_globalvars.time + 2 + g_random(  );
	newmis->s.v.velocity[0] = xdir * 2;
	newmis->s.v.velocity[1] = ydir * 2;
	newmis->s.v.velocity[2] = zdir * 15 + 1;
	SetVector( newmis->s.v.avelocity, 250, 300, 400 );
	if ( tf_data.birthday == 1 && g_random(  ) < 0.3 )
		setmodel( newmis, "progs/grenade3.mdl" );
	else
		setmodel( newmis, "progs/grenade2.mdl" );

	setsize( newmis, 0, 0, 0, 0, 0, 0 );
	setorigin( newmis, PASSVEC3( org ) );
}

#ifdef TG

void Detpack_SetClip(  )
{
	gedict_t *te;
//	gedict_t *pl;

	for ( te = world; te = find( te, FOFS( s.v.classname ), "detpack" ); )
	{
	        switch(tg_data.detpack_clip)
	        {
	        	case TG_DETPACK_CLIP_ALL:
	        		te->s.v.solid = SOLID_NOT;
	        		break;
	        	case TG_DETPACK_SOLID_ALL:
				te->s.v.solid = SOLID_BBOX;
				te->s.v.owner = EDICT_TO_PROG( world );
	        		break;

	        	case TG_DETPACK_CLIP_OWNER:
	        	default:	
				te->s.v.solid = SOLID_BBOX;
				te->s.v.owner = EDICT_TO_PROG( te->real_owner );
				break;
	        }
	}
}
#endif
void TeamFortress_SetDetpack( float timer )
{
	gedict_t *te;
	gedict_t *at_spot;

	self->s.v.impulse = 0;
	self->last_impulse = 0;
#ifndef TG
	if ( !( self->weapons_carried & WEAP_DETPACK ) )
		return;
	if ( (tf_data.disable_grens & DG_TYPE_DETPACK) )
		return;
	if ( self->ammo_detpack <= 0 )
		return;
#else
	if ( !( self->weapons_carried & WEAP_DETPACK ) && !tg_data.tg_enabled )
		return;
	if ( (tf_data.disable_grens & DG_TYPE_DETPACK) && !tg_data.tg_enabled )
		return;
	if ( self->ammo_detpack <= 0 && !tg_data.tg_enabled )
		return;
#endif
	for ( at_spot = world; at_spot = findradius( at_spot, self->s.v.origin, 65 ); )
	{
		if ( streq( at_spot->s.v.classname, "player" ) && self != at_spot )
		{
			G_sprint( self, 2, "You can't set a detpack on someone!\n" );
			return;
		}
		if ( streq( at_spot->mdl, "progs/turrbase.mdl" ) || streq( at_spot->mdl, "progs/turrgun.mdl" ) )
		{
			G_sprint( self, 2, "You can't set a detpack on a sentry gun!\n" );
			return;
		}
		if ( streq( at_spot->mdl, "progs/disp.mdl" ) )
		{
			G_sprint( self, 2, "You can't set a detpack on a dispenser!\n" );
			return;
		}
		if ( tf_data.birthday == 1 && streq( at_spot->mdl, "progs/detpack2.mdl" ) )
		{
			G_sprint( self, 2, "Detpacks can't be stacked!\n" );
			return;
		}
		if ( streq( at_spot->s.v.classname, "detpack" ) )
		{
			G_sprint( self, 2, "Detpacks can't be stacked!\n" );
			return;
		}
	}
	if ( tf_data.detpack_block )
	{
		for ( at_spot = world; at_spot = findradius( at_spot, self->s.v.origin, 100 ); )
		{
			if ( tf_data.birthday == 1 && streq( at_spot->mdl, "progs/detpack2.mdl" ) )
			{
				G_sprint( self, 2, "Detpacks can't be stacked!\n" );
				return;
			}
			if ( streq( at_spot->s.v.classname, "detpack" ) )
			{
				G_sprint( self, 2, "Detpacks can't be stacked!\n" );
				return;
			}
		}
	}
	if ( !( ( int ) self->s.v.flags & FL_ONGROUND ) )
	{
		G_sprint( self, 2, "You can't set detpacks in the air!\n" );
		return;
	}
#ifdef TG
        if( !tg_data.tg_enabled )
#endif
	for ( te = world; te = find( te, FOFS( s.v.classname ), "detpack" ); )
	{
		if ( te->s.v.owner == EDICT_TO_PROG( self ) )
		{
			G_sprint( self, 2, "You can only have 1 detpack active at a time.\n" );
			return;
		}
	}

	if ( timer < 5 )
	{
		G_sprint( self, 2, "You can't set detpacks for less that 5 seconds.\n" );
		return;
	}
	self->is_detpacking = 1;
	self->ammo_detpack = self->ammo_detpack - 1;
	self->tfstate = self->tfstate | TFSTATE_CANT_MOVE;
	self->s.v.weapon = self->current_weapon;
	self->current_weapon = 0;
	self->s.v.weaponmodel = "";
	self->s.v.weaponframe = 0;
	TeamFortress_SetSpeed( self );
	self->pausetime = g_globalvars.time + WEAP_DETPACK_SETTIME;
	G_sprint( self, 2, "Setting detpack for %.0f seconds...\n", timer );
	newmis = spawn(  );
	g_globalvars.newmis = EDICT_TO_PROG( newmis );
	newmis->s.v.owner = EDICT_TO_PROG( self );
#ifdef TG
	newmis->real_owner = self;
//	VectorCopy( self->s.v.origin, newmis->s.v.origin );
#endif
	newmis->s.v.classname = "timer";
	newmis->s.v.netname = "detpack_timer";
	newmis->s.v.nextthink = g_globalvars.time + WEAP_DETPACK_SETTIME;
	newmis->s.v.think = ( func_t ) TeamFortress_DetpackSet;
	newmis->s.v.health = timer;
}

void TeamFortress_DetpackStop(  )
{
	gedict_t *detpack_timer;

	for ( detpack_timer = world; detpack_timer = find( detpack_timer, FOFS( s.v.netname ), "detpack_timer" ); )
	{
		if ( detpack_timer->s.v.owner = EDICT_TO_PROG( self ) )
			break;
	}
	if ( !detpack_timer )
		return;
	G_sprint( self, 2, "Detpack retrieved.\n" );
	self->ammo_detpack = self->ammo_detpack + 1;
	dremove( detpack_timer );
	self->tfstate = self->tfstate - ( self->tfstate & TFSTATE_CANT_MOVE );
	self->is_detpacking = 0;
	self->current_weapon = self->s.v.weapon;
	W_SetCurrentAmmo(  );
	TeamFortress_SetSpeed( self );
	self->pausetime = g_globalvars.time;
}

void TeamFortress_DetpackSet(  )
{
	gedict_t *countd;
	gedict_t *oldself, *owner;

//      int     skinno;

	owner = PROG_TO_EDICT( self->s.v.owner );
	self->is_detpacking = 0;
	owner->tfstate = owner->tfstate - ( owner->tfstate & TFSTATE_CANT_MOVE );
	TeamFortress_SetSpeed( owner );
	sound( owner, 2, "doors/medtry.wav", 1, 1 );
	oldself = self;
	self = owner;
	self->is_detpacking = 0;
	self->current_weapon = self->s.v.weapon;
	W_SetCurrentAmmo(  );
	self = oldself;
	newmis = spawn(  );
	g_globalvars.newmis = EDICT_TO_PROG( newmis );
	newmis->s.v.owner = self->s.v.owner;
	VectorCopy( owner->s.v.origin, newmis->s.v.origin );
	newmis->s.v.origin[2] -= 23;
#ifdef TG
	newmis->real_owner = self->real_owner;
#endif
	newmis->s.v.movetype = MOVETYPE_BOUNCE;
	newmis->s.v.solid = SOLID_BBOX;
	newmis->s.v.classname = "detpack";
	newmis->s.v.flags = 256;
	SetVector( newmis->s.v.angles, 90, 0, 0 );
	newmis->s.v.angles[1] = owner->s.v.angles[1];
	SetVector( newmis->s.v.velocity, 0, 0, 0 );
	SetVector( newmis->s.v.avelocity, 0, 0, 0 );
	newmis->weaponmode = 0;
	newmis->s.v.touch = ( func_t ) TeamFortress_DetpackTouch;
	if ( tf_data.birthday == 1 )
		setmodel( newmis, "progs/detpack2.mdl" );
	else
		setmodel( newmis, "progs/detpack.mdl" );
	setsize( newmis, -16, -16, 0, 16, 16, 8 );
	setorigin( newmis, PASSVEC3( owner->s.v.origin ) );
	countd = spawn(  );
	newmis->linked_list = countd;
	countd->s.v.think = ( func_t ) TeamFortress_DetpackCountDown;
	countd->s.v.health = self->s.v.health - 1;
	countd->s.v.owner = self->s.v.owner;
	countd->s.v.classname = "countdown_timer";
	countd->s.v.enemy = EDICT_TO_PROG( newmis );
	newmis->oldenemy = countd;
	if ( self->s.v.health <= 255 )
		countd->s.v.nextthink = g_globalvars.time + 1;
	else
	{
		countd->s.v.nextthink = g_globalvars.time + self->s.v.health - 10;
		countd->s.v.health = 9;
	}
	newmis->s.v.nextthink = g_globalvars.time + self->s.v.health;
	newmis->s.v.think = ( func_t ) TeamFortress_DetpackExplode;
	G_sprint( owner, 2, "Detpack set!\n" );
	dremove( self );
}

void TeamFortress_DetpackExplode(  )
{
	float   pos;
	float   points;
	gedict_t *head;
	gedict_t *te, *owner;
	vec3_t  org, vtemp;

	owner = PROG_TO_EDICT( self->s.v.owner );
	G_bprint( 1, "FIRE IN THE HOLE!\n" );
/*#ifndef TG
	G_sprint( owner, 2, "Your detpack exploded\n" );
#else
	G_sprint( self->real_owner, 2, "Your detpack exploded\n" );
#endif*/
	sound( self, 1, "weapons/detpack.wav", 1, 0 );
	if ( tf_data.birthday == 1 )
#ifndef TG
		G_bprint( 1, "%s spreads good cheer!\n", owner->s.v.netname );
#else
		G_bprint( 1, "%s spreads good cheer!\n", self->real_owner->s.v.netname );
#endif
	pos = trap_pointcontents( PASSVEC3( self->s.v.origin ) );
#ifndef TG
	if ( pos != -2 && pos != -6 && owner->has_disconnected != 1 )
	{
#else
	if ( pos != -2 && pos != -6 && self->real_owner->has_disconnected != 1 )
	{
#endif
		tf_data.deathmsg = 12;
		for ( head = world; head = findradius( head, self->s.v.origin, WEAP_DETPACK_SIZE ); )
		{
			if ( streq( head->s.v.classname, "info_tfgoal" ) )
			{
				if ( ( head->goal_activation & 2 ) && !head->search_time )
				{
					traceline( PASSVEC3( self->s.v.origin ), PASSVEC3( head->s.v.origin ), 1,
						   self );
					if ( g_globalvars.trace_fraction == 1 )
					{
#ifndef TG
						if ( Activated( head, owner ) )
							DoResults( head, owner, 1 );
#else
						if ( Activated( head, self->real_owner ) )
							DoResults( head, self->real_owner, 1 );
#endif
						else
						{
							if ( head->else_goal )
							{
								te = Findgoal( head->else_goal );
								if ( te )
#ifndef TG
									AttemptToActivate( te, owner, head );
#else
									AttemptToActivate( te, self->real_owner, head );
#endif
							}
						}
					}
				}
			} else
			{
				VectorSubtract( head->s.v.origin, self->s.v.origin, vtemp );
				if ( head->s.v.takedamage && vlen( vtemp ) <= 700 )
				{
					VectorAdd( head->s.v.mins, head->s.v.maxs, org );
					VectorScale( org, 0.5, org );
					VectorAdd( head->s.v.origin, org, org );
					VectorSubtract( head->s.v.origin, org, vtemp );
					points = 0.5 * vlen( vtemp );
					if ( points < 0 )
						points = 0;
					points = 700 - points;
					if ( points )
					{
						if ( CanDamage( head, self ) )
#ifndef TG
							TF_T_Damage( head, self, owner, points * 2, 2, 4 );
#else
							TF_T_Damage( head, self, self->real_owner, points * 2, 2, 4 );
#endif
					}
				}
			}
		}
		trap_WriteByte( 4, 23 );
		trap_WriteByte( 4, 3 );
		trap_WriteCoord( 4, self->s.v.origin[0] );
		trap_WriteCoord( 4, self->s.v.origin[1] );
		trap_WriteCoord( 4, self->s.v.origin[2] );
		trap_multicast( PASSVEC3( self->s.v.origin ), 1 );
	} else
#ifndef TG
		G_sprint( owner, 2, "Your detpack fizzled out.\n" );
#else
		G_sprint( self->real_owner, 2, "Your detpack fizzled out.\n" );
#endif
	if ( self->weaponmode == 1 )
	{
		TeamFortress_SetSpeed( PROG_TO_EDICT( self->s.v.enemy ) );
		dremove( self->oldenemy );
		dremove( self->observer_list );
	}
	BecomeExplosion(  );
}

void TeamFortress_DetpackTouch(  )
{
	gedict_t *disarm, *owner;
	vec3_t  source;
	vec3_t  dest;

//      vec3_t  org;
//      vec3_t  def;


#ifndef TG
#ifndef FIXDETPACKDROP
	CheckBelowBuilding( self );
#endif
#else
	if ( tg_data.detpack_drop )
		CheckBelowBuilding( self );
#endif
#ifdef TG
	if ( tg_data.disable_disarm )
		return;
#endif
	if ( strneq( other->s.v.classname, "player" ) )
		return;
	if ( other->playerclass != PC_SCOUT )
		return;
	if ( other->s.v.deadflag )
		return;
	if ( self->weaponmode == 1 )
		return;
	owner = PROG_TO_EDICT( self->s.v.owner );
#ifdef TG
	if ( (other->team_no == owner->team_no && owner->team_no ) && !tg_data.tg_enabled )
		return;
#else
	if ( other->team_no == owner->team_no && owner->team_no )
		return;
#endif
	makevectors( other->s.v.v_angle );
	VectorCopy( other->s.v.origin, source );
	source[2] += 16;

	VectorScale( g_globalvars.v_forward, 64, dest );
	VectorAdd( source, dest, dest );
	traceline( PASSVEC3( source ), PASSVEC3( dest ), 0, other );
	if ( g_globalvars.trace_fraction == 1 || g_globalvars.trace_ent != EDICT_TO_PROG( self ) )
		return;
// other.immune_to_check = g_globalvars.time + 5;
	other->tfstate = other->tfstate | TFSTATE_CANT_MOVE;
	G_sprint( other, 2, "Disarming detpack...\n" );
	TeamFortress_SetSpeed( other );
	disarm = spawn(  );
	disarm->s.v.owner = EDICT_TO_PROG( other );
	disarm->s.v.enemy = EDICT_TO_PROG( self );
	disarm->s.v.classname = "timer";
	disarm->s.v.nextthink = g_globalvars.time + WEAP_DETPACK_DISARMTIME;
	disarm->s.v.think = ( func_t ) TeamFortress_DetpackDisarm;
	self->weaponmode = 1;
	self->s.v.enemy = EDICT_TO_PROG( other );
	self->observer_list = disarm;
}

void TeamFortress_DetpackDisarm(  )
{
	gedict_t *owner = PROG_TO_EDICT( self->s.v.owner );
	gedict_t *enemy = PROG_TO_EDICT( self->s.v.enemy );

	if ( owner->s.v.deadflag )
	{
		dremove( self );
		return;
	}
	if ( tf_data.birthday == 1 )
	{
#ifndef TG
		G_bprint( 1, "%s stole %s's birthday present!\n", owner->s.v.netname,
			  PROG_TO_EDICT( enemy->s.v.owner )->s.v.netname );
#else
		G_bprint( 1, "%s stole %s's birthday present!\n", owner->s.v.netname, enemy->real_owner->s.v.netname );
#endif
	} else
	{
#ifndef TG
		G_bprint( 1, "%s's detpack was defused by %s\n", PROG_TO_EDICT( enemy->s.v.owner )->s.v.netname,
			  owner->s.v.netname );
#else
		G_bprint( 1, "%s's detpack was defused by %s\n", enemy->real_owner->s.v.netname, owner->s.v.netname );
#endif
	}
	owner->tfstate = owner->tfstate - ( owner->tfstate & TFSTATE_CANT_MOVE );
	TF_AddFrags( owner, 1 );
	TeamFortress_SetSpeed( owner );
	dremove( enemy->oldenemy );
	dremove( enemy );
	dremove( self );
}

void TeamFortress_DetpackCountDown(  )
{
	gedict_t *owner = PROG_TO_EDICT( self->s.v.owner );
	gedict_t *enemy = PROG_TO_EDICT( self->s.v.enemy );

#ifndef TG
#ifndef FIXDETPACKDROP
	CheckBelowBuilding( self );
#endif
#else
	if ( tg_data.detpack_drop )
		CheckBelowBuilding( self );
#endif

	self->s.v.nextthink = g_globalvars.time + 1;
	self->s.v.health = self->s.v.health - 1;
	if ( self->s.v.health < 10 )
	{
		G_sprint( owner, 2, "%.0f...\n", self->s.v.health );
		if ( self->s.v.health < 5 && !self->has_disconnected )
		{
			sound( enemy, 2, "doors/baseuse.wav", 1, 1 );
			self->has_disconnected = 1;
		}
		if ( self->s.v.health < 5 && !enemy->s.v.skin )
			enemy->s.v.skin = 1;
		else
			enemy->s.v.skin = 0;
		if ( !self->s.v.health )
			dremove( self );
	}
}
