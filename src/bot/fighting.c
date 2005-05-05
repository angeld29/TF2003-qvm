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
 *  $Id: fighting.c,v 1.2 2005-05-05 14:51:44 AngelD Exp $
 */
#include "g_local.h"

void ReloadWeapons(  )
{
	if ( ( self->tfstate & TFSTATE_GRENPRIMED ) || ( self->tfstate & TFSTATE_GRENTHROWING ) )
		return;

	if ( ( self->weapons_carried & WEAP_ROCKET_LAUNCHER ) && self->reload_rocket_launcher != 0 )
	{
		self->s.v.impulse = ( self->current_weapon == WEAP_ROCKET_LAUNCHER ) ? TF_RELOAD : 7;
		return;
	}
	if ( ( self->weapons_carried & WEAP_GRENADE_LAUNCHER ) && self->reload_grenade_launcher != 0 )
	{
		self->s.v.impulse = ( self->current_weapon == WEAP_GRENADE_LAUNCHER ) ? TF_RELOAD : 6;

		return;
	}
	if ( ( self->weapons_carried & WEAP_SUPER_SHOTGUN ) && self->reload_super_shotgun != 0 )
	{
		self->s.v.impulse = ( self->current_weapon == WEAP_SUPER_SHOTGUN ) ? TF_RELOAD : 3;

		return;
	}
	if ( ( self->weapons_carried & WEAP_SHOTGUN ) && self->reload_shotgun != 0 )
	{
		self->s.v.impulse = ( self->current_weapon == WEAP_SHOTGUN ) ? TF_RELOAD : 2;
	}
}

qboolean AllTargetsFull(  )
{
	if ( !self->target3 )
		return false;
	else
		return true;
}

void AddTarget( gedict_t * targetToAdd )
{
	if ( self->target1 == targetToAdd || self->target2 == targetToAdd || self->target3 == targetToAdd )
		return;
	if ( !self->target1 )
	{
		self->target1 = targetToAdd;
	} else
	{
		if ( !self->target2 )
			self->target2 = targetToAdd;
		else
		{
			if ( !self->target3 )
				self->target3;
			else
				return;
		}
	}
	sound( self, 1, "weapons/turrspot.wav", 1, 1 );
	self->action = BOT_FIGHTING;
	VectorCopy( targetToAdd->s.v.origin, targetToAdd->lastSighted );
}

void ClearTarget( int targetToClear )
{
	switch ( targetToClear )
	{

	default:
		self->target1 = self->target2;

	case 2:
		self->target2 = self->target3;
	case 3:
		self->target3 = NULL;
		break;
	}
	if ( !self->target1 )
	{
		self->action = BOT_IDLE;
		self->s.v.button0 = 0;
		self->s.v.button2 = 0;
		self->keys = 0;
		self->s.v.impulse = TF_GRENADE_T;
	}
}

void ClearAllTargets(  )
{
	self->target1 = self->target2 = self->target3 = NULL;
	self->s.v.button0 = 0;
	self->s.v.button2 = 0;
}

