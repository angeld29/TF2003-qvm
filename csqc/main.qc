// csprogs.dat — клиентский модуль TF2003 (CSQC, без предикции).
// Этап 3 (тест всех аспектов CSQC-каналов mvdsv):
//   - статы 32..127 (clientstat/pointerstat) — Hud_Draw (hud.qc);
//   - CSQC-сущности (флаги сервера, svc_fte_csqcentities) — CSQC_Ent_Update/Remove,
//     рисуются 2D-маркерами в Hud_Draw;
//   - sendevent клиент→сервер (clcfte_qcrequest) — команды csqc_sendevent
//     и csqc_sendevent6 (6 аргументов — терминатор);
//   - cgamepacket сервер→клиент — эхо "echo" разбирается в CSQC_Parse_Event.
// Детали: docs/tf2003_csqc_client_plan.md + docs/ezquake_csqc_pr2.md.

string g_echo;
float g_echo_time;

// 2D-маркеры флагов (payload сервера: entnum, skin=team, frame=carried,
// flagf-маркер hi-бита, [таймер]). Детали: docs/ezquake_csqc_pr2.md.
const float CSQC_MAXFLAGS = 8;
// flagf-бит: в payload есть таймер — доставлен hi-бит 40 сервера (зеркало
// GCSQC_FLAGF_T40 в include/g_csqc.h).
const float CSQC_FLAGF_T40 = 1;

// Слоты (все поля — float; entnum уникален).
float fm_entnum[CSQC_MAXFLAGS];
float fm_team[CSQC_MAXFLAGS];
float fm_carried[CSQC_MAXFLAGS];
float fm_time[CSQC_MAXFLAGS];
float fm_t40[CSQC_MAXFLAGS];
float fm_used[CSQC_MAXFLAGS];

float FlagIdxByEnt(float entnum) {
	local float i;
	for (i = 0; i < CSQC_MAXFLAGS; i = i + 1)
		if (fm_used[i] && fm_entnum[i] == entnum)
			return i;
	return -1;
}

float FlagFreeSlot() {
	local float i;
	for (i = 0; i < CSQC_MAXFLAGS; i = i + 1)
		if (!fm_used[i])
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

	fm_entnum[idx] = e;
	fm_team[idx] = team;
	fm_carried[idx] = carried;
	fm_time[idx] = t;
	fm_t40[idx] = flagf >= CSQC_FLAGF_T40;
	fm_used[idx] = 1;
};

void() CSQC_Ent_Remove = {
	local float idx;

	idx = FlagIdxByEnt(self.entnum);
	if (idx >= 0)
		fm_used[idx] = 0;
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
		}
		g_echo = strcat(s, ")");
		g_echo_time = time;
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
	g_echo = "";
	g_echo_time = 0;
	registercommand("csqc_sendevent");
	registercommand("csqc_sendevent6");
};

void() CSQC_WorldLoaded = {
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
	local vector ss;
	local float i, yy;
	local string s;

	ss = (vector)getproperty(VF_SCREENVSIZE);

	// маркеры флагов
	yy = 60;
	for (i = 0; i < CSQC_MAXFLAGS; i = i + 1) {
		if (fm_used[i]) {
			s = sprintf("FLAG #%d T%d c%d%s %s",
				fm_entnum[i], fm_team[i], fm_carried[i],
				fm_t40[i] ? " *T40" : "",
				fm_time[i] >= 0 ? sprintf("t%.1f", fm_time[i]) : "-");
			drawstring([10, yy, 0], s, [12, 12], '1 1 1', 1, 0);
			yy = yy + 14;
		}
	}

	// эхо sendevent → cgamepacket (гаснет через 5 c)
	if (time - g_echo_time < 5) {
		drawstring([10, yy, 0], g_echo, [12, 12], '1 1 0', 1, 0);
		yy = yy + 14;
	}

	// канарейки: hp(0)/items(15) стандартные, ps50(50) фикс, tick(40) — тест getstatf
	s = sprintf("t%d hp%g it%g ps50%g tk%g",
		time, getstatf(0), getstatf(15), getstatf(50), getstatf(40));
	drawstring([10, ss.y * 0.5 - 40, 0], s, [12, 12], '0 1 0', 1, 0);
}
