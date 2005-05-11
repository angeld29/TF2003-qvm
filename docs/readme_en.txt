QuakeWorld TeamFortress 2003
by [sd] angel for MTFL

Based on TF 2.8.1, using code from TF 2.8.1b1, TF 2.9, Clan TF 3.0,
Dragon TF, Omi TF and some original developments.

The full list of updates as on August 20, 2004.
[fix] - various bug fixes
[add] - minor but useful add-ons
[new] - innovations
[opt] - optional features

Scout:
[fix] Command "discard" drops cells also.
      // "discard" is supposed to drop all unused ammo.
      In some TF versions cells are used for scanner, but TF 2.8 abandoned it.
[fix] Dead scout can't disarm detpacks.
      // Formerly, it was possible to start detpack disarming while laying dead. If you respawn
      after that, detpack would be disarmed.
[fix] Corrected and completed class help.
      // It's up-to-date now.
[fix] Scanner correctly points at nearest enemy.
      // Formerly, it chose random enemy within specified range and pointed at him only.
      Whenever chosen enemy was out of range, scanner didn't work despite of fact that other
      enemies were nearby.
[fix] Fixed bug with scan distance change.
      // Formerly, all scans were working within same scanrange.  From now on, scan range could
      actually be altered by appropriate aliases.
[add] Added new alias "scan50", virtually replacing old "scan100".
      // Since scan of more than 50 does not work (and never did).  Anyway, "scan100" alias
      stays for config compatibility.
[add] Added aliases "scane" and "scanf", to scan for enenmies and teammates, respectively.
      // So far, pre-impulse should be used for this task.
[add] Scaner could point at more than one player.
      // This possibility supposed to be in TF from the beginning. However, it was implemented
      in such way, that client didn't drew more than one lightning although was supposed to.
[add] Scaner could show one target only, the nearest one.
      Enabled with either "setinfo ms off" or "setinfo multiscan off".
      // For those who prefer old-style scaner.
[new] Built in automatic scaner. Turned on/off by "autoscan" command.  Works once a halfsecond.
      Range is either 30 or last used scanrange.
      // So far, it could be accomplished by binding scan commands to movement keys, which led
      to excess packet transmitting and, possibly, other impulses loss.
[new] Console messages "Scanning.." and "No blips.." do not show up while autoscanning.
      // There should be no excess spam in console.
[new] Autoscan status (off/on) and all scan settings are displayed on status bar.
      // It's just convenient.
[new] New impulses introduced for pre-impulse 159:
      162 - scanf_off  "Friendly Scanning disabled."
      163 - scanf_on  "Friendly Scanning enabled."
      164 - scane_off  "Enemy Scanning disabled."
      165 - scane_on  "Enemy Scanning enabled."
      166 - autoscan_off  "Scanner off."                 
      167 - autoscan_on  "Scanner on."
      // For convenient scaner tuning.
[new] Scaner settings do not reset after death.
      // It is unnecessary to write additional scripts or waste time on scaner tuning each
      time you respawn.
[opt] New flash grenade effect: screen is completely white during 1.6 senconds. During that
      period, the "id" command does not work. On qw262 server, no entity visual information is
      sent to client.
      Effect could be enabled server-side with "localinfo new_flash off" or "localinfo nf off".
      // Formerly, screen was not at full brightness, therefore on different video settings flash
      effect could look differently. From now on, flash blinds everyone identical for 1.6 seconds.
      If effect is enabled on server side, old antiflash tricks do not work anymore.

Sniper:
[fix] Corrected and completed class help.
      // It's up-to-date now.
[add] Whenever sniper round hits wall appropriate sprites are drawn.
      // Just as other weapons do.
[opt] Sniper rifle strength does not depend on player fps anymore.
      Altered on server using either "localinfo snip_fps <number>" or "localinfo sf <number>"
      // So far, player with greater fps had his rifle primed faster, then low-fps one.
      Thus, the former received advantage over the latter. From now on, all players have equal
      rifle prime time, identical to 72 fps game.
[opt] It is possible to get rid of sniper rifle range limit.
      Turned on server with either "localinfo snip_range_fix on" or "localinfo srf on".
      // Actually, sniper rifle range limit is a bug, caused by internal buffer overflow
      within TF code. However, limited range is logically valid. Fix is made optional since
      it has a significant influence upon a gameplay on some maps.
