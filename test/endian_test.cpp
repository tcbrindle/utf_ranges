
// Copyright (c) 2016 Tristan Brindle (tcbrindle at gmail dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "catch.hpp"

#include <tcb/utf_ranges/view/endian_convert.hpp>

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


#define TEST_STRING "123456";

// Native endian test strings
const std::string test_string8n = u8"" TEST_STRING;
const std::u16string test_string16n = u"" TEST_STRING;
const std::u32string test_string32n = U"" TEST_STRING;
const std::wstring test_stringwn = L"" TEST_STRING;

// Little endian test strings
const std::string test_string8l = test_string8n;
const std::u16string test_string16l = to_little_endian(test_string16n);
const std::u32string test_string32l = to_little_endian(test_string32n);
const std::wstring test_stringwl = to_little_endian(test_stringwn);

// Big endian test strings
const std::string test_string8b = test_string8n;
const std::u16string test_string16b = to_big_endian(test_string16n);
const std::u32string test_string32b = to_big_endian(test_string32n);
const std::wstring test_stringwb = to_big_endian(test_stringwn);

using tcb::utf_ranges::view::endian_convert;
using namespace boost::endian;

TEST_CASE("Byte swap native-to-native works", "[endian]")
{
    SECTION("...for UTF-8") {
        const std::string test = endian_convert<order::native>(test_string8n);
        REQUIRE(test == test_string8n);
    }

    SECTION("...for UTF-16") {
        const std::u16string test = endian_convert<order::native>(test_string16n);
        REQUIRE(test == test_string16n);
    }

    SECTION("...for UTF-32") {
        const std::u32string test = endian_convert<order::native>(test_string32n);
        REQUIRE(test == test_string32n);
    }

    SECTION("... for wide chars") {
        const std::wstring test = endian_convert<order::native>(test_stringwn);
        REQUIRE(test == test_stringwn);
    }
}

TEST_CASE("Byte swap native-to-little works", "[endian]")
{
    SECTION("...for UTF-8") {
        const std::string test = endian_convert<order::little>(test_string8n);
        REQUIRE(test == test_string8n);
    }

    SECTION("...for UTF-16") {
        const std::u16string test = endian_convert<order::little>(test_string16n);
        REQUIRE(test == test_string16n);
    }

    SECTION("...for UTF-32") {
        const std::u32string test = endian_convert<order::little>(test_string32n);
        REQUIRE(test == test_string32n);
    }

    SECTION("... for wide chars") {
        const std::wstring test = endian_convert<order::little>(test_stringwn);
        REQUIRE(test == test_stringwn);
    }
}

TEST_CASE("Byte swap native-to-big works", "[endian]")
{
    SECTION("...for UTF-8") {
        const std::string test = endian_convert<order::big>(test_string8n);
        REQUIRE(test == test_string8b);
    }

    SECTION("...for UTF-16") {
        const std::u16string test = endian_convert<order::big>(test_string16n);
        REQUIRE(test == test_string16b);
    }

    SECTION("...for UTF-32") {
        const std::u32string test = endian_convert<order::big>(test_string32n);
        REQUIRE(test == test_string32b);
    }

    SECTION("... for wide chars") {
        const std::wstring test = endian_convert<order::big>(test_stringwn);
        REQUIRE(test == test_stringwb);
    }
}

TEST_CASE("Byte swap little-to-native works", "[endian]")
{
    SECTION("...for UTF-8") {
        const std::string test = endian_convert<order::native>(test_string8l,
                                                               order::little);
        REQUIRE(test == test_string8n);
    }

    SECTION("...for UTF-16") {
        const std::u16string test = endian_convert<order::native>(test_string16l,
                                                                  order::little);
        REQUIRE(test == test_string16n);
    }

    SECTION("...for UTF-32") {
        const std::u32string test = endian_convert<order::native>(test_string32l,
                                                                  order::little);
        REQUIRE(test == test_string32n);
    }

    SECTION("... for wide chars") {
        const std::wstring test = endian_convert<order::native>(test_stringwl,
                                                                order::little);
        REQUIRE(test == test_stringwn);
    }
}

