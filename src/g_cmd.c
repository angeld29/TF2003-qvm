/*
 *  QWProgs-DM
 *  Copyright (C) 2004  [sd] angel
 *
 *  This code is based on Q3 VM code by Id Software, Inc.
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
 *  $Id: g_cmd.c,v 1.1 2004-09-05 22:48:30 AngelD Exp $
 */

#include "g_local.h"

extern void trap_CmdArgv( int arg, char *valbuff, int sizebuff );

void    ClientKill(  );
qboolean ClientCommand(  )
{
	char    cmd_command[1024];

	self = PROG_TO_EDICT( g_globalvars.self );
	trap_CmdArgv( 0, cmd_command, sizeof( cmd_command ) );

	if ( !strcmp( cmd_command, "kill" ) )
	{
		ClientKill(  );
		return true;
	}
/*	if ( !strcmp( cmd_command, "builddisp" ) )
	{
		if ( self->is_building )
			TeamFortress_EngineerBuild(  );
		else
			TeamFortress_Build( 1 );
		return true;
	}
	if ( !strcmp( cmd_command, "buildsentry" ) )
	{
		if ( self->is_building )
			TeamFortress_EngineerBuild(  );
		else
			TeamFortress_Build( 2 );
		return true;
	}*/
	//G_Printf("ClientCommand %s\n",cmd_command);
	return false;
}
