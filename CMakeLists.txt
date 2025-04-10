cmake_minimum_required(VERSION 3.11.2)
project(Rope_Pulling_Game_Simulation_In_C C)

set(CMAKE_C_STANDARD 11)

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

add_executable(referee src/referee.c src/utils/config.c
        src/utils/random.c src/utils/player_utils.c src/utils/referee_orders.c src/game.c)

add_executable(player src/player.c src/utils/player_utils.c src/utils/random.c src/utils/config.c src/utils/file.c src/utils/referee_orders.c src/game.c)

add_executable(rope_game_graphics src/graphics/main_graphics.c src/utils/config.c)

add_executable(rope_pulling_game_main src/main.c src/game.c src/utils/config.c)

target_link_libraries(rope_game_graphics
        PRIVATE
        ${OPENGL_LIBRARIES}
        ${GLUT_LIBRARIES} m
)

target_include_directories(rope_game_graphics
        PRIVATE
        ${OPENGL_INCLUDE_DIRS}
        ${GLUT_INCLUDE_DIRS}
)

set_target_properties(referee
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
)

set_target_properties(player
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
)

set_target_properties(rope_game_graphics
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
)

set_target_properties(rope_pulling_game_main
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin
)


add_custom_target(

    run
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/bin/rope_pulling_game_main
    DEPENDS rope_pulling_game_main
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

include_directories(include)