void PickBestWeapon(  )
{
	float   targetdistance;
	vec3_t  vtemp;
	gedict_t *eGren = NULL;
	gedict_t *eTemp;

	if ( self->tfstate & TFSTATE_GRENPRIMED )
	{
         	for ( eTemp = world; ( eTemp = trap_find( eTemp, FOFS( s.v.classname ), "primer" ) ); )
         	{
         		if ( eTemp->s.v.owner == EDICT_TO_PROG( self ) )
         			eGren = eTemp;

         	}
         	if ( eGren && eGren->heat < g_globalvars.time + 0.5 )
         	{
         		return;
         	}
	}
	switch ( self->playerclass )
	{
	case PC_SCOUT:
		if ( self->s.v.ammo_nails > 0 )
		{
			self->s.v.impulse = 4;
			return;
		}
		if ( self->s.v.ammo_shells > 0 )
		{
			self->s.v.impulse = 2;
			return;
		}
		break;
	case PC_SNIPER:
		if ( self->s.v.ammo_shells > 0 )
		{
			self->s.v.impulse = 3;
			return;
		}
		if ( self->s.v.ammo_nails > 0 )
		{
			self->s.v.impulse = 4;
			return;
		}
		break;
	case PC_SOLDIER:

		if ( !self->target1 )
		{
			self->s.v.impulse = 7;
			return;
		}
		VectorSubtract( self->target1->s.v.origin, self->s.v.origin, vtemp );
		targetdistance = vlen( vtemp );
		if ( targetdistance < 92 )
		{
			if ( self->s.v.ammo_shells > 1 )
			{
				self->s.v.impulse = 3;
				return;
			}
		}
		if ( targetdistance > 600 )
		{
			if ( self->s.v.ammo_shells > 1 && self->reload_shotgun < 7 )
			{
				self->s.v.impulse = 3;
				return;
			}
		}
		if ( self->target1->s.v.health < 30 )
		{
			if ( self->s.v.ammo_shells > 1 )
			{
				if ( targetdistance > 400 )
					self->s.v.impulse = 2;
				else
					self->s.v.impulse = 3;
				return;
			}
		}
		if ( self->s.v.ammo_rockets > 0 )
		{
			self->s.v.impulse = 7;
//                      self->current_weapon = WEAP_ROCKET_LAUNCHER;
			return;
		}
		if ( self->s.v.ammo_shells > 1 )
		{
			self->s.v.impulse = 3;
			return;
		}
		if ( self->s.v.ammo_shells > 0 )
		{
			self->s.v.impulse = 2;
			return;
		}
		break;
	case PC_DEMOMAN:

		if ( self->s.v.ammo_rockets > 0 )
		{
			self->s.v.impulse = 6;
			return;
		}
		if ( self->s.v.ammo_shells > 0 )
		{
			self->s.v.impulse = 2;
			return;
		}
		break;
	case PC_MEDIC:
		if ( !self->target1 )
		{
			self->s.v.impulse = 3;
			return;
		}
		VectorSubtract( self->s.v.origin, self->target1->s.v.origin, vtemp );
		if ( vlen( vtemp ) < 64
		     && !( self->target1->tfstate & TFSTATE_INFECTED ) && self->target1->playerclass != 5 )
		{
			if ( self->current_weapon != 2 )
				self->s.v.impulse = 1;
			return;
		}
		if ( self->s.v.ammo_nails > 0 )
		{
			self->s.v.impulse = 5;
			return;
		}
		if ( self->s.v.ammo_shells > 0 )
		{
			self->s.v.impulse = 3;
			return;
		}
		break;
	case PC_HVYWEAP:

//		if ( !self->target1 )
		{
			self->s.v.impulse = 3;
			return;
		}

		if ( self->s.v.ammo_shells > 0 )
		{
		
			if ( self->target1->playerclass == PC_SNIPER )
			{
				self->s.v.impulse = 3;
				return;
			} else
			{
				if ( self->s.v.ammo_cells > 3 )
				{
					self->s.v.impulse = 7;
					return;
				} else
				{
					if ( self->s.v.ammo_shells > 1 )
					{
						self->s.v.impulse = 3;
						return;
					} else
					{
						self->s.v.impulse = 2;
						return;
					}
				}
			}
		}
		break;
	case PC_PYRO:
		if ( !self->target1 )
		{
			self->s.v.impulse = 7;
			return;
		}
		VectorSubtract( self->s.v.origin, self->target1->s.v.origin, vtemp );
		if ( vlen( vtemp ) > 275 )
		{
			if ( self->s.v.ammo_rockets > 0 )
			{
				if ( self->current_weapon != WEAP_INCENDIARY )
					self->s.v.impulse = 7;
				return;
			} else
			{
				self->s.v.impulse = 2;
				return;
			}
		}
		if ( self->s.v.ammo_cells > 0 )
		{
			self->s.v.impulse = 6;
			return;
		}
		if ( self->s.v.ammo_shells > 0 )
		{
			self->s.v.impulse = 2;
			return;
		}
		break;
	case PC_SPY:
		if ( !self->target1 )
		{
			self->s.v.impulse = 3;
			return;
		}
		VectorSubtract( self->s.v.origin, self->target1->s.v.origin, vtemp );
		if ( vlen( vtemp ) < 64 )
		{
			self->s.v.impulse = 1;
			return;
		}
		if ( self->s.v.ammo_shells > 0 && !( self->target1->tfstate & TFSTATE_TRANQUILISED ) )
		{
			self->s.v.impulse = 2;
			return;
		}
		if ( self->s.v.ammo_shells > 10 )
		{
			self->s.v.impulse = 3;
			return;
		}
		if ( self->s.v.ammo_nails > 0 )
		{
			self->s.v.impulse = 4;
			return;
		}
		break;
	case PC_ENGINEER:
		if ( !self->target1 )
		{
			self->s.v.impulse = 3;
			return;
		}
		VectorSubtract( self->s.v.origin, self->target1->s.v.origin, vtemp );
		if ( self->reload_super_shotgun < 14 && self->s.v.ammo_shells > 0 && vlen( vtemp ) < 90 )
		{
			self->s.v.impulse = 3;
			return;
		}
		if ( self->s.v.ammo_nails > 0 )
		{
			self->s.v.impulse = 2;
			return;
		}
		break;
	default:
		break;
	}
}

