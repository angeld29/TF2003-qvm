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

#ifndef G_EXT_H
#define G_EXT_H

// Механизм именованных расширений двигателя (G_Map_Extension, слоты >= 256).
// Порядок слотов совпадает с ktx-orig, G_VISIBLETO добавлен из fteqw.
// Слоты не конфликтуют с gameImport_t (заканчивается на _G__LASTAPI).

#define G_EXTENSIONS_FIRST 256

enum
{
	G_SETEXTFIELD = G_EXTENSIONS_FIRST,   // 256
	G_GETEXTFIELD,                        // 257
	G_CHANGELEVEL_HUB,                    // 258
	G_URI_QUERY,                          // 259
	G_PARTICLEEFFECTNUM,                  // 260
	G_TRAILPARTICLES,                     // 261
	G_POINTPARTICLES,                     // 262
	G_CLIENTSTAT,                         // 263
	G_POINTERSTAT,                        // 264
	G_MAPEXTFIELDPTR,                     // 265
	G_SETEXTFIELDPTR,                     // 266
	G_GETEXTFIELDPTR,                     // 267
	G_SETSENDNEEDED,                      // 268
	G_VISIBLETO,                          // 269
	G_EXTENSIONS_LAST
};

extern qboolean g_ext_available[G_EXTENSIONS_LAST - G_EXTENSIONS_FIRST];
#define HAVEEXT(slot) g_ext_available[(slot) - G_EXTENSIONS_FIRST]

// Базовые пути доступа к расширенным полям сущностей.
// mvdsv: Ptr-путь (MapExtFieldPtr/SetExtFieldPtr/GetExtFieldPtr).
// fteqw: Field-путь (SetExtField/GetExtField).
// Запуск возможен, если доступен хотя бы один из путей.
#define G_EXT_BASE_PTR_OK   (HAVEEXT(G_MAPEXTFIELDPTR) && HAVEEXT(G_SETEXTFIELDPTR) && HAVEEXT(G_GETEXTFIELDPTR))
#define G_EXT_BASE_FIELD_OK (HAVEEXT(G_SETEXTFIELD) && HAVEEXT(G_GETEXTFIELD))
#define G_EXT_BASE_OK       (G_EXT_BASE_PTR_OK || G_EXT_BASE_FIELD_OK)

// Регистрация расширений; вызывается один раз при GAME_INIT.
// Отсутствие помеченных критичными расширений (и отсутствие обоих базовых
// путей) приводит к фатальной ошибке при запуске.
void G_InitExtensions(void);

// Guarded-обёртки трапов: проверяют доступность расширения перед вызовом.
// Вызов неотмапленного слота на некоторых движках (fteqw) — крах, поэтому
// напрямую трапы вне g_ext.c вызывать нельзя.

intptr_t  G_Ext_SetExtField(gedict_t *ed, const char *fieldname, intptr_t val);  // 0 — расширение недоступно
qboolean  G_Ext_GetExtField(gedict_t *ed, const char *fieldname, intptr_t *out); // false — недоступно
void      G_Ext_ChangeLevelHub(const char *name, const char *entityname, const char *startspot);
int       G_Ext_URI_Query(const char *uri, int vmentry, void *cbcontext, const char *mimetype, const char *data, intptr_t datasize);
int       G_Ext_ParticleEffectNum(const char *effectname);                        // 0 — недоступно/не найдено
void      G_Ext_TrailParticles(int effecttype, int entnum, float start_x, float start_y, float start_z, float end_x, float end_y, float end_z);
void      G_Ext_PointParticles(int effecttype, float org_x, float org_y, float org_z, float vel_x, float vel_y, float vel_z, int count);
void      G_Ext_ClientStat(int statidx, int stattype, int fieldoffset);
void      G_Ext_PointerStat(int statidx, int stattype, void *ptr);
qboolean  G_Ext_MapExtFieldPtr(const char *fieldname, intptr_t *out);            // false — недоступно
qboolean  G_Ext_SetExtFieldPtr(gedict_t *ed, intptr_t fieldref, void *data, intptr_t size);
qboolean  G_Ext_GetExtFieldPtr(gedict_t *ed, intptr_t fieldref, void *data, intptr_t size);
void      G_Ext_SetSendNeeded(gedict_t *ed, intptr_t sendflags, gedict_t *to);
qboolean  G_Ext_VisibleTo(gedict_t *viewer, gedict_t *viewee);

// Хелперы работы с расширенными полями сущностей.
// Выбирают доступный путь: Ptr (MapExtFieldPtr/Set|GetExtFieldPtr),
// иначе Field (SetExtField/GetExtField).
void ExtFieldSetAlpha(gedict_t *ed, float alpha);
float ExtFieldGetAlpha(gedict_t *ed);
void ExtFieldSetColorMod(gedict_t *ed, float r, float g, float b);
void ExtFieldSetPvsFlags(gedict_t *ed, float pvsflags);

#endif
