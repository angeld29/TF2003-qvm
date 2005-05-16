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
 *  $Id: tforttm.c,v 1.14 2005-05-16 09:35:46 AngelD Exp $
 */
#include "g_local.h"

int     teamscores[5];
int     teamfrags[5];
int     teamlives[5];
int     teammaxplayers[5];
float   teamadvantage[5];
int     illegalclasses[5];
int     civilianteams;

int TeamFortress_TeamPutPlayerInTeam(  )
{
	int     i, j, lowest, likely_team;

	likely_team = (int)( g_random(  ) * number_of_teams ) + 1;

	if ( !likely_team )
		likely_team = number_of_teams;

	lowest = 33;

	for ( i = 1; i < number_of_teams + 1; i++ )
	{
		j = TeamFortress_TeamGetNoPlayers( i );
		if ( j < lowest )
		{
			if ( TeamFortress_TeamGetMaxPlayers( i ) > j )
			{
				lowest = j;
				likely_team = i;
			}
		}
	}
	return TeamFortress_TeamSet( likely_team );
}

static const int     team_colors[5] = { 0, 14, 5, 13, 12 };
int           team_top_colors[5] = { 0, 13, 4, 12, 11 };

int TeamFortress_TeamGetColor( int tno )
{
	if ( tno < 5 )
		return team_colors[tno];
	else
		return 0;
}

int TeamFortress_TeamGetTopColor( int tno )
{
	if ( tno < 5 )
		return team_top_colors[tno];
	else
		return 0;
}

static char    team_names[4][10] = { "blue", "red", "yell", "gren" };

void LoadTeamNames(  )
{
	GetSVInfokeyString( "team1", "t1", team_names[0], 10, "blue" );
	GetSVInfokeyString( "team2", "t2", team_names[1], 10, "red" );
	GetSVInfokeyString( "team3", "t3", team_names[2], 10, "yell" );
	GetSVInfokeyString( "team4", "t4", team_names[3], 10, "gren" );
}

char   *GetTeamName( int tno )
{
	LoadTeamNames();
	if ( tno > 0 && tno <= 4 )
		return team_names[tno - 1];
	else
		return "\"\"";
}

void SetTeamName( gedict_t * p )
{
        if ( p->isBot )
        {
                if( p->team_no >0 && p->team_no <=4)
                        trap_SetBotUserInfo(NUM_FOR_EDICT( p ),"team",team_names[p->team_no-1 ]);
                else
                        trap_SetBotUserInfo(NUM_FOR_EDICT( p ),"team","");
        }else
	       stuffcmd( p, "team %s\n", GetTeamName( p->team_no ) );
}


int TeamFortress_TeamSet( int tno )
{
	int     tc;

	if ( teamplay < 1 )
	{
		G_sprint( self, 2, "Teamplay is not On, so FortressTeams are inactive.\n" );
		return 0;
	}
	if ( tno > number_of_teams && number_of_teams )
	{
		G_sprint( self, 2, "There can be only %d teams on this map.\nTry again\n", number_of_teams );
		return 0;
	}
	if ( self->team_no > 0 )
	{
		G_sprint( self, 2, "You're already in Team No %d.\n", self->team_no );
		return 0;
	}
	tc = TeamFortress_TeamGetNoPlayers( tno );
	if ( tc >= TeamFortress_TeamGetMaxPlayers( tno ) )
	{
		G_sprint( self, 2, "That team is full. Pick another.\n" );
		return 0;
	}
	if ( !TeamFortress_TeamGetColor( tno ) )
	{
		//TeamFortress_TeamSetColor( tno );
		if ( !TeamFortress_TeamGetColor( tno ) )
		{
			G_sprint( self, 2, "You can't start a new team with your color, since another " );
			G_sprint( self, 2, "already using that color. Change your pants color, then try again.\n" );
			return 0;
		}
		G_bprint( 2, "%s has started Team No %d.\n", self->s.v.netname, tno );

		self->immune_to_check = g_globalvars.time + 10;
		if ( ( tf_data.toggleflags & 128 ) || ( tf_data.toggleflags & 2048 ) )
			self->s.v.frags = TeamFortress_TeamGetScore( tno );
		TeamFortress_SetColor( self, TeamFortress_TeamGetTopColor( tno ), 
		                      TeamFortress_TeamGetColor( tno ) - 1 );


		self->team_no = tno;
		self->lives = TeamFortress_TeamGetLives( tno );
		SetTeamName( self );
		if ( !self->playerclass )
		{
			if ( TeamFortress_TeamIsCivilian( self->team_no ) )
			{
				self->s.v.impulse = 1;
				TeamFortress_ChangeClass(  );
			}
		}
		return 1;
	}
	G_bprint( 2, "%s has joined Team No %d.\n", self->s.v.netname, tno );
	TeamFortress_SetColor( self, TeamFortress_TeamGetTopColor( tno ), TeamFortress_TeamGetColor( tno ) - 1 );

	self->team_no = tno;
	self->immune_to_check = g_globalvars.time + 10;
	self->lives = TeamFortress_TeamGetLives( tno );
	if ( ( tf_data.toggleflags & 128 ) || ( tf_data.toggleflags & 2048 ) )
		self->s.v.frags = TeamFortress_TeamGetScore( tno );

	TeamFortress_TeamShowMemberClasses( self );
	SetTeamName( self );
	if ( !self->playerclass )
	{
		if ( TeamFortress_TeamIsCivilian( self->team_no ) )
		{
			self->s.v.impulse = 1;
			TeamFortress_ChangeClass(  );
		}
	}
	return 1;
}

