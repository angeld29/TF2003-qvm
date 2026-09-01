// csprogs.dat — клиентский модуль TF2003 (CSQC, без предикции).
// Этап 2: HUD из стандартных статов (hud.qc).
// Детали: docs/tf2003_csqc_client_plan.md (корень воркспейса).

void(float apiver, string enginename, float enginever) CSQC_Init = {
};

void() CSQC_WorldLoaded = {
};

void(float width, float height, float menushown) CSQC_UpdateView = {
	// 3D-сцена: при активном CSQC модуль сам рендерит вид.
	// Без clearscene/addentities/renderscene мир не рисуется вовсе.
	clearscene();
	setproperty(VF_DRAWWORLD, 1);
	setproperty(VF_DRAWCROSSHAIR, 1);
	setproperty(VF_DRAWENGINESBAR, 1);
	addentities(MASK_ENGINE | MASK_VIEWMODEL);   // MASK_VIEWMODEL — вьюмодель оружия
	renderscene();

	// HUD (этап 2) поверх 3D.
	Hud_Draw();
};

void(float isnew) CSQC_Ent_Update = {
	error("Unhandled CSQC entity\n");
};

void() CSQC_Ent_Remove = {
	remove(self);
};

void() CSQC_Input_Frame = {
};

void() CSQC_Parse_Event = {
};

float(string cmd) CSQC_ConsoleCommand = {
	return FALSE;
};

void() CSQC_Shutdown = {
};
