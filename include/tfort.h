typedef struct {
	int     bitmask;
	int     maxhealth, maxspeed, maxstrafespeed;
	int     weapons_carried, current_weapon;
	int     ammo_shells, ammo_nails, ammo_rockets, ammo_cells, ammo_detpack, ammo_medikit;
	int     maxammo_shells, maxammo_nails, maxammo_rockets, maxammo_cells, maxammo_detpack, maxammo_medikit;
	int     no_grenades_1, no_grenades_2;
	int     tp_grenades_1, tp_grenades_2;
	int     og_tp_grenades_1, og_tp_grenades_2;
	int     tf_items, tf_items_flags;
	int     armorclass, armorvalue, maxarmor;
	float   armortype, armor_allowed;
	int     items_allowed, items;
	char   *name;
	char   *defaultskin;
//	char    skin[20];
//	char    *skin;
	char   *infokey4skin[4];
} class_settings_t;

extern char class_skin[][20];
extern class_settings_t class_set[];

//tfort.c
char   *TeamFortress_GetClassName( int pc );
void    TF_AddFrags( gedict_t * pl, int fr );
void    TeamFortress_RemoveTimers(  );
void    TeamFortress_SetupRespawn( int Suicided );
void    TeamFortress_ChangeClass(  );
void    TeamFortress_CheckClassStats(  );
int     IsLegalClass( int pc );
void    TeamFortress_DescribeArmor( gedict_t * Player, int Armorclass );
void    TeamFortress_PrintClassName( gedict_t *, int, int );
void    TeamFortress_SetEquipment(  );
void    TeamFortress_SetHealth(  );
void    TeamFortress_SetSpeed( gedict_t * );
void    TeamFortress_SetSkin( gedict_t * );
void    TeamFortress_ExecClassScript( gedict_t * );
void    TeamFortress_ExecMapScript( gedict_t * p );
int     TeamFortress_TeamPutPlayerInTeam(  );
void    KickCheater( gedict_t * p );
void    TeamFortress_Alias( char *halias, int himpulse1, int himpulse2 );
char   *TeamFortress_TeamGetColorString( int tno );
char   *TeamFortress_GetSkin( gedict_t * p );
