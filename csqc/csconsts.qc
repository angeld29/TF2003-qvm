// csconsts.qc — константы клиента TF2003 (зеркало серверных).
// WEAP_* биты — из include/g_tf_consts.h:339-358; приходят в STAT_ACTIVEWEAPON
// (ent->v->weapon). Это битмаска, НЕ номер слота.

// Оружие (битмаска).
const float WEAP_HOOK              = 1;
const float WEAP_BIOWEAPON         = 2;
const float WEAP_MEDIKIT           = 4;
const float WEAP_SPANNER           = 8;
const float WEAP_AXE               = 16;
const float WEAP_SNIPER_RIFLE      = 32;
const float WEAP_AUTO_RIFLE        = 64;
const float WEAP_SHOTGUN           = 128;
const float WEAP_SUPER_SHOTGUN     = 256;
const float WEAP_NAILGUN           = 512;
const float WEAP_SUPER_NAILGUN     = 1024;
const float WEAP_GRENADE_LAUNCHER  = 2048;
const float WEAP_FLAMETHROWER      = 4096;
const float WEAP_ROCKET_LAUNCHER   = 8192;
const float WEAP_INCENDIARY        = 16384;
const float WEAP_ASSAULT_CANNON    = 32768;
const float WEAP_LIGHTNING         = 65536;
const float WEAP_DETPACK           = 131072;
const float WEAP_TRANQ             = 262144;
const float WEAP_LASER             = 524288;

// Типы аммо.
const float AMMO_SHELLS = 0;
const float AMMO_NAILS  = 1;
const float AMMO_ROCKETS= 2;
const float AMMO_CELLS  = 3;
const float AMMO_NONE   = -1;

// Имя оружия по биту.
string weapon_name(float w) {
	if (w == WEAP_HOOK)               return "HOOK";
	if (w == WEAP_BIOWEAPON)          return "BIO";
	if (w == WEAP_MEDIKIT)            return "MEDIKIT";
	if (w == WEAP_SPANNER)            return "SPANNER";
	if (w == WEAP_AXE)                return "AXE";
	if (w == WEAP_SNIPER_RIFLE)       return "SNIPER";
	if (w == WEAP_AUTO_RIFLE)         return "AUTORIFLE";
	if (w == WEAP_SHOTGUN)            return "SHOTGUN";
	if (w == WEAP_SUPER_SHOTGUN)      return "SUPER SHOTGUN";
	if (w == WEAP_NAILGUN)            return "NAILGUN";
	if (w == WEAP_SUPER_NAILGUN)      return "SUPER NAILGUN";
	if (w == WEAP_GRENADE_LAUNCHER)   return "GRENADE";
	if (w == WEAP_FLAMETHROWER)       return "FLAMETHROWER";
	if (w == WEAP_ROCKET_LAUNCHER)    return "ROCKET";
	if (w == WEAP_INCENDIARY)         return "INCENDIARY";
	if (w == WEAP_ASSAULT_CANNON)     return "AC";
	if (w == WEAP_LIGHTNING)          return "LIGHTNING";
	if (w == WEAP_DETPACK)            return "DETPACK";
	if (w == WEAP_TRANQ)              return "TRANQ";
	if (w == WEAP_LASER)              return "LASER";
	return ftos(w);
}

// Тип аммо оружия по биту (для отображения текущего).
float weapon_ammo(float w) {
	if (w == WEAP_SHOTGUN || w == WEAP_SUPER_SHOTGUN || w == WEAP_AUTO_RIFLE)
		return AMMO_SHELLS;
	if (w == WEAP_NAILGUN || w == WEAP_SUPER_NAILGUN ||
	    w == WEAP_SNIPER_RIFLE || w == WEAP_TRANQ)
		return AMMO_NAILS;
	if (w == WEAP_GRENADE_LAUNCHER || w == WEAP_ROCKET_LAUNCHER)
		return AMMO_ROCKETS;
	if (w == WEAP_FLAMETHROWER || w == WEAP_INCENDIARY ||
	    w == WEAP_ASSAULT_CANNON || w == WEAP_LIGHTNING)
		return AMMO_CELLS;
	return AMMO_NONE;
}

//===========================================================================
// Кастомные статы (зеркало серверных G_RegisterClientStat, g_csqc.h).
//===========================================================================

// Текущее оружие: точный WEAP_* бит (серверный стат 35 = current_weapon).
const float STAT_WEAPON_TF = 35;

//===========================================================================
// IT_* биты в STAT_ITEMS (s.v.items). Активное оружие идёт через бит СЛОТА
// (wi->bit_item, weapons.c), НЕ через STAT_ACTIVEWEAPON (TF2003 ставит .weapon=0).
// STAT_ITEMS также несёт тип аммо активного оружия, тип брони и powerups.
// Значения — из include/g_consts.h:84-111.
//===========================================================================

// Слоты оружия.
const float IT_SHOTGUN           = 1;
const float IT_SUPER_SHOTGUN     = 2;
const float IT_NAILGUN           = 4;
const float IT_SUPER_NAILGUN     = 8;
const float IT_GRENADE_LAUNCHER  = 16;
const float IT_ROCKET_LAUNCHER   = 32;
const float IT_LIGHTNING         = 64;
const float IT_AXE               = 4096;
const float IT_WEAPON_MASK = IT_SHOTGUN | IT_SUPER_SHOTGUN | IT_NAILGUN |
	IT_SUPER_NAILGUN | IT_GRENADE_LAUNCHER | IT_ROCKET_LAUNCHER |
	IT_LIGHTNING | IT_AXE;

