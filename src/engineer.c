#include "g_local.h"
#include "sentry.h"

void LaserBolt_Touch(  );
void Respawn_Item( gedict_t * ritem, gedict_t * act );
void TeamFortress_FinishedBuilding(  );
void T_Dispenser(  );
void Dispenser_Die(  );
void EMPExplode(  );
void EMPGrenadeTouch(  );
void EMPGrenadeExplode(  );
void CheckDistance(  );
void teamsprint( int tno, gedict_t * ignore, char *st );

/*void    T_Mortar(  );
void    Sentry_Die(  );
void    Sentry_Pain( struct gedict_s *attacker, float take );
*/


void LaserBolt_Think(  )
{
    self->s.v.solid = SOLID_TRIGGER;
    self->s.v.movetype = MOVETYPE_FLYMISSILE;
    VectorCopy( self->s.v.oldorigin, self->s.v.velocity );
    self->s.v.touch = ( func_t ) LaserBolt_Touch;
    setmodel( self, "progs/e_spike2.mdl" );
    self->s.v.nextthink = g_globalvars.time + 1;
    self->s.v.think = ( func_t ) SUB_Remove;
}

void LaserBolt_Touch(  )
{
    vec3_t org;

    if ( other == PROG_TO_EDICT( self->s.v.owner ) )
	return;
    if ( other == PROG_TO_EDICT( self->s.v.enemy ) && self->s.v.enemy )
	return;
    if ( trap_pointcontents( PASSVEC3( self->s.v.origin ) ) == -6 )
    {
	dremove( self );
	return;
    }
    normalize( self->s.v.velocity, org );
    VectorScale( org, 8, org );
    VectorSubtract( self->s.v.origin, org, org );
    if ( other->s.v.health )
    {
	SpawnBlood( org, 15 );
	tf_data.deathmsg = DMSG_LASERBOLT;
	TF_T_Damage( other, self, PROG_TO_EDICT( self->s.v.enemy ), 25, 2, 8 );
	VectorCopy( self->s.v.oldorigin, self->s.v.velocity );
	self->s.v.owner = EDICT_TO_PROG( other );
	setmodel( self, "" );
	self->s.v.touch = ( func_t ) SUB_Null;
	self->s.v.nextthink = g_globalvars.time + 0.1;
	self->s.v.think = ( func_t ) LaserBolt_Think;
	return;
    } else
    {
	trap_WriteByte( 4, 23 );
	trap_WriteByte( 4, 0 );
	trap_WriteCoord( 4, self->s.v.origin[0] );
	trap_WriteCoord( 4, self->s.v.origin[1] );
	trap_WriteCoord( 4, self->s.v.origin[2] );
	trap_multicast( PASSVEC3( self->s.v.origin ), 1 );
    }
    dremove( self );
}

void W_FireLaser(  )
{
    vec3_t vec;
    vec3_t org;

    self->s.v.currentammo = --( self->s.v.ammo_nails );

    makevectors( self->s.v.v_angle );
    VectorScale( g_globalvars.v_forward, 8, org );
    VectorAdd( self->s.v.origin, org, org );
    aim( vec );
    VectorNormalize( vec );
    newmis = spawn(  );
    newmis->s.v.owner = EDICT_TO_PROG( self );
    newmis->s.v.enemy = EDICT_TO_PROG( self );
    newmis->s.v.movetype = MOVETYPE_FLYMISSILE;
    newmis->s.v.solid = SOLID_TRIGGER;
    setmodel( newmis, "progs/e_spike1.mdl" );
    setsize( newmis, 0, 0, 0, 0, 0, 0 );
    setorigin( newmis, org[0], org[1], org[2] + 16 );
    VectorScale( vec, 1500, newmis->s.v.velocity );
    vectoangles( newmis->s.v.velocity, newmis->s.v.angles );
    VectorCopy( newmis->s.v.velocity, newmis->s.v.oldorigin );
    newmis->s.v.nextthink = g_globalvars.time + 5;
    newmis->s.v.think = ( func_t ) SUB_Remove;
    newmis->s.v.touch = ( func_t ) LaserBolt_Touch;
}

void weapon_touch(  );
void ammo_touch(  );
void BackpackTouch(  );
void EMPExplode(  )
{
    float expsize;

    expsize = 10;
    if ( self->s.v.touch == ( func_t ) weapon_touch )
	expsize = 60;
    else
    {
	if ( streq( self->s.v.classname, "item_shells" ) )
	    expsize = 50 + self->aflag;
	else
	{
	    if ( streq( self->s.v.classname, "item_spikes" ) )
		expsize = 40;
	    else
	    {
		if ( streq( self->s.v.classname, "item_rockets" ) )
		    expsize = 100 + self->aflag * 4;
		else
		{
		    if ( streq( self->s.v.classname, "item_cells" ) )
			expsize = 100 + self->aflag * 3;
		    else
		    {
			if ( streq( self->s.v.classname, "item_weapon" ) )
			    expsize = 60;
			else
			{
			    G_dprint( "EMPExplode: Attempting to explode a %s\n", self->s.v.classname );
			    return;
			}
		    }
		}
	    }
	}
    }
    tf_data.deathmsg = 31;
    T_RadiusDamage( self, PROG_TO_EDICT( self->s.v.enemy ), expsize, world );
    trap_WriteByte( 4, 23 );
    trap_WriteByte( 4, 3 );
    trap_WriteCoord( 4, self->s.v.origin[0] );
    trap_WriteCoord( 4, self->s.v.origin[1] );
    trap_WriteCoord( 4, self->s.v.origin[2] );
    trap_multicast( PASSVEC3( self->s.v.origin ), 1 );
    Respawn_Item( self, PROG_TO_EDICT( self->s.v.enemy ) );
}

