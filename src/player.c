#include "g_local.h"

void player_touch(  )
{
	gedict_t *Bio;
	gedict_t *te;

//      float   v1;
//      float   v2;
	float   found;

	if ( !tf_data.invis_only && ( self->playerclass == PC_SPY || other->playerclass == PC_SPY ) )
	{
		if ( streq( other->s.v.classname, "player" ) )
		{
			if ( self->undercover_team || self->undercover_skin )
			{
				if ( ( other->playerclass == PC_SPY || other->playerclass == PC_SCOUT )
				     && other->team_no != self->team_no )
				{
					TF_AddFrags( other, 1 );
					G_bprint( 1, "%s uncovered an enemy spy!\n", other->s.v.netname );
					Spy_RemoveDisguise( self );
				}
			}
			if ( other->undercover_team || other->undercover_skin )
			{
				if ( ( self->playerclass == PC_SPY || self->playerclass == PC_SCOUT )
				     && self->team_no != other->team_no )
				{
					TF_AddFrags( self, 1 );
					G_bprint( 1, "%s uncovered an enemy spy!\n", other->s.v.netname );
					Spy_RemoveDisguise( other );
				}
			}
		}
	}
	if ( ( self->tfstate & TFSTATE_INFECTED ) && tf_data.cb_prematch_time < g_globalvars.time )
	{
		if ( streq( other->s.v.classname, "player" ) && other->playerclass )
		{
			if ( !( other->tfstate & TFSTATE_INFECTED ) )
			{
				if ( other->playerclass != PC_MEDIC )
				{
					if ( !
					     ( ( teamplay & 16 )
					       && PROG_TO_EDICT( self->s.v.owner )->team_no ==
					       PROG_TO_EDICT( self->s.v.enemy )->team_no
					       && PROG_TO_EDICT( self->s.v.owner )->team_no ) )
					{
						found = 0;
						te = find( world, FOFS( s.v.classname ), "timer" );
						while ( te && !found )
						{
							if ( PROG_TO_EDICT( te->s.v.owner ) == self
							     && te->s.v.think == ( func_t ) BioInfection_Decay )
								found = 1;
							else
								te = find( te, FOFS( s.v.classname ), "timer" );
						}
						Bio = spawn(  );
						Bio->s.v.nextthink = 2;
						Bio->s.v.think = ( func_t ) BioInfection_Decay;
						Bio->s.v.owner = EDICT_TO_PROG( other );
						Bio->s.v.classname = "timer";
						Bio->s.v.enemy = te->s.v.enemy;
						other->tfstate = other->tfstate | TFSTATE_INFECTED;
						other->infection_team_no = self->infection_team_no;
						G_sprint( other, 1, "You have been infected by %s!\n",
							  self->s.v.netname );
						G_sprint( self, 1, "You have infected %s!\n", other->s.v.netname );
					}
				}
			}
		}
	}
}
void player_stand1(  )
{
	self->s.v.frame = 17;
	self->s.v.think = ( func_t ) player_stand1;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 0;
	if ( self->s.v.velocity[0] || self->s.v.velocity[1] )
	{
		self->walkframe = 0;
		player_run();
		return;
	}
	if ( self->current_weapon <= 16 )
	{
		if ( self->walkframe >= 12 )
			self->walkframe = 0;
		self->s.v.frame = 17 + self->walkframe;
	} else
	{
		if ( self->walkframe >= 5 )
			self->walkframe = 0;
		self->s.v.frame = 12 + self->walkframe;
	}
	self->walkframe = self->walkframe + 1;
}

void player_run(  )
{
	self->s.v.frame = 6;
	self->s.v.think = ( func_t ) player_run;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 0;
	if ( !self->s.v.velocity[0] && !self->s.v.velocity[1] )
	{
		self->walkframe = 0;
		player_stand1(  );
		return;
	}
	if ( self->current_weapon <= 16 )
	{
		if ( self->walkframe >= 6 )
			self->walkframe = 0;
		self->s.v.frame = 0 + self->walkframe;
	} else
	{
		if ( self->walkframe >= 6 )
			self->walkframe = 0;
		self->s.v.frame = self->s.v.frame + self->walkframe;
	}
	self->walkframe = self->walkframe + 1;
}

void player_shot1(  )
{
	self->s.v.frame = 113;
	self->s.v.think = ( func_t ) player_shot2;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 1;
	muzzleflash(  );
}

void player_shot2(  )
{
	self->s.v.frame = 114;
	self->s.v.think = ( func_t ) player_shot3;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 2;
}

void player_shot3(  )
{
	self->s.v.frame = 115;
	self->s.v.think = ( func_t ) player_shot4;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 3;
}

void player_shot4(  )
{
	self->s.v.frame = 116;
	self->s.v.think = ( func_t ) player_shot5;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 4;
}

void player_shot5(  )
{
	self->s.v.frame = 117;
	self->s.v.think = ( func_t ) player_shot6;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 5;
}

void player_shot6(  )
{
	self->s.v.frame = 118;
	self->s.v.think = ( func_t ) player_run;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 6;
}

void player_autorifle1(  )
{
	self->s.v.frame = 113;
	self->s.v.think = ( func_t ) player_autorifle2;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 1;
	muzzleflash(  );
}

void player_autorifle2(  )
{
	self->s.v.frame = 114;
	self->s.v.think = ( func_t ) player_autorifle3;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 2;
}

void player_autorifle3(  )
{
	self->s.v.frame = 118;
	self->s.v.think = ( func_t ) player_run;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 6;
}

void player_axe1(  )
{
	self->s.v.frame = 119;
	self->s.v.think = ( func_t ) player_axe2;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 1;
}

void player_axe2(  )
{
	self->s.v.frame = 120;
	self->s.v.think = ( func_t ) player_axe3;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 2;
}

void player_axe3(  )
{
	self->s.v.frame = 121;
	self->s.v.think = ( func_t ) player_axe4;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 3;
	if ( self->current_weapon == WEAP_AXE )
		W_FireAxe(  );
	else
		W_FireSpanner(  );
}

void player_axe4(  )
{
	self->s.v.frame = 122;
	self->s.v.think = ( func_t ) player_run;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 4;
}

