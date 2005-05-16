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
 *  $Id: waypoint.c,v 1.1 2005-05-16 09:35:46 AngelD Exp $
 */
#include "g_local.h"

waypoint_t waypoints[MAX_WAYPOINTS];
static int     numwaypoints = 0;
waypoint_t* AddWaypoint(waypoint_t* wp)
{
        waypoint_t* w;
        w = &waypoints[numwaypoints++];
        *w = *wp;
        return w;
}

waypoint_t* WaypointFindNearest(vec3_t point)
{
        int i,index = -1;
        float minlen = 10e8;
        float len;
        vec3_t vtmp;

        for ( i = 0 ; i < numwaypoints ; i++ )
        {
                VectorSubtract(point, waypoints[i].origin,vtmp);
                len  = vlen(vtmp);
                if( minlen > len )
                {
                        minlen = len;
                        index = i;
                }
        }
        if( index == -1 )
                return NULL;
        return waypoints + index;
}

waypoint_t* WaypointFindNearestVisible(vec3_t point)
{
        int i,index = -1;
        float minlen = 10e8;
        float len;
        vec3_t vtmp;

        for ( i = 0 ; i < numwaypoints ; i++ )
        {
                traceline( PASSVEC3( point ), PASSVEC3( waypoints[i].origin ), 0, self );
                if( g_globalvars.trace_fraction != 1 ) 
                        continue;
                VectorSubtract(point, waypoints[i].origin,vtmp);
                len  = vlen(vtmp);
                if( minlen > len )
                {
                        minlen = len;
                        index = i;
                }
        }
        if( index == -1 )
                return NULL;
        return waypoints + index;
}

int WaypointFindInRadius( vec3_t vPos, float fRadius, int* pTab, int maxcount)
{
        return 0;
}

path_t temp_path[1024];
int     WaypointFindPath( vec3_t srcpoint, vec3_t destpoint, path_t* path, int maxpathlen )
{
    return 0;
}
