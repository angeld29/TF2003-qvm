#include "g_local.h"

char   *DisguiseToSbar( gedict_t * pl, char *str );
char   *SentryInfoToSBar( gedict_t * pl, char *str );
char   *DispHealthToSbar( gedict_t * pl, char *str );
char   *AddStatusSize( gedict_t * pl, char *strp );
char   *ClipSizeToString( gedict_t * pl, char *str );

void CenterPrint( gedict_t * pl, const char *fmt, ... )
{
	va_list argptr;
	char    text[1024];

	va_start( argptr, fmt );
	vsprintf( text, fmt, argptr );
	va_end( argptr );

	trap_CenterPrint( NUM_FOR_EDICT( pl ), text );

	pl->StatusRefreshTime = g_globalvars.time + 1.5;
}

void StatusPrint( gedict_t * pl, float fTime, const char *fmt, ... )
{
	va_list argptr;
	char    text[1024];

	va_start( argptr, fmt );
	vsprintf( text, fmt, argptr );
	va_end( argptr );

	trap_CenterPrint( NUM_FOR_EDICT( pl ), text );

	pl->StatusRefreshTime = g_globalvars.time + fTime;
}

char   *teamnames[5] = {
	"  Blue:",
	"  Blue:",
	"  Red :",
	"  Yell:",
	"  Gren:"
};

#define MAX_SBAR_STR 16000
char    sbar_str[MAX_SBAR_STR];
char   *end_of_sbar_str = sbar_str + MAX_SBAR_STR;

void RefreshStatusBar( gedict_t * pl )
{
	char   *sbar_p;
	int     win;
	int     sec;
	gedict_t *te;

	if ( !pl->StatusBarSize )
	{
		pl->StatusRefreshTime = g_globalvars.time + 60;
		return;
	}
	pl->StatusRefreshTime = g_globalvars.time + 1.5;
	if ( !pl->playerclass )
		return;

	sbar_p = AddStatusSize( pl, sbar_str );

	if ( pl->playerclass == 9 || ( (pl->playerclass == 2 ||
	     pl->playerclass == 3 || pl->playerclass == 5 || 
	     pl->playerclass == 6 || pl->playerclass == 7 || tg_data.tg_sbar) && tg_data.tg_enabled) )
	{			/// eng sbar
		if ( pl->has_dispenser )
		{
			sbar_p = DispHealthToSbar( pl, sbar_p );
			if ( pl->has_sentry )
				*sbar_p++ = '|';
			*sbar_p = 0;
		}
		if ( pl->has_sentry )
		{
			sbar_p = SentryInfoToSBar( pl, sbar_p );
		}
		if ( pl->is_building )
		{
			strcat( sbar_p, "  building                             " );
			while ( *( ++sbar_p ) );
		}
	} else
	{
		if ( pl->playerclass == 8 )
		{
			if ( pl->is_feigning )
				strcat( sbar_p, "feigned  " );

			else
				strcat( sbar_p, "         " );
			while ( *( ++sbar_p ) );

			sbar_p = DisguiseToSbar( pl, sbar_p );
		}
///////////////////
		if ( pl->playerclass == 4 )
		{
			if ( pl->is_detpacking )
			{
				strcat( sbar_p, "  setting detpack                      " );
				while ( *( ++sbar_p ) );
			} else
			{
				for ( te =
				      find( world, FOFS( s.v.classname ), "detpack" ); te;
				      te = find( te, FOFS( s.v.classname ), "detpack" ) )
				{
					if ( te->s.v.owner != EDICT_TO_PROG( self ) )
						continue;
					sprintf( sbar_p, "Detpack: %d                 ",
						 ( int ) ( te->s.v.nextthink - g_globalvars.time ) );
					while ( *( ++sbar_p ) );
					break;
				}
			}
		}
		if ( pl->playerclass == 1 )
		{
			sprintf( sbar_p, "Scan %3d %s%s %3s                       ",
				 pl->ScanRange, ( pl->tf_items_flags & 1 ) ? "En" : "  ",
				 ( pl->tf_items_flags & 2 ) ? "Fr" : "  ", ( pl->ScannerOn ) ? "On " : "Off" );
			while ( *( ++sbar_p ) );
		}
	}

	*sbar_p++ = '\n';
	*sbar_p = 0;

	win = TeamFortress_TeamGetWinner(  );
	sec = TeamFortress_TeamGetSecond(  );

	sprintf( sbar_p, "%s%3d %s%3d",
		 teamnames[win], TeamFortress_TeamGetScore( win ), teamnames[sec], TeamFortress_TeamGetScore( sec ) );

	while ( *( ++sbar_p ) );
	sbar_p = ClipSizeToString( pl, sbar_p );
	*sbar_p++ = '\n';
	*sbar_p = 0;
//      *(char*)0 = 0;
	trap_CenterPrint( NUM_FOR_EDICT( pl ), sbar_str );
}

