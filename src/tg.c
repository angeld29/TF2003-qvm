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
 *  $Id: tg.c,v 1.16 2006-03-04 13:01:15 AngelD Exp $
 */

#include "g_local.h"

tf_tg_server_data_t tg_data;
void Detpack_SetClip(  );

void TG_LoadSettings()
{
    char    st[10];

    GetSVInfokeyString( "sg_fire", NULL, st, sizeof( st ), "on" );
    if ( !strcmp( st, "off" ) )
        tg_data.sg_disable_fire = 1;
    else
        tg_data.sg_disable_fire = 0;

    GetSVInfokeyString( "sg_find", NULL, st, sizeof( st ), "" );
    if ( !strcmp( st, "all" ) )
        tg_data.sg_allow_find = TG_SG_FIND_IGNORE_OFF;
    else if( !strcmp( st, "self" ) )
        tg_data.sg_allow_find = TG_SG_FIND_IGNORE_OWNER;
    else if( !strcmp( st, "no" ) )
        tg_data.sg_allow_find = TG_SG_FIND_IGNORE_ALL;
    else
        tg_data.sg_allow_find = TG_SG_FIND_IGNORE_TEAM;

    /*       	GetSVInfokeyString( "sg_fire_type", NULL, st, sizeof( st ), "" );
                if ( !strcmp( st, "nodmg" ) )
                tg_data.sg_fire_type = TG_SG_FIRE_LIGHTING;
                else if( !strcmp( st, "bullets" ) )
                tg_data.sg_fire_type = TG_SG_FIRE_BULLETS;
                else
                tg_data.sg_fire_type = TG_SG_FIRE_NORMAL;*/
    GetSVInfokeyString( "sg_fire_bullets", NULL, st, sizeof( st ), "on" );
    if( !strcmp(st,"off") )
        tg_data.sg_fire_bullets = false;
    else
        tg_data.sg_fire_bullets = true;
    GetSVInfokeyString( "sg_fire_rockets", NULL, st, sizeof( st ), "on" );
    tg_data.sg_fire_rockets = true;
    if( !strcmp(st,"off") )
        tg_data.sg_fire_rockets = false;
    else
        tg_data.sg_fire_rockets = true;
    GetSVInfokeyString( "sg_fire_lighting", NULL, st, sizeof( st ), "off" );
    if( !strcmp(st,"off") )
        tg_data.sg_fire_lighting = false;
    else
        tg_data.sg_fire_lighting = true;
    GetSVInfokeyString( "sg_unlimit_ammo", NULL, st, sizeof( st ), "off" );
    if( !strcmp(st,"on") )
        tg_data.sg_unlimit_ammo = true;
    else
        tg_data.sg_unlimit_ammo = false;


    GetSVInfokeyString( "gren_eff", NULL, st, sizeof( st ), "" );
    if ( !strcmp( st, "off" ) )
        tg_data.gren_effect = TG_GREN_EFFECT_OFF;
    else if( !strcmp( st, "self" ) )
        tg_data.gren_effect = TG_GREN_EFFECT_OFF_FORSELF;
    else
        tg_data.gren_effect = TG_GREN_EFFECT_ON;

    GetSVInfokeyString( "gren_time", NULL, st, sizeof( st ), "" );
    if ( !strcmp( st, "10" ) )
        tg_data.gren_time = 10;
    else if( !strcmp( st, "5" ) )
        tg_data.gren_time = 5;
    else
        tg_data.gren_time = 0;


    GetSVInfokeyString( "god", NULL, st, sizeof( st ), "off" );
    if ( !strcmp( st, "on" ) )
        tg_data.godmode = 1;
    else
        tg_data.godmode = 0;

    GetSVInfokeyString( "disable_reload", NULL, st, sizeof( st ), "off" );
    if ( !strcmp( st, "on" ) )
        tg_data.disable_reload = 1;
    else
        tg_data.disable_reload = 0;

    GetSVInfokeyString( "unl_ammo", NULL, st, sizeof( st ), "off" );
    if ( !strcmp( st, "on" ) )
        tg_data.unlimit_ammo = 1;
    else
        tg_data.unlimit_ammo = 0;

    GetSVInfokeyString( "unl_gren", NULL, st, sizeof( st ), "off" );
    if ( !strcmp( st, "on" ) )
        tg_data.unlimit_grens = 1;
    else
        tg_data.unlimit_grens = 0;


    GetSVInfokeyString( "dp_da", NULL, st, sizeof( st ), "on" );
    if ( !strcmp( st, "off" ) )
        tg_data.disable_disarm = 1;
    else
        tg_data.disable_disarm = 0;

    GetSVInfokeyString( "dp_drop", NULL, st, sizeof( st ), "off" );
    if ( !strcmp( st, "on" ) )
        tg_data.detpack_drop = 1;
    else
        tg_data.detpack_drop = 0;


    GetSVInfokeyString( "dp_clip", NULL, st, sizeof( st ), "" );
    if ( !strcmp( st, "off" ) )
        tg_data.detpack_clip = TG_DETPACK_SOLID_ALL;
    else if( !strcmp( st, "all" ) )
        tg_data.detpack_clip = TG_DETPACK_CLIP_ALL;
    else
        tg_data.detpack_clip = TG_DETPACK_CLIP_OWNER;
    Detpack_SetClip();

    tf_data.sg_newfind = true;
    tf_data.sg_sfire   = SG_SFIRE_NEW;

    GetSVInfokeyString( "sg", NULL, st, sizeof( st ), "new" );
    if ( !strcmp( st, "old" ) )
    {
        tf_data.sg_newfind = false;
        tf_data.sg_sfire   = SG_SFIRE_281;
    }
    if ( !strcmp( st, "fix" ) )
    {
        tf_data.sg_newfind = false;
        tf_data.sg_sfire   = SG_SFIRE_MTFL2;
    }

    if ( !strcmp( st, "oldmtfl" ) )
    {
        tf_data.sg_newfind = false;
        tf_data.sg_sfire   = SG_SFIRE_MTFL1;
    }
    if ( !strcmp( st, "mtflf" ) )
    {
        tf_data.sg_newfind = true;
        tf_data.sg_sfire   = SG_SFIRE_MTFL1;
    }

    if ( !strcmp( st, "oldf" ) )
    {
        tf_data.sg_newfind = true;
        tf_data.sg_sfire   = SG_SFIRE_281;
    }

    GetSVInfokeyString( "sg_newfind", NULL, st, sizeof( st ), "on" );
    if ( !strcmp( st, "off" ) )
        tf_data.sg_newfind = false;
    else
        tf_data.sg_newfind = true;


    GetSVInfokeyString( "sg_sfire", NULL, st, sizeof( st ), "new" );

    if( !strcmp(st, "old"))
        tf_data.sg_sfire = SG_SFIRE_281;

    if( !strcmp(st, "mtfl1"))
        tf_data.sg_sfire = SG_SFIRE_MTFL1;

    if( !strcmp(st, "mtfl2"))
        tf_data.sg_sfire = SG_SFIRE_MTFL2;

    GetSVInfokeyString( "sg_rfire", NULL, st, sizeof( st ), "old" );

    if( !strcmp(st, "new"))
        tf_data.sg_rfire = true;
    else
        tf_data.sg_rfire = false;

    tf_data.sgppl = GetSVInfokeyInt( "sgppl", NULL, 12 );
    if ( tf_data.sgppl < 0 )
        tf_data.sgppl = 0;

    GetSVInfokeyString( "dtpb", NULL, st, sizeof( st ), "on" );
    if ( !strcmp( st, "off" ) )
        tf_data.detpack_block = 0;
    else
        tf_data.detpack_block = 1;

    GetSVInfokeyString( "tg_sbar", NULL, st, sizeof( st ), "off" );
    if ( !strcmp( st, "on" ) )
        tg_data.tg_sbar = 1;
    else
        tg_data.tg_sbar = 0;

}

