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
 *  $Id: tfort.h,v 1.12 2005-05-28 19:03:44 AngelD Exp $
 */

//tfort.c
const   char   *TeamFortress_GetClassName( int pc );
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
void    TeamFortress_SetColor( gedict_t * p, int top, int bottom );
void    TeamFortress_ExecClassScript( gedict_t * );
void    TeamFortress_ExecMapScript( gedict_t * p );
int     TeamFortress_TeamPutPlayerInTeam(  );
void    KickCheater( gedict_t * p );
void    TeamFortress_Alias( char *halias, int himpulse1, int himpulse2 );
const char   *TeamFortress_TeamGetColorString( int tno );
const char   *TeamFortress_GetSkin( gedict_t * p );
const char* TeamFortress_GetSkinByTeamClass( int tn, int pc );
void    TeamFortress_AmmoboxRemove(  );

//tforttm.c
void    TeamFortress_TeamShowMemberClasses( gedict_t * Player );
int     TeamFortress_TeamGetNoPlayers( int tno );
int     TeamFortress_TeamGetMaxPlayers( int tno );
int     TeamFortress_TeamGetLives( int tno );
void    TeamFortress_DisplayLegalClasses(  );
void    TeamFortress_Inventory(  );
void    TeamFortress_SaveMe(  );
void    TeamFortress_ID(  );
void    TeamFortress_ShowIDs(  );
void    TeamFortress_ShowTF(  );
void    TeamFortress_SniperWeapon(  );
void    TeamFortress_AssaultWeapon(  );
void    TeamFortress_IncendiaryCannon(  );
void    TeamFortress_FlameThrower(  );
void    TeamFortress_PrimeGrenade(  );
void    TeamFortress_ThrowGrenade(  );
void    TeamFortress_Discard(  );
void    TeamFortress_DetonatePipebombs(  );
void    TeamFortress_ReloadCurrentWeapon(  );
void    TeamFortress_AutoZoomToggle(  );
void    TeamFortress_StatusQuery(  );
void    TeamFortress_SpyGoUndercover(  );
void    TeamFortress_EngineerBuild(  );
void    TeamFortress_TeamShowMemberClasses_New( gedict_t * Player );
void    TeamFortress_CTF_FlagInfo(  );
void    TeamFortress_MOTD(  );
void    TeamFortress_HelpMap(  );
void    TF_MovePlayer(  );
void    DumpClanScores(  );
void    DropGoalItems(  );

//classes
void    ConcussionGrenadeTimer(  );
void    OldConcussionGrenadeTimer(  );
void    PipebombTouch(  );
void    TeamFortress_DetpackStop(  );
void    SniperSight_Create(  );
void    DropKey(  );
void    UseSpecialSkill(  );
void    RemoveFlare(  );
void    RemoveOldFlare( int tno );
//void    TeamFortress_Scan( float scanrange );
void TeamFortress_ScannerSet( int impulse );
void    TeamFortress_Scan_Angel( int scanrange, int typescan );
void    TeamFortress_SetDetpack( float timer );
void    Engineer_UseDispenser( gedict_t * disp );
void    Engineer_UseSentryGun( gedict_t * gun );
void    Engineer_UseMortar( gedict_t * mortar );
void    BioInfection_Decay(  );
void    BioInfection_MonsterDecay(  );
void    ScannerSwitch(  );
void    Engineer_RemoveBuildings(  );
void    DestroyBuilding( gedict_t * eng, char *bld );
void    TeamFortress_Build( int objtobuild );
void    TF_zoom( int zoom_level );
void    CheckBelowBuilding( gedict_t * bld );

//admin
void    CTF_FlagCheck(  );
void    BirthdayTimer(  );
void    CeaseFire_think(  );
void    PreMatch_Think(  );
void    Admin_CountPlayers(  );
void    Admin_CycleKick(  );
void    Admin_DoKick(  );
void    Admin_CeaseFire(  );

//tforttm
float   TeamEqualiseDamage( gedict_t * targ, gedict_t * attacker, float damage );
const   char   *GetTeamName( int tno );
int     TeamFortress_TeamGetWinner(  );
int     TeamFortress_TeamGetScore( int tno );
int     TeamFortress_TeamGetSecond(  );
void    TeamFortress_CheckTeamCheats(  );
void    TeamFortress_DetpackExplode(  );
int     TeamFortress_TeamGetColor( int tno );
int     TeamFortress_TeamGetTopColor( int tno );
int     TeamFortress_GetNoPlayers(  );
qboolean TeamFortress_TeamIsCivilian( int tno );
int     ClassIsRestricted( int tno, int pc );
void    SetTeamName( gedict_t * p );
int     TeamFortress_TeamSet( int tno );
void    TeamFortress_TeamShowScores( int all );
void    teamsprint( int tno, gedict_t * ignore, char *st );

