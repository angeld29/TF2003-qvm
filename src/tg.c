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

       	GetSVInfokeyString( "unl_ammo", NULL, st, sizeof( st ), "off" );
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

       	tf_data.sentry_type = SENTRY_NEW;

       	GetSVInfokeyString( "sg", NULL, st, sizeof( st ), "new" );
       	if ( !strcmp( st, "old" ) )
       		tf_data.sentry_type = SENTRY_OLD;
       	else if ( !strcmp( st, "fix" ) )
       		tf_data.sentry_type = SENTRY_FIX;
       	else if ( !strcmp( st, "oldmtfl" ) )
       		tf_data.sentry_type = SENTRY_MTFL;
       	else if ( !strcmp( st, "mtflf" ) )
       		tf_data.sentry_type = SENTRY_MTFL_NEWFIND;
       	else if ( !strcmp( st, "oldf" ) )
       		tf_data.sentry_type = SENTRY_OLD_NEWFIND;
       	else
       		tf_data.sentry_type = SENTRY_NEW;

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
