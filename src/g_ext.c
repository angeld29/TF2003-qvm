/*
 *  QWProgs-TF2003
 *  Copyright (C) 2004  [sd] angel
 *
 *  This code is based on Q3 VM code by Id Software, Inc.
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
 *  $Id$
 */

#include "g_local.h"

// Механизм G_Map_Extension: регистрация расширений по имени, отслеживание
// доступности и guarded-обёртки. Детали в include/g_ext.h и docs/g_ext_plan.md.

typedef struct
{
	const char *name;
	int slot;
	qboolean critical;	// при отсутствии расширения — фатальная ошибка при запуске
} g_exttrap_t;

static const g_exttrap_t g_exttraps[] =
{
	{"SetExtField",        G_SETEXTFIELD,        false},
	{"GetExtField",        G_GETEXTFIELD,        false},
	{"ChangeLevelHub",     G_CHANGELEVEL_HUB,    false},
	{"URI_Query",          G_URI_QUERY,          false},
	{"particleeffectnum",  G_PARTICLEEFFECTNUM,  false},
	{"trailparticles",     G_TRAILPARTICLES,     false},
	{"pointparticles",     G_POINTPARTICLES,     false},
	{"clientstat",         G_CLIENTSTAT,         false},
	{"pointerstat",        G_POINTERSTAT,        false},
	{"MapExtFieldPtr",     G_MAPEXTFIELDPTR,     false},
	{"SetExtFieldPtr",     G_SETEXTFIELDPTR,     false},
	{"GetExtFieldPtr",     G_GETEXTFIELDPTR,     false},
	{"setsendneeded",      G_SETSENDNEEDED,      false},
	{"VisibleTo",          G_VISIBLETO,          false},
};

qboolean g_ext_available[G_EXTENSIONS_LAST - G_EXTENSIONS_FIRST];

void G_InitExtensions(void)
{
	int i;

	for (i = 0; i < (int)(sizeof(g_exttraps) / sizeof(g_exttraps[0])); i++)
	{
		intptr_t r = trap_Map_Extension(g_exttraps[i].name, g_exttraps[i].slot);
		HAVEEXT(g_exttraps[i].slot) = (r == g_exttraps[i].slot);
		if (!HAVEEXT(g_exttraps[i].slot))
			G_conprintf("Extension '%s' not available (%ld)\n", g_exttraps[i].name, (long)r);
	}

	// Расширения, помеченные критичными: отсутствие — фатально.
	for (i = 0; i < (int)(sizeof(g_exttraps) / sizeof(g_exttraps[0])); i++)
	{
		if (g_exttraps[i].critical && !HAVEEXT(g_exttraps[i].slot))
			G_Error("Required extension '%s' is not available\n", g_exttraps[i].name);
	}

	// Должен быть доступен хотя бы один базовый путь к расширенным полям:
	// Ptr (mvdsv) или Field (fteqw). Иначе мод не сможет работать нигде.
	if (!G_EXT_BASE_OK)
		G_Error("No extended entity fields support: need MapExtFieldPtr/SetExtFieldPtr/GetExtFieldPtr or SetExtField/GetExtField\n");
}

//===========================================================================
// Guarded-обёртки трапов.
// Вызов неотмапленного слота на некоторых движках (fteqw) — крах,
// поэтому всегда проверяем HAVEEXT до вызова.
//===========================================================================

intptr_t G_Ext_SetExtField(gedict_t *ed, const char *fieldname, intptr_t val)
{
	if (!HAVEEXT(G_SETEXTFIELD))
	{
		if (cvar("developer"))
			G_dprintf("SetExtField not available\n");
		return 0;
	}
	trap_SetExtField(ed, fieldname, val);
	return 1;
}

qboolean G_Ext_GetExtField(gedict_t *ed, const char *fieldname, intptr_t *out)
{
	if (!HAVEEXT(G_GETEXTFIELD))
	{
		if (out)
			*out = 0;
		if (cvar("developer"))
			G_dprintf("GetExtField not available\n");
		return false;
	}
	if (out)
		*out = trap_GetExtField(ed, fieldname);
	return true;
}