void FindTargets(  )
{
	gedict_t *possibleTarget;
	vec3_t  vBotPointOfView;
	vec3_t  vTargetBodyPart;

	if ( self->s.v.solid != 3 )
		return;
	if ( AllTargetsFull(  ) != 0 )
		return;
/*	if ( self->s.v.origin[1] < -600 || self->s.v.origin[1] > 250 )
	{

	        ClearAllTargets();
		self->action = BOT_IDLE;
		return;
		//fixme!!!!!!
	}*/

	for ( possibleTarget = world;
	      possibleTarget = trap_find( possibleTarget, FOFS( s.v.classname ), "player" ); )

	{
		if ( possibleTarget->has_disconnected )
			continue;
		if ( !possibleTarget->team_no )
			continue;
		if ( !possibleTarget->playerclass )
			continue;
		if ( possibleTarget->team_no == self->team_no )
			continue;
		if ( !( possibleTarget->s.v.origin[1] > -600 && possibleTarget->s.v.origin[1] < 250 ) )
			continue;
		if ( possibleTarget->s.v.health <= 0 )
			continue;


		traceline( PASSVEC3( self->s.v.origin ),
			   PASSVEC3( possibleTarget->s.v.origin ), false, world );
		if ( g_globalvars.trace_fraction == 1 )
		{
			AddTarget( possibleTarget );
			return;
		}
		VectorCopy( self->s.v.origin, vBotPointOfView );
		vBotPointOfView[2] += 16;
		VectorCopy( possibleTarget->s.v.origin, vTargetBodyPart );
		vTargetBodyPart[2] -= 16;
		traceline( PASSVEC3( vBotPointOfView ), PASSVEC3( vTargetBodyPart ), false, world );
		if ( g_globalvars.trace_fraction == 1 )
		{
			AddTarget( possibleTarget );
			return;
		}
		VectorCopy( self->s.v.origin, vBotPointOfView );
		vBotPointOfView[2] += 20;
		VectorCopy( possibleTarget->s.v.origin, vTargetBodyPart );
		vTargetBodyPart[2] += 16;
		traceline( PASSVEC3( vBotPointOfView ), PASSVEC3( vTargetBodyPart ), false, world );
		if ( g_globalvars.trace_fraction == 1 )
		{
			AddTarget( possibleTarget );
			return;
		}
	}
	for ( possibleTarget = world;
	      possibleTarget = trap_find( possibleTarget, FOFS( s.v.classname ), "building_sentrygun" ); )
	{
		if ( possibleTarget->team_no != self->team_no )
		{
			traceline( PASSVEC3( self->s.v.origin ), PASSVEC3( possibleTarget->s.v.origin ),
				   false, world );
			if ( g_globalvars.trace_fraction == 1 )
			{
				if ( possibleTarget->s.v.health > 0 )
				{
					AddTarget( possibleTarget );
					return;
				}
			}
		}
	}
}

