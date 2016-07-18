
// Copyright (c) 2016 Tristan Brindle (tcbrindle at gmail dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "catch.hpp"

#include <tcb/utf_ranges/view.hpp>
#include <range/v3/algorithm/equal.hpp>

#include <experimental/string_view>

using namespace tcb::utf_ranges;
using std::experimental::string_view;
using std::experimental::u16string_view;
using std::experimental::u32string_view;
using std::experimental::wstring_view;

#define TEST_STRING "$€0123456789你好abcdefghijklmnopqrstyvwxyz\U0001F60E"

/*
 * Default construction
 */

TEST_CASE("utf_convert_view can be default constructed", "[view]")
{
    constexpr char str[] = u8"" TEST_STRING;
    const auto v = tcb::utf_ranges::utf_convert_view<
            rng::view::all_t<decltype(str)>, char, char32_t>{};
    static_assert(rng::ForwardRange<decltype(v)>(), "");
    REQUIRE(v.empty());
}

/*
 * Handling empty ranges
 */

TEST_CASE("UTF-8 -> UTF-8 view handles empty ranges", "[view]")
{
    constexpr string_view str{};
    const auto v = view::utf8(str);
    REQUIRE(v.empty());
}

TEST_CASE("UTF-8 -> UTF-16 view handles empty ranges", "[view]")
{
    constexpr string_view str{};
    const auto v = view::utf16(str);
    REQUIRE(v.empty());
}


TEST_CASE("UTF-8 -> UTF-32 view handles empty ranges", "[view]")
{
    constexpr string_view str{};
    const auto v = view::utf32(str);
    REQUIRE(v.empty());
}

TEST_CASE("UTF-16 -> UTF-8 view handles empty ranges", "[view]")
{
    constexpr u16string_view str{};
    const auto v = view::utf8(str);
    REQUIRE(v.empty());
}

TEST_CASE("UTF-16 -> UTF-16 view handles empty ranges", "[view]")
{
    constexpr u16string_view str{};
    const auto v = view::utf16(str);
    REQUIRE(v.empty());
}


TEST_CASE("UTF-16 -> UTF-32 view handles empty ranges", "[view]")
{
    constexpr u16string_view str{};
    const auto v = view::utf32(str);
    REQUIRE(v.empty());
}

TEST_CASE("UTF-32 -> UTF-8 view handles empty ranges", "[view]")
{
    constexpr u32string_view str{};
    const auto v = view::utf8(str);
    REQUIRE(v.empty());
}

TEST_CASE("UTF-32 -> UTF-16 view handles empty ranges", "[view]")
{
    constexpr u32string_view str{};
    const auto v = view::utf16(str);
    REQUIRE(v.empty());
}

TEST_CASE("UTF-32 -> UTF-32 view handles empty ranges", "[view]")
{
    constexpr u32string_view str{};
    const auto v = view::utf32(str);
    REQUIRE(v.empty());
}

TEST_CASE("wchar -> UTF-8 view handles empty ranges", "[view]")
{
    constexpr wstring_view str{};
    const auto v = view::utf8(str);
    REQUIRE(v.empty());
}

TEST_CASE("wchar -> UTF-16 view handles empty ranges", "[view]")
{
    constexpr wstring_view str{};
    const auto v = view::utf16(str);
    REQUIRE(v.empty());
}

TEST_CASE("wchar -> UTF-32 view handles empty ranges", "[view]")
{
    constexpr wstring_view str{};
    const auto v = view::utf32(str);
    REQUIRE(v.empty());
}

/*
 * Valid (compiler-generated) UTF
 */

TEST_CASE("UTF-8 -> UTF-8 view handles valid UTF correctly", "[view]")
{
    constexpr char str[] = u8"" TEST_STRING;
    const auto v = view::utf8(str);
    REQUIRE(rng::equal(str, v));
}

TEST_CASE("UTF-8 -> UTF-16 view handles valid UTF correctly", "[view]")
{
    constexpr char str[] = u8"" TEST_STRING;
    constexpr char16_t check[] = u"" TEST_STRING;
    const auto v = view::utf16(str);
    REQUIRE(rng::equal(check, v));
}

TEST_CASE("UTF-8 -> UTF-32 view handles valid UTF correctly", "[view]")
{
    constexpr char str[] = u8"" TEST_STRING;
    constexpr char32_t check[] = U"" TEST_STRING;
    const auto v = view::utf32(str);
    REQUIRE(rng::equal(check, v));
}

TEST_CASE("UTF-16 -> UTF-8 view handles valid UTF correctly", "[view]")
{
    constexpr char16_t str[] = u"" TEST_STRING;
    constexpr char check[] = u8"" TEST_STRING;
    const auto v = view::utf8(str);
    REQUIRE(rng::equal(check, v));
}

TEST_CASE("UTF-16 -> UTF-16 view handles valid UTF correctly", "[view]")
{
    constexpr char16_t str[] = u"" TEST_STRING;
    constexpr char16_t check[] = u"" TEST_STRING;
    const auto v = view::utf16(str);
    REQUIRE(rng::equal(check, v));
}

TEST_CASE("UTF-16 -> UTF-32 view handles valid UTF correctly", "[view]")
{
    constexpr char16_t str[] = u"" TEST_STRING;
    constexpr char32_t check[] = U"" TEST_STRING;
    const auto v = view::utf32(str);
    REQUIRE(rng::equal(check, v));
}

TEST_CASE("UTF-32 -> UTF-8 view handles valid UTF correctly", "[view]")
{
    constexpr char32_t str[] = U"" TEST_STRING;
    constexpr char check[] = u8"" TEST_STRING;
    const auto v = view::utf8(str);
    REQUIRE(rng::equal(check, v));
}

TEST_CASE("UTF-32 -> UTF-16 view handles valid UTF correctly", "[view]")
{
    constexpr char32_t str[] = U"" TEST_STRING;
    constexpr char16_t check[] = u"" TEST_STRING;
    const auto v = view::utf16(str);
    REQUIRE(rng::equal(check, v));
}

TEST_CASE("UTF-32 -> UTF-32 view handles valid UTF correctly", "[view]")
{
    constexpr char32_t str[] = U"" TEST_STRING;
    constexpr char32_t check[] = U"" TEST_STRING;
    const auto v = view::utf32(str);
    REQUIRE(rng::equal(check, v));
}

TEST_CASE("wchar -> UTF-8 view handles valid UTF correctly", "[view]")
{
    constexpr wchar_t str[] = L"" TEST_STRING;
    constexpr char check[] = u8"" TEST_STRING;
    const auto v = view::utf8(str);
    REQUIRE(rng::equal(check, v));
}

TEST_CASE("wchar -> UTF-16 view handles valid UTF correctly", "[view]")
{
    constexpr wchar_t str[] = L"" TEST_STRING;
    constexpr char16_t check[] = u"" TEST_STRING;
    const auto v = view::utf16(str);
    REQUIRE(rng::equal(check, v));
}

TEST_CASE("wchar -> UTF-32 view handles valid UTF correctly", "[view]")
{
    constexpr wchar_t str[] = L"" TEST_STRING;
    constexpr char32_t check[] = U"" TEST_STRING;
    const auto v = view::utf32(str);
    REQUIRE(rng::equal(check, v));
}

TEST_CASE("Non-character conversions work as expected", "[view]")
{
    constexpr char str[] = u8"" TEST_STRING;
    constexpr char16_t check[] = u"" TEST_STRING;
    std::vector<uint16_t> vec = view::utf_convert<uint16_t>(str);
    REQUIRE(rng::equal(check, vec));
}