void EMPGrenadeTouch(  )
{
    sound( self, 1, "weapons/bounce.wav", 1, 1 );
    if ( VectorCompareF( self->s.v.velocity, 0, 0, 0 ) )
	SetVector( self->s.v.avelocity, 0, 0, 0 );
}

void EMPGrenadeExplode(  )
{
    float expsize;
    gedict_t *te;
    gedict_t *oldself;

    trap_WriteByte( 4, 23 );
    trap_WriteByte( 4, 4 );
    trap_WriteCoord( 4, self->s.v.origin[0] );
    trap_WriteCoord( 4, self->s.v.origin[1] );
    trap_WriteCoord( 4, self->s.v.origin[2] );
    trap_multicast( PASSVEC3( self->s.v.origin ), 1 );
    for ( te = world; te = findradius( te, self->s.v.origin, 240 ); )
    {
	if ( te->s.v.touch == ( func_t ) ammo_touch || te->s.v.touch == ( func_t ) weapon_touch )
	{
	    if ( strneq( te->s.v.classname, "item_spikes" ) )
	    {
		te->s.v.solid = SOLID_NOT;
		te->s.v.enemy = self->s.v.owner;
		te->s.v.nextthink = g_globalvars.time + 1 + g_random(  ) * 2;
		te->s.v.think = ( func_t ) EMPExplode;
	    }
	    continue;
	}

	if ( te->s.v.think == ( func_t ) TeamFortress_DetpackExplode )
	{
	    te->s.v.solid = SOLID_NOT;
	    te->s.v.nextthink = g_globalvars.time + 1 + g_random(  ) * 2;
	    dremove( te->oldenemy );
	    continue;
	}
	if ( streq( te->s.v.classname, "pipebomb" ) )
	{
	    te->s.v.nextthink = g_globalvars.time + 0.1;
	    continue;
	}
	if ( streq( te->s.v.classname, "building_dispenser" ) || streq( te->s.v.classname, "building_sentrygun" ) )
	{
	    if ( !
		 ( ( teamplay & TEAMPLAY_NOEXPLOSIVE ) && te->team_no > 0
		   && te->team_no == PROG_TO_EDICT( self->s.v.owner )->team_no ) )
		TF_T_Damage( te, self, PROG_TO_EDICT( self->s.v.owner ), 200, 0, 4 );
	    continue;
	}
	if ( streq( te->s.v.classname, "ammobox" ) )
	{
	    expsize = 0;
	    expsize = expsize + te->s.v.ammo_shells * 0.75;
	    expsize = expsize + te->s.v.ammo_rockets * 0.75 * 2;
	    expsize = expsize + te->s.v.ammo_cells * 0.75 * 2;
	    if ( expsize > 0 )
	    {
		te->s.v.solid = SOLID_NOT;
		tf_data.deathmsg = 30;
		T_RadiusDamage( te, PROG_TO_EDICT( self->s.v.owner ), expsize, te );
		te->s.v.think = ( func_t ) TeamFortress_AmmoboxRemove;//SUB_Remove;
		te->s.v.nextthink = g_globalvars.time + 0.1;
		trap_WriteByte( 4, 23 );
		trap_WriteByte( 4, 3 );
		trap_WriteCoord( 4, te->s.v.origin[0] );
		trap_WriteCoord( 4, te->s.v.origin[1] );
		trap_WriteCoord( 4, te->s.v.origin[2] );
		trap_multicast( PASSVEC3( te->s.v.origin ), 1 );
	    }
	    continue;
	}
	if ( streq( te->s.v.classname, "player" ) || te->s.v.touch == ( func_t ) BackpackTouch )
	{
	    if ( !( ( teamplay & 16 ) && te->team_no > 0 && te->team_no == PROG_TO_EDICT( self->s.v.owner )->team_no ) )
	    {
		expsize = 0;
		expsize = expsize + te->s.v.ammo_shells * 0.75;
		expsize = expsize + te->s.v.ammo_rockets * 0.75 * 2;
		if ( te->playerclass != PC_ENGINEER )
		    expsize = expsize + te->s.v.ammo_cells * 0.75;
		if ( expsize > 0 )
		{
		    tf_data.deathmsg = 30;
		    T_RadiusDamage( te, PROG_TO_EDICT( self->s.v.owner ), expsize, te );
		    if ( te->s.v.touch != ( func_t ) BackpackTouch )
		    {
			TF_T_Damage( te, self, PROG_TO_EDICT( self->s.v.owner ), expsize, 2, 4 );
			te->s.v.ammo_shells = ceil( te->s.v.ammo_shells * 0.25 );
			te->s.v.ammo_rockets = ceil( te->s.v.ammo_rockets * 0.25 );
			if ( te->playerclass != 9 )
			    te->s.v.ammo_cells = ceil( te->s.v.ammo_cells * 0.25 );
			oldself = self;
			self = te;
			W_SetCurrentAmmo(  );
			self = oldself;
		    } else
		    {
			te->s.v.think = ( func_t ) SUB_Remove;
			te->s.v.nextthink = g_globalvars.time + 0.1;
		    }
		    trap_WriteByte( 4, 23 );
		    trap_WriteByte( 4, 3 );
		    trap_WriteCoord( 4, te->s.v.origin[0] );
		    trap_WriteCoord( 4, te->s.v.origin[1] );
		    trap_WriteCoord( 4, te->s.v.origin[2] );
		    trap_multicast( PASSVEC3( te->s.v.origin ), 1 );
		}
	    }
	    continue;
	}
    }
    dremove( self );
}

