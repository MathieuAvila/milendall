#include <gtest/gtest.h>

#include "viewables_registrar_impl.hxx"

#include "common.hxx"

using namespace std;

static auto console = getConsole("ut_viewables_registrar_impl");

class ViewablesRegistrarImplTest : public ::testing::Test {
 protected:
  void SetUp() override {
     spdlog::get("ut_viewables_registrar_impl")->set_level(spdlog::level::debug);
     spdlog::get("viewables_registrar_impl")->set_level(spdlog::level::debug);
  }
};


TEST_F(ViewablesRegistrarImplTest, init) {

}
