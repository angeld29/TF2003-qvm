# Проверка версии CMake
cmake_minimum_required( VERSION 3.4.0 )
# Если версия установленой программы ниже, то ошибка выполнения

add_subdirectory(tools/q3asm)
add_subdirectory(tools/q3lcc)
# Название проекта и проверка доступности компиляторя с++
project( qwprogs C )			
#ADD_CUSTOM_TARGET(qvm 
# Установка переменной со списком исходников
set ( DIR_SRC "src" )
set( SRC_NATIVE "${DIR_SRC}/g_syscalls.c")
set( SRC_VM_SYSCALL "g_syscalls.asm")
set( VM_LIB   
    "${DIR_SRC}/bg_lib.c"
    "${DIR_SRC}/qmalloc.c"
    )
set( SOURCE_LIB 
    "${DIR_SRC}/g_main.c"
    "${DIR_SRC}/bot/ai.c"
    "${DIR_SRC}/bot/ai-misc.c"
    "${DIR_SRC}/bot/fighting.c"
    "${DIR_SRC}/bot/move.c"
    "${DIR_SRC}/bot/qw.c"
    "${DIR_SRC}/bot/ui.c"
    "${DIR_SRC}/bot/waypoint.c"
    "${DIR_SRC}/actions.c"
    "${DIR_SRC}/admin.c"
    "${DIR_SRC}/admin_cmd.c"
    "${DIR_SRC}/arena.c"
    "${DIR_SRC}/buttons.c"
    "${DIR_SRC}/clan.c"
    "${DIR_SRC}/client.c"
    "${DIR_SRC}/combat.c"
    "${DIR_SRC}/coop.c"
    "${DIR_SRC}/crc32.c"
    "${DIR_SRC}/ctf.c"
    "${DIR_SRC}/debug.c"
    "${DIR_SRC}/demoman.c"
    "${DIR_SRC}/doors.c"
    "${DIR_SRC}/engineer.c"
    "${DIR_SRC}/flare.c"
    "${DIR_SRC}/g_cmd.c"
    "${DIR_SRC}/g_mem.c"
    "${DIR_SRC}/g_mod_command.c"
    "${DIR_SRC}/g_spawn.c"
    "${DIR_SRC}/g_tf_utils.c"
    "${DIR_SRC}/g_utils.c"
    "${DIR_SRC}/hook.c"
    "${DIR_SRC}/items.c"
    "${DIR_SRC}/mathlib.c"
    "${DIR_SRC}/menu.c"
    "${DIR_SRC}/misc.c"
    "${DIR_SRC}/plats.c"
    "${DIR_SRC}/player.c"
    "${DIR_SRC}/pyro.c"
    "${DIR_SRC}/q_shared.c"
    "${DIR_SRC}/qmd5.c"
    "${DIR_SRC}/scout.c"
    "${DIR_SRC}/sentry.c"
    "${DIR_SRC}/server.c"
    "${DIR_SRC}/sniper.c"
    "${DIR_SRC}/spectate.c"
    "${DIR_SRC}/spy.c"
    "${DIR_SRC}/status.c"
    "${DIR_SRC}/subs.c"
    "${DIR_SRC}/tfort.c"
    "${DIR_SRC}/tforthlp.c"
    "${DIR_SRC}/tfortmap.c"
    "${DIR_SRC}/tforttm.c"
    "${DIR_SRC}/tg.c"
    "${DIR_SRC}/triggers.c"
    "${DIR_SRC}/tsoldier.c"
    "${DIR_SRC}/version.c"
    "${DIR_SRC}/vote.c"
    "${DIR_SRC}/weapons.c"
    "${DIR_SRC}/world.c"
    )		

# Включение экспорта всех символов для создания динамической библиотеки
set ( CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON )
set ( VM_DIR "vm" )
set ( QVM_COMPILER "q3lcc" )
set ( QVM_LINKER "q3asm" )
set ( QVM_LIST_FILE "${CMAKE_CURRENT_BINARY_DIR}/game.q3asm" )
set ( QVM_C_FLAGS -DQ3_VM -S -Wf-target=bytecode -Wf-g)
set ( INCFLAGS -I${CMAKE_CURRENT_SOURCE_DIR}/include )
add_custom_target(vmdir 
    COMMAND ${CMAKE_COMMAND} -E make_directory ${VM_DIR}
    )
set ( SRC_VM  ${SOURCE_LIB} ${VM_LIB} )
file(WRITE ${QVM_LIST_FILE} "-o qwprogs\n")

foreach(file ${SRC_VM} )
    string(REGEX REPLACE "^${DIR_SRC}/" "" file_asm ${file})
    get_filename_component (file_asm ${file_asm} NAME_WE)
    string( CONCAT  file_asm ${file_asm} ".asm")
    #message(STATUS ${file_asm})
    list( APPEND SOURCE_ASM ${file_asm})
    #list( APPEND SOURCE_ASM ${VM_DIR}/${file_asm})
    file(APPEND ${QVM_LIST_FILE} "${VM_DIR}/${file_asm}\n")
    #add_custom_command(OUTPUT ${VM_DIR}/${file_asm}
    #COMMAND ${QVM_COMPILER} ${QVM_C_FLAGS} ${INCFLAGS} "${CMAKE_CURRENT_SOURCE_DIR}/${file}" -o "${VM_DIR}/${file_asm}"
    #DEPENDS vmdir ${file}
    #)
    add_custom_target(${file_asm}
        COMMAND ${QVM_COMPILER} ${QVM_C_FLAGS} ${INCFLAGS} "${CMAKE_CURRENT_SOURCE_DIR}/${file}" -o "${VM_DIR}/${file_asm}"
        DEPENDS vmdir 
        SOURCES ${file}
    )
endforeach(file)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${DIR_SRC}/${SRC_VM_SYSCALL} ${VM_DIR}/${SRC_VM_SYSCALL} )
file(APPEND ${QVM_LIST_FILE} "${VM_DIR}/${SRC_VM_SYSCALL}\n")
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