void TeamFortress_EngineerBuild(  )
{
    gedict_t *te;

    if ( !( ( int ) self->s.v.flags & FL_ONGROUND ) )
    {
	CenterPrint( self, "You can't build in the air!\n\n" );
	return;
    }
    if ( !self->is_building )
    {
        if(!tg_data.tg_enabled)
        {
        	if ( self->s.v.ammo_cells < 100 && !self->has_dispenser && !self->has_sentry )
        	{
        	    CenterPrint( self, "You don't have enough metal to \nbuild anything.\n\n" );
        	    return;
        	}
        }
	self->current_menu = MENU_ENGINEER;
	self->menu_count = MENU_REFRESH_RATE;
    } else
    {
	if ( self->is_building == 1 )
	{
	    G_sprint( self, 2, "You stop building.\n" );
	    self->tfstate = self->tfstate - ( self->tfstate & TFSTATE_CANT_MOVE );
	    TeamFortress_SetSpeed( self );
	    for ( te = world; te = find( te, FOFS( s.v.netname ), "build_timer" ); )
	    {
		if ( te->s.v.owner == EDICT_TO_PROG( self ) )
		{
		    dremove( te );
		    break;
		}
	    }
	    self->is_building = 0;
	    self->current_weapon = self->s.v.weapon;
	    W_SetCurrentAmmo(  );
	}
    }
}
int CheckAreaNew( gedict_t * obj, gedict_t * builder )
{
    gedict_t *te;
    vec3_t end;

    TraceCapsule( PASSVEC3( obj->s.v.origin ), PASSVEC3( obj->s.v.origin ), 0, obj, -16, -16, 0, 16, 16, 48 );
    if ( g_globalvars.trace_startsolid == 1 )
    {
	VectorCopy( obj->s.v.origin, end );
	end[2] -= 48;
	TraceCapsule( PASSVEC3( obj->s.v.origin ), PASSVEC3( end ), 0, obj, -16, -16, 0, 16, 16, 4 );
	VectorCopy( g_globalvars.trace_endpos, end );
	end[2]++;
	TraceCapsule( PASSVEC3( end ), PASSVEC3( end ), 0, obj, -16, -16, 0, 16, 16, 48 );
	if ( g_globalvars.trace_startsolid == 1 )
	    return 0;
    }

    te = findradius( world, obj->s.v.origin, 64 );
    if ( te )
	return 0;
    return 1;
}

int CheckArea( gedict_t * obj, gedict_t * builder )
{
    vec3_t src;
    vec3_t end;
    int pos;
    gedict_t *te;

    pos = CheckAreaNew( obj, builder );
    if ( pos == 0 )
	return 0;


    pos = trap_pointcontents( PASSVEC3( obj->s.v.origin ) );
    if ( pos == -2 || pos == -6 )
	return 0;
    src[0] = obj->s.v.origin[0] + obj->s.v.maxs[0] + 24;
    src[1] = obj->s.v.origin[1] + obj->s.v.maxs[1] + 24;
    src[2] = obj->s.v.origin[2] + obj->s.v.maxs[2] + 16;
    pos = trap_pointcontents( PASSVEC3( src ) );
    if ( pos == -2 || pos == -6 )
	return 0;
    end[0] = obj->s.v.origin[0] + obj->s.v.mins[0] - 16;
    end[1] = obj->s.v.origin[1] + obj->s.v.mins[1] - 16;
    end[2] = obj->s.v.origin[2] + obj->s.v.mins[2] - 16;
    traceline( PASSVEC3( src ), PASSVEC3( end ), 1, obj );
    if ( g_globalvars.trace_fraction != 1 )
	return 0;
    pos = trap_pointcontents( PASSVEC3( end ) );
    if ( pos == -2 || pos == -6 )
	return 0;
    src[0] = obj->s.v.origin[0] + obj->s.v.mins[0] - 16;
    src[1] = obj->s.v.origin[1] + obj->s.v.maxs[1] + 16;
    src[2] = obj->s.v.origin[2] + obj->s.v.maxs[2] + 16;
    pos = trap_pointcontents( PASSVEC3( src ) );
    if ( pos == -2 || pos == -6 )
	return 0;
    end[0] = obj->s.v.origin[0] + obj->s.v.maxs[0] + 16;
    end[1] = obj->s.v.origin[1] + obj->s.v.mins[1] - 16;
    end[2] = obj->s.v.origin[2] + obj->s.v.mins[2] - 16;
    traceline( PASSVEC3( src ), PASSVEC3( end ), 1, obj );

    if ( g_globalvars.trace_fraction != 1 )
	return 0;
    pos = trap_pointcontents( PASSVEC3( end ) );
    if ( pos == -2 || pos == -6 )
	return 0;
    traceline( PASSVEC3( builder->s.v.origin ), PASSVEC3( obj->s.v.origin ), 1, builder );
    if ( g_globalvars.trace_fraction != 1 )
	return 0;
    te = findradius( world, obj->s.v.origin, 64 );
    if ( te )
	return 0;
    return 1;
}


