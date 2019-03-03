
typedef union set_val_s
{
	float		_float;
    int	        _int;
    char*       _pstr;
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
}set_id_t;

typedef enum set_item_s
{
    set_id_t id;
    const char* key;
    const char* key2;
    const char* name;
    set_types_t type;
    set_val_t   val;
    set_val_t   default_val;
    const set_bits_t *bitsdesc;
}set_item_t;

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

