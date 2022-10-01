#include <gtest/gtest.h>

#include <filesystem>
#include <iostream>

#include "file_library.hxx"
#include "json_helper_accessor.hxx"

#include "gltf/gltf_exception.hxx"

using namespace nlohmann;


json get_json_element()
{
    auto fl = FileLibrary();
    std::string pwd = std::filesystem::current_path();
    fl.addRootFilesystem(pwd + "/../game/test/sample/json");
    auto ref = fl.getRoot().getSubPath("simple.json");
    auto raw_json = ref.readStringContent();
    auto json_element = json::parse(raw_json.c_str());
    std::cout << json_element << std::endl;
    return json_element;
}

TEST(GLTF_ACCESSOR, VALID)
{
    auto json_element = get_json_element();
    auto get_room_0 = jsonGetElementByIndex(json_element, "rooms", 0);
    std::cout << get_room_0 << std::endl;
    EXPECT_TRUE( to_string(get_room_0) == "{\"r\":1}" );

    auto get_room_1 = jsonGetElementByIndex(json_element, "rooms", 1);
    std::cout << get_room_1 << std::endl;
    EXPECT_TRUE( to_string(get_room_1) == "{\"r\":2}" );

    auto get_room_2 = jsonGetElementByIndex(json_element, "rooms", 2);
    std::cout << get_room_2 << std::endl;
    EXPECT_TRUE( to_string(get_room_2) == "{\"r\":3}" );

    auto get_gate_2 = jsonGetElementByIndex(json_element, "gates", 2);
    std::cout << get_gate_2 << std::endl;
    EXPECT_TRUE( to_string(get_gate_2) == "{\"g\":3}" );
}

TEST(GLTF_ACCESSOR, INVALID )
{
    auto json_element = get_json_element();
    EXPECT_THROW( jsonGetElementByIndex(json_element, "no_element", 0), GltfException);
    EXPECT_THROW( jsonGetElementByIndex(json_element, "rooms", 10), GltfException);
}

TEST(GLTF_ACCESSOR, GET_VALID_FIELD )
{
    auto json_element = get_json_element();
    EXPECT_TRUE(to_string(jsonGetElementByName(json_element, "rooms"))
     == "[{\"r\":1},{\"r\":2},{\"r\":3}]" );
}

TEST(GLTF_ACCESSOR, GET_INVALID_FIELD )
{
    auto json_element = get_json_element();
    EXPECT_TRUE(to_string(jsonGetElementByName(json_element, "rooms"))
     == "[{\"r\":1},{\"r\":2},{\"r\":3}]" );
}