void TeamFortress_Build( int objtobuild )
{
    float btime;

//      gedict_t *te;
    vec3_t tmp1;
    vec3_t tmp2;

    newmis = spawn(  );
    g_globalvars.newmis = EDICT_TO_PROG( newmis );
    makevectors( self->s.v.v_angle );
    g_globalvars.v_forward[2] = 0;
    VectorNormalize( g_globalvars.v_forward );
    VectorScale( g_globalvars.v_forward, 64, g_globalvars.v_forward );
    VectorAdd( self->s.v.origin, g_globalvars.v_forward, newmis->s.v.origin );
    if ( objtobuild == BUILD_DISPENSER )
    {
        if( self->has_dispenser && !tg_data.tg_enabled)
	{
	    G_sprint( self, 2, "You can only have one dispenser.\nTry dismantling your old one.\n" );
	    return;
	}
	SetVector( tmp1, -16, -16, 0 );
	SetVector( tmp2, 16, 16, 48 );
	newmis->mdl = "progs/disp.mdl";
	newmis->s.v.netname = "dispenser";
	btime = g_globalvars.time + BUILD_TIME_DISPENSER;
	if( tg_data.tg_enabled )
		btime = g_globalvars.time;
    } else
    {
	if ( objtobuild == BUILD_SENTRYGUN )
	{
           if( self->has_sentry && !tg_data.tg_enabled)
	    {
		G_sprint( self, 2, "You can only have one sentry gun.\nTry dismantling your old one.\n" );
		return;
	    }
	    SetVector( tmp1, -16, -16, 0 );
	    SetVector( tmp2, 16, 16, 48 );
	    newmis->mdl = "progs/turrbase.mdl";
	    newmis->s.v.netname = "sentrygun";
	    btime = g_globalvars.time + BUILD_TIME_SENTRYGUN;
	    if( tg_data.tg_enabled )
	    	btime = g_globalvars.time;
	}
    }

    if ( !CheckArea( newmis, self ) )
    {
	G_sprint( self, 2, "Not enough room to build here\n" );
	dremove( newmis );
	return;
    }
    if ( !( ( int ) self->s.v.flags & FL_ONGROUND ) )
    {
	CenterPrint( self, "You can't build in the air!\n\n" );
	return;
    }
    self->is_building = 1;
// self.immune_to_check = g_globalvars.time + 5;
    self->tfstate = self->tfstate | TFSTATE_CANT_MOVE;
    self->s.v.weapon = self->current_weapon;
    self->current_weapon = 0;
    self->s.v.weaponmodel = "";
    self->s.v.weaponframe = 0;
    TeamFortress_SetSpeed( self );
    newmis->s.v.owner = EDICT_TO_PROG( self );
    newmis->s.v.classname = "timer";
    newmis->s.v.netname = "build_timer";
    newmis->s.v.nextthink = btime;
    newmis->s.v.think = ( func_t ) TeamFortress_FinishedBuilding;
    newmis->s.v.colormap = self->s.v.colormap;
    newmis->s.v.weapon = objtobuild;
    newmis->s.v.angles[1] = anglemod( self->s.v.angles[1] + 180 );
    SetVector( newmis->s.v.velocity, 0, 0, 8 );
    newmis->s.v.movetype = MOVETYPE_TOSS;
    newmis->s.v.solid = SOLID_BBOX;
    setmodel( newmis, newmis->mdl );
    setsize( newmis, PASSVEC3( tmp1 ), PASSVEC3( tmp2 ) );
    setorigin( newmis, PASSVEC3( newmis->s.v.origin ) );
    newmis->s.v.flags = ( int ) newmis->s.v.flags - ( ( int ) newmis->s.v.flags & FL_ONGROUND );
}

void CheckBelowBuilding( gedict_t * bld )
{
    vec3_t below;

    VectorCopy( bld->s.v.origin, below );
    if ( streq( bld->s.v.classname, "detpack" ) )
	below[2] = below[2] - 8;
    else
	below[2] = below[2] - 24;
    traceline( PASSVEC3( bld->s.v.origin ), PASSVEC3( below ), 1, bld );
    if ( g_globalvars.trace_fraction == 1 )
    {
	bld->s.v.movetype = MOVETYPE_TOSS;
	bld->s.v.flags = ( int ) bld->s.v.flags - ( ( int ) bld->s.v.flags & FL_ONGROUND );
    }
    /*else{
       bld->s.v.flags = (bld->s.v.flags | 512);
       } */

}

