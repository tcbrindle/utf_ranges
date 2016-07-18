
#include "catch.hpp"

#include <tcb/utf_ranges/view/bytes.hpp>
#include <range/v3/algorithm/equal.hpp>

#include <codecvt>

#define TEST_STRING "$€0123456789你好abcdefghijklmnopqrstyvwxyz\U0001F60E"

TEST_CASE("Bytes view works for UTF-8", "[bytes]")
{
    std::string str = u8"" TEST_STRING;
    std::string test = str | tcb::utf_ranges::view::bytes;

    REQUIRE(str == test);
}

TEST_CASE("Bytes view works for UTF-16", "[bytes]")
{
    // std::codecvt_utf16<char32_t> is defined to work with UTF-16-encoded
    // byte strings, which is usually massively inconvenient but actually
    // does exactly what we want in this one particular instance

    using codecvt = std::codecvt_utf16<char32_t, 0x10ffff,
                                       std::codecvt_mode::little_endian>;

    std::u16string u16 = u"" TEST_STRING;
    std::u32string u32 = U"" TEST_STRING;

    std::string test = u16 | tcb::utf_ranges::view::bytes;

    std::string u16bytes = std::wstring_convert<codecvt, char32_t>{}.to_bytes(u32);

    REQUIRE(test == u16bytes);

    //REQUIRE(ranges::equal(test, u16bytes));
}