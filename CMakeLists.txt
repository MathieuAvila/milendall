cmake_minimum_required(VERSION 3.16)
project(milendall)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# to be set with an option
set(CMAKE_BUILD_TYPE Debug)

find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(spdlog REQUIRED)
find_package(glm REQUIRED)
find_package(FreeImagePlus REQUIRED)
find_package(FreeImage REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW 2.0 REQUIRED)
find_package(glfw3 3.3 REQUIRED)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(Freetype REQUIRED)

#find_package(Lua 5.1 REQUIRED)
find_package(Lua51 REQUIRED)

set(GLFW_INCLUDE_PATH "" CACHE PATH "The directory that contains GL/glfw.h" )
set(GLFW_LIBRARY "" CACHE FILEPATH "libglfw.a or glfw.lib" )
set(GLEW_INCLUDE_PATH "" CACHE PATH "The directory that contains GL/glew.h" )
set(GLEW_LIBRARY "" CACHE FILEPATH "libglew.a or glew.lib" )

find_package(GTest REQUIRED)
find_package(GMock REQUIRED)
include(GoogleTest)

include_directories(${FREETYPE_INCLUDE_DIR_ft2build} ${FREETYPE_INCLUDE_DIR_freetype2})
include_directories(${LUA_INCLUDE_DIR})
include_directories(game/src/ game/src/helpers/ game/src/objects/ game/src/gltf/ game/src/structure)

SET (CMAKE_C_COMPILER               "/usr/bin/clang-11")
SET (CMAKE_CXX_COMPILER             "/usr/bin/clang++-11")
SET (CMAKE_CXX_FLAGS                "-std=c++17 -Wall -O0 -DGLM_ENABLE_EXPERIMENTAL")
SET (CMAKE_CXX_FLAGS_DEBUG          "-g")

# Cannot be set due to local static "console" variable in all files.
# SET (CMAKE_UNITY_BUILD ON)

add_library(game_lib STATIC
  game/src/common.hxx
  game/src/common.cxx
  game/src/structure/level_exception.hxx
  game/src/structure/level.hxx
  game/src/structure/level.cxx
  game/src/structure/room.hxx
  game/src/structure/room.cxx
  game/src/structure/room_node.hxx
  game/src/structure/room_node.cxx
  game/src/structure/room_resolver.hxx
  game/src/structure/room_draw_context.hxx
  game/src/structure/space_resolver.hxx
  game/src/structure/face_list.hxx
  game/src/structure/face_list.cxx
  game/src/structure/face_hard.hxx
  game/src/structure/face_hard.cxx
  game/src/structure/room_node_gravity.hxx
  game/src/structure/room_node_gravity.cxx
  game/src/structure/gravity_provider.hxx
  game/src/structure/gravity_information.hxx
  game/src/structure/gravity_information.cxx
  game/src/structure/states_list.hxx
  game/src/structure/states_list.cxx
  game/src/structure/room_animation.hxx
  game/src/structure/room_animation.cxx
  game/src/structure/trigger.hxx
  game/src/structure/trigger.cxx
  game/src/structure/structure_object_type.hxx
  game/src/structure/gate_id.hxx
  game/src/structure/gate_id.cxx
  game/src/structure/iface_room_node_portal_register.hxx
  game/src/structure/impl_room_node_portal_register.hxx
  game/src/structure/impl_room_node_portal_register.cxx
  game/src/structure/viewables_registrar.hxx
  game/src/structure/viewables_registrar.cxx
  game/src/structure/viewables_registrar_impl.hxx
  game/src/structure/viewables_registrar_impl.cxx
  game/src/structure/viewable_object.hxx
  game/src/structure/level_info.hxx
  game/src/structure/level_info.cxx
  game/src/gltf/gltf_mesh.hxx
  game/src/gltf/gltf_mesh.cxx
  game/src/gltf/gltf_model.hxx
  game/src/gltf/gltf_model.cxx
  game/src/gltf/gltf_instance_iface.hxx
  game/src/gltf/gltf_instance_iface.cxx
  game/src/gltf/gltf_material_accessor_iface.hxx
  game/src/gltf/gltf_material_accessor_library_iface.hxx
  game/src/gltf/gltf_material_accessor.cxx
  game/src/gltf/gltf_data_accessor_iface.hxx
  game/src/gltf/gltf_data_accessor.hxx
  game/src/gltf/gltf_data_accessor.cxx
  game/src/gltf/gltf_exception.hxx
  game/src/gltf/gltf_exception.cxx
  game/src/gltf/gltf_animation.hxx
  game/src/gltf/gltf_animation.cxx
  game/src/gltf/gltf_animation_target_iface.hxx
  game/src/helpers/helper_math.hxx
  game/src/helpers/helper_math.cxx
  game/src/helpers/clipping_planes.hxx
  game/src/helpers/clipping_planes.cxx
  game/src/helpers/json_helper_accessor.hxx
  game/src/helpers/json_helper_accessor.cxx
  game/src/helpers/json_helper_math.hxx
  game/src/helpers/json_helper_math.cxx
  game/src/helpers/file_library.hxx
  game/src/helpers/file_library.cxx
  game/src/helpers/gl_init.hxx
  game/src/helpers/fonts.cxx
  game/src/helpers/fonts.hxx
  game/src/helpers/script.hxx
  game/src/helpers/script.cxx
  game/src/helpers/min_max_box.hxx
  game/src/helpers/min_max_box.cxx
  game/src/helpers/i18n_string.hxx
  game/src/helpers/i18n_string.cxx
  game/src/span.hxx
  game/src/objects/movable_object.hxx
  game/src/objects/object_manager.cxx
  game/src/objects/object_manager.hxx
  game/src/objects/managed_object.hxx
  game/src/objects/managed_object.cxx
  game/src/objects/managed_object_instance.hxx
  game/src/objects/managed_object_instance.cxx
  game/src/objects/player.hxx
  game/src/objects/player.cxx
  game/src/objects/object_option.hxx
  game/src/objects/object_option.cxx
  game/src/objects/object_exit.hxx
  game/src/objects/object_exit.cxx
  game/src/objects/model_registry.hxx
  game/src/objects/model_registry.cxx
  game/src/objects/object_type_register.hxx
  game/src/objects/object_type_register.cxx
  game/src/point_of_view.hxx
  game/src/point_of_view.cxx
)

