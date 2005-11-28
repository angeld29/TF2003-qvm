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
 *  $Id: g_cmd.c,v 1.20 2005-11-28 18:30:57 AngelD Exp $
 */

#include "g_local.h"


#define CMD_NOT_PREMATCH 	1
#define CMD_NOT_DEAD		2
#define CMD_NOT_ATTACK		4
#define CMD_NOT_TEAM		8
#define CMD_NOT_CLASS		16
#define CMD_SPECTATOR_ALLOWED	32


typedef struct {
	char   *command;
	void    ( *Action ) (  );
	int     allowed;
} cmd_t;

void    ClientKill(  );
void    Test(  );
void    Bot();
void    RemoveBot(  );
void    TG_Cmd(  );
void    Vote_Cmd(  );
void    TeamFortress_Cmd_Discard(  );
void    Engineer_RotateSG(  );
void    TeamFortress_Cmd_Detpack(  );
void    Admin_Cmd(  );
void    Client_Set_Cmd(  );

const static cmd_t   cmds[] = {
	{"kill", ClientKill},
	{"test", Test},
	{"bot", Bot, CMD_SPECTATOR_ALLOWED},
	{"tg", TG_Cmd, CMD_SPECTATOR_ALLOWED},
	{"vote", Vote_Cmd, CMD_NOT_PREMATCH},
        {"set", Client_Set_Cmd},
	{"admin", Admin_Cmd, CMD_SPECTATOR_ALLOWED},
	{"discard", TeamFortress_Cmd_Discard, CMD_NOT_PREMATCH | CMD_NOT_DEAD | CMD_NOT_TEAM | CMD_NOT_CLASS},
	{"sg_rotate", Engineer_RotateSG, CMD_NOT_PREMATCH | CMD_NOT_DEAD | CMD_NOT_TEAM | CMD_NOT_CLASS},
	{"detpack", TeamFortress_Cmd_Detpack, CMD_NOT_PREMATCH | CMD_NOT_DEAD | CMD_NOT_TEAM | CMD_NOT_CLASS},
	{NULL, NULL, 0}
};
extern void trap_CmdArgv( int arg, char *valbuff, int sizebuff );


qboolean ClientCommand(  )
{
	char    cmd_command[1024];
	const cmd_t  *ucmd;

	self = PROG_TO_EDICT( g_globalvars.self );

	trap_CmdArgv( 0, cmd_command, sizeof( cmd_command ) );
	for ( ucmd = cmds; ucmd->command; ucmd++ )
	{
		if ( strcmp( cmd_command, ucmd->command ) )
			continue;
		if ( (!( ucmd->allowed & CMD_SPECTATOR_ALLOWED )) && self->isSpectator)
		{     
			G_sprint( self, 2, "cmd '%s' not allowed for spectators\n", cmd_command );
			return true;
 	        }
		if ( ( ucmd->allowed & CMD_NOT_PREMATCH )
		     && ( tf_data.cb_prematch_time > g_globalvars.time ) )
		{
			G_sprint( self, 2, "cmd '%s' not allowed in prematch\n", cmd_command );
			return true;
		}

		if ( ( ucmd->allowed & CMD_NOT_DEAD ) && ( self->s.v.deadflag ) )
			return true;

		if ( ( ucmd->allowed & CMD_NOT_ATTACK ) && ( g_globalvars.time < self->attack_finished ) )
			return true;

		if ( ( ucmd->allowed & CMD_NOT_TEAM ) && ( !self->team_no ) )
			return true;

		if ( ( ucmd->allowed & CMD_NOT_CLASS ) && ( !self->playerclass ) )
			return true;


		ucmd->Action(  );
		return true;
	}
	return false;
}


qboolean SetClientSetting( gedict_t*p , const char*key, const char* value );

qboolean ClientUserInfoChanged(  )
{
	char    key[1024];
	char    value[1024];
	const   char   *sk;
	int     color;

	self = PROG_TO_EDICT( g_globalvars.self );

	trap_CmdArgv( 1, key, sizeof( key ) );
	trap_CmdArgv( 2, value, sizeof( value ) );

	if ( !strcmp( key, "team" ) )
	{
		if ( !self->team_no )
			return 0;
		sk = GetTeamName( self->team_no );
		if ( strneq( value, sk ) )
		{
			SetTeamName( self );
			G_sprint( self, 2, "you cannot change your team setinfo\n" );
			return 1;
		}
		return 0;
	}
	if ( !strcmp( key, "skin" ) )
	{
		if ( !self->playerclass || !self->team_no )
			return 0;
		sk = TeamFortress_GetSkin( self );
		if ( strneq( value, sk ) )
		{
			G_sprint( self, 2, "you cannot change your skin setinfo\n" );
			TeamFortress_SetSkin( self );
			return 1;
		}
		return 0;
	}
	if ( ( !strcmp( key, "topcolor" ) ) && tf_data.topcolor_check )
	{
		if ( !self->team_no )
			return 0;
		color = atoi( value );
		if ( self->playerclass == PC_SPY && self->undercover_team )
		{
			if ( TeamFortress_TeamGetTopColor( self->undercover_team ) != color )
			{
				G_sprint( self, 2, "you cannot change your topcolor setinfo\n" );
				return 1;
			}
		} else
		{
			if ( TeamFortress_TeamGetTopColor( self->team_no ) != color )
			{
				G_sprint( self, 2, "you cannot change your topcolor setinfo\n" );
				return 1;
			}
		}
		return 0;
	}

	if ( ( !strcmp( key, "bottomcolor" ) ) )
	{
		if ( !self->team_no )
			return 0;
		color = atoi( value );
		if ( self->playerclass == PC_SPY && self->undercover_team )
		{
			if ( TeamFortress_TeamGetColor( self->undercover_team ) - 1 != color )
			{
				G_sprint( self, 2, "you cannot change your bottomcolor setinfo\n" );
				return 1;
			}
		} else
		{
			if ( TeamFortress_TeamGetColor( self->team_no ) - 1 != color )
			{
				G_sprint( self, 2, "you cannot change your bottomcolor setinfo\n" );
				return 1;
			}
		}
		return 0;
	}

	SetClientSetting( self, Q_strlwr( key ), value);
	return 0;

}

void Test(  )
{
}

