/*
 *  QWProgs-TF2003
 *  Copyright (C) 2004  [sd] angel
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
 *  $Id: g_mod_command.c,v 1.9 2006-11-25 18:54:32 AngelD Exp $
 */

#include "g_local.h"
typedef struct mod_s{
    char   *command;
    void    ( *Action ) ( int );
} mod_t;
void WP_command( int argc );
void Red_Def_command( int argc );

void   TF_Set_Cmd( int argc  );
const static mod_t   mod_cmds[] = {
    {"wp", WP_command},
    {"red_def", Red_Def_command},
    {"set", TF_Set_Cmd },
    {NULL, NULL}
};



void ModCommand()
{
    char    cmd_command[1024];
    int argc;
    const mod_t  *ucmd;

    argc = trap_CmdArgc();
    if( argc < 2 )
        return;


    trap_CmdArgv( 1, cmd_command, sizeof( cmd_command ) );

    for ( ucmd = mod_cmds; ucmd->command; ucmd++ )
    {
        if ( strcmp( cmd_command, ucmd->command ) )
            continue;
        ucmd->Action( argc );
        return;
    }
    return;

}
extern vec3_t end_pos;

void DrawWPS();
void ClearWaypoints(  );
static void cmd_addLink( int argc, qboolean dual )
{
    char    cmd_command[100];
    wp_link_t link;
    int i1,i2;
    if( argc < 5 )
        return;

    memset(&link, 0 ,sizeof(link));
    trap_CmdArgv( 3, cmd_command, sizeof( cmd_command ) );
    i1 = atoi( cmd_command );
    trap_CmdArgv( 4, cmd_command, sizeof( cmd_command ) );
    i2 = atoi( cmd_command );
    link.teams = 15;
    if( argc > 5 )
    {
        trap_CmdArgv( 5, cmd_command, sizeof( cmd_command ) );
        link.flags = atoi( cmd_command );
    }
    if( argc > 6 )
    {
        trap_CmdArgv( 6, cmd_command, sizeof( cmd_command ) );
        link.teams = atoi( cmd_command );
    }

    if( argc > 7 )
    {
        trap_CmdArgv( 7, cmd_command, sizeof( cmd_command ) );
        link.req_velocity = atof( cmd_command );

    }
    AddLink( i1, i2, &link);
    if( dual )
        AddLink( i2, i1, &link);

}

void WP_command( int argc )
{
    char    cmd_command[100];
    if( argc < 3)
        return;

    trap_CmdArgv( 2, cmd_command, sizeof( cmd_command ) );

    if(!strcmp(cmd_command, "add"))
    {
        waypoint_t wp;

        if( argc < 7 )
            return;

        memset(&wp, 0 ,sizeof(wp));
        trap_CmdArgv( 3, cmd_command, sizeof( cmd_command ) );
        wp.index = atoi( cmd_command );
        trap_CmdArgv( 4, cmd_command, sizeof( cmd_command ) );
        wp.origin[0] = atof( cmd_command );
        trap_CmdArgv( 5, cmd_command, sizeof( cmd_command ) );
        wp.origin[1] = atof( cmd_command );
        trap_CmdArgv( 6, cmd_command, sizeof( cmd_command ) );
        wp.origin[2] = atof( cmd_command );
        wp.teams = 15 ;// (1<<0)+(1<<1)+(1<<2)+(1<<3);
        if( argc > 7 )
        {
            trap_CmdArgv( 7, cmd_command, sizeof( cmd_command ) );
            wp.flags = atoi( cmd_command );

        }
        if( argc > 8 )
        {
            trap_CmdArgv( 8, cmd_command, sizeof( cmd_command ) );
            wp.teams = atoi( cmd_command );
        }

        if( argc > 9 )
        {
            trap_CmdArgv( 9, cmd_command, sizeof( cmd_command ) );
            wp.radius = atof( cmd_command );

        }
        AddWaypoint(&wp);
        return;
    }

    if(!strcmp(cmd_command, "link"))
    {
        cmd_addLink(argc, false);
        return;
    }

    if(!strcmp(cmd_command, "dlink"))
    {
        cmd_addLink(argc, true);
        return;
    }

    if(!strcmp(cmd_command, "target"))
    {
        if( argc < 6 )
            return;

        trap_CmdArgv( 3, cmd_command, sizeof( cmd_command ) );
        end_pos[0] = atof( cmd_command );
        trap_CmdArgv( 4, cmd_command, sizeof( cmd_command ) );
        end_pos[1] = atof( cmd_command );
        trap_CmdArgv( 5, cmd_command, sizeof( cmd_command ) );
        end_pos[2] = atof( cmd_command );
        return;
    }
    if(!strcmp(cmd_command, "draw"))
    {
        DrawWPS();
        return;
    }
    if(!strcmp(cmd_command, "clear"))
    {
        ClearWaypoints();
        return;
    }
}

static gedict_t* getTeam2Flag()
{
    gedict_t *Goal,*te;
    Goal = trap_find( world, FOFS( s.v.classname ), "info_tfdetect" );

    if ( !Goal )
        return NULL;
    if ( !Goal->display_item_status1 )
        return NULL;
    te = Finditem( Goal->display_item_status1 );

    if( !te || te == world || te->team_no != 2 )
    {
        te = Finditem( Goal->display_item_status2 );
    }
    if( !te || te == world || te->team_no != 2 )
    {
        te = Finditem( Goal->display_item_status3 );
    }
    if( !te || te == world || te->team_no != 2 )
    {
        te = Finditem( Goal->display_item_status4 );
    }

    if ( te == world || te->team_no != 2 )
    {
        if( self != world )
            G_sprint(self,2,"Item is missing.\n" );
        return NULL;
    }
    return Goal;
}
void Red_Def_command( int argc )
{
    char    cmd_command[1024];
    static  qboolean red_def_mode =  false;
    gedict_t *Goal,*saveself;
    gedict_t *te,*owner;

    if( tfset(mtfl) )
    {
        if( self != world )
            G_sprint(self,2,"Not allowed for mtfl.\n" );
        return;
    }
    if( argc < 3)
    {
        if( self == world )
            return;
        if( red_def_mode )
            G_sprint(self, 2, "Red-Def mode is ON\n");
        else
            G_sprint(self, 2, "Red-Def mode is OFF\n");
        return;
    }
    saveself = self;
    trap_CmdArgv( 2, cmd_command, sizeof( cmd_command ) );
    if( !strcmp(cmd_command,"on") )
    {
        if( red_def_mode )
        {
            if( self != world )
                G_sprint(self, 2, "Red-Def mode already activated\n");
            return;
        }

        if( (Goal = getTeam2Flag()))
        {
            owner = PROG_TO_EDICT(te->s.v.owner);
            if( owner != world )
            {
                tfgoalitem_RemoveFromPlayer( te, owner, 1 );
            }
            te->take_sshot = 1;
            red_def_mode = true;
            G_bprint(3, "Red-DEF mode activated\n");
        }

        return;
    }
    if( !strcmp(cmd_command,"off") )
    {
        if( !red_def_mode )
        {
            if( self != world )
                G_sprint(self,3,"Red-Def mode not active\n");
            return;
        }
        if( (Goal = getTeam2Flag()))
        {
            te->take_sshot = 0;
            red_def_mode = false;
            G_bprint(3, "Red-DEF mode deactivated\n");
        }
        return;
    }

}