void G_Ext_ChangeLevelHub(const char *name, const char *entityname, const char *startspot)
{
	if (!HAVEEXT(G_CHANGELEVEL_HUB))
	{
		if (cvar("developer"))
			G_dprintf("ChangeLevelHub not available\n");
		return;
	}
	trap_changelevelHub(name, entityname, startspot);
}

int G_Ext_URI_Query(const char *uri, int vmentry, void *cbcontext, const char *mimetype, const char *data, intptr_t datasize)
{
	if (!HAVEEXT(G_URI_QUERY))
	{
		if (cvar("developer"))
			G_dprintf("URI_Query not available\n");
		return 0;
	}
	return trap_URI_Query(uri, vmentry, cbcontext, mimetype, data, datasize);
}

int G_Ext_ParticleEffectNum(const char *effectname)
{
	if (!HAVEEXT(G_PARTICLEEFFECTNUM))
	{
		if (cvar("developer"))
			G_dprintf("particleeffectnum not available\n");
		return 0;
	}
	return trap_particleeffectnum(effectname);
}

void G_Ext_TrailParticles(int effecttype, int entnum, float start_x, float start_y, float start_z, float end_x, float end_y, float end_z)
{
	if (!HAVEEXT(G_TRAILPARTICLES))
	{
		if (cvar("developer"))
			G_dprintf("trailparticles not available\n");
		return;
	}
	trap_trailparticles(effecttype, entnum, start_x, start_y, start_z, end_x, end_y, end_z);
}

void G_Ext_PointParticles(int effecttype, float org_x, float org_y, float org_z, float vel_x, float vel_y, float vel_z, int count)
{
	if (!HAVEEXT(G_POINTPARTICLES))
	{
		if (cvar("developer"))
			G_dprintf("pointparticles not available\n");
		return;
	}
	trap_pointparticles(effecttype, org_x, org_y, org_z, vel_x, vel_y, vel_z, count);
}

void G_Ext_ClientStat(int statidx, int stattype, int fieldoffset)
{
	if (!HAVEEXT(G_CLIENTSTAT))
	{
		if (cvar("developer"))
			G_dprintf("clientstat not available\n");
		return;
	}
	trap_clientstat(statidx, stattype, fieldoffset);
}

void G_Ext_PointerStat(int statidx, int stattype, void *ptr)
{
	if (!HAVEEXT(G_POINTERSTAT))
	{
		if (cvar("developer"))
			G_dprintf("pointerstat not available\n");
		return;
	}
	trap_pointerstat(statidx, stattype, ptr);
}

qboolean G_Ext_MapExtFieldPtr(const char *fieldname, intptr_t *out)
{
	if (!HAVEEXT(G_MAPEXTFIELDPTR))
	{
		if (out)
			*out = 0;
		if (cvar("developer"))
			G_dprintf("MapExtFieldPtr not available\n");
		return false;
	}
	if (out)
		*out = trap_MapExtFieldPtr(fieldname);
	return true;
}

qboolean G_Ext_SetExtFieldPtr(gedict_t *ed, intptr_t fieldref, void *data, intptr_t size)
{
	if (!HAVEEXT(G_SETEXTFIELDPTR))
	{
		if (cvar("developer"))
			G_dprintf("SetExtFieldPtr not available\n");
		return false;
	}
	return trap_SetExtFieldPtr(ed, fieldref, data, size) != 0;
}

qboolean G_Ext_GetExtFieldPtr(gedict_t *ed, intptr_t fieldref, void *data, intptr_t size)
{
	if (!HAVEEXT(G_GETEXTFIELDPTR))
	{
		if (cvar("developer"))
			G_dprintf("GetExtFieldPtr not available\n");
		return false;
	}
	return trap_GetExtFieldPtr(ed, fieldref, data, size) != 0;
}

void G_Ext_SetSendNeeded(gedict_t *ed, intptr_t sendflags, gedict_t *to)
{
	if (!HAVEEXT(G_SETSENDNEEDED))
	{
		if (cvar("developer"))
			G_dprintf("setsendneeded not available\n");
		return;
	}
	trap_SetSendNeeded(NUM_FOR_EDICT(ed), sendflags, to ? NUM_FOR_EDICT(to) : 0);
}

