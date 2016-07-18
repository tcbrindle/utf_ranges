
// Copyright (c) 2016 Tristan Brindle (tcbrindle at gmail dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef TCB_UTF_RANGES_CONVERT_HPP_INCLUDED
#define TCB_UTF_RANGES_CONVERT_HPP_INCLUDED

#include <tcb/utf_ranges/detail/utf.hpp>

#include <range/v3/range_fwd.hpp>

#include <string>

namespace tcb {
namespace utf_ranges {

namespace rng = ::ranges::v3;

template <typename OutCharT,
          typename InIter, typename Sentinel,
          typename OutIter,
          typename InCharT = typename std::iterator_traits<InIter>::value_type>
OutIter utf_convert(InIter first, Sentinel last, OutIter out)
{
    while (first != last) {
        const char32_t c = detail::utf_traits<InCharT>::decode(first, last);
        detail::utf_traits<OutCharT>::encode(c, out);
    }
    return out;
}

template <typename OutCharT,
          typename InRange,
          typename OutIter,
          typename InCharT = rng::range_value_t<InRange>,
          CONCEPT_REQUIRES_(rng::ForwardRange<InRange>())>
OutIter utf_convert(InRange&& range, OutIter out)
{
    return utf_convert<OutCharT, rng::range_iterator_t<InRange>,
                       rng::range_sentinel_t<InRange>, OutIter, InCharT>(
            rng::begin(range), rng::end(range), std::move(out));
}

template <typename Range, typename OutCharT,
          typename InCharT = rng::range_value_t<Range>>
std::basic_string<OutCharT>
to_utf_string(Range&& range)
{
    using string_type = std::basic_string<OutCharT>;

    string_type output;

    // Try to minimise the number of reallocations
    if /*constexpr*/ (::ranges::RandomAccessRange<Range>()) {
        output.reserve(::ranges::size(range));
    }

    utf_convert<OutCharT>(std::forward<Range>(range), std::back_inserter(output));

    return output;
}

template <typename Range>
std::string to_u8string(Range&& range)
{
    return to_utf_string<Range, char>(std::forward<Range>(range));
}

template <typename Range>
std::u16string to_u16string(Range&& range)
{
    return to_utf_string<Range, char16_t>(std::forward<Range>(range));
}

template <typename Range>
std::u32string to_u32string(Range&& range)
{
    return to_utf_string<Range, char32_t>(std::forward<Range>(range));
}

template <typename Range>
std::wstring to_wstsring(Range&& range)
{
    return to_utf_string<Range, wchar_t>(std::forward<Range>(range));
}

} // end namespace utf_ranges
} // end namespace tcb

#endif // TCB_UTF_RANGES_CONVERT_HPP_INCLUDED