int     SbarReSizes_old[] = { 0, 5, 12, 17, 23, 25, 35, 50, 70 };
char   *AddStatusSize_old( gedict_t * pl, char *strp )
{
	int     i;

	for ( i = 0; i < 11; i++ )
		*strp++ = '\n';
	if ( pl->StatusBarSize != 1 )
	{
		*strp++ = '\n';
		*strp++ = '\n';
		*strp++ = '\n';
	}
	for ( i = 0; i < SbarReSizes_old[pl->StatusBarRes]; i++ )
		*strp++ = '\n';
	*strp = 0;
	return strp;
}
int     SbarReSizes[2][9] = {{ 14 ,19, 26, 32, 37, 39, 49, 64, 84 },
			     { 11 ,16, 23, 29, 34, 36, 46, 61, 81 }};

char   *AddStatusSize( gedict_t * pl, char *strp )
{
	int     i,num;

	if( pl->StatusBarSize == 1)
		num = SbarReSizes[1][pl->StatusBarRes];
	else
		num = SbarReSizes[0][pl->StatusBarRes];
		
	for ( i = 0; i < num; i++ )
		*strp++ = '\n';
	*strp = 0;
	return strp;
}

char   *sbar_msg[] = {
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


char   *ClipSizeToString( gedict_t * pl, char *str )
{
	int     num;

	if ( pl->current_weapon == 128 )
	{
//  if (8 - pl.reload_shotgun > pl.ammo_shells) 
//   pl.reload_shotgun = 8 - pl.ammo_shells;
		num = 8 - pl->reload_shotgun;
	} else
	{
		if ( pl->current_weapon == 256 )
		{
//   if (16 - pl.reload_super_shotgun > pl.ammo_shells) 
//    pl.reload_super_shotgun = 16 - pl.ammo_shells;
			num = 16 - pl->reload_super_shotgun;
		} else
		{
			if ( pl->current_weapon == 2048 )
			{
//    if (6 - pl.reload_grenade_launcher > pl.ammo_rockets) 
//     pl.reload_grenade_launcher = 6 - pl.ammo_rockets;
				num = 6 - pl->reload_grenade_launcher;
			} else
			{
				if ( pl->current_weapon == 8192 )
				{
//     if (4 - pl.reload_rocket_launcher > pl.ammo_rockets) 
//      pl.reload_rocket_launcher = 4 - pl.ammo_rockets;
					num = 4 - pl->reload_rocket_launcher;
				} else
					num = -1;
			}
		}
	}
	if ( num < 0 )
	{
		strcat( str, "         " );
		while ( *( ++str ) );
		return str;
	}
	sprintf( str, "  ÃÌÉÐ:%2d", num );
	while ( *( ++str ) );
	return str;
}
char   *spy_teams[] = {
	"  ",
	"Blue",
	"Red",
	"Yellow",
	"Green"
};
char   *spy_skins[] = {
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

char   *DisguiseToSbar( gedict_t * pl, char *str )
{

	if ( !pl->is_undercover )
	{
		strcat( str, "                                      " );
		while ( *( ++str ) );
		return str;
	}

	if ( pl->is_undercover == 2 )
	{
		strcat( str, "going undercover                       " );
		while ( *( ++str ) );
		return str;
	}

	if ( tf_data.invis_only )
	{
		strcat( str, "undercover                             " );
		while ( *( ++str ) );
		return str;
	}
	sprintf( str, "%s %s                             ",
		 spy_teams[pl->undercover_team], spy_skins[pl->undercover_skin] );
	while ( *( ++str ) );
	return str;
}

char   *SentryInfoToSBar( gedict_t * pl, char *str )
{
	gedict_t *te;

	for ( te = world; (te = find( te, FOFS( s.v.classname ), "building_sentrygun" )); )
	{
		if ( te->real_owner != pl )
			continue;
		if ( (te->has_sentry && te->s.v.health <= 0 ) && tg_data.tg_enabled )
		{
			sprintf( str, "SENTRY:dead" );
			break;
		}
		if ( te->s.v.health > 0 )
			sprintf( str, " SENTRY: %3d ", ( int ) te->s.v.health );
		while ( *( ++str ) );
		if ( !te->s.v.ammo_shells )
		{
			if ( !te->s.v.ammo_rockets && te->s.v.weapon == 3 )
				strcat( str, "no ammo                   " );
			else
				strcat( str, "no shells                 " );
		} else
		{
			if ( !te->s.v.ammo_rockets && te->s.v.weapon == 3 )
				strcat( str, " no rockets                " );
			else
				strcat( str, "                           " );
		}
		break;
	}
	while ( *( ++str ) );
	return str;
}

char   *DispHealthToSbar( gedict_t * pl, char *str )
{
	gedict_t *te;

	for ( te = world; (te = find( te, FOFS( s.v.classname ), "building_dispenser" )); )
	{
		if ( te->real_owner != pl )
			continue;
		sprintf( str, "DISP: %3d ", ( int ) te->s.v.health );
		while ( *( ++str ) );
		return str;
	}
	return str;
}
