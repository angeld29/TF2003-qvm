// hud.qc — HUD TF2003 из стандартных статов (этап 2).
// CSQC 2D-координаты: origin (0,0) = верхний левый угол, виртуальные пиксели.
// Активное оружие — из STAT_ITEMS (бит слота); броня/powerups — тоже из STAT_ITEMS.

// CSQC-тест: маркеры флагов/эхо/счёт команд (определена в main.qc).
void Hud_DrawCSQC();

// Аммо текущего оружия по имени типа (или 0).
float ammo_by_type(string t) {
	if (t == "SHELLS")  return getstatf(STAT_SHELLS);
	if (t == "NAILS")   return getstatf(STAT_NAILS);
	if (t == "ROCKETS") return getstatf(STAT_ROCKETS);
	if (t == "CELLS")   return getstatf(STAT_CELLS);
	return 0;
}

void Hud_Draw() {
	float hp = getstatf(STAT_HEALTH);
	float armor = getstatf(STAT_ARMOR);
	float items = getstatf(STAT_ITEMS);
	float shells = getstatf(STAT_SHELLS);
	float nails = getstatf(STAT_NAILS);
	float rockets = getstatf(STAT_ROCKETS);
	float cells = getstatf(STAT_CELLS);

	// Активное оружие: точное имя из серверного кастомного стата
	// STAT_WEAPON_TF (current_weapon, WEAP_* бит); фолбэк — на слот из STAT_ITEMS.
	string wname = weapon_name(getstati(STAT_WEAPON_TF));
	if (wname == "" || getstati(STAT_WEAPON_TF) == 0)
		wname = weapon_slot_name(active_weapon_slot(items));
	string ammo_name = ammo_type_name(items);
	string armor_name = armor_type_name(items);
	string pups = powerup_names(items);

	// Левая колонка сверху, одна строка на элемент. Метка перед значением,
	// поля разделены пробелами (значения фикс-ширины не нужны — строка одна).
	float y = 30;
	drawstring([10, y, 0], sprintf("Weapon %s", wname), [16, 16], '1 1 1', 1, 0);
	y += 22;
	drawstring([10, y, 0], sprintf("HP %g", hp), [16, 16], '0 1 0', 1, 0);
	y += 22;
	drawstring([10, y, 0], sprintf("Armor %g %s", armor, armor_name), [16, 16], '1 1 0', 1, 0);
	y += 22;
	drawstring([10, y, 0],
		sprintf("Ammo %g %s %s", ammo_by_type(ammo_name), ammo_name, pups),
		[16, 16], '0 1 1', 1, 0);
	y += 22;
	drawstring([10, y, 0],
		sprintf("S%g N%g R%g C%g", shells, nails, rockets, cells),
		[12, 12], '1 1 1', 1, 0);
	y += 22;

	// Канарейка: t/hp/it/ps50/tk, поля через пробел.
	drawstring([10, y, 0],
		sprintf("t%d hp%g it%g ps50 %g tk %g", time, hp, items, getstatf(50), getstatf(40)),
		[12, 12], '0 1 0', 1, 0);

	// CSQC-тест: маркеры флагов, эхо sendevent (main.qc).
	Hud_DrawCSQC();
}