[opt] It is possible to increase sniper rifle ammo consumption.
      Set on server using "localinfo snip_ammo <number>" (default 1)
      // Formerly, sniper could handle lots of enemies standing still at one spot.
      Now he should care of his ammo amount.
[opt] It is possible to increase minimum time between two successive shots from sniper rifle.
      It does not affect other impulses.
      Turned on server with "localinfo snip_time <number>" (default 1.5)
      // Snipers on public servers will shoot a bit rarely :)

Soldier:
[fix] Removed bug causing nail grenade to fly through ceiling.
      // So far, nail grenade could be thrown in such way, that it penetrates through ceiling
      or wall (of any thickness) into nearby room.
[add] Command "special" switches between rocket launcher and super shotgun now.
      // Before, "special" was used as an alias for "reload", which is redundant.

Demoman:
[fix] Corrected class help.
      // It's up-to-date now.
[fix] Detpack is telefragged whenever player respawns into it position.
      // From now on, it's impossible to block respawn point with detpack, either accidentally
      or intentionally.
[fix] Detpack could not be moved.
      // Formerly, detpack set on some surfaces (eg button) could be dropped on the floor.
[fix] Removed bug related to MIRV spawned grenades falling through floor.
      // So far, whenever MIRV explode, some of the grenades could be spawn inside wall
      and penetrate into lower or nearby room.
[opt] Inreased minimal distance between armed detapacks in 1.5 times.
      (Old range could be reverted by setting on server "localinfo dtpb off").
      // Now, player always could walk between two detpacks set next to each other.
[add] Added new model for pipe launcher.
      // Model itself is same as grenade launcher, but it has a yellow sripe on its top.
      It's convenient and visually clear when launchers are being switched.
      In qw262 client, "cl_currentweapon" cvar set to 7 whenever pipe launcher is selected.
[new] Additional pipe for "fireworks" (one per team).
      Enabled on server side "localinfo add_pipe off".
      // If one demoman has full trap layed, and other uses one pipe for "fireworks",
      pipes from the trap will not explode. On the other hand, if they want to lay two
      traps, they still could not use more than seven pipes at the same time.
[new] Countdown for detpack displayed at status bar.
      // Before, there should be used separate timer for this purpose.
[new] Detpack status shown on "inv" command, in spite of class of player owning detpack.
      // Player could always see his detpack status.
[new] Whenever server runs altprogs, pipe limit is increased up to 30.
      // Jast as in LAN TF.
[new] Whenever server runs altprogs, MIRV splits into 12 grenades.
      // Jast as in LAN TF.

Medic:
[fix] Removed flash cure bug.
      // Since new grenades were introduced in TF 2.8, medic couldn't anymore cure
      old grenades effect. Instead of curing blindness, it was made permanent.
      From now on, he cures blindness as supposed to.
[fix] Whenever axe is selected, medikit amount is dislpayed.
      // So far, it was not displayed at all, and in TF 2.9 it was shown, but was
      not updated during expense. From now on, it works as it should.
[add] Command "special" switches between axe and super nailgun. If there is no available
      nails, it switches between axe and super shotgun.
      // In early TF versions, "special" was switching between bioweapon and medikit.
      Since these two functions were merged into one, there was no much sence in using
      this alias. From now on, it switches between axe and primary weapon.
[new] Now medic is able to cure "server-side" flash grenade effect.
      // Medic is supposed to heal :)

Heavyguy:
[fix] "item_armorinv" entity gives 300 units of red armor on all maps.
      // On some old maps (designed before hwguy was given maximum of 300 armor), he could
      not fill armor to more than 200 or 250 units. From now on, maximum amount of
      armor is equal on all maps.
[fix] Now hwguy could fire standing on the bottom of water pond.
      // Actually, it was QW server bug, not TF one. In QW, player standing still on the
      bottom of water has non-zero speed at the same time. This speed is great enough for
      hwguy unable to start shooting from fired up cannon.
[fix] Removed console spam, whenever cannon is being tried to fire up while jumping.
      // Formerly, message "not possible to shoot in air" was printed somewhat about ten
      times per second and caused to scroll away all other console messages.
[add] "special" command switches between cannon and super shotgun.
      // In earlier versions, "special" just switched weapon to cannon.
[add] Changed displaying of cannon bullet hits, actual shot spread is shown.
      // It looks much more spectacular :)
