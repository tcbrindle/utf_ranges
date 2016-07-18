
#ifndef TCB_UTF_RANGES_VIEW_BOM_HPP_INCLUDED
#define TCB_UTF_RANGES_VIEW_BOM_HPP_INCLUDED

#include <tcb/utf_ranges/view/endian_convert.hpp>
#include <tcb/utf_ranges/view/utf_convert.hpp>

#include <range/v3/algorithm/copy_n.hpp>
#include <range/v3/algorithm/equal.hpp>
#include <range/v3/view_interface.hpp>
#include <range/v3/view/concat.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/single.hpp>
#include <range/v3/view/take.hpp>

namespace tcb {
namespace utf_ranges {

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

template <typename Rng>
struct bom_concat_view : rng::view_adaptor<bom_concat_view<Rng>, Rng>
{
private:
    using string_type = std::basic_string<rng::range_value_t<Rng>>;

    struct adaptor : rng::adaptor_base {

        adaptor() = default;

        adaptor(bom_concat_view& view)
                : bom_first_(view.bom_.begin()),
                  bom_last_(view.bom_.end())
        {}

        auto get(rng::range_iterator_t<Rng> it) const
        {
            if (bom_first_ != bom_last_) {
                return *bom_first_;
            } else {
                return *it;
            }
        }

        void next(rng::range_iterator_t<Rng>& it)
        {
            if (bom_first_ != bom_last_) {
                ++bom_first_;
            } else {
                ++it;
            }
        }

        rng::range_iterator_t<string_type> bom_first_{};
        rng::range_sentinel_t<string_type> bom_last_{};
    };


public:
    bom_concat_view() = default;

    bom_concat_view(Rng rng, string_type bom)
            : rng::view_adaptor<bom_concat_view, Rng>(std::move(rng)),
              bom_(std::move(bom))
    {}

    adaptor begin_adaptor() { return adaptor{*this}; }

private:
    string_type bom_;
};

} // end namespace detail

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

        return endian_convert<>(
                detail::bom_concat_view<rng::view::all_t<Range>>(
                        rng::view::all(std::forward<Range>(range)),
                        std::move(buf)),
                byte_order);
    }

    decltype(auto) operator()() const {
        return rng::make_pipeable(std::bind(*this));
    }
};

RANGES_INLINE_VARIABLE(rng::view::view<consume_bom_fn>, consume_bom)

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

} // end namespace view
} // end namespace utf_ranges
} // end namespace tcb

#endif // TCB_UTF_RANGES_VIEW_BOM_HPP_INCLUDED
