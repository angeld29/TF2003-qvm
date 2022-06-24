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
 *  $Id: tforthlp.c,v 1.10 2006-11-21 21:47:50 AngelD Exp $
 */
#include "g_local.h"

void TeamFortress_MOTD(  )
{

    if ( self->motd == 4 )
        stuffcmd( self, "is_aliased\n" );
    if ( self->motd <= 20 )
    {
        self->motd += 1;
        return;
    }
    if ( self->motd == 21 )
    {
        if ( !0 )
            self->tfstate |= TFSTATE_ZOOMOFF;

        if ( teamplay && !self->team_no )
        {
            TeamFortress_SetColor( self, 0,0);
        }
        if ( !self->got_aliases )
            G_sprint( self, 2, "\n\n\n" );
        G_sprint( self, 2, "Type help for more info.\n" );
        G_sprint( self, 3, "QuakeWorld TeamFortress 2003\n" );

        if( tfset(enable_bot) )
        {
            G_sprint( self, 2, "Bots Enabled\n" );
        }

        G_sprint( self, 2, _BY_SD_ANGEL "\n" );
        G_sprint( self, 2, "Bug reports to angel_death@mail.ru\n" );
        G_sprint( self, 2, "http://" _m _t _f _l "."  _s _d "."  _r _u"\n" );

        self->menu_displaytime = 0;
        self->current_menu = MENU_INTRO;
        self->menu_count = MENU_REFRESH_RATE;
        self->motd = 22;
        if ( self->got_aliases == 1 )
            return;
        G_sprint( self, 2, "binding aliases...\n" );
        TeamFortress_Alias( "inv", TF_INVENTORY, 0 );
        TeamFortress_Alias( "showtf", TF_SHOWTF, 0 );
        TeamFortress_Alias( "autozoom", TF_AUTOZOOM, 0 );
        TeamFortress_Alias( "changeclass", TF_CHANGEPC_MENU, 0 );
        TeamFortress_Alias( "scout", TF_CHANGEPC + PC_SCOUT, 0 );
        TeamFortress_Alias( "sniper", TF_CHANGEPC + PC_SNIPER, 0 );
        TeamFortress_Alias( "soldier", TF_CHANGEPC + PC_SOLDIER, 0 );
        TeamFortress_Alias( "demoman", TF_CHANGEPC + PC_DEMOMAN, 0 );
        TeamFortress_Alias( "medic", TF_CHANGEPC + PC_MEDIC, 0 );
        TeamFortress_Alias( "hwguy", TF_CHANGEPC + PC_HVYWEAP, 0 );
        TeamFortress_Alias( "pyro", TF_CHANGEPC + PC_PYRO, 0 );
        TeamFortress_Alias( "spy", TF_CHANGEPC + PC_SPY, 0 );
        TeamFortress_Alias( "engineer", TF_CHANGEPC + PC_ENGINEER, 0 );
        TeamFortress_Alias( "randompc", TF_CHANGEPC + PC_RANDOM, 0 );
        TeamFortress_Alias( "is_aliased", TF_ALIAS_CHECK, 0 );
        return;
    }
    if ( self->motd <= 95 )
        self->motd += 1;
    if ( self->motd == 25 )
    {
        TeamFortress_Alias( "detpipe", TF_PB_DETONATE, 0 );
        TeamFortress_Alias( "reload", TF_RELOAD, 0 );
        TeamFortress_Alias("weapnext", TF_WEAPNEXT, 0);
        TeamFortress_Alias("weapprev", TF_WEAPPREV, 0);
        TeamFortress_Alias("weaplast", TF_WEAPLAST, 0);
        TeamFortress_Alias( "scan10", TF_SCAN_10, 0 );
        TeamFortress_Alias( "scan30", TF_SCAN_30, 0 );
        TeamFortress_Alias( "scan50", TF_SCAN_100, 0 );
        TeamFortress_Alias( "scan100", TF_SCAN_100, 0 );
        TeamFortress_Alias( "scanf", TF_SCAN_FRIENDLY, 0 );
        TeamFortress_Alias( "scane", TF_SCAN_ENEMY, 0 );
        TeamFortress_Alias( "autoscan", AUTOSCAN_IMPULSE, 0 );
        TeamFortress_Alias("scanf_off", TF_SCAN, TF_POST_SCANF_OFF);
        TeamFortress_Alias("scanf_on" , TF_SCAN, TF_POST_SCANF_ON );
        TeamFortress_Alias("scane_off", TF_SCAN, TF_POST_SCANE_OFF);
        TeamFortress_Alias("scane_on" , TF_SCAN, TF_POST_SCANE_ON );
        TeamFortress_Alias("autoscan_on" ,  TF_SCAN, TF_POST_AUTOSCAN_ON  );
        TeamFortress_Alias("autoscan_off" , TF_SCAN, TF_POST_AUTOSCAN_OFF );
        return;
    }
    if ( self->motd == 35 )
    {
        TeamFortress_Alias( "primeone", TF_GRENADE_1, 0 );
        TeamFortress_Alias( "primetwo", TF_GRENADE_2, 0 );
        TeamFortress_Alias( "throwgren", TF_GRENADE_T, 0 );
        TeamFortress_Alias( "+gren1", TF_GRENADE_1, 0 );
        TeamFortress_Alias( "+gren2", TF_GRENADE_2, 0 );
        TeamFortress_Alias( "-gren1", TF_GRENADE_T, 0 );
        TeamFortress_Alias( "-gren2", TF_GRENADE_T, 0 );
        TeamFortress_Alias( "showscores", TF_TEAM_SCORES, 0 );
        TeamFortress_Alias( "showclasses", TF_TEAM_CLASSES, 0 );
        TeamFortress_Alias( "query", TF_STATUS_QUERY, 0 );
        return;
    }
    if ( self->motd == 45 )
    {
        TeamFortress_Alias( "dropkey", TF_DROPKEY, 0 );
        TeamFortress_Alias( "dropammo", TF_DROP_AMMO, 0 );
        TeamFortress_Alias( "dropitems", TF_DROPGOAL, 0 );
        TeamFortress_Alias( "showloc", TF_DISPLAYLOCATION, 0 );
        TeamFortress_Alias( "special", TF_SPECIAL_SKILL, 0 );
        TeamFortress_Alias( "saveme", TF_MEDIC_HELPME, 0 );
        TeamFortress_Alias( "discard", TF_DISCARD, 0 );
        TeamFortress_Alias( "id", TF_ID, 0 );
        return;
    }
    if ( self->motd == 55 )
    {
        TeamFortress_Alias( "flaginfo", TF_FLAG_INFO, 0 );
        TeamFortress_Alias( "maphelp", TF_HELP_MAP, 0 );
        TeamFortress_Alias( "showids", TF_SHOWIDS, 0 );
        stuffcmd( self, "bind 9 \"impulse 9\"\n" );
        stuffcmd( self, "bind 0 \"impulse 10\"\n" );
        return;
    }
    if ( self->motd == 65 )
    {
        TeamFortress_Alias( "sbar_on", TF_STATUSBAR_ON, 0 );
        TeamFortress_Alias( "sbar_off", TF_STATUSBAR_OFF, 0 );
        TeamFortress_Alias( "sbar_200", TF_STATUSBAR_RES_START, 0 );
        TeamFortress_Alias( "sbar_240", TF_STATUSBAR_RES_START + 1, 0 );
        TeamFortress_Alias( "sbar_300", TF_STATUSBAR_RES_START + 2, 0 );
        TeamFortress_Alias( "sbar_350", TF_STATUSBAR_RES_START + 3, 0 );
        TeamFortress_Alias( "sbar_384", TF_STATUSBAR_RES_START + 4, 0 );
        TeamFortress_Alias( "sbar_400", TF_STATUSBAR_RES_START + 5, 0 );
        TeamFortress_Alias( "sbar_480", TF_STATUSBAR_RES_START + 6, 0 );
        TeamFortress_Alias( "sbar_600", TF_STATUSBAR_RES_START + 7, 0 );
        TeamFortress_Alias( "sbar_768", TF_STATUSBAR_RES_START + 8, 0 );
        return;
    }
    if ( self->motd == 75 )
    {
        TeamFortress_Alias( "+det5", TF_DETPACK_5, 0 );
        TeamFortress_Alias( "-det5", TF_DETPACK_STOP, 0 );
        TeamFortress_Alias( "+det20", TF_DETPACK_20, 0 );
        TeamFortress_Alias( "-det20", TF_DETPACK_STOP, 0 );
        TeamFortress_Alias( "+det50", TF_DETPACK_50, 0 );
        TeamFortress_Alias( "-det50", TF_DETPACK_STOP, 0 );
        return;
    }
    if ( self->motd == 85 )
    {
        if(tfset(tg_enabled))
        {
            TeamFortress_Alias( "mod_menu", TG_MAINMENU_IMPULSE, 0 );
            TeamFortress_Alias( "sg_rebuild", TG_SG_REBUILD_IMPULSE, 0 );
            TeamFortress_Alias( "sg_reload", TG_SG_RELOAD_IMPULSE, 0 );
            TeamFortress_Alias( "sg_upgrade", TG_SG_UPGRADE_IMPULSE, 0 );
            TeamFortress_Alias( "disp_load", TG_DISP_LOAD_IMPULSE, 0 );
            TeamFortress_Alias( "disp_unload", TG_DISP_UNLOAD_IMPULSE, 0 );
            TeamFortress_Alias( "eff_conc", TG_CONC_IMPULSE, 0);
            TeamFortress_Alias( "eff_flash", TG_FLASH_IMPULSE, 0);
            TeamFortress_Alias( "eff_remove", TG_EFF_REMOVE_IMPULSE, 0);
        }
        return;
    }
    if ( self->motd == 95 )
    {
        TeamFortress_Alias( "disguise", TF_SPY_SPY, 0 );
        TeamFortress_Alias( "feign", TF_SPY_DIE, 0 );
        TeamFortress_Alias( "sfeign", TF_SPY_SFEIGN_IMPULSE, 0 );
        TeamFortress_Alias( "build", TF_ENGINEER_BUILD, 0 );
        TeamFortress_Alias( "detsentry", TF_ENGINEER_DETSENTRY, 0 );
        TeamFortress_Alias( "detdispenser", TF_ENGINEER_DETDISP, 0 );
    }
}

void TeamFortress_HelpMap(  )
{
	gedict_t *te;

	te = trap_find( world, FOFS( s.v.classname ), "info_tfdetect" );
	if ( te )
	{
		if ( te->non_team_broadcast )
		{
			G_sprint( self, 2, "%s", te->non_team_broadcast );
			return;
		}
	}
	G_sprint( self, 2, "There is no help for this map.\n" );
}