[new] Cannon damage is increased 2 times (as in TF 2.9). One half of the damage amount
      is same as in TF2.8, second half has its spread increased 3 times during 3 seconds.
      // In TF 2.8, cannon was effective at middle range, thanks to small bullet spread.
      However at close range it didn't have enough strength (if ammo expense would be
      taken into account, actual damage should be 3 times as it actually was).
      In TF 2.9, damage was increased 2 times, but large spread was introduced. The gun
      became effective on very close range only. In this version, somewhat intermediate
      variant is implemented.
[new] There is no screen shaking during fire anymore. However, concussion effect is now
      applied at full strength to hwguy.
      // So far, thanks to that shaking, hwguy was practically immune to concussion
      effect, that is, he was able to fire at the same accuracy under conc as at normal condition.
[new] It's possible to jump on grenades. However, acceleration is twice as small as other
      classes have. (Earlier acceleration was 3 times smaller).
      // Heavyguy was the only class that couldn't boost his speed using grenade blast and
      jumping somewhere high. From now on, he could accelerate and jump at moderate level.
[opt] It's possible to set minimal shells amount, when reached, cannon is automatically
      switched to super shotgun: "setinfo sb <number>" (default 20).
      // Hwguy is the only class using the same ammo type for all his weapons,
      therefore and because of high cannon firerate, often he became short on shells.
      In order to somewhat balance this deficit, there was introduced such ammo division:
      first part for cannon and second for shotgun. Although amount and type of ammo remains
      the same.

Pyro:
[fix] Fixed bug with napalm grenade model falling.
      // Now, grenade model hangs at where explosion is take place.
[fix] If there is no available rockets or cells, "special" command switches to available
      primary weapon (either flame thrower or rocket launcher, respectively).
      // Earlier, in such case after switching to shotgun with "impulse 2" it was impossible
      to switch back to primary weapon using "special"
[fix] Engineer facilities no longer burn under water - it is impossible to set fire
      underwater anymore.
      // Burning players stop fire when step in water. It was never possible to set fire on
      player under water, but buildings were burning under water as on air.
[fix] Flame thrower flames fade on water surface.
      // Flame thrower didn't work under water from beginning, however being above water
      surface it was possible to spread flame thrower fire underwater.
[add] Flame thrower fire produces dynamic light also.
      // From now on, the fire not only warms but shines, too :)
[add] It is possible to replace flame thrower model despite of grenade launcher model.
      // Flame thrower model now has name "v_flame.mdl" and comes alongside.
      In fact, it's just renamed stanard "v_rock.mdl". However, now it could be replaced
      by any custom model not affecting demoman's grenade launcher.
[new] Whenever server runs altprogs flame thrower will start fire on surfaces.
      // Just as LAN TF.
[new] Whenever server runs altprogs napalm splits into flames.
      // Just as LAN TF.
[new] Whenever server runs altprogs flames limit increased up to 60.
      // Just as LAN TF.

Spy:
[fix] "discard" command discards cells.
      // "discard" supposed to drop unused ammo.
      In early TF versions, cells were used during spy class-specific tasks, then it was
      abandoned. However, cells were not added to discarded ammo.
[fix] Feigned spy couldn't be "id"entified.
      // So far, using "id" on spy feigned death, the one could exactly recognize him.
      Taking into account that most advanced players used "autoid", there was no any
      chance for spy to survive.
[fix] Sentry does not detect feigned spies.
      // So far, whenever undisguised spy feigned death near the sentry, it shoot above
      him as long as there was enough ammo.
[fix] Spy drops all carried goal_item whenever feigns death.
      // Before, it was looked as glowing corpse, sometimes even invisible.
[fix] "special" menu works correctly on maps not allowing spy with flag to change color.
      // Before, it didn't displayed at all, now it's displayed in brief form, that is
      without color/skin change options.
[add] Updated class help.
      // It's up-to-date now.
[add] There is "feigned" message displayed in status bar whenever spy feigns death.
      // It's visually clear (especially for those who play without weapon models).
[add] It's possible to replace tranq gun model not affecting shotgun model.
      // Tranquilizer model has "v_tgun.mdl" name and comes alongside. Actually, it's just
      a renamed "v_shot.mdl". However, from now on it could be replaced with any custom
      model.
[new] From now on, spy has yellow-class armor.
      // Before, 100 units of green armor was impartially not enough for spy. He ever
      could spend all armor only if medic would cure him constantly. Otherwise he couldn't
      use more than 50 units of armor before his death. Besides, enemy engineers could easily
      spot disguised spy since armor amount is displayed to them.
