
// Copyright (c) 2016 Tristan Brindle (tcbrindle at gmail dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "catch.hpp"

#include <tcb/utf_ranges/ostreambuf_iterator.hpp>

#include <sstream>
#include <range/v3/algorithm/copy.hpp>

const std::string test_str = u8"$€0123456789你好abcdefghijklmnopqrstyvwxyz\U0001F60E";

TEST_CASE("ostreambuf_iterator works as expected", "[ostreambuf_iterator]")
{
    std::ostringstream ss;

    ranges::copy(test_str, tcb::utf_ranges::ostreambuf_iterator<char>(ss));

    REQUIRE(ss.str() == test_str);
}