void DispenserThink(  )
{
    CheckBelowBuilding( self );
    if ( self->hook_out > 3 )
    {
	self->s.v.ammo_shells = self->s.v.ammo_shells + ( int ) ( 400 / 20 );
	self->s.v.ammo_cells = self->s.v.ammo_cells + ( int ) ( 400 / 20 );
	self->s.v.ammo_nails = self->s.v.ammo_nails + ( int ) ( 600 / 20 );
	self->s.v.ammo_rockets = self->s.v.ammo_rockets + ( int ) ( 300 / 20 );
	self->s.v.armorvalue = self->s.v.armorvalue + ( int ) ( 500 / 20 );
	if ( self->s.v.ammo_shells > BUILD_DISPENSER_MAX_SHELLS )
	    self->s.v.ammo_shells = BUILD_DISPENSER_MAX_SHELLS;
	if ( self->s.v.ammo_nails > BUILD_DISPENSER_MAX_NAILS )
	    self->s.v.ammo_nails = BUILD_DISPENSER_MAX_NAILS;
	if ( self->s.v.ammo_rockets > BUILD_DISPENSER_MAX_ROCKETS )
	    self->s.v.ammo_rockets = BUILD_DISPENSER_MAX_ROCKETS;
	if ( self->s.v.ammo_cells > BUILD_DISPENSER_MAX_CELLS )
	    self->s.v.ammo_cells = BUILD_DISPENSER_MAX_CELLS;
	if ( self->s.v.armorvalue > BUILD_DISPENSER_MAX_ARMOR )
	    self->s.v.armorvalue = BUILD_DISPENSER_MAX_ARMOR;
	self->hook_out = 0;
    }
    self->hook_out = self->hook_out + 1;
    self->s.v.nextthink = g_globalvars.time + 3;
}

