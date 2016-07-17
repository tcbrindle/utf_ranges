
#pragma once

#include <tcb/ranges/view/endian_convert.hpp>
#include <tcb/ranges/view/utf_convert.hpp>

#include <range/v3/algorithm/copy_n.hpp>
#include <range/v3/algorithm/equal.hpp>
#include <range/v3/view_interface.hpp>
#include <range/v3/view/concat.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/single.hpp>
#include <range/v3/view/take.hpp>

namespace tcb {
namespace ranges {

using rng::static_const;
using rng::operator|;

namespace view {

namespace detail {
template <std::size_t> constexpr std::size_t bom_size_helper = 0;
template<> constexpr std::size_t bom_size_helper<1> = 3;
template<> constexpr std::size_t bom_size_helper<2> = 1;
template<> constexpr std::size_t bom_size_helper<4> = 1;

template <typename T>
struct bom_size {
    static constexpr std::size_t value = bom_size_helper<sizeof(T)>;
};

template <typename T> constexpr std::size_t bom_size_v = bom_size<T>::value;

constexpr boost::endian::order nonnative_order =
        boost::endian::order::native == boost::endian::order::big
        ? boost::endian::order::little
        : boost::endian::order::big;

template <typename Range>
bool has_bom(const Range& range)
{
    using v = rng::range_value_t<Range>;

    // Better to use if constexpr once we get it, but this should be
    // easily optimised too
    switch (sizeof(v)) {
    case 1: {
        auto it = rng::cbegin(range);
        return rng::size(range) >= 3 &&
                static_cast<uint8_t>(*it) == 0xEF &&
                static_cast<uint8_t>(*++it) == 0xBB &&
                static_cast<uint8_t>(*++it) == 0xBF;
    }
    case 2:
        return rng::size(range) != 0 &&
                static_cast<char16_t>(*rng::begin(range)) == u'\uFEFF';
    case 4:
        return rng::size(range) != 0 &&
                static_cast<char32_t>(*rng::begin(range)) == U'\uFEFF';
    default:
        return false;
    }
}

template <typename Range>
bool has_swapped_bom(const Range& range)
{
    using v = rng::range_value_t<Range>;

    switch (sizeof(v)) {
    case 2:
        return static_cast<std::uint16_t>(*rng::begin(range)) ==
                boost::endian::endian_reverse(static_cast<uint16_t>(u'\uFEFF'));
    case 4:
        return static_cast<std::uint32_t>(*rng::begin(range)) ==
                boost::endian::endian_reverse(static_cast<uint32_t>(U'\uFEFF'));
    default:
        return false;
    }
}

template <typename Range>
struct concat_range : rng::view_interface<concat_range<Range>> {
private:
    using value_type = rng::range_value_t<Range>;
    std::basic_string<value_type> buf_;
    Range range_;
    boost::endian::order order_ = boost::endian::order::native;
    using view_type = decltype(endian_convert<>(rng::view::concat(buf_, range_), order_));
    view_type view_ = endian_convert<>(rng::view::concat(buf_, range_), order_);

public:
    auto begin() { return rng::begin(view_); }

    CONCEPT_REQUIRES(rng::Range<const view_type>())
    auto begin() const { return rng::begin(view_); }

    auto end() { return rng::end(view_); }

    CONCEPT_REQUIRES(rng::Range<const view_type>())
    auto end() const { return rng::end(view_); }

