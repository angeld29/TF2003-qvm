
#define TG_MAINMENU_IMPULSE 220
#define TG_SG_REBUILD_IMPULSE 221
#define TG_SG_RELOAD_IMPULSE 222
#define TG_SG_UPGRADE_IMPULSE 223
#define TG_DISP_LOAD_IMPULSE 224
#define TG_DISP_UNLOAD_IMPULSE 225

#define TG_MENU_MAIN 200
#define TG_MENU_SG 201
#define TG_MENU_DETPACK 202
#define TG_MENU_CHEATS 203
#define TG_MENU_POSITION 204
////////TG SG Options
#define TG_SG_FIND_IGNORE_OFF 3
#define TG_SG_FIND_IGNORE_TEAM 1
#define TG_SG_FIND_IGNORE_OWNER 2
#define TG_SG_FIND_IGNORE_ALL 0

#define TG_SG_FIRE_NORMAL 0
#define TG_SG_FIRE_BULLETS 1
#define TG_SG_FIRE_LIGHTING 2

typedef struct{
 int godmode,detpack_clip,detpack_drop, detpack_disarm,  unlimit_ammo;
 int gren_effect,gren_time, gren_eff_time,fullflash_for_self;
 int sg_allow_find;
}tf_tg_server_data_t;

extern tf_tg_server_data_t tg_data;
