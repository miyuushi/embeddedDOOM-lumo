# This file was automatically generated for projects
# without default 'CMakeLists.txt' file.

# main/CMakeLists.txt
cmake_minimum_required(VERSION 3.16)

# 1. Core target source files (the ones that run on ESP32)
set(CS
    doomdef.c doomstat.c dstrings.c i_system.c tables.c f_finale.c    
    f_wipe.c d_main.c d_items.c g_game.c m_menu.c m_misc.c      
    m_argv.c m_bbox.c m_fixed.c m_swap.c m_cheat.c m_random.c    
    am_map.c p_ceilng.c p_doors.c p_enemy.c p_floor.c p_inter.c   
    p_lights.c p_map.c p_maputl.c p_plats.c p_pspr.c p_setup.c   
    p_sight.c p_spec.c p_switch.c p_mobj.c p_telept.c p_tick.c    
    p_saveg.c p_user.c r_bsp.c r_data.c r_draw.c r_main.c      
    r_plane.c r_segs.c r_sky.c r_things.c w_wad.c wi_stuff.c    
    v_video.c st_lib.c st_stuff.c hu_stuff.c hu_lib.c z_zone.c      
    info.c i_net.c d_net.c stubs.c sounds.c s_sound.c i_sound.c
    i_video.c
)

set(GEN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/support")

# -------------------------------------------------------------------
# ESP-IDF Target Registration
# -------------------------------------------------------------------
set(COMPONENT_SRCS 
    ${CS} i_main.c
    #"${GEN_DIR}/baked_texture_data.c"
    #"${GEN_DIR}/baked_map_data.c"
    #"${GEN_DIR}/rawwad_use.c"
)

# Standard ESP-IDF component registration macro
idf_component_register(SRCS "${COMPONENT_SRCS}"
                       INCLUDE_DIRS "."
                       REQUIRES lumo32)


set(PRECOMPILED_OBJS
    "${GEN_DIR}/baked_map_data.o"
    "${GEN_DIR}/baked_texture_data.o"
    "${GEN_DIR}/rawwad_use.o"
)
set_source_files_properties(${PRECOMPILED_OBJS} PROPERTIES
    EXTERNAL_OBJECT TRUE
    GENERATED TRUE
)

target_link_options(${COMPONENT_LIB} INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/support/baked_texture_data.o"
    "${CMAKE_CURRENT_SOURCE_DIR}/support/baked_map_data.o"
    "${CMAKE_CURRENT_SOURCE_DIR}/support/rawwad_use.o"
)

#target_sources(${COMPONENT_LIB} PRIVATE ${PRECOMPILED_OBJS})

# Force the ESP32 cross-compiler environment into GNU99 mode to prevent C23 legacy code breakage
target_compile_options(${COMPONENT_LIB} PRIVATE 
    -std=gnu99
    -fno-stack-protector
    -fdata-sections
    -ffunction-sections
    -g
    -Os
    -DRV32IMA_BUILD
    -DDISABLE_NETWORK
    -Wno-error=sequence-point
    -Wno-error=char-subscripts
    -Wno-error=dangling-pointer=
    -Wno-incompatible-pointer-types
    -Wno-error=maybe-uninitialized
    -Wno-error=parentheses
    -Wno-error=sizeof-pointer-memaccess
    -Wno-error=pointer-sign
    -Wno-error=unused-but-set-parameter
    -Wno-error=unused-const-variable=
    -Wno-error=enum-compare
    -Wno-int-conversion
    -Wno-format
)

# Re-inject the standard Doom configuration definitions to the target hardware layer
target_compile_definitions(${COMPONENT_LIB} PRIVATE 
    ${COMMON_COMPILE_OPTIONS}
    NORMALUNIX 
    MAXPLAYERS=1 
    RANGECHECK 
    DISABLE_NETWORK 
    SET_MEMORY_DEBUG=0
)
