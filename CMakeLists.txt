# Проверка версии CMake
cmake_minimum_required( VERSION 3.4.0 )
# Если версия установленой программы ниже, то ошибка выполнения

# Название проекта и проверка доступности компиляторя с++
project( qwprogs C )			
#ADD_CUSTOM_TARGET(qvm 
# Установка переменной со списком исходников
set( SRC_NATIVE "src/g_syscalls.c")
set( VM_LIB   
    "src/bg_lib.c"
    "src/qmalloc.c"
    )
set( SOURCE_LIB 
    "src/g_main.c"
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
    "src/g_mem.c"
    "src/g_mod_command.c"
    "src/g_spawn.c"
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
set ( VM_DIR "vm" )
set ( QVM_COMPILER "lcc" )
set ( QVM_LINKER "q3asm" )
set ( QVM_LIST_FILE "${CMAKE_CACHEFILE_DIR}/game.q3asm" )
set ( QVM_C_FLAGS -DQ3_VM -S -Wf-target=bytecode -Wf-g)
set ( INCFLAGS -I../include)
add_custom_target(vmdir COMMAND 
    ${CMAKE_COMMAND} -E make_directory ${VM_DIR}
    )
set ( SRC_VM  ${SOURCE_LIB} ${VM_LIB} )
file(WRITE ${QVM_LIST_FILE} "-o qwprogs\n")
foreach(file ${SRC_VM} )
    string(REGEX REPLACE "^src/" "" file_asm ${file})
    get_filename_component (file_asm ${file_asm} NAME_WE)
    string( CONCAT  file_asm ${file_asm} ".asm")
    #message(STATUS ${file_asm})
    list( APPEND SOURCE_ASM ${file_asm})
    file(APPEND ${QVM_LIST_FILE} "${VM_DIR}/${file_asm}\n")
    add_custom_command(OUTPUT ${file_asm}
        COMMAND ${QVM_COMPILER} ${QVM_C_FLAGS} ${INCFLAGS} "../${file}" -o "${VM_DIR}/${file_asm}"
        DEPENDS vmdir 
        )
endforeach(file)
file(APPEND ${QVM_LIST_FILE} "../src/g_syscalls.asm\n")
message(STATUS ${QVM_LIST_FILE})
add_custom_command(OUTPUT qwprogs.qvm 
        COMMAND ${QVM_LINKER} -m -f ${QVM_LIST_FILE}
        DEPENDS ${SOURCE_ASM}
    )
add_custom_target(qvm DEPENDS qwprogs.qvm )
# Создание динамической библиотеки с именем example
add_library( qwprogs SHARED ${SOURCE_LIB} ${SRC_NATIVE} )	
target_include_directories ( qwprogs PRIVATE "include" )
set_target_properties(qwprogs PROPERTIES PREFIX "")

