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
 */

#include "g_local.h"
#include "tf_set.h"

const set_bits_t sv_settings_bits[] = {
  { "Clan Battle", "clan", "c", svsb_clanbattle         , false },
  { "Locked Game", "locked_game", "lg", svsb_game_locked        , false },
  { "Disable Spy", "spy_off", "", svsb_spy_off            , false },
  { "Grapple", "grapple", "g", svsb_allow_hook         , false },
  { "Old grenades", "old_grens", "og", svsb_old_grens          , true },
  { "Spy invis", "spyinvis", "s", svsb_invis_only         , false },
  { "Server conc grenade", "svconc", "", svsb_svconc             , true },
  { "Drop Goals", "allow_drop_goal", "adg", svsb_allow_drop_goal    , false },
  { "Additional pipebomb", "add_pipe", "", svsb_add_pipe           , true },
  { "New flash", "new_flash", "nf", svsb_new_flash          , true  },
  { "Disable powerups", "disable_powerups", "dp", svsb_disable_powerups   , false },
  { "Birthday mode", "bd", "", svsb_birthday           , false },
  { "Old spanner", "old_spanner", "", svsb_old_spanner        , false },
  { "Flag timer", "flag_timer", "ft", svsb_flag_timer         , true  },
  { "Detpack blocking", "dtpb", "", svsb_detpack_block      , true  },
  { "Sniper range fix", "snip_range_fix", "srf", svsb_snip_range_fix     , false },
  { "Random team spawn", "random_team_spawn", "rts", svsb_random_tf_spawn    , true  },
  { "Lan Mode", "lan", "", svsb_lan_mode           , false },
  { "MTFL settings", "mtfl", "", svsb_mtfl               , false },
  { "Bots", "enable_bot", "", svsb_enable_bot         , false },
  { "Training ground mode", "tg", "", svsb_tg_enabled         , false },
  { "Sentry Gun New Find", "sg_newfind", "", svsb_sg_new_find        , true  },
  { "Sentry Gun New Rockets", "sg_rfire", "", svsb_sg_rfire        , true  },
  { "Pyro flame walls", "pyrotype", "", svsb_pyrotype        , false  },
  { NULL, },
};

static set_item_t tf_settings[] = {
    { "Settings bits", "", "",  TFS_INT_BITS, 0, &sv_settings_bits[0], "0" },
    { "Respawn delay", "respawn_delay", "rd",  TFS_FLOAT, 0, NULL, "0"  },
    { "Prematch time", "prematch", "pm",  TFS_FLOAT, 0, NULL, "0"  },
    { "CeaseFire time", "ceasefire_time", "cft",  TFS_FLOAT, 0, NULL, "0"  },
    { "Autokick time", "autokick_time", "akt",  TFS_FLOAT, 0, NULL, "0"  },
    { "Autokick kills", "autokick_kills", "akk",  TFS_FLOAT, 0, NULL, "0"  },
    { "Cheat Pause", "cheat_pause", "cp",  TFS_INT, 0, NULL, "1"  },
    { "Disable Grenades", "disable_grens", "dg",  TFS_INT, 0, NULL, "0"  },
    { "Sentry ppl emulation", "sgppl", "",  TFS_INT, 12, NULL, "12"  },
    { "Sentry shells fire", "sg_sfire", "",  TFS_INT, 0, NULL, "0"  },
    { "Sniper fps", "snip_fps", "sf",  TFS_INT, 0, NULL, "72"  },
    { "Sniper ammo on shot", "snip_ammo", "",  TFS_INT, 0, NULL, "1"  },
    { "Sniper reload time", "snip_time", "",  TFS_FLOAT, 0, NULL, "1.5" },
    { "Gas grenade effects", "new_gas", "",  TFS_INT, 131, NULL, "131"  },
    { "Grenades in backpack", "gren2box", "g2b",  TFS_INT, 0, NULL, "0"  },
    { "Arena Mode", "arena", "",  TFS_INT, 0, NULL, "0"  },
    { NULL } 
};

char           *G_NewString( const char *string );
static void _tfset_print_bits( unsigned int  val, const set_bits_t* sb )
{
    for(; sb->name; sb++ ){
        G_conprintf( "%s:%s: %s\n", sb->key, sb->name, val & sb->bit ? _ON: _OFF );
    }
}

