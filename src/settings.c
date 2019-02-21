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
 *  $Id: client.c,v 1.61 2006-11-29 23:19:23 AngelD Exp $
 */
#include "g_local.h"

typedef enum
{
    SET_HWGUY_SB,
    SET_SBAR_RES,
    SET_SBAR_RES,
    SET_DISCARD_SHELLS,
    SET_DISCARD_NAILS,
    SET_DISCARD_ROCKETS,
    SET_DISCARD_CELLS,
    SET_BITS,
    SET_CLASSHELP,
    SET_MULTISCAN,
    SET_EXEC_CLASS,
    SET_EXEC_MAP,
    SET_TAKE_SSHOT,
    SET_GREN_SOUND,
} cl_setting_id_t;

typedef enum
{
    CS_T_BOOL,
    CS_T_INT,
    CS_T_STRING,
}cl_settings_type_t;
typedef struct cl_settings_s{
    const char* key1;
    const char* key2;
    const char* name;
    const char* desc;
    cl_setting_id_t id;
    cl_settings_type_t type;
}cl_settings_t;
static cl_settings_t cl_set[] = {
    {"mxs", "1","Discard shells", "", SET_DISCARD_SHELLS, CS_C_INT,}
    {"mxn", "2","Discard nails", "", SET_DISCARD_SHELLS, CS_C_INT,}
    {"mxr", "3","Discard rockets", "", SET_DISCARD_SHELLS, CS_C_INT,}
    {"mxc", "4","Discard cells", "", SET_DISCARD_SHELLS, CS_C_INT,}
};
qboolean SetClientSetting( gedict_t * p, const char *key, const char *value )
{
    unsigned int crc;

    crc = crc32( 0L, ( unsigned char * ) key, strlen( key ) );
    switch ( crc )
    {
        case 0xE67738BE:	//sb
            p->assault_min_shells = atoi( value );
            if ( p->assault_min_shells < 0 )
                p->assault_min_shells = DEFAULT_ASSAULT_MIN_SHELLS;
            break;
        case 0x4036162E:	//sbr
        case 0x0EF36728:	//sbar_res
            {
                int     sbres = atoi( value );

                switch ( sbres )
                {
                    case 768:
                        p->StatusBarRes = 8;
                        break;
                    case 600:
                        p->StatusBarRes = 7;
                        break;
                    case 480:
                        p->StatusBarRes = 6;
                        break;
                    case 400:
                        p->StatusBarRes = 5;
                        break;
                    case 384:
                        p->StatusBarRes = 4;
                        break;
                    case 350:
                        p->StatusBarRes = 3;
                        break;
                    case 300:
                        p->StatusBarRes = 2;
                        break;
                    case 240:
                        p->StatusBarRes = 1;
                        break;
                    case 200:
                        p->StatusBarRes = 0;
                        break;
                    default:
                        p->StatusBarRes = 0;
                        break;
                }
                p->StatusRefreshTime = g_globalvars.time + 0.2;
            }
            break;
        case 0x373126B8:	//sbs
        case 0xBA05048F:	//sbar_size
            {
                p->StatusRefreshTime = g_globalvars.time + 0.2;
                p->StatusBarSize = atoi( value );
                if ( p->StatusBarSize > 2 || p->StatusBarSize < 0 )
                    p->StatusBarSize = 0;
                if( p->StatusBarSize )
                    p->StatusRefreshTime = g_globalvars.time + 0.2;
                else
                    p->StatusRefreshTime = g_globalvars.time + 60;
            }
            break;
        case 0x91A45219:	//mxs
        case 0x83DCEFB7:	//1
            if ( value[0] )
                p->discard_shells = atoi( value );
            else
                p->discard_shells = -1;

            break;
        case 0xF2A23EC0:	//mxn
        case 0x1AD5BE0D:	//2
            if ( value[0] )
                p->discard_nails = atoi( value );
            else
                p->discard_nails = -1;
            break;
        case 0xE6A3628F:	//mxr
        case 0x6DD28E9B:	//3
            if ( value[0] )
                p->discard_rockets = atoi( value );
            else
                p->discard_rockets = -1;
            break;
        case 0x8C13427D:	//mxc
        case 0xF3B61B38:	//4
            if ( value[0] )
                p->discard_cells = atoi( value );
            else
                p->discard_cells = -1;
            break;
        case 0x1B0ECF0B:	//s
            p->settings_bits = atoi( value );
            break;
        case 0x4C60C3F1:	//ch
        case 0x33653DAA:	//classhelp
            if ( strcmp( value, "off" ) )
                p->settings_bits |= TF_CLASS_HELP_MASK;
            else
                p->settings_bits -= p->settings_bits & TF_CLASS_HELP_MASK;
            break;

        case 0x58862793:	//ms
        case 0x02E5FD61:	//multiscan
            if ( !strcmp( value, "off" ) )
                p->settings_bits |= TF_MULTISCAN_MASK;
            else
                p->settings_bits -= p->settings_bits & TF_MULTISCAN_MASK;
            break;
        case 0x8DE8BDFF:	//ec
        case 0x6BDB6A88:	//exec_class
            if ( !strcmp( value, "on" ) )
                p->settings_bits |= TF_EXEC_CLASS_MASK;
            else
                p->settings_bits -= p->settings_bits & TF_EXEC_CLASS_MASK;
            break;
        case 0x6A5090F8:	//em
        case 0x5B8710D3:	//exec_map
            if ( !strcmp( value, "on" ) )
                p->settings_bits |= TF_EXEC_MAP_MASK;
            else
                p->settings_bits -= p->settings_bits & TF_EXEC_MAP_MASK;
            break;
        case 0xCAD67046:	//take_sshot
            if ( !value[0] )
                p->take_sshot = 1;
            else
                p->take_sshot = 0;
            break;
        case 0xE5A257DF:	//grensound
            if ( !strcmp( value, "on" ) )
                p->internal_settings_bits |= TF_INTERNAL_GRENSOUND;
            else
                p->internal_settings_bits -= p->settings_bits & TF_INTERNAL_GRENSOUND;
            break;
        default:
            return false;
    }
    return true;
}

