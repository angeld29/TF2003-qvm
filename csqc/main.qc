// csprogs.dat — клиентский модуль TF2003 (CSQC, без предикции).
// Этап 3 (тест всех аспектов CSQC-каналов mvdsv):
//   - статы 32..127 (clientstat/pointerstat) — Hud_Draw (hud.qc);
//   - CSQC-сущности (флаги сервера, svc_fte_csqcentities) — CSQC_Ent_Update/Remove,
//     рисуются 2D-маркерами в Hud_DrawCSQC;
//   - sendevent клиент→сервер (clcfte_qcrequest) — команды csqc_sendevent
//     и csqc_sendevent6 (6 аргументов — терминатор);
//   - cgamepacket сервер→клиент — эхо "echo" разбирается в CSQC_Parse_Event.
// Детали: docs/tf2003_csqc_client_plan.md + docs/ezquake_csqc_pr2.md.
//
// Примечание (PR1VM, Step 0): слоты флагов — явные глобалы, НЕ массивы fm_*[8]:
// динамическая индексация массивов компилируется в GLOBALADDRESS/LOADA_F/
// BOUNDCHECK, которых нет в классическом подмножестве опкодов ezquake-PR1.

// Эхо sendevent → cgamepacket (E1). Текст собираем и выводим dprint-ом в момент
// приёма (валидация всех argtypes); кросс-кадрово храним только скаляры (счётчик,
// маска типов, время). Полный текстовый echo «пережил» бы кадр лишь с
// persistent-строками движка — кольцо PR1VM даёт стабильность внутри вызова,
// не между кадрами (см. ezquake csqc_client/PR1VM temp-ring; ограничение E1).
float g_echo_time;
float g_echo_n;
float g_echo_argmask;
float g_echo_has;

// CSQC_Input_Frame debug-канарейка (постоянная, под cvar csqc_inputdebug через
// builtin cvar #45; движок ezquake вызывает Input_Frame перед отправкой каждого
// usercmd, заполняя input_* глобалы). Скаляры: последние buttons/impulse +
// счётчик вызовов (≈ частоте отправки команд).
float g_if_btns;
float g_if_imp;
float g_if_cnt;

// C1.2: счётчики событий CSQC_InputEvent (модуль возвращает 0 — не блокирует).
float g_ie_mabs;	// IE_MOUSEABS (3)
float g_ie_mdelta;	// IE_MOUSEDELTA (2)
float g_ie_keys;	// IE_KEYDOWN/IE_KEYUP (0/1)

// C6.1: сводная верификация builtins — эффект-пикер (видимые: звук/dlight/частицы/
// te_*/beams) + smoke-статусы (невидимые: #459/#345/#346/#347/buf_*). Только под
// cvar csqc_d2d 1 (см. CSQC_Fx_Do/CSQC_Fx_Smoke/Hud_DrawCSQC). Реестр «не проверено»
// в docs/ezquake_csqc_client_roadmap.md §C6 закрывается этим шагом.
const float CSQC_FXMAX = 33;
float g_fx_sel;		// текущий индекс эффекта (0..31)
float g_fx_fire;	// срабатываний LMB с момента включения d2d
float g_fx_count;	// всего IE_KEYDOWN под d2d (для отладки доставки)
float g_sm_run;		// smoke выполнен для текущего включения d2d (0/1)
float g_p1a_r;		// P1a: фикс. сэмпл random() (обновляется на включение d2d)
vector g_p1a_rv;	// P1a: фикс. сэмпл randomvec() (иначе каждая строка скачет)
float g_sm459;		// edict_num(1) — entity-handle (≥0; 0 только вне арены)
float g_sm345;		// getinputstate(input_sequence) — 0/1
float g_sm346;		// setsensitivityscaler(1) — 1 (void, вызвано)
float g_sm347;		// runstandardplayerphysics(edict_num(0)) — 1 (не краш)
float g_sm_buf;		// buf-цепочка #460..469 — число успешных шагов
float g_dbg_prev;	// предыдущий csqc_dbg (консоль-смоук T1/T2 при включении экрана 4)
float g_t5_iss;		// T5: isserver() (считается на включение экрана 4)
float g_t5_slot;	// T5: num_for_edict(spawn-сущности) — слот
float g_t5_free0;	// T5: wasfreed(до remove)
float g_t5_free1;	// T5: wasfreed(после remove)

// Шаг 2 (ADR 0019-план): debug-уровень модуля живёт в модульном cvar csqc_dbg
// (registercvar #93, создаётся в CSQC_Init; движковых csqc_d2d/csqc_inputdebug
// больше нет). Уровень: 0 — выкл; 1 — графика+курсор; 2 — текст «до ie»;
// 3 — остальное (экраны, каждый <= 10 строк).
float DbgLevel() {
	return cvar("csqc_dbg");
}
float DbgAt(float lvl) {
	return cvar("csqc_dbg") >= lvl;
}

// 2D-маркеры флагов (payload сервера: entnum, skin=team, frame=carried,
// flagf-маркер hi-бита, [таймер]). Детали: docs/ezquake_csqc_pr2.md.
const float CSQC_MAXFLAGS = 8;
// flagf-бит: в payload есть таймер — доставлен hi-бит 40 сервера (зеркало
// GCSQC_FLAGF_T40 в include/g_csqc.h).
const float CSQC_FLAGF_T40 = 1;

// Слоты (все поля — float; entnum уникален). Скалярные глобалы на слот:
//  slot<N>_entnum / _team / _carried / _t40 / _time / _used.
float slot0_entnum, slot0_team, slot0_carried, slot0_t40, slot0_time, slot0_used;
float slot1_entnum, slot1_team, slot1_carried, slot1_t40, slot1_time, slot1_used;
float slot2_entnum, slot2_team, slot2_carried, slot2_t40, slot2_time, slot2_used;
float slot3_entnum, slot3_team, slot3_carried, slot3_t40, slot3_time, slot3_used;
float slot4_entnum, slot4_team, slot4_carried, slot4_t40, slot4_time, slot4_used;
float slot5_entnum, slot5_team, slot5_carried, slot5_t40, slot5_time, slot5_used;
float slot6_entnum, slot6_team, slot6_carried, slot6_t40, slot6_time, slot6_used;
float slot7_entnum, slot7_team, slot7_carried, slot7_t40, slot7_time, slot7_used;

// Доступ к слоту — через сравнения с константой (без индексируемых массивов).
float FlagSlot_used(float i) {
	if (i == 0) return slot0_used;
	if (i == 1) return slot1_used;
	if (i == 2) return slot2_used;
	if (i == 3) return slot3_used;
	if (i == 4) return slot4_used;
	if (i == 5) return slot5_used;
	if (i == 6) return slot6_used;
	return slot7_used;
}

float FlagSlot_entnum(float i) {
	if (i == 0) return slot0_entnum;
	if (i == 1) return slot1_entnum;
	if (i == 2) return slot2_entnum;
	if (i == 3) return slot3_entnum;
	if (i == 4) return slot4_entnum;
	if (i == 5) return slot5_entnum;
	if (i == 6) return slot6_entnum;
	return slot7_entnum;
}