void TeamFortress_CheckTeamCheats(  )
{
	char    st[20], *sk;
	int     tc;

	if ( self->immune_to_check > g_globalvars.time )
		return;
	if ( self->s.v.deadflag )
		return;
	if ( !( self->s.v.netname[0] ) )
		KickCheater( self );
	else
	{
		if ( self->playerclass && !self->team_no && teamplay > 0 )
			KickCheater( self );
	}
	if ( self->team_no > 0 && teamplay > 0 )
	{

		tc = GetInfokeyInt( self, "bottomcolor", NULL, 0 );

		if ( self->playerclass == PC_SPY && self->undercover_team )
		{
			if ( TeamFortress_TeamGetColor( self->undercover_team ) - 1 != tc )
			{
			        TeamFortress_SetColor( self,
					  TeamFortress_TeamGetTopColor( self->undercover_team ),
					  TeamFortress_TeamGetColor( self->undercover_team ) - 1 );

				G_bprint( 1, "%s has been kicked for changing color.\n", self->s.v.netname );
				G_sprint( self, 2,
					  "You have been kicked for changing your pants color. Don't do it.\n" );
				KickCheater( self );
				return;
			}
		} else
		{
			if ( tc != TeamFortress_TeamGetColor( self->team_no ) - 1 )
			{
				TeamFortress_SetColor( self,
					  TeamFortress_TeamGetTopColor( self->team_no ),
					  TeamFortress_TeamGetColor( self->team_no ) - 1 );
				G_bprint( 1, "%s has been kicked for changing color.\n", self->s.v.netname );
				G_sprint( self, 2,
					  "You have been kicked for changing your pants color. Don't do it.\n" );
				KickCheater( self );
				return;
			}
		}
		if ( tf_data.topcolor_check )
		{
			tc = GetInfokeyInt( self, "topcolor", NULL, 0 );
			if ( self->playerclass == PC_SPY && self->undercover_team )
			{
				if ( TeamFortress_TeamGetTopColor( self->undercover_team ) != tc )
				{
				        TeamFortress_SetColor( self, 
						  TeamFortress_TeamGetTopColor( self->undercover_team ),
						  TeamFortress_TeamGetColor( self->undercover_team ) - 1 );

					G_bprint( 1, "%s has been kicked for changing color.\n", self->s.v.netname );
					G_sprint( self, 2,
						  "You have been kicked for changing your top color. Don't do it.\n" );
					KickCheater( self );
					return;
				}
			} else
			{
				if ( tc != TeamFortress_TeamGetTopColor( self->team_no ) )
				{
					TeamFortress_SetColor( self, 
						  TeamFortress_TeamGetTopColor( self->undercover_team ),
						  TeamFortress_TeamGetColor( self->undercover_team ) - 1 );

					G_bprint( 1, "%s has been kicked for changing color.\n", self->s.v.netname );
					G_sprint( self, 2,
						  "You have been kicked for changing your top color. Don't do it.\n" );
					KickCheater( self );
					return;
				}
			}
		}
		if ( self->playerclass )
		{
			GetInfokeyString( self, "skin", NULL, st, sizeof( st ), "" );
			tc = 0;
			sk = TeamFortress_GetSkin( self );
			if ( strneq( st, sk ) )
			{
				TeamFortress_SetSkin( self );
				G_bprint( 1, "%s has been kicked for changing skin.\n", self->s.v.netname );
				G_sprint( self, 2, "You have been kicked for changing your skin. Don't do it.\n" );
				KickCheater( self );
			}
			if ( tc == 8 )
				self->playerclass = 8;
		}
		sk = GetTeamName( self->team_no );
		GetInfokeyString( self, "team", NULL, st, sizeof( st ), "" );
		if ( strneq( st, sk ) )
		{
			SetTeamName( self );
			G_bprint( 1, " has been kicked for changing team.\n", self->s.v.netname );
			G_sprint( self, 2, "You have been kicked for changing your team. Don't do it.\n" );
			KickCheater( self );
			return;
		}
	}

}


