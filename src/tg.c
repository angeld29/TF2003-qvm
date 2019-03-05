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

    tg_data.sg_disable_fire = !GetSVInfokeyBool( "tg_sg_fire", NULL, true );

    GetSVInfokeyString( "tg_sg_find", NULL, st, sizeof( st ), "" );
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
    tg_data.sg_fire_bullets = GetSVInfokeyBool( "tg_sg_fire_bullets", NULL, true );
    tg_data.sg_fire_rockets = GetSVInfokeyBool( "tg_sg_fire_rockets", NULL, true );
    tg_data.sg_fire_lighting = GetSVInfokeyBool( "tg_sg_fire_lighting", NULL, false );
    tg_data.sg_unlimit_ammo = GetSVInfokeyBool( "tg_sg_unlimit_ammo", NULL, false );

    GetSVInfokeyString( "gren_eff", NULL, st, sizeof( st ), "" );
    if ( !strcmp( st, "off" ) )
        tg_data.gren_effect = TG_GREN_EFFECT_OFF;
    else if( !strcmp( st, "self" ) )
        tg_data.gren_effect = TG_GREN_EFFECT_OFF_FORSELF;
    else
        tg_data.gren_effect = TG_GREN_EFFECT_ON;

    tg_data.gren_time = GetSVInfokeyInt( "tg_gren_time", NULL, 0 );
    tg_data.godmode = GetSVInfokeyBool( "tg_god", NULL, false );
    tg_data.disable_reload = GetSVInfokeyBool( "tg_disable_reload", NULL, false );
    tg_data.unlimit_ammo = GetSVInfokeyBool( "tg_unl_ammo", NULL, false );
    tg_data.unlimit_grens = GetSVInfokeyBool( "tg_unl_gren", NULL, false );
    tg_data.disable_disarm = GetSVInfokeyBool( "tg_dg_da", "tg_disable_disarm", false );
    tg_data.detpack_drop = GetSVInfokeyBool( "tg_dp_drop", NULL, false );
    tg_data.tg_sbar = GetSVInfokeyBool( "tg_sbar", NULL, false );
    
    GetSVInfokeyString( "tg_dp_clip", NULL, st, sizeof( st ), "" );

    if ( !strcmp( st, "off" ) )
        tg_data.detpack_clip = TG_DETPACK_SOLID_ALL;
    else if( !strcmp( st, "all" ) )
        tg_data.detpack_clip = TG_DETPACK_CLIP_ALL;
    else
        tg_data.detpack_clip = TG_DETPACK_CLIP_OWNER;
    Detpack_SetClip();

}

void TG_Cmd()
{
    char    cmd_command[50];
    char    st[50];
    int argc;


    if( !tfset(tg_enabled) )
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
            G_sprint(self, 2, "sgppl %d\n",tfset_sgppl);
            return;
        }

        trap_CmdArgv( 2, st, sizeof( st ) );
        tfset_sgppl = atoi(st);
        if( tfset_sgppl < 0)
            tfset_sgppl = 0;
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
		if ( tfset(old_grens) )
        {
			stuffcmd( pl, "v_idlescale 0\nfov 90\n" );
            pl->eff_info.conc_idle = 0;
        }
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

			if ( tfset_new_gas & GAS_MASK_PALETTE )
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
			if ( tfset(new_flash) )
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
