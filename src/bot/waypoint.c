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
 *  $Id: waypoint.c,v 1.6 2005-06-07 04:12:39 AngelD Exp $
 */
#include "g_local.h"

typedef struct wp_list_s {
	struct wp_list_s *next, *prev;
	waypoint_t *wp;
} wp_list_t;

static wp_list_t *wp_list_start = NULL;
static int numwaypoints = 0;

waypoint_t *AddWaypoint( waypoint_t * newwp )
{

	waypoint_t *wp;
	wp_list_t *wp_l;

	wp = malloc( sizeof( waypoint_t ) );
	memcpy( wp, newwp, sizeof( waypoint_t ) );

	if ( wp->flags & WP_FL_TEMP )	//dont allow temp wp in wp list
		wp->flags -= WP_FL_TEMP;

	if ( wp->radius == 0 )
		wp->radius = 60;

	wp_l = malloc( sizeof( wp_list_t ) );

	wp_l->next = wp_list_start;
	wp_l->prev = NULL;
	wp_l->wp = wp;
	if ( wp_list_start )
		wp_list_start->prev = wp_l;
	wp_list_start = wp_l;
	memset( wp->links, 0, sizeof( wp->links ) );
//FIXME!!!! add check for dublicate indexes
	numwaypoints++;
	return wp;
}

waypoint_t *WaypointFindNearest( vec3_t point )
{
	float   minlen = 10e8;
	float   len;
	vec3_t  vtmp;
	wp_list_t *wp_l;
	waypoint_t *wp = NULL;

	for ( wp_l = wp_list_start; wp_l; wp_l = wp_l->next )
	{
		VectorSubtract( point, wp_l->wp->origin, vtmp );
		len = vlen( vtmp );
		if ( minlen > len )
		{
			minlen = len;
			wp = wp_l->wp;
		}
	}
	return wp;
}

waypoint_t *WaypointFindNearestVisible( vec3_t point, int nomonsters )
{

	float   minlen = 10e8;
	float   len;
	vec3_t  vtmp;
	wp_list_t *wp_l;
	waypoint_t *wp = NULL;

	for ( wp_l = wp_list_start; wp_l; wp_l = wp_l->next )
	{
		traceline( PASSVEC3( point ), PASSVEC3( wp_l->wp->origin ), nomonsters,  self );
//                TraceCapsule( PASSVEC3( self->s.v.origin ), PASSVEC3( wp_l->wp->origin ), 1, self, PASSVEC3( self->s.v.mins)+TRACECAP_ADD, PASSVEC3( self->s.v.maxs)-TRACECAP_ADD );
		if ( g_globalvars.trace_fraction != 1 )
			continue;

		VectorSubtract( point, wp_l->wp->origin, vtmp );
		len = vlen( vtmp );
		if ( minlen > len )
		{
			minlen = len;
			wp = wp_l->wp;
		}
	}
	return wp;
}
int WaypointFindVisible( vec3_t point, int nomonsters, waypoint_t ** wps, int maxcount )
{
	int     num = 0;
	wp_list_t *wp_l;

	if ( !wps )
		return 0;

	for ( wp_l = wp_list_start; wp_l && num < maxcount; wp_l = wp_l->next )
	{
		//traceline( PASSVEC3( point ), PASSVEC3( wp_l->wp->origin ), nomonsters, self );
                TraceCapsule( PASSVEC3( self->s.v.origin ), PASSVEC3( wp_l->wp->origin ), 1, self, PASSVEC3( self->s.v.mins)+TRACECAP_ADD, PASSVEC3( self->s.v.maxs)-TRACECAP_ADD );
		if ( g_globalvars.trace_fraction != 1 )
			continue;
		*wps++ = wp_l->wp;
		num++;
	}
	return num;
}

int WaypointFindByType( int flags, int teams, waypoint_t ** wps, int maxcount )
{
	int     num = 0;
	wp_list_t *wp_l;

	if ( !wps )
		return 0;

	for ( wp_l = wp_list_start; wp_l && num < maxcount; wp_l = wp_l->next )
	{
		if ( !( flags && ( wp_l->wp->flags & flags ) ) )
			continue;

		if ( !( teams && ( wp_l->wp->teams & teams ) ) )
			continue;

		*wps++ = wp_l->wp;
		num++;
	}
	return num;
}

