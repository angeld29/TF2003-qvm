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
 *  $Id: qw.c,v 1.8 2005-06-05 05:10:41 AngelD Exp $
 */
#include "g_local.h"

int CL_KeyState( int key )
{
	return ( self->keys & key ) != 0;
}

void Bot_CL_KeyMove(  )
{
	int     forwardmove = 0, sidemove = 0, upmove = 0;
	int     buttons = 0;

	buttons = ( ( self->s.v.button0 ) ? 1 : 0 ) + ( ( self->s.v.button2 ) ? 2 : 0 );

	sidemove += self->maxstrafespeed * CL_KeyState( KEY_MOVERIGHT );
	sidemove -= self->maxstrafespeed * CL_KeyState( KEY_MOVELEFT );

	upmove += 200 * CL_KeyState( KEY_MOVEUP );
	upmove -= 200 * CL_KeyState( KEY_MOVEDOWN );

	forwardmove += self->maxfbspeed * CL_KeyState( KEY_MOVEFORWARD );
	forwardmove -= self->maxfbspeed * CL_KeyState( KEY_MOVEBACK );

	if ( self->s.v.v_angle[0] > 80 )
		self->s.v.v_angle[0] = 80;
	else
	{
		if ( self->s.v.v_angle[0] < -70 )
			self->s.v.v_angle[0] = -70;
	}

	//self->s.v.v_angle[2] = 0;
	self->s.v.v_angle[1] = anglemod( self->s.v.v_angle[1] );

	trap_SetBotCMD( NUM_FOR_EDICT( self ), bot_frametime * 1000,
			PASSVEC3(self->s.v.v_angle),
			forwardmove, sidemove, upmove, buttons, self->s.v.impulse );
}
void botFrame( void )
{
    gedict_t *te, *oself;

    bot_frametime = g_globalvars.frametime;
    oself = self;
    for ( te = world; ( te = trap_find( te, FOFS( s.v.classname ), "player" ) ); )
    {
        if ( te->has_disconnected )
            continue;

        if ( !te->isBot )
            continue;

        self = te;
        if( !tf_data.enable_bot )
        {
            botDisconnect( self );
            continue;
        }
        self->old_button0 = self->s.v.button0;
        self->old_button2 = self->s.v.button2;
        self->old_keys = self->keys;

        self->s.v.button0 = 0;
        self->s.v.button2 = 0;
        self->s.v.impulse = 0;
        self->keys = 0;
        if( self->team_no && self->team_no != 1)
        {
            G_bprint(3,"%s: i'm dont know how to play for team 2\n",self->s.v.netname);
            botDisconnect(self);
            continue;
        }
        Bot_AI(  );
        Bot_CL_KeyMove(  );
    }
    self = oself;

}

void botConnect( int whichteam, int whichClass, char* name )
{
	gedict_t *oself;
	int     ednum;

	gedict_t *ent = &g_edicts[ednum = trap_AddBot( name, 0, 0, "base" )];

	if ( ent == world )
	{
		G_sprint( self, 2, "cannot add bot\n" );
		return;
	}

	oself = self;
	self = ent;
	self->bot_skin = whichClass;
	self->bot_team = whichteam;
	
	ClearAllWaypoints(  );
	ClearAllTargets(  );
	self->action = BOT_RESUPPLYING;
	self->obs_time = 0;
	self->oldAction = BOT_IDLE;

	self = oself;

}

void botDisconnect( gedict_t * te )
{
	gedict_t *oself;

	if ( !te->isBot )
	{
		G_bprint( 2, "WARNING!!! Trying to disconnect non bot client\n" );
		return;
	}
	oself = self;
	self = te;
	trap_RemoveBot( NUM_FOR_EDICT( te ) );
	self = oself;
}

void botPutInServer()
{
    ClearAllWaypoints( );
    ClearAllTargets( );
    self->obs_time = 0;
    self->action = BOT_IDLE;
    self->s.v.button0 = 0;
    self->s.v.button1 = 0;
    self->s.v.button2 = 0;
    self->keys = 0;
    self->fBotMessageTime = 0;
}