float FlagSlot_team(float i) {
	if (i == 0) return slot0_team;
	if (i == 1) return slot1_team;
	if (i == 2) return slot2_team;
	if (i == 3) return slot3_team;
	if (i == 4) return slot4_team;
	if (i == 5) return slot5_team;
	if (i == 6) return slot6_team;
	return slot7_team;
}

float FlagSlot_carried(float i) {
	if (i == 0) return slot0_carried;
	if (i == 1) return slot1_carried;
	if (i == 2) return slot2_carried;
	if (i == 3) return slot3_carried;
	if (i == 4) return slot4_carried;
	if (i == 5) return slot5_carried;
	if (i == 6) return slot6_carried;
	return slot7_carried;
}

float FlagSlot_t40(float i) {
	if (i == 0) return slot0_t40;
	if (i == 1) return slot1_t40;
	if (i == 2) return slot2_t40;
	if (i == 3) return slot3_t40;
	if (i == 4) return slot4_t40;
	if (i == 5) return slot5_t40;
	if (i == 6) return slot6_t40;
	return slot7_t40;
}

float FlagSlot_time(float i) {
	if (i == 0) return slot0_time;
	if (i == 1) return slot1_time;
	if (i == 2) return slot2_time;
	if (i == 3) return slot3_time;
	if (i == 4) return slot4_time;
	if (i == 5) return slot5_time;
	if (i == 6) return slot6_time;
	return slot7_time;
}

void FlagSlot_set(float i, float entnum, float team, float carried, float t40, float t) {
	if (i == 0) {
		slot0_entnum = entnum; slot0_team = team; slot0_carried = carried;
		slot0_t40 = t40; slot0_time = t; slot0_used = 1;
	} else if (i == 1) {
		slot1_entnum = entnum; slot1_team = team; slot1_carried = carried;
		slot1_t40 = t40; slot1_time = t; slot1_used = 1;
	} else if (i == 2) {
		slot2_entnum = entnum; slot2_team = team; slot2_carried = carried;
		slot2_t40 = t40; slot2_time = t; slot2_used = 1;
	} else if (i == 3) {
		slot3_entnum = entnum; slot3_team = team; slot3_carried = carried;
		slot3_t40 = t40; slot3_time = t; slot3_used = 1;
	} else if (i == 4) {
		slot4_entnum = entnum; slot4_team = team; slot4_carried = carried;
		slot4_t40 = t40; slot4_time = t; slot4_used = 1;
	} else if (i == 5) {
		slot5_entnum = entnum; slot5_team = team; slot5_carried = carried;
		slot5_t40 = t40; slot5_time = t; slot5_used = 1;
	} else if (i == 6) {
		slot6_entnum = entnum; slot6_team = team; slot6_carried = carried;
		slot6_t40 = t40; slot6_time = t; slot6_used = 1;
	} else if (i == 7) {
		slot7_entnum = entnum; slot7_team = team; slot7_carried = carried;
		slot7_t40 = t40; slot7_time = t; slot7_used = 1;
	}
}

void FlagSlot_clear(float i) {
	if (i == 0) slot0_used = 0;
	else if (i == 1) slot1_used = 0;
	else if (i == 2) slot2_used = 0;
	else if (i == 3) slot3_used = 0;
	else if (i == 4) slot4_used = 0;
	else if (i == 5) slot5_used = 0;
	else if (i == 6) slot6_used = 0;
	else if (i == 7) slot7_used = 0;
}

float FlagIdxByEnt(float entnum) {
	local float i;
	for (i = 0; i < CSQC_MAXFLAGS; i = i + 1)
		if (FlagSlot_used(i) == 1 && FlagSlot_entnum(i) == entnum)
			return i;
	return -1;
}

float FlagFreeSlot() {
	local float i;
	for (i = 0; i < CSQC_MAXFLAGS; i = i + 1)
		if (FlagSlot_used(i) == 0)
			return i;
	return -1;
}

void(float isnew) CSQC_Ent_Update = {
	local float e, team, carried, t, flagf, idx;

	// Параметры функции — нет; контекст сущности задаёт движок: self.entnum
	// (ADR 0017 P2/D3). payload сервера G_CSQC_Example_FlagSendEntity (без
	// WriteEntity — identity через self.entnum):
	// WriteShort(skin), WriteByte(frame), WriteByte(flagf)
	// [GCSQC_FLAGF_T40 = дошёл hi-бит 40], и если flagf — WriteLong(time).
	e = self.entnum;
	team = readshort();
	carried = readbyte();
	flagf = readbyte();
	t = -1;
	if (flagf >= CSQC_FLAGF_T40)
		t = readlong();

	idx = FlagIdxByEnt(e);
	if (idx < 0)
		idx = FlagFreeSlot();
	if (idx < 0)
		return;	// слотов нет — игнорируем

	FlagSlot_set(idx, e, team, carried, flagf >= CSQC_FLAGF_T40, t);
};

void() CSQC_Ent_Remove = {
	local float e, idx;

	// Контекст сущности — движок (self.entnum); builtin-стрим не используется
	// (ADR 0017 P2/D3).
	e = self.entnum;
	idx = FlagIdxByEnt(e);
	if (idx >= 0)
		FlagSlot_clear(idx);
};

void() CSQC_Parse_Event = {
	local string nm, s;
	local float cnt, i, t;

	nm = readstring();
	if (nm == "echo") {
		cnt = readbyte();
		s = "ECHO(";
		for (i = 0; i < cnt; i = i + 1) {
			t = readbyte();
			if (t == 0)            // float (coord)
				s = strcat(s, sprintf("%.2f ", readcoord()));
			else if (t == 1) {     // vector (3 coord)
				s = strcat(s, sprintf("%.0f/%.0f/%.0f ", readcoord(), readcoord(), readcoord()));
			}
			else if (t == 2)       // string
				s = strcat(s, sprintf("\"%s\" ", readstring()));
			else if (t == 3)       // entity
				s = strcat(s, sprintf("ent%d ", readentitynum()));
			else if (t == 4)       // int (long)
				s = strcat(s, sprintf("%.0f ", readlong()));
			else                   // UNKNOWN / не доставлено
				s = strcat(s, "? ");
			// маска типов аргументов (десятичный «цифровой код», t+1 на арг.)
			g_echo_argmask = g_echo_argmask * 10 + t + 1;
		}
		s = strcat(s, ")");
		// Валидация полного текста (все argtypes) — в консоль; на экран — только
		// скалярный индикатор (см. Hud_DrawCSQC).
		dprint(strcat("CSQC echo: ", strcat(s, "\n")));
		g_echo_n = g_echo_n + 1;
		g_echo_time = time;
		g_echo_has = 1;
		return;
	}
};

// Тестовые команды консоли клиента: посылают sendevent на сервер.
void SendEchoEvent() {
	// float, vector, string
	sendevent("csqc_echo", "fvs", 3.5, [10, 20, 30], "hello");
}

