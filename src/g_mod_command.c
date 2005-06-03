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
 *  $Id: g_mod_command.c,v 1.1 2005-06-03 08:50:33 AngelD Exp $
 */

#include "g_local.h"

void WP_command( int argc );
void ModCommand()
{
        char    cmd_command[1024];
        int argc;
        argc = trap_CmdArgc();
        if( argc < 2 )
                return;

        trap_CmdArgv( 1, cmd_command, sizeof( cmd_command ) );
        if( !strcmp(cmd_command,"wp") )
                WP_command( argc );

}
void WP_command( int argc )
{
        char    cmd_command[1024];
        if( argc < 3)
                return;
        
        trap_CmdArgv( 2, cmd_command, sizeof( cmd_command ) );

        if(!strcmp(cmd_command, "add"))
        {
                waypoint_t wp;

                if( argc < 7 )
                        return;

                trap_CmdArgv( 3, cmd_command, sizeof( cmd_command ) );
                wp.index = atoi( cmd_command );
                trap_CmdArgv( 4, cmd_command, sizeof( cmd_command ) );
                wp.origin[0] = atof( cmd_command );
                trap_CmdArgv( 5, cmd_command, sizeof( cmd_command ) );
                wp.origin[1] = atof( cmd_command );
                trap_CmdArgv( 6, cmd_command, sizeof( cmd_command ) );
                wp.origin[2] = atof( cmd_command );
                AddWaypoint(&wp);
                return;
        }

        if(!strcmp(cmd_command, "link"))
        {
                wp_link_t link;
                int i1,i2;
                if( argc < 5 )
                        return;

                trap_CmdArgv( 3, cmd_command, sizeof( cmd_command ) );
                i1 = atoi( cmd_command );
                trap_CmdArgv( 4, cmd_command, sizeof( cmd_command ) );
                i2 = atoi( cmd_command );
                AddLink( i1, i2, &link);
                return;
        }
}