[new] Maximum armor amount reduced to 70 units.
      // While 100 green armor is like a joke on spy, 100 units of yellow is a joke on his
      enemies :). For now, it is limited to 70 units as an intermediate between 40 and 100
      (given 90 HP, 100 green armor units are equal to 40 yellow units). It's fairly
      enough for grenade jump (there would remain ~10 armor and ~50 HP left).
[new] Spy could feign death in air
      // Why not? When feigned, speed is reset to zero so it's impossible to fly through enemy
      defences disguised as a corpse.
[new] Spy could feign death silently using "sfeign" command.
      // Somewhat world de facto standard :)
[new] "Silent Feign" command added to "special" menu.
      // As there is a command, there should be an option for it in menu.
[new] New gas effect. Under it, other players in view are pretend to randomly change their
      skin and color (skin is twice as rare as color), besides "id" command does not work
      on them. Distracting explosions and sounds work just as under old gas effect.
      // Palette change in old gas effect was correctly working in GL mode within QW262
      client only. Besides, it didn't work neither on Win2000 nor WinXP on any client.
      Therefore, gas effect was practically unusable and the only affected by it were
      players using either Software client or Win9x. From now on, random color change
      performed not by means of palette.
[opt] Fine tuning for new gas effect on server with "localinfo new_gas <number>":
      1 - change players color (blue/red)
      2 - change players skin.
      4 - change players color (blue/red/yellow/green)
      8 - change players color (all colors)
      16 - palette change (does not work in GL on Win2000 and WinXP).
      32 - additional sounds and explosions (same as with "new_gren" setting).
      64 - players change their skin to spy.
      128 - turn off "id".
      256 - gas effect period same as with new_gren.
      512 - gas damage same as with new_gren.
      default is new_gas 131.
      old_gren setting does not affect gas effect.
      old_gren on = new_gas 16.
      old_gren off = new_gas 800.
      // Just a fine tuning for effects.

Engineer:
[fix] Updated class help.
      // It's up-to-date now.
[fix] Fixed bug with dropcells.
      // When dropping cells, engineer in fact produces cells from cells. So far, because
      of rounding error he could make 10 cells out from 9.
[fix] EMP grenade does not blow up friendly facilities if server has friendly fire damage
      turned off.
      // No doubt a bug, although not that critical novadays.
[fix] Partially fixed bug with sentry gun misses with shells.
      // Stanard sentry gun from TF 2.8.1 often misses when firing shells. This fix, taken
      from TF 2.8.1.b1, address this issue on the most maps out there.
[add] Presence and health of dispenser are displayed at status bar.
      // Before, dispenser availability could only be controlled through "build" menu.
[add] "id" command displays healh and ammo of any friendly sentry gun. So dispenser health do.
      // There is no need to come close and check it all with spanner hit.
[add] Added console message about dispenser explosion (the cause of explosion does not
      reported though).
      // So far, it was not reported at all.
[add] When server has "watervis" enabled, sentry gun could aim and shoot through water surface.
      // If player could see a gun, then gun should see a player also :)
[new] New sentry gun shooting algorithm. From now on, it could hit anything that it could
      see.
      // Standard Moscow TF league fix didn't address issue completely. The new one had it done.
[new] It is possible to make sentry to seek for targets independently of players number,
      by fixing virtual number of players on server: "localinfo sgppl <number>" (default 12).
      // Sentry seeks for target differently when there are different number of players on
      server. From now, it's possible to force sentry to aim in 3x3 game the same way as
      it aims in 6x6 game. Or on the other hand sentry could aim without delay in 10x10 game.
[opt] Implemented new build place precheck in order to prevent objects falling through
      floor (required qwsv262 from 28.02.04 or later).
      Could be turned off with "localinfo new_build_check off" or "localinfo nbc off".
      // Sometimes facilities fall through solid floor.
[opt] It is possible to choose different sentry algorithms on server:
      "localinfo sg old" - target seek from 2.8, shooting from 2.8.
      "localinfo sg oldf" - new target seek, shooting from 2.8.
      "localinfo sg mtflf" - new target seek, shooting from MTFL (actual TF 2.8.1b1).
      "localinfo sg fix" - target seek from 2.8, new shooting.
      "localinfo sg oldmtfl" - target seek from 2.8, shooting from MTFL (actual TF 2.8.1b1).
      Any other value - new target seek, new shooting.
      // Just a choice possibility.

Common: 
[fix] Removed bug related to disappearing of last remaining grenade whenever goal_item is taken.
      // Formerly, whenever goal_item (flags, backpacks etc) was taken, the last primed
      grenade of certain type was silently missing. In some cases (flag touchdown) this
      bug affected not only player that activated goal, but his whole team also.