int TeamFortress_TeamGetScore( int tno )
{
	if ( tno <= 0 || tno > 4 )
		return 0;

	return teamscores[tno];
}

void TeamFortress_TeamIncreaseScore( int tno, int scoretoadd )
{
	gedict_t *e;

	if ( !tno || !scoretoadd || tno > 4 )
		return;
	teamscores[tno] += scoretoadd;

	if ( ( tf_data.toggleflags & 128 ) || ( tf_data.toggleflags & 2048 ) )
	{
		
		for ( e = world; ( e = trap_find( e, FOFS( s.v.classname ), "player" ) ) ; )
		{
			if ( e->team_no == tno )
				e->s.v.frags = TeamFortress_TeamGetScore( tno );
		}
	}
}


int TeamFortress_TeamGetScoreFrags( int tno )
{
	if ( tno <= 0 || tno > 4 )
		return 0;

	if ( ( tf_data.toggleflags & 128 ) || ( tf_data.toggleflags & 2048 ) )
		return teamscores[tno];
	else
		return teamfrags[tno];

	return 0;
}

int TeamFortress_TeamGetLives( int tno )
{
	if ( tno <= 0 || tno > 4 )
		return 0;

	return teamlives[tno];
}

int TeamFortress_TeamGetNoPlayers( int tno )
{
	int     size_team = 0;
	gedict_t *search;

	for ( search = world; ( search = trap_find( search, FOFS( s.v.classname ), "player" ) ); )
	{
		if ( search->team_no == tno )
			size_team++;
	}
	return size_team;
}

int TeamFortress_GetNoPlayers(  )
{
	int     nump = 0;
	gedict_t *search;

	for ( search = world;( search = trap_find( search, FOFS( s.v.classname ), "player" ) ) ; )
		nump++;
	return nump;
}


int TeamFortress_TeamGetMaxPlayers( int tno )
{
	if ( tno <= 0 || tno > 4 )
		return 0;
	return teammaxplayers[tno];
}


int TeamFortress_TeamGetWinner(  )
{
	int     i, j, highest = 0, highestteam = 0;

	for ( i = 1; i < number_of_teams + 1; i++ )
	{
		j = TeamFortress_TeamGetScoreFrags( i );
		if ( j > highest || highestteam == 0 )
		{
			highest = j;
			highestteam = i;
		}
	}
	return highestteam;
}

int TeamFortress_TeamGetSecond(  )
{
	int     i, j, highest, highestteam, secondteam = 0, second = 0;

	highestteam = TeamFortress_TeamGetWinner(  );

	highest = TeamFortress_TeamGetScoreFrags( highestteam );

	for ( i = 1; i < number_of_teams + 1; i++ )
	{
		j = TeamFortress_TeamGetScoreFrags( i );
		if ( j <= highest && ( j > second || secondteam == 0 ) && i != highestteam )
		{
			second = j;
			secondteam = i;
		}
	}
	return secondteam;
}



void TeamFortress_TeamShowScores( int all )
{
	int     i;

	if ( all == 2 )
	{
		for ( i = 1; i <= number_of_teams; i++ )
		{
			if ( TeamFortress_TeamGetColor( i ) > 0 )
			{
				G_bprint( 2, "%s: %d ", TeamFortress_TeamGetColorString( i ),
					  TeamFortress_TeamGetScore( i ) );

			}
		}
		G_bprint( 2, "\n" );
		return;
	}
	for ( i = 1; i <= number_of_teams; i++ )
	{
		if ( TeamFortress_TeamGetColor( i ) > 0 )
		{
			if ( all )
				G_bprint( 2, "Team %d (%s) : %d\n", i,
					  TeamFortress_TeamGetColorString( i ), TeamFortress_TeamGetScore( i ) );
			else
				G_sprint( self, 2, "Team %d (%s) : %d\n", i,
					  TeamFortress_TeamGetColorString( i ), TeamFortress_TeamGetScore( i ) );
		}
	}
}

