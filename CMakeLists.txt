cmake_minimum_required(VERSION 2.6)

# Name of project
set(PROJ_NAME "dna")
project(${PROJ_NAME})

set (CMAKE_CXX_STANDARD 17)
set(COMPILER_FLAGS "-O3")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Specify project files: header files and source files
set(HDRS
    include/engine/application.h
    include/engine/view.h
    include/engine/transform.h
    include/engine/camera.h 
    include/engine/resource.h 
    include/engine/resource_manager.h 
    include/engine/mesh.h
    include/engine/shader.h
    include/engine/texture.h
    include/engine/scene_graph.h 
    include/engine/scene_node.h 
    include/engine/random.h
    include/engine/defines.h
    include/engine/control.h
    include/engine/light.h
    include/engine/collision_data.h
    include/engine/node_types.h
    include/lib/stb_image.h
    include/game/game.h 
    include/game/asteroid.h
    include/game/player.h
    include/game/tp_player.h
    include/game/enemy.h
    include/game/tree.h
    include/game/text.h
    include/game/trigger.h
    include/game/collision_manager.h
    include/game/terrain.h
    include/game/agent.h
    include/game/fp_player.h
    include/game/colliders/colliders.h
    include/game/story_data.h
    include/game/menu_controller.h
    include/game/skybox.h
    include/game/item.h
    include/game/beacon.h
    include/game/thrust.h
    include/game/mooneye.h
    include/game/mooncloud.h
    include/game/rocket.h
    include/game/explosion.h
    include/game/toggle.h
)
 
set(SRCS
    src/main.cpp
    src/engine/application.cpp
    src/engine/view.cpp
    src/engine/transform.cpp
    src/engine/camera.cpp 
    src/engine/resource.cpp 
    src/engine/resource_manager.cpp 
    src/engine/mesh.cpp
    src/engine/shader.cpp
    src/engine/texture.cpp
    src/engine/scene_graph.cpp 
    src/engine/scene_node.cpp 
    src/engine/random.cpp 
    src/engine/control.cpp
    src/engine/light.cpp
    src/game/text.cpp
    src/game/game.cpp 
    src/game/asteroid.cpp
    src/game/tp_player.cpp
    src/game/enemy.cpp
    src/game/tree.cpp
    src/game/trigger.cpp
    src/game/collision_manager.cpp
    src/game/terrain.cpp
    src/game/fp_player.cpp
    src/game/agent.cpp
    src/game/colliders/colliders.cpp
    src/game/menu_controller.cpp
    src/game/item.cpp
    src/game/beacon.cpp
    src/game/thrust.cpp
    src/game/mooneye.cpp
    src/game/mooncloud.cpp
    src/game/rocket.cpp
    src/game/explosion.cpp
    src/game/toggle.cpp
)

# Add path name to configuration file
#configure_file(path_config.h.in path_config.h)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/engine/path_config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/engine/path_config.h)

# Add executable based on the header and source files
add_executable(${PROJ_NAME} ${HDRS} ${SRCS})
add_definitions(${COMPILER_FLAGS})

target_include_directories(${PROJ_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/engine)
target_include_directories(${PROJ_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/game)
target_include_directories(${PROJ_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/lib)

# Require OpenGL library
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
target_link_libraries(${PROJ_NAME} ${OPENGL_gl_LIBRARY})


# Other libraries needed
set(LIBRARY_PATH ${CMAKE_CURRENT_SOURCE_DIR}/libs CACHE PATH "Folder with GLEW, GLFW, and GLM libraries")
include_directories(${LIBRARY_PATH}/include)
if(NOT WIN32)
    find_library(GLEW_LIBRARY GLEW)
    find_library(GLFW_LIBRARY glfw)
    find_library(SOIL_LIBRARY SOIL)
    find_library(IRRKLANG_LIBRARY libIrrKlang.so HINTS ${LIBRARY_PATH}/lib)
elseif(WIN32)
    find_library(GLEW_LIBRARY glew32s HINTS ${LIBRARY_PATH}/lib)
    find_library(GLFW_LIBRARY glfw3 HINTS ${LIBRARY_PATH}/lib)
    find_library(SOIL_LIBRARY SOIL HINTS ${LIBRARY_PATH}/lib)
    find_library(IRRKLANG_LIBRARY IrrKlang HINTS ${LIBRARY_PATH}/lib)
endif(NOT WIN32)
target_link_libraries(${PROJ_NAME} ${GLEW_LIBRARY})
target_link_libraries(${PROJ_NAME} ${GLFW_LIBRARY})
target_link_libraries(${PROJ_NAME} ${IRRKLANG_LIBRARY})

# target_link_libraries(${PROJ_NAME} ${SOIL_LIBRARY})

# The rules here are specific to Windows Systems
if(WIN32)
    # Avoid ZERO_CHECK target in Visual Studio
    set(CMAKE_SUPPRESS_REGENERATION TRUE)

    set_target_properties(${PROJ_NAME} PROPERTIES
        VS_GLOBAL_CXX_FLAGS "${VS_GLOBAL_CXX_FLAGS} /STACK:8388608"
    )
    
    target_compile_options(${PROJ_NAME} PRIVATE "/bigobj")
 
    # This will use the proper libraries in debug mode in Visual Studio
    set_target_properties(${PROJ_NAME} PROPERTIES DEBUG_POSTFIX _d)

    # Specify the source directory containing DLLs
    set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs/lib)

    # Specify the destination directory
    set(DESTINATION_DIR ${CMAKE_CURRENT_BINARY_DIR}/Release)

    # Get a list of all DLLs in the source directory
    file(GLOB DLLS ${SOURCE_DIR}/*.dll)

    # Create a custom command for each DLL
    foreach(DLL ${DLLS})
        get_filename_component(DLL_NAME ${DLL} NAME)
        add_custom_command(
            TARGET ${PROJ_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${DLL}
                ${DESTINATION_DIR}/${DLL_NAME}
            COMMENT "Copying ${DLL_NAME} to ${DESTINATION_DIR}"
        )
    endforeach()

    # Create a custom target to group the commands
    add_custom_target(CopyDLLs ALL DEPENDS ${DLLS})
endif(WIN32)
