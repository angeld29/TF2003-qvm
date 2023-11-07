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
 *  $Id: ctf.c,v 1.3 2004-12-23 03:16:15 AngelD Exp $
 */
#include "g_local.h"

void TeamFortress_CTF_FlagInfo(  )
{
	gedict_t *te;
	int flag_time;

	te = Finditem( 1 );
	if ( te->goal_state == 1 )
	{
		if ( self == PROG_TO_EDICT( te->s.v.owner ) )
			G_sprint( self, 10, "You have the enemy flag.\n" );
		else
		{
			G_sprint( self, 10, "%s has", PROG_TO_EDICT( te->s.v.owner )->s.v.netname );
			if ( self->team_no == 1 )
				G_sprint( self, 10, " your flag.\n" );
			else
				G_sprint( self, 10, " the enemy flag.\n" );
		}
	} else
	{
		if ( !VectorCompare( te->s.v.origin, te->s.v.oldorigin ) )
		{
			if ( self->team_no == 1 )
				G_sprint( self, 10, "Your flag is lying about. " );
			else
				G_sprint( self, 10, "The enemy flag is lying about. " );
			if ( tfset(flag_timer) )
			{
				flag_time = FlagTimeLeft(te);
				G_sprint( self, 10, " :%3d\n", flag_time );
			} else {
				G_sprint( self, 10, "\n" );
			}
		} else
		{
			if ( self->team_no == 1 )
				G_sprint( self, 10, "Your flag is in your base.\n" );
			else
				G_sprint( self, 10, "The enemy flag is in their base.\n" );
		}
	}
	te = Finditem( 2 );
	if ( te->goal_state == 1 )
	{
		if ( self == PROG_TO_EDICT( te->s.v.owner ) )
			G_sprint( self, 10, "You have the enemy flag.\n" );
		else
		{
			G_sprint( self, 10, "%s has", PROG_TO_EDICT( te->s.v.owner )->s.v.netname );
			if ( self->team_no == 2 )
				G_sprint( self, 10, " your flag.\n" );
			else
				G_sprint( self, 10, " the enemy flag.\n" );
		}
	} else
	{
		if ( !VectorCompare( te->s.v.origin, te->s.v.oldorigin ) )
		{
			if ( self->team_no == 2 )
				G_sprint( self, 10, "Your flag is lying about.\n" );
			else
				G_sprint( self, 10, "The enemy flag is lying about.\n" );
			if ( tfset(flag_timer) )
			{
				flag_time = FlagTimeLeft(te);
				G_sprint( self, 10, " :%3d\n", flag_time );
			} else {
				G_sprint( self, 10, "\n" );
			}
		} else
		{
			if ( self->team_no == 2 )
				G_sprint( self, 10, "Your flag is in your base.\n" );
			else
				G_sprint( self, 10, "The enemy flag is in their base.\n" );
		}
	}
}