//////////////////////
const char   *colornames[15] = { "DarkBlue",
	"White",
	"Brown",
	"Blue",
	"Green",
	"Red",
	"Tan",
	"Pink",
	"Orange",
	"Purple",
	"DarkPurple",
	"Grey",
	"DarkGreen",
	"Yellow",
	"DarkBlue"
};

const char   *TeamFortress_TeamGetColorString( int tno )
{
	int     col;

	col = TeamFortress_TeamGetColor( tno );
	if ( col > 14 )
		col = 0;
	return colornames[col];
}

void TeamFortress_TeamShowMemberClasses( gedict_t * Player )
{
	gedict_t *e;
	int     found = 0;

	found = 0;

	for ( e = world; ( e = trap_find( e, FOFS( s.v.classname ), "player" ) ); )
	{
		if ( ( e->team_no == Player->team_no || !e->team_no ) && e != Player )
		{
			if ( e->s.v.model && e->s.v.model[0] )
			{
				if ( !found )
				{
					found = 1;
					G_sprint( Player, 2, "The other members of your team are:\n" );
				}
				G_sprint( Player, 2, "%s : ", e->s.v.netname );
				TeamFortress_PrintClassName( Player, e->playerclass, e->tfstate & 8 );
			}
		}
	}
	if ( !found )
		G_sprint( Player, 2, "There are no other players on your team.\n" );
}

void TeamFortress_TeamShowMemberClasses_New( gedict_t * Player )
{
	int		stf;
	gedict_t*	e,*te;
	int	found = 0;
	stf = GetInfokeyInt( self, "s", NULL, 0 );
	for(e= world; (e = trap_find( e, FOFS( s.v.classname ), "player" )); )
	{
		if ( ( e->team_no == Player->team_no || !e->team_no ) && e != Player )
		{
			if ( e->s.v.model && e->s.v.model[0] )
			{
				if ( !found )
				{
					found = 1;
					G_sprint( Player, 2, "The other members of your team are:\n" );
				}
				G_sprint( Player, 2, "%s : %s", e->s.v.netname, TeamFortress_GetClassName(e->playerclass) );

				if ( e->tfstate & TFSTATE_RANDOMPC)
					G_sprint(Player, 2, " (Random)");
				if( !e->playerclass)
				{
					G_sprint(Player, 2, "\n");
					continue;
				}
				if((stf & TF_STATUS_STATS_MASK) ==0)
				{

					if(e->s.v.armortype == (float) 0.8)
						G_sprint(Player, 2, " R");
					else if(e->s.v.armortype == (float) 0.6)
						G_sprint(Player, 2, " Y");
					else if(e->s.v.armortype == (float) 0.3)
						G_sprint(Player, 2, " G");
					else 
						G_sprint(Player, 2, "  ");

					G_sprint(Player, 2, "%.0f/%.0f",e->s.v.armorvalue,e->s.v.health);

					if((e->playerclass == PC_SPY) && ((stf & TF_STATUS_SPY_MASK) ==0))
					{
						G_sprint(Player, 2, " %s %s", GetTeamName(e->undercover_team),TeamFortress_GetClassName(e->undercover_skin) );
					}

					if((e->playerclass == PC_DEMOMAN) && ((stf & TF_STATUS_DETPACK_MASK) !=0))
					{
						for(te= world; (te = trap_find( te, FOFS( s.v.classname ), "detpack" )); )
						{
							if (te->s.v.owner == EDICT_TO_PROG(e))
							{
								G_sprint(Player, 2, " DTP:%.0f", te->s.v.nextthink - g_globalvars.time);
								break;
							}
						}
					}
					if((e->playerclass == PC_ENGINEER) && ((stf & TF_STATUS_SENTRY_MASK) !=0))
					{

						if( e->has_sentry)
						{
							for(te= world; (te = trap_find( te, FOFS( s.v.classname ), "building_sentrygun" )); )
							{
								if (te->real_owner == e)
								{
									G_sprint(Player, 2, " SENTRY:%.0f", te->s.v.weapon );
									break;
								}
							}
						}else
						{
							G_sprint(Player, 2, " NOSENTRY");
						}
					}
					if((stf & TF_STATUS_COORD_MASK) !=0)
						G_sprint(Player, 2, " '%3.0f %3.0f %3.0f'",PASSVEC3(e->s.v.origin));
					G_sprint(Player, 2, "\n");
				}
			}
		}
	
	}
	if ( !found )
		G_sprint( Player, 2, "There are no other players on your team.\n" );
}

