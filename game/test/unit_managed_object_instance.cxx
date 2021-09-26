#include <gtest/gtest.h>

#include <filesystem>
#include <iostream>
#include <memory>

#include "common.hxx"
#include "managed_object_instance.hxx"

#include <glm/gtx/string_cast.hpp>

static auto console = getConsole("test_managed_object_instance");

class ManagedObjectInstanceTest : public ::testing::Test {
 protected:
  void SetUp() override {
     //spdlog::get("managed_object_instance")->set_level(spdlog::level::debug);
     spdlog::get("test_managed_object_instance")->set_level(spdlog::level::debug);
  }
};

TEST_F(ManagedObjectInstanceTest, advance_0_) {


}