void AimHere( vec3_t aimAtThisPoint )
{
	vec3_t  directionToTarget, vtemp;


	VectorSubtract( aimAtThisPoint, self->s.v.origin, vtemp );

	vectoangles( vtemp, directionToTarget );
	if ( directionToTarget[0] != 0 )
	{
		if ( directionToTarget[0] > 0 )
		{
			if ( directionToTarget[0] < 90 )
				directionToTarget[0] = -directionToTarget[0];
		}
		if ( directionToTarget[0] > 270 )
		{
			if ( directionToTarget[0] < 360 )
				directionToTarget[0] = fabs( directionToTarget[0] - 360 );
		}
	}
	self->s.v.v_angle[0] = directionToTarget[0];
	self->s.v.v_angle[1] = directionToTarget[1];
	self->s.v.v_angle[2] = 0;
}

void InstantAim(  )
{
	AimHere( self->target1->s.v.origin );
	traceline( PASSVEC3( self->s.v.origin ), PASSVEC3( self->target1->s.v.origin ), false, world );
	if ( g_globalvars.trace_fraction == 1 )
		self->s.v.button0 = 1;
	else
		self->s.v.button0 = 0;
}

float GetCrosshairAngle( gedict_t * aTarget )
{
	vec3_t  directionToTarget, vtemp;
	float   fTemp;
	float   enemyCrosshairAngle;

	VectorSubtract( self->s.v.origin, aTarget->s.v.origin, vtemp )
	vectoangles( vtemp, directionToTarget );
	fTemp = aTarget->s.v.v_angle[1];
	if ( fTemp < 0 )
		fTemp = fTemp + 360;
	enemyCrosshairAngle = directionToTarget[1] - fTemp;
	if ( enemyCrosshairAngle > 180 )
		enemyCrosshairAngle = enemyCrosshairAngle - 360;
	if ( enemyCrosshairAngle < -180 )
		enemyCrosshairAngle = enemyCrosshairAngle + 360;
	return enemyCrosshairAngle;
}

void LeadAim( float fDistance )
{
	float   fTime;
	float   fMissileSpeed;
	vec3_t  vTarget;

	switch ( self->current_weapon )
	{
	case WEAP_NAILGUN:
	case WEAP_SUPER_NAILGUN:
		fMissileSpeed = 1000;
		break;
	case WEAP_GRENADE_LAUNCHER:
	case WEAP_INCENDIARY:
		fMissileSpeed = 600;
		break;
	case WEAP_TRANQ:
	case WEAP_LASER:
		fMissileSpeed = 1500;
		break;
	case WEAP_ROCKET_LAUNCHER:
		fMissileSpeed = 900;
		break;
	default:
		InstantAim(  );
		return;
	}
	fTime = fDistance / fMissileSpeed;
	if ( ( int ) ( self->target1->s.v.flags ) & FL_ONGROUND )
	{
		fTime = fTime * ( 0.4 + g_random(  ) * 0.4 );
		VectorScale( self->target1->s.v.velocity, fTime, vTarget );
		VectorAdd( vTarget, self->target1->s.v.origin, vTarget );
	} else
	{
		fTime = fTime * 0.9;
		VectorScale( self->target1->s.v.velocity, fTime, vTarget );
		VectorAdd( self->target1->s.v.origin, vTarget, vTarget );
		vTarget[2] -= sv_gravity * fTime * fTime;
	}
	if ( ( self->current_weapon & ( WEAP_ROCKET_LAUNCHER + WEAP_INCENDIARY ) )
	     && ( ( int ) ( self->target1->s.v.flags ) & FL_ONGROUND ) )
	{
		vTarget[2] = vTarget[2] - 45;
		AimHere( vTarget );
		traceline( PASSVEC3( self->s.v.origin ), PASSVEC3( vTarget ) + 45, false, world );
		if ( g_globalvars.trace_fraction == 1 )
			self->s.v.button0 = 1;
		else
			self->s.v.button0 = 0;
		return;
	}
	AimHere( vTarget );
	traceline( PASSVEC3( self->s.v.origin ), PASSVEC3( vTarget ) + 45, false, world );
	if ( g_globalvars.trace_fraction == 1 )
		self->s.v.button0 = 1;
	else
		self->s.v.button0 = 0;
}


