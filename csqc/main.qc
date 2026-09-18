// csprogs.dat — клиентский модуль TF2003 (CSQC, без предикции).
// Этап 3 (тест всех аспектов CSQC-каналов mvdsv):
//   - статы 32..127 (clientstat/pointerstat) — Hud_Draw (hud.qc);
//   - CSQC-сущности (флаги сервера, svc_fte_csqcentities) — CSQC_Ent_Update/Remove,
//     рисуются 2D-маркерами в Hud_DrawCSQC;
//   - sendevent клиент→сервер (clcfte_qcrequest) — команды csqc_sendevent
//     и csqc_sendevent6 (6 аргументов — терминатор);
//   - cgamepacket сервер→клиент — эхо "echo" разбирается в CSQC_Parse_Event.
// Детали: docs/plans/tf2003_csqc_client_plan.md + docs/adr/0004-csqc-pr2.md.
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

// CSQC_Input_Frame debug-канарейка (постоянная, под csqc_dbg — экран 3; движок
// ezquake вызывает Input_Frame перед отправкой каждого usercmd, заполняя input_*
// глобалы). Скаляры: последние buttons/impulse + счётчик вызовов (≈ частоте
// отправки команд).
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
// в docs/plans/ezquake_csqc_client_roadmap.md §C6 закрывается этим шагом.
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
// C5-A (предикция EXT_CSQC_1): окно модуля — live под csqc_dbg 3.
float g_c5_sc;		// servercommandframe (ack сервера; движок пишет каждый кадр)
float g_c5_cc;		// clientcommandframe (следующий отправляемый кадр)
float g_c5_iseq;	// input_sequence (последняя отправленная команда)
float g_c5_345;		// getinputstate(input_sequence) — 0/1 (живой probe #345)
// C5-B (канарейка-механика #347, экран csqc_dbg 7): синтетический арена-энт.
float g_c5b_bad;	// агрегат (число провалов) — цель 0
float g_c5b_fall;	// падение: z_start - z_min (>0 = гравитация)
float g_c5b_land;	// FL_ONGROUND после посадки — 0/1
float g_c5b_sink;	// дрейф z на земле (|z_idle - z_land|; ~0 = не проваливается)
float g_c5b_move;	// горизонтальное смещение на входе (>0 = input_* читаются)
float g_c5b_stop;	// горизонтальная скорость после обнуления входа (затухание)
// round-trip entity-соглашения #459/#512: num_for_edict(edict_num(slot)) == slot
// (int-биты vs float — ловит регресс entity-аргументов, см. ADR 0019).
float g_c5b_rt;
// C5-E: предикция локального игрока (вариант «а» — движковый мост
// player_state→edict, deltalisten #371). Модуль получает авторитетное состояние
// в CSQC_PlayerPred, предсказывает от servercommandframe к clientcommandframe.
entity g_pp_ent;		// локальная сущность игрока (мост движка)
float g_pp_valid;		// 1 — сущность получена
vector pp_org, pp_vel;	// авторитетное состояние на pp_seq
float pp_pmflags;
float pp_seq;			// servercommandframe авторитетного состояния
float pp_frame;			// текущий проигрываемый кадр
vector g_e_predorg;		// последний прогноз (для оценки ошибки)
float g_e_upd;			// число авторитетных обновлений
float g_e_err;			// |предыдущий прогноз - пере-прогноз| при обновлении
float g_e_tele;			// телепорт-снапы (>64)
float g_e_dents;		// вызовов на не-локальные (delta) сущности
// C5-E Ф1 (no-op revision): view_angles / project-unproject / listener.
vector g_f1_va;			// view_angles (движок публикует каждый кадр)
vector g_f1_proj;		// project(vieworg)
vector g_f1_pred;		// предсказанный view-origin (VF_ORIGIN override)
float g_f1_rt;			// |unproject(project(w)) - w| (round-trip)
float g_f1_lis;			// SetListener вызван (кадр активен)
// C5-E Ф2 (#504 getentity, экран csqc_dbg 10): чтение интерп. состояния
// engine-сущностей по серверному номеру. Канарейка на локальном игроке
// (player_localentnum) + инварианты.
float g_ge_bad;			// агрегат жёстких инвариантов (0 = ok)
float g_ge_done;		// console dprint канарейки выполнен (однократно)
float g_ge_act;			// getentity(local, GE_ACTIVE) — 1
float g_ge_model;		// getentity(local, GE_MODELINDEX) — >0
float g_ge_max;			// getentity(0, GE_MAXENTS) — >0
float g_ge_inv;			// getentity(0, GE_ACTIVE) — 0 (невалидный номер)
float g_ge_pants;		// getentity(local, GE_PANTSCOLOR) — 0..15
float g_ge_shirt;		// getentity(local, GE_SHIRTCOLOR) — 0..15
vector g_ge_org;		// getentity(local, GE_ORIGIN) — интерп. origin
vector g_ge_mn;			// getentity(local, GE_MINS)
vector g_ge_mx;			// getentity(local, GE_MAXS)
float g_ge_dorg;		// |GE_ORIGIN - мост g_pp_ent.origin| (информативно)
// Ф3 (renderscene takeover): mod-side отрисовка. Тест-энтити (модель оружия
// lightning) рисуется через CSQC render path на экране csqc_dbg 11
// (#302/#301 arena + renderscene); без dbg — простой renderscene.
entity g_r_e;
float g_r_midx;			// Ф3: #200 getmodelindex("progs/tf_flag.mdl")
// C4 Этап 1 (#301/#302 predraw+renderflags): две arena-тест-сущности — с .predraw,
// возвращающим PREDRAW_AUTOADD (должна рисоваться) и PREDRAW_NEXT (пропускается);
// счётчики pred/skip и значение .renderflags (RF_NOSHADOW) для дисплея.
entity g_c4_e1;
entity g_c4_e2;
float g_c4_pred;
float g_c4_skip;
float g_c4_rf;
// C4 Этап 3 (MASK_DELTA): delta-callback'и возвращают 1 (+ drawmask=MASK_DELTA) —
// движок не рисует сущность, рисует CSQC. Счётчики owned (player/backpack) и
// флаг регистрации backpack-слушателя (deltalisten).
float g_c4_owned_pl;
float g_c4_owned_bp;
float g_c4_bp_reg;
float(float isnew) CSQC_PlayerPred;
float(float isnew) CSQC_BackpackPred;	// C4 Э3: backpack delta-callback
void Pred_Run(entity ent, float endframe);
void Pred_UpdateLocal();
float g_dbg_prev;	// предыдущий csqc_dbg (консоль-смоук T1/T2 при включении экрана 4)
float g_t5_iss;		// T5: isserver() (считается на включение экрана 4)
float g_t5_slot;	// T5: num_for_edict(spawn-сущности) — слот
float g_t5_free0;	// T5: wasfreed(до remove)
float g_t5_free1;	// T5: wasfreed(после remove)
float g_2d_isc0;	// 2D-доп: iscachedpic до freepic
float g_2d_isc1;	// 2D-доп: iscachedpic после freepic
float g_2d_isz_x;	// 2D-доп: drawgetimagesize x
float g_2d_isz_y;	// 2D-доп: drawgetimagesize y
float g_vm_fo;		// VM-остаток: findfloat(.solid==SOLID_BBOX) найденный слот/-1/-2
float g_rf_n, g_rf_is, g_rf_ic, g_rf_io;	// рефлексия: num/idx solid/classname/origin
float g_rf_v, g_rf_cls;	// рефлексия: solid-значение (stof), класс после put

// Шаг 2 (ADR 0019-план): debug-уровень модуля живёт в модульном cvar csqc_dbg
// (registercvar #93, создаётся в CSQC_Init; движковых debug-cvars нет).
// Уровень: 0 — выкл; 1 — графика+курсор; 2 — текст «до ie»;
// 3 — остальное (экраны, каждый <= 10 строк).
float DbgLevel() {
	return cvar("csqc_dbg");
}
float DbgAt(float lvl) {
	// Экраны строго раздельные: уровень = точное совпадение (не >=).
	return cvar("csqc_dbg") == lvl;
}