void SendEchoEvent6() {
	// float, string, vector, float, int, float — ровно 6 аргументов (терминатор)
	sendevent("csqc_echo", "fsvfif", 1.1, "abc", [5, 6, 7], 2.2, (int)42, 3.3); // (int)-каст: спецификатор 'i' (иначе F333)
}

// P1c фатальный probe: objerror #11. НЕ вызывается из общих авто-тестов
// (CSQC_Fx_Smoke при csqc_dbg) — семантика движкозависима: FTE и ezquake
// (паритет) при cvar developer 0 фатальны (дисконнект клиента: FTE —
// CSQC_Abort/EndGame, ezquake — CSQC_Client_Abort), при developer 1 —
// нефатальны (печать, модуль жив). Команда нужна для ручной проверки этой
// семантики на обоих движках.
void ObjErrorProbe() {
	dprint("P1C objerror smoke (cmd csqc_objerror)\n");
	objerror("csqc_p1c objerror smoke");
}

void(float apiver, string enginename, float enginever) CSQC_Init = {
	g_echo_time = 0;
	g_echo_n = 0;
	g_echo_argmask = 0;
	g_echo_has = 0;
	g_if_btns = 0;
	g_if_imp = 0;
	g_if_cnt = 0;
	g_ie_mabs = 0;
	g_ie_mdelta = 0;
	g_ie_keys = 0;
	// C6.1: состояние пикера/smoke чистое на старте модуля.
	g_fx_sel = 0;
	g_fx_fire = 0;
	g_fx_count = 0;
	g_sm_run = 0;
	g_sm459 = 0;
	g_sm345 = 0;
	g_sm346 = 0;
	g_sm347 = 0;
	g_sm_buf = 0;
	g_dbg_prev = 0;
	g_t5_iss = 0;
	g_t5_slot = 0;
	g_t5_free0 = 0;
	g_t5_free1 = 0;
	registercommand("csqc_sendevent");
	registercommand("csqc_sendevent6");
	// P1c фатальный probe (objerror) — отдельной командой, не в общем смоуке.
	registercommand("csqc_objerror");
	// Шаг 2: модульный debug-уровень (0..3); движковых csqc_d2d/csqc_inputdebug
	// нет — экраны читают csqc_dbg (см. DbgLevel/DbgAt).
	registercvar("csqc_dbg", "0");
};

void() CSQC_WorldLoaded = {
	// Новая карта: слоты флагов пусты, эхо-индикатор сброшен (движок вызывает
	// это на каждый вход в мир).
	local float i;
	for (i = 0; i < CSQC_MAXFLAGS; i = i + 1)
		FlagSlot_clear(i);
	g_echo_time = 0;
	g_echo_n = 0;
	g_echo_argmask = 0;
	g_echo_has = 0;
	g_if_btns = 0;
	g_if_imp = 0;
	g_if_cnt = 0;
	g_ie_mabs = 0;
	g_ie_mdelta = 0;
	g_ie_keys = 0;
	// C6.1: новая карта — smoke переиграется при включении csqc_d2d.
	g_sm_run = 0;
	g_sm459 = 0;
	g_sm345 = 0;
	g_sm346 = 0;
	g_sm347 = 0;
	g_sm_buf = 0;
};

void(float width, float height, float menushown) CSQC_UpdateView = {
	clearscene();
	setproperty(VF_DRAWWORLD, 1);
	setproperty(VF_DRAWCROSSHAIR, 1);
	setproperty(VF_DRAWENGINESBAR, 1);
	addentities(MASK_ENGINE | MASK_VIEWMODEL);
	renderscene();

	Hud_Draw();
};

float(string cmd) CSQC_ConsoleCommand = {
	tokenize(cmd);
	if (argv(0) == "csqc_sendevent") {
		SendEchoEvent();
		return 1;
	}
	if (argv(0) == "csqc_sendevent6") {
		SendEchoEvent6();
		return 1;
	}
	if (argv(0) == "csqc_objerror") {
		ObjErrorProbe();
		return 1;
	}
	return 0;
};

void() CSQC_Shutdown = {
};

// CSQC_Input_Frame (без предикции): движок (ezquake) вызывает перед отправкой
// каждого usercmd и заполняет input_* (timelength/angles/movevalues/buttons/
// impulse) — модуль может читать кнопки/импульсы и (позже, для меню) менять
// их. Сейчас — только канарейка под csqc_inputdebug; usercmd не изменяем.
void() CSQC_Input_Frame = {
	if (DbgAt(1) == 0)
		return;
	g_if_btns = input_buttons;
	g_if_imp = input_impulse;
	g_if_cnt = g_if_cnt + 1;
};

// C6.1: имя эффекта каталога по индексу (для HUD-строки fx#). Полный набор
// эффектов ezquake CSQC (реестр docs/ezquake_csqc_client_roadmap.md §C6): звук/
// dlight/частицы/te_* #405-#430 (кроме не-зарегистрированных #426/#433 и
// #431 te_beam — требует progs/beam.mdl, которого в стенде нет).
string CSQC_FxName(float idx) {
	if (idx == 0)  return "sound railgun";
	if (idx == 1)  return "sound flmgr";
	if (idx == 2)  return "sound detpack";
	if (idx == 3)  return "dlight red";
	if (idx == 4)  return "parts blood";
	if (idx == 5)  return "parts spark";
	if (idx == 6)  return "parts explosion";
	if (idx == 7)  return "te_blood";
	if (idx == 8)  return "te_bloodshower";
	if (idx == 9)  return "te_explosionrgb";
	if (idx == 10) return "te_particlecube";
	if (idx == 11) return "te_particlerain";
	if (idx == 12) return "te_particlesnow";
	if (idx == 13) return "te_spark";
	if (idx == 14) return "te_gunshotquad";
	if (idx == 15) return "te_spikequad";
	if (idx == 16) return "te_superspikequad";
	if (idx == 17) return "te_explosionquad";
	if (idx == 18) return "te_smallflash";
	if (idx == 19) return "te_customflash";
	if (idx == 20) return "te_gunshot";
	if (idx == 21) return "te_spike";
	if (idx == 22) return "te_superspike";
	if (idx == 23) return "te_explosion";
	if (idx == 24) return "te_tarexplosion";
	if (idx == 25) return "te_wizspike";
	if (idx == 26) return "te_knightspike";
	if (idx == 27) return "te_lavasplash";
	if (idx == 28) return "te_explosion2";
	if (idx == 29) return "te_lightning1";
	if (idx == 30) return "te_lightning2";
	if (idx == 31) return "te_lightning3";
	if (idx == 32) return "te_beam";
	return "?";
}