void DodgeEnemy( float fDistance )
{
	float   fEnemyAim;
	float   fDodgeAngle;

	fEnemyAim = GetCrosshairAngle( self->target1 );
	if ( fDistance > 700 )
		fDistance = 700;
	if ( fDistance > 200 )
		fDodgeAngle = 10 + ( 700 - fDistance ) * 20 / 700;
	else
		fDodgeAngle = 30 + ( 200 - fDistance ) * 60 / 200;
	if ( fEnemyAim >= 0 && fEnemyAim < fDodgeAngle )
		StrafeRight(  );
	if ( fEnemyAim <= 0 && fEnemyAim >= fDodgeAngle * ( -1 ) )
		StrafeLeft(  );
}

void DodgeEnemyGrenades(  )
{
	gedict_t *possibleGrenade;
	vec3_t  vtemp;

	for ( possibleGrenade = world;
	      ( possibleGrenade = findradius( possibleGrenade, self->s.v.origin, 225 ) ); )
	{
		if ( streq( possibleGrenade->s.v.classname, "grenade" ) )
		{
			if ( self->team_no != PROG_TO_EDICT( possibleGrenade->s.v.owner )->team_no
			     || PROG_TO_EDICT( possibleGrenade->s.v.owner ) == self )
			{
				if ( possibleGrenade->s.v.touch == ( func_t ) NailGrenadeTouch )
				{
					if ( ( int ) ( self->s.v.flags ) & FL_ONGROUND )
						self->s.v.button2 = 1;
					else
						self->s.v.button2 = 0;
				}
				self->action = BOT_AVOIDING_HAZARD;
				VectorSubtract( self->s.v.origin, possibleGrenade->s.v.origin, vtemp );
				self->keys = GetKeysByDir( vtemp );
				return;
			}
		}
	}
	if ( self->action == BOT_AVOIDING_HAZARD )
	{
		self->keys = 0;
		self->s.v.button1 = 0;
		if ( self->target1 )
			self->action = BOT_FIGHTING;
		else
		{
			if ( !VectorCompareF( self->waypoint1, 0, 0, 0 ) )
				self->action = BOT_MOVING;
			else
				self->action = BOT_IDLE;
		}
	}
}

void HandleGrens(  )
{
	float   fTime;
	float   fDistance;
	gedict_t *eGren = NULL;
	gedict_t *eTemp;
	vec3_t  vtemp;



	if ( ( self->tfstate & TFSTATE_GRENPRIMED ) || ( self->tfstate & TFSTATE_GRENTHROWING ) )
	{
		for ( eTemp = world; eTemp = trap_find( eTemp, FOFS( s.v.classname ), "primer" ); )
		{
			if ( PROG_TO_EDICT( eTemp->s.v.owner ) == self )
				eGren = eTemp;
		}
		if ( !eGren )
			return;
		VectorSubtract( self->target1->s.v.origin, self->s.v.origin, vtemp );
		fDistance = vlen( vtemp );
		fTime = fDistance / 600;
		if ( eGren->heat <= g_globalvars.time + fTime + 0.6 )
		{
			if ( eGren->heat <= g_globalvars.time + fTime + 0.4 )
			{
				if ( fDistance > 600 )
					self->s.v.button2 = 1;
				self->s.v.button0 = 0;
			} else
			{
				if ( self->current_weapon & ( WEAP_INCENDIARY + WEAP_SUPER_NAILGUN ) )
					self->s.v.button0 = 0;
			}
		}
		if ( eGren->heat <= g_globalvars.time + fTime || eGren->heat < g_globalvars.time + 0.4 )
		{
			if ( fDistance > 500 )
			{
				if ( fDistance < 600 )
					self->s.v.v_angle[0] -= 20;
				else
				{
					if ( fDistance < 100 )
						self->s.v.v_angle[0] -= 30;
					else
						self->s.v.v_angle[0] -= 10;
				}
			}
			self->s.v.button0 = 0;
			self->s.v.impulse = TF_GRENADE_T;
		}
	} else
	{
		if ( self->no_grenades_1 > 0 )
			self->s.v.impulse = TF_GRENADE_1;
	}
}

void DitchGrens(  )
{
	if ( ( self->tfstate & TFSTATE_GRENPRIMED ) || ( self->tfstate & TFSTATE_GRENTHROWING ) )
		self->s.v.impulse = TF_GRENADE_T;
}

