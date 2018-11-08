#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "common/string.hpp"
#include <doctest.h>
TEST_CASE("split") {
  std::string aa = "aa bb cc dd";
  auto res = yunyuncc::split(aa, ' ');
  REQUIRE(res.size() == 4);
  REQUIRE(res[0] == "aa");
  REQUIRE(res[1] == "bb");
  REQUIRE(res[2] == "cc");
  REQUIRE(res[3] == "dd");
}
