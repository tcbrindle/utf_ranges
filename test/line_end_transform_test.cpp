
// Copyright (c) 2016 Tristan Brindle (tcbrindle at gmail dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "catch.hpp"

#include <tcb/utf_ranges/view.hpp>

#include <range/v3/algorithm/count_if.hpp>

#define TEST_STRING "\n \r \r\n \u0085 \u000b \u000c \u2028 \u2029"

TEST_CASE("Line end transformations work as expected", "[line_end]")
{
    const std::string str = u8"" TEST_STRING;

    auto v = tcb::utf_ranges::view::line_end_transform(str);

    static_assert(ranges::ForwardRange<decltype(v)>(), "");

    REQUIRE(ranges::count_if(v, [](char c) { return c == '\n'; }) == 8);
}