void UseGren(  )
{
	float   fTime;
	float   fDistance;

	gedict_t *eGren = NULL;
	gedict_t *eTemp;

	vec3_t  vtemp;

	if ( ( self->tfstate & TFSTATE_GRENPRIMED ) || ( self->tfstate & TFSTATE_GRENTHROWING ) )
	{
		for ( eTemp = world; eTemp = trap_find( eTemp, FOFS( s.v.classname ), "primer" ); )
		{
			if ( PROG_TO_EDICT( eTemp->s.v.owner ) == self )
				eGren = eTemp;
		}
		if ( !eGren )
			return;
		VectorSubtract( self->target1->s.v.origin, self->s.v.origin, vtemp );
		fDistance = vlen( vtemp );

		fTime = fDistance / 600;
		if ( eGren->heat < g_globalvars.time + 0.5 )
			fTime = 0;
		if ( eGren->heat <= g_globalvars.time + fTime + 0.6 )
		{
			if ( eGren->heat <= g_globalvars.time + fTime + 0.4 )
			{
				if ( fDistance > 600 )
					self->s.v.button2 = 1;
				self->s.v.button0 = 0;
			} else
			{
				if ( self->current_weapon & ( WEAP_INCENDIARY + WEAP_SUPER_NAILGUN ) )
					self->s.v.button0 = 0;
			}
		}
		if ( eGren->heat <= g_globalvars.time + fTime )
		{
			if ( fDistance < 100 )
				self->s.v.v_angle[0] -= 30;
			if ( fDistance > 500 )
			{
				if ( fDistance < 600 )
					self->s.v.v_angle[0] -= 20;
				else
					self->s.v.v_angle[0] -= 10;
			}
			self->s.v.button0 = 0;
			self->s.v.impulse = TF_GRENADE_T;
		}
	}
}

void AttackVisible(  )
{
	float   fDistance;
	vec3_t  vtemp;
	gedict_t *eGren = NULL;
//	gedict_t *eTemp;

/*	if ( self->tfstate & TFSTATE_GRENPRIMED )
	{
         	for ( eTemp = world; ( eTemp = trap_find( eTemp, FOFS( s.v.classname ), "primer" ) ); )
         	{
         		if ( eTemp->s.v.owner == EDICT_TO_PROG( self ) )
         			eGren = eTemp;

         	}
         	if ( eGren && eGren->heat < g_globalvars.time + 0.5 )
         	{
         	        UseGren(  );
         		return;
         	}
	}*/
	PickBestWeapon(  );
	if ( self->current_weapon & ( WEAP_BIOWEAPON + WEAP_AXE ) )
	{
		VectorSubtract( self->s.v.origin, self->target1->s.v.origin, vtemp );
		fDistance = vlen( vtemp );
		LeadAim( fDistance );
		GotoLocation( self->target1->s.v.origin );
		DodgeEnemy( fDistance );
		self->s.v.button0 = 1;
		UseGren(  );
	}
	if ( self->current_weapon & ( WEAP_ASSAULT_CANNON + WEAP_FLAMETHROWER ) )
	{

		VectorSubtract( self->s.v.origin, self->target1->s.v.origin, vtemp );
		fDistance = vlen( vtemp );

		LeadAim( fDistance );
		GotoLocation( self->target1->s.v.origin );
		DodgeEnemy( fDistance );
		UseGren(  );
		return;
	}
	if ( self->current_weapon & ( WEAP_LASER + WEAP_ROCKET_LAUNCHER + WEAP_GRENADE_LAUNCHER ) )
	{
		VectorSubtract( self->s.v.origin, self->target1->s.v.origin, vtemp );
		fDistance = vlen( vtemp );
		LeadAim( fDistance );
		if ( fDistance > 400 )
			self->keys = KEY_MOVEFORWARD;
		else
		{
			if ( fDistance < 200 )
				self->keys = KEY_MOVEBACK;
			else
			{
				if ( g_random(  ) < 0.5 )
					self->keys = KEY_MOVEFORWARD;
				else
					self->keys = KEY_MOVEBACK;
			}
		}
		DodgeEnemy( fDistance );
		if ( ( int ) ( self->s.v.flags ) & FL_ONGROUND )
		{

			if ( vlen( self->s.v.velocity ) > 0.8 * self->maxfbspeed )
			{
				if ( g_random(  ) < 0.05 )
					self->s.v.button2 = 1;
			} else
				self->s.v.button2 = 0;
		}
		HandleGrens(  );
		return;
	} else
	{
		VectorSubtract( self->s.v.origin, self->target1->s.v.origin, vtemp );
		fDistance = vlen( vtemp );
		LeadAim( fDistance );
		GotoLocation( self->target1->s.v.origin );
		DodgeEnemy( fDistance );
		if ( fDistance > 200 )
		{
			if ( ( int ) ( self->s.v.flags ) & FL_ONGROUND )
			{
				if ( vlen( self->s.v.velocity ) > 0.8 * self->maxfbspeed )
				{
					if ( g_random(  ) < 0.05 )
						self->s.v.button2 = 1;
				} else
					self->s.v.button2 = 0;
			}
		}
		HandleGrens(  );
		return;
	}
}


