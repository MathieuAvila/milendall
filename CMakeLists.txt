cmake_minimum_required(VERSION 3.16)
project(milendall)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(spdlog REQUIRED)
find_package(glm REQUIRED)
find_package(FreeImagePlus REQUIRED)
find_package(FreeImage REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW 2.0 REQUIRED)
find_package(glfw3 3.3 REQUIRED)

set(GLFW_INCLUDE_PATH "" CACHE PATH "The directory that contains GL/glfw.h" )
set(GLFW_LIBRARY "" CACHE FILEPATH "libglfw.a or glfw.lib" )
set(GLEW_INCLUDE_PATH "" CACHE PATH "The directory that contains GL/glew.h" )
set(GLEW_LIBRARY "" CACHE FILEPATH "libglew.a or glew.lib" )

find_package(Boost 1.40 COMPONENTS program_options REQUIRED)

find_package(GTest REQUIRED)
find_package(GMock REQUIRED)
include(GoogleTest)



SET (CMAKE_C_COMPILER               "/usr/bin/clang-11")
SET (CMAKE_CXX_COMPILER             "/usr/bin/clang++-11")
SET (CMAKE_CXX_FLAGS                "-std=c++17 -Wall")
SET (CMAKE_CXX_FLAGS_DEBUG          "-g")

# Cannot be set due to local static "console" variable in all files.
# SET (CMAKE_UNITY_BUILD ON)

add_library(game_lib STATIC
  game/src/common.hxx
  game/src/level.hxx
  game/src/level.cxx
  game/src/room.hxx
  game/src/room.cxx
  game/src/gltf_mesh.hxx
  game/src/gltf_mesh.cxx
  game/src/gltf_model.hxx
  game/src/gltf_model.cxx
  game/src/gltf_instance_iface.hxx
  game/src/gltf_instance_iface.cxx
  game/src/gltf_material_accessor_iface.hxx
  game/src/gltf_material_accessor_library_iface.hxx
  game/src/gltf_material_accessor.cxx
  game/src/gltf_data_accessor_iface.hxx
  game/src/gltf_data_accessor.hxx
  game/src/gltf_data_accessor.cxx
  game/src/gltf_exception.hxx
  game/src/gltf_exception.cxx
  game/src/json_helper_accessor.hxx
  game/src/json_helper_accessor.cxx
  game/src/json_helper_math.hxx
  game/src/json_helper_math.cxx
  game/src/file_library.hxx
  game/src/file_library.cxx
  game/src/helper_math.hxx
  game/src/helper_math.cxx
)

add_library(real_gl STATIC
    game/src/gl_init.cxx
    game/src/gl_init.hxx
)

set(core_lib
    ${Boost_LIBRARIES}
    game_lib
    nlohmann_json::nlohmann_json
    spdlog::spdlog
    glm::glm
    ${FREEIMAGEPLUS_LIBRARIES} ${FreeImage_LIBRARIES}
)

set(gl_libs
    ${OPENGL_LIBRARY}
    real_gl
    GLEW::GLEW
    glfw
)

# add the preview executable
add_executable(preview_room
    game/src/main_preview_room.cxx
)

target_link_libraries(preview_room
    ${core_lib}
    ${gl_libs}
)

# add the game executable
add_executable(milendall
    game/src/main.cxx
    )

target_link_libraries(milendall
    ${core_lib}
    ${gl_libs}
)

add_library(glmock SHARED game/test/glmock/glmock.cpp)

add_executable(unit_test
game/test/unit_main.cpp
game/test/unit_file_library.cpp
game/test/unit_level.cpp
game/test/unit_room.cpp
game/test/unit_gltf_material_accessor.cpp
game/test/unit_gltf_table_accessor.cpp
game/test/unit_gltf_data_accessor.cpp
game/test/unit_json_helper_math.cpp
game/test/unit_gltf_model.cxx
game/test/unit_gltf_node.cxx
)

enable_testing()
target_link_libraries(unit_test
    ${core_lib}
    glmock
    gtest gtest_main gmock pthread
)
include(GoogleTest)
gtest_discover_tests(unit_test)

target_include_directories(unit_test PRIVATE game/src/ game/test/glmock/ )