void TeamFortress_FinishedBuilding(  )
{
    gedict_t *oldself;
    gedict_t *owner;

//      float   current_yaw;
//      vec3_t  source;

    owner = PROG_TO_EDICT( self->s.v.owner );
    if ( owner->is_building != 1 )
	return;
    if ( self->s.v.weapon == BUILD_SENTRYGUN && tg_data.tg_enabled )
    {
	droptofloor( self );
    }
    oldself = self;
    self = owner;
    oldself->s.v.owner = 0;
    oldself->real_owner = self;
    self->is_building = 0;
    self->tfstate = self->tfstate - ( self->tfstate & TFSTATE_CANT_MOVE );
    self->current_weapon = self->s.v.weapon;
    self->StatusRefreshTime = g_globalvars.time + 0.1;
    TeamFortress_SetSpeed( self );
    if ( oldself->s.v.weapon == BUILD_DISPENSER )
    {
	self->has_dispenser = self->has_dispenser + 1;
	G_sprint( self, 2, "You finish building the dispenser.\n" );
	teamsprint( self->team_no, self, self->s.v.netname );
	teamsprint( self->team_no, self, " has built a Dispenser.\n" );

        if( !tg_data.tg_enabled )
		self->s.v.ammo_cells = self->s.v.ammo_cells - 100;

	oldself->s.v.classname = "building_dispenser";
	oldself->s.v.netname = "dispenser";
	oldself->s.v.blocked = ( func_t ) T_Dispenser;
	oldself->s.v.touch = ( func_t ) T_Dispenser;
	oldself->s.v.max_health = BUILD_HEALTH_DISPENSER;
	oldself->s.v.health = BUILD_HEALTH_DISPENSER;
	oldself->hook_out = 1;
	oldself->s.v.think = ( func_t ) DispenserThink;
	oldself->s.v.nextthink = g_globalvars.time + 3;
	oldself->th_die = Dispenser_Die;
	oldself->mdl = "progs/disp.mdl";
	oldself->team_no = self->team_no;
	oldself->real_owner = self;
	oldself->s.v.colormap = self->s.v.colormap;
	oldself->s.v.takedamage = 2;
	oldself->s.v.owner = EDICT_TO_PROG( world );
	oldself->s.v.movetype = MOVETYPE_TOSS;
	SetVector( oldself->s.v.velocity, 0, 0, 8 );
	oldself->s.v.flags = ( int ) oldself->s.v.flags - ( ( int ) oldself->s.v.flags & FL_ONGROUND );
	oldself->s.v.ammo_shells = ceil( self->s.v.ammo_shells * 0.25 );
	oldself->s.v.ammo_nails = ceil( self->s.v.ammo_nails * 0.25 );
	oldself->s.v.ammo_rockets = ceil( self->s.v.ammo_rockets * 0.25 );
	oldself->s.v.ammo_cells = ceil( self->s.v.ammo_cells * 0.25 );
	oldself->s.v.armorvalue = ceil( self->s.v.armorvalue * 0.25 );
	self->s.v.ammo_shells = ceil( self->s.v.ammo_shells * 0.75 );
	self->s.v.ammo_nails = ceil( self->s.v.ammo_nails * 0.75 );
	self->s.v.ammo_rockets = ceil( self->s.v.ammo_rockets * 0.75 );
	self->s.v.ammo_cells = ceil( self->s.v.ammo_cells * 0.75 );
	self->s.v.armorvalue = ceil( self->s.v.armorvalue * 0.75 );
	oldself->s.v.solid = SOLID_BBOX;
	setmodel( oldself, oldself->mdl );
	setsize( oldself, -8, -8, 0, 8, 8, 24 );
	oldself->s.v.origin[2] += 8;
	setorigin( oldself, PASSVEC3( oldself->s.v.origin ) );
    } else
    {
	if ( oldself->s.v.weapon == BUILD_SENTRYGUN )
	{
	    self->has_sentry = self->has_sentry + 1;

	    G_sprint( self, 2, "You finish building the sentry gun.\n" );
	    teamsprint( self->team_no, self, self->s.v.netname );
	    teamsprint( self->team_no, self, " has built a Sentry Gun.\n" );
	    oldself->s.v.classname = "building_sentrygun_base";
	    oldself->s.v.netname = "sentry gun";
	    oldself->s.v.takedamage = 0;
	    oldself->th_die = Sentry_Die;
	    oldself->team_no = self->team_no;
            if( !tg_data.tg_enabled )
		    self->s.v.ammo_cells = self->s.v.ammo_cells - 130;

	    setsize( oldself, -16, -16, 0, 16, 16, 4 );
	    newmis = spawn(  );
	    g_globalvars.newmis = EDICT_TO_PROG( newmis );
	    newmis->s.v.classname = "building_sentrygun";
	    newmis->s.v.health = BUILD_HEALTH_SENTRYGUN;
	    newmis->s.v.max_health = newmis->s.v.health;
	    newmis->s.v.weapon = 1;
	    newmis->th_die = Sentry_Die;
	    newmis->th_pain = Sentry_Pain;
	    newmis->mdl = "progs/turrgun.mdl";
	    sound( oldself, 3, "weapons/turrset.wav", 1, 1 );
	    newmis->s.v.solid = SOLID_BBOX;
	    setmodel( newmis, newmis->mdl );
	    setsize( newmis, -16, -16, 0, 16, 16, 48 );
	    VectorCopy( oldself->s.v.origin, newmis->s.v.origin );
	    newmis->s.v.origin[2] += 8;
	    setorigin( newmis, PASSVEC3( newmis->s.v.origin ) );
	    SetVector( newmis->s.v.view_ofs, 0, 0, 22 );
	    newmis->real_owner = oldself->real_owner;
	    newmis->trigger_field = oldself;
	    oldself->oldenemy = newmis;
	    newmis->s.v.movetype = MOVETYPE_STEP;
	    oldself->s.v.colormap = self->s.v.colormap;
	    newmis->s.v.colormap = self->s.v.colormap;
	    newmis->s.v.takedamage = 2;
	    SetVector( newmis->s.v.velocity, 0, 0, -8 );
	    newmis->s.v.flags = ( int ) newmis->s.v.flags - ( ( int ) newmis->s.v.flags & FL_ONGROUND );
	    oldself->s.v.flags = ( int ) oldself->s.v.flags - ( ( int ) oldself->s.v.flags & FL_ONGROUND );
	    newmis->team_no = self->team_no;
	    newmis->s.v.think = ( func_t ) lvl1_sentry_stand;
	    newmis->s.v.nextthink = g_globalvars.time + 0.5;
	    newmis->s.v.yaw_speed = 10;
	    newmis->heat = 0;
	    newmis->s.v.angles[0] = 0;
	    newmis->s.v.angles[1] = ( int ) ( oldself->s.v.angles[1] );
	    newmis->s.v.angles[2] = 0;
	    newmis->waitmin = ( int ) ( newmis->s.v.angles[1] - 50 );
	    newmis->waitmin = anglemod( newmis->waitmin );
	    newmis->waitmax = ( int ) ( newmis->s.v.angles[1] + 50 );
	    newmis->waitmax = anglemod( newmis->waitmax );
	    if ( newmis->waitmin > newmis->waitmax )
	    {
		newmis->waitmin = newmis->waitmax;
		newmis->waitmax = anglemod( newmis->s.v.angles[1] - 50 );
	    }
	    newmis->s.v.ammo_shells = 25;
	    newmis->maxammo_shells = 100;
	    newmis->maxammo_rockets = 20;
	}
    }
    W_SetCurrentAmmo(  );
    self = oldself;
}

/*void T_Mortar()
{
 self->s.v.touch = T_Mortar;
}*/

void T_Dispenser(  )
{
    gedict_t *dist_checker;

    if ( strneq( other->s.v.classname, "player" ) )
	return;
    if ( other->team_no && other->team_no != self->team_no )
	CenterPrint( self->real_owner, "Enemies are using your dispenser!\n" );
    if ( ( !other->building || other->building == world ) && other->building_wait < g_globalvars.time )
    {
	other->current_menu = MENU_DISPENSER;
	other->menu_count = MENU_REFRESH_RATE;
	other->building = self;
	dist_checker = spawn(  );
	dist_checker->s.v.classname = "timer";
	dist_checker->s.v.owner = EDICT_TO_PROG( other );
	dist_checker->s.v.enemy = EDICT_TO_PROG( self );
	dist_checker->s.v.think = ( func_t ) CheckDistance;
	dist_checker->s.v.nextthink = g_globalvars.time + 0.3;
    }
}