[fix] Fixed bug allowing to pick more grenades from goal_item than class is allowed to.
      // On some maps, it was possible to pick up more grenades than allowed.
[fix] Fixed bugs related to armor picked up from goal_item.
      // From now on, armor is picked up always, and is maximum possible color for player
      picking it.
[fix] Removed flag replication bug.
      // So far, whenever flag was taken during 0.1 second before it return, the flag was
      taken and returned at the same time. That is, there became two flags of the same color on
      the map.
[fix] Removed "reload" and sbar bug.
      // Whenever status bar was turned on, and all ammo was dropped and taken immediately,
      instead of first shot there was a reload.
[fix] Removed command score sbar displaying bug.
      // Earlier, in some cases, score of one team could be displayed twice within sbar.
[fix] Removed command score bug, not showing score of 100.
      // So far, score 100 (10 flags) didn't displayed on sbar.
[fix] Removed bug with current ammmo displaying.
      // In some cases, ammo amount in menu was reduced by 2 instead of 1.
[fix] Corpses do not interact with buttons, doors and platforms anymore.
      // It was possible to lay dead on some buttons or within some doors and they were
      stay active or open, since they were reacting on corpse as on alive player.
[fix] Grenades do not explode inside player after respawn whenever respawn_delay set to 0.
      // Frequently, grenades primed just before death were exploded inside player after
      his respawn.
[fix] Removed bug with grenade throw delay (OmiTF).
      // Sometimes, there was additional delay whenever grenade was supposed to be thrown.
[fix] Medikits heal leg injuries, even if player health is at maximum.
      // Comes to imortance when leg injuries slow down player.
      For example, after scout caltrops or sniper legshots.
[fix] Fixed most flaws allowed to change color and skin while playing.
      // Before, it was possible to play with foreign skin and color all game.
[fix] Closed some possibilities for spy to illegally change skin and/or color.
      Could be tuned using "cheat pause" server variable: "localinfo cp <time>" (default 1).
      For internet gameplay it is recommended to increase it value.
      // Since spy is supposed to change color and skin frequently using "disguise"
      command, whenever "cheat pause" is 1 or less, server could incorrectly kick innocent
      player with poor connect. In order to avoid such problems, check time could be set
      manually (earlier, it was fixed to 10 seconds).
[add] From now on, "old_grens" server setting is turned "on" by default.
      // Historically, in Russia old_grens is a de facto standard for TF gameplay.
[add] "showtf" command displays all new optional settings.
      // The one could examine server setting not using rcon.
[add] Server executes "mapdefault.cfg" just before "<mapname>.cfg".
      // It is should be this way :)
[new] Builtin autoid, same as performing "id" command once a second, but not using any
      impulses. Turned on client using "setinfo autoid on", begins to work after next respawn.
      // Earlier, autoid could be emulated by binding "id" on movement buttons.
      Therefore, other more important impulses could be lost, despite of fact that client
      was sending unnecessary impulses to server.
      From now on, "id" information could be distributed by server automatically,
      without any impulses.
[new] New bitmask setting "setinfo s <number>"
      1 - exec map on
      2 - exec class on
      4 - class help on
      8 - autoid on
      16 - multiscan off (for scout only)
      32 - "showclasses" does NOT display health and armor info.
      64 - "showclasses" displays teammates coordinates.
      128 - "showclasses" does NOT display spy skin info.
      256 - "showclasses" displays detpack info.
      512 - "showclasses" displays sentry gun info.
      // In order to save space in setinfo buffer.
[new] Customizable discard: 
      "setinfo mxs <to keep>" or "setinfo 1 <to keep>"
      "setinfo mxn <to keep>" or "setinfo 2 <to keep>"
      "setinfo mxr <to keep>" or "setinfo 3 <to keep>"
      "setinfo mxc <to keep>" or "setinfo 4 <to keep>"
      Empty string correspond to standard discard amount for these ammo.
      // It is possible to make custom disacrd in client using scripts by means of
      "dropammo". However, there could be a problem on server related to max entity limit.
      From now on, all unnecessary ammo could be discarded in one backpack.
[new] "showclasses" displays armor, health and coordinates of teammates.
      // Simple tracker, available for anyone. Using qw262 triggers/huds, could become a
      powerful situation tracker. Very useful for team coordination.
