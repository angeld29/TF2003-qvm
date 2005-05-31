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
 *  $Id: status.c,v 1.13 2005-05-31 20:01:30 AngelD Exp $
 */
#include "g_local.h"


void CenterPrint( gedict_t * pl, const char *fmt, ... )
{
	va_list argptr;
	char text[16000];

	va_start( argptr, fmt );
	_vsnprintf( text, sizeof(text), fmt, argptr );
	va_end( argptr );

	trap_CenterPrint( NUM_FOR_EDICT( pl ), text );

	pl->StatusRefreshTime = g_globalvars.time + 1.5;
}

/*void StatusPrint( gedict_t * pl, float fTime, const char *fmt, ... )
{
	va_list argptr;
	char text[16000];

	va_start( argptr, fmt );
	_vsnprintf( text, sizeof(text), fmt, argptr );
	va_end( argptr );

	trap_CenterPrint( NUM_FOR_EDICT( pl ), text );

	pl->StatusRefreshTime = g_globalvars.time + fTime;
}*/

const char   *teamnames[5] = {
	"  Blue:",
	"  Blue:",
	"  Red :",
	"  Yell:",
	"  Gren:"
};

//#define MAX_SBAR_STR 16000
//static char    sbar_str[MAX_SBAR_STR];
//static char   *end_of_sbar_str = sbar_str + MAX_SBAR_STR;

const char   *GetStatusSize( gedict_t * pl );
const char   *GetEngSbar( gedict_t * pl );
const char   *GetSpySbar( gedict_t * pl );

//const char   *ClipSizeToString( gedict_t * pl );
int GetClipSize( gedict_t * pl );

const char* default_status = "";
void RefreshStatusBar( gedict_t * pl )
{
	int     win;
	int     sec;
	gedict_t *te;
	const char *status_size,*status;
	char   stmp[1024];
	int    clip;

	if ( !pl->StatusBarSize )
	{
		pl->StatusRefreshTime = g_globalvars.time + 60;
		return;
	}
	pl->StatusRefreshTime = g_globalvars.time + 1.5;
	if ( !pl->playerclass )
		return;

	status_size = GetStatusSize( pl );
	status = default_status;

	if ( pl->playerclass == 9 || ( (pl->playerclass == 2 ||
	     pl->playerclass == 3 || pl->playerclass == 5 || 
	     pl->playerclass == 6 || pl->playerclass == 7 || tg_data.tg_sbar) && tg_data.tg_enabled) )
	{			/// eng sbar
	        status = GetEngSbar( pl );
	} else
	{
		if ( pl->playerclass == 8 )
		        status = GetSpySbar( pl );

		if ( pl->playerclass == 4 )
		{
			if ( pl->is_detpacking )
			{
				status = "  setting detpack                      ";
			} else
			{
				for ( te = world; (te = trap_find( te, FOFS( s.v.classname ), "detpack" ));)
				{
					if ( te->s.v.owner != EDICT_TO_PROG( self ) )
						continue;
					_snprintf( stmp, sizeof(stmp) , "Detpack: %d                 ",
						 ( int ) ( te->s.v.nextthink - g_globalvars.time ) );
					status = stmp;
					break;
				}
			}
		}
		if ( pl->playerclass == 1 )
		{
			_snprintf( stmp, sizeof(stmp), "Scan %3d %s%s %3s                       ",
				 pl->ScanRange, ( pl->tf_items_flags & 1 ) ? "En" : "  ",
				 ( pl->tf_items_flags & 2 ) ? "Fr" : "  ", ( pl->ScannerOn ) ? "On " : "Off" );
			status = stmp;
		}
	}

	win = TeamFortress_TeamGetWinner(  );
	sec = TeamFortress_TeamGetSecond(  );
	clip = GetClipSize( pl );
	if( clip >= 0)
	        G_centerprint( pl, "%s%s\n%s%3d %s%3d  ÃÌÉÐ:%2d\n", status_size, status, 
	        teamnames[win], TeamFortress_TeamGetScore( win ), teamnames[sec], TeamFortress_TeamGetScore( sec ),
	        clip);
	else
	        G_centerprint( pl, "%s%s\n%s%3d %s%3d         \n", status_size, status, 
	        teamnames[win], TeamFortress_TeamGetScore( win ), teamnames[sec], TeamFortress_TeamGetScore( sec ));
}

static const int     SbarReSizes[2][9] = {{ 14 ,19, 26, 32, 37, 39, 49, 64, 84 },
			     { 11 ,16, 23, 29, 34, 36, 46, 61, 81 }};

const char   *GetStatusSize( gedict_t * pl )
{
	static char ssize[100];
	char* strp = ssize;
    int num;

	if( pl->StatusBarSize == 1)
		num = SbarReSizes[1][pl->StatusBarRes];
	else
		num = SbarReSizes[0][pl->StatusBarRes];
		
	while( num-- )
	        *strp++ = '\n';
	*strp = 0;
	return ssize;
}