void player_axeb1(  )
{
	self->s.v.frame = 125;
	self->s.v.think = ( func_t ) player_axeb2;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 5;
}

void player_axeb2(  )
{
	self->s.v.frame = 126;
	self->s.v.think = ( func_t ) player_axeb3;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 6;
}

void player_axeb3(  )
{
	self->s.v.frame = 127;
	self->s.v.think = ( func_t ) player_axeb4;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 7;
	if ( self->current_weapon == WEAP_AXE )
		W_FireAxe(  );
	else
		W_FireSpanner(  );
}

void player_axeb4(  )
{
	self->s.v.frame = 128;
	self->s.v.think = ( func_t ) player_run;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 8;
}

void player_axec1(  )
{
	self->s.v.frame = 131;
	self->s.v.think = ( func_t ) player_axec2;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 1;
}

void player_axec2(  )
{
	self->s.v.frame = 132;
	self->s.v.think = ( func_t ) player_axec3;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 2;
}

void player_axec3(  )
{
	self->s.v.frame = 133;
	self->s.v.think = ( func_t ) player_axec4;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 3;
	if ( self->current_weapon == WEAP_AXE )
		W_FireAxe(  );
	else
		W_FireSpanner(  );
}

void player_axec4(  )
{
	self->s.v.frame = 134;
	self->s.v.think = ( func_t ) player_run;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 4;
}

void player_axed1(  )
{
	self->s.v.frame = 137;
	self->s.v.think = ( func_t ) player_axed2;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 5;
}

void player_axed2(  )
{
	self->s.v.frame = 138;
	self->s.v.think = ( func_t ) player_axed3;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 6;
}

void player_axed3(  )
{
	self->s.v.frame = 139;
	self->s.v.think = ( func_t ) player_axed4;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 7;
	if ( self->current_weapon == WEAP_AXE )
		W_FireAxe(  );
	else
		W_FireSpanner(  );
}

void player_axed4(  )
{
	self->s.v.frame = 140;
	self->s.v.think = ( func_t ) player_run;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 8;
}

void player_chain1(  )
{
	self->s.v.frame = 137;
	self->s.v.think = ( func_t ) player_chain1a;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 1;
	Throw_Grapple(  );
}

void player_chain1a(  )
{
	self->s.v.frame = 137;
	self->s.v.think = ( func_t ) player_chain2;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 2;
}

void player_chain2(  )
{
	self->s.v.frame = 138;
	self->s.v.think = ( func_t ) player_chain2a;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 3;
}

void player_chain2a(  )
{
	self->s.v.frame = 138;
	self->s.v.think = ( func_t ) player_chain3;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 4;
}

void player_chain3(  )
{
	self->s.v.frame = 139;
	self->s.v.think = ( func_t ) player_chain3;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 6;
	if ( !self->hook_out )
	{
		player_chain5(  );
		return;
	}
	if ( vlen( self->s.v.velocity ) >= 750 )
	{
		player_chain4(  );
		return;
	}
}
void player_chain4(  )
{
	self->s.v.frame = 73;
	self->s.v.think = ( func_t ) player_chain4;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 6;
	if ( !self->hook_out )
	{
		player_chain5(  );
		return;
	}
	if ( vlen( self->s.v.velocity ) < 750 )
	{
		player_chain3(  );
		return;
	}
}
void player_chain5(  )
{
	self->s.v.frame = 140;
	self->s.v.think = ( func_t ) player_run;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 0;
}

void player_medikit1(  )
{
	self->s.v.frame = 119;
	self->s.v.think = ( func_t ) player_medikit2;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 1;
}

void player_medikit2(  )
{
	self->s.v.frame = 120;
	self->s.v.think = ( func_t ) player_medikit3;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 2;
}

void player_medikit3(  )
{
	self->s.v.frame = 121;
	self->s.v.think = ( func_t ) player_medikit4;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 3;
	W_FireMedikit(  );
}

void player_medikit4(  )
{
	self->s.v.frame = 122;
	self->s.v.think = ( func_t ) player_run;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 4;
}

void player_medikitb1(  )
{
	self->s.v.frame = 125;
	self->s.v.think = ( func_t ) player_medikitb2;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 5;
}

void player_medikitb2(  )
{
	self->s.v.frame = 126;
	self->s.v.think = ( func_t ) player_medikitb3;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 6;
}

void player_medikitb3(  )
{
	self->s.v.frame = 127;
	self->s.v.think = ( func_t ) player_medikitb4;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 7;
	W_FireMedikit(  );
}

void player_medikitb4(  )
{
	self->s.v.frame = 128;
	self->s.v.think = ( func_t ) player_run;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 8;
}

void player_medikitc1(  )
{
	self->s.v.frame = 131;
	self->s.v.think = ( func_t ) player_medikitc2;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 1;
}

void player_medikitc2(  )
{
	self->s.v.frame = 132;
	self->s.v.think = ( func_t ) player_medikitc3;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 2;
}

void player_medikitc3(  )
{
	self->s.v.frame = 133;
	self->s.v.think = ( func_t ) player_medikitc4;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 3;
	W_FireMedikit(  );
}

void player_medikitc4(  )
{
	self->s.v.frame = 134;
	self->s.v.think = ( func_t ) player_run;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 4;
}

void player_medikitd1(  )
{
	self->s.v.frame = 137;
	self->s.v.think = ( func_t ) player_medikitd2;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 5;
}

void player_medikitd2(  )
{
	self->s.v.frame = 138;
	self->s.v.think = ( func_t ) player_medikitd3;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 6;
}

void player_medikitd3(  )
{
	self->s.v.frame = 139;
	self->s.v.think = ( func_t ) player_medikitd4;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 7;
	W_FireMedikit(  );
}

void player_medikitd4(  )
{
	self->s.v.frame = 140;
	self->s.v.think = ( func_t ) player_run;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 8;
}

void player_bioweapon1(  )
{
	self->s.v.frame = 119;
	self->s.v.think = ( func_t ) player_bioweapon2;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 1;
}

void player_bioweapon2(  )
{
	self->s.v.frame = 120;
	self->s.v.think = ( func_t ) player_bioweapon3;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 2;
}

