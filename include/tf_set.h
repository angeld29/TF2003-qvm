
typedef union set_val_s
{
    int	        _int;
	float		_float;
    char*       _str;
    unsigned int _uint;
} set_val_t;	

typedef struct set_bits_s
{
    const char* name;
    const char* key;
    const char* key2;
    unsigned int bit;
    qboolean    default_val;
}set_bits_t;

typedef enum set_types_s
{
    TFS_FLOAT,
    TFS_INT,
    TFS_INT_BITS,
    TFS_BOOL,
    TFS_STRING,
    TFS_INT_SET,
}set_types_t;

typedef enum set_id_s
{
    set_toggleflags,
    set_respawn_delay,
    set_server_settings,
}set_id_t;

typedef struct set_set_s
{
//    set_id_t id;
    const char* name;
    const char* key;
    int         val;
}set_set_t;

typedef struct set_item_s
{
//    set_id_t id;
    const char* name;
    const char* key;
    const char* key2;
    set_types_t type;
    set_val_t   val;
    const set_bits_t *bitsdesc;
    const set_set_t *setdesc;
    const char* default_val;
}set_item_t;
extern set_item_t tf_settings[];

#define tfset(name) (tf_settings[0].val._uint & svsb_ ## name )

#define tfset_toggleflags 		(tf_settings[1].val._int)
#define tfset_autoteam 			(tf_settings[2].val._float)
#define tfset_respawn_delay		(tf_settings[3].val._float)
#define tfset_prematch_time		(tf_settings[4].val._float)
#define tfset_ceasefire_time	(tf_settings[6].val._float)
#define tfset_autokick_time		(tf_settings[7].val._float)
#define tfset_autokick_kills	(tf_settings[8].val._int)
#define tfset_cheat_pause		(tf_settings[9].val._int)
#define tfset_disable_grens		(tf_settings[10].val._int)
#define tfset_sgppl			    (tf_settings[11].val._int)
#define tfset_sg_sfire			(tf_settings[12].val._int)
#define tfset_snip_fps			(tf_settings[13].val._int)
#define tfset_snip_ammo			(tf_settings[14].val._int)
#define tfset_snip_time			(tf_settings[15].val._float)
#define tfset_new_gas			(tf_settings[16].val._int)
#define tfset_gren2box			(tf_settings[17].val._int)
#define tfset_arenamode         (tf_settings[18].val._int)

#define svsb_clanbattle         1<<0
#define svsb_game_locked        1<<1
#define svsb_spy_off            1<<2
#define svsb_allow_hook         1<<3
#define svsb_old_grens          1<<4
#define svsb_invis_only         1<<5
#define svsb_svconc             1<<6
#define svsb_allow_drop_goal    1<<7
#define svsb_add_pipe           1<<8
#define svsb_new_flash          1<<9
#define svsb_disable_powerups   1<<10
#define svsb_birthday           1<<11
#define svsb_old_spanner        1<<12
#define svsb_flag_timer         1<<13
#define svsb_detpack_block      1<<14
#define svsb_snip_range_fix     1<<15
#define svsb_random_tf_spawn    1<<16
#define svsb_lan_mode           1<<17
#define svsb_mtfl               1<<18
#define svsb_enable_bot         1<<19
#define svsb_tg_enabled         1<<20
#define svsb_sg_new_find        1<<21
#define svsb_sg_rfire           1<<22
#define svsb_pyrotype           1<<23

