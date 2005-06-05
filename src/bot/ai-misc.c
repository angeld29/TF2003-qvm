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
 *  $Id: ai-misc.c,v 1.3 2005-06-05 05:10:41 AngelD Exp $
 */
#include "g_local.h"

qboolean CheckForResupply()
{
	if ( self->action & ( BOT_FIGHTING + BOT_AVOIDING_HAZARD ) )
		return false;
	if ( self->s.v.health < self->s.v.max_health
	     || self->s.v.armorvalue < self->maxarmor )
		return true;
	return false;
}

void GoForResupply()
{
	if ( self->action == BOT_FIGHTING )
		return;
	self->action = BOT_RESUPPLYING;
	ClearAllTargets();
	ClearAllWaypoints();
	self->s.v.button1 = 0;
	self->s.v.button2 = 0;
}

char   *PickAName()
{
	return "Padla-Bot";
}

int BotImpulseMenu()
{
	int     subItem = 0;

	subItem = self->nextimpulse;
	self->nextimpulse = 0;
	switch ( self->current_menu )
	{
	case MENU_TEAM:
	        if( self->bot_lastmenu == MENU_TEAM )
	        {
	                if( self->bot_lastmenucount++ > 5) 
	                {
	                        G_bprint(2,"%s :cannot join team %d\n",self->s.v.netname, self->bot_team);
	                        botDisconnect( self );
	                        return 0;
	                }
	        }else
	        {
	                self->bot_lastmenu = MENU_TEAM;
	                self->bot_lastmenucount = 1;
	        }
		return self->bot_team;
	case MENU_CLASS:
	        if( self->bot_lastmenu == MENU_CLASS )
	        {
	                if( self->bot_lastmenucount++ > 5) 
	                {
	                        G_bprint(2,"%s :cannot select class %d\n",self->s.v.netname, self->bot_skin);
	                        botDisconnect( self );
	                        return 0;
	                }
	        }else
	        {
	                self->bot_lastmenu = MENU_CLASS;
	                self->bot_lastmenucount = 1;
	        }
		return self->bot_skin;
	case MENU_SPY:
		return 4;
	case MENU_SPY_SKIN:
		return 8;
	case MENU_SPY_COLOR:
		return self->bot_team;
	case MENU_ENGINEER:
		return ( subItem ) ? subItem : 4;
	case MENU_ENGINEER_FIX_SENTRYGUN:
		return ( subItem ) ? ( self->s.v.button0 = 0, subItem ) : 5;
	case MENU_DISPENSER:
		return 3;
	default:
		return self->s.v.impulse;
	}
}

int BotImpulse()
{
	int     Temp;

	if ( self->nextimpulse )
	{
		Temp = self->nextimpulse;
		self->nextimpulse = 0;
		return Temp;

	}
        return self->s.v.impulse;
}