void TG_Cmd()
{
        char    cmd_command[50];
        char    st[50];
        int argc;


	if( !tg_data.tg_enabled )
	{
		G_sprint(self, 2, "Training Ground mode not active\n");
		return;
	}
	argc = trap_CmdArgc();
	if(argc<2)
		return;

        trap_CmdArgv( 1, cmd_command, sizeof( cmd_command ) );
        if(!strcmp(cmd_command,"sgppl"))
        {
                if(argc<3)
                {
                	G_sprint(self, 2, "sgppl %d\n",tf_data.sgppl);
                	return;
                }
                	
                trap_CmdArgv( 2, st, sizeof( st ) );
                tf_data.sgppl = atoi(st);
                if( tf_data.sgppl < 0)
                	tf_data.sgppl = 0;
                return;
        }

        if(!strcmp(cmd_command,"sg_fire"))
        {
                if(argc<3)
                {
                	G_sprint(self, 2, "SG_Fire %s\n",(!tg_data.sg_disable_fire)?"On":"Off");
                	return;
                }
                	
                trap_CmdArgv( 2, st, sizeof( st ) );
          	if ( !strcmp( st, "off" ) )
          		tg_data.sg_disable_fire = 1;
          	else if( !strcmp( st, "toggle" ) )
          	{
          	        tg_data.sg_disable_fire = (tg_data.sg_disable_fire)?0:1;
          	}else
          	{
          		tg_data.sg_disable_fire = 0;
          	}
                return;
        }
        if(!strcmp(cmd_command,"sg_find"))
        {
                if(argc<3)
                {
                	G_sprint(self, 2, "SG_Find \n");
                	return;
                }
                	
                trap_CmdArgv( 2, st, sizeof( st ) );
          	if ( !strcmp( st, "all" ) )
          		tg_data.sg_allow_find = TG_SG_FIND_IGNORE_OFF;
          	else if( !strcmp( st, "self" ) )
          		tg_data.sg_allow_find = TG_SG_FIND_IGNORE_OWNER;
          		else if( !strcmp( st, "no" ) )
          				tg_data.sg_allow_find = TG_SG_FIND_IGNORE_ALL;
          			else
          				tg_data.sg_allow_find = TG_SG_FIND_IGNORE_TEAM;
                return;
        }

        if(!strcmp(cmd_command,"sg_fire_bullets"))
        {
                if(argc<3)
                {
                	G_sprint(self, 2, "sg_fire_bullets is %s\n", tg_data.sg_fire_bullets?"on":"off");
                	return;
                }
                trap_CmdArgv( 2, st, sizeof( st ) );
         	if( !strcmp(st,"off") )
         	        tg_data.sg_fire_bullets = false;
         	else
         	        tg_data.sg_fire_bullets = true;
         	return;
        }
        if(!strcmp(cmd_command,"sg_fire_rockets"))
        {
                if(argc<3)
                {
                	G_sprint(self, 2, "sg_fire_rockets is %s\n", tg_data.sg_fire_rockets?"on":"off");
                	return;
                }
                trap_CmdArgv( 2, st, sizeof( st ) );
         	if( !strcmp(st,"off") )
         	        tg_data.sg_fire_rockets = false;
         	else
         	        tg_data.sg_fire_rockets = true;
         	return;
        }
        if(!strcmp(cmd_command,"sg_fire_lighting"))
        {
                if(argc<3)
                {
                	G_sprint(self, 2, "sg_fire_lighting is %s\n", tg_data.sg_fire_lighting?"on":"off");
                	return;
                }
                trap_CmdArgv( 2, st, sizeof( st ) );
         	if( strcmp(st,"on") )
         	        tg_data.sg_fire_lighting = false;
         	else
         	        tg_data.sg_fire_lighting = true;
         	return;
        }

/*        if(!strcmp(cmd_command,"sg_fire_type"))
        {
                if(argc<3)
                {
                	G_sprint(self, 2, "sg_fire_type \n");
                	return;
                }
                	
                trap_CmdArgv( 2, st, sizeof( st ) );
        	if ( !strcmp( st, "nodmg" ) )
        		tg_data.sg_fire_type = TG_SG_FIRE_LIGHTING;
        	else if( !strcmp( st, "bullets" ) )
        			tg_data.sg_fire_type = TG_SG_FIRE_BULLETS;
       		else
        			tg_data.sg_fire_type = TG_SG_FIRE_NORMAL;
                return;
        }*/
	if ( !strcmp( cmd_command, "eff_conc" ) )
	{
	     TG_Eff_Conc(self);
	     return;
	}

	if ( !strcmp( cmd_command, "god" ) )
	{

	     if ( ( int ) self->s.v.flags & FL_GODMODE )
	     {
	        G_sprint(self,2,"God mode off\n");
	        self->s.v.flags = ( int ) self->s.v.flags - FL_GODMODE;
	     }else
	     {
	        G_sprint(self,2,"God mode on\n");
	        self->s.v.flags = ( int ) self->s.v.flags | FL_GODMODE;
	     }
	     return;
	}
}



