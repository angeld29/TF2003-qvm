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
 *  $Id: menu.h,v 1.5 2004-12-23 03:16:14 AngelD Exp $
 */
#define MENU_REFRESH_RATE 			25
typedef enum {
	MENU_NULL,
	MENU_DEFAULT,			//1  
	MENU_TEAM,			//2  
	MENU_CLASS,			//3  
	MENU_DROP,		        //4
	MENU_INTRO,		        //5
	MENU_CLASSHELP,		        //6
	MENU_CLASSHELP2,	        //7
	MENU_REPEATHELP,	        //8
	MENU_PICKBINDS,		        //9
	MENU_SHOWBINDS1,	        //10
	MENU_SHOWBINDS2,	        //11
	MENU_SPY,		        //12
	MENU_SPY_SKIN,		        //13
	MENU_SPY_COLOR,		        //14
	MENU_ENGINEER,		        //15
	MENU_ENGINEER_FIX_DISPENSER,	//16 
	MENU_ENGINEER_FIX_SENTRYGUN,	//17 
	MENU_ENGINEER_FIX_MORTAR,	//18 
	MENU_DISPENSER,			//19 
	MENU_CHANGECLASS,		//20
	MENU_BIRTHDAY1 = 23,		
	MENU_BIRTHDAY2 = 24,		
	MENU_BIRTHDAY3 = 25,
	TG_MENU_MAIN,
	TG_MENU_SG,
	TG_MENU_DETPACK,
	TG_MENU_CHEATS,
	TG_MENU_POSITION
} menunum_t;