qboolean PrintClientSetting( gedict_t * p, const char *key )
{
    unsigned int crc;

    crc = crc32( 0L, ( unsigned char * ) key, strlen( key ) );
    switch ( crc )
    {
        case 0xE67738BE:	//sb
            G_sprint( p, 2, "%s %3d\n", key, p->assault_min_shells );
            break;
        case 0x4036162E:	//sbr
        case 0x0EF36728:	//sbar_res
            {
                static const int sbres_tbl[] = { 200, 240, 300, 350, 384, 400, 480, 600, 768 };
                int     index = p->StatusBarRes;

                if ( index > 8 || index < 0 )
                    index = 0;
                G_sprint( p, 2, "%s %3d\n", key, sbres_tbl[index] );
            }
            break;
        case 0x373126B8:	//sbs
        case 0xBA05048F:	//sbar_size
            {
                G_sprint( p, 2, "%s %3d\n", key, p->StatusBarSize );
            }
            break;
        case 0x91A45219:	//mxs
        case 0x83DCEFB7:	//1
            G_sprint( p, 2, "Discard shells %3d\n", p->discard_shells );
            break;
        case 0xF2A23EC0:	//mxn
        case 0x1AD5BE0D:	//2
            G_sprint( p, 2, "Discard nails %3d\n", p->discard_nails );
            break;
        case 0xE6A3628F:	//mxr
        case 0x6DD28E9B:	//3
            G_sprint( p, 2, "Discard rockets %3d\n", p->discard_rockets );
            break;
        case 0x8C13427D:	//mxc
        case 0xF3B61B38:	//4
            G_sprint( p, 2, "Discard cells %3d\n", p->discard_cells );
            break;
        case 0x1B0ECF0B:	//s
            G_sprint( p, 2, "TF setings bits %4d\n", p->settings_bits );
            break;
        case 0x4C60C3F1:	//ch
        case 0x33653DAA:	//classhelp
            if ( p->settings_bits & TF_CLASS_HELP_MASK )
                G_sprint( p, 2, "Classhelp is " _ON "\n" );
            else
                G_sprint( p, 2, "Classhelp is " _OFF "\n" );
            break;

        case 0x58862793:	//ms
        case 0x02E5FD61:	//multiscan
            if ( !( p->settings_bits & TF_MULTISCAN_MASK ) )
                G_sprint( p, 2, "Multiscan is " _ON "\n" );
            else
                G_sprint( p, 2, "Multiscan is " _OFF "\n" );
            break;
        case 0x8DE8BDFF:	//ec
        case 0x6BDB6A88:	//exec_class
            if ( p->settings_bits & TF_EXEC_CLASS_MASK )
                G_sprint( p, 2, "Exec class is " _ON "\n" );
            else
                G_sprint( p, 2, "Exec class is " _OFF "\n" );
            break;
        case 0x6A5090F8:	//em
        case 0x5B8710D3:	//exec_map
            if ( p->settings_bits & TF_EXEC_MAP_MASK )
                G_sprint( p, 2, "Exec map is " _ON "\n" );
            else
                G_sprint( p, 2, "Exec map is " _OFF "\n" );
            break;
        case 0xCAD67046:	//take_sshot
            if ( p->settings_bits & TF_EXEC_MAP_MASK )
                G_sprint( p, 2, "Take screenshot is " _ON "\n" );
            else
                G_sprint( p, 2, "Take screenshot is " _OFF "\n" );
            break;
        case 0xE5A257DF:	//grensound
            if ( p->internal_settings_bits & TF_INTERNAL_GRENSOUND )
                G_sprint( p, 2, "Grenade sound is " _ON "\n" );
            else
                G_sprint( p, 2, "Grenade sound is " _OFF "\n" );
            break;
        default:
            return false;
    }
    return true;
}

