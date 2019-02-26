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
 *  $Id: player.h,v 1.4 2005-05-16 09:35:43 AngelD Exp $
 */
#include "spy.h"

void ThrowGib( char *gibname, float dm );
void Headless_Think();
void player_stand1();
void player_run();
void player_touch();

void player_naxe( int frame, int wframe, th_die_func_t f );
void player_shot(int start);

void player_autorifle1();
void player_autorifle2();
void player_autorifle3();

void player_chain1();
void player_chain1a();
void player_chain2();
void player_chain2a();
void player_chain3();
void player_chain4();
void player_chain5();

void player_nail1();
void player_nail2();

void player_assaultcannonup1();
void player_assaultcannonup2();
void player_assaultcannon1();
void player_assaultcannon2();
void player_assaultcannondown1();

void player_light1();
void player_light2();


void player_pain( gedict_t * attacker, float take  );

void DeathBubblesSpawn();
void DeathSound();
void PlayerDead();
void KillPlayer();
void GibPlayer();
void PlayerDie();

void set_suicide_frame();
void ParseUserInfo();
void Client_Set_Cmd();

void ApplyDmgRoll( gedict_t* self );