void player_bioweapon3(  )
{
	self->s.v.frame = 121;
	self->s.v.think = ( func_t ) player_bioweapon4;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 3;
	W_FireBioweapon(  );
}

void player_bioweapon4(  )
{
	self->s.v.frame = 122;
	self->s.v.think = ( func_t ) player_run;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 4;
}

void player_bioweaponb1(  )
{
	self->s.v.frame = 125;
	self->s.v.think = ( func_t ) player_bioweaponb2;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 5;
}

void player_bioweaponb2(  )
{
	self->s.v.frame = 126;
	self->s.v.think = ( func_t ) player_bioweaponb3;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 6;
}

void player_bioweaponb3(  )
{
	self->s.v.frame = 127;
	self->s.v.think = ( func_t ) player_bioweaponb4;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 7;
	W_FireBioweapon(  );
}

void player_bioweaponb4(  )
{
	self->s.v.frame = 128;
	self->s.v.think = ( func_t ) player_run;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 8;
}

void player_bioweaponc1(  )
{
	self->s.v.frame = 131;
	self->s.v.think = ( func_t ) player_bioweaponc2;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 1;
}

void player_bioweaponc2(  )
{
	self->s.v.frame = 132;
	self->s.v.think = ( func_t ) player_bioweaponc3;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 2;
}

void player_bioweaponc3(  )
{
	self->s.v.frame = 133;
	self->s.v.think = ( func_t ) player_bioweaponc4;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 3;
	W_FireBioweapon(  );
}

void player_bioweaponc4(  )
{
	self->s.v.frame = 134;
	self->s.v.think = ( func_t ) player_run;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 4;
}

void player_bioweapond1(  )
{
	self->s.v.frame = 137;
	self->s.v.think = ( func_t ) player_bioweapond2;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 5;
}

void player_bioweapond2(  )
{
	self->s.v.frame = 138;
	self->s.v.think = ( func_t ) player_bioweapond3;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 6;
}

void player_bioweapond3(  )
{
	self->s.v.frame = 139;
	self->s.v.think = ( func_t ) player_bioweapond4;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 7;
	W_FireBioweapon(  );
}

void player_bioweapond4(  )
{
	self->s.v.frame = 140;
	self->s.v.think = ( func_t ) player_run;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 8;
}

void player_nail1(  )
{
	self->s.v.frame = 103;
	self->s.v.think = ( func_t ) player_nail2;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	muzzleflash(  );
	if ( !self->s.v.button0 || intermission_running )
	{
		player_run( );
		return;
	}
	self->s.v.weaponframe = self->s.v.weaponframe + 1;
	if ( self->s.v.weaponframe == 9 )
		self->s.v.weaponframe = 1;
	SuperDamageSound(  );
	if ( !self->nailpos )
	{
		W_FireSpikes( 4 );
		self->nailpos = 1;
	} else
	{
		W_FireSpikes( -4 );
		self->nailpos = 0;
	}
	Attack_Finished( 0.2 );
}

void player_nail2(  )
{
	self->s.v.frame = 104;
	self->s.v.think = ( func_t ) player_nail1;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	if ( !self->s.v.button0 || intermission_running )
	{
		player_run(  );
		return;
	}
	self->s.v.weaponframe = self->s.v.weaponframe + 1;
	if ( self->s.v.weaponframe == 9 )
		self->s.v.weaponframe = 1;
	Attack_Finished( 0.2 );
}

void player_assaultcannonup1(  )
{
	self->s.v.frame = 103;
	self->s.v.think = ( func_t ) player_assaultcannonup2;
	self->s.v.nextthink = g_globalvars.time + 0.1;


	if ( !self->s.v.button0 || self->s.v.ammo_shells <= self->assault_min_shells || intermission_running )
	{
		self->tfstate = self->tfstate - ( self->tfstate & TFSTATE_AIMING );
		TeamFortress_SetSpeed( self );
		self->count = 1;
		self->heat = 0;
		player_assaultcannondown1(  );
		return;
	}
	self->fire_held_down = 1;
	if ( self->heat == 1 )
		sound( self, 1, "weapons/asscan1.wav", 1, 1 );
	SuperDamageSound(  );
	Attack_Finished( 0.1 );
	if ( self->heat != 2 && self->heat != 4 )
	{
		if ( self->s.v.weaponframe >= 3 )
			self->s.v.weaponframe = 0;
		else
			self->s.v.weaponframe = self->s.v.weaponframe + 1;
	}
	self->heat = self->heat + 1;
	if ( self->heat >= 7 )
	{
		self->heat = 0;
		self->ScanRange = 0;
		player_assaultcannon1(  );
	}
}
void player_assaultcannonup2(  )
{
	self->s.v.frame = 103;
	self->s.v.think = ( func_t ) player_assaultcannonup1;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	if ( !self->s.v.button0 || self->s.v.ammo_shells <= self->assault_min_shells || intermission_running )
	{
		self->tfstate = self->tfstate - ( self->tfstate & TFSTATE_AIMING );
		TeamFortress_SetSpeed( self );
		self->count = 1;
		self->heat = 0;
		player_assaultcannondown1(  );
		return;
	}
	SuperDamageSound(  );
	Attack_Finished( 0.1 );
	if ( self->heat != 2 && self->heat != 4 && self->heat != 7 )
	{
		if ( self->s.v.weaponframe == 2 && self->heat >= 9 )
			self->s.v.weaponframe = 0;
		else
		{
			if ( self->s.v.weaponframe >= 3 )
				self->s.v.weaponframe = 0;
			else
				self->s.v.weaponframe = self->s.v.weaponframe + 1;
		}
	}
	self->heat = self->heat + 1;
	if ( self->heat >= 13 )
	{
		self->heat = 0;
		self->ScanRange = 0;
		player_assaultcannon1(  );
	}
}
void player_assaultcannon1(  )
{
	vec3_t  v;
#ifndef NEWHWGUY
	int     st;
#endif

	self->s.v.frame = 103;
	self->s.v.think = ( func_t ) player_assaultcannon2;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	VectorCopy( self->s.v.velocity, v );
	v[2] = 0;
// if (vlen(self->s.v.velocity) < 50 && (self.flags & #FL_ONGROUND)) {
	if ( vlen( v ) < 20 && ( ( int ) self->s.v.flags & FL_ONGROUND ) )
	{
		muzzleflash(  );
		sound( self, 1, "weapons/asscan2.wav", 1, 1 );
		if ( self->s.v.weaponframe == 2 )
			self->s.v.weaponframe = 4;
		else
			self->s.v.weaponframe = 2;
		SuperDamageSound(  );
		W_FireAssaultCannon(  );
#ifndef NEWHWGUY
		stuffcmd( self, "v_idlescale " );
		if ( self->heat < 5 )
			st = ( self->heat * 4 );
		else
			st = 20;
		stuffcmd( self, "%d\n", st );
#endif
	} else
	{
		sound( self, 1, "weapons/asscan4.wav", 1, 1 );
		if ( self->s.v.weaponframe == 2 )
			self->s.v.weaponframe = 0;
		else
			self->s.v.weaponframe = 2;
#ifndef NEWHWGUY
		stuffcmd( self, "v_idlescale 5\n" );
#endif
	}
	if ( !self->s.v.button0 || self->s.v.ammo_shells <= self->assault_min_shells || intermission_running )
	{
#ifndef  NEWHWGUY
		stuffcmd( self, "v_idlescale 0\n" );
#endif
		self->tfstate = self->tfstate - ( self->tfstate & TFSTATE_AIMING );
		TeamFortress_SetSpeed( self );
		self->s.v.weaponframe = 0;
		self->count = 1;
		player_assaultcannondown1(  );
		return;
	}
	Attack_Finished( 0.1 );
}