    concat_range(std::basic_string<value_type> buf,
                 rng::view::all_t<Range> range,
                 boost::endian::order o)
            : buf_(std::move(buf)),
              range_(std::move(range)),
              order_{o}
    {}
};

} // end namespace detail

#if 0
template <typename Range,
          CONCEPT_REQUIRES_(rng::ForwardRange<Range>())>
auto consume_bom(Range&& range)
{
    using value_type = rng::range_value_t<Range>;

    rng::range_difference_t<Range> bom_size = 0;
    boost::endian::order byte_order = boost::endian::order::native;

    if (detail::has_bom(range)) {
        bom_size = detail::bom_size_v<value_type>;
    } else if (detail::has_swapped_bom(range)) {
        bom_size = detail::bom_size_v<value_type>;
        byte_order = detail::nonnative_order;
    }

    return endian_convert(rng::view::drop(std::forward<Range>(range), bom_size),
                          byte_order);
}



template <typename Range,
          CONCEPT_REQUIRES_(rng::InputRange<Range>() &&
                            !rng::ForwardRange<Range>())>
auto consume_bom(Range&& range)
{
    using value_type = rng::range_value_t<Range>;
    constexpr rng::range_difference_t<Range> bom_size = detail::bom_size_v<value_type>;

    boost::endian::order byte_order = boost::endian::order::native;

    // For InputRanges (only), testing for the BOM will "eat up" the first
    // character(s) of the range. So save them in a temporary string so that
    // we can put them back later if it turns out not to be a BOM.
    std::basic_string<value_type> buf{};
    rng::copy_n(rng::begin(range), bom_size, rng::back_inserter(buf));

    if (detail::has_bom(buf)) {
        buf.clear();
    } else if (detail::has_swapped_bom(buf)) {
        buf.clear();
        byte_order = detail::nonnative_order;
    }

    return detail::concat_range<rng::view::all_t<Range>>{std::move(buf),
                                                         rng::view::all(std::forward<Range>(range)),
                                                         byte_order};
}
#endif

struct consume_bom_fn {
    template <typename Range,
              CONCEPT_REQUIRES_(rng::ForwardRange<Range>())>
    auto operator()(Range&& range) const
    {
        using value_type = rng::range_value_t<Range>;

        rng::range_difference_t<Range> bom_size = 0;
        boost::endian::order byte_order = boost::endian::order::native;

        if (detail::has_bom(range)) {
            bom_size = detail::bom_size_v<value_type>;
        } else if (detail::has_swapped_bom(range)) {
            bom_size = detail::bom_size_v<value_type>;
            byte_order = detail::nonnative_order;
        }

        return endian_convert<>(rng::view::drop(std::forward<Range>(range), bom_size),
                                byte_order);
    }

    template <typename Range,
              CONCEPT_REQUIRES_(rng::InputRange<Range>() &&
                                !rng::ForwardRange<Range>())>
    auto operator()(Range&& range) const
    {
        using value_type = rng::range_value_t<Range>;
        constexpr rng::range_difference_t<Range> bom_size = detail::bom_size_v<value_type>;

        boost::endian::order byte_order = boost::endian::order::native;

        // For InputRanges (only), testing for the BOM will "eat up" the first
        // character(s) of the range. So save them in a temporary string so that
        // we can put them back later if it turns out not to be a BOM.
        std::basic_string<value_type> buf{};
        rng::copy_n(rng::begin(range), bom_size, rng::back_inserter(buf));

        if (detail::has_bom(buf)) {
            buf.clear();
        } else if (detail::has_swapped_bom(buf)) {
            buf.clear();
            byte_order = detail::nonnative_order;
        }

        return detail::concat_range<rng::view::all_t<Range>>{std::move(buf),
                                                             rng::view::all(std::forward<Range>(range)),
                                                             byte_order};
    }

    decltype(auto) operator()() const {
        return rng::make_pipeable(std::bind(*this));
    }
};

RANGES_INLINE_VARIABLE(rng::view::view<consume_bom_fn>, consume_bom)

#if 0

template <typename Range>
auto add_bom(Range&& range)
{
    using char_type = rng::range_value_t<Range>;
    constexpr char32_t bom = U'\uFEFF';

    return rng::view::concat(utf_convert<char_type>(rng::view::single(bom)),
                             std::forward<Range>(range));
}

#endif

struct add_bom_fn {

    template <typename Range>
    auto operator()(Range&& range) const
    {
        using char_type = rng::range_value_t<Range>;
        constexpr char32_t bom = U'\uFEFF';

        return rng::view::concat(utf_convert<char_type>(rng::view::single(bom)),
                                 std::forward<Range>(range));
    }

    decltype(auto) operator()() const
    {
        return rng::make_pipeable(std::bind(*this));
    }
};

RANGES_INLINE_VARIABLE(rng::view::view<add_bom_fn>, add_bom)

}
}
}