int   GetClipSize( gedict_t * pl )
{
	if ( pl->current_weapon == 128 )
	{
		return (8 - pl->reload_shotgun);
	} else
	{
		if ( pl->current_weapon == 256 )
		{
			return (16 - pl->reload_super_shotgun);
		} else
		{
			if ( pl->current_weapon == 2048 )
			{
				return  (6 - pl->reload_grenade_launcher);
			} else
			{
				if ( pl->current_weapon == 8192 )
				{
					return (4 - pl->reload_rocket_launcher);
				} else
					return -1;
			}
		}
	}
}
const char   *spy_teams[] = {
	"  ",
	"Blue",
	"Red",
	"Yellow",
	"Green"
};
const char   *spy_skins[] = {
	"  ",
	"Scout",
	"Sniper",
	"Soldier",
	"Demoman",
	"Medic",
	"Hvy Wpns",
	"Pyro",
	"Spy",
	"Engineer"
};
const char* GetSpySbar( gedict_t * pl )
{
        static char spy_status[1024];

        if( !pl->is_undercover )
        {
                if ( pl->is_feigning )
                        return "feigned                                        ";
                else
                        return "                                               ";
        }                
        if ( pl->is_undercover == 2 )
        {
                if ( pl->is_feigning )
                        return "feigned going undercover                       ";
                else
                        return "        going undercover                       ";
        }
        if ( tf_data.invis_only )
        {
                if ( pl->is_feigning )
                        return "feigned undercover                             ";
                else
                        return "        undercover                             ";
        }
        if( pl->is_feigning )
	       _snprintf( spy_status, sizeof( spy_status ),"feigned %s %s                             ",
		 spy_teams[pl->undercover_team], spy_skins[pl->undercover_skin] );
        else
	       _snprintf( spy_status, sizeof( spy_status ),"        %s %s                             ",
		 spy_teams[pl->undercover_team], spy_skins[pl->undercover_skin] );
	return spy_status;
}


const char* GetEngSbar( gedict_t * pl )
{
        const char  *build;
        char        *strp;
        gedict_t    *te;
        static char sg_status[1024];

        if( pl->is_building )
                build = "  building                             ";
        else
                build = "                                       ";

        if( !pl->has_dispenser && !pl->has_sentry )
                return build;
                        
        strp = sg_status;

      	if ( pl->has_dispenser )
      	{

        	for ( te = world; (te = trap_find( te, FOFS( s.v.classname ), "building_dispenser" )); )
        	{
        		if ( te->real_owner != pl )
        			continue;
        		_snprintf( strp, 30, "DISP: %3d ", ( int ) te->s.v.health );
        		strp += 10;
        		break;
        	}
      		if ( pl->has_sentry )
      			*strp++ = '|';
      	}
      	*strp = 0;
      	if ( pl->has_sentry )
      	{
        	for ( te = world; (te = trap_find( te, FOFS( s.v.classname ), "building_sentrygun" )); )
        	{
        		if ( te->real_owner != pl )
        			continue;
        		if ( (te->has_sentry && te->s.v.health <= 0 ) && tg_data.tg_enabled )
        		{
        			strcat( strp, "SENTRY:dead " );
        			break;
        		}
        		if ( te->s.v.health > 0 )
        		{
        			_snprintf( strp, 30, " SENTRY: %3d ", ( int ) te->s.v.health );
        			strp += 13;
        		}

        		if ( !te->s.v.ammo_shells )
        		{
        			if ( !te->s.v.ammo_rockets && te->s.v.weapon == 3 )
        				strcat( strp, "no ammo   " );
        			else
        				strcat( strp, "no shells " );
        		} else
        		{
        			if ( !te->s.v.ammo_rockets && te->s.v.weapon == 3 )
        				strcat( strp, "no rockets ");
        			else
        				strcat( strp, "           ");
        		}
        		strp += 11;
        		break;
        	}
        	
      	}
      	strcat( strp, build );
      	return sg_status;
}



static const char   *sbar_msg[] = {
	"Status Bar Res set to 320x200.\n",
	"Status Bar Res set to 320x240.\n",
	"Status Bar Res set to 400x300.\n",
	"Status Bar Res set to 320x350.\n",
	"Status Bar Res set to 512x384.\n",
	"Status Bar Res set to 640x400.\n",
	"Status Bar Res set to 640x480.\n",
	"Status Bar Res set to 800x600.\n",
	"Status Bar Res set to 1024x768.\n"
};
void StatusRes( int res )
{
	if ( res > 8 )
		res = 8;
	self->StatusBarRes = res;
	G_sprint( self, 2, sbar_msg[res] );
}