// 2D-маркеры флагов (payload сервера: entnum, skin=team, frame=carried,
// flagf-маркер hi-бита, [таймер]). Детали: docs/adr/0004-csqc-pr2.md.
const float CSQC_MAXFLAGS = 8;
// flagf-бит: в payload есть таймер — доставлен hi-бит 40 сервера (зеркало
// GCSQC_FLAGF_T40 в include/g_csqc.h).
const float CSQC_FLAGF_T40 = 1;
// flagf-бит: далее WriteLong(count)+count filler-байт (канарейка PR228 [15b]);
// модуль дочитывает их, чтобы не было underread (зеркало GCSQC_FLAGF_TESTFILL).
const float CSQC_FLAGF_TESTFILL = 128;

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
	local float e, team, carried, t, flagf, idx, frn, fri;

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
	if (flagf & CSQC_FLAGF_T40)
		t = readlong();
	if (flagf & CSQC_FLAGF_TESTFILL)
	{	// PR228 [15b] canary filler: WriteLong(count)+count bytes; consume all
		// so the client engine does not report underread.
		frn = readlong();
		for (fri = 0; fri < frn; fri++)
			readbyte();
	}

	idx = FlagIdxByEnt(e);
	if (idx < 0)
		idx = FlagFreeSlot();
	if (idx < 0)
		return;	// слотов нет — игнорируем

	FlagSlot_set(idx, e, team, carried, (flagf & CSQC_FLAGF_T40) != 0, t);
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
	g_2d_isc0 = 0;
	g_2d_isc1 = 0;
	g_2d_isz_x = 0;
	g_2d_isz_y = 0;
	g_vm_fo = -2;
	g_rf_n = 0;
	g_rf_is = -1;
	g_rf_ic = -1;
	g_rf_io = -1;
	g_rf_v = 0;
	g_rf_cls = 0;
	registercommand("csqc_sendevent");
	registercommand("csqc_sendevent6");
	// P1c фатальный probe (objerror) — отдельной командой, не в общем смоуке.
	registercommand("csqc_objerror");
	// Ф3: просмотр списка уровней csqc_dbg с описаниями.
	registercommand("csqc_dbg_list");
	// Шаг 2: модульный debug-уровень (0..3); движковых debug-cvars нет — экраны
	// читают csqc_dbg (см. DbgLevel/DbgAt).
	registercvar("csqc_dbg", "0");
	// C4 Э2: тумблер MASK_VIEWMODEL для канарейки csqc_dbg 13 (default 1 — как было).
	registercvar("csqc_vm", "1");
	// C4 Э3 (MASK_DELTA): persistent-тумблер — модуль сам рисует delta-сущности
	// (player/backpack) и просит движок их не рисовать (return 1). Default 0
	// (listen-only, движковый рендер). ВНИМАНИЕ: CSQC render-слой неполон
	// (видимость/анимация/скины/смерть) — см. docs/plans/ezquake_csqc_client_render_layer.md.
	registercvar("csqc_delta", "0");
	// C5-E: слушатель сущностей (FTE-паритет): движок зовёт CSQC_PlayerPred при
	// обновлении сущностей с зарегистрированной моделью. Регистрируем модель
	// игрока (FTE-канон csqctest/entrypoints.qc:237) — колбэк сам фильтрует
	// локального игрока; чужие игроки идут в g_e_dents (deltalisten #371).
	g_pp_valid = 0;
	g_pp_ent = world;
	pp_seq = 0;
	pp_frame = 0;
	g_r_e = world;
	g_r_midx = -1;
	g_c4_e1 = world;
	g_c4_e2 = world;
	g_c4_pred = 0;
	g_c4_skip = 0;
	g_c4_rf = 0;
	g_c4_owned_pl = 0;
	g_c4_owned_bp = 0;
	g_c4_bp_reg = 0;
	g_e_upd = 0;
	g_e_err = 0;
	g_e_tele = 0;
	g_e_dents = 0;
	g_e_predorg = '0 0 0';
	g_f1_rt = 0;
	g_f1_lis = 0;
	g_f1_va = '0 0 0';
	g_f1_proj = '0 0 0';
	g_f1_pred = '0 0 0';
	g_ge_bad = 0;
	g_ge_done = 0;
	g_ge_act = 0;
	g_ge_model = 0;
	g_ge_max = 0;
	g_ge_inv = -1;
	g_ge_pants = -1;
	g_ge_shirt = -1;
	g_ge_org = '0 0 0';
	g_ge_mn = '0 0 0';
	g_ge_mx = '0 0 0';
	g_ge_dorg = 0;
	precache_model("progs/player.mdl");
	deltalisten("progs/player.mdl", CSQC_PlayerPred, RSES_NOLERP | RSES_NOROTATE);
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
	// C5-E: новая карта — сущность игрока придёт заново (мост движка).
	g_pp_valid = 0;
	g_pp_ent = world;
	g_r_e = world;	// Ф3: арена очищена — тест-энтити пересоздаётся
	g_r_midx = -1;
	g_c4_e1 = world;	// C4 Э1: арена очищена — тест-энты пересоздаются
	g_c4_e2 = world;
	g_c4_owned_pl = 0;	// C4 Э3: счётчики; backpack-слушатель перерегистрируется
	g_c4_owned_bp = 0;
	g_c4_bp_reg = 0;
	g_e_upd = 0;
	g_e_err = 0;
	g_e_tele = 0;
	// C5-E Ф2: сброс канарейки getentity (player_localentnum будет опубликован).
	g_ge_bad = 0;
	g_ge_done = 0;
	g_ge_act = 0;
	g_ge_model = 0;
	g_ge_max = 0;
	g_ge_inv = -1;
	g_ge_pants = -1;
	g_ge_shirt = -1;
	g_ge_org = '0 0 0';
	g_ge_mn = '0 0 0';
	g_ge_mx = '0 0 0';
	g_ge_dorg = 0;
};

// Ф3 (renderscene takeover): mod-side отрисовка тест-модели флага.
// Спавн один раз (арена), позиция — vieworg + v_forward*64 с опусканием по Z
// на 24, абсолютные углы '0 90 0' (поворот на 90° вокруг Y), drawmask=4;
// addentities(4) добавляет arena-эдикт в сцену (#301), renderscene() рисует
// (#304). Работает на FTE (нативные builtins) и ezq (takeover).
void CSQC_RenderTest() {
	local vector v;
	makevectors(getproperty(VF_ANGLES));
	v = getproperty(VF_ORIGIN) + v_forward * 64;
	v_z = v_z - 24;
	if (g_r_e == world) {
		g_r_e = spawn();
		if (g_r_e != world) {
			precache_model("progs/tf_flag.mdl");
			setmodel(g_r_e, "progs/tf_flag.mdl");
			setsize(g_r_e, '-16 -16 -16', '16 16 16');
		}
	}
	if (g_r_e != world) {
		setorigin(g_r_e, v);
		g_r_e.angles = '0 90 0';
		g_r_e.drawmask = 4;
		addentities(4);
	}
	g_r_midx = getmodelindex("progs/tf_flag.mdl");	// Ф3: #200 (реестр моделей)
}

// C4 Этап 1: .predraw-канарейки. Возврат PREDRAW_AUTOADD (0) — движок добавляет
// сущность сам; PREDRAW_NEXT (1) — пропускает (не рисуется). Движковый #301/#302
// должен вызвать predraw до добавления и учесть возврат (FTE pr_csqc.c:1450-1457).
float() CSQC_C4PredAdd = {
	g_c4_pred = g_c4_pred + 1;
	return PREDRAW_AUTOADD;
};
float() CSQC_C4PredNext = {
	g_c4_skip = g_c4_skip + 1;
	return PREDRAW_NEXT;
};

// C4 Этап 1 (csqc_dbg 12): две arena-сущности флага — e1 (predraw=AUTOADD, видна,
// .renderflags=RF_NOSHADOW) и e2 (predraw=NEXT, не добавляется). drawmask=8
// (отдельный бит), addentities(8). Работает на FTE (нативные builtins) и ezq.
void CSQC_C4Test() {
	local vector v;
	makevectors(getproperty(VF_ANGLES));
	v = getproperty(VF_ORIGIN) + v_forward * 96;
	if (g_c4_e1 == world || g_c4_e2 == world)
		precache_model("progs/tf_flag.mdl");
	if (g_c4_e1 == world) {
		g_c4_e1 = spawn();
		if (g_c4_e1 != world) {
			setmodel(g_c4_e1, "progs/tf_flag.mdl");
			setsize(g_c4_e1, '-16 -16 -16', '16 16 16');
		}
	}
	if (g_c4_e2 == world) {
		g_c4_e2 = spawn();
		if (g_c4_e2 != world) {
			setmodel(g_c4_e2, "progs/tf_flag.mdl");
			setsize(g_c4_e2, '-16 -16 -16', '16 16 16');
		}
	}
	if (g_c4_e1 != world) {
		setorigin(g_c4_e1, v - v_right * 32);
		g_c4_e1.angles = '0 90 0';
		g_c4_e1.drawmask = 8;
		g_c4_e1.predraw = CSQC_C4PredAdd;
		g_c4_e1.renderflags = RF_NOSHADOW;
		g_c4_rf = g_c4_e1.renderflags;
	}
	if (g_c4_e2 != world) {
		setorigin(g_c4_e2, v + v_right * 32);
		g_c4_e2.angles = '0 90 0';
		g_c4_e2.drawmask = 8;
		g_c4_e2.predraw = CSQC_C4PredNext;
	}
	addentities(8);
}

