/*
 *  QWProgs-KTFFA
 *  Copyright (C) 2004  [sd] angel
 *
 *  This code is based on QuakeWorld KTFFA mod code by Cenobite and rc\sturm.
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
 *  $Id: vote.c,v 1.1 2004-11-14 07:04:08 AngelD Exp $
 */

// vote.q: mapchange voting functions
#include "g_local.h"
int		k_vote;
void NextLevel();

int CountPlayers()
{
	gedict_t	*p=world;
	int 		num = 0;

	while((p = find(p, FOFS(s.v.classname), "player"))) 
		if(p->s.v.netname[0]) 
			num++;

	return num;
}

void VoteThink()
{
	gedict_t* p=world;

	G_bprint(2, "The voting has timed out.\n");
	self->s.v.nextthink = -1;
	k_vote = 0;
        while((p = find(p, FOFS(s.v.classname), "player"))) 
        {
		if(p->s.v.netname[0] ) p->k_voted = 0;
	}
	dremove(self);
}

void VoteChange()
{
	int f1;
	gedict_t* voteguard;

	if(k_vote) return;

	f1 = CountPlayers();
	if(f1 == 1) {
		G_bprint(2, "%s changes map\n",self->s.v.netname);
		NextLevel();
		return;	// in case we get back here
	}

	
	G_bprint(2, "%s votes for mapchange\n",self->s.v.netname);

	if(f1/2 !=1)
		G_bprint(2, "%d‘ total votes needed\nType ",f1/2);
	else
		G_bprint(2, "%d‘ total vote needed\nType ",f1/2);

	G_bprint(3, "γνδ φοτε ωεσ");
	G_bprint(2, " in console to approve\n");

	k_vote = 1;
	self->k_voted = 1;
	voteguard = spawn(); // Check the 1 minute timeout for voting
	voteguard->s.v.owner = EDICT_TO_PROG(world);
	voteguard->s.v.classname  = "voteguard";
	voteguard->s.v.think = (func_t) VoteThink;
	voteguard->s.v.nextthink = g_globalvars.time + 30;
}

void VoteYes()
{
	int 		f1, f2;
	gedict_t* 	p = world;

	if(!k_vote) return;

	if(self->k_voted) {
		G_sprint(self, 2, "--- your vote is still good ---\n");
		return;
	}

// register the vote
	k_vote ++;
	G_bprint(2, "%s votes for mapchange\n",self->s.v.netname);
	f1 = CountPlayers();
	f2 = f1 / 2 + 1;
	if(k_vote >= f2) {
		G_bprint(3, "Map changed by majority vote\n");
		k_vote = 0;
        	while((p = find(p, FOFS(s.v.classname), "player"))) 
        	{
			if(p->s.v.netname[0] ) p->k_voted = 0;
		}

		p = find(world, FOFS(s.v.classname), "voteguard");
		if(p) {
			p->s.v.classname  = "";
		 dremove(p);
		}
		NextLevel();
		return;
	}

// calculate how many more votes are needed
	self->k_voted = 1;
	f1 = f2 - k_vote;
	if(f1>1)
		G_bprint(2, "%d‘ more votes needed\n",f1);
	else
		G_bprint(2, "%d‘ more vote needed\n",f1);	
}

void VoteNo()
{
	gedict_t* p;
	int f1, f2;

// withdraw one's vote
	if(!k_vote || !self->k_voted) return;

	G_bprint(2, "%s would rather play this map\n",self->s.v.netname);
	self->k_voted = 0;
	k_vote--;
	if(!k_vote) {
		G_bprint(3, "Voting is closed\n");
		p = find(world, FOFS(s.v.classname), "voteguard");
		if(p) 
		{
			p->s.v.classname  = "";
		 	dremove(p);
		}
		return;
	}
	f1 = CountPlayers();
	f2 = (f1 / 2) + 1;
	f1 = f2 - k_vote;
	if(f1 > 1)
		G_bprint(2, "%d‘ more votes needed\n",f1);
	else
		G_bprint(2, "%d‘ more vote needed\n",f1);
}
void Vote_Cmd()
{
        char    cmd_command[50];
        int argc;

	if ( tf_data.cb_prematch_time > g_globalvars.time )
	{
		return;
	}

	argc = trap_CmdArgc();
	if( argc != 2 )
	{
	        G_sprint(self, 2, "Usage:\n"
	        		  "start vote: γνδ φοτε γθαξηεναπ\n"
	        		  "vote yes  : γνδ φοτε ωεσ\n"
	        		  "vote no   : γνδ φοτε ξο\n"
	        		  );
		return;
	}

	trap_CmdArgv( 1, cmd_command, sizeof( cmd_command ) );
	if(!strcmp(cmd_command,"changemap"))
	{
	        VoteChange();
		return;
	}

	if(!strcmp(cmd_command,"yes"))
	{
	        VoteYes();
		return;
	}
	
	if(!strcmp(cmd_command,"no"))
	{
	        VoteNo();
		return;
	}
}