void player_assaultcannon2(  )
{
	vec3_t  v;

	self->s.v.frame = 104;
	self->s.v.think = ( func_t ) player_assaultcannon1;
	self->s.v.nextthink = g_globalvars.time + 0.1;
	VectorCopy( self->s.v.velocity, v );
	v[2] = 0;
#ifndef NEWHWGUY
	stuffcmd( self, "v_idlescale 0\n" );
#endif
// if (vlen(self->s.v.velocity) < 50 && (self.flags & #FL_ONGROUND)) {
	if ( vlen( v ) < 20 && ( ( int ) self->s.v.flags & FL_ONGROUND ) )
	{
		if ( self->s.v.weaponframe == 2 )
			self->s.v.weaponframe = 4;
		else
			self->s.v.weaponframe = 2;
		SuperDamageSound(  );
		W_FireAssaultCannon(  );
		self->heat = self->heat + 0.1;
		stuffcmd( self, "bf\n" );
	} else
	{
		if ( self->s.v.weaponframe == 2 )
			self->s.v.weaponframe = 0;
		else
			self->s.v.weaponframe = 2;
	}
	if ( !self->s.v.button0 || self->s.v.ammo_shells <= self->assault_min_shells || intermission_running )
	{
#ifndef NEWHWGUY
		stuffcmd( self, "v_idlescale 0\n" );
#endif
		self->tfstate = self->tfstate - ( self->tfstate & TFSTATE_AIMING );
		TeamFortress_SetSpeed( self );
		self->s.v.weaponframe = 0;
		self->count = 1;
		player_assaultcannondown1(  );
		return;
	}
	Attack_Finished( 0.1 );
}

void player_assaultcannondown1(  )
{
//      gedict_t *tcool;

	self->s.v.frame = 103;
	self->s.v.think = ( func_t ) player_assaultcannondown1;
	self->s.v.nextthink = g_globalvars.time + 0.1;


	if ( self->count == 1 )
		sound( self, 1, "weapons/asscan3.wav", 1, 1 );
	if ( self->count >= 15 )
	{
		self->heat = 0;
		self->fire_held_down = 0;
		self->tfstate = self->tfstate - ( self->tfstate & TFSTATE_AIMING );
		TeamFortress_SetSpeed( self );
		if ( self->s.v.ammo_shells <= self->assault_min_shells || self->s.v.ammo_cells < 4 )
		{
			self->last_weapon = self->current_weapon;
			self->last_weaponmode = self->weaponmode;
			self->current_weapon = W_BestWeapon(  );
			W_SetCurrentAmmo(  );
			W_PrintWeaponMessage(  );
			return;
		}
		player_run(  );
		return;
	}
	if ( self->count != 8 && self->count != 10 && self->count != 12 && self->count != 14 )
	{
		if ( self->s.v.weaponframe == 3 )
			self->s.v.weaponframe = 0;
		else
			self->s.v.weaponframe = self->s.v.weaponframe + 1;
	}
	self->count = self->count + 1;
	Attack_Finished( 0.1 );
}

void player_light1(  )
{
	self->s.v.frame = 105;
	self->s.v.think = ( func_t ) player_light2;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	muzzleflash(  );
	if ( !self->s.v.button0 || intermission_running )
	{
		player_run(  );
		return;
	}
	self->s.v.weaponframe = self->s.v.weaponframe + 1;
	if ( self->s.v.weaponframe == 5 )
		self->s.v.weaponframe = 1;
	SuperDamageSound(  );
	W_FireLightning(  );
	Attack_Finished( 0.2 );
}

void player_light2(  )
{
	self->s.v.frame = 106;
	self->s.v.think = ( func_t ) player_light1;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	if ( !self->s.v.button0 || intermission_running )
	{
		player_run(  );
		return;
	}
	self->s.v.weaponframe = self->s.v.weaponframe + 1;
	if ( self->s.v.weaponframe == 5 )
		self->s.v.weaponframe = 1;
	SuperDamageSound(  );
	W_FireLightning(  );
	Attack_Finished( 0.2 );
}

void player_rocket1(  )
{
	self->s.v.frame = 107;
	self->s.v.think = ( func_t ) player_rocket2;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 1;
	muzzleflash(  );
}

void player_rocket2(  )
{
	self->s.v.frame = 108;
	self->s.v.think = ( func_t ) player_rocket3;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 2;
}

void player_rocket3(  )
{
	self->s.v.frame = 109;
	self->s.v.think = ( func_t ) player_rocket4;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 3;
}

void player_rocket4(  )
{
	self->s.v.frame = 110;
	self->s.v.think = ( func_t ) player_rocket5;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 4;
}

