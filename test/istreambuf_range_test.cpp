
#include "catch.hpp"

#include <tcb/ranges/istreambuf_range.hpp>
#include <range/v3/algorithm/equal.hpp>

#include <sstream>

#define TEST_STRING "$€0123456789你好abcdefghijklmnopqrstyvwxyz\U0001F60E"

TEST_CASE("Basic istreambuf_range test", "[istreambuf_range]")
{
    std::istringstream ss{TEST_STRING};

    auto rng = tcb::ranges::istreambuf(ss);

    // The stringstream eats the final NUL of the string literal, so we have to
    // wrap it in a string to do the same (although string_view would do too)
    REQUIRE(ranges::equal(rng, std::string(TEST_STRING)));
}


TEST_CASE("Basic istreambuf_range<char16_t> test", "[istreambuf_range]")
{
    std::basic_istringstream<char16_t> ss{u"" TEST_STRING};

    auto rng = tcb::ranges::istreambuf(ss);

    REQUIRE(ranges::equal(rng, std::u16string{u"" TEST_STRING}));
}

TEST_CASE("Basic istreambuf_range<char32_t> test", "[istreambuf_range]")
{
    std::basic_istringstream<char32_t> ss{U"" TEST_STRING};

    auto rng = tcb::ranges::istreambuf(ss);

    REQUIRE(ranges::equal(rng, std::u32string{U"" TEST_STRING}));
}
