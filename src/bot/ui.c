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
 *  $Id: ui.c,v 1.8 2006-03-04 15:10:06 AngelD Exp $
 */
#include "g_local.h"

typedef struct bot_cmd_s{
	char		*command;
	void		(*func) ();
}bot_cmd_t;

void AddBot(  );
void RemoveBot(  );
void ResupplySetBot();
void BotReport();
void BotSet();
void BotTest();
bot_cmd_t bot_cmds[]=
{
	{"add", AddBot},
	{"remove", RemoveBot},
	{"resupply", ResupplySetBot},
	{"report", BotReport},
	{"set", BotSet},
//	{"test", BotTest},
	{NULL}
};

void BotReport(  )
{
	gedict_t *te;


	for ( te = world; ( te = trap_find( te, FOFS( s.v.classname ), "player" ) ); )
	{
		if ( te->has_disconnected )
			continue;
		if ( te->isBot )
		{
                   G_bprint(3,"Bot: %s, state %d, menu %d flags %d velocity %3.0f\norigin [%.0f %.0f %.0f]\n",
                           te->s.v.netname,te->action,te->current_menu,
                           (int)(te->s.v.flags),
                           vlen(te->s.v.velocity),
                           PASSVEC3(te->s.v.origin)
                   );

                   if( te->wp )
                       G_bprint(3, "wp     [%.0f %.0f %.0f]\n", PASSVEC3(te->wp->origin));
                   else
                       G_bprint(3, "nowp\n");

                   if( te->end_wp )
                       G_bprint(3, "endwp  [%.0f %.0f %.0f]\n", PASSVEC3(te->end_wp->origin));

		}
	}
}

void AddBot(  )
{
	char    value[1024];
	int     team, class, argc;
	char*  name;

	argc = trap_CmdArgc(  );

	if ( argc < 4 )
	{
		G_sprint( self, 2, "usage: cmd bot add <team> <class> [<name>]\n" );
		return;
	}

	trap_CmdArgv( 2, value, sizeof( value ) );
	team = atoi( value );
	trap_CmdArgv( 3, value, sizeof( value ) );
	class = atoi( value );
	if( argc >= 5 )
	{
	        trap_CmdArgv( 4, value, sizeof( value ) );
	        name = value;
	}else
	       name = PickAName(  );

	botConnect( team, class, name );
}


void RemoveBot(  )
{
	gedict_t *te;


	for ( te = world; ( te = trap_find( te, FOFS( s.v.classname ), "player" ) ); )
	{
		if ( te->has_disconnected )
			continue;
		if ( te->isBot )
			break;
	}

	if ( !te )
		return;
	botDisconnect( te );
}

void ResupplySetBot()
{
        int argc;
        char    value[1024];

        argc = trap_CmdArgc();
        if( argc != 3 )
        {
                if( tf_data.botResupplyBetweenKills)
                        G_sprint( self, 2, "Bot Resupply Between Kills: On\n" );
                else
                        G_sprint( self, 2, "Bot Resupply Between Kills: Off\n" );
		return;
        }

        if( tf_data.arena_mode )
        {
                tf_data.botResupplyBetweenKills = 0;
        }else
        {
              trap_CmdArgv( 2, value, sizeof( value ) );
              if( !strcmp(value,"on") )
                      tf_data.botResupplyBetweenKills = 1;
              else
                      tf_data.botResupplyBetweenKills = 0;
        }
        if( tf_data.botResupplyBetweenKills)
                G_sprint( self, 2, "Bot Resupply Between Kills: On\n" );
        else
                G_sprint( self, 2, "Bot Resupply Between Kills: Off\n" );

}

void Bot()
{
        
        char    cmd_command[50];
        int argc,i;
        bot_cmd_t*ucmd;

        if( api_ver < 7 )
        {
                G_sprint( self, 2, "Bots disabled (API version %d)\n",api_ver );
                return;
        }

	if ( !tf_data.enable_bot )
	{
		G_sprint( self, 2, "Bots disabled\n" );
//FIXME!!!!
                return;
	}

	argc = trap_CmdArgc();
	
	if( argc == 1 )
	{
	        G_sprint( self, 2, "Avaliable commands\n");
		for ( ucmd = bot_cmds ; ucmd->command  ; ucmd++ )
		{
			G_sprint( self, 2, "%s\n",ucmd->command);
		}
		return;
	}

	trap_CmdArgv( 1, cmd_command, sizeof( cmd_command ) );
	for ( ucmd = bot_cmds,i=0 ; ucmd->command  ; ucmd++,i++ )
	{
		if( !strcmp(cmd_command,ucmd->command) )
		{
			ucmd->func();
			return;
		}
	}
}

extern float max_yaw_per_sek;
void BotSet()
{
        char    cmd_command[50];
        int argc;

        argc = trap_CmdArgc();

        if( argc < 3 )
        {
                return;
        }
        trap_CmdArgv( 2, cmd_command, sizeof( cmd_command ) );

        if( !strncmp(cmd_command,"max_yaw_speed",sizeof(cmd_command)))
        {
                if( argc == 3 )
                        G_sprint(self,2,"max_yaw_speed = %3.0f\n",max_yaw_per_sek);
                else
                {
                        trap_CmdArgv( 3, cmd_command, sizeof( cmd_command ) );
                        max_yaw_per_sek = atof(cmd_command);
                        if( max_yaw_per_sek < 10 )
                                max_yaw_per_sek = 10;

                        G_sprint(self,2,"max_yaw_speed set to %3.0f\n",max_yaw_per_sek);
                }
        
        }
        

}

void BotTest()
{
/* 	gedict_t *te;
        char    cmd_command[50];
        int argc;
        float origin,speed;

	argc = trap_CmdArgc();
        if( argc < 4 )
        {
                return;
        }
        trap_CmdArgv( 2, cmd_command, sizeof( cmd_command ) );
        origin = atof(cmd_command);

        trap_CmdArgv( 3, cmd_command, sizeof( cmd_command ) );
        speed = atof(cmd_command);

	for ( te = world; ( te = trap_find( te, FOFS( s.v.classname ), "player" ) ); )
	{
		if ( te->has_disconnected )
			continue;
		if ( te->isBot )
		{
		   te->s.v.origin[2] = origin;
		   te->s.v.velocity[2] = speed;
                   return;
		}
	}
*/
}
