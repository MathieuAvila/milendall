cmake_minimum_required(VERSION 3.16)
project(milendall)

find_package(nlohmann_json 3.2.0 REQUIRED)

find_package(Boost 1.40 COMPONENTS program_options REQUIRED)


# add the executable
add_executable(milendall
        game/src/main.cxx
        game/src/level.hxx
        game/src/level.cxx
        game/src/file_library.hxx
        game/src/file_library.cxx
        )

target_link_libraries(milendall ${Boost_LIBRARIES} nlohmann_json::nlohmann_json)

add_executable(game_test
  game/test/unit_level.cpp
  catch2/catch_amalgamated.cpp
  catch2/catch_amalgamated.hpp
  )

target_include_directories(game_test PRIVATE catch2/ )