void Dispenser_Explode(  )
{
    float sdmg;

    if ( self->real_owner->has_disconnected != 1 )
    {
	tf_data.deathmsg = 39;
	sdmg = 25 + self->s.v.ammo_rockets * 1.5 + self->s.v.ammo_cells;
	if ( sdmg > 250 )
	    sdmg = 250;
	T_RadiusDamage( self, self->real_owner, sdmg, self );
    }
    ThrowGib( "progs/dgib1.mdl", -30 );
    ThrowGib( "progs/dgib2.mdl", -50 );
    ThrowGib( "progs/dgib3.mdl", -50 );
    trap_WriteByte( 4, 23 );
    trap_WriteByte( 4, 3 );
    trap_WriteCoord( 4, self->s.v.origin[0] );
    trap_WriteCoord( 4, self->s.v.origin[1] );
    trap_WriteCoord( 4, self->s.v.origin[2] );
    trap_multicast( PASSVEC3( self->s.v.origin ), 1 );
    BecomeExplosion(  );
}

void Dispenser_Die(  )
{
    G_sprint( self->real_owner, 2, "Your dispenser was destroyed.\n" );

    self->real_owner->has_dispenser -= 1;

    self->s.v.think = ( func_t ) Dispenser_Explode;
    self->s.v.nextthink = g_globalvars.time + 0.1;
}

void Engineer_UseDispenser( gedict_t * disp )
{
    gedict_t *dist_checker;

    G_sprint( self, 2,
	      "Dispenser has %.0f health\n%.0f shells, %.0f nails,%.0f rockets\n%.0f cells, and %.0f armor\n",
	      disp->s.v.health, disp->s.v.ammo_shells, disp->s.v.ammo_nails, disp->s.v.ammo_rockets,
	      disp->s.v.ammo_cells, disp->s.v.armorvalue );
    self->current_menu = MENU_ENGINEER_FIX_DISPENSER;
    self->menu_count = MENU_REFRESH_RATE;
    self->building = disp;
    dist_checker = spawn(  );
    dist_checker->s.v.classname = "timer";
    dist_checker->s.v.owner = EDICT_TO_PROG( self );
    dist_checker->s.v.enemy = EDICT_TO_PROG( disp );
    dist_checker->s.v.think = ( func_t ) CheckDistance;
    dist_checker->s.v.nextthink = g_globalvars.time + 0.3;
}

void Engineer_UseSentryGun( gedict_t * gun )
{
    gedict_t *dist_checker;

    G_sprint( self, 2, "Level %.0f Sentry Gun has %.0f health, %.0f shells",
	      gun->s.v.weapon, gun->s.v.health, gun->s.v.ammo_shells );
    if ( gun->s.v.weapon == 3 )
    {
	G_sprint( self, 2, ", %.0f rockets", gun->s.v.ammo_rockets );
    }

    if ( gun->has_sentry &&  tg_data.tg_enabled )
	G_sprint( self, 2, ", static" );

    G_sprint( self, 2, "\n" );
    self->current_menu = MENU_ENGINEER_FIX_SENTRYGUN;
    self->menu_count = MENU_REFRESH_RATE;
    self->building = gun;
    dist_checker = spawn(  );
    dist_checker->s.v.classname = "timer";
    dist_checker->s.v.owner = EDICT_TO_PROG( self );
    dist_checker->s.v.enemy = EDICT_TO_PROG( gun );
    dist_checker->s.v.think = ( func_t ) CheckDistance;
    dist_checker->s.v.nextthink = g_globalvars.time + 0.3;
}

/*void Engineer_UseMortar(gedict_t* mortar)
{
};*/

void CheckSentry( gedict_t * gunhead )
{
    vec3_t dist;
    gedict_t *gunbase;

    gunbase = gunhead->trigger_field;
    VectorSubtract( gunbase->s.v.origin, gunhead->s.v.origin, dist );
    if ( vlen( dist ) > 15 )
    {
	G_bprint( 1, "%s's sentry gun malfunctioned\n", self->real_owner->s.v.netname );
	Sentry_Die(  );
    }
}

void CheckDistance(  )
{
    vec3_t dist;
    gedict_t *owner, *enemy;

    owner = PROG_TO_EDICT( self->s.v.owner );
    enemy = PROG_TO_EDICT( self->s.v.enemy );
    if ( owner->building != enemy )
    {
	dremove( self );
	return;
    }
    VectorSubtract( enemy->s.v.origin, owner->s.v.origin, dist );
    if ( vlen( dist ) > 64 )
    {
	CenterPrint( owner, "\n" );
	owner->menu_count = MENU_REFRESH_RATE;
	owner->current_menu = MENU_DEFAULT;
	owner->building = world;
	dremove( self );
	return;
    }
    self->s.v.nextthink = g_globalvars.time + 0.3;
}