// C6.1: запуск текущего эффекта каталога (0..CSQC_FXMAX-1). Точка — по
// направлению взгляда, как у выстрела: makevectors(viewangles) -> v_forward;
// org = vieworg + v_forward*64; направленные эффекты летят по v_forward.
// bbox-эффекты (bloodshower/particlecube/rain/snow) — куб org±24; beams —
// отрезок org..org+v_forward*64. Движковый #1 makevectors регистрируется
// ezquake (FTE-паритет, C6.1). #431 te_beam присутствует (idx 32): при
// отсутствии модели (progs/beam.mdl) ezquake пишет варн в консоль и делает
// no-op — без дисконнекта (движковый фикс C6.1).
void CSQC_Fx_Do(float idx) {
	local vector org, end, dir, mn, mx;
	local float p;

	if (idx < 0 || idx >= CSQC_FXMAX)
		return;
	makevectors(getproperty(15));		// #1: v_forward/v_right/v_up из viewangles
	org = getproperty(11) + v_forward * 64;	// vieworg + 64 по взгляду
	end = org + v_forward * 64;			// beams: org..org+64 по взгляду
	dir = v_forward;					// направление — как выстрел
	mn = org - '24 24 24';				// bbox для #406/#408/#409/#410
	mx = org + '24 24 24';

	if (idx == 0) {
		localsound("weapons/railgun.wav");
		return;
	}
	if (idx == 1) {
		localsound("weapons/flmgrexp.wav");
		return;
	}
	if (idx == 2) {
		localsound("weapons/detpack.wav");
		return;
	}
	if (idx == 3) {
		dynamiclight_add(org, 200, '1 0 0');
		return;
	}
	// частицы: #335 particleeffectnum -> #337 pointparticles
	p = particleeffectnum("blood");
	if (p > 0 && idx == 4) {
		pointparticles(p, org, dir, 50);
		return;
	}
	p = particleeffectnum("spark");
	if (p > 0 && idx == 5) {
		pointparticles(p, org, dir, 50);
		return;
	}
	p = particleeffectnum("explosion");
	if (p > 0 && idx == 6) {
		pointparticles(p, org, dir, 50);
		return;
	}
	if (idx == 7) {
		te_blood(org, dir, 40);
		return;
	}
	if (idx == 8) {
		te_bloodshower(mn, mx, 100, 40);
		return;
	}
	if (idx == 9) {
		te_explosionrgb(org, '1 0 0');
		return;
	}
	if (idx == 10) {
		te_particlecube(mn, mx, dir, 40, 73, 0, 0);
		return;
	}
	if (idx == 11) {
		te_particlerain(mn, mx, '0 0 -160', 60, 0);
		return;
	}
	if (idx == 12) {
		te_particlesnow(mn, mx, '0 0 -80', 60, 0);
		return;
	}
	if (idx == 13) {
		te_spark(org, dir, 30);
		return;
	}
	if (idx == 14) {
		te_gunshotquad(org);
		return;
	}
	if (idx == 15) {
		te_spikequad(org);
		return;
	}
	if (idx == 16) {
		te_superspikequad(org);
		return;
	}
	if (idx == 17) {
		te_explosionquad(org);
		return;
	}
	if (idx == 18) {
		te_smallflash(org);
		return;
	}
	if (idx == 19) {
		te_customflash(org, 200, 0.2, '1 0 0');
		return;
	}
	if (idx == 20) {
		te_gunshot(org);
		return;
	}
	if (idx == 21) {
		te_spike(org);
		return;
	}
	if (idx == 22) {
		te_superspike(org);
		return;
	}
	if (idx == 23) {
		te_explosion(org);
		return;
	}
	if (idx == 24) {
		te_tarexplosion(org);
		return;
	}
	if (idx == 25) {
		te_wizspike(org);
		return;
	}
	if (idx == 26) {
		te_knightspike(org);
		return;
	}
	if (idx == 27) {
		te_lavasplash(org);
		return;
	}
	if (idx == 28) {
		te_explosion2(org, 255, 0.1);
		return;
	}
	if (idx == 29) {
		te_lightning1(edict_num(1), org, end);
		return;
	}
	if (idx == 30) {
		te_lightning2(edict_num(1), org, end);
		return;
	}
	if (idx == 31) {
		te_lightning3(edict_num(1), org, end);
		return;
	}
	if (idx == 32) {
		te_beam(edict_num(1), org, end);	// нет beam.mdl → варн+no-op (движок)
		return;
	}
}

// FTE-пул Шаг 7 (часть 2): канарейки entity-слоя трасс (dispatch MOVE_* + forent).
// Окружение «как в FTE»: ezquake сущности игроков НЕ фабрикует (сервер mvdsv+TF
// игроков в CSQC не шлёт) — поэтому зеркально-зависимых тестов (C7 self/selfsk/play)
// нет, они N/A до серверной эмиссии игроков модом. Тест-сущности — spawn/setsize
// с .solid (обязательно после классификации) и опционально .flags=FL_MONSTER;
// каждая сразу удаляется. Все статусы в консоль (dprint).
void CSQC_Fx_Trace7() {
	local vector eye, above, tgt;
	local entity ce;

	eye = getproperty(11);
	above = eye + '0 0 96';
	makevectors(getproperty(15));

	// --- dispatch: стена-solid (solid=SOLID_BBOX) ---
	tgt = eye + v_forward * 96;
	ce = spawn();
	if (ce != world) {
		setorigin(ce, tgt);
		setsize(ce, '-8 -8 -8', '8 8 8');
		ce.solid = SOLID_BBOX;

		// NORMAL (0): бьёт стену; NOMONSTERS (1): сущности игнорирует.
		traceline(eye, eye + v_forward * 140, MOVE_NORMAL, world);
		dprint(sprintf("C7 nrm frac %g wall %s\n", trace_fraction,
			(trace_ent == ce) ? "yes" : "no"));
		traceline(eye, eye + v_forward * 140, MOVE_NOMONSTERS, world);
		dprint(sprintf("C7 nom frac %g wall %s world %s\n", trace_fraction,
			(trace_ent == ce) ? "yes" : "no",
			(trace_ent == world) ? "yes" : "no"));
		remove(ce);
	}

	// --- SOLID_NOT: NORMAL мимо, EVERYTHING (32) бьёт даже несолидную ---
	ce = spawn();
	if (ce != world) {
		setorigin(ce, tgt);
		setsize(ce, '-8 -8 -8', '8 8 8');
		ce.solid = SOLID_NOT;
		traceline(eye, eye + v_forward * 140, MOVE_NORMAL, world);
		dprint(sprintf("C7 notn frac %g wall %s\n", trace_fraction,
			(trace_ent == ce) ? "yes" : "no"));
		traceline(eye, eye + v_forward * 140, MOVE_EVERYTHING, world);
		dprint(sprintf("C7 evry frac %g wall %s\n", trace_fraction,
			(trace_ent == ce) ? "yes" : "no"));
		remove(ce);
	}

	// --- SOLID_TRIGGER: TRIGGERS (16) бьёт только триггер ---
	ce = spawn();
	if (ce != world) {
		setorigin(ce, tgt);
		setsize(ce, '-8 -8 -8', '8 8 8');
		ce.solid = SOLID_TRIGGER;
		traceline(eye, eye + v_forward * 140, MOVE_TRIGGERS, world);
		dprint(sprintf("C7 trig frac %g wall %s\n", trace_fraction,
			(trace_ent == ce) ? "yes" : "no"));
		remove(ce);
	}

	// --- MISSILE (2): FL_MONSTER-сущность ловится ---
	ce = spawn();
	if (ce != world) {
		setorigin(ce, tgt);
		setsize(ce, '-8 -8 -8', '8 8 8');
		ce.solid = SOLID_BBOX;
		ce.flags = FL_MONSTER;
		traceline(eye, eye + v_forward * 140, MOVE_MISSILE, world);
		dprint(sprintf("C7 miss frac %g wall %s\n", trace_fraction,
			(trace_ent == ce) ? "yes" : "no"));
		remove(ce);
	}

	// --- forent-skip: трасса из точки НАД головой с forent=ce не бьёт саму
	// стену ce — «не бьёт себя» у своего спавна. (owner-игнор в движке есть,
	// но канарейка снимается: запись .owner даёт opcode STOREF_I (#222), который
	// PR1VM пока не исполняет — отдельный шаг.)
	ce = spawn();
	if (ce != world) {
		setorigin(ce, above + v_forward * 96);
		setsize(ce, '-8 -8 -8', '8 8 8');
		ce.solid = SOLID_BBOX;
		traceline(above, above + v_forward * 140, MOVE_NORMAL, ce);
		dprint(sprintf("C7 foe frac %g wall %s ent %g\n", trace_fraction,
			(trace_ent == ce) ? "yes" : "no", trace_ent.entnum));
		remove(ce);
	}
}

