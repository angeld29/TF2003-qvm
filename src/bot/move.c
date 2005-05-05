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
 *  $Id: move.c,v 1.2 2005-05-05 14:51:44 AngelD Exp $
 */
#include "g_local.h"

qboolean AtLocation( vec3_t vDestination )
{
	vec3_t  spot1;

	VectorSubtract( vDestination, self->s.v.origin, spot1 );

	return vlen( spot1 ) < 45;
}

void StrafeLeft()
{
	if ( self->obs_time < g_globalvars.time )
		self->keys |= KEY_MOVELEFT;
}

void StrafeRight()
{
	if ( self->obs_time < g_globalvars.time )
		self->keys |= KEY_MOVERIGHT;
}

void BotTouch()
{
	return;
}

void ClearAllWaypoints()
{
	SetVector( self->waypoint1, 0, 0, 0 );
	SetVector( self->waypoint2, 0, 0, 0 );
	SetVector( self->waypoint3, 0, 0, 0 );
	SetVector( self->waypoint4, 0, 0, 0 );
	SetVector( self->waypoint5, 0, 0, 0 );
	self->distanceToWaypoint = 5000;
	self->checkMovementTime = g_globalvars.time + 0.7;
}

void frik_obstructed( vec3_t whichway )
{

}

float angcomp( float y1, float y2 )
{
	float   answer;

	while ( y1 > 180 )
		y1 = y1 - 360;
	while ( y1 < -180 )
		y1 = y1 + 360;
	while ( y2 > 180 )
		y2 = y2 - 360;
	while ( y2 < -180 )
		y2 = y2 + 360;
	answer = y1 - y2;
	if ( fabs( answer ) >= 180 )
		answer = 360 - fabs( answer );
	return answer;
}

int GetKeysByDir( vec3_t sdir )
{
	vec3_t  keydir;
	vec3_t  lookhere;
	int     outkeys;
	float   tang;

	outkeys = 0;
	vectoangles( sdir, keydir );

	if ( !( self->action & ( BOT_FIGHTING | BOT_AVOIDING_HAZARD ) ) )
	{
		VectorCopy( keydir, lookhere );
		lookhere[0] = 0;
		VectorCopy( lookhere, self->s.v.v_angle );
	}
	if ( sdir[0] || sdir[1] )
	{
		tang = angcomp( keydir[1], self->s.v.v_angle[1] );
		if ( fabs( tang ) <= 80 )
			outkeys |= KEY_MOVEFORWARD;
		tang = angcomp( keydir[1], self->s.v.v_angle[1] - 90 );
		if ( fabs( tang ) <= 80 )
			outkeys |= KEY_MOVERIGHT;
		tang = angcomp( keydir[1], self->s.v.v_angle[1] + 90 );
		if ( fabs( tang ) <= 80 )
			outkeys |= KEY_MOVELEFT;
		tang = angcomp( keydir[1], self->s.v.v_angle[1] - 180 );
		if ( fabs( tang ) <= 80 )
			outkeys |= KEY_MOVEBACK;
	}
	if ( sdir[2] > 0.7 )
		outkeys |= KEY_MOVEUP;
	else
	{
		if ( sdir[2] < 0.7 )
			outkeys |= KEY_MOVEDOWN;
	}
	return outkeys;
}

int GetKeysByLoc( vec3_t vLocation )
{
	vec3_t  vDirection;

	VectorSubtract( vLocation, self->s.v.origin, vDirection );

	return GetKeysByDir( vDirection );
}

void GotoLocation( vec3_t vDestination )
{
	self->keys = GetKeysByLoc( vDestination );
}

void MoveToLocation( vec3_t vDestination )
{
	ClearAllWaypoints();
	if ( self->action != BOT_MOVING )
	{
		self->oldAction = self->action;
		self->action = BOT_MOVING;
	}
	VectorCopy( vDestination, self->waypoint1 );
}


void DoMovement()
{
	float   botDistanceToWaypoint;
	vec3_t   vtmp;

	if ( VectorCompareF( self->waypoint1, 0, 0, 0 ) )
	{
		if ( self->oldAction != BOT_MOVING )
			self->action = self->oldAction;
		else
			self->action = BOT_IDLE;

		self->oldAction = BOT_IDLE;
		if ( self->action == BOT_DEFEND )
			VectorCopy( self->botDirection, self->s.v.v_angle );
		self->obs_time = 0;
		self->keys = 0;
		return;
	}
	VectorSubtract( self->waypoint1, self->s.v.origin, vtmp );
	if ( vlen( vtmp ) < 50 )
	{
		if ( VectorCompareF( self->waypoint2, 0, 0, 0 ) )
			SetVector( self->waypoint1, 0, 0, 0 );
		else
		{
			VectorCopy( self->waypoint2, self->waypoint1 );
			VectorCopy( self->waypoint3, self->waypoint2 );
			VectorCopy( self->waypoint4, self->waypoint3 );
			VectorCopy( self->waypoint5, self->waypoint4 );
			SetVector( self->waypoint5, 0, 0, 0 );
		}
		self->distanceToWaypoint = 5000;
		self->checkMovementTime = g_globalvars.time + 0.7;
	}
	if ( self->checkMovementTime < g_globalvars.time )
	{
		VectorSubtract( self->waypoint1, self->s.v.origin, vtmp );
		botDistanceToWaypoint = vlen( vtmp );
		if ( self->distanceToWaypoint < botDistanceToWaypoint )
		{
			ClearAllWaypoints();
			self->keys = 0;
			self->action = BOT_IDLE;
			return;
		} else
		{
			self->checkMovementTime = g_globalvars.time + 0.7;
			self->distanceToWaypoint = botDistanceToWaypoint;
		}
	}
	if ( !VectorCompareF( self->waypoint1, 0, 0, 0 ) )
		self->keys = GetKeysByLoc( self->waypoint1 );
}
