#include <gtest/gtest.h>

#include "common.hxx"
#include "i18n_string.hxx"

#include <filesystem>

static auto console = getConsole("unit_i18n_string");

class I18NStringTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        spdlog::get("unit_i18n_string")->set_level(spdlog::level::debug);
        spdlog::get("i18n_string")->set_level(spdlog::level::debug);
    }
};

TEST_F(I18NStringTest, test_load)
{
    auto json_element = json::parse("{\"name\": [{\"lang\": \"fr\", \"value\":\"guignol\"}]}");
    I18NString found(json_element, "name");
    EXPECT_EQ(found.getString("fr","fr"), "guignol");
    I18NString notfound(json_element, "notfound");
    EXPECT_EQ(notfound.getString("fr","fr"), "");
}

TEST_F(I18NStringTest, test_order)
{
    auto json_element = json::parse("{\"name\": [ {\"lang\": \"fr\", \"value\":\"guignol\"}, {\"lang\": \"en\", \"value\":\"trump\"} ]}");
    I18NString found(json_element, "name");
    EXPECT_EQ(found.getString("fr","en"), "guignol");
    EXPECT_EQ(found.getString("gr","en"), "trump");
    EXPECT_EQ(found.getString("gr","sw"), "guignol");
    I18NString notfound(json_element, "notfound");
    EXPECT_EQ(notfound.getString("gr","sw"), "");
}

