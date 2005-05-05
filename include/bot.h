/*
 *  QWProgs-TF2003
 *  Copyright (C) 2004  [sd] angel
 *
 *
 *  This program is free software you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *
 *  $Id: bot.h,v 1.2 2005-05-05 14:51:43 AngelD Exp $
 */

#define KEY_MOVEUP       1
#define KEY_MOVEDOWN     2
#define KEY_MOVELEFT     4
#define KEY_MOVERIGHT    8
#define KEY_MOVEFORWARD 16
#define KEY_MOVEBACK    32
#define KEY_LOOKUP      64
#define KEY_LOOKDOWN   128
#define KEY_LOOKLEFT   256
#define KEY_LOOKRIGHT  512

#define BOT_IDLE                1
#define BOT_FIGHTING            2
#define BOT_MOVING              4
#define BOT_DEFEND              8
#define BOT_RESUPPLYING         16
#define BOT_AVOIDING_HAZARD     32
/*
#define BOT_AI_LEAD             2
#define BOT_AI_FOLLOW           3
#define BOT_AI_REFUEL           4
#define BOT_AI_GRAPPLE          5
#define BOT_AI_FLEE             6
#define BOT_AI_BLAST            7
#define BOT_AI_STEALTH          8
#define BOT_AI_FLAGRUN          9
#define BOT_AI_SCOUT            10
*/

void BotFrame();
void botConnect(int whichteam, int whichClass);
void botDisconnect(gedict_t* te);
void Bot_AI();

void ClearAllWaypoints();
char   *PickAName();
void StrafeLeft();
void StrafeRight();
int GetKeysByDir( vec3_t sdir );
int GetKeysByLoc( vec3_t vLocation );
void GotoLocation( vec3_t vDestination );
void MoveToLocation( vec3_t vDestination );
void DoMovement();

void ReloadWeapons(  );
qboolean AllTargetsFull(  );
void AddTarget( gedict_t * targetToAdd );
void ClearTarget( int targetToClear );
void ClearAllTargets(  );
void PickBestWeapon(  );
void FindTargets(  );
void AimHere( vec3_t aimAtThisPoint );
void InstantAim(  );
void LeadAim( float fDistance );
void DodgeEnemy( float fDistance );
void DodgeEnemyGrenades(  );
void HandleGrens(  );
void DitchGrens(  );
void UseGren(  );
void AttackVisible(  );
void AttackTargets(  );

qboolean CheckForResupply();
void GoForResupply();
int BotImpulse();
int BotImpulseMenu();
