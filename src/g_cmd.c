/*
 *  QWProgs-DM
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
 *  $Id: g_cmd.c,v 1.6 2004-11-14 07:04:08 AngelD Exp $
 */

#include "g_local.h"

extern void trap_CmdArgv( int arg, char *valbuff, int sizebuff );

void    ClientKill(  );
void Test();
void TG_Cmd();
void Vote_Cmd();
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
	if ( !strcmp( cmd_command, "test" ) )
	{
	     //!!!!!!remove me
	     Test();
	     return true;
	}
	if ( !strcmp( cmd_command, "tg" ) )
	{
	     TG_Cmd();
	     return true;
	}

	if ( !strcmp( cmd_command, "vote" ) )
	{
	     Vote_Cmd();
	     return true;
	}

	//G_Printf("ClientCommand %s\n",cmd_command);
	return false;
}


qboolean ClientUserInfoChanged()
{
	char    key[1024];
	char    value[1024];
	char	*sk;

	self = PROG_TO_EDICT( g_globalvars.self );
	
	trap_CmdArgv( 1, key, sizeof( key ) );

	if(strcmp(key,"team"))
		return 0;
	trap_CmdArgv( 2, value, sizeof( value ) );

	if(!self->team_no)
		return 0;
	sk = GetTeamName( self->team_no );
	if( strneq( value, sk ) )
	{
		SetTeamName( self );
	        G_sprint( self, 2, "you cannot change your team setinfo\n");
	        return 1;
	}
	return 0;

}

void Test()
{
	char    value[1024];
	float	h;
	
	if( trap_CmdArgc() < 2)
	{
		G_sprint( self, 2, "health %f %f\n",self->s.v.health, self->s.v.deadflag);
	        return;
	}
	trap_CmdArgv( 1, value, sizeof( value ) );
	h = atof(value);
	if(h < 0)
		h = 1;
	TF_T_Damage( self, world, world, h, TF_TD_IGNOREARMOUR, 0 );
}
void Test_q()
{
	char    value[1024];
	int type,dist;
	vec3_t  src;
	vec3_t  dst;
	vec3_t  dir;
	gedict_t *targ;

	trap_CmdArgv( 1, value, sizeof( value ) );
	type = atoi(value);
	trap_CmdArgv( 2, value, sizeof( value ) );
	dist = atoi(value);

	VectorAdd( self->s.v.origin, self->s.v.view_ofs, src );
	makevectors( self->s.v.v_angle );
	VectorScale(g_globalvars.v_forward, dist, dir);
	VectorAdd(src,dir,dst);
	traceline( PASSVEC3( src ), PASSVEC3( dst ), type, self );
       	trap_WriteByte( MSG_BROADCAST, SVC_TEMPENTITY );
       	trap_WriteByte( MSG_BROADCAST, TE_TAREXPLOSION );
       	trap_WriteCoord( MSG_BROADCAST, g_globalvars.trace_endpos[0] );
       	trap_WriteCoord( MSG_BROADCAST, g_globalvars.trace_endpos[1] );
       	trap_WriteCoord( MSG_BROADCAST, g_globalvars.trace_endpos[2] );
       	trap_multicast( PASSVEC3( g_globalvars.trace_endpos ), MULTICAST_PHS );
       	G_sprint(self, 2, "fraction %.3f\n",g_globalvars.trace_fraction);

	if(g_globalvars.trace_ent)
	{
	        targ = PROG_TO_EDICT(g_globalvars.trace_ent);
		G_sprint(self, 2, "ent %s\n",targ->s.v.classname);
//		VectorAdd( targ->s.v.origin, targ->s.v.view_ofs, dst );
//		traceline( PASSVEC3( src ), PASSVEC3( dst ), type, self );
         	
	}
	
		
}