[new] Precautions taken to handle most important impulses.
      // Impulses were lost during shooting, since during weapon animation sequence
      impulses could not be used.  From now on, such impulses work after animation is
      complete. During automatic fire, there should be pause taken in shooting in order
      for impulse to work.
[new] It is possible to fix common topcolor for each of commands.
      Turned on server (applied after map change):
      "localinfo tc1 <color>"
      "localinfo tc2 <color>"
      "localinfo tc3 <color>"
      "localinfo tc4 <color>"
      By default, topcolor is equal to bottomcolor (team color). Color range is from 0 to 15.
      If any of the above settings turned on, server-side color check is applied.
      // At Moscow TF League it is forbidden by rules to alter team topcolor. But there always been
      those who break rules law and those who complain about this offending. From now on,
      this problem could be solved on server side :).
      Besides, it is possible to assign single topcolor to all team members, different
      from their team color. It could be useful for distinguish clan members at
      tournaments, since same clan is often forced to use different colors during
      tournament. Besides, this topcolor handling takes into account spies abilities
      since whenever spy changes skin color it will automatically change top color to
      enemy.
[opt] It is possible to display time left until flag return on "flaginfo" command.
      Turned off on server with "localinfo flag_timer off" or "localinfo ft off".
      // Since some players are already using timers by means of modern QW clients, it is
      fairly enough to share time-related information to all.
[opt] It is possible to drop goal_item despite of limits on particular map.
      Drop is performed with standard command "dropitems".
      Turned on server with "localinfo allow_drop_goal on" or "localinfo adg on".
      // The possibility to drop goal_item (in particulary, flags), was in TF for a long
      time. However, on the most maps it is denied by flag entity properties. From now on,
      it could be allowed anywhere without need to alter map bsp file.
[opt] Implemented random team respawn (turned on by default).
      Could be turned off with "localinfo random_team_spawn off" or "localinfo rts off".
      // Respawn points no longer assigned by fixed order (as defined withing map itself),
      random order is used. So it is impossible to predict next respawn place.
[opt] It is possible to deny powerups on all maps.
      Turned on server with "localinfo disable_powerups on" or "localinfo dp on".
      // There is no need to alter each map, although there is no need for this option on
      official league maps.
[opt] It is possible to deny specific grenade types and detpack usage.
      Turned on server with "localinfo disable_grens <number>" or "localinfo dg <number>".
      2 - NORMAL
      4 - CONCUSSION
      8 - NAIL
      16 - MIRV
      32 - NAPALM
      64 - FLARE
      128 - GAS
      256 - EMP
      512 - FLASH
      1024 - CALTROPS
      2048 - DETPACK
      // There is no need to alter each map. Must have for arena or massacre type maps.
[opt] It is possible to pick up items other than ammo from dead players backpacks.
      Turned on server with "localinfo g2b <number>"
      1 - GREN
      2 - GREN_BYTYPE
      4 - ARMOR
      8 - HEALTH
      16 - DETPACK
      // Possibility is fairly logical, but missed in original TF mod.
      When "1", grenades are taken as in respawn, that is the only difference is grenade
      type despite of class picking up backpack.
      When "2", it is possible to pick up grenades only of type that could be used by
      player's who picking up backpack. That is, soldier could not pick up napalm grenade
      from pyro's backpack.
      Armor could be gathered only if there is more armor within backpack than player has.
      Health is picked up from medic's backpacks, only if he had some medikits before death.
      This feature is made optional since it affects gameplay signtifically.
[opt] Automatically resets some options according to MTFL rules.
      Turned on server with "localinfo mtfl on".
      Displayed by "showtf" command as "MTFL Settings ON".
      mtfl = 1
      allow_hook = 0
      old_grens = 1
      invis_only = 0
      cheat_pause = 1
      topcolor_check =1
      allow_drop_goal = 0
      add_pipe = 1
      new_flash = 1
      new_gas = 131
      sentry_type = SENTRY_NEW
      sgppl = 12
      disable_grens = 0
      dtpb = 1
      disable_powerups = 0
      flag_timer = 1
      snip_fps = 72
      snip_range_fix = 0
      snip_ammo = 1
      snip_time = 1.5
      gren2box = 0
      random_tf_spawn = 1
      (for above settings, corresponding localinfo value is ignored)

This version is not final and subject to change.

Ideas and programming - [sd] angel
Ideas, beta-testing and documentation - [sd] hunter
English translation - se7en

http://clan.sd.ru
http://mtfl.sd.ru
