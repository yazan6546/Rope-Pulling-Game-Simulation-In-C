cmake_minimum_required(VERSION 3.28)
project(Rope_Pulling_Game_Simulation_In_C C)

set(CMAKE_C_STANDARD 11)

add_executable(Rope_Pulling_Game_Simulation_In_C src/main.c src/utils/config.c
src/utils/random.c src/utils/player_utils.c src/utils/file.c src/utils/referee_orders.c)


add_executable(player src/player.c src/utils/player_utils.c src/utils/random.c)


set_target_properties(Rope_Pulling_Game_Simulation_In_C
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
)

set_target_properties(player
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
)

#add_custom_target(
#    run
#    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/bin/Rope_Pulling_Game_Simulation_In_C
#    DEPENDS Rope_Pulling_Game_Simulation_In_C
#    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#)

include_directories(include)