int     WaypointFindInRadius( vec3_t vPos, float fRadius, waypoint_t** wps, int maxcount )
{
	int     num = 0;
	vec3_t  vtmp;
	wp_list_t *wp_l;

	if ( !wps )
		return 0;

	for ( wp_l = wp_list_start; wp_l && num < maxcount; wp_l = wp_l->next )
	{
		VectorSubtract( vPos, wp_l->wp->origin, vtmp );
		if( vlen( vtmp ) > fRadius )
		        continue;

		*wps++ = wp_l->wp;
		num++;
	}
	return num;
}

qboolean AddLink( int src_index, int dest_index, wp_link_t * newlink )
{
	wp_list_t *wp_l;
	waypoint_t *wp_src = NULL, *wp_dst = NULL;
	wp_link_t *link;
	int     index = 0;

	for ( wp_l = wp_list_start; wp_l; wp_l = wp_l->next )
	{
		if ( wp_l->wp->index == src_index )
			wp_src = wp_l->wp;

		if ( wp_l->wp->index == dest_index )
			wp_dst = wp_l->wp;
	}
	if ( !wp_src || !wp_dst )
		return false;

	for ( index = 0; index < MAX_WP_LINKS; index++ )
	{
		if ( !wp_src->links[index] )
			break;
	}
	if ( index == MAX_WP_LINKS )
	{
		G_dprintf( "AddLink: MAX_WP_LINKS in wp %d\n", src_index );
		return false;
	}
	link = malloc( sizeof( wp_link_t ) );
	memcpy( link, newlink, sizeof( wp_link_t ) );
	link->src_wp = wp_src;
	link->dest_wp = wp_dst;
	VectorSubtract( wp_src->origin, wp_dst->origin, link->dir );
	link->length = VectorNormalize( link->dir );
	vectoangles( link->dir, link->dirangles );

	wp_src->links[index] = link;
	return true;
}


#define MAX_PATH_LEN  100
typedef struct {
	waypoint_t *wp;
	int     link_index;
} tmp_path_t;

void FreePath( wp_path_t * path )
{
	wp_path_t *tmp;

	while ( path )
	{
		tmp = path->next;
		free( path );
		path = tmp;
	}
}
void PrintPath(wp_path_t * path)
{

        while( path )
        {
                G_dprintf("WP %3d = %3f %3f %3f\n", path->link->src_wp->index,PASSVEC3( path->link->src_wp->origin));
                path = path->next;
        }
}

wp_path_t *WaypointFindPathOld( waypoint_t * wp_src, waypoint_t * wp_dst )
{
	int     pathlen = 0, i;
	tmp_path_t tmppath[MAX_PATH_LEN];
	wp_path_t *path = NULL, *tmpp;

	if ( !wp_dst || !wp_src )
	{
		G_dprintf( "WaypointFindPath: NULL arguments\n" );
		return NULL;
	}
	if ( wp_src == wp_dst )
	{
		G_dprintf( "WaypointFindPath: wp_src == wp_dst\n" );
		return NULL;
	}

	tmppath[pathlen].wp = wp_src;
	tmppath[pathlen].link_index = -1;

//clear marked 
        {
            wp_list_t *wp_l;
            for ( wp_l = wp_list_start; wp_l ; wp_l = wp_l->next )
                wp_l->wp->marked = 0;
        }
//
	while ( 1 )
	{
		waypoint_t *curr_wp = tmppath[pathlen].wp;
		int     link_index = tmppath[pathlen].link_index;
                curr_wp->marked = 1;
		
		if ( link_index < 0 )
			for ( i = 0; i < MAX_WP_LINKS; i++ )
			{
				if ( !curr_wp->links[i] )
					continue;
				if ( curr_wp->links[i]->dest_wp == wp_dst )
				{
					tmppath[pathlen++].link_index = i;
					goto found;
				}
			}
		while ( ++link_index < MAX_WP_LINKS )
		{
			if ( !curr_wp->links[link_index] )
			        continue;
			if ( !curr_wp->links[link_index]->dest_wp->marked )
				break;
		}
		if ( link_index >= MAX_WP_LINKS )
		{
			if ( !( pathlen-- ) )
				return NULL;
			continue;
		}
		tmppath[pathlen].link_index = link_index;
		if ( ++pathlen >= MAX_PATH_LEN )
		{
			pathlen -= 2;
			continue;
		}

		tmppath[pathlen].wp = curr_wp->links[link_index]->dest_wp;
		tmppath[pathlen].link_index = -1;

	}
      found:

	path = malloc( sizeof( wp_path_t ) );
	path->link = tmppath[0].wp->links[tmppath[0].link_index];
	path->next = NULL;
	tmpp = path;
        G_dprintf( "WaypointFindPath: pathlen = %d\nWP  0 %3d = %3f %3f %3f\n", pathlen, tmpp->link->src_wp->index, PASSVEC3( tmpp->link->src_wp->origin));
	for ( i = 1; i < pathlen; i++ )
	{
		tmpp->next = malloc( sizeof( wp_path_t ) );
		tmpp = tmpp->next;
		tmpp->link = tmppath[i].wp->links[tmppath[i].link_index];
		tmpp->next = NULL;
                G_dprintf("WP %2d %3d = %3f %3f %3f\n", i,tmpp->link->src_wp->index,PASSVEC3( tmpp->link->src_wp->origin));
	}
        G_dprintf("WP %2d %3d = %3f %3f %3f\n", i,tmpp->link->dest_wp->index, PASSVEC3( tmpp->link->dest_wp->origin));
	return path;
}