// Список уровней csqc_dbg с описаниями (команда csqc_dbg_list).
void CSQC_DbgList() {
	print("csqc_dbg levels:\n");
	print("  0 - off (no debug)\n");
	print("  1 - 2D graphics + cursor (LayerD)\n");
	print("  2 - pre-IE text (keys/mouse/playerkey/property)\n");
	print("  3 - input/fx picker/smokes/C5-A window\n");
	print("  4 - L2 trivial smokes (math/string/cvar/2D/fonts)\n");
	print("  5 - entity field reflection (#496-500) + stubs\n");
	print("  6 - CSQC stat wire 78/79 + bit integrity\n");
	print("  7 - C5-B #347 runstandardplayerphysics canary\n");
	print("  8 - C5-E local player prediction (deltalisten #371)\n");
	print("  9 - C5-E F1 view_angles/project-unproject/listener\n");
	print(" 10 - C5-E F2 #504 getentity canary\n");
	print(" 11 - F3 renderscene: progs/tf_flag.mdl (Y+90, z-24) + #200 midx\n");
	print(" 12 - C4 Э1: #301/#302 predraw (AUTOADD/NEXT) + renderflags\n");
	print(" 13 - C4 Э2: #301 MASK_VIEWMODEL (тумблер csqc_vm)\n");
	print(" 14 - C4 Э3: MASK_DELTA status (тумблер csqc_delta)\n");
}

void(float width, float height, float menushown) CSQC_UpdateView = {
	local float am;
	clearscene();
	setproperty(VF_DRAWWORLD, 1);
	setproperty(VF_DRAWCROSSHAIR, 1);
	setproperty(VF_DRAWENGINESBAR, 1);
	// C4 Э2: MASK_VIEWMODEL запрашиваем при csqc_vm!=0 (канарейка csqc_dbg 13).
	am = MASK_ENGINE;
	if (cvar("csqc_vm") != 0)
		am = am | MASK_VIEWMODEL;
	addentities(am);
	if (DbgAt(11))
		CSQC_RenderTest();	// Ф3: arena-эдикт через mod-render path (#301)/#302
	if (DbgAt(12))
		CSQC_C4Test();		// C4 Э1: predraw (AUTOADD/NEXT) + renderflags
	renderscene();

	// C5-E: каждый кадр — догнать предсказание до clientcommandframe.
	Pred_UpdateLocal();

	// C5-E Ф1 (экран 9): view_angles / project-unproject / listener.
	// Точка для project — ПЕРЕД камерой (проекция самой камеры вырождена: clip-w≈0,
	// FTE без guard даёт NaN). Сырые значения — диагностика (без маскировки).
	if (cvar("csqc_dbg") == 9) {
		local vector w, s, u, sd;
		g_f1_va = view_angles;
		makevectors(view_angles);
		w = getproperty(VF_ORIGIN) + v_forward * 64;
		s = project(w);
		g_f1_proj = s;
		// FTE-конвенция: project даёт NDC-z (−1..1), unproject ждёт normalized (0..1).
		sd_x = s_x; sd_y = s_y; sd_z = (s_z + 1) / 2;
		u = unproject(sd);
		g_f1_rt = vlen(u - w);
		SetListener(getproperty(VF_ORIGIN), v_forward, v_right, v_up);
		g_f1_lis = 1;
		// #303 prediction-view (FTE-паттерн prediction.qc): камера от предсказанной
		// позиции локального игрока + viewheight. Значение двигается от input.
		g_f1_pred = '0 0 0';
		if (g_pp_valid == 1 && g_pp_ent != world) {
			local vector po;
			po = g_pp_ent.origin;
			po_z = po_z + getstatf(STAT_VIEWHEIGHT);
			setproperty(VF_ORIGIN, po);
			setproperty(VF_ANGLES, view_angles);
			g_f1_pred = po;
		}
	}

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
	if (argv(0) == "csqc_dbg_list") {
		CSQC_DbgList();
		return 1;
	}
	return 0;
};

void() CSQC_Shutdown = {
};

// CSQC_Input_Frame (без предикции): движок (ezquake) вызывает перед отправкой
// каждого usercmd и заполняет input_* (timelength/angles/movevalues/buttons/
// impulse) — модуль может читать кнопки/импульсы и (позже, для меню) менять
// их. Сейчас — только канарейка под csqc_dbg (экран 3); usercmd не изменяем.
void() CSQC_Input_Frame = {
	if (DbgAt(3) == 0)		// канарейка отображается на экране 3
		return;
	g_if_btns = input_buttons;
	g_if_imp = input_impulse;
	g_if_cnt = g_if_cnt + 1;
};

// C6.1: имя эффекта каталога по индексу (для HUD-строки fx#). Полный набор
// эффектов ezquake CSQC (реестр docs/plans/ezquake_csqc_client_roadmap.md §C6): звук/
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