// C6.1: smoke-статусы «невидимых» builtins — однократно на включение csqc_d2d.
// Только скалярные результаты (строки между кадрами не храним; PR1VM temp-ring).
// #459 edict_num(1), #345 getinputstate(input_sequence), #346 setsensitivityscaler(1),
// #347 runstandardplayerphysics(edict_num(0)) (не краш; суть — C5/EXT_CSQC_1),
// buf-цепочка #460..469. g_sm_buf — число успешных шагов (max 7).
void CSQC_Fx_Smoke() {
	local entity e;
	local float b, ok;
	local float r, c1, c2, e1, e2;	// P1c: registercvar/cvar/cvar_set/checkextension
	local vector mv;				// P1b: stov
	local string z;					// P1b: strzone/strunzone
	local entity ce, cf;			// P1d: обход/одиночные spawn
	local entity pe0, pe1, pe2, pe3, pe4;	// P1d: 5 явных spawn-сущностей (без массивов)
	local float nu, nf, nfind;		// P1d: счётчики
	local float df, wm, pc, pc2;		// P1d C2: droptofloor/walkmove/pointcontents

	// #459 edict_num(1): валидная entity не-world (0 вне арены). entity->float
	// qcc-каст запрещён — статус 1/0.
	e = edict_num(1);
	g_sm459 = (e != world) ? 1 : 0;
	g_sm345 = getinputstate(input_sequence);
	setsensitivityscaler(1);
	g_sm346 = 1;
	runstandardplayerphysics(edict_num(0));
	g_sm347 = 1;

	// buf-цепочка: create -> add x2 -> getsize -> set/get -> sort -> implode ->
	// free -> del. Семантика ezquake C2.2: add order<=0 — append.
	ok = 0;
	b = buf_create();
	if (b > 0) {
		ok = ok + 1;					// create
		bufstr_add(b, "zz", 0);
		bufstr_add(b, "aa", 0);			// [zz, aa]
		if (buf_getsize(b) == 2)
			ok = ok + 1;				// add x2 / getsize
		bufstr_set(b, 1, "mm");			// [zz, mm]
		if (bufstr_get(b, 1) == "mm")
			ok = ok + 1;				// set / get
		buf_sort(b, 0, 0);				// -> [mm, zz]
		if (bufstr_get(b, 0) == "mm")
			ok = ok + 1;				// sort
		if (buf_implode(b, ",") == "mm,zz")
			ok = ok + 1;				// implode
		bufstr_free(b, 0);				// -> [zz]
		if (buf_getsize(b) == 1)
			ok = ok + 1;				// free
		buf_del(b);
		ok = ok + 1;					// del
	}
	g_sm_buf = ok;

	// P1c: cvar/exec smoke — результат в консоль клиента (dprint).
	// registercvar: 1 на первом включении d2d (создаёт движковый cvar csqc_p1c),
	// далее 0 (уже есть); cvar_set ставит 7. calltimeofday — функция "timeofday"
	// в модуле не объявлена -> no-op (не краш). Фатальный objerror (#11) из
	// общего смоука вынесен: семантика зависит от движка/cvar developer
	// (FTE/ezquake-паритет: developer 0 — фатальна), дёргается отдельной
	// командой csqc_objerror (см. ObjErrorProbe).
	r = registercvar("csqc_p1c", "1");
	c1 = cvar("csqc_p1c");
	cvar_set("csqc_p1c", "7");
	c2 = cvar("csqc_p1c");
	e1 = checkextension("DP_REGISTERCVAR");
	e2 = checkextension("NOSUCH_EXT");
	dprint(sprintf("P1C reg %g cv %g->%g ext %g/%g\n", r, c1, c2, e1, e2));
	calltimeofday();
	// #46 localcmd — echo в консоль клиента. Ведущий \n изолирует от остатка
	// буфера команд движка; терминатор обязателен не в самой строке (парсер
	// буфера делит по \n/;, см. cmd.c Cbuf_ExecuteEx), а для чистоты вывода.
	localcmd("\necho csqc-p1c localcmd\n");

	// P1b: строки/конверсии smoke (консоль). Ожидания: v "'1.0 2.0 3.0'",
	// l 3, f 1.5, sub "ell", stov 1 2 3, cv "\"0\"", z "zoned" (ring; unzone no-op).
	dprint(sprintf("P1B v %s l %g f %g sub %s\n",
		vtos('1 2 3'), strlen("abc"), stof("1.5"), substring("hello", 1, 3)));
	mv = stov("1 2 3");
	z = strzone("zoned");
	dprint(sprintf("P1B stov %g %g %g cv \"%s\" z %s\n",
		mv_x, mv_y, mv_z, cvar_string("csqc_dbg"), z));
	strunzone(z);

	// P1e: подсистемы — прекеши (side-effect), particle (#48), lightstyle (#35,
	// no-op), ambientsound (#74, тихо). sound/breakpoint/setpause — регистрация.
	// Возврат precache_sound/precache_file не печатаем: на FTE он не-строковый
	// (PF_cs_PrecacheSound не пишет OFS_RETURN, precachefile — число download);
	// на ezquake возвращает имя. Печатаем константные имена.
	precache_model("progs/tf_flag.mdl");
	precache_sound("weapons/railgun.wav");
	precache_file("maps/bastion.bsp");
	dprint(sprintf("P1E pm %s ps %s pf %s\n",
		"progs/tf_flag.mdl", "weapons/railgun.wav", "maps/bastion.bsp"));
	particle('0 0 0', '0 1 0', 73, 8);
	lightstyle(1, "a");
	ambientsound('0 0 0', "weapons/detpack.wav", 0.15, 0);

	// P1d C1 (functional): 5 явных spawn → set*/обход/remove. nextent идёт по
	// всему пулу (сетевые+свои) — в очистке НЕ используем «nextent→remove всех»
	// (на сетевой сущности remove игнор → бесконечный цикл при живых сетевых).
	nu = 0;
	pe0 = spawn();
	if (pe0 != world) { nu = nu + 1; setorigin(pe0, '16 16 16'); setmodel(pe0, "progs/tf_flag.mdl"); setsize(pe0, '-8 -8 -8', '8 8 8'); }
	pe1 = spawn();
	if (pe1 != world) { nu = nu + 1; setorigin(pe1, '16 48 16'); setmodel(pe1, "progs/tf_flag.mdl"); setsize(pe1, '-8 -8 -8', '8 8 8'); }
	pe2 = spawn();
	if (pe2 != world) { nu = nu + 1; setorigin(pe2, '16 80 16'); setmodel(pe2, "progs/tf_flag.mdl"); setsize(pe2, '-8 -8 -8', '8 8 8'); }
	pe3 = spawn();
	if (pe3 != world) { nu = nu + 1; setorigin(pe3, '16 112 16'); setmodel(pe3, "progs/tf_flag.mdl"); setsize(pe3, '-8 -8 -8', '8 8 8'); }
	pe4 = spawn();
	if (pe4 != world) { nu = nu + 1; setorigin(pe4, '16 144 16'); setmodel(pe4, "progs/tf_flag.mdl"); setsize(pe4, '-8 -8 -8', '8 8 8'); }
	nf = 0;
	ce = world;
	while (1) {
		ce = nextent(ce);
		if (ce == world)
			break;
		nf = nf + 1;
		if (nf == 1)
			eprint(ce);
	}
	nfind = 0;
	ce = world;
	while (1) {
		ce = find(ce, model, "progs/tf_flag.mdl");
		if (ce == world)
			break;
		nfind = nfind + 1;
	}
	cf = findradius('16 16 16', 300);
	coredump();
	dprint(sprintf("P1D spawn %g next %g find %g rad %s\n",
		nu, nf, nfind, (cf == world) ? "none" : "yes"));
	if (pe0 != world) remove(pe0);
	if (pe1 != world) remove(pe1);
	if (pe2 != world) remove(pe2);
	if (pe3 != world) remove(pe3);
	if (pe4 != world) remove(pe4);
	coredump();

	// P1d C2 (functional): трассы/физика. traceline вниз от точки над игроком
	// (fraction<1, endpos_z), pointcontents ниже, droptofloor/walkmove на
	// спавн-сущности через self (потом self возвращаем в world).
	mv = getproperty(11) + '0 0 96';
	traceline(mv, mv + '0 0 -600', 0, world);
	dprint(sprintf("C2 tr frac %g endz %g\n", trace_fraction, trace_endpos_z));
	pc = pointcontents(getproperty(11));
	pc2 = pointcontents(mv + '0 0 -400');
	dprint(sprintf("C2 pc eye %g below %g\n", pc, pc2));
	ce = spawn();
	if (ce != world) {
		setorigin(ce, mv);
		self = ce;
		df = droptofloor();
		dprint(sprintf("C2 drop %g z %g\n", df, ce.origin_z));
		wm = walkmove(0, 16);
		dprint(sprintf("C2 walk %g z %g\n", wm, ce.origin_z));
		self = world;
		remove(ce);
	}

	// P1d C2 entity-слой (Шаг 7): стена-спавн впереди по взгляду; traceline должен
	// попасть в неё (trace_ent==ce), после remove — снова в мир. Вывод идентичности:
	// entnum (0 у своих), classname/model полей найденной сущности. Со Шага 7
	// (часть 2) стена требует solid=SOLID_BBOX (классификация MOVE_*).
	makevectors(getproperty(15));
	ce = spawn();
	if (ce != world) {
		setorigin(ce, getproperty(11) + v_forward * 48);
		setsize(ce, '-8 -8 -8', '8 8 8');
		ce.solid = SOLID_BBOX;
		traceline(getproperty(11), getproperty(11) + v_forward * 96, 0, world);
		dprint(sprintf("C2E tr %g wall %s entnum %g class \"%s\" model \"%s\" world %s\n",
			trace_fraction,
			(trace_ent == ce) ? "yes" : "no",
			trace_ent.entnum,
			trace_ent.classname,
			trace_ent.model,
			(trace_ent == world) ? "yes" : "no"));
		remove(ce);
		traceline(getproperty(11), getproperty(11) + v_forward * 96, 0, world);
		dprint(sprintf("C2E2 tr %g world %s\n", trace_fraction,
			(trace_ent == world) ? "yes" : "no"));
	}

	// FTE-пул Шаг 7 (часть 2): канарейки dispatch MOVE_* + forent-skip.
	CSQC_Fx_Trace7();
}

