
// Copyright (c) 2016 Tristan Brindle (tcbrindle at gmail dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "catch.hpp"

#include <tcb/utf_ranges/view/bom.hpp>

#include <range/v3/algorithm/equal.hpp>
#include <range/v3/istream_range.hpp>
#include <iostream>
#include <sstream>

#define TEST_STRING "$€0123456789你好abcdefghijklmnopqrstyvwxyz\U0001F60E"

const auto to_little_endian = [] (const auto& in) {
    using char_type = ranges::range_value_t<decltype(in)>;
    std::basic_string<char_type> out;
    for (auto c : in) {
        out.push_back(boost::endian::native_to_little(
                tcb::utf_ranges::detail::make_swap_wrapper(c)).value);
    }
    return out;
};

const auto to_big_endian = [] (const auto& in) {
    using char_type = ranges::range_value_t<decltype(in)>;
    std::basic_string<char_type> out;
    for (auto c : in) {
        out.push_back(boost::endian::native_to_big(
                tcb::utf_ranges::detail::make_swap_wrapper(c)).value);
    }
    return out;
};


TEST_CASE("Byte order mark is prepended correctly", "[bom]")
{
    SECTION("...for UTF-8") {
        const std::string str = u8"" TEST_STRING;
        const std::string test = tcb::utf_ranges::view::add_bom(str);
        REQUIRE(test == u8"\ufeff" + str);
    }

    SECTION("...for UTF-16") {
        const std::u16string str = u"" TEST_STRING;
        const std::u16string test = tcb::utf_ranges::view::add_bom(str);
        REQUIRE(test == u"\ufeff" + str);
    }

    SECTION("...for UTF-32") {
        const std::u32string str = U"" TEST_STRING;
        const std::u32string test = tcb::utf_ranges::view::add_bom(str);
        REQUIRE(test == U"\ufeff" + str);
    }

    SECTION("...for wchar_t") {
        const std::wstring str = L"" TEST_STRING;
        const std::wstring test = tcb::utf_ranges::view::add_bom(str);
        REQUIRE(test == L"\ufeff" + str);
    }
}

TEST_CASE("Byte order marks are correctly identified", "[bom]")
{
    SECTION("...in UTF-8") {
        const std::string str = u8"\uFEFF" TEST_STRING;
        const std::string test = tcb::utf_ranges::view::consume_bom(str);
        REQUIRE(test == u8"" TEST_STRING);
    }

    SECTION("...in UTF-16") {
        const std::u16string str = u"\uFEFF" TEST_STRING;
        const std::u16string test = tcb::utf_ranges::view::consume_bom(str);
        REQUIRE(test == u"" TEST_STRING);
    }

    SECTION("...in UTF-32") {
        const std::u32string str = U"\uFEFF" TEST_STRING;
        const std::u32string test = tcb::utf_ranges::view::consume_bom(str);
        REQUIRE(test == U"" TEST_STRING);
    }
}

TEST_CASE("Strings without byte order marks are unchanged", "[bom]")
{
    SECTION("...in UTF-8") {
        const std::string str = u8"" TEST_STRING;
        const std::string test = tcb::utf_ranges::view::consume_bom(str);
        REQUIRE(test == u8"" TEST_STRING);
    }

    SECTION("...in UTF-16") {
        const std::u16string str = u"" TEST_STRING;
        const std::u16string test = tcb::utf_ranges::view::consume_bom(str);
        REQUIRE(test == u"" TEST_STRING);
    }

    SECTION("...in UTF-32") {
        const std::u32string str = U"" TEST_STRING;
        const std::u32string test = tcb::utf_ranges::view::consume_bom(str);
        REQUIRE(test == U"" TEST_STRING);
    }
}

TEST_CASE("Native endian InputRanges with byte order marks are stripped correctly", "[bom]")
{
    SECTION("...in UTF-8") {
        std::stringstream ss;
        ss << u8"\uFEFF" TEST_STRING;
        const std::string test = tcb::utf_ranges::view::consume_bom(
                ranges::istream_range<char>(ss));
        REQUIRE(test == u8"" TEST_STRING);
    }
}

TEST_CASE("InputRanges without byte order marks are unchanged", "[bom]")
{
    SECTION("...in UTF-8") {
        std::stringstream ss;
        ss << u8"" TEST_STRING;
        const std::string test = tcb::utf_ranges::view::consume_bom(
                ranges::istream_range<char>(ss));
        REQUIRE(test == u8"" TEST_STRING);
    }
}

TEST_CASE("Byte order marks are correctly used", "[bom]")
{
    SECTION("...for \"UTF-8BE\"") {
        const auto str = to_big_endian(std::string(u8"\uFEFF" TEST_STRING));
        const std::string test = tcb::utf_ranges::view::consume_bom(str);
        REQUIRE(test == u8"" TEST_STRING);
    }

    SECTION("...for wide UTF-16BE") {
        const auto str = to_big_endian(std::u16string(u"\uFEFF" TEST_STRING));
        const std::u16string test = tcb::utf_ranges::view::consume_bom(str);
        REQUIRE(test == u"" TEST_STRING);
    }

    SECTION("...for wide UTF-32BE") {
        const auto str = to_big_endian(std::u32string(U"\uFEFF" TEST_STRING));
        const std::u32string test = tcb::utf_ranges::view::consume_bom(str);
        REQUIRE(test == U"" TEST_STRING);
    }

    SECTION("...with big-endian wide strings") {
        const auto str = to_big_endian(std::wstring(L"\uFEFF" TEST_STRING));
        const std::wstring test = tcb::utf_ranges::view::consume_bom(str);
        REQUIRE(test == L"" TEST_STRING);
    }

    SECTION("...for \"UTF-8LE\"") {
        const auto str = to_little_endian(std::string(u8"\uFEFF" TEST_STRING));
        const std::string test = tcb::utf_ranges::view::consume_bom(str);
        REQUIRE(test == u8"" TEST_STRING);
    }

    SECTION("...for wide UTF-16LE") {
        const auto str = to_little_endian(std::u16string(u"\uFEFF" TEST_STRING));
        const std::u16string test = tcb::utf_ranges::view::consume_bom(str);
        REQUIRE(test == u"" TEST_STRING);
    }

    SECTION("...for wide UTF-32LE") {
        const auto str = to_little_endian(std::u32string(U"\uFEFF" TEST_STRING));
        const std::u32string test = tcb::utf_ranges::view::consume_bom(str);
        REQUIRE(test == U"" TEST_STRING);
    }

    SECTION("...with little-endian wide strings") {
        const auto str = to_little_endian(std::wstring(L"\uFEFF" TEST_STRING));
        const std::wstring test = tcb::utf_ranges::view::consume_bom(str);
        REQUIRE(test == L"" TEST_STRING);
    }
}