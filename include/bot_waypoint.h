/*
 *  QWProgs-TF2003
 *  Copyright (C) 2004  [sd] angel
 *
 *
 *  This program is free software you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 *  $Id: bot_waypoint.h,v 1.4 2005-06-07 04:12:37 AngelD Exp $
 */

#define MAX_WP_LINKS 20
//#define MAX_WAYPOINTS 10000
#define WPLINK_FL_WALK               1
#define WPLINK_FL_JUMP               2
#define WPLINK_FL_LIFT               4
#define WPLINK_FL_TELEPORT           8

#define WP_FL_TEMP              1
#define WP_FL_CAP               2
#define WP_FL_RESUPLY           4
#define WP_FL_FIGHT             8

#define TRACECAP_ADD 15 

struct waypoint_s;

typedef struct wp_link_s
{
        struct waypoint_s       *src_wp,*dest_wp;
        int                     flags;
        int                     teams;
        float                   length;
        vec3_t                  dir,    // normalized
                                dirangles; 
        float                   req_velocity;
}wp_link_t;

typedef struct waypoint_s
{
        vec3_t                  origin;
        float                   radius;
        int                     index;
        int                     flags;
        int                     teams;
        wp_link_t*              links[MAX_WP_LINKS];
        // for search
        int                     marked; 
        wp_link_t*              parent;
}waypoint_t;

typedef struct wp_path_s
{
        wp_link_t               *link;
        struct wp_path_s        *next;
}wp_path_t;
extern const wp_link_t default_link;

int             WaypointFindInRadius( vec3_t vPos, float fRadius, waypoint_t** wps, int maxcount );
int             WaypointFindVisible( vec3_t point, int nomonsters, waypoint_t** wps, int maxcount );
int             WaypointFindByType( int flags, int teams, waypoint_t** wps, int maxcount );
waypoint_t*     WaypointFindNearestVisible(vec3_t point, int nomonsters );
waypoint_t*     WaypointFindNearest(vec3_t point);
waypoint_t*     AddWaypoint(waypoint_t* );
qboolean        AddLink( int src_index, int dest_index, wp_link_t* newlink);
void            FreePath(wp_path_t *path);

wp_path_t*      WaypointFindPath( waypoint_t*, waypoint_t* );