add_library(real_gl STATIC
    game/src/helpers/gl_init.cxx
    game/src/helpers/fps_counter.hxx
    game/src/helpers/fps_counter.cxx
)

set(core_lib
    game_lib
    nlohmann_json::nlohmann_json
    spdlog::spdlog
    glm::glm
    ${FREEIMAGEPLUS_LIBRARIES} ${FreeImage_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    ${LUA_LIBRARIES}
)

set(gl_libs
    ${OPENGL_LIBRARY}
    real_gl
    GLEW::GLEW
    glfw
)

# add the room preview executable
add_executable(preview_room
    game/src/main_preview_room.cxx
)

target_link_libraries(preview_room
    ${core_lib}
    ${gl_libs}
)

# add the object preview executable
add_executable(preview_object
    game/src/main_preview_object.cxx
)

target_link_libraries(preview_object
    ${core_lib}
    ${gl_libs}
)

# add the game executable
add_executable(milendall
    game/src/menu.hxx
    game/src/menu.cxx
    game/src/main.cxx
    game/src/game.hxx
    game/src/game.cxx
    game/src/keyboard.hxx
    game/src/keyboard.cxx
    )

target_link_libraries(milendall
    ${core_lib}
    ${gl_libs}
)

add_library(glmock SHARED game/test/glmock/glmock.cpp)

add_executable(unit_test
game/test/unit_main.cxx
game/test/test_common.hxx
game/test/structure/unit_level.cxx
game/test/structure/unit_room.cxx
game/test/structure/unit_room_node_gravity.cxx
game/test/structure/unit_gravity_information.cxx
game/test/structure/unit_states_list.cxx
game/test/structure/unit_room_animation.cxx
game/test/structure/unit_trigger.cxx
game/test/structure/unit_gate_id.cxx
game/test/structure/unit_impl_room_node_portal_register.cxx
game/test/structure/unit_viewables_registrar_impl.cxx
game/test/structure/unit_level_info.cxx
game/test/structure/unit_face_list.cxx
game/test/gltf/unit_gltf_material_accessor.cxx
game/test/gltf/unit_gltf_table_accessor.cxx
game/test/gltf/unit_gltf_data_accessor.cxx
game/test/gltf/unit_gltf_animation.cxx
game/test/gltf/unit_gltf_model.cxx
game/test/gltf/unit_gltf_node.cxx
game/test/helpers/unit_json_helper_math.cxx
game/test/helpers/unit_helper_math.cxx
game/test/helpers/unit_clipping_planes.cxx
game/test/helpers/unit_file_library.cxx
game/test/helpers/unit_point_of_view.cxx
game/test/helpers/unit_script.cxx
game/test/helpers/unit_min_max_box.cxx
game/test/helpers/unit_i18n_string.cxx
game/test/objects/unit_model_registry.cxx
game/test/objects/unit_object_manager.cxx
game/test/objects/unit_player.cxx
game/test/objects/unit_managed_object_instance.cxx
)

enable_testing()
target_link_libraries(unit_test
    ${core_lib}
    glmock
    gtest gtest_main gmock pthread
)
target_include_directories(unit_test PRIVATE game/test/glmock/ game/test/)

include(GoogleTest)
gtest_discover_tests(unit_test)
