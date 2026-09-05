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

	// payload сервера G_CSQC_Example_FlagSendEntity:
	// WriteEntity(self), WriteShort(skin), WriteByte(frame),
	// WriteByte(flagf) [GCSQC_FLAGF_T40 = дошёл hi-бит 40], и если flagf — WriteLong(time)
	e = readentitynum();
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

	// Временное решение (до edict-модели клиента): entnum передаётся движком
	// через builtin-стрим (readentitynum), а не через self.entnum.
	e = readentitynum();
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
	sendevent("csqc_echo", "fsvfif", 1.1, "abc", [5, 6, 7], 2.2, 42, 3.3);
}

void(float apiver, string enginename, float enginever) CSQC_Init = {
	g_echo_time = 0;
	g_echo_n = 0;
	g_echo_argmask = 0;
	g_echo_has = 0;
	registercommand("csqc_sendevent");
	registercommand("csqc_sendevent6");
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
	return 0;
};

void() CSQC_Shutdown = {
};

// Дополнительные HUD-элементы CSQC-теста (вызывается из Hud_Draw в hud.qc):
// 2D-маркеры флагов (entity-канал), эхо sendevent, счёт команд (pointerstat 40..43).
void Hud_DrawCSQC() {
	local float i, yy;
	local string s;

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
}
