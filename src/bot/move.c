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
 *  $Id: move.c,v 1.11 2005-06-07 04:12:39 AngelD Exp $
 */
#include "g_local.h"

void SelectWP();
#define DEFAULT_MAX_YAW_PER_SEK 250
//#define SPEED_REPORT
//#define MAX_YAW_PER_SEK 1800
//#define MAX_STRAFE_ANGLE 30 //MAX abs(dir-vel)
//#define STRAFE_ANGLE_DELTA 5.0
//#define MAX_STRAFE_ANGLE_AT_START 60
//#define SPEED_COEFFICIENT 1.4
#define CHANGE_WP_DISTANCE 50

#ifdef  SPEED_REPORT
static float last_report = 0;
#endif
static float maxspeed = 0;

const wp_link_t default_link = { NULL, NULL, 0, 0, 0};
float max_yaw_per_sek = DEFAULT_MAX_YAW_PER_SEK;
float bot_frametime;
static float max_yaw_pf;
static vec3_t dir2move, curr_vel, cur_angle;
static float vel_yaw, dir_yaw, view_yaw; 
static float velocity,sv_accelerate;


qboolean AtLocation( vec3_t vDestination )
{
	vec3_t  spot1;

	VectorSubtract( vDestination, self->s.v.origin, spot1 );

	return vlen( spot1 ) < 45;
}

void ClearAllWaypoints()
{

        FreePath( self->wp_path );

        if( self->end_wp && (self->end_wp->flags & WP_FL_TEMP ) )
                free( self->end_wp );

        if( self->wp && (self->wp->flags & WP_FL_TEMP ) )
                free( (void*)self->wp );
                
        self->end_wp = NULL;
        self->wp = NULL;
        self->wp_link = NULL;
        self->wp_path = NULL;
	self->checkMovementTime = g_globalvars.time + 0.7;
}

/*float angcomp( float y1, float y2 )
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
}*/



float   subangle( float y1, float y2 )
{
        float o;
        
        o = anglemod(y1) - anglemod(y2);

/*	if( o > 180)
	        o -=360;
	if( o < -180)
	        o +=360;*/
	o = anglemod( o );
	if( o > 180 )
	        o -= 360;
	return o;

}

void InitCalcMovement()
{
        vec3_t vtmp;

        max_yaw_pf = max_yaw_per_sek * bot_frametime;

	VectorCopy( self->s.v.velocity, curr_vel );
	vectoangles( curr_vel, vtmp );
	vel_yaw = vtmp[1];
	curr_vel[2] = 0;
	velocity = vlen( self->s.v.velocity );
	velocity = vlen( curr_vel );

	VectorSubtract( self->wp->origin, self->s.v.origin, dir2move );

	dir2move[2] = 0;
	vectoangles( dir2move, vtmp );

	dir_yaw = vtmp[1];

	VectorCopy( self->s.v.v_angle, cur_angle);

	cur_angle[2] = 0;

	view_yaw = cur_angle[1];

	sv_accelerate = trap_cvar("sv_accelerate");
}
int counter = 0;

void ChangeBotYaw( float wish_yaw, float max_delta_yaw )
{
        float ftmp= subangle( wish_yaw, view_yaw);
        if( fabs(ftmp) < max_yaw_pf )
                self->s.v.v_angle[1] = wish_yaw;
        else
        {
                if( ftmp < 0 )
                        self->s.v.v_angle[1] -= max_delta_yaw;
                else
                        self->s.v.v_angle[1] += max_delta_yaw;
        }
}

