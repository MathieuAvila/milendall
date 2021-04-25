cmake_minimum_required(VERSION 3.16)
project(milendall)

find_package(nlohmann_json 3.2.0 REQUIRED)

find_package(Boost 1.40 COMPONENTS program_options REQUIRED)

SET (CMAKE_CXX_COMPILER             "/usr/bin/clang++-11")
SET (CMAKE_CXX_FLAGS                "-std=c++17 -Wall")
SET (CMAKE_CXX_FLAGS_DEBUG          "-g")
SET (CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
SET (CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")


add_library(game_lib STATIC
  game/src/level.hxx
  game/src/level.cxx
  game/src/room.hxx
  game/src/room.cxx
  game/src/gltf_frame.hxx
  game/src/gltf_frame.cxx
  game/src/gltf_model.hxx
  game/src/gltf_model.cxx
  game/src/gltf_material_accessor_iface.hxx
  game/src/gltf_data_accessor_iface.hxx
  game/src/gltf_model.cxx
  game/src/file_library.hxx
  game/src/file_library.cxx
)

# add the executable
add_executable(milendall
        game/src/main.cxx
        )

target_link_libraries(milendall ${Boost_LIBRARIES} game_lib nlohmann_json::nlohmann_json)

add_executable(game_test
  game/test/unit_level.cpp
  game/test/unit_main.cpp
  game/test/unit_file_library.cpp
  catch2/catch_amalgamated.cpp
  catch2/catch_amalgamated.hpp
  )

target_link_libraries(game_test ${Boost_LIBRARIES} game_lib nlohmann_json::nlohmann_json)

target_include_directories(game_test PRIVATE catch2/ )
target_include_directories(game_test PRIVATE game/src/ )