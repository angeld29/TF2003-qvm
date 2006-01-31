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
 *  $Id: tg.c,v 1.14 2006-01-31 13:39:00 AngelD Exp $
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
       				
       	GetSVInfokeyString( "sg_fire_type", NULL, st, sizeof( st ), "" );
       	if ( !strcmp( st, "nodmg" ) )
       		tg_data.sg_fire_type = TG_SG_FIRE_LIGHTING;
       	else if( !strcmp( st, "bullets" ) )
       			tg_data.sg_fire_type = TG_SG_FIRE_BULLETS;
      		else
       			tg_data.sg_fire_type = TG_SG_FIRE_NORMAL;
      			
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

        if(!strcmp(cmd_command,"sg_fire_type"))
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
        }
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

