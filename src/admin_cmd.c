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
 *  $Id: admin_cmd.c,v 1.1 2004-12-23 03:16:14 AngelD Exp $
 */
#include "g_local.h"

typedef struct {
	char		*command;
	void		(*Action) (int);
	int		level;
}admin_cmd_t;

void Admin_Auth(int argc);
//void Admin_ListPlayers(int argc);
void Admin_Status(int argc);
void Admin_Kick(int argc);
void Admin_Ban(int argc);
void Admin_UnBan(int argc);
void Admin_BanList(int argc);
void Admin_Console(int argc);
void Admin_Map(int argc);

admin_cmd_t admin_cmds[] = 
{
	{"auth", Admin_Auth,0},
//	{"listplayers", Admin_ListPlayers, 1},
	{"kick", Admin_Kick, 1},
	{"map", Admin_Map, 2},
	{"status", Admin_Status, 3},
	{"ban", Admin_Ban, 3},
	{"banlist", Admin_BanList, 3},
	{"unban", Admin_UnBan, 3},
	{"console", Admin_Console, 999},
	{NULL,NULL,0}
};

int CheckString(char *str)
{
	char *strp =str,ch;
	while(ch = *strp++)
	{
		switch(ch)
		{
			case ';':
			case 0x0d:
			case 0x0a:
			case '\b':
			case '"':
				return 0;
		}
	}
	return 1;
}
gedict_t* Admin_FindPlayerById(int id)
{
	
    	int i;
    	int plid;
    	

	for ( i = 0 ; i < MAX_CLIENTS ; i++ )
	{

		if( g_edicts[i + 1].s.v.netname[0]  == 0)
			continue;
		plid = GetInfokeyInt( &g_edicts[i + 1], "*userid", NULL, -1 );
		if( id == plid)
			return &g_edicts[i + 1];
	}
	return NULL;
}

void Admin_Cmd()
{
	int argc;
	char    cmd_command[1024];
	admin_cmd_t* ucmd;

	argc = trap_CmdArgc();

	if( argc == 1 )
	{
	        G_sprint( self, 2, "Allowed admin commands:\n");
		for ( ucmd = admin_cmds ; ucmd->command  ; ucmd++ )
		{
			if( self->is_admin >= ucmd->level )
			{
				G_sprint( self, 2, "%s\n",ucmd->command);
			}
		}
		return;
	}
	trap_CmdArgv( 1, cmd_command, sizeof( cmd_command ) );
	for ( ucmd = admin_cmds ; ucmd->command  ; ucmd++ )
	{
		if( !strcmp(cmd_command,ucmd->command) )
		{
			if( self->is_admin >= ucmd->level )
			{
				ucmd->Action(argc);
			        return;
			}else
			{
				G_sprint( self, 2, "You have limited Access, You cannot use this command.\n");
			        return;
			}
		}
	}
	G_sprint( self, 2, "Unknown admin command.\n");
}

void Admin_Auth(int argc)
{
        char    sv_admin_pwd[100];
        char    admin_pwd[100];

	if( argc != 3)
	{
	        G_sprint( self, 2, "Usage: cmd admin auth <password>\n");
		return;
	}
	if(!GetSVInfokeyString( "apw", "adminpwn", sv_admin_pwd, sizeof( sv_admin_pwd ), "" ))
		return;

	trap_CmdArgv( 2, admin_pwd, sizeof( admin_pwd ) );
	if( !strcmp(admin_pwd,sv_admin_pwd) )
	{
		self->is_admin = GetSVInfokeyInt("adminlevel",NULL,1);
		if( self->is_admin )
		{
	        	G_sprint( self, 2, "You gain admin level %d\n",self->is_admin);
		}
	}
}

/*void Admin_ListPlayers(int argc)
{
    	int i;
    	G_sprint( self, 2, "Players:\n");

	for ( i = 0 ; i < MAX_CLIENTS ; i++ )
	{

		if( g_edicts[i + 1].s.v.netname[0] )
		{
			G_sprint( self, 2, "%2d %c %s\n", i, g_edicts[i + 1].is_admin?'A':' ',g_edicts[i + 1].s.v.netname);
		}
	}
}*/

void Admin_Kick(int argc)
{
	char    value[100];
	gedict_t*p;
	int id;

	if( argc != 3)
	{
	        G_sprint( self, 2, "Usage: cmd admin kick <userid>\n");
		return;
	}

	trap_CmdArgv( 2, value, sizeof( value ) );
	id = atoi(value);
	p = Admin_FindPlayerById(id);
	if(!p)
	{
		G_sprint( self, 2,"Invalid player id\n");
		return;
	}

	G_bprint(  2, "%s was kicked by %s\n",p->s.v.netname, self->s.v.netname);
//	localcmd("kick %d",id);
	KickCheater( p );
}

void Admin_Ban(int argc)
{
	char    value[100];
	int id;
	gedict_t*p;

	if( argc != 3)
	{
	        G_sprint( self, 2, "Usage: cmd admin ban <userid>\n");
		return;
	}

	trap_CmdArgv( 2, value, sizeof( value ) );
	id = atoi(value);
	p = Admin_FindPlayerById(id);
	if(!p)
	{
		G_sprint( self, 2,"Invalid player id\n");
		return;
	}
	
	GetInfokeyString( p , "ip", NULL, value, sizeof( value ), "" );
	G_sprint( self, 2,"banning ip %s\n", value);
	localcmd("addip %s\n",value);
	trap_executecmd();
}

void Admin_UnBan(int argc)
{
	char    value[100];

	if( argc != 3)
	{
	        G_sprint( self, 2, "Usage: cmd admin unban <ipmask>\n");
		return;
	}

	trap_CmdArgv( 2, value, sizeof( value ) );

	if(!CheckString(value))
		return;

	localcmd("removeip \"%s\"\n",value);
	trap_executecmd();
}

void Admin_BanList(int argc)
{
	localcmd("listip\n");
	trap_executecmd();
}

void Admin_Status(int argc)
{
	localcmd("status\n");
	trap_executecmd();
}

void Admin_Map(int argc)
{
	char    value[100];

	if( argc != 3)
	{
	        G_sprint( self, 2, "Usage: cmd admin map <mapname>\n");
		return;
	}

	trap_CmdArgv( 2, value, sizeof( value ) );
	if(!CheckString(value))
		return;
	localcmd("map \"%s\"\n",value);
//	trap_executecmd();
}

void Admin_Console(int argc)
{

}


