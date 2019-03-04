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
    { "Settings bits", "", "",  TFS_INT_BITS, 0, &sv_settings_bits[0], 0 },
    { NULL } 
};

static void   tf_set( const char* name, const char*val, qboolean noprint  )
{
    set_item_t* si = &tf_settings[0];

    for(; si->name; si++ ){
        if( si->type == TFS_INT_BITS ){
            const set_bits_t* sb = si->bitsdesc;
            for(; sb->name; sb++ ){
                if( streq( sb->key, name ) || streq( sb->key2, name ) ){
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
                    if( ! noprint )
                        G_conprintf( "%s: %s %d\n", sb->name, bit_set? _ON: _OFF, si->val._uint );
                    return;
                }
            }
        }
    }
}

void   TF_Set_Cmd( int argc  )
{
    char    cmd_command[64];
    char    val[64] = "";

    if( argc < 3 ) return;

    trap_CmdArgv( 2, cmd_command, sizeof( cmd_command ) );
    if( argc == 4 ){
        trap_CmdArgv( 3, val, sizeof( val ) );
    }
    tf_set( cmd_command, val, 0 );

}