qboolean G_Ext_VisibleTo(gedict_t *viewer, gedict_t *viewee)
{
	if (!HAVEEXT(G_VISIBLETO))
	{
		if (cvar("developer"))
			G_dprintf("VisibleTo not available\n");
		return false;
	}
	return trap_VisibleTo(NUM_FOR_EDICT(viewer), NUM_FOR_EDICT(viewee)) != 0;
}

//===========================================================================
// Хелперы работы с расширенными полями сущностей.
// Токены (cookie движка) нельзя хардкодить — получаем в рантайме и кэшируем.
//===========================================================================

typedef union fi_s
{
	float _float;
	intptr_t _int;
} fi_t;

static intptr_t field_ref_alpha = 0;
static intptr_t field_ref_colormod = 0;
static intptr_t field_ref_pvsflags = 0;

void ExtFieldSetAlpha(gedict_t *ed, float alpha)
{
	if (alpha < 0.0f)
		alpha = 0.0f;
	else if (alpha > 1.0f)
		alpha = 1.0f;

	if (!field_ref_alpha && G_EXT_BASE_PTR_OK)
		field_ref_alpha = trap_MapExtFieldPtr("alpha");

	if (field_ref_alpha)
	{
		trap_SetExtFieldPtr(ed, field_ref_alpha, (void *)&alpha, sizeof(float));
	}
	else if (G_EXT_BASE_FIELD_OK)
	{
		fi_t v;
		v._float = alpha;
		trap_SetExtField(ed, "alpha", v._int);
	}
	else if (cvar("developer"))
	{
		G_dprintf("ExtFieldSetAlpha: no extended fields support in server\n");
	}
}

float ExtFieldGetAlpha(gedict_t *ed)
{
	fi_t tmp;
	tmp._float = -1.0f;

	if (!field_ref_alpha && G_EXT_BASE_PTR_OK)
		field_ref_alpha = trap_MapExtFieldPtr("alpha");

	if (field_ref_alpha)
	{
		trap_GetExtFieldPtr(ed, field_ref_alpha, (void *)&tmp._float, sizeof(float));
	}
	else if (G_EXT_BASE_FIELD_OK)
	{
		tmp._int = trap_GetExtField(ed, "alpha");
	}
	else if (cvar("developer"))
	{
		G_dprintf("ExtFieldGetAlpha: no extended fields support in server\n");
	}
	return tmp._float;
}

void ExtFieldSetColorMod(gedict_t *ed, float r, float g, float b)
{
	if (!field_ref_colormod && G_EXT_BASE_PTR_OK)
		field_ref_colormod = trap_MapExtFieldPtr("colormod");

	if (field_ref_colormod)
	{
		float rgb[3];
		rgb[0] = (r > 0.0f) ? r : 0.0f;
		rgb[1] = (g > 0.0f) ? g : 0.0f;
		rgb[2] = (b > 0.0f) ? b : 0.0f;
		trap_SetExtFieldPtr(ed, field_ref_colormod, (void *)rgb, sizeof(rgb));
	}
	else if (cvar("developer"))
	{
		G_dprintf("ExtFieldSetColorMod: needs MapExtFieldPtr and SetExtFieldPtr support\n");
	}
}

void ExtFieldSetPvsFlags(gedict_t *ed, float pvsflags)
{
	if (!field_ref_pvsflags && G_EXT_BASE_PTR_OK)
		field_ref_pvsflags = trap_MapExtFieldPtr("pvsflags");

	if (field_ref_pvsflags)
	{
		trap_SetExtFieldPtr(ed, field_ref_pvsflags, (void *)&pvsflags, sizeof(float));
	}
	else if (G_EXT_BASE_FIELD_OK)
	{
		fi_t tmp;
		tmp._float = pvsflags;
		trap_SetExtField(ed, "pvsflags", tmp._int);
	}
	else if (cvar("developer"))
	{
		G_dprintf("ExtFieldSetPvsFlags: needs MapExtFieldPtr and SetExtFieldPtr support\n");
	}
}