void AttackTargets(  )
{
	gedict_t *tempTarget;
	vec3_t  vtemp, vtemp2;

	if ( !self->target1 )
		return;
	if ( self->target1->s.v.health <= 0 || self->target1->has_disconnected == 1
	     || ( self->target1->team_no == 1 && self->target1->s.v.origin[1] < -700 )
	     || ( self->target1->team_no == 2 && self->target1->s.v.origin[1] > 350 ) )
	{
		ClearTarget( 1 );
		return;
	}
	traceline( PASSVEC3( self->s.v.origin ), PASSVEC3( self->target1->s.v.origin ), false, world );
	if ( g_globalvars.trace_fraction == 1 )
	{
		VectorCopy( self->target1->s.v.origin, self->target1->lastSighted );
		AttackVisible(  );
		return;
	}
	if ( self->target2 )
	{
		traceline( PASSVEC3( self->s.v.origin ), PASSVEC3( self->target2->s.v.origin ), false,
			   world );
		if ( g_globalvars.trace_fraction == 1 )
		{

			VectorCopy( self->target2->s.v.origin, self->target2->lastSighted );
			tempTarget = self->target1;
			self->target1 = self->target2;
			self->target2 = tempTarget;
			AttackVisible(  );
			return;
		}
	}
	if ( self->target3 )
	{
		traceline( PASSVEC3( self->s.v.origin ), PASSVEC3( self->target3->s.v.origin ), false,
			   world );
		if ( g_globalvars.trace_fraction == 1 )
		{
			VectorCopy( self->target3->s.v.origin, self->target3->lastSighted );
			tempTarget = self->target1;
			self->target1 = self->target3;
			self->target3 = tempTarget;
			AttackVisible(  );
			return;
		}
	}
	self->s.v.button0 = 0;

	VectorSubtract( self->target1->lastSighted, self->target1->s.v.origin, vtemp );
	if ( vlen( vtemp ) > 500 )
		ClearTarget( 1 );
	else
	{
		VectorSubtract( self->target1->lastSighted, self->s.v.origin, vtemp );

		if ( vlen( vtemp ) > 20 )
			GotoLocation( self->target1->lastSighted );
		else
		{
			VectorSubtract( self->s.v.origin, self->target1->s.v.origin, vtemp );
			if ( vlen( vtemp ) > 500 )
				ClearTarget( 1 );
			else
				GotoLocation( self->target1->s.v.origin );
		}
	}

	if ( self->target2 )
	{

		VectorSubtract( self->target2->lastSighted, self->target2->s.v.origin, vtemp );
		VectorSubtract( self->s.v.origin, self->target2->s.v.origin, vtemp2 );
		if ( self->target2 || vlen( vtemp ) > 500 || vlen( vtemp2 ) > 500
		     || self->target2->s.v.health <= 0 )
			ClearTarget( 2 );
	}
	if ( self->target3 )
	{
		VectorSubtract( self->target3->lastSighted, self->target3->s.v.origin, vtemp );
		VectorSubtract( self->s.v.origin, self->target3->s.v.origin, vtemp2 );
		if ( self->target3 || vlen( vtemp ) > 500 || vlen( vtemp2 ) > 500
		     || self->target3->s.v.health <= 0 )
			ClearTarget( 3 );
	}
}
