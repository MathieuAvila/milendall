#include "catch_amalgamated.hpp"

#include <filesystem>
#include <iostream>

#include "file_library.hxx"
#include "gltf_table_accessor.hxx"

#include "gltf_exception.hxx"

using namespace nlohmann;

TEST_CASE( "Get table values", "[gltf]" ) {


    auto fl = FileLibrary();
    std::string pwd = std::filesystem::current_path();
    fl.addRootFilesystem(pwd + "/../game/test/sample/json");
    auto ref = fl.getRoot().getSubPath("simple.json");
    auto raw_json = ref.readStringContent();
    auto json_element = json::parse(raw_json.c_str());
    std::cout << json_element << std::endl;

SECTION("valid")
{
    auto get_room_0 = gltfGetElementByIndex(json_element, "rooms", 0);
    std::cout << get_room_0 << std::endl;
    REQUIRE( to_string(get_room_0) == "{\"r\":1}" );

    auto get_room_1 = gltfGetElementByIndex(json_element, "rooms", 1);
    std::cout << get_room_1 << std::endl;
    REQUIRE( to_string(get_room_1) == "{\"r\":2}" );

    auto get_room_2 = gltfGetElementByIndex(json_element, "rooms", 2);
    std::cout << get_room_2 << std::endl;
    REQUIRE( to_string(get_room_2) == "{\"r\":3}" );

    auto get_gate_2 = gltfGetElementByIndex(json_element, "gates", 2);
    std::cout << get_gate_2 << std::endl;
    REQUIRE( to_string(get_gate_2) == "{\"g\":3}" );
}

SECTION( "invalid", "[gltf]" ) {

    REQUIRE_THROWS( gltfGetElementByIndex(json_element, "no_element", 0));
    REQUIRE_THROWS( gltfGetElementByIndex(json_element, "rooms", 10));
}
}

TEST_CASE( "fields op", "[gltf]" )
{
    auto fl = FileLibrary();
    std::string pwd = std::filesystem::current_path();
    fl.addRootFilesystem(pwd + "/../game/test/sample/json");
    auto ref = fl.getRoot().getSubPath("simple.json");
    auto raw_json = ref.readStringContent();
    auto json_element = json::parse(raw_json.c_str());

SECTION( "Get valid field" ) {


    REQUIRE(to_string(gltfGetElementByName(json_element, "rooms"))
     == "[{\"r\":1},{\"r\":2},{\"r\":3}]" );
}
SECTION( "Get invalid field" ) {

    REQUIRE(to_string(gltfGetElementByName(json_element, "rooms"))
     == "[{\"r\":1},{\"r\":2},{\"r\":3}]" );
}

}