int WPChange()
{
        int dist;
	if( self->s.v.fixangle )
	{
	        self->botNoMoveTime = g_globalvars.time + BOT_TIMEWAIT_AFTER_TELEPORT;
	        ClearAllWaypoints(  );
	        SelectWP();
	        return 0;
	}

        //TraceCapsule( PASSVEC3( self->s.v.origin ), PASSVEC3( self->wp->origin ), 1, self,PASSVEC3( self->s.v.mins)+TRACECAP_ADD, PASSVEC3( self->s.v.maxs)-TRACECAP_ADD );
        traceline( PASSVEC3( self->s.v.origin ), PASSVEC3( self->wp->origin ), 1, self);

        if( g_globalvars.trace_fraction != 1 )
        {
                ClearAllWaypoints(  );
                SelectWP();
                return 0;
        }

        InitCalcMovement();
/*        if( self->wp_path ) // попробовать срезать путь( для случаев когда ближайший вп сзади )
        {
                int flags = self->wp_path->link->flags;
                if( !flags || ( flags & WPLINK_FL_WALK) )
                if( abs( subangle(dir_yaw, self->wp_path->link->dirangles[1] ) ) >= 160 )
                {
                      TraceCapsule( PASSVEC3( self->s.v.origin ), PASSVEC3( self->wp_path->link->dest_wp->origin ), 1, self,
                               PASSVEC3( self->s.v.mins)+TRACECAP_ADD, PASSVEC3( self->s.v.maxs)-TRACECAP_ADD );
                      if( g_globalvars.trace_fraction == 1 )  
                      {
                               wp_path_t *path = self->wp_path;
           	               self->wp = path->link->dest_wp;
            	               self->wp_link = path->link;
                    	       self->wp_path = path->next;
                    	       free(path);
                    	       InitCalcMovement();
                               self->time_for_wpchange = g_globalvars.time + bot_frametime * 10;
	                       return 2;
                      }
                }
        }*/
        dist = vlen( dir2move );
                
	if ( velocity && (( dist / velocity) < 0.1) )
	{
	        wp_path_t *path = self->wp_path;
	        if( !path )
	        {
	                if( self->end_wp )
	                {
	                       if( dist >  self->wp->radius ) 
	                       {
 	                         TraceCapsule( PASSVEC3( self->s.v.origin ), PASSVEC3( self->end_wp->origin ), 1, self,
	                               PASSVEC3( self->s.v.mins)+TRACECAP_ADD, PASSVEC3( self->s.v.maxs)-TRACECAP_ADD );
                                 if( g_globalvars.trace_fraction != 1 )  
                                      return 1;
	                       }
                               self->wp_link = &default_link;
                               self->wp = self->end_wp;
                               self->end_wp = NULL;
	                }else
	                {
         	                if( dist < self->wp->radius )
         	                {
         	                   ClearAllWaypoints();
         			   self->keys = 0;
         			   return 0;
         	                }
	                }
	        }else
	        {
	                if( dist >  self->wp->radius )
	                {
	                       //traceline( PASSVEC3( self->s.v.origin ), PASSVEC3( path->link->dest_wp->origin ), 0, self );
 	                       TraceCapsule( PASSVEC3( self->s.v.origin ), PASSVEC3( path->link->dest_wp->origin ), 1, self,
	                               PASSVEC3( self->s.v.mins)+TRACECAP_ADD, PASSVEC3( self->s.v.maxs)-TRACECAP_ADD );
                                if( g_globalvars.trace_fraction != 1 )  return 1;
	                }

       	                self->wp = path->link->dest_wp;
       	                self->wp_link = path->link;
              	        self->wp_path = path->next;
              	        free(path);
	        }
	        InitCalcMovement();
	        self->time_for_wpchange = g_globalvars.time + bot_frametime * 10;
	        return 2;
	}
	return 1;
}

