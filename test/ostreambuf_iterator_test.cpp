
#include "catch.hpp"

#include <tcb/ranges/ostreambuf_iterator.hpp>

#include <sstream>
#include <range/v3/algorithm/copy.hpp>

const std::string test_str = u8"$€0123456789你好abcdefghijklmnopqrstyvwxyz\U0001F60E";

TEST_CASE("ostreambuf_iterator works as expected", "[ostreambuf_iterator]")
{
    std::ostringstream ss;

    ranges::copy(test_str, tcb::ranges::ostreambuf_iterator<char>(ss));

    REQUIRE(ss.str() == test_str);
}