TEST_CASE("Byte swap little-to-little works", "[endian]")
{
    SECTION("...for UTF-8") {
        const std::string test = endian_convert<order::little>(test_string8l,
                                                               order::little);
        REQUIRE(test == test_string8l);
    }

    SECTION("...for UTF-16") {
        const std::u16string test = endian_convert<order::little>(test_string16l,
                                                                  order::little);
        REQUIRE(test == test_string16l);
    }

    SECTION("...for UTF-32") {
        const std::u32string test = endian_convert<order::little>(test_string32l,
                                                                  order::little);
        REQUIRE(test == test_string32l);
    }

    SECTION("... for wide chars") {
        const std::wstring test = endian_convert<order::little>(test_stringwl,
                                                                order::little);
        REQUIRE(test == test_stringwl);
    }
}

TEST_CASE("Byte swap little-to-big works", "[endian]")
{
    SECTION("...for UTF-8") {
        const std::string test = endian_convert<order::big>(test_string8l,
                                                            order::little);
        REQUIRE(test == test_string8b);
    }

    SECTION("...for UTF-16") {
        const std::u16string test = endian_convert<order::big>(test_string16l,
                                                               order::little);
        REQUIRE(test == test_string16b);
    }

    SECTION("...for UTF-32") {
        const std::u32string test = endian_convert<order::big>(test_string32l,
                                                               order::little);
        REQUIRE(test == test_string32b);
    }

    SECTION("... for wide chars") {
        const std::wstring test = endian_convert<order::big>(test_stringwl,
                                                             order::little);
        REQUIRE(test == test_stringwb);
    }
}

TEST_CASE("Byte swap big-to-native works", "[endian]")
{
    SECTION("...for UTF-8") {
        const std::string test = endian_convert<order::native>(test_string8b,
                                                               order::big);
        REQUIRE(test == test_string8n);
    }

    SECTION("...for UTF-16") {
        const std::u16string test = endian_convert<order::native>(test_string16b,
                                                                  order::big);
        REQUIRE(test == test_string16n);
    }

    SECTION("...for UTF-32") {
        const std::u32string test = endian_convert<order::native>(test_string32b,
                                                                  order::big);
        REQUIRE(test == test_string32n);
    }

    SECTION("... for wide chars") {
        const std::wstring test = endian_convert<order::native>(test_stringwb,
                                                                order::big);
        REQUIRE(test == test_stringwn);
    }
}

TEST_CASE("Byte swap big-to-little works", "[endian]")
{
    SECTION("...for UTF-8") {
        const std::string test = endian_convert<order::little>(test_string8b,
                                                               order::big);
        REQUIRE(test == test_string8l);
    }

    SECTION("...for UTF-16") {
        const std::u16string test = endian_convert<order::little>(test_string16b,
                                                                  order::big);
        REQUIRE(test == test_string16l);
    }

    SECTION("...for UTF-32") {
        const std::u32string test = endian_convert<order::little>(test_string32b,
                                                                  order::big);
        REQUIRE(test == test_string32l);
    }

    SECTION("... for wide chars") {
        const std::wstring test = endian_convert<order::little>(test_stringwb,
                                                                order::big);
        REQUIRE(test == test_stringwl);
    }
}

TEST_CASE("Byte swap big-to-big works", "[endian]")
{
    SECTION("...for UTF-8") {
        const std::string test = endian_convert<order::big>(test_string8b,
                                                            order::big);
        REQUIRE(test == test_string8b);
    }

    SECTION("...for UTF-16") {
        const std::u16string test = endian_convert<order::big>(test_string16b,
                                                               order::big);
        REQUIRE(test == test_string16b);
    }

    SECTION("...for UTF-32") {
        const std::u32string test = endian_convert<order::big>(test_string32b,
                                                               order::big);
        REQUIRE(test == test_string32b);
    }

    SECTION("... for wide chars") {
        const std::wstring test = endian_convert<order::big>(test_stringwb,
                                                             order::big);
        REQUIRE(test == test_stringwb);
    }
}