// C1.2: события ввода от движка (csdefs.qc:159; типы IE_* 0..6). Счётчики —
// канарейка C1.2; возврат 0 (события не блокируем) кроме C6.1-пикера под
// csqc_d2d 1: MOUSE1 -> эффект, MOUSE2 -> следующий (handled 1 — управление/
// атаку этими кнопками под тестом не трогаем).
float CSQC_InputEvent(float evtype, float scanx, float chary, float devid) {
	local string k;

	if (evtype == 3)
		g_ie_mabs = g_ie_mabs + 1;		// IE_MOUSEABS
	else if (evtype == 2)
		g_ie_mdelta = g_ie_mdelta + 1;	// IE_MOUSEDELTA
	else if (evtype == 0 || evtype == 1)
		g_ie_keys = g_ie_keys + 1;		// IE_KEYDOWN/UP

	// C6.1: эффект-пикер — потребляем MOUSE1/2 (return 1) ТОЛЬКО на экране 3
	// (csqc_dbg 3), иначе клики уходят движку (атака/бинды работают в игре).
	if (DbgAt(3) == 1 && (evtype == 0 || evtype == 1)) {
		k = keynumtostring(scanx);
		if (k == "MOUSE1" || k == "MOUSE2") {
			if (evtype == 0) {
				g_fx_count = g_fx_count + 1;
				if (k == "MOUSE1") {
					CSQC_Fx_Do(g_fx_sel);
					g_fx_fire = g_fx_fire + 1;
				} else {
					g_fx_sel = (g_fx_sel + 1) % CSQC_FXMAX;
				}
			}
			return 1;
		}
	}
	return 0;
}