static qboolean _tfset_getby_key( const char* key, set_item_t** outsi, int*idx){
    set_item_t* si = &tf_settings[0];

    for(; si->name; si++ ){
        if( streq( si->key, key ) || streq( si->key2, key ) ){
            *outsi = si;
            *idx = -1;
            return true;
        }
        if( si->type == TFS_INT_BITS ){
            const set_bits_t* sb = si->bitsdesc;
            int i = 0;
            for(; sb->name; sb++, i++ ){
                if( streq( sb->key, key ) || streq( sb->key2, key ) ){
                    *outsi = si;
                    *idx = i;
                    return true;
                }
            }
        }
    }
    return false;
}

static void   tf_set_val( set_item_t* si, int idx,  const char*val, qboolean oninit  )
{
    switch( si->type ){
        case TFS_INT_BITS:
            if( idx < 0 ){
                if( val && val[0] ){
                    si->val._int = atoi(val);
                }
                if( !oninit ){
                    G_conprintf( "%s:%s: %d\n", si->key, si->name, si->val._uint );
                    _tfset_print_bits( si->val._uint, si->bitsdesc );
                }
            }else{
                const set_bits_t* sb = si->bitsdesc + idx;
                qboolean bit_set = si->val._uint & sb->bit;
                if( val && val[0] ){
                    bit_set = sb->default_val;
                    if( streq( val, "on" ) || streq( val, "1" ) ){
                        bit_set = true;
                    }else if( streq( val, "off" ) || streq( val, "0" ) ){
                        bit_set = false;
                    }
                    if( bit_set ){
                        si->val._uint |= sb->bit;
                    }else{
                        si->val._uint -= si->val._uint & sb->bit;
                    }
                }
                if( !oninit )
                    G_conprintf( "%s:%s: %s\n", sb->key, sb->name, bit_set? _ON: _OFF );
            }
            break;
        case TFS_INT:
            if( val && val[0] ) si->val._int = atoi( val );
            if( !oninit ) G_conprintf( "%s:%s: %d\n", si->key, si->name, si->val._int );
            break;
        case TFS_FLOAT:
            if( val && val[0] ) si->val._int = atof( val );
            if( !oninit ) G_conprintf( "%s:%s: %.2f\n", si->key, si->name, si->val._float );
            break;
        case TFS_STRING:
            if( val && val[0] ){
                if( oninit ) {
                    si->val._str = G_NewString( val );
                }else{
                    G_conprintf( "Changing string settings allowed only by localinfo\n"
                            "%s:%s: %.2f\n", si->key, si->name, si->val._float );
                }
            }
            break;
        case TFS_BOOL:
            if( val && val[0] ){
                si->val._int = atoi(si->default_val);
                if( streq( val, "on" ) || streq( val, "1" ) ){
                    si->val._int = 1;
                }else if( streq( val, "off" ) || streq( val, "0" ) ){
                    si->val._int = 0;
                }
            }
            if( !oninit ) G_conprintf( "%s:%s: %s\n", si->key, si->name, si->val._int? _ON : _OFF );
            break;
        case TFS_INT_SET:
            break;
        default: 
            break;
    }
}
static void   tf_set( const char* name, const char*val, qboolean oninit  )
{
    set_item_t* si;
    int idx;
    if( !_tfset_getby_key( name, &si, &idx ))
        return;
    tf_set_val( si, idx, val, oninit );
}

void   TF_Set_Cmd( int argc  )
{
    char    name[64];
    char    val[64] = "";

    if( argc == 2 ) {
        set_item_t* si = &tf_settings[0];
        for(; si->name; si++ ){
            tf_set_val( si, -1, NULL, 0 );
        }
        return;
    }

    trap_CmdArgv( 2, name, sizeof( name ) );
    if( argc == 4 ){
        trap_CmdArgv( 3, val, sizeof( val ) );
    }
    tf_set( name, val, 0 );

}

void   TF_LocalinfoSettings( )
{
    char    value[100];
    set_item_t* si = &tf_settings[0];
    for(; si->name; si++ ){
        if( si->key && si->key[0] ){
            GetSVInfokeyString( si->key, si->key2, value, sizeof( value ), si->default_val);
            if( value[0] ) tf_set( si->key, value, 1);
        }
        if( si->type == TFS_INT_BITS ){
            const set_bits_t* sb = si->bitsdesc;
            for(; sb->name; sb++ ){
                if( sb->key && sb->key[0] ){
                    GetSVInfokeyString( sb->key, sb->key2, value, sizeof( value ), sb->default_val? "1":"0");
                    if( value[0] ) tf_set( sb->key, value, 1);
                }
            }
        }
    }
}
