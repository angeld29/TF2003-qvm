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
 *  $Id: move.c,v 1.3 2005-05-09 00:33:02 AngelD Exp $
 */
#include "g_local.h"

#define MAX_YAW_PER_SEK 180
#define MAX_STRAFE_ANGLE 30 //MAX abs(dir-vel)
#define MAX_STRAFE_ANGLE_AT_START 60
#define SPEED_COEFFICIENT 1.4
#define CHANGE_WP_DISTANCE 50

float max_yaw_pf;
vec3_t dir2move, curr_vel, cur_angle;
float vel_yaw, dir_yaw, view_yaw; 
float velocity,sv_accelerate;



qboolean AtLocation( vec3_t vDestination )
{
	vec3_t  spot1;

	VectorSubtract( vDestination, self->s.v.origin, spot1 );

	return vlen( spot1 ) < 45;
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



float   subangle( float y1, float y2 )
{
        float o;
        
        o = anglemod(y1) -anglemod(y2);

/*	if( o > 180)
	        o -=360;
	if( o < -180)
	        o +=360;*/
	return anglemod( o );

}

void InitCalcMovement()
{
        vec3_t vtmp;

        max_yaw_pf = MAX_YAW_PER_SEK * g_globalvars.frametime;

	VectorCopy( self->s.v.velocity, curr_vel );
	vectoangles( curr_vel, vtmp );
	vel_yaw = vtmp[1];
	curr_vel[2] = 0;
	velocity = vlen( curr_vel );

	VectorSubtract( self->waypoint1, self->s.v.origin, dir2move );

	dir2move[2] = 0;
	vectoangles( dir2move, vtmp );

	dir_yaw = vtmp[1];

/*	{
	        vec3_t end;
	        normalize( dir2move, vtmp );
	        VectorScale(vtmp, 120, vtmp);
	        VectorAdd(self->s.v.origin,vtmp,end);
	        TraceCapsule( PASSVEC3( self->s.v.origin ), PASSVEC3( end ), 0, self 
	        , PASSVEC3( self->s.v.mins), PASSVEC3( self->s.v.maxs));
	        if( g_globalvars.trace_ent)
                dir_yaw += (g_random()<0.5)?90:-90;
	}*/

	VectorCopy( self->s.v.v_angle, cur_angle);

	cur_angle[2] = 0;

	view_yaw = cur_angle[1];

	sv_accelerate = trap_cvar("sv_accelerate");
}
int counter = 0;

void ChangeBotVAngle(float pitch, float yaw, float roll)
{

        float ftmp = angcomp( yaw, view_yaw);

        self->s.v.v_angle[0] = pitch;
        self->s.v.v_angle[2] = roll;
        self->s.v.v_angle[1] = yaw;
        return;

        if( abs(ftmp) < max_yaw_pf )
        {
                self->s.v.v_angle[1] = yaw;
                return;
        }else
        {
                if( ftmp < 0 )
                        self->s.v.v_angle[1] -= max_yaw_pf;
                else
                        self->s.v.v_angle[1] += max_yaw_pf;
        }
}
void DoMovement()
{
        float maxaccel, ideal_angle , ftmp;
        float max_strafe_angle ,range;

	if ( VectorCompareF( self->waypoint1, 0, 0, 0 ) )
	{
		self->keys = 0;
		return;
	}

	VectorSubtract( self->waypoint1, self->s.v.origin, dir2move );

	range = vlen(self->s.v.velocity) * g_globalvars.frametime ;
	if( range < CHANGE_WP_DISTANCE )
	        range  = CHANGE_WP_DISTANCE;

	if ( vlen( dir2move ) < range )
	{
		if ( VectorCompareF( self->waypoint2, 0, 0, 0 ) )
		{
			SetVector( self->waypoint1, 0, 0, 0 );
			self->keys = 0;
			return;
		} else
		{
			VectorCopy( self->waypoint2, self->waypoint1 );
			VectorCopy( self->waypoint3, self->waypoint2 );
			VectorCopy( self->waypoint4, self->waypoint3 );
			VectorCopy( self->waypoint5, self->waypoint4 );
			SetVector( self->waypoint5, 0, 0, 0 );
		}
	}

	if( self->s.v.fixangle )
	        self->botNoMoveTime = g_globalvars.time + BOT_TIMEWAIT_AFTER_TELEPORT;

	if( g_globalvars.time < self->botNoMoveTime )
	{
	        return;
	}
	        
	InitCalcMovement();

        if ( ( int ) ( self->s.v.flags ) & FL_ONGROUND )
	{
	        if( velocity < self->maxfbspeed)
	        {
         	        self->keys |= KEY_MOVEFORWARD;
         	        self->keys |= KEY_MOVELEFT;
         	        self->s.v.button2 = 0;
	        }else
	        {
               	        self->s.v.button2 = 1;
	                if( self->bot_strafe_state == BOT_STRAFE_LEFT)
	                {
	                        self->keys |= KEY_MOVELEFT;

                         //      self->bot_strafe_state = BOT_STRAFE_RIGHT;
	                }else
	                {
	                        self->keys |= KEY_MOVERIGHT;
       	                 //      self->bot_strafe_state = BOT_STRAFE_LEFT;
	                }
	        }
                return;
	}
	//Air Move
	self->s.v.button2 = 0;
	ftmp =  subangle( dir_yaw,vel_yaw );

//	maxaccel = sv_accelerate * self->maxfbspeed * g_globalvars.frametime;
//	ideal_angle = acos ( (self->maxfbspeed - maxaccel) /velocity ) * 180.0 / M_PI;

/*	if( counter++ > 10  )
	{
	        //G_bprint(2,"%3.0f %3.0f %3.0f %3.0f %3.0f %3.0f\n",
	        //dir_yaw,vel_yaw,ftmp, velocity,ideal_angle,self->s.v.v_angle[1]);
	        counter =0;
	
	}*/


	//max_strafe_angle = ( velocity < self->maxfbspeed *SPEED_COEFFICIENT)? MAX_STRAFE_ANGLE_AT_START:MAX_STRAFE_ANGLE;

	max_strafe_angle = 80 - 40 * (velocity /(float) self->maxfbspeed - 1.0);

	if( max_strafe_angle < 30 ) max_strafe_angle = 30;

	if( (ftmp  > max_strafe_angle) && (ftmp < 360 - max_strafe_angle) )
	{//change vel direction
	        if( ftmp > 180)
	        {
	                self->bot_strafe_state = BOT_STRAFE_RIGHT;
	                self->keys |= KEY_MOVERIGHT;
	        }else
	        {
	                self->keys |= KEY_MOVELEFT;
	                self->bot_strafe_state = BOT_STRAFE_LEFT;
	        }
	        ChangeBotVAngle(0,vel_yaw,0);
	        return;
	}

	//ftmp =  g_globalvars.frametime * 0.001;

/*        ftmp = vel_yaw - self->s.v.v_angle[1];
        if( abs(ftmp) > max_yaw_pf )
        {
                if( ftmp > 0)
                        self->s.v.v_angle[1] += max_yaw_pf;
                else
                        self->s.v.v_angle[1] -= max_yaw_pf;
        }else*/

        ftmp = asin( 1/ 2 *velocity)* 180.0 / M_PI;
        //ftmp = 0;
        if(self->bot_strafe_state == BOT_STRAFE_RIGHT)
        {
                ChangeBotVAngle(0, vel_yaw+ftmp, 0);
        }else
        {
                ChangeBotVAngle(0, vel_yaw-ftmp, 0);
        }
        

        if( self->bot_strafe_state == BOT_STRAFE_LEFT)
        {
/*                self->s.v.v_angle[0] = 0;
                self->s.v.v_angle[1] = vel_yaw + ftmp;
                self->s.v.v_angle[2] = 0;*/

                self->keys |= KEY_MOVELEFT;
        }else
        {
/*                self->s.v.v_angle[0] = 0;
                self->s.v.v_angle[1] = vel_yaw - ftmp;
                self->s.v.v_angle[2] = 0;*/
                self->keys |= KEY_MOVERIGHT;
        }
}