void   Client_Set_Cmd(  )
{
    char    key[20];
    char    value[20];
    int argc;


    argc = trap_CmdArgc();
    if(argc < 2)
        return;
    trap_CmdArgv( 1, key, sizeof( key ) );
    Q_strlwr( key );
    if(argc < 3)
    {
        PrintClientSetting( self, key);
        return;
    }
    trap_CmdArgv( 2, value, sizeof( value ) );
    if( SetClientSetting( self, key, value) )
        PrintClientSetting( self, key);
}
//=======================
void ParseUserInfo()
{
    char    value[100];
    self->assault_min_shells = DEFAULT_ASSAULT_MIN_SHELLS;
    self->StatusBarRes = 0;
    self->StatusBarSize = 0;
    self->discard_shells = -1;
    self->discard_nails = -1;
    self->discard_rockets = -1;
    self->discard_cells = -1;
    self->settings_bits = TF_CLASS_HELP_MASK;
    self->internal_settings_bits = 0;
    self->take_sshot = 0;
    if( GetInfokeyString( self, "sbr", "sbar_res" , value, sizeof( value ), NULL))
        SetClientSetting( self, "sbr", value);
    if( GetInfokeyString( self, "sbs", "sbar_size" , value, sizeof( value ), NULL))
        SetClientSetting( self, "sbs", value);

    if( GetInfokeyString( self, "mxs", "1" , value, sizeof( value ), NULL))
        SetClientSetting( self, "mxs", value);
    if( GetInfokeyString( self, "mxn", "2" , value, sizeof( value ), NULL))
        SetClientSetting( self, "mxn", value);
    if( GetInfokeyString( self, "mxr", "3" , value, sizeof( value ), NULL))
        SetClientSetting( self, "mxr", value);
    if( GetInfokeyString( self, "mxc", "4" , value, sizeof( value ), NULL))
        SetClientSetting( self, "mxc", value);

    if( GetInfokeyString( self, "sb", NULL , value, sizeof( value ), NULL))
        SetClientSetting( self, "sb", value);

    if( GetInfokeyString( self, "s", NULL , value, sizeof( value ), NULL))
        SetClientSetting( self, "s", value);

    if( GetInfokeyString( self, "ch", "classhelp", value, sizeof( value ), NULL))
        SetClientSetting( self, "ch", value);

    if( GetInfokeyString( self, "ms", "multiscan", value, sizeof( value ), NULL))
        SetClientSetting( self, "ms", value);

    if( GetInfokeyString( self, "ec", "exec_class", value, sizeof( value ), NULL))
        SetClientSetting( self, "ec", value);

    if( GetInfokeyString( self, "em", "exec_map", value, sizeof( value ), NULL))
        SetClientSetting( self, "em", value);

    if( GetInfokeyString( self, "take_sshot", NULL, value, sizeof( value ), ""))
        SetClientSetting( self, "take_sshot", value);
}