wp_path_t *WaypointFindPath( waypoint_t * wp_src, waypoint_t * wp_dst )
{
	int     pathlen = 0, i;
	wp_path_t *path = NULL, *tmpp;
	wp_list_t *wp_l,*wp_l_end;

	if ( !wp_dst || !wp_src )
	{
		G_dprintf( "WaypointFindPath: NULL arguments\n" );
		return NULL;
	}
	if ( wp_src == wp_dst )
	{
		G_dprintf( "WaypointFindPath: wp_src == wp_dst\n" );
		return NULL;
	}


//clear marked 
        {
            for ( wp_l = wp_list_start; wp_l ; wp_l = wp_l->next )
            {
                wp_l->wp->marked = 0;
                wp_l->wp->parent = NULL;
            }
        }
//
        wp_l = malloc( sizeof( wp_list_t ));
        wp_l_end = wp_l;
        wp_l->next = NULL;
        wp_l->prev = NULL;
        wp_l->wp = wp_src;
        wp_src->marked = 1;

	while ( wp_l_end )
	{
		wp_list_t *wpl_tmp  = wp_l_end;
		waypoint_t *wp = wp_l_end->wp;
		wp_l_end = wpl_tmp->prev;
		if( wp_l_end )
		        wp_l_end->next = NULL;
		else
		{
		        wp_l = NULL;
		        wp_l_end = NULL;
		}
		
		free( wpl_tmp );

		if( wp == wp_dst )
		{
		        while(wp_l)
		        {
		                wpl_tmp = wp_l->next;
		                free(wp_l);
		                wp_l = wpl_tmp;
		        }
		        break;
		}

		for ( i = 0 ; i < MAX_WP_LINKS ; i++ )
		{
      			if ( !wp->links[i] )
      				continue;
      			if( wp->links[i]->dest_wp->marked )
      			        continue;

      			wp->links[i]->dest_wp->marked = 1;
      			wp->links[i]->dest_wp->parent = wp->links[i];
      			wpl_tmp = malloc( sizeof( wp_list_t ));
      			wpl_tmp->wp = wp->links[i]->dest_wp;
      			wpl_tmp->next = wp_l;
      			wpl_tmp->prev = NULL;
      			if( wp_l )
      			     wp_l->prev = wpl_tmp;
      			else
      			     wp_l_end = wpl_tmp;
      			wp_l = wpl_tmp;
		}

	}
	if( !wp_dst->parent )
	        return NULL;

	
	path = malloc( sizeof( wp_path_t ) );
	path->link = wp_dst->parent;
	path->next = NULL;
	while( path->link->src_wp != wp_src )
	{
	        tmpp = path;
	        path = malloc( sizeof( wp_path_t ) );
	        path->next = tmpp;
	        path->link = tmpp->link->src_wp->parent;
	}
	
	PrintPath(path);
	return path;
}

static int wpdrawed = 0;
void DrawWPS()
{
        gedict_t* te;
        wp_list_t *wp_l;

        if( wpdrawed )
        {
                for ( te = world; ( te = trap_find( te, FOFS( s.v.classname ) , "wp" )); )
                        dremove(te);
                wpdrawed = 0;
                return;
        }
        wpdrawed = 1;
 	for ( wp_l = wp_list_start; wp_l; wp_l = wp_l->next )
 	{
 	        te = spawn(  );
 	        te->s.v.takedamage = DAMAGE_NO;
          	te->s.v.movetype = MOVETYPE_NONE;
          	te->s.v.solid = SOLID_NOT;
          	te->s.v.classname = "wp";
          	setmodel( te, "progs/s_light.spr" );
          	setorigin( te, PASSVEC3( wp_l->wp->origin));
 	}

        
}
