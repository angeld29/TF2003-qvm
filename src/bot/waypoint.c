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
 *  $Id: waypoint.c,v 1.5 2005-06-05 05:10:41 AngelD Exp $
 */
#include "g_local.h"

typedef struct wp_list_s
{
        struct wp_list_s        *next,*prev;
        waypoint_t              *wp;
}wp_list_t;

static wp_list_t* wp_list_start = NULL;
static int     numwaypoints = 0;

waypoint_t* AddWaypoint(waypoint_t* newwp)
{

        waypoint_t      *wp;
        wp_list_t       *wp_l;

        wp = malloc( sizeof(waypoint_t) );
        memcpy( wp, newwp, sizeof(waypoint_t) );

        if( wp->flags & WP_FL_TEMP )      //dont allow temp wp in wp list
                wp->flags -= WP_FL_TEMP;

        if( wp->radius == 0 )
                wp->radius = 60;

        wp_l = malloc( sizeof(wp_list_t) );

        wp_l->next = wp_list_start;
        wp_l->prev = NULL;
        wp_l->wp = wp;
        if( wp_list_start )
                wp_list_start->prev = wp_l;
        wp_list_start = wp_l;
        memset( wp->links, 0, sizeof(wp->links));
//FIXME!!!! add check for dublicate indexes
        numwaypoints++;
        return wp;
}

waypoint_t* WaypointFindNearest(vec3_t point)
{
        float minlen = 10e8;
        float len;
        vec3_t vtmp;
        wp_list_t       *wp_l;
        waypoint_t      *wp = NULL;

//        G_dprintf("WaypointFindNearest: numwps = %d\n, WP %3f %3f %3f\n",numwaypoints, PASSVEC3( point));
        for ( wp_l =  wp_list_start; wp_l ; wp_l = wp_l->next )
        {
                VectorSubtract( point, wp_l->wp->origin,vtmp);
                len  = vlen(vtmp);
//                G_dprintf("len = %3f, minlen = %3f WP %3f %3f %3f\n", len, minlen, PASSVEC3( wp_l->wp->origin));
                if( minlen > len )
                {
                        minlen = len;
                        wp = wp_l->wp;
                }
        }
        return wp;
}

waypoint_t* WaypointFindNearestVisible(vec3_t point, int nomonsters )
{

        float minlen = 10e8;
        float len;
        vec3_t vtmp;
        wp_list_t       *wp_l;
        waypoint_t      *wp = NULL;

        for ( wp_l =  wp_list_start; wp_l ; wp_l = wp_l->next )
        {
                traceline( PASSVEC3( point ), PASSVEC3( wp_l->wp->origin ), nomonsters, self );
                if( g_globalvars.trace_fraction != 1 ) 
                        continue;

                VectorSubtract( point, wp_l->wp->origin,vtmp);
                len  = vlen(vtmp);
                if( minlen > len )
                {
                        minlen = len;
                        wp = wp_l->wp;
                }
        }
        return wp;
}
int             WaypointFindVisible(vec3_t point, int nomonsters, waypoint_t** wps, int maxcount )
{
        int num = 0;
        wp_list_t       *wp_l;

        if( !wps )
                return 0;

        for ( wp_l =  wp_list_start; wp_l && num < maxcount ; wp_l = wp_l->next )
        {
                traceline( PASSVEC3( point ), PASSVEC3( wp_l->wp->origin ), nomonsters, self );
                if( g_globalvars.trace_fraction != 1 ) 
                        continue;
                *wps++ = wp_l->wp;
                num++;
        }
        return num;
}