// Дополнительные HUD-элементы CSQC-теста (вызывается из Hud_Draw в hud.qc):
// 2D-маркеры флагов (entity-канал), эхо sendevent, счёт команд (pointerstat 40..43).
void Hud_DrawCSQC() {
	local float i, yy, tw;
	local string s;
	local float k;
	local string ks;
	local vector mp;
	local float D;
	local entity t5e;	// T5: временный spawn для etos/num_for_edict/wasfreed

	// Блок флагов — ниже всех строк hud.qc (после канарейки). Метка видна
	// всегда; строки флагов — по слотам ниже.
	yy = 176;
	drawstring([10, yy, 0], "FLAGS:", [12, 12], '1 1 1', 1, 0);
	yy = yy + 22;
	for (i = 0; i < CSQC_MAXFLAGS; i = i + 1) {
		if (FlagSlot_used(i) == 1) {
			s = sprintf("FLAG #%d T%d c%d%s %s",
				FlagSlot_entnum(i), FlagSlot_team(i), FlagSlot_carried(i),
				FlagSlot_t40(i) == 1 ? " *T40" : "",
				FlagSlot_time(i) >= 0 ? sprintf("t%.1f", FlagSlot_time(i)) : "-");
			drawstring([10, yy, 0], s, [12, 12], '1 1 1', 1, 0);
			yy = yy + 22;
		}
	}

	// Эхо sendevent → cgamepacket: индикатор в фиксированной зоне (не зависит от
	// числа флагов, не уходит за экран). Полный текст — в консоли (dprint в
	// CSQC_Parse_Event); на экране — счётчик событий + код типов аргументов.
	if (g_echo_has == 1 && time - g_echo_time < 5) {
		drawstring([460, 30, 0],
			sprintf("ECHO ev%d m%g", g_echo_n, g_echo_argmask),
			[12, 12], '1 1 0', 1, 0);
	}

	// Debug-экраны модуля (cvar csqc_dbg): 0 — выкл; 1 — графика+курсор;
	// 2 — текст «до ie»; 3 — остальное (по группам); 4 — L2-тривиалы T1 (математика).
	// Каждый экран рисуется от верхнего якоря y=470 (там, где картинки), строк на
	// экран <= 10. ECHO/FLAGS выше — не debug.
	D = cvar("csqc_dbg");
	// Экран 3: при включении — старые smoke-статусы/сэмплы и их консоль-выводы
	// (P1D/C2E/C7/smoke 459-347-buf) — только у этого экрана (у 1/2/4 их нет).
	if (D == 3 && g_sm_run == 0) {
		CSQC_Fx_Smoke();
		g_p1a_r = random();
		g_p1a_rv = randomvec();
		g_sm_run = 1;
	}
	if (D == 0) {
		setcursormode(0);
		g_sm_run = 0;
	}

	// Экран 4 (L2-тривиалы): при включении — значения и в консоль (dprint),
	// чтобы сверять их по qconsole.log (визуальные строки — ниже в D==4).
	// Строки <= 8 аргументов sprintf (varargs-лимит).
	if (D == 4 && g_dbg_prev != 4) {
		// T5: spawn→etos/num/for_edict/wasfreed → remove → wasfreed; isserver.
		g_t5_iss = isserver();
		t5e = spawn();
		if (t5e != world) {
			g_t5_slot = num_for_edict(t5e);
			g_t5_free0 = wasfreed(t5e);
			remove(t5e);
			g_t5_free1 = wasfreed(t5e);
		} else {
			g_t5_slot = 0;
			g_t5_free0 = 0;
			g_t5_free1 = 0;
		}
		dprint(sprintf("T1 asin%g acos%g atan%g atan2%g tan%g log%g\n",
			asin(1), acos(0), atan(1), atan2(1, 1), tan(0), log(2.718282)));
		dprint(sprintf("T1 am%g mod%g bs%g/%g crc16 %g/%g t%g\n",
			anglemod(450), mod(7, 3), bitshift(3, 2), bitshift(1, -1),
			crc16(0, "ABC"), crc16(1, "ABC"), gettime()));
		dprint(sprintf("T2 itos%s/%s htos%s/%s\n",
			itos(-7), itos(stoi("42")), htos(42), htos(stoh("ff"))));
		dprint(sprintf("T3 def\"%s\" type%g des\"%s\"\n",
			cvar_defstring("csqc_dbg"), cvar_type("csqc_dbg"), cvar_description("csqc_dbg")));
		dprint(sprintf("T4 s2c %g/%g/%g chr[%s] pad[%s]/[%s]\n",
			str2chr("abc", 1), str2chr("abc", -1), str2chr("abc", 5),
			chr2str(65, 66, 67), strpad(6, "ab"), strpad(-6, "ab")));
		dprint(sprintf("T4 cmp %g/%g/%g low[%s] up[%s]\n",
			strncmp("abc", "abd"), strcasecmp("AbC", "abc"),
			strncasecmp("Abc", "abb", 2), strtolower("AbC"), strtoupper("aBc")));
		dprint(sprintf("T4 rep[%s] irep[%s] ig[%s]\n",
			strreplace("a", "X", "banana"), strireplace("A", "X", "banana"),
			infoget(infoadd("", "k", "v"), "k")));
		dprint(sprintf("T5 iss%g ent entity%g num%g fr%g/%g\n",
			g_t5_iss, g_t5_slot, g_t5_slot, g_t5_free0, g_t5_free1));
		// cprint — центр-экран (видимая проверка; как FTE).
		cprint("CSQC T5 cprint");
	}
	g_dbg_prev = D;

	// --- экран 1: графика (LayerD) + курсор модуля ---
	if (D == 1) {
		setcursormode(1, "gfx/ttl_main", '0 0 0');
		tw = stringwidth("LayerD 2D test", 0, [12, 12]);
		drawfill([60, 470, 0], [tw + 16, 20, 0], '0 0 0', 0.6, 0);
		drawstring([68, 473, 0], "LayerD 2D test", [12, 12], '1 1 1', 1, 0);
		drawline(1, [60, 470, 0], [60 + tw + 15, 470, 0], '1 0 0', 1, 0);
		tw = stringwidth("LayerD 2D test", 0, [16, 16]);
		drawfill([60, 500, 0], [tw + 16, 26, 0], '0 0 0', 0.6, 0);
		drawstring([68, 503, 0], "LayerD 2D test", [16, 16], '1 1 1', 1, 0);
		drawline(1, [60, 500, 0], [60 + tw + 15, 500, 0], '1 0 0', 1, 0);
		drawpic([60, 540, 0], "gfx/ttl_main", [96, 24, 0], '1 1 1', 1, 0);
		drawpic([170, 540, 0], "gfx/ttl_main", [96, 24, 0], '1 0 0', 1, 0);
		drawsubpic([60, 576, 0], [64, 16, 0], "gfx/ttl_main", [0, 0, 0], [48, 16, 0], '0 1 0', 1, 0);
		drawstring([132, 576, 0], "subpic", [12, 12], '0 1 0', 1, 0);
		drawcharacter([60, 606, 0], 'A', [12, 12], '0 1 1', 1, 0);
		drawstring([78, 606, 0], "drawcharacter", [12, 12], '0 1 1', 1, 0);
	}
	else {
		setcursormode(0);
	}

	// --- экран 2: текст «до ie» (A4-A7, C1.2) ---
	if (D == 2) {
		yy = 470;
		k = stringtokeynum("ESCAPE");
		ks = keynumtostring(k);
		drawstring([60, yy, 0],
			sprintf("key %s (%d)", (ks == "ESCAPE") ? ks : "tokfail", k),
			[12, 12], (ks == "ESCAPE") ? '0 1 0' : '1 0 0', 1, 0);
		yy = yy + 18;
		drawstring([60, yy, 0],
			sprintf("isdemo %g host %s", isdemo(), serverkey("hostname")),
			[12, 12], '1 1 1', 1, 0);
		yy = yy + 18;
		mp = getmousepos();
		drawstring([60, yy, 0],
			sprintf("mouse %g %g", mp_x, mp_y), [12, 12], '0 1 1', 1, 0);
		yy = yy + 18;
		i = 0;
		ks = getplayerkeyvalue(0, "name");
		while (i < 4 && ks == "") {
			i = i + 1;
			ks = getplayerkeyvalue(i, "name");
		}
		if (ks != "") {
			drawstring([60, yy, 0],
				sprintf("P%d %s %s %s", i, ks,
					getplayerkeyvalue(i, "frags"), getplayerkeyvalue(i, "ping")),
				[12, 12], '1 1 1', 1, 0);
			yy = yy + 18;
		}
		mp = getproperty(204);
		drawstring([60, yy, 0],
			sprintf("scr %g %g", mp_x, mp_y), [12, 12], '1 1 1', 1, 0);
		yy = yy + 18;
		mp = getproperty(7);
		drawstring([60, yy, 0],
			sprintf("fov %g %g vp %g %g", getproperty(9), getproperty(10), mp_x, mp_y),
			[12, 12], '1 1 1', 1, 0);
		yy = yy + 18;
		drawstring([60, yy, 0],
			sprintf("ie a%g d%g k%g", g_ie_mabs, g_ie_mdelta, g_ie_keys),
			[12, 12], '1 1 1', 1, 0);
	}

	// --- экран 3: остальное (по группам: input, fx/smoke, p1a) ---
	if (D == 3) {
		yy = 470;
		if (g_if_cnt > 0) {
			drawstring([60, yy, 0],
				sprintf("in b%g i%g c%g", g_if_btns, g_if_imp, g_if_cnt),
				[12, 12], '0 1 0', 1, 0);
			yy = yy + 18;
		}
		drawstring([60, yy, 0],
			sprintf("fx#%d %s n%g", g_fx_sel, CSQC_FxName(g_fx_sel), g_fx_fire),
			[12, 12], '1 0 1', 1, 0);
		yy = yy + 18;
		drawstring([60, yy, 0],
			sprintf("smoke 459:%g 345:%g 346:%g 347:%g buf:%g",
				g_sm459, g_sm345, g_sm346, g_sm347, g_sm_buf),
			[12, 12], (g_sm345 == 1 && g_sm_buf == 7) ? '0 1 0' : '1 1 0', 1, 0);
		yy = yy + 18;
		drawstring([60, yy, 0], "LMB fx | RMB next", [12, 12], '1 1 1', 1, 0);
		yy = yy + 18;
		mp = normalize('0 3 4');
		drawstring([60, yy, 0],
			sprintf("p1a r%g vlen%g yaw%g mx%g nrm %g %g %g",
				g_p1a_r, vlen('0 3 4'), vectoyaw('0 1 0'), max(3, 1, 2),
				mp_x, mp_y, mp_z),
			[12, 12], '1 1 1', 1, 0);
		yy = yy + 18;
		drawstring([60, yy, 0],
			sprintf("p1a ri%g fl%g ce%g fa%g pw%g bd%g mn%g",
				rint(1.6), floor(1.6), ceil(1.6), fabs(-3),
				pow(2, 10), bound(1, 5, 10), min(3, 1, 2)),
			[12, 12], '1 1 1', 1, 0);
		yy = yy + 18;
		drawstring([60, yy, 0],
			sprintf("p1a si%g co%g sq%g rv %g %g %g",
				sin(1.0), cos(0), sqrt(16),
				g_p1a_rv_x, g_p1a_rv_y, g_p1a_rv_z),
			[12, 12], '1 1 1', 1, 0);
	}

	// --- экран 4: L2-тривиалы T1 (математика, 2026-09-07). Значения считаются
	// каждый кадр (детерминированные); ожидания: asin1=1.57 acos0=1.57 atan1=0.785
	// atan2(1,1)=0.785 tan0=0 log(e)=1; anglemod(450)=90 mod(7,3)=1 bs(3,2)=12
	// bs(1,-1)=0; crc16(0,"ABC")=62728 crc16(1,"ABC")=20810; gettime() растёт.
	// T2: stoi("42")=42 itos(-7)="-7" stoh("ff")=255 htos(42)="0000002a".
	if (D == 4) {
		yy = 470;
		drawstring([60, yy, 0],
			sprintf("t1 asin%g acos%g atan%g atan2%g tan%g log%g",
				asin(1), acos(0), atan(1), atan2(1, 1), tan(0), log(2.718282)),
			[12, 12], '1 1 1', 1, 0);
		yy = yy + 18;
		drawstring([60, yy, 0],
			sprintf("t1 am%g mod%g bs%g/bs%g",
				anglemod(450), mod(7, 3), bitshift(3, 2), bitshift(1, -1)),
			[12, 12], '1 1 1', 1, 0);
		yy = yy + 18;
		drawstring([60, yy, 0],
			sprintf("t1 crc16 %g/%g t%g",
				crc16(0, "ABC"), crc16(1, "ABC"), gettime()),
			[12, 12], '1 1 1', 1, 0);
		yy = yy + 18;
		drawstring([60, yy, 0],
			sprintf("t2 itos%s/%s htos%s/%s",
				itos(-7), itos(stoi("42")), htos(42), htos(stoh("ff"))),
			[12, 12], '1 1 1', 1, 0);
		yy = yy + 18;
		drawstring([60, yy, 0],
			sprintf("t3 def\"%s\" type%g",
				cvar_defstring("csqc_dbg"), cvar_type("csqc_dbg")),
			[12, 12], '1 1 1', 1, 0);
		yy = yy + 18;
		drawstring([60, yy, 0],
			sprintf("t4 s2c %g/%g/%g chr[%s] pad[%s]/[%s]",
				str2chr("abc", 1), str2chr("abc", -1), str2chr("abc", 5),
				chr2str(65, 66, 67), strpad(6, "ab"), strpad(-6, "ab")),
			[12, 12], '1 1 1', 1, 0);
		yy = yy + 18;
		drawstring([60, yy, 0],
			sprintf("t4 cmp %g/%g/%g low[%s] up[%s]",
				strncmp("abc", "abd"), strcasecmp("AbC", "abc"),
				strncasecmp("Abc", "abb", 2), strtolower("AbC"), strtoupper("aBc")),
			[12, 12], '1 1 1', 1, 0);
		yy = yy + 18;
		drawstring([60, yy, 0],
			sprintf("t4 rep[%s] irep[%s] ig[%s]",
				strreplace("a", "X", "banana"), strireplace("A", "X", "banana"),
				infoget(infoadd("", "k", "v"), "k")),
			[12, 12], '1 1 1', 1, 0);
		yy = yy + 18;
		drawstring([60, yy, 0],
			sprintf("t5 iss%g ent entity%g num%g fr%g/%g",
				g_t5_iss, g_t5_slot, g_t5_slot, g_t5_free0, g_t5_free1),
			[12, 12], '1 1 1', 1, 0);
	}
}