void DoMovement()
{
        float ftmp;
        float max_strafe_angle;
        vec3_t v;

        self->keys = 0;
        if( self->checkMovementTime > g_globalvars.time )
                return;
	if ( !self->wp )
	{
		self->keys = 0;
		return;
	}
	if( self->s.v.health <=0  || self->s.v.deadflag )
	{
	        maxspeed = 0;
		self->keys = 0;
	        return;
	}

	if( g_globalvars.time < self->botNoMoveTime )
	{
	        return;
	}

	if(! WPChange())
	        return;

	if ( ( int ) ( self->s.v.flags ) & ( FL_INWATER ) )
	{
	        self->keys |= KEY_MOVEFORWARD;
	        self->s.v.button2 = 0;
	        self->s.v.v_angle[1] = dir_yaw;
	        return;
	}
        if ( ( int ) ( self->s.v.flags ) & ( FL_ONGROUND |FL_PARTIALGROUND ) )
	{
	        if( velocity < self->maxfbspeed * 0.95 || (self->wp_link && (self->wp_link->flags & WPLINK_FL_WALK)) )
	        {
	                
	                if( fabs(dir_yaw - vel_yaw) > 5 )
	                {
	                       self->s.v.v_angle[1] = dir_yaw;
	                }
         	        self->keys |= KEY_MOVEFORWARD;
         	        self->s.v.button2 = 0;
	        }else
	        {
	                if( !self->old_button2 )
                	        self->s.v.button2 = 1;
	                else
                	        self->s.v.button2 = 0;
	                        
	                if( self->bot_strafe_state == BOT_STRAFE_LEFT)
	                {
	                        self->keys |= KEY_MOVELEFT;
	                }else
	                {
	                        self->keys |= KEY_MOVERIGHT;
	                }
	        }
                return;
	}

	//Air Move
	self->s.v.button2 = 0;


	ftmp = atan2( 30, velocity) * 180 / M_PI; // angle per frame

	if( ftmp > max_yaw_pf )
	        ftmp = max_yaw_pf;

	max_strafe_angle = (ftmp / 5 ) / bot_frametime;
	if(  max_strafe_angle > 60 )
	        max_strafe_angle = 60;

#ifdef SPEED_REPORT
	if( vlen(curr_vel) - maxspeed > 30 ) 
	{
	       G_bprint(3,"maxspeed = %3.0f %3.0f\n", 
	       vlen(curr_vel), (vlen(curr_vel) - maxspeed)/ (g_globalvars.time -last_report ));
	       maxspeed = vlen(curr_vel);
	       last_report = g_globalvars.time;
	}
#endif

/*	if( (int)velocity < 300 )
	        max_strafe_angle = 80;
	else
	        if( (int)velocity < 400 )
	                max_strafe_angle = 60;
	        else
      	                max_strafe_angle = 30;*/

	VectorScale(self->s.v.velocity, bot_frametime * 3, v);
	VectorAdd(self->s.v.origin, v,v);
        TraceCapsule( PASSVEC3( self->s.v.origin ), PASSVEC3( v ), 1, self,
                           PASSVEC3( self->s.v.mins)+TRACECAP_ADD, PASSVEC3( self->s.v.maxs)-TRACECAP_ADD );

      	ftmp =  subangle( dir_yaw,vel_yaw );
	if( abs( ftmp) >  max_strafe_angle  || g_globalvars.trace_fraction != 1  || self->time_for_wpchange > g_globalvars.time )
	{
	        if( abs( ftmp ) < 1 )
	                self->time_for_wpchange = 0;
	                
	        if( ftmp < 0 )
	        {
	                self->bot_strafe_state = BOT_STRAFE_RIGHT;
	                self->keys |= KEY_MOVERIGHT;
	                
	        }else
	        {
	                self->keys |= KEY_MOVELEFT;
	                self->bot_strafe_state = BOT_STRAFE_LEFT;
	        }
	        self->s.v.v_angle[1] = vel_yaw;
                return;
	}
        TraceCapsule( PASSVEC3( self->wp->origin ), PASSVEC3( v ), 1, self,
                           PASSVEC3( self->s.v.mins)+TRACECAP_ADD, PASSVEC3( self->s.v.maxs)-TRACECAP_ADD );
        if( g_globalvars.trace_fraction != 1 )
        {
	        if( ftmp < 0 )
	        {
	                self->bot_strafe_state = BOT_STRAFE_RIGHT;
	        }else
	        {
	                self->bot_strafe_state = BOT_STRAFE_LEFT;
	        }
        }

        if(self->bot_strafe_state == BOT_STRAFE_RIGHT)
        {
                self->keys |= KEY_MOVERIGHT;
                
        }else
        {
                self->keys |= KEY_MOVELEFT;
        }
        ChangeBotYaw(vel_yaw,max_yaw_pf);
}