qboolean        AddLink( int src_index, int dest_index, wp_link_t*newlink)
{
        wp_list_t       *wp_l;
        waypoint_t      *wp_src = NULL,*wp_dst = NULL;
        wp_link_t       *link;
        int             index = 0;
        vec3_t vtmp;

        for ( wp_l =  wp_list_start; wp_l ; wp_l = wp_l->next )
        {
                if( wp_l->wp->index == src_index )
                        wp_src = wp_l->wp;

                if( wp_l->wp->index == dest_index )
                        wp_dst = wp_l->wp;
        }
        if( !wp_src || !wp_dst )
                return false;

        for ( index = 0 ; index < MAX_WP_LINKS ; index++ )
        {
                if( !wp_src->links[index])
                        break;
        }
        if( index == MAX_WP_LINKS )
        {
                G_dprintf("AddLink: MAX_WP_LINKS in wp %d\n",src_index );
                return false;
        }
        link = malloc( sizeof(wp_link_t));
        memcpy( link, newlink, sizeof(wp_link_t) );
        link->src_wp = wp_src;
        link->dest_wp = wp_dst;
        VectorSubtract( wp_src->origin, wp_dst->origin,vtmp);
        link->length = vlen(vtmp);

        wp_src->links[index] = link;
        return true;
}

int WaypointFindInRadius( vec3_t vPos, float fRadius, int* pTab, int maxcount)
{
        return 0;
}

#define MAX_PATH_LEN  100
typedef struct
{
        waypoint_t      *wp;
        int             link_index;
}tmp_path_t;

void FreePath(wp_path_t *path)
{
        wp_path_t *tmp;
        while( path )
        {
                tmp = path->next;
                free(path);
                path = tmp;
        }
}

wp_path_t*      WaypointFindPath( waypoint_t*wp_src, waypoint_t* wp_dst )
{
        int             pathlen = 0,i;
        tmp_path_t      tmppath[MAX_PATH_LEN];
        wp_path_t       *path = NULL,*tmpp;

        if( !wp_dst || !wp_src)
        {
                G_dprintf("WaypointFindPath: NULL arguments\n");
                return NULL;
        }
        if( wp_src == wp_dst )
        {
                G_dprintf("WaypointFindPath: wp_src == wp_dst\n");
                return NULL;
        }

        tmppath[pathlen].wp = wp_src;
        tmppath[pathlen].link_index = -1;

        while( 1 ) 
        {
                waypoint_t* curr_wp = tmppath[pathlen].wp;
                int link_index = tmppath[pathlen].link_index;
                

                if( link_index < 0 )
                 for ( i = 0 ; i < MAX_WP_LINKS ; i++ )
                 {
                        if( !curr_wp->links[i])
                                continue;
                        if( curr_wp->links[i]->dest_wp == wp_dst )
                        {
                                tmppath[pathlen++].link_index = i;
                                goto found;
                        }
                 }
                while( ++link_index < MAX_WP_LINKS )
                {
                        if( curr_wp->links[link_index] )
                                break;
                }
                if( link_index >= MAX_WP_LINKS)
                {
                        if( !(pathlen--) )
                                return NULL;
                        continue;
                }
                tmppath[pathlen].link_index = link_index;
                if( ++pathlen >= MAX_PATH_LEN )
                {
                        pathlen-=2;
                        continue;
                }
                
                tmppath[pathlen].wp = curr_wp->links[link_index]->dest_wp;
                tmppath[pathlen].link_index = -1;

        }
found:

        path = malloc( sizeof( wp_path_t ));
        path->link = tmppath[0].wp->links[tmppath[0].link_index];
        path->next = NULL;
        tmpp = path;
//        G_bprint(2,"WaypointFindPath: pathlen = %d\nWP  0 = %3f %3f %3f\n", pathlen, PASSVEC3( tmpp->link->src_wp->origin));
        for ( i = 1 ; i < pathlen ; i++ )
        {
                tmpp->next = malloc( sizeof( wp_path_t ));
                tmpp = tmpp->next;
                tmpp->link = tmppath[i].wp->links[tmppath[i].link_index];
                tmpp->next = NULL;
//                G_bprint(2,"WP %2d = %3f %3f %3f\n", i,PASSVEC3( tmpp->link->src_wp->origin));
        }
//        G_bprint(2,"WP %2d = %3f %3f %3f\n", i, PASSVEC3( tmpp->link->dest_wp->origin));
        return path;
}