void CSQC_Stub_Check() {
	local float okf, oks, okv, bad;
	local string stmp;
	local vector vtmp;
	okf = 0;
	oks = 0;
	okv = 0;
	bad = 0;
	// L2-заглушки (тип-correct no-op): вызов всех 141 и проверка возврата.

	if (fopen("",0,0) != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",110,"fopen")); bad = bad + 1; } else okf = okf + 1;
	fclose(0);
	stmp = fgets(0);
	if (stmp != "") { dprint(sprintf("STUB bad #%d %s !=empty\n",112,"fgets")); bad = bad + 1; } else oks = oks + 1;
	fputs(0,"","","","","","","");
	if (getmodelindex("",0) != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",200,"getmodelindex")); bad = bad + 1; } else okf = okf + 1;
	// v6 no-op (ADR 0020): multiprogs/вариант — нереализуемо в классике; прототипы отключены в csdefs.qc.
	//if (externcall(0,"") != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",201,"externcall")); bad = bad + 1; } else okf = okf + 1;
	if (addprogs("") != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",202,"addprogs")); bad = bad + 1; } else okf = okf + 1;
	//if (externvalue(0,"") != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",203,"externvalue")); bad = bad + 1; } else okf = okf + 1;
	//externset(0,0,"");
	if (externrefcall() != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",205,"externrefcall")); bad = bad + 1; } else okf = okf + 1;
	if (instr("","") != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",206,"instr")); bad = bad + 1; } else okf = okf + 1;
	openportal(world,0);
	fork();
	abort(0);
	sleep();
	particle2('0 0 0','0 0 0','0 0 0',0,0,0);
	particle3('0 0 0','0 0 0',0,0,0);
	particle4('0 0 0',0,0,0,0);
	te_lightningblood('0 0 0');
	stmp = strconv(0,0,0,"");
	if (stmp != "") { dprint(sprintf("STUB bad #%d %s !=empty\n",224,"strconv")); bad = bad + 1; } else oks = oks + 1;
	if (isbackbuffered(world) != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",234,"isbackbuffered")); bad = bad + 1; } else okf = okf + 1;
	rotatevectorsbyangle('0 0 0');
	rotatevectorsbyvectors('0 0 0','0 0 0','0 0 0');
	if (skinforname(0,"") != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",237,"skinforname")); bad = bad + 1; } else okf = okf + 1;
	if (shaderforname("","") != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",238,"shaderforname")); bad = bad + 1; } else okf = okf + 1;
	te_bloodqw('0 0 0',0);
	if (sendpacket() != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",242,"sendpacket")); bad = bad + 1; } else okf = okf + 1;
	vtmp = rotatevectorsbytag(world,0);
	if (vtmp_x != 0 || vtmp_y != 0 || vtmp_z != 0) { dprint(sprintf("STUB bad #%d %s !=0v\n",244,"rotatevectorsbytag")); bad = bad + 1; } else okv = okv + 1;
	if (skel_create(0,0) != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",263,"skel_create")); bad = bad + 1; } else okf = okf + 1;
	if (skel_build(0,world,0,0,0,0,0) != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",264,"skel_build")); bad = bad + 1; } else okf = okf + 1;
	if (skel_get_numbones(0) != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",265,"skel_get_numbones")); bad = bad + 1; } else okf = okf + 1;
	stmp = skel_get_bonename(0,0);
	if (stmp != "") { dprint(sprintf("STUB bad #%d %s !=empty\n",266,"skel_get_bonename")); bad = bad + 1; } else oks = oks + 1;
	if (skel_get_boneparent(0,0) != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",267,"skel_get_boneparent")); bad = bad + 1; } else okf = okf + 1;
	if (skel_find_bone(0,"") != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",268,"skel_find_bone")); bad = bad + 1; } else okf = okf + 1;
	vtmp = skel_get_bonerel(0,0);
	if (vtmp_x != 0 || vtmp_y != 0 || vtmp_z != 0) { dprint(sprintf("STUB bad #%d %s !=0v\n",269,"skel_get_bonerel")); bad = bad + 1; } else okv = okv + 1;
	vtmp = skel_get_boneabs(0,0);
	if (vtmp_x != 0 || vtmp_y != 0 || vtmp_z != 0) { dprint(sprintf("STUB bad #%d %s !=0v\n",270,"skel_get_boneabs")); bad = bad + 1; } else okv = okv + 1;
	skel_set_bone(0,0,'0 0 0','0 0 0','0 0 0','0 0 0');
	skel_premul_bone(0,0,'0 0 0','0 0 0','0 0 0','0 0 0');
	skel_premul_bones(0,0,0,'0 0 0','0 0 0','0 0 0','0 0 0');
	skel_copybones(0,0,0,0);
	skel_delete(0);
	if (frameforname(0,"") != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",276,"frameforname")); bad = bad + 1; } else okf = okf + 1;
	if (frameduration(0,0) != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",277,"frameduration")); bad = bad + 1; } else okf = okf + 1;
	if (skel_ragupdate(world,"",0) != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",281,"skel_ragupdate")); bad = bad + 1; } else okf = okf + 1;
	//if (skel_mmap(0) != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",282,"skel_mmap")); bad = bad + 1; } else okf = okf + 1;	// v6 no-op (ADR 0020)
	skel_set_bone_world(world,0,'0 0 0','0 0 0','0 0 0','0 0 0');
	stmp = frametoname(0,0);
	if (stmp != "") { dprint(sprintf("STUB bad #%d %s !=empty\n",284,"frametoname")); bad = bad + 1; } else oks = oks + 1;
	stmp = skintoname(0,0);
	if (stmp != "") { dprint(sprintf("STUB bad #%d %s !=empty\n",285,"skintoname")); bad = bad + 1; } else oks = oks + 1;
	if (resourcestatus(0,0,"") != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",286,"resourcestatus")); bad = bad + 1; } else okf = okf + 1;
	if (hash_createtab(0,0) != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",287,"hash_createtab")); bad = bad + 1; } else okf = okf + 1;
	hash_destroytab(0);
	hash_add(0,"",0,0);
	if (hash_get(0,"",0,0,0) != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",290,"hash_get")); bad = bad + 1; } else okf = okf + 1;
	if (hash_delete(0,"") != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",291,"hash_delete")); bad = bad + 1; } else okf = okf + 1;
	stmp = hash_getkey(0,0);
	if (stmp != "") { dprint(sprintf("STUB bad #%d %s !=empty\n",292,"hash_getkey")); bad = bad + 1; } else oks = oks + 1;
	hash_getcb();
	addentity(world);
	R_BeginPolygon("",0);
	R_PolygonVertex('0 0 0','0 0 0','0 0 0',0);
	R_EndPolygon();
	vtmp = unproject('0 0 0');
	if (vtmp_x != 0 || vtmp_y != 0 || vtmp_z != 0) { dprint(sprintf("STUB bad #%d %s !=0v\n",310,"unproject")); bad = bad + 1; } else okv = okv + 1;
	vtmp = project('0 0 0');
	if (vtmp_x != 0 || vtmp_y != 0 || vtmp_z != 0) { dprint(sprintf("STUB bad #%d %s !=0v\n",311,"project")); bad = bad + 1; } else okv = okv + 1;
	setmodelindex(world,0);
	stmp = modelnameforindex(0);
	if (stmp != "") { dprint(sprintf("STUB bad #%d %s !=empty\n",334,"modelnameforindex")); bad = bad + 1; } else oks = oks + 1;
	if (findfont("") != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",356,"findfont")); bad = bad + 1; } else okf = okf + 1;
	if (loadfont("","","",0,0,0) != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",357,"loadfont")); bad = bad + 1; } else okf = okf + 1;
	stmp = particleeffectquery(0,0);
	if (stmp != "") { dprint(sprintf("STUB bad #%d %s !=empty\n",374,"particleeffectquery")); bad = bad + 1; } else oks = oks + 1;
	// v6 no-op (ADR 0020): FTE_MEMALLOC — pointer-модель нереализуема в классике; прототипы отключены в csdefs.qc.
	//if (memalloc(0) != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",384,"memalloc")); bad = bad + 1; } else okf = okf + 1;
	//memfree(0);
	//memcpy(0,0,0);
	//memfill8(0,0,0);
	//if (memgetval(0,0) != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",388,"memgetval")); bad = bad + 1; } else okf = okf + 1;
	//memsetval(0,0,0);
	//if (memptradd(0,0) != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",390,"memptradd")); bad = bad + 1; } else okf = okf + 1;
	copyentity(world,world);
	if (findchain(classname,"") != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",402,"findchain")); bad = bad + 1; } else okf = okf + 1;
	if (findchainfloat(solid,0) != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",403,"findchainfloat")); bad = bad + 1; } else okf = okf + 1;
	effect('0 0 0',"",0,0,0);
	te_teleport('0 0 0');
	vectorvectors('0 0 0');
	te_plasmaburn('0 0 0');
	setattachment(world,world,"");
	if (search_begin("",0,0) != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",444,"search_begin")); bad = bad + 1; } else okf = okf + 1;
	search_end(0);
	if (search_getsize(0) != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",446,"search_getsize")); bad = bad + 1; } else okf = okf + 1;
	stmp = search_getfilename(0,0);
	if (stmp != "") { dprint(sprintf("STUB bad #%d %s !=empty\n",447,"search_getfilename")); bad = bad + 1; } else oks = oks + 1;
	if (findflags(world,solid,0) != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",449,"findflags")); bad = bad + 1; } else okf = okf + 1;
	if (findchainflags(solid,0) != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",450,"findchainflags")); bad = bad + 1; } else okf = okf + 1;
	if (gettagindex(world,"") != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",451,"gettagindex")); bad = bad + 1; } else okf = okf + 1;
	vtmp = gettaginfo(world,0);
	if (vtmp_x != 0 || vtmp_y != 0 || vtmp_z != 0) { dprint(sprintf("STUB bad #%d %s !=0v\n",452,"gettaginfo")); bad = bad + 1; } else okv = okv + 1;
	te_flamejet();
	if (strlennocol("") != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",476,"strlennocol")); bad = bad + 1; } else okf = okf + 1;
	stmp = strdecolorize("");
	if (stmp != "") { dprint(sprintf("STUB bad #%d %s !=empty\n",477,"strdecolorize")); bad = bad + 1; } else oks = oks + 1;
	if (gecko_create("") != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",487,"gecko_create")); bad = bad + 1; } else okf = okf + 1;
	gecko_destroy("");
	gecko_navigate("","");
	if (gecko_keyevent("",0,0) != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",490,"gecko_keyevent")); bad = bad + 1; } else okf = okf + 1;
	gecko_mousemove("",0,0);
	gecko_resize("",0,0);
	vtmp = gecko_get_texture_extent("");
	if (vtmp_x != 0 || vtmp_y != 0 || vtmp_z != 0) { dprint(sprintf("STUB bad #%d %s !=0v\n",493,"gecko_get_texture_extent")); bad = bad + 1; } else okv = okv + 1;
	boxparticles(0,world,'0 0 0','0 0 0','0 0 0','0 0 0',0,0);
	stmp = whichpack("",0);
	if (stmp != "") { dprint(sprintf("STUB bad #%d %s !=empty\n",503,"whichpack")); bad = bad + 1; } else oks = oks + 1;
	stmp = uri_escape("");
	if (stmp != "") { dprint(sprintf("STUB bad #%d %s !=empty\n",510,"uri_escape")); bad = bad + 1; } else oks = oks + 1;
	stmp = uri_unescape("");
	if (stmp != "") { dprint(sprintf("STUB bad #%d %s !=empty\n",511,"uri_unescape")); bad = bad + 1; } else oks = oks + 1;
	if (uri_get("",0,"","") != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",513,"uri_get")); bad = bad + 1; } else okf = okf + 1;
	buf_cvarlist(0);
	loadfromdata("");
	loadfromfile("");
	if (buf_loadfile("",0) != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",535,"buf_loadfile")); bad = bad + 1; } else okf = okf + 1;
	if (buf_writefile(0,0,0,0) != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",536,"buf_writefile")); bad = bad + 1; } else okf = okf + 1;
	if (bufstr_find() != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",537,"bufstr_find")); bad = bad + 1; } else okf = okf + 1;
	physics_enable(world,0);
	physics_addforce(world,'0 0 0','0 0 0');
	physics_addtorque(world,'0 0 0');
	writetofile(0,world);
	if (gethostcachevalue(0) != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",611,"gethostcachevalue")); bad = bad + 1; } else okf = okf + 1;
	stmp = gethostcachestring(0,0);
	if (stmp != "") { dprint(sprintf("STUB bad #%d %s !=empty\n",612,"gethostcachestring")); bad = bad + 1; } else oks = oks + 1;
	parseentitydata(world,"");
	resethostcachemasks();
	sethostcachemaskstring(0,0,"",0);
	sethostcachemasknumber(0,0,0,0);
	resorthostcache();
	sethostcachesort(0,0);
	refreshhostcache();
	if (gethostcachenumber(0,0) != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",621,"gethostcachenumber")); bad = bad + 1; } else okf = okf + 1;
	if (gethostcacheindexforkey("") != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",622,"gethostcacheindexforkey")); bad = bad + 1; } else okf = okf + 1;
	addwantedhostcachekey("");
	stmp = getextresponse();
	if (stmp != "") { dprint(sprintf("STUB bad #%d %s !=empty\n",624,"getextresponse")); bad = bad + 1; } else oks = oks + 1;
	stmp = netaddress_resolve("",0);
	if (stmp != "") { dprint(sprintf("STUB bad #%d %s !=empty\n",625,"netaddress_resolve")); bad = bad + 1; } else oks = oks + 1;
	stmp = getgamedirinfo();
	if (stmp != "") { dprint(sprintf("STUB bad #%d %s !=empty\n",626,"getgamedirinfo")); bad = bad + 1; } else oks = oks + 1;
	if (CL_RotateMoves() != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",638,"CL_RotateMoves")); bad = bad + 1; } else okf = okf + 1;
	stmp = digest_hex("","");
	if (stmp != "") { dprint(sprintf("STUB bad #%d %s !=empty\n",639,"digest_hex")); bad = bad + 1; } else oks = oks + 1;
	if (V_CalcRefdef() != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",640,"V_CalcRefdef")); bad = bad + 1; } else okf = okf + 1;
	fcopy();
	frename();
	fremove();
	if (fexists() != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",653,"fexists")); bad = bad + 1; } else okf = okf + 1;
	rmtree();
	if (controller_query() != 0) { dprint(sprintf("STUB bad #%d %s !=0\n",740,"controller_query")); bad = bad + 1; } else okf = okf + 1;
	controller_rumble();
	controller_rumbletriggers();
	dprint(sprintf("STUBCHK n141 okf %g oks %g okv %g bad %g\n", okf, oks, okv, bad));
}

// Рефлексия: индекс поля модуля по имени (entityfieldname), -1 если нет.
float CSQC_FieldIdx(string nm) {
	local float i;
	for (i = 0; i < numentityfields(); i = i + 1)
		if (entityfieldname(i) == nm)
			return i;
	return -1;
}

// Агрегат несовпадений стат-канарейки (csqc_dbg 6): число бит/значений, отличных
// от ожидаемых. Биты INTEGER-стата 55 = 0x12345678 (nib 8..1): 8 nibble-групп
// покрывают все 32 бита, count<=24 -> значения точны в float32. Плюс фикс-значения
// типов: float 51=12345.5, vector 52=1/54=3, entity 37=0. (string 53 — динамич.
// netname, только информативно.) bad==0 <=> все биты доставлены корректно.
float CSQC_StatWire_Bad() {
	local float b;
	b = 0;
	b = b + (getstatf(55, 0, 4) != 8);
	b = b + (getstatf(55, 4, 4) != 7);
	b = b + (getstatf(55, 8, 4) != 6);
	b = b + (getstatf(55, 12, 4) != 5);
	b = b + (getstatf(55, 16, 4) != 4);
	b = b + (getstatf(55, 20, 4) != 3);
	b = b + (getstatf(55, 24, 4) != 2);
	b = b + (getstatf(55, 28, 4) != 1);
	b = b + (getstatf(55, 0, 8) != 120);
	b = b + (getstatf(55, 8, 8) != 86);
	b = b + (getstatf(55, 16, 8) != 52);
	b = b + (getstatf(55, 24, 8) != 18);
	b = b + (getstatf(55, 0, 16) != 22136);
	b = b + (getstatf(55, 16, 16) != 4660);
	b = b + (getstatf(55, 0, 24) != 3430008);
	b = b + (getstatf(55, 8, 24) != 1193046);
	b = b + (getstatf(51) != 12345.5);
	b = b + (getstatf(52) != 1);
	b = b + (getstatf(54) != 3);
	b = b + (getstati(37) != 0);
	return b;
}

// C5-E: предикция локального игрока (вариант «а»). CSQC_PlayerPred — колбэк
// deltalisten (#371): движок отдаёт авторитетное (no-lerp) состояние сущности
// игрока (self/.entnum, PARM0=isnew); мост реализован в ezquake (csqc_client).
// Модуль сохраняет базу на servercommandframe и проигрывает input_*-кадры
// (getinputstate + runstandardplayerphysics) до clientcommandframe — как
// FTE csqctest/prediction.qc. Ошибка = |прошлый прогноз - пере-прогноз|;
// >64 — телепорт-snap.
void Pred_Run(entity ent, float endframe) {
	if (servercommandframe >= pp_seq + 63) {
		// троттлинг (как FTE): не отставать больше кольца истории кадров
		pp_frame = servercommandframe - 63;
		return;
	}
	ent.origin = pp_org;
	ent.velocity = pp_vel;
	ent.pmove_flags = pp_pmflags;
	pp_frame = pp_seq + 1;
	while (pp_frame <= endframe) {
		if (!getinputstate(pp_frame))
			break;
		runstandardplayerphysics(ent);
		pp_frame = pp_frame + 1;
	}
}

void Pred_UpdateLocal() {
	if (g_pp_valid != 1 || g_pp_ent == world)
		return;
	Pred_Run(g_pp_ent, clientcommandframe);
	g_e_predorg = g_pp_ent.origin;
}

float(float isnew) CSQC_BackpackPred = {
	// C4 Этап 3: дельта-сущность backpack (packet entity). При csqc_delta!=0 модуль
	// рисует её сам (setmodel + drawmask=MASK_DELTA) и возвращает 1 → движок не
	// рисует дубль (FTE-семантика: возврат delta-callback !=0). Иначе — listen-only.
	if (cvar("csqc_delta") == 0) {
		self.drawmask = 0;
		return 0;
	}
	setmodel(self, "progs/backpack.mdl");
	self.drawmask = MASK_DELTA;
	g_c4_owned_bp = g_c4_owned_bp + 1;
	return 1;
};

float(float isnew) CSQC_PlayerPred = {
	local vector o;
	if (self.entnum != player_localentnum) {
		g_e_dents = g_e_dents + 1;	// не-локальные (delta) сущности
		// C4 Этап 3 (csqc_delta!=0): удалённого игрока рисует CSQC (setmodel +
		// drawmask=MASK_DELTA), возврат 1 → движок не рисует дубль. Иначе listen-only:
		// drawmask=0 + return 0 — движок рисует игрока один раз (нет дублей/«призраков»).
		if (cvar("csqc_delta") != 0) {
			setmodel(self, "progs/player.mdl");
			self.drawmask = MASK_DELTA;
			g_c4_owned_pl = g_c4_owned_pl + 1;
			return 1;
		}
		self.drawmask = 0;
		return 0;
	}
	if (g_pp_valid != 1 || isnew == 1) {
		g_pp_ent = self;
		g_pp_valid = 1;
		g_e_predorg = self.origin;
	}
	o = g_e_predorg;			// прошлый прогноз
	pp_org = self.origin;		// авторитетное состояние (мост движка)
	pp_vel = self.velocity;
	pp_pmflags = self.pmove_flags;
	pp_seq = servercommandframe;
	Pred_Run(self, clientcommandframe);
	g_e_err = vlen(o - self.origin);	// коррекция при ресинке
	if (g_e_err > 64) {
		g_e_tele = g_e_tele + 1;
		g_e_err = 0;			// телепорт — мгновенный snap
	}
	g_e_predorg = self.origin;
	g_e_upd = g_e_upd + 1;
	self.drawmask = 0;	// listen-only: рендер игрока — движковый (см. выше)
	return 0;
};

// C5-B: канарейка-механика #347 (csqc_dbg 7). Арена-энт ставится в view origin
// игрока (стоящий игрок -> энт сразу на его полу), синтетический вход input_*
// (forward=300, msec=50, angles = view angles игрока), серия #347:
//   land  — FL_ONGROUND (solid-набор собран, B2);
//   sink  — z стабилен на земле (не проваливается сквозь мир);
//   move  — скорость/смещение вперёд (input_* реально читаются, B1);
//   stop  — после обнуления входа скорость затухает (friction).
//   fall  — информативно (z-дельта при оседании).
// Запускать стоя на полу (в воздухе/у стены land/move могут не сойтись).
// Возврат/глобал g_c5b_bad — число провалов (0 = ok). input_* не восстанавливаем:
// CSQC_Input_Frame перезаписывает их из cmd перед каждым кадром.
float CSQC_C5B_Run() {
	local entity e;
	local vector o, o0;
	local float i, bad, z0, zmin, zland, zidle, sp_move, sp_stop, eslot;

	bad = 0;
	e = spawn();
	if (e == world)
		return 1;
	// #347 принимает entity. Квирк C5-B (передача *локальной* entity приходила как 0)
	// разобран: причина — float-чтение entity в клиентских builtins движка, а не VM.
	// Теперь entity — единые int-биты (ezq #347/#428-431/#459), вызов идёт напрямую с `e`.
	// eslot нужен только для round-trip канарейки #459/#512 ниже.
	eslot = num_for_edict(e);
	// round-trip: edict_num(eslot) обязан вернуть ту же entity (int-биты), тогда
	// num_for_edict снова даст eslot. На старом float-соглашении не сходится.
	g_c5b_rt = (num_for_edict(edict_num(eslot)) == eslot) ? 1 : 0;
	if (g_c5b_rt != 1)
		bad = bad + 1;

	// Якорь — view origin игрока (без подъёма): у стоящего игрока энт сразу
	// садится на его пол (проверка solid/world), не зависит от ямы под +64.
	o = getproperty(VF_ORIGIN);
	e.origin = o;
	e.velocity = '0 0 0';
	e.movetype = MOVETYPE_WALK;
	e.mins = '-16 -16 -24';
	e.maxs = '16 16 32';
	e.flags = 0;
	e.pmove_flags = 0;

	input_timelength = 0.05;
	input_angles = getproperty(VF_ANGLES);	// вперёд — куда смотрит игрок
	input_movevalues = '300 0 0';
	input_buttons = 0;
	input_impulse = 0;

	// Фаза 1: осесть на пол (8 шагов x 50 мс) — ожидание FL_ONGROUND.
	z0 = e.origin_z;
	zmin = z0;
	for (i = 0; i < 8; i = i + 1) {
		runstandardplayerphysics(e);
		if (e.origin_z < zmin)
			zmin = e.origin_z;
	}
	g_c5b_fall = z0 - zmin;	// информативно
	g_c5b_land = (e.flags & FL_ONGROUND) ? 1 : 0;
	if (g_c5b_land != 1)
		bad = bad + 1;

	// Фаза 2: простой на земле (5 шагов) — не проваливаемся (мир в solid-наборе).
	zland = e.origin_z;
	for (i = 0; i < 5; i = i + 1)
		runstandardplayerphysics(e);
	zidle = e.origin_z;
	g_c5b_sink = fabs(zidle - zland);
	if (g_c5b_sink > 4)
		bad = bad + 1;

	// Фаза 3: движение вперёд (10 шагов) — input_* читаются (скорость/смещение).
	o0 = e.origin;
	for (i = 0; i < 10; i = i + 1)
		runstandardplayerphysics(e);
	o = e.origin;
	g_c5b_move = sqrt((o_x - o0_x) * (o_x - o0_x) + (o_y - o0_y) * (o_y - o0_y));
	sp_move = sqrt(e.velocity_x * e.velocity_x + e.velocity_y * e.velocity_y);
	if (g_c5b_move <= 1 && sp_move <= 10)
		bad = bad + 1;

	// Фаза 4: остановка (вход 0, 20 шагов) — скорость затухает.
	input_movevalues = '0 0 0';
	for (i = 0; i < 20; i = i + 1)
		runstandardplayerphysics(e);
	sp_stop = sqrt(e.velocity_x * e.velocity_x + e.velocity_y * e.velocity_y);
	g_c5b_stop = sp_stop;
	if (sp_stop >= sp_move)
		bad = bad + 1;

	remove(e);
	g_c5b_bad = bad;
	dprint(sprintf("C5B347 bad %g rt %g fall %g land %g sink %g move %g stop %g\n",
		bad, g_c5b_rt, g_c5b_fall, g_c5b_land, g_c5b_sink, g_c5b_move, g_c5b_stop));
	return bad;
}

// Дополнительные HUD-элементы CSQC-теста (вызывается из Hud_Draw в hud.qc):
// 2D-маркеры флагов (entity-канал), эхо sendevent, счёт команд (pointerstat 40..43)
// + debug-экраны (csqc_dbg 1..7; стат-канарейки — экран 6; C5-B #347 — экран 7).
void Hud_DrawCSQC() {
	local float i, yy, tw;
	local string s;
	local float k;
	local string ks;
	local vector mp;
	local float D;
	local entity t5e;	// T5: временный spawn для etos/num_for_edict/wasfreed
	local entity vme;	// VM-остаток: spawn для findfloat-канарейки
	local float cx, cy;	// 2D-доп: правый верхний угол от размера экрана
	local float bad;	// экран 6: агрегат стат-канарейки (CSQC_StatWire_Bad)
	local float le;		// экран 10: player_localentnum для #504 getentity

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
	// 2 — текст «до ie»; 3 — остальное (по группам); 4 — сводные L2-смоки
	// (T1..T12/2D); 5 — Entity-рефлексия (#496-500); 6 — CSQC stat wire
	// 78/79 + бит-целостность #331/#332 (int/float/string/vector/entity);
	// 7 — C5-B канарейка-механика #347 runstandardplayerphysics;
	// 8 — C5-E предикция локального игрока (deltalisten #371 + #345/#347);
	// 9 — C5-E Ф1 (view_angles / project-unproject / listener);
	// 10 — C5-E Ф2 (#504 getentity); 11 — Ф3 renderscene (mod-отрисовка флага);
	// 12 — C4 Этап 1 (#301/#302 predraw + renderflags); 13 — C4 Этап 2 (#301 MASK_VIEWMODEL);
	// 14 — C4 Этап 3 (MASK_DELTA status; тумблер csqc_delta).
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
	// C4 Э3: при включении csqc_delta — precache + регистрация backpack-слушателя
	// (deltalisten пер-модельный; см. CSQC_BackpackPred). Экран 14 — статус.
	if (cvar("csqc_delta") != 0 && g_c4_bp_reg == 0) {
		precache_model("progs/backpack.mdl");
		deltalisten("progs/backpack.mdl", CSQC_BackpackPred, RSES_NOLERP | RSES_NOROTATE);
		g_c4_bp_reg = 1;
	}
	// Экран 10: одноразовый console-dprint — сброс вне экрана (печать при входе).
	if (D != 10)
		g_ge_done = 0;

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
			crc16(0, "ABC"), crc16(1, "ABC"), gettimef()));
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
		// 2D-доп: precache→iscachedpic 1→размер→freepic (no-op, как FTE)→isc остаётся 1.
		precache_pic("gfx/ttl_main");
		g_2d_isc0 = iscachedpic("gfx/ttl_main");
		mp = drawgetimagesize("gfx/ttl_main");
		g_2d_isz_x = mp_x;
		g_2d_isz_y = mp_y;
		freepic("gfx/ttl_main");
		g_2d_isc1 = iscachedpic("gfx/ttl_main");
		// #329 drawrotpic_dp — smoke (no-op, не краш).
		drawrotpic_dp([0, 0, 0], "gfx/ttl_main", [96, 24, 0], [0, 0, 0], 0, [1, 1, 1], 1);
		dprint(sprintf("2D isc %g/%g sz %gx%g\n",
			g_2d_isc0, g_2d_isc1, g_2d_isz_x, g_2d_isz_y));
		// VM-остаток: findfloat по .solid == SOLID_BBOX (находит свой спавн).
		vme = spawn();
		if (vme != world) {
			vme.solid = SOLID_BBOX;
			t5e = findfloat(world, solid, SOLID_BBOX);
			g_vm_fo = (t5e == vme) ? num_for_edict(t5e) : -1;
			remove(vme);
		} else {
			g_vm_fo = -2;
		}
		dprint(sprintf("T9 findfloat slot %g\n", g_vm_fo));
		// Звук: pointsound/SetListener разово при включении (в стенде -nosound,
		// проверка — не-краш); getsoundtime/soundlength — no-op (-1/0).
		pointsound(getproperty(11), "weapons/railgun.wav", 1, 1);
		SetListener(getproperty(11), [0, 0, 0], [0, 0, 0], [0, 0, 0]);
		dprint("T10 sound pointsound/SetListener ok\n");
		// Свет/decals/скины: no-op (нет аналогов) — вызовы без краша.
		adddecal("progs/none.mdl", getproperty(11), [0, 0, 1], [1, 0, 0], [1, 1, 1], 1);
		setcustomskin(world, "");
		dynamiclight_get(0, -1);
		dynamiclight_set(0, 0, 0);
		dprint("T11 light/decal/skin noop ok\n");
		// Интроспекция/кон: checkcommand (status=1, developer=cvar=3), argescape,
		// isfunction (Hud_DrawCSQC=1, нет=0); con_* — no-op (не вызываются).
		dprint(sprintf("T12 cc%g/%g esc[%s] is%g/%g\n",
			checkcommand("status"), checkcommand("developer"),
			argescape("a b c"), isfunction("Hud_DrawCSQC"), isfunction("zzz_none")));
		dprint(sprintf("T7 tf[%s]/[%s]\n",
			strftime(1, "%H:%M:%S"), strftime(0, "%H:%M:%S")));
		dprint(sprintf("T7 tc %g a1[%s] s0 %g/%g s1 %g/%g\n",
			tokenize_console("aa bb cc"), argv(1),
			argv_start_index(0), argv_end_index(0),
			argv_start_index(1), argv_end_index(1)));
		dprint(sprintf("T7 sep %g s0 %g/%g s2 %g/%g\n",
			tokenizebyseparator("a;b;;c", ";"),
			argv_start_index(0), argv_end_index(0),
			argv_start_index(2), argv_end_index(2)));
		k = stringtokeynum_menu("ESCAPE");
		ks = keynumtostring_omgwtf(k);
		dprint(sprintf("T8 esc%s(%d) bind[%s] keys[%s]\n",
			(ks == "ESCAPE") ? "ESCAPE" : "?", k,
			getkeybind(k), findkeysforcommand(getkeybind(k))));
		mp = getresolution(0);
		tw = mp_x;
		k = mp_y;
		mp = getbindmaps();
		dprint(sprintf("T8 res %gx%g mt%g bm %g %g %g\n",
			tw, k, getmousetarget(), mp_x, mp_y, mp_z));
		// cprint — центр-экран (видимая проверка; как FTE).
		cprint("CSQC T5 cprint");
	}

	// Экран 5 (Entity-рефлексия #496-500): при включении — вычисление + консоль.
	if (D == 5 && g_dbg_prev != 5) {
		CSQC_Stub_Check();
		g_rf_n = numentityfields();
		g_rf_is = CSQC_FieldIdx("solid");
		g_rf_ic = CSQC_FieldIdx("classname");
		g_rf_io = CSQC_FieldIdx("origin");
		vme = spawn();
		if (vme != world) {
			vme.solid = SOLID_BBOX;
			setorigin(vme, '1 2 3');
			if (g_rf_is >= 0)
				g_rf_v = stof(getentityfieldstring(g_rf_is, vme));
			if (g_rf_ic >= 0) {
				putentityfieldstring(g_rf_ic, vme, "rf_set");
				g_rf_cls = (getentityfieldstring(g_rf_ic, vme) == "rf_set") ? 1 : 0;
			}
			remove(vme);
		}
		dprint(sprintf("T13 rf n%g i%g/%g/%g v%g cls%g\n",
			g_rf_n, g_rf_is, g_rf_ic, g_rf_io, g_rf_v, g_rf_cls));
	}

	// Экран 6 (CSQC stat wire 78/79): сводка в консоль (qconsole.log).
	if (D == 6 && g_dbg_prev != 6) {
		bad = CSQC_StatWire_Bad();
		dprint(sprintf("SW78/79 stat55 0x12345678 bad %g\n", bad));
		dprint(sprintf("SW f51 %g s53 %s v %g/%g e37 %g\n",
			getstatf(51), getstats(53), getstatf(52), getstatf(54), getstati(37)));
	}

	// Экран 7 (C5-B #347): одноразовый прогон канарейки-механики + консоль.
	if (D == 7 && g_dbg_prev != 7)
		CSQC_C5B_Run();
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
		// C5-A: окно предикции (движок ezq csqc_client PatchFrames). Ожидание live:
		// sc < cc (есть необработанные кадры), iseq — последняя отправленная,
		// #345(input_sequence) == 1; при движении cc растёт.
		g_c5_sc = servercommandframe;
		g_c5_cc = clientcommandframe;
		g_c5_iseq = input_sequence;
		g_c5_345 = getinputstate(input_sequence);
		drawstring([60, yy, 0],
			sprintf("c5 sc%g cc%g is%g 345:%g", g_c5_sc, g_c5_cc, g_c5_iseq, g_c5_345),
			[12, 12], (g_c5_sc < g_c5_cc && g_c5_345 == 1) ? '0 1 0' : '1 0 0', 1, 0);
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
	// bs(1,-1)=0; crc16(0,"ABC")=62728 crc16(1,"ABC")=20810; gettimef() растёт.
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
				crc16(0, "ABC"), crc16(1, "ABC"), gettimef()),
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
		yy = yy + 18;
		drawstring([60, yy, 0],
			sprintf("2d isc %g/%g sz %gx%g",
				g_2d_isc0, g_2d_isc1, g_2d_isz_x, g_2d_isz_y),
			[12, 12], '1 1 1', 1, 0);
		yy = yy + 18;
		// ST: strftime (локальное/GMT) + tokenize_console + tokenizebyseparator.
		drawstring([60, yy, 0],
			sprintf("t7 l[%s] g[%s]",
				strftime(1, "%H:%M:%S"), strftime(0, "%H:%M:%S")),
			[12, 12], '1 1 1', 1, 0);
		yy = yy + 18;
		tw = tokenize_console("aa bb cc");
		drawstring([60, yy, 0],
			sprintf("t7 tc%g a1[%s] s0 %g/%g s1 %g/%g",
				tw, argv(1), argv_start_index(0), argv_end_index(0),
				argv_start_index(1), argv_end_index(1)),
			[12, 12], '1 1 1', 1, 0);
		yy = yy + 18;
		tw = tokenizebyseparator("a;b;;c", ";");
		drawstring([60, yy, 0],
			sprintf("t7 sep%g s0 %g/%g s2 %g/%g",
				tw, argv_start_index(0), argv_end_index(0),
				argv_start_index(2), argv_end_index(2)),
			[12, 12], '1 1 1', 1, 0);
		yy = yy + 18;
		// Ввод/меню: keynum round-trip, getkeybind/findkeysforcommand, res/mt/bm.
		k = stringtokeynum_menu("ESCAPE");
		ks = keynumtostring_omgwtf(k);
		drawstring([60, yy, 0],
			sprintf("t8 esc%s(%d) bind[%s] keys[%s]",
				(ks == "ESCAPE") ? "ESCAPE" : "?", k,
				getkeybind(k), findkeysforcommand(getkeybind(k))),
			[12, 12], '1 1 1', 1, 0);
		yy = yy + 18;
		mp = getresolution(0);
		tw = mp_x;
		k = mp_y;
		mp = getbindmaps();
		drawstring([60, yy, 0],
			sprintf("t8 res %gx%g mt%g bm %g %g %g",
				tw, k, getmousetarget(), mp_x, mp_y, mp_z),
			[12, 12], '1 1 1', 1, 0);
		yy = yy + 18;
		drawstring([60, yy, 0],
			sprintf("t9 findfloat slot %g", g_vm_fo),
			[12, 12], '1 1 1', 1, 0);
		yy = yy + 18;
		drawstring([60, yy, 0],
			sprintf("t10 gst%g sl%g",
				getsoundtime(world, 0), soundlength("weapons/railgun.wav")),
			[12, 12], '1 1 1', 1, 0);
		yy = yy + 18;
		drawstring([60, yy, 0], "t11 light/decal/skin noop", [12, 12], '1 1 1', 1, 0);
		yy = yy + 18;
		drawstring([60, yy, 0],
			sprintf("t12 cc%g/%g esc[%s] is%g/%g",
				checkcommand("status"), checkcommand("developer"),
				argescape("a b c"), isfunction("Hud_DrawCSQC"), isfunction("zzz_none")),
			[12, 12], '1 1 1', 1, 0);
		// 2D-доп визуально (правый верхний угол — от размера экрана getproperty
		// 204): cliparea обрезает drawpic (пикча центрирована по clip, видна
		// центральная часть), raw vs colored.
		mp = getproperty(204);
		cx = mp_x - 16 - 90;	// margin 16 + clipw 90
		cy = 16;
		drawsetcliparea(cx, cy, 90, 24);
		drawpic([cx - 35, cy - 8], "gfx/ttl_main", [160, 40], '1 1 1', 1);
		drawresetcliparea();
		drawstring([cx, cy + 30], "&cF00col", [12, 12], '1 1 1', 1, 0);
		drawrawstring([cx, cy + 52], "&cF00raw", [12, 12], '1 1 1', 1);
	}

	// --- экран 5: Entity-рефлексия (#496-500) ---
	if (D == 5) {
		yy = 470;
		drawstring([60, yy, 0],
			sprintf("t13 rf n%g i%g/%g/%g v%g cls%g",
				g_rf_n, g_rf_is, g_rf_ic, g_rf_io, g_rf_v, g_rf_cls),
			[12, 12], '1 1 1', 1, 0);
	}

	// --- экран 6: CSQC stat wire 78/79 + бит-целостность #331/#332 ---
	// stat 55 = 0x12345678 (nib 8..1, byte 120/86/52/18): 8 nibble-групп покрывают
	// все 32 бита; count<=24 -> точны в float32. Шапка green/red по агрегату bad.
	if (D == 6) {
		yy = 470;
		bad = CSQC_StatWire_Bad();
		drawstring([60, yy, 0],
			sprintf("SW78/79 stat55 0x12345678 bad %g", bad),
			[12, 12], (bad == 0) ? '0 1 0' : '1 0 0', 1, 0);
		yy = yy + 18;
		drawstring([60, yy, 0],
			sprintf("nib %d/%d/%d/%d",
				getstatf(55, 0, 4), getstatf(55, 4, 4), getstatf(55, 8, 4), getstatf(55, 12, 4)),
			[12, 12], '1 1 1', 1, 0);
		yy = yy + 18;
		drawstring([60, yy, 0],
			sprintf("nib %d/%d/%d/%d",
				getstatf(55, 16, 4), getstatf(55, 20, 4), getstatf(55, 24, 4), getstatf(55, 28, 4)),
			[12, 12], '1 1 1', 1, 0);
		yy = yy + 18;
		drawstring([60, yy, 0],
			sprintf("b8 %d/%d/%d/%d c16 %d/%d",
				getstatf(55, 0, 8), getstatf(55, 8, 8), getstatf(55, 16, 8), getstatf(55, 24, 8),
				getstatf(55, 0, 16), getstatf(55, 16, 16)),
			[12, 12], '1 1 1', 1, 0);
		yy = yy + 18;
		drawstring([60, yy, 0],
			sprintf("c24 %d/%d", getstatf(55, 0, 24), getstatf(55, 8, 24)),
			[12, 12], '1 1 1', 1, 0);
		yy = yy + 18;
		drawstring([60, yy, 0],
			sprintf("i ps50 %g tk %g g40 %g", getstatf(50), getstatf(40), getstati(40)),
			[12, 12], '1 1 1', 1, 0);
		yy = yy + 18;
		drawstring([60, yy, 0],
			sprintf("f51 %g s53 %s", getstatf(51), getstats(53)),
			[12, 12], '1 1 1', 1, 0);
		yy = yy + 18;
		drawstring([60, yy, 0],
			sprintf("v %g/%g e37 %g i50 %g",
				getstatf(52), getstatf(54), getstati(37), getstati(50)),
			[12, 12], '1 1 1', 1, 0);
	}

	// --- экран 7: C5-B канарейка-механика #347 ---
	// Ожидание: bad 0, rt 1, fall>0, land1, sink~0, move>0, stop<move.
	if (D == 7) {
		yy = 470;
		drawstring([60, yy, 0],
			sprintf("C5B347 bad %g rt %g fall %g land %g",
				g_c5b_bad, g_c5b_rt, g_c5b_fall, g_c5b_land),
			[12, 12], (g_c5b_bad == 0) ? '0 1 0' : '1 0 0', 1, 0);
		yy = yy + 18;
		drawstring([60, yy, 0],
			sprintf("sink %g move %g stop %g",
				g_c5b_sink, g_c5b_move, g_c5b_stop),
			[12, 12], '1 1 1', 1, 0);
	}

	// --- экран 8: C5-E предикция локального игрока ---
	// Ожидание: upd>0 (мост отдаёт состояние), err мала (сходимость), tele —
	// снапы при телепортах; pred-origin двигается при ходьбе.
	if (D == 8) {
		yy = 470;
		drawstring([60, yy, 0],
			sprintf("E pred %g %g %g",
				g_pp_ent.origin_x, g_pp_ent.origin_y, g_pp_ent.origin_z),
			[12, 12], (g_e_upd > 0) ? '0 1 0' : '1 1 0', 1, 0);
		yy = yy + 18;
		drawstring([60, yy, 0],
			sprintf("E upd %g seq %g err %g tele %g",
				g_e_upd, pp_seq, g_e_err, g_e_tele),
			[12, 12], (g_e_upd > 0 && g_e_err < 32) ? '0 1 0' : '1 1 0', 1, 0);
		yy = yy + 18;
		drawstring([60, yy, 0],
			sprintf("E dents %g vext %g %g", g_e_dents, g_pp_ent.velocity_x, g_pp_ent.velocity_z),
			[12, 12], '1 1 1', 1, 0);
	}

	// --- экран 9: C5-E Ф1 (view_angles / project-unproject / listener) ---
	// Ожидание: va ≈ углы вида (ненулевые), rt ≈ 0 (round-trip), proj — экранные
	// координаты, lis 1 (SetListener активен).
	if (D == 9) {
		yy = 470;
		drawstring([60, yy, 0],
			sprintf("F1 va %g %g %g", g_f1_va_x, g_f1_va_y, g_f1_va_z),
			[12, 12], (g_f1_va_x != 0 || g_f1_va_y != 0) ? '0 1 0' : '1 1 0', 1, 0);
		yy = yy + 18;
		drawstring([60, yy, 0],
			sprintf("F1 proj %g %g %g", g_f1_proj_x, g_f1_proj_y, g_f1_proj_z),
			[12, 12], '1 1 1', 1, 0);
		yy = yy + 18;
		drawstring([60, yy, 0],
			sprintf("F1 pred %g %g %g", g_f1_pred_x, g_f1_pred_y, g_f1_pred_z),
			[12, 12], '1 1 1', 1, 0);
		yy = yy + 18;
		drawstring([60, yy, 0],
			sprintf("F1 rt %g lis %g", g_f1_rt, g_f1_lis),
			[12, 12], (g_f1_rt < 1 && g_f1_lis == 1) ? '0 1 0' : '1 1 0', 1, 0);
	}

	// --- экран 10: C5-E Ф2 (#504 getentity) ---
	// Ожидание: bad 0 (act 1, model>0, maxents>0, origin ненулевой, hull z<0/max>0,
	// невалидные номера -> 0); org — интерп. origin игрока; dorg — расхождение с
	// мостом C5-E (lerp vs no-lerp, информативно); col — цвета 0..15.
	if (D == 10) {
		le = player_localentnum;
		g_ge_act = getentity(le, GE_ACTIVE);
		g_ge_model = getentity(le, GE_MODELINDEX);
		g_ge_max = getentity(0, GE_MAXENTS);
		g_ge_org = getentity(le, GE_ORIGIN);
		g_ge_mn = getentity(le, GE_MINS);
		g_ge_mx = getentity(le, GE_MAXS);
		g_ge_pants = getentity(le, GE_PANTSCOLOR);
		g_ge_shirt = getentity(le, GE_SHIRTCOLOR);
		g_ge_inv = getentity(0, GE_ACTIVE);
		g_ge_bad = 0;
		g_ge_bad = g_ge_bad + (g_ge_act != 1);
		g_ge_bad = g_ge_bad + (g_ge_model <= 0);
		g_ge_bad = g_ge_bad + (g_ge_max <= 0);
		g_ge_bad = g_ge_bad + (vlen(g_ge_org) <= 0);
		g_ge_bad = g_ge_bad + (g_ge_mn_z >= 0);
		g_ge_bad = g_ge_bad + (g_ge_mx_z <= 0);
		g_ge_bad = g_ge_bad + (g_ge_inv != 0);
		g_ge_dorg = 0;
		if (g_pp_valid == 1 && g_pp_ent != world)
			g_ge_dorg = vlen(g_ge_org - g_pp_ent.origin);
		if (g_ge_done == 0) {
			g_ge_done = 1;
			dprint(sprintf("GE504 bad %g act %g mdl %g mx %g inv %g\n",
				g_ge_bad, g_ge_act, g_ge_model, g_ge_max, g_ge_inv));
		}
		yy = 470;
		drawstring([60, yy, 0],
			sprintf("GE bad %g act %g mdl %g mx %g",
				g_ge_bad, g_ge_act, g_ge_model, g_ge_max),
			[12, 12], (g_ge_bad == 0) ? '0 1 0' : '1 0 0', 1, 0);
		yy = yy + 18;
		drawstring([60, yy, 0],
			sprintf("GE org %g %g %g", g_ge_org_x, g_ge_org_y, g_ge_org_z),
			[12, 12], '1 1 1', 1, 0);
		yy = yy + 18;
		drawstring([60, yy, 0],
			sprintf("GE min %g %g %g", g_ge_mn_x, g_ge_mn_y, g_ge_mn_z),
			[12, 12], '1 1 1', 1, 0);
		yy = yy + 18;
		drawstring([60, yy, 0],
			sprintf("GE max %g %g %g", g_ge_mx_x, g_ge_mx_y, g_ge_mx_z),
			[12, 12], '1 1 1', 1, 0);
		yy = yy + 18;
		drawstring([60, yy, 0],
			sprintf("GE col p%g s%g inv %g", g_ge_pants, g_ge_shirt, g_ge_inv),
			[12, 12], '1 1 1', 1, 0);
		yy = yy + 18;
		drawstring([60, yy, 0],
			sprintf("GE dorg %g", g_ge_dorg),
			[12, 12], '1 1 1', 1, 0);
	}

	// --- экран 11: Ф3 renderscene / mod-отрисовка ---
	// Ожидание: впереди флаг (progs/tf_flag.mdl, Y+90, z-24), виден на FTE и ezq;
	// midx — #200 getmodelindex (ezq: индекс CSQC-реестра >0; FTE: precache-индекс).
	if (D == 11) {
		yy = 470;
		drawstring([60, yy, 0],
			sprintf("F3 flag mdl, midx %g", g_r_midx),
			[12, 12], (g_r_midx != 0) ? '0 1 0' : '1 1 0', 1, 0);
	}

	// --- экран 12: C4 Этап 1 (#301/#302 predraw + renderflags) ---
	// Ожидание: pred > 0 (predraw вызван движком), skip > 0 (возврат PREDRAW_NEXT
	// учтён), rf = 32 (RF_NOSHADOW); e1 виден, e2 — нет. Зелёный при pred>0 && skip>0.
	if (D == 12) {
		yy = 470;
		drawstring([60, yy, 0],
			sprintf("C4 Э1 pred %g skip %g rf %g", g_c4_pred, g_c4_skip, g_c4_rf),
			[12, 12], (g_c4_pred > 0 && g_c4_skip > 0) ? '0 1 0' : '1 1 0', 1, 0);
	}

	// --- экран 13: C4 Э2 (#301 MASK_VIEWMODEL) ---
	// Ожидание: csqc_vm 1 — вьюмодель (пушка) видна; csqc_vm 0 — скрыта (паритет FTE).
	if (D == 13) {
		yy = 470;
		drawstring([60, yy, 0],
			sprintf("C4 Э2 vm %g (csqc_vm 0/1)", cvar("csqc_vm")),
			[12, 12], (cvar("csqc_vm") != 0) ? '0 1 0' : '1 1 0', 1, 0);
	}

	// --- экран 14: C4 Э3 (MASK_DELTA) — статус ---
	// Persistent-тумблер csqc_delta (default 0). При 1 модуль рисует delta-сущности
	// (player/backpack) сам, движок их не рисует (return 1). CSQC render-слой неполон
	// (видимость/анимация/скины/смерть) — см. docs/plans/ezquake_csqc_client_render_layer.md.
	if (D == 14) {
		yy = 470;
		drawstring([60, yy, 0],
			sprintf("C4 Э3 delta %g owned pl %g bp %g", cvar("csqc_delta"), g_c4_owned_pl, g_c4_owned_bp),
			[12, 12], (cvar("csqc_delta") != 0) ? '0 1 0' : '1 1 0', 1, 0);
	}
}