//tfortmap.c
void    DisplayItemStatus( gedict_t * Goal, gedict_t * Player, gedict_t * Item );
gedict_t *Finditem( int );
gedict_t *Findgoal( int );
int     Activated( gedict_t * Goal, gedict_t * AP );
void    AttemptToActivate( gedict_t * Goal, gedict_t * AP, gedict_t * ActivatingGoal );
int     CheckExistence(  );
void    ParseTFDetect( gedict_t * AD );
void    SetupTeamEqualiser(  );
void    tfgoalitem_GiveToPlayer( gedict_t * Item, gedict_t * AP, gedict_t * Goal );
void    tfgoalitem_RemoveFromPlayer( gedict_t * Item, gedict_t * AP, int method );
void    DoResults( gedict_t * Goal, gedict_t * AP, float addb );
void    TeamFortress_DisplayDetectionItems(  );
void    RemoveGoal( gedict_t * Goal );
void    tfgoal_touch(  );
void    item_tfgoal_touch(  );
void    tfgoal_timer_tick(  );
void    tfgoalitem_remove(  );
void    info_tfgoal_use(  );
void    DoRespawn(  );
void    TeamFortress_TeamIncreaseScore( int tno, int scoretoadd );
void    TF_PlaceItem(  );
void    TF_StartItem(  );
void    TF_PlaceGoal(  );
void    TF_StartGoal(  );
gedict_t *Findteamspawn( int gno );
void    InactivateGoal( gedict_t * Goal );
void    RestoreGoal( gedict_t * Goal );
int     IsAffectedBy( gedict_t * Goal, gedict_t * Player, gedict_t * AP );
void    Apply_Results( gedict_t * Goal, gedict_t * Player, gedict_t * AP, float addb );
int     APMeetsCriteria( gedict_t * Goal, gedict_t * AP );
void    SetupRespawn( gedict_t * Goal );
void    DoItemGroupWork( gedict_t * Item, gedict_t * AP );
void    DoTriggerWork( gedict_t * Goal, gedict_t * AP );
void    RemoveResults( gedict_t * Goal, gedict_t * Player );
void    tfgoalitem_drop( gedict_t * Item, float PAlive, gedict_t * P );
void    tfgoalitem_checkgoalreturn( gedict_t * Item );
void    ReturnItem(  );
void    ForceRespawn( gedict_t * P );
void    execute_changelevel(  );
gedict_t *SelectSpawnPoint(  );
void    TeamFortress_Regenerate(  );
void    TeamFortress_RegenerateCells(  );
void    TeamFortress_ExplodePerson(  );
void    TeamFortress_AmmoboxTouch(  );
void    FlareGrenadeExplode(  );
void    FlareGrenadeTouch(  );
void    NormalGrenadeTouch(  );
void    NormalGrenadeExplode(  );
void    ConcussionGrenadeTouch(  );
void    ConcussionGrenadeExplode(  );
void    NailGrenadeTouch(  );
void    NailGrenadeExplode(  );
void    MirvGrenadeTouch(  );
void    MirvGrenadeExplode(  );
void    NapalmGrenadeTouch(  );
void    NapalmGrenadeExplode(  );
void    GasGrenadeTouch(  );
void    GasGrenadeExplode(  );
void    EMPGrenadeTouch(  );
void    EMPGrenadeExplode(  );
void    FlashGrenadeTouch(  );
void    FlashGrenadeExplode(  );
void    CaltropTouch(  );
void    ScatterCaltrops(  );
void    CanisterTouch(  );
int     TeamFortress_TeamGetIllegalClasses( int tno );
void    increment_team_ammoboxes( int tno );
void    decrement_team_ammoboxes( int tno );
void    TeamFortress_AutoID(  );
void    DoGoalWork( gedict_t * Goal, gedict_t * AP );
void    DoGroupWork( gedict_t * Goal, gedict_t * AP );
int     TeamFortress_GetMaxAmmo( gedict_t * Retriever, int AmmoType );
int     TeamFortress_CanGetWeapon( gedict_t * Retriever, int WeaponType );

qboolean TeamFortress_isTeamsAllied(int teamno1, int teamno2);

void TeamFortress_PrepareForArenaRespawn(  );