void player_rocket5(  )
{
	self->s.v.frame = 111;
	self->s.v.think = ( func_t ) player_rocket6;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 5;
}

void player_rocket6(  )
{
	self->s.v.frame = 112;
	self->s.v.think = ( func_t ) player_run;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	self->s.v.weaponframe = 6;
}


void    DeathBubbles( int num_bubbles );

void PainSound(  )
{
	float   rs;

	if ( self->s.v.health < 0 )
		return;
	if ( streq( damage_attacker->s.v.classname, "teledeath" ) )
	{
		sound( self, 2, "player/teledth1.wav", 1, 0 );
		return;
	}
	if ( self->s.v.watertype == -3 && self->s.v.waterlevel == 3 )
	{
		DeathBubbles( 1 );
		if ( g_random(  ) > 0.5 )
			sound( self, 2, "player/drown1.wav", 1, 1 );
		else
			sound( self, 2, "player/drown2.wav", 1, 1 );
		return;
	}
	if ( self->s.v.watertype == -4 )
	{
		if ( g_random(  ) > 0.5 )
			sound( self, 2, "player/lburn1.wav", 1, 1 );
		else
			sound( self, 2, "player/lburn2.wav", 1, 1 );
		return;
	}
	if ( self->s.v.watertype == -5 )
	{
		if ( g_random(  ) > 0.5 )
			sound( self, 2, "player/lburn1.wav", 1, 1 );
		else
			sound( self, 2, "player/lburn2.wav", 1, 1 );
		return;
	}
	if ( self->pain_finished > g_globalvars.time )
	{
		self->axhitme = 0;
		return;
	}
	self->pain_finished = g_globalvars.time + 0.5;
	if ( self->axhitme == 1 )
	{
		self->axhitme = 0;
		sound( self, 2, "player/axhit1.wav", 1, 1 );
		return;
	}
	rs = ( int ) ( g_random(  ) * 5 + 1 );
	self->s.v.noise = "";
	if ( rs == 1 )
		self->s.v.noise = "player/pain1.wav";
	else
	{
		if ( rs == 2 )
			self->s.v.noise = "player/pain2.wav";
		else
		{
			if ( rs == 3 )
				self->s.v.noise = "player/pain3.wav";
			else
			{
				if ( rs == 4 )
					self->s.v.noise = "player/pain4.wav";
				else
				{
					if ( rs == 5 )
						self->s.v.noise = "player/pain5.wav";
					else
						self->s.v.noise = "player/pain6.wav";
				}
			}
		}
	}
	sound( self, 2, self->s.v.noise, 1, 1 );
	return;
}

void player_pain1(  )
{
	self->s.v.frame = 35;
	self->s.v.think = ( func_t ) player_pain2;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	PainSound(  );
	self->s.v.weaponframe = 0;
}

