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
 *  $Id: sentry.h,v 1.3 2004-12-23 03:16:14 AngelD Exp $
 */
void    lvl1_sentry_stand();
void    lvl1_sentry_atk1();
void    lvl1_sentry_atk2();
void    lvl1_sentry_atk3();
void    lvl2_sentry_stand();
void    lvl2_sentry_atk1();
void    lvl2_sentry_atk2();
void    lvl2_sentry_atk3();
void    lvl3_sentry_stand();
void    lvl3_sentry_atk1();
void    lvl3_sentry_atk2();
void    lvl3_sentry_atk3();

void    Sentry_Rotate();
int     Sentry_FindTarget();
int     Sentry_FindTarget_Angel();
void    Sentry_FoundTarget();
void    Sentry_HuntTarget();
void    Sentry_Pain( struct gedict_s *attacker, float take );
void    Sentry_Die();
int     Sentry_Fire();
void    Sentry_MuzzleFlash();
void    CheckSentry( gedict_t * gunhead );
