
#include "catch.hpp"

#include <tcb/ranges/view.hpp>

#include <range/v3/algorithm/count_if.hpp>

#include <experimental/string_view>

#define TEST_STRING "\n \r \r\n \u0085 \u000b \u000c \u2028 \u2029"

TEST_CASE("Line end transformations work as expected", "[line_end]")
{
    //constexpr char str[] = u8"" TEST_STRING;
    const std::experimental::string_view str = u8"" TEST_STRING;

    auto v = tcb::ranges::view::line_end_transform(str);

    static_assert(tcb::ranges::rng::ForwardRange<decltype(v)>(), "");

    REQUIRE(ranges::count_if(v, [](char c) { return c == '\n'; }) == 8);
}