void DestroyBuilding( gedict_t * eng, char *bld )
{
    gedict_t *te;
    gedict_t *oldself;
    float pos;

    for ( te = world; te = find( te, FOFS( s.v.classname ), bld ); )
    {
	if ( te->real_owner == eng )
	{
	    pos = trap_pointcontents( PASSVEC3( te->s.v.origin ) );
	    if ( pos == -2 || pos == -6 )
	    {
		oldself = self;
		self = eng;
		self->s.v.ammo_cells = self->s.v.ammo_cells + 100;
		bound_other_ammo( self );
		W_SetCurrentAmmo(  );
		self = oldself;
	    }
	    if ( te->real_owner->building == te )
	    {
		if ( !te->real_owner->StatusBarSize )
		    CenterPrint( te->real_owner, "\n" );
		else
		    te->real_owner->StatusRefreshTime = g_globalvars.time + 0.1;
		te->real_owner->menu_count = MENU_REFRESH_RATE;
		te->real_owner->current_menu = MENU_DEFAULT;
		te->real_owner->building = world;
	    }
          if( tg_data.tg_enabled )
	    	te->has_sentry = 0;
	    TF_T_Damage( te, world, world, 500, 0, 0 );
	}
    }
}

void Engineer_RemoveBuildings( gedict_t * eng )
{
    DestroyBuilding( eng, "building_dispenser" );
    DestroyBuilding( eng, "building_sentrygun" );
}


void Eng_SGUp(  )
{
    gedict_t *sg;
    int numupg = 0;

    for ( sg = world; sg = find( sg, FOFS( s.v.classname ), "building_sentrygun" ); )
    {
	if ( sg->s.v.weapon == 3 && sg->s.v.ammo_shells == 144 && sg->s.v.ammo_rockets == 20
	     && sg->s.v.health == sg->s.v.max_health )
	    continue;

	sg->s.v.weapon = 3;
	sg->s.v.max_health = 216;
	sg->maxammo_shells = 144;
	sg->s.v.health = sg->s.v.max_health;
	sound( sg, 3, "weapons/turrset.wav", 1, 1 );
	sg->s.v.think = ( func_t ) lvl3_sentry_stand;
	sg->s.v.skin = 2;
	G_sprint( self, 2, "You have upgraded/fixed/reloaded sentrygun\n" );
	sg->s.v.ammo_shells = 144;
	sg->s.v.ammo_rockets = 20;
	return;
    }
    G_sprint( self, 2, "no sg to upgrade\n" );
}

void Eng_DispLoad(  )
{
    gedict_t *disp;
    float power;

    for ( disp = world; disp = find( disp, FOFS( s.v.classname ), "building_dispenser" ); )
    {
	disp->s.v.ammo_cells = disp->s.v.ammo_cells + 20;
	disp->s.v.ammo_rockets = disp->s.v.ammo_rockets + 15;
	if ( disp->s.v.ammo_rockets > 300 )
	    disp->s.v.ammo_rockets = 300;
	if ( disp->s.v.ammo_cells > 400 )
	    disp->s.v.ammo_cells = 400;
	self->hook_out = 0;
	power = ceil( 25 + disp->s.v.ammo_rockets * 1.5 + disp->s.v.ammo_cells );
	if ( power > 250 )
	    power = 250;
	G_bprint( 2, "maximum detdispenser damage - %.0f\n", power );
	return;
    }
    G_sprint( self, 2, "no disp\n" );
}

void Eng_DispUnload(  )
{
    gedict_t *disp;
    float power;

    for ( disp = world; disp = find( disp, FOFS( s.v.classname ), "building_dispenser" ); )
    {
	disp->s.v.ammo_cells = disp->s.v.ammo_cells - 20;
	disp->s.v.ammo_rockets = disp->s.v.ammo_rockets - 15;
	if ( disp->s.v.ammo_rockets < 0 )
	    disp->s.v.ammo_rockets = 0;
	if ( disp->s.v.ammo_cells < 0 )
	    disp->s.v.ammo_cells = 0;
	self->hook_out = 0;
	power = ceil( 25 + disp->s.v.ammo_rockets * 1.5 + disp->s.v.ammo_cells );
	if ( power > 250 )
	    power = 250;
	G_bprint( 2, "maximum detdispenser damage - %.0f\n", power );
	return;
    }
    G_sprint( self, 2, "no disp\n" );
}

void Eng_SGReload(  )
{
    gedict_t *sg;

    for ( sg = world; sg = find( sg, FOFS( s.v.classname ), "building_sentrygun" ); )
    {
	if ( sg->s.v.ammo_shells == sg->maxammo_shells && sg->s.v.ammo_rockets == sg->maxammo_rockets
	     && sg->s.v.health == sg->s.v.max_health )
	    continue;
	sg->s.v.health = sg->s.v.max_health;
	sg->s.v.ammo_shells = sg->maxammo_shells;
	sg->s.v.ammo_rockets = sg->maxammo_rockets;
	sound( sg, 3, "weapons/turrset.wav", 1, 1 );
	G_sprint( self, 2, "You have fixed/reloaded ONE sentrygun\n" );
	return;
    }
    G_sprint( self, 2, "no sg to reload\n" );

}


