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
 *  $Id: version.c,v 1.3 2004-12-23 03:16:15 AngelD Exp $
 */

#include "g_local.h"


char *ANGEL_VERSION = "Compiled: " __DATE__ " " __TIME__ " ";

static char *date = __DATE__ ;
static char *mon[12] = 
{ "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
static char mond[12] = 
{ 31,    28,    31,    30,    31,    30,    31,    31,    30,    31,    30,    31 };

int build_number (void) {
	int m = 0; 
	int d = 0;
	int y = 0;
	static int b = 0;

	if (b != 0)
		return b;

	for (m = 0; m < 11; m++) {
		if (Q_stricmpn( &date[0], mon[m], 3 ) == 0)
			break;
		d += mond[m];
	}

	d += atoi( &date[4] ) - 1;
	y = atoi( &date[7] ) - 1900;
	b = d + (int)((y - 1) * 365.25);

	if (((y % 4) == 0) && m > 1)
		b += 1;

        b -= 37399 + 1; //27 May 2003 tf2003 first version


	return b;
}