void CalculateTeamEqualiser(  )
{
	float   t1, t2, t3, t4;
	float   t_ave;
	float   calc;

	teamadvantage[0] = teamadvantage[1] = teamadvantage[2] = teamadvantage[3] = teamadvantage[4] = 1;

	if ( number_of_teams < 2 )
	{
		self->s.v.nextthink = g_globalvars.time + 60;
		return;
	}

	if ( teamplay & 32 )
	{
		t1 = TeamFortress_TeamGetNoPlayers( 1 );
		t2 = TeamFortress_TeamGetNoPlayers( 2 );
		t3 = TeamFortress_TeamGetNoPlayers( 3 );
		t4 = TeamFortress_TeamGetNoPlayers( 4 );
		if ( t1 + t2 + t3 + t4 < 1 )
		{
			self->s.v.nextthink = g_globalvars.time + 30;
			return;
		}
		t_ave = ( t1 + t2 + t3 + t4 ) / number_of_teams;
		if ( t1 > 0 )
		{
			calc = t_ave / t1 - 1;
			if ( calc )
				calc = calc / 3 + 1;
			else
				calc = calc + 1;
			teamadvantage[1] = calc;
		}
		if ( t2 > 0 && number_of_teams >= 2 )
		{
			calc = t_ave / t2 - 1;
			if ( calc )
				calc = calc / 3 + 1;
			else
				calc = calc + 1;
			teamadvantage[2] = calc;
		}
		if ( t3 > 0 && number_of_teams >= 3 )
		{
			calc = t_ave / t3 - 1;
			if ( calc )
				calc = calc / 3 + 1;
			else
				calc = calc + 1;
			teamadvantage[3] = calc;
		}
		if ( t4 > 0 && number_of_teams >= 4 )
		{
			calc = t_ave / t4 - 1;
			if ( calc )
				calc = calc / 3 + 1;
			else
				calc = calc + 1;
			teamadvantage[4] = calc;
		}
	}
	if ( teamplay & 64 )
	{
		t1 = teamscores[0] + 60;
		if ( number_of_teams >= 2 )
			t2 = teamscores[2] + 60;
		else
			t2 = 0;
		if ( number_of_teams >= 3 )
			t3 = teamscores[3] + 60;
		else
			t3 = 0;
		if ( number_of_teams >= 4 )
			t4 = teamscores[4] + 60;
		else
			t4 = 0;
		t_ave = ( t2 + t3 + t4 ) / ( number_of_teams - 1 );
		calc = t_ave / t1 - 1;
		if ( calc )
			calc = calc / 3 + 1;
		else
			calc = calc + 1;
		teamadvantage[0] *= calc;
		if ( number_of_teams >= 2 )
		{
			t_ave = ( t1 + t3 + t4 ) / ( number_of_teams - 1 );
			calc = t_ave / t2 - 1;
			if ( calc )
				calc = calc / 3 + 1;
			else
				calc = calc + 1;
			teamadvantage[2] *= calc;
		}
		if ( number_of_teams >= 3 )
		{
			t_ave = ( t2 + t1 + t4 ) / ( number_of_teams - 1 );
			calc = t_ave / t3 - 1;
			if ( calc )
				calc = calc / 3 + 1;
			else
				calc = calc + 1;
			teamadvantage[3] *= calc;
		}
		if ( number_of_teams >= 4 )
		{
			t_ave = ( t2 + t3 + t1 ) / ( number_of_teams - 1 );
			calc = t_ave / t4 - 1;
			if ( calc )
				calc = calc / 3 + 1;
			else
				calc = calc + 1;
			teamadvantage[4] *= calc;
		}
	}
	self->s.v.nextthink = g_globalvars.time + 10;
}

void SetupTeamEqualiser(  )
{
	gedict_t *TE;

	teamadvantage[0] = teamadvantage[1] = teamadvantage[2] = teamadvantage[3] = teamadvantage[4] = 1;
	if ( !( teamplay & ( 32 | 64 ) ) )
		return;
	TE = spawn(  );
	TE->s.v.classname = "Team Equaliser";
	TE->s.v.think = ( func_t ) CalculateTeamEqualiser;
	TE->s.v.nextthink = g_globalvars.time + 30;
}