void player_pain2(  )
{
	self->s.v.frame = 36;
	self->s.v.think = ( func_t ) player_pain3;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_pain3(  )
{
	self->s.v.frame = 37;
	self->s.v.think = ( func_t ) player_pain4;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_pain4(  )
{
	self->s.v.frame = 38;
	self->s.v.think = ( func_t ) player_pain5;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_pain5(  )
{
	self->s.v.frame = 39;
	self->s.v.think = ( func_t ) player_pain6;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_pain6(  )
{
	self->s.v.frame = 40;
	self->s.v.think = ( func_t ) player_run;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_axpain1(  )
{
	self->s.v.frame = 29;
	self->s.v.think = ( func_t ) player_axpain2;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	PainSound(  );
	self->s.v.weaponframe = 0;
}

void player_axpain2(  )
{
	self->s.v.frame = 30;
	self->s.v.think = ( func_t ) player_axpain3;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_axpain3(  )
{
	self->s.v.frame = 31;
	self->s.v.think = ( func_t ) player_axpain4;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_axpain4(  )
{
	self->s.v.frame = 32;
	self->s.v.think = ( func_t ) player_axpain5;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_axpain5(  )
{
	self->s.v.frame = 33;
	self->s.v.think = ( func_t ) player_axpain6;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_axpain6(  )
{
	self->s.v.frame = 34;
	self->s.v.think = ( func_t ) player_run;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_pain( gedict_t * attacker, float take  )
{
	if ( self->s.v.weaponframe )
	{
		if ( tf_data.deathmsg == 37 )
			PainSound(  );
		return;
	}
	if ( self->invisible_finished > g_globalvars.time )
		return;
	if ( self->is_feigning )
	{
		PainSound(  );
		return;
	}
	if ( self->s.v.button0 && self->current_weapon == WEAP_ASSAULT_CANNON )
		return;
	if ( self->current_weapon <= 16 )
		player_axpain1(  );
	else
		player_pain1(  );
}


void    player_diea1(  );
void    player_dieb1(  );
void    player_diec1(  );
void    player_died1(  );
void    player_diee1(  );
void    player_die_ax1(  );

void DeathBubblesSpawn(  )
{
	if ( PROG_TO_EDICT( self->s.v.owner )->s.v.waterlevel != 3 )
		return;
	newmis = spawn(  );
	setmodel( newmis, "progs/s_bubble.spr" );
	setorigin( newmis, PROG_TO_EDICT( self->s.v.owner )->s.v.origin[0],
		   PROG_TO_EDICT( self->s.v.owner )->s.v.origin[1],
		   PROG_TO_EDICT( self->s.v.owner )->s.v.origin[2] + 24 );

	newmis->s.v.movetype = MOVETYPE_NOCLIP;
	newmis->s.v.solid = SOLID_NOT;
	SetVector( newmis->s.v.velocity, 0, 0, 15 );
	newmis->s.v.nextthink = g_globalvars.time + 0.5;
	newmis->s.v.think = ( func_t ) bubble_bob;
	newmis->s.v.classname = "bubble";
	newmis->s.v.frame = 0;
	newmis->cnt = 0;
	setsize( newmis, -8, -8, -8, 8, 8, 8 );
	self->s.v.nextthink = g_globalvars.time + 0.1;
	self->s.v.think = ( func_t ) DeathBubblesSpawn;
	self->air_finished = self->air_finished + 1;
	if ( self->air_finished >= self->bubble_count )
		dremove( self );
}

void DeathBubbles( int num_bubbles )
{
	gedict_t *bubble_spawner;

	bubble_spawner = spawn(  );
	setorigin( bubble_spawner, PASSVEC3( self->s.v.origin ) );
	bubble_spawner->s.v.movetype = MOVETYPE_NONE;
	bubble_spawner->s.v.solid = SOLID_NOT;
	bubble_spawner->s.v.nextthink = g_globalvars.time + 0.1;
	bubble_spawner->s.v.think = ( func_t ) DeathBubblesSpawn;
	bubble_spawner->air_finished = 0;
	bubble_spawner->s.v.owner = EDICT_TO_PROG( self );
	bubble_spawner->bubble_count = num_bubbles;
	return;
}

void DeathSound(  )
{
	float   rs;

	if ( self->s.v.waterlevel == 3 )
	{
		if ( self->is_feigning )
			DeathBubbles( 2 );
		else
			DeathBubbles( 10 );
		sound( self, 2, "player/h2odeath.wav", 1, 0 );
		return;
	}
	rs = ( int ) ( g_random(  ) * 5 + 1 );
	if ( rs == 1 )
		self->s.v.noise = "player/death1.wav";
	if ( rs == 2 )
		self->s.v.noise = "player/death2.wav";
	if ( rs == 3 )
		self->s.v.noise = "player/death3.wav";
	if ( rs == 4 )
		self->s.v.noise = "player/death4.wav";
	if ( rs == 5 )
		self->s.v.noise = "player/death5.wav";
	sound( self, 2, self->s.v.noise, 1, 0 );
	return;
}

void PlayerDead(  )
{
	self->s.v.nextthink = -1;
	self->s.v.deadflag = DEAD_DEAD;
}

void VelocityForDamage( float dm, vec3_t v )
{

	v[0] = 100 * crandom(  );
	v[1] = 100 * crandom(  );
	v[2] = 200 + 100 * g_random(  );
	if ( dm > -50 )
		VectorScale( v, 0.7, v );
	else
	{
		if ( dm > -200 )
			VectorScale( v, 2, v );
		else
			VectorScale( v, 10, v );
	}
	return;
}

void ThrowGib( char *gibname, float dm )
{
	newmis = spawn(  );
	VectorCopy( self->s.v.origin, newmis->s.v.origin );
	if ( tf_data.birthday == 1 && g_random(  ) < 0.3 )
	{
		setmodel( newmis, "progs/grenade3.mdl" );
		newmis->s.v.skin = g_random(  ) * 4;
		newmis->s.v.skin = floor( newmis->s.v.skin );
	} else
		setmodel( newmis, gibname );
	setsize( newmis, 0, 0, 0, 0, 0, 0 );
	VelocityForDamage( dm, newmis->s.v.velocity );
	newmis->s.v.movetype = MOVETYPE_BOUNCE;
	newmis->s.v.solid = SOLID_NOT;
	newmis->s.v.avelocity[0] = g_random(  ) * 600;
	newmis->s.v.avelocity[1] = g_random(  ) * 600;
	newmis->s.v.avelocity[2] = g_random(  ) * 600;
	newmis->s.v.think = ( func_t ) SUB_Remove;
	newmis->s.v.ltime = g_globalvars.time;
	newmis->s.v.nextthink = g_globalvars.time + 10 + g_random(  ) * 10;
	newmis->s.v.frame = 0;
	newmis->s.v.flags = 0;
}

void ThrowHead( char *gibname, float dm )
{
	setmodel( self, gibname );
	self->s.v.skin = 0;
	self->s.v.frame = 0;
	self->s.v.nextthink = -1;
	self->s.v.movetype = MOVETYPE_BOUNCE;
	self->s.v.takedamage = 0;
	self->s.v.solid = SOLID_NOT;
	SetVector( self->s.v.view_ofs, 0, 0, 8 );
	setsize( self, -16, -16, 0, 16, 16, 56 );
	VelocityForDamage( dm, self->s.v.velocity );
	self->s.v.origin[2] = self->s.v.origin[2] - 24;
	self->s.v.flags = ( int ) self->s.v.flags - ( ( int ) self->s.v.flags & 512 );
	SetVector( self->s.v.avelocity, 0, crandom(  ) * 600, 0 );
}

void HeadShotThrowHead( char *gibname )
{
	setmodel( self, gibname );
	self->s.v.frame = 0;
	self->s.v.nextthink = -1;
	self->s.v.movetype = MOVETYPE_BOUNCE;
	self->s.v.takedamage = 0;
	self->s.v.solid = SOLID_NOT;
	SetVector( self->s.v.view_ofs, 0, 0, 8 );
	setsize( self, -16, -16, 0, 16, 16, 56 );
	normalize( self->head_shot_vector, self->s.v.velocity );
	VectorScale( self->s.v.velocity, 600, self->s.v.velocity );
	//self->s.v.velocity = normalize(self->head_shot_vector) * 600;
	self->s.v.origin[2] = self->s.v.origin[2] + 24;
	self->s.v.flags = ( int ) self->s.v.flags - ( ( int ) self->s.v.flags & 512 );
	SetVector( self->s.v.avelocity, 0, 0, 0 );
}

void KillPlayer(  )
{
	PROG_TO_EDICT( self->s.v.owner )->s.v.deadflag = 2;
	dremove( self );
}

void GibPlayer(  )
{
	ThrowHead( "progs/h_player.mdl", self->s.v.health );
	ThrowGib( "progs/gib1.mdl", self->s.v.health );
	ThrowGib( "progs/gib2.mdl", self->s.v.health );
	ThrowGib( "progs/gib3.mdl", self->s.v.health );
	if ( tf_data.deathmsg == 36 )
	{
		newmis = spawn(  );
		newmis->s.v.owner = EDICT_TO_PROG( self );
		newmis->s.v.think = ( func_t ) KillPlayer;
		newmis->s.v.nextthink = g_globalvars.time + 1;
	} else
		self->s.v.deadflag = 2;
	TeamFortress_SetupRespawn( 0 );
	if ( streq( damage_attacker->s.v.classname, "teledeath" ) )
	{
		sound( self, 2, "player/teledth1.wav", 1, 0 );
		self->respawn_time = self->respawn_time + 2 + g_random(  ) * 2;
		return;
	}
	if ( streq( damage_attacker->s.v.classname, "teledeath2" ) )
	{
		sound( self, 2, "player/teledth1.wav", 1, 0 );
		self->respawn_time = self->respawn_time + 2 + g_random(  ) * 2;
		return;
	}
	if ( g_random(  ) < 0.5 )
		sound( self, 2, "player/gib.wav", 1, 0 );
	else
		sound( self, 2, "player/udeath.wav", 1, 0 );
}

void PlayerDie(  )
{
	float   i;
	gedict_t *te;
	gedict_t *saveself;

	if ( self->hook_out )
	{
		Reset_Grapple( self->hook );
		Attack_Finished( 0.75 );
		self->hook_out = 1;
	}
	self->tf_impulse = 0;
	self->imp1 = 0;
	self->imp2 = 0;
	self->imp3 = 0;
	self->imp4 = 0;

	for(te = world; (te = find( te, FOFS( s.v.classname ), "primer" ));)
	{
		if(te->s.v.owner == EDICT_TO_PROG( self ))
			break;
	
	}
	if ( te )
//  te->s.v.nextthink = g_globalvars.time;
	{
		self->s.v.deadflag = DEAD_DYING;
		saveself = self;
		self = te;
		( ( void ( * )(  ) ) ( self->s.v.think ) ) (  );
		self = saveself;
		self->s.v.deadflag = DEAD_NO;
	}
	self->s.v.items = ( int ) self->s.v.items - ( ( int ) self->s.v.items & 524288 );
	self->invisible_finished = 0;
	self->invincible_finished = 0;
	self->super_damage_finished = 0;
	self->radsuit_finished = 0;
	self->s.v.modelindex = modelindex_player;
	if ( ( self->tfstate & TFSTATE_INFECTED ) && self == PROG_TO_EDICT( self->s.v.enemy ) )
	{
		te = find( world, FOFS( s.v.classname ), "timer" );
		while ( te )
		{
			if ( PROG_TO_EDICT( te->s.v.owner ) == self && te->s.v.think == ( func_t ) BioInfection_Decay )
			{
				logfrag( PROG_TO_EDICT( te->s.v.enemy ), self );
				TF_AddFrags( PROG_TO_EDICT( te->s.v.enemy ), 1 );
			}
			te = find( te, FOFS( s.v.classname ), "timer" );
		}
	}
	TeamFortress_RemoveTimers(  );
	if ( deathmatch || coop )
		DropBackpack(  );
	self->s.v.weaponmodel = "";
	SetVector( self->s.v.view_ofs, 0, 0, -8 );
	self->s.v.deadflag = 1;
	self->s.v.solid = SOLID_NOT;
	self->s.v.flags = ( int ) self->s.v.flags - ( ( int ) self->s.v.flags & 512 );
	self->s.v.movetype = MOVETYPE_TOSS;
	if ( self->s.v.velocity[2] < 10 )
		self->s.v.velocity[2] = self->s.v.velocity[2] + g_random(  ) * 300;
	if ( self->s.v.health < -40 )
	{
		GibPlayer(  );
		return;
	}
	DeathSound(  );
	self->s.v.angles[0] = 0;
	self->s.v.angles[2] = 0;
	if ( self->current_weapon <= 16 )
	{
		player_die_ax1(  );
		TeamFortress_SetupRespawn( 0 );
		return;
	}
	i = 1 + (int)( g_random(  ) * 6 );
	if ( i == 1 )
		player_diea1(  );
	else
	{
		if ( i == 2 )
			player_dieb1(  );
		else
		{
			if ( i == 3 )
				player_diec1(  );
			else
			{
				if ( i == 4 )
					player_died1(  );
				else
					player_diee1(  );
			}
		}
	}
	TeamFortress_SetupRespawn( 0 );
}

void set_suicide_frame(  )
{
	if ( strneq( self->s.v.model, "progs/player.mdl" ) )
		return;
	setmodel( self, "" );
	setsize( self, -16, -16, -24, 16, 16, 32 );
}

void player_diea1(  )
{
	self->s.v.frame = 50;
	self->s.v.think = ( func_t ) player_diea2;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_diea2(  )
{
	self->s.v.frame = 51;
	self->s.v.think = ( func_t ) player_diea3;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_diea3(  )
{
	self->s.v.frame = 52;
	self->s.v.think = ( func_t ) player_diea4;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_diea4(  )
{
	self->s.v.frame = 53;
	self->s.v.think = ( func_t ) player_diea5;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_diea5(  )
{
	self->s.v.frame = 54;
	self->s.v.think = ( func_t ) player_diea6;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_diea6(  )
{
	self->s.v.frame = 55;
	self->s.v.think = ( func_t ) player_diea7;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_diea7(  )
{
	self->s.v.frame = 56;
	self->s.v.think = ( func_t ) player_diea8;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_diea8(  )
{
	self->s.v.frame = 57;
	self->s.v.think = ( func_t ) player_diea9;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_diea9(  )
{
	self->s.v.frame = 58;
	self->s.v.think = ( func_t ) player_diea10;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_diea10(  )
{
	self->s.v.frame = 59;
	self->s.v.think = ( func_t ) player_diea11;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_diea11(  )
{
	self->s.v.frame = 60;
	self->s.v.think = ( func_t ) player_diea11;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	PlayerDead(  );
}

void player_dieb1(  )
{
	self->s.v.frame = 61;
	self->s.v.think = ( func_t ) player_dieb2;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_dieb2(  )
{
	self->s.v.frame = 62;
	self->s.v.think = ( func_t ) player_dieb3;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_dieb3(  )
{
	self->s.v.frame = 63;
	self->s.v.think = ( func_t ) player_dieb4;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_dieb4(  )
{
	self->s.v.frame = 64;
	self->s.v.think = ( func_t ) player_dieb5;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_dieb5(  )
{
	self->s.v.frame = 65;
	self->s.v.think = ( func_t ) player_dieb6;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_dieb6(  )
{
	self->s.v.frame = 66;
	self->s.v.think = ( func_t ) player_dieb7;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_dieb7(  )
{
	self->s.v.frame = 67;
	self->s.v.think = ( func_t ) player_dieb8;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_dieb8(  )
{
	self->s.v.frame = 68;
	self->s.v.think = ( func_t ) player_dieb9;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_dieb9(  )
{
	self->s.v.frame = 69;
	self->s.v.think = ( func_t ) player_dieb9;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	PlayerDead(  );
}

void player_diec1(  )
{
	self->s.v.frame = 70;
	self->s.v.think = ( func_t ) player_diec2;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_diec2(  )
{
	self->s.v.frame = 71;
	self->s.v.think = ( func_t ) player_diec3;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_diec3(  )
{
	self->s.v.frame = 72;
	self->s.v.think = ( func_t ) player_diec4;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_diec4(  )
{
	self->s.v.frame = 73;
	self->s.v.think = ( func_t ) player_diec5;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_diec5(  )
{
	self->s.v.frame = 74;
	self->s.v.think = ( func_t ) player_diec6;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_diec6(  )
{
	self->s.v.frame = 75;
	self->s.v.think = ( func_t ) player_diec7;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_diec7(  )
{
	self->s.v.frame = 76;
	self->s.v.think = ( func_t ) player_diec8;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_diec8(  )
{
	self->s.v.frame = 77;
	self->s.v.think = ( func_t ) player_diec9;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_diec9(  )
{
	self->s.v.frame = 78;
	self->s.v.think = ( func_t ) player_diec10;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_diec10(  )
{
	self->s.v.frame = 79;
	self->s.v.think = ( func_t ) player_diec11;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_diec11(  )
{
	self->s.v.frame = 80;
	self->s.v.think = ( func_t ) player_diec12;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_diec12(  )
{
	self->s.v.frame = 81;
	self->s.v.think = ( func_t ) player_diec13;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_diec13(  )
{
	self->s.v.frame = 82;
	self->s.v.think = ( func_t ) player_diec14;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_diec14(  )
{
	self->s.v.frame = 83;
	self->s.v.think = ( func_t ) player_diec15;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_diec15(  )
{
	self->s.v.frame = 84;
	self->s.v.think = ( func_t ) player_diec15;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	PlayerDead(  );
}

void player_died1(  )
{
	self->s.v.frame = 85;
	self->s.v.think = ( func_t ) player_died2;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_died2(  )
{
	self->s.v.frame = 86;
	self->s.v.think = ( func_t ) player_died3;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_died3(  )
{
	self->s.v.frame = 87;
	self->s.v.think = ( func_t ) player_died4;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_died4(  )
{
	self->s.v.frame = 88;
	self->s.v.think = ( func_t ) player_died5;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_died5(  )
{
	self->s.v.frame = 89;
	self->s.v.think = ( func_t ) player_died6;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_died6(  )
{
	self->s.v.frame = 90;
	self->s.v.think = ( func_t ) player_died7;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_died7(  )
{
	self->s.v.frame = 91;
	self->s.v.think = ( func_t ) player_died8;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_died8(  )
{
	self->s.v.frame = 92;
	self->s.v.think = ( func_t ) player_died9;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_died9(  )
{
	self->s.v.frame = 93;
	self->s.v.think = ( func_t ) player_died9;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	PlayerDead(  );
}

void player_diee1(  )
{
	self->s.v.frame = 94;
	self->s.v.think = ( func_t ) player_diee2;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_diee2(  )
{
	self->s.v.frame = 95;
	self->s.v.think = ( func_t ) player_diee3;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_diee3(  )
{
	self->s.v.frame = 96;
	self->s.v.think = ( func_t ) player_diee4;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_diee4(  )
{
	self->s.v.frame = 97;
	self->s.v.think = ( func_t ) player_diee5;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_diee5(  )
{
	self->s.v.frame = 98;
	self->s.v.think = ( func_t ) player_diee6;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_diee6(  )
{
	self->s.v.frame = 99;
	self->s.v.think = ( func_t ) player_diee7;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_diee7(  )
{
	self->s.v.frame = 100;
	self->s.v.think = ( func_t ) player_diee8;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_diee8(  )
{
	self->s.v.frame = 101;
	self->s.v.think = ( func_t ) player_diee9;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_diee9(  )
{
	self->s.v.frame = 102;
	self->s.v.think = ( func_t ) player_diee9;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	PlayerDead(  );
}

void player_die_ax1(  )
{
	self->s.v.frame = 41;
	self->s.v.think = ( func_t ) player_die_ax2;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_die_ax2(  )
{
	self->s.v.frame = 42;
	self->s.v.think = ( func_t ) player_die_ax3;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_die_ax3(  )
{
	self->s.v.frame = 43;
	self->s.v.think = ( func_t ) player_die_ax4;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_die_ax4(  )
{
	self->s.v.frame = 44;
	self->s.v.think = ( func_t ) player_die_ax5;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_die_ax5(  )
{
	self->s.v.frame = 45;
	self->s.v.think = ( func_t ) player_die_ax6;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_die_ax6(  )
{
	self->s.v.frame = 46;
	self->s.v.think = ( func_t ) player_die_ax7;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_die_ax7(  )
{
	self->s.v.frame = 47;
	self->s.v.think = ( func_t ) player_die_ax8;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_die_ax8(  )
{
	self->s.v.frame = 48;
	self->s.v.think = ( func_t ) player_die_ax9;
	self->s.v.nextthink = g_globalvars.time + 0.1;

}

void player_die_ax9(  )
{
	self->s.v.frame = 49;
	self->s.v.think = ( func_t ) player_die_ax9;
	self->s.v.nextthink = g_globalvars.time + 0.1;

	PlayerDead(  );
}

void Headless_Think(  )
{
	self->s.v.frame = self->s.v.frame + 1;
	if ( self->s.v.frame == 7 || self->s.v.frame == 18 )
	{
		self->s.v.nextthink = g_globalvars.time + 10 + g_random(  ) * 10;
		self->s.v.think = ( func_t ) SUB_Remove;
		return;
	}
	self->s.v.nextthink = g_globalvars.time + 0.1;
}
