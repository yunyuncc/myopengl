
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "common/error.hpp"
#include <doctest.h>
#include <iostream>
using namespace std;
TEST_CASE("exception") {
  REQUIRE(true);
  try {
    throw_exception("test exception");
    REQUIRE(false);
  } catch (const std::exception &e) {
    REQUIRE(true);
    cout << e.what() << endl;
  }
}