float TeamEqualiseDamage( gedict_t * targ, gedict_t * attacker, float damage )
{
	float   adv = 0;
	float   newdam;

	if ( strneq( targ->s.v.classname, "player" ) || strneq( attacker->s.v.classname, "player" ) )
		return damage;
	if ( !targ->team_no || !attacker->team_no || attacker->team_no == targ->team_no )
		return damage;

	if ( attacker->team_no > 0 && attacker->team_no < 4 )
		adv = teamadvantage[attacker->team_no];

	if ( !adv )
	{
		G_dprint( "There is a team with an advantage of 0\n" );
		adv = 0.1;
	}
	newdam = damage * adv;
	if ( targ->team_no > 0 && targ->team_no < 4 )
		adv = teamadvantage[targ->team_no];

	if ( !adv )
	{
		G_dprint( "There is a team with an advantage of 0\n" );
		adv = 0.1;
	}
	newdam = newdam * 1 / adv;
	return newdam;
}

void TeamFortress_StatusQuery(  )
{

	G_sprint( self, 2, "players per team: %d %d %d %d\n",
		  TeamFortress_TeamGetNoPlayers( 1 ),
		  TeamFortress_TeamGetNoPlayers( 2 ),
		  TeamFortress_TeamGetNoPlayers( 3 ), TeamFortress_TeamGetNoPlayers( 4 ) );

	G_sprint( self, 2, "    equalisation: %.0f %.0f %.0f %.0f\n",
		  teamadvantage[1], teamadvantage[2], teamadvantage[3], teamadvantage[4] );

	G_sprint( self, 2, "Teamplay is %d\n", teamplay );
}


int TeamFortress_TeamGetIllegalClasses( int tno )
{
	if ( tno <= 0 || tno > 4 )
		return 0;
	return illegalclasses[tno];
}



qboolean TeamFortress_TeamIsCivilian( int tno )
{
	if ( tno <= 0 || tno > 4 )
		return 0;

	return ( civilianteams & ( 1 << ( tno - 1 ) ) ) != 0;
}

//////////////
const static char   *li_classrestricted[10][2] = {
	{"cr_sc", "cr_scout"},
	{"cr_sn", "cr_sniper"},
	{"cr_so", "cr_soldier"},
	{"cr_de", "cr_demoman"},
	{"cr_me", "cr_medic"},
	{"cr_hw", "cr_hwguy"},
	{"cr_py", "cr_pyro"},
	{"cr_sp", "cr_spy"},
	{"cr_en", "cr_engineer"},
	{"cr_ra", "cr_random"}
};
int ClassIsRestricted( int tno, int pc )
{
	int     max, num = 0;
	gedict_t *te;

	if ( pc <= 0 || pc > 10 )
		return 0;
	if ( !tno )
		return 0;

	max = GetSVInfokeyInt( li_classrestricted[pc-1][0], li_classrestricted[pc-1][1], 0 );

	if ( max > 0 )
	{
	        for ( te = world; ( te = trap_find( te, FOFS( s.v.classname ), "player" ) ); )
		{
			if ( te->team_no == tno )
			{
				if ( pc == 10 )
				{
					if ( te->tfstate & TFSTATE_RANDOMPC )
						num++;
				} else
				{
					if ( te->playerclass == pc || te->nextpc == pc )
					{
						if ( !( te->tfstate & TFSTATE_RANDOMPC ) )
							num++;
					}
				}
			}
		}
		if ( num >= max )
			return 1;
	}
	if ( max == -1 )
		return 1;
	return 0;
}

void teamsprint( int tno, gedict_t * ignore, char *st )
{
	gedict_t *te;

	if ( !tno )
		return;

	for ( te = world;( te = trap_find( te, FOFS( s.v.classname ), "player" ) ) ; )
	{
		if ( te->team_no == tno && te != ignore )
			G_sprint( te, 2, st );
	}
}

int teams_allied = 0;
const static int team_allied_bitindex[4][4] = 
{
	{  0,  1,  2,  4},
	{  1,  0,  8, 16},
	{  2,  8,  0, 32},
	{  4, 16, 32,  0}
};
/*
    1  - team 1 & 2
    2  - team 1 & 3
    4  - team 1 & 4
    8  - team 2 & 3
   16  - team 2 & 4
   32  - team 3 & 4
*/
qboolean TeamFortress_isTeamsAllied( int teamno1, int teamno2 )
{
        if( teamno1 == teamno2 )
        	return true;
	
	if(!teamno1 || !teamno2 || teamno1 > 4 || teamno2 > 4 )
		   return false;
	return teams_allied & team_allied_bitindex[teamno1-1][teamno2-1];
}