void FlashPlayer( gedict_t*p, gedict_t*attacker );
void ConcPlayer( gedict_t*p, gedict_t*attacker );
void ConcussionGrenadeTimer(  );
void OldConcussionGrenadeTimer(  );
void TG_Eff_Flash( gedict_t * te )
{
	if ( strneq( te->s.v.classname, "player" ) )
		return;
	if ( te->s.v.health <= 0 )
		return;

	FlashPlayer( te, NULL );
}

void TG_Eff_Conc( gedict_t * head )
{
	if ( strneq( head->s.v.classname, "player" ) )
		return;
	if ( head->s.v.health <= 0 )
		return;

	ConcPlayer( head, NULL);
}

void    HallucinationTimer(  );
void    TranquiliserTimer(  );

void TG_Eff_Remove( gedict_t * pl )
{
	gedict_t *te;
	float   healam;

	if ( strneq( pl->s.v.classname, "player" ) )
		return;
	if ( pl->s.v.health <= 0 )
		return;

	for ( te = world; ( te = trap_find( te, FOFS( s.v.classname ), "timer" ) ); )
	{
		if ( te->s.v.owner != EDICT_TO_PROG( pl ) )
			continue;
		if ( te->s.v.think != ( func_t ) ConcussionGrenadeTimer &&
		     te->s.v.think != ( func_t ) OldConcussionGrenadeTimer )
			continue;
		if ( tf_data.old_grens == 1 )
			stuffcmd( pl, "v_idlescale 0\nfov 90\n" );
		dremove( te );
		break;
	}
	if ( pl->tfstate & TFSTATE_HALLUCINATING )
	{
		for ( te = world; ( te = trap_find( te, FOFS( s.v.classname ), "timer" ) ); )
		{
			if ( te->s.v.owner != EDICT_TO_PROG( pl ) )
				continue;
			if ( te->s.v.think != ( func_t ) HallucinationTimer )
				continue;

			pl->tfstate -= ( pl->tfstate & TFSTATE_HALLUCINATING );

			ResetGasSkins( pl );

			if ( tf_data.new_gas & GAS_MASK_PALETTE )
				stuffcmd( pl, "v_cshift; wait; bf\n" );
			dremove( te );
			break;
		}
		if ( !te )
			G_conprintf( "Warning: Error in Hallucination Timer logic.\n" );
	}
	if ( pl->tfstate & TFSTATE_TRANQUILISED )
	{
		for ( te = world; ( te = trap_find( te, FOFS( s.v.classname ), "timer" ) ); )
		{
			if ( te->s.v.owner != EDICT_TO_PROG( pl ) )
				continue;
			if ( te->s.v.think != ( func_t ) TranquiliserTimer )
				continue;

			pl->tfstate -= ( pl->tfstate & TFSTATE_TRANQUILISED );
			TeamFortress_SetSpeed( pl );
			dremove( te );
			break;
		}
		if ( !te )
			G_conprintf( "Warning: Error in Tranquilisation Timer logic.\n" );
	}
	if ( pl->FlashTime > 0 )
	{
		for ( te = world; ( te = trap_find( te, FOFS( s.v.netname ), "flashtimer" ) ); )
		{
			if ( te->s.v.owner != EDICT_TO_PROG( pl ) )
				continue;
			if ( strneq( te->s.v.classname, "timer" ) )
				continue;

			pl->FlashTime = 0;
			if ( tf_data.new_flash )
				disableupdates( pl, -1 );	/* server-side flash */
			break;
		}
		if ( !te )
		{
			G_conprintf( "Warning: Error in Flash Timer logic.\n" );
			pl->FlashTime = 0;
		}
	}
	if ( pl->tfstate & TFSTATE_INFECTED )
	{
		healam = Q_rint( pl->s.v.health / 2 );
		pl->tfstate -= ( pl->tfstate & TFSTATE_INFECTED );
		tf_data.deathmsg = DMSG_MEDIKIT;
		T_Damage( pl, self, self, healam );
		return;
	}
	if ( pl->numflames > 0 )
	{
		sound( pl, 1, "items/r_item1.wav", 1, 1 );
		pl->numflames = 0;
		return;
	}
}
