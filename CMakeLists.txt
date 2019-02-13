# Проверка версии CMake
cmake_minimum_required( VERSION 3.4.0 )
# Если версия установленой программы ниже, то ошибка выполнения

# Название проекта и проверка доступности компиляторя с++
project( qwprogs C )			

# Установка переменной со списком исходников
set( SOURCE_LIB 
    "src/bot/ai.c"
    "src/bot/ai-misc.c"
    "src/bot/fighting.c"
    "src/bot/move.c"
    "src/bot/qw.c"
    "src/bot/ui.c"
    "src/bot/waypoint.c"
    "src/actions.c"
    "src/admin.c"
    "src/admin_cmd.c"
    "src/arena.c"
    "src/buttons.c"
    "src/clan.c"
    "src/client.c"
    "src/combat.c"
    "src/coop.c"
    "src/crc32.c"
    "src/ctf.c"
    "src/debug.c"
    "src/demoman.c"
    "src/doors.c"
    "src/engineer.c"
    "src/flare.c"
    "src/g_cmd.c"
    "src/g_main.c"
    "src/g_mem.c"
    "src/g_mod_command.c"
    "src/g_spawn.c"
    "src/g_syscalls.c"
    "src/g_tf_utils.c"
    "src/g_utils.c"
    "src/hook.c"
    "src/items.c"
    "src/mathlib.c"
    "src/menu.c"
    "src/misc.c"
    "src/plats.c"
    "src/player.c"
    "src/pyro.c"
    "src/q_shared.c"
    "src/qmd5.c"
    "src/scout.c"
    "src/sentry.c"
    "src/server.c"
    "src/sniper.c"
    "src/spectate.c"
    "src/spy.c"
    "src/status.c"
    "src/subs.c"
    "src/tfort.c"
    "src/tforthlp.c"
    "src/tfortmap.c"
    "src/tforttm.c"
    "src/tg.c"
    "src/triggers.c"
    "src/tsoldier.c"
    "src/version.c"
    "src/vote.c"
    "src/weapons.c"
    "src/world.c"
    )		

# Включение экспорта всех символов для создания динамической библиотеки
set ( CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON )

# Создание динамической библиотеки с именем example
add_library( qwprogs SHARED ${SOURCE_LIB} )	
target_include_directories ( qwprogs PRIVATE "include" )