// Тип аммо активного оружия (чистится+ставится при каждом переключении).
const float IT_SHELLS  = 256;
const float IT_NAILS   = 512;
const float IT_ROCKETS = 1024;
const float IT_CELLS   = 2048;
const float IT_AMMO_MASK = IT_SHELLS | IT_NAILS | IT_ROCKETS | IT_CELLS;

// Тип брони.
const float IT_ARMOR1 = 8192;
const float IT_ARMOR2 = 16384;
const float IT_ARMOR3 = 32768;
const float IT_ARMOR_MASK = IT_ARMOR1 | IT_ARMOR2 | IT_ARMOR3;

// Powerups.
const float IT_SUPERHEALTH    = 65536;
const float IT_KEY1           = 131072;
const float IT_KEY2           = 262144;
const float IT_INVISIBILITY   = 524288;
const float IT_INVULNERABILITY= 1048576;
const float IT_SUIT           = 2097152;
const float IT_QUAD           = 4194304;
const float IT_HOOK           = 8388608;

// Имя СЛОТА оружия по биту из STAT_ITEMS.
string weapon_slot_name(float bit) {
	if (bit == IT_AXE)               return "AXE";
	if (bit == IT_SHOTGUN)           return "SHOTGUN";
	if (bit == IT_SUPER_SHOTGUN)     return "SUPER SHOTGUN";
	if (bit == IT_NAILGUN)           return "NAILGUN";
	if (bit == IT_SUPER_NAILGUN)     return "SUPER NAILGUN";
	if (bit == IT_GRENADE_LAUNCHER)  return "GRENADE";
	if (bit == IT_ROCKET_LAUNCHER)   return "ROCKET";
	if (bit == IT_LIGHTNING)         return "LIGHTNING";
	return ftos(bit);
}

// Слот активного оружия из STAT_ITEMS (старший установленный бит маски).
float active_weapon_slot(float items) {
	float i, mask = IT_WEAPON_MASK, bit = 0;
	for (i = 1; i <= IT_LIGHTNING; i *= 2)
		if ((mask & (int)i) && ((int)items & (int)i))
			bit = i;
	if ((int)items & (int)IT_AXE)
		bit = IT_AXE;
	return bit;
}

// Тип аммо активного оружия из STAT_ITEMS (бит IT_*).
string ammo_type_name(float items) {
	if ((int)items & (int)IT_SHELLS)  return "SHELLS";
	if ((int)items & (int)IT_NAILS)   return "NAILS";
	if ((int)items & (int)IT_ROCKETS) return "ROCKETS";
	if ((int)items & (int)IT_CELLS)   return "CELLS";
	return "";
}

// Тип брони из STAT_ITEMS.
string armor_type_name(float items) {
	if ((int)items & (int)IT_ARMOR3)  return "A3";
	if ((int)items & (int)IT_ARMOR2)  return "A2";
	if ((int)items & (int)IT_ARMOR1)  return "A1";
	return "";
}

// Powerups, активные сейчас (короткие метки).
string powerup_names(float items) {
	string s = "";
	if ((int)items & (int)IT_QUAD)            s = strcat(s, "Q");
	if ((int)items & (int)IT_INVULNERABILITY) s = strcat(s, "I");
	if ((int)items & (int)IT_INVISIBILITY)    s = strcat(s, "V");
	if ((int)items & (int)IT_SUIT)            s = strcat(s, "S");
	return s;
}

// Имя оружия по модели вьюмодели (STAT_WEAPONMODELI → modelnameforindex).
// Модели из weapons_info (src/weapons.c); точные имена по s.v.weaponmodel.
string weapon_name_by_model(string mdl) {
	if (strstrofs(mdl, "v_axe.mdl") != -1)      return "AXE";
	if (strstrofs(mdl, "v_knife.mdl") != -1)    return "KNIFE";
	if (strstrofs(mdl, "v_grap.mdl") != -1)     return "HOOK";
	if (strstrofs(mdl, "v_span.mdl") != -1)     return "SPANNER";
	if (strstrofs(mdl, "v_bio.mdl") != -1)      return "MEDIKIT";
	if (strstrofs(mdl, "v_srifle.mdl") != -1)   return "SNIPER";
	if (strstrofs(mdl, "v_shot.mdl") != -1)     return "SHOTGUN";
	if (strstrofs(mdl, "v_shot2.mdl") != -1)    return "SUPER SHOTGUN";
	if (strstrofs(mdl, "v_nail.mdl") != -1)     return "NAILGUN";
	if (strstrofs(mdl, "v_nail2.mdl") != -1)    return "SUPER NAILGUN";
	if (strstrofs(mdl, "v_rock.mdl") != -1)     return "GRENADE";
	if (strstrofs(mdl, "v_pipe.mdl") != -1)     return "GRENADE";
	if (strstrofs(mdl, "v_rock2.mdl") != -1)    return "ROCKET";
	if (strstrofs(mdl, "v_flame.mdl") != -1)    return "FLAMETHROWER";
	if (strstrofs(mdl, "v_asscan.mdl") != -1)   return "AC";
	if (strstrofs(mdl, "v_light.mdl") != -1)    return "LIGHTNING";
	if (strstrofs(mdl, "v_tgun.mdl") != -1)     return "TRANQ";
	if (strstrofs(mdl, "v_rail.mdl") != -1)     return "LASER";
	return "";
}
