cmake_minimum_required(VERSION 3.16)
project(milendall)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(spdlog REQUIRED)
find_package(glm REQUIRED)
find_package(FreeImagePlus REQUIRED)
find_package(FreeImage REQUIRED)

find_package(Boost 1.40 COMPONENTS program_options REQUIRED)

SET (CMAKE_CXX_COMPILER             "/usr/bin/clang++-11")
SET (CMAKE_CXX_FLAGS                "-std=c++17 -Wall")
SET (CMAKE_CXX_FLAGS_DEBUG          "-g")
SET (CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
SET (CMAKE_CXX_FLAGS_RELEASE        "-O4 -DNDEBUG")
SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")


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
  game/src/gltf_material_accessor_iface.hxx
  game/src/gltf_material_accessor_library_iface.hxx
  game/src/gltf_material_accessor.cxx
  game/src/gltf_data_accessor_iface.hxx
  game/src/gltf_data_accessor.hxx
  game/src/gltf_data_accessor.cxx
  game/src/gltf_exception.hxx
  game/src/json_helper_accessor.hxx
  game/src/json_helper_accessor.cxx
  game/src/json_helper_math.hxx
  game/src/json_helper_math.cxx
  game/src/file_library.hxx
  game/src/file_library.cxx
  game/src/helper_math.hxx
  game/src/helper_math.cxx
)

# add the executable
add_executable(milendall
        game/src/main.cxx
        )

target_link_libraries(milendall
  ${Boost_LIBRARIES}
  game_lib
  nlohmann_json::nlohmann_json
  spdlog::spdlog
  glm::glm
  ${FREEIMAGEPLUS_LIBRARIES} ${FreeImage_LIBRARIES}
)

add_executable(unit_test
  game/test/unit_level.cpp
  game/test/unit_main.cpp
  game/test/unit_file_library.cpp
  game/test/unit_gltf_table_accessor.cpp
  game/test/unit_gltf_data_accessor.cpp
  game/test/unit_json_helper_math.cpp
  game/test/unit_gltf_model.cxx
  game/test/unit_gltf_node.cxx
  catch2/catch_amalgamated.cpp
  catch2/catch_amalgamated.hpp
  )

target_link_libraries(unit_test
    ${Boost_LIBRARIES}
    game_lib
    nlohmann_json::nlohmann_json
    glm::glm
    ${FREEIMAGEPLUS_LIBRARIES} ${FreeImage_LIBRARIES}
)

target_include_directories(unit_test PRIVATE catch2/ )
target_include_directories(unit_test PRIVATE game/src/ )