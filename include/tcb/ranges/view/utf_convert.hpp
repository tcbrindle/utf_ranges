
#ifndef TCB_RANGES_VIEW_UTF_CONVERT_HPP
#define TCB_RANGES_VIEW_UTF_CONVERT_HPP

#include <range/v3/view_facade.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/view.hpp>

#include <tcb/ranges/detail/utf.hpp>

namespace tcb {
namespace ranges {

namespace rng = ::ranges::v3;
using rng::static_const;
using rng::operator|;

namespace view {

template <typename Range, typename InCharT, typename OutCharT>
class utf_convert_view
        : public rng::view_facade<utf_convert_view<Range, InCharT, OutCharT>, rng::unknown> {
    struct cursor {
        cursor() = default;

        cursor(utf_convert_view& parent)
                : first_(rng::begin(parent.range_)),
                  last_(rng::end(parent.range_))
        {
            prev_ = first_;
            char32_t c = ranges::detail::utf_traits<InCharT>::decode(first_,
                                                                     last_);
            next_chars_ = ranges::detail::utf_traits<OutCharT>::encode(c);
        }

        cursor(const utf_convert_view& parent)
                : first_(rng::begin(parent.range_)),
                  last_(rng::end(parent.range_))
        {
            prev_ = first_;
            char32_t c = ranges::detail::utf_traits<InCharT>::decode(first_,
                                                                     last_);
            next_chars_ = ranges::detail::utf_traits<OutCharT>::encode(c);
        }

        void next()
        {
            if (++idx_ == next_chars_.size()) {
                prev_ = first_;
                char32_t c = ranges::detail::utf_traits<InCharT>::decode(first_, last_);
                next_chars_ = ranges::detail::utf_traits<OutCharT>::encode(c);
                idx_ = 0;
            }
        }

        OutCharT get() const
        {
            return next_chars_[idx_];
        }

        bool done() const
        {
            return prev_ == last_;
        }

        bool equal(const cursor& other) const
        {
            return std::tie(next_chars_) ==
                    std::tie(other.next_chars_);
        }

        ranges::detail::encoded_chars<OutCharT> next_chars_;
        char idx_ = 0;
        rng::range_iterator_t<Range> first_{};
        // FIXME: Hack hack hack!
        rng::range_iterator_t<Range> prev_{};
        rng::range_sentinel_t<Range> last_{};
    };

public:
    cursor begin_cursor() { return cursor{*this}; }

    CONCEPT_REQUIRES(rng::Range<const Range>())
    cursor begin_cursor() const { return cursor{*this}; }

    utf_convert_view() = default;

    utf_convert_view(Range range)
            : range_{std::move(range)} {}

private:
    Range range_{};
    friend rng::range_access;
};

#if 0
template <typename OutCharT,
          typename Range,
          typename InCharT = rng::range_value_t<Range>>
utf_convert_view<rng::view::all_t<Range>, InCharT, OutCharT>
utf_convert(Range&& range)
{
    return utf_convert_view<rng::view::all_t<Range>, InCharT, OutCharT>{
            rng::view::all(std::forward<Range>(range))};
}
#endif

template <typename OutCharT>
struct utf_convert_fn {
    template <typename Range,
              typename InCharT = rng::range_value_t<Range>>
    utf_convert_view<rng::view::all_t<Range>, InCharT, OutCharT>
    operator()(Range&& range) const
    {
        return {rng::view::all(std::forward<Range>(range))};
    }

    decltype(auto) operator()() const
    {
        return rng::make_pipeable(std::bind(*this));
    }
};

inline namespace
{
    template <typename OutCharT>
    constexpr auto& utf_convert = static_const<rng::view::view<utf_convert_fn<OutCharT>>>::value;
}

struct as_utf8_fn {
    template <typename Range>
    utf_convert_view<rng::view::all_t<Range>, rng::range_value_t<Range>, char>
    operator()(Range&& range) const
    {
        return {rng::view::all(std::forward<Range>(range))};
    }

    decltype(auto) operator()() const
    {
        return rng::make_pipeable(std::bind(*this));
    }
};

RANGES_INLINE_VARIABLE(rng::view::view<as_utf8_fn>, as_utf8);

struct as_utf16_fn {
    template <typename Range>
    utf_convert_view<rng::view::all_t<Range>, rng::range_value_t<Range>, char16_t>
    operator()(Range&& range) const
    {
        return {rng::view::all(std::forward<Range>(range))};
    }

    decltype(auto) operator()() const
    {
        return rng::make_pipeable(std::bind(*this));
    }
};

RANGES_INLINE_VARIABLE(rng::view::view<as_utf16_fn>, as_utf16);

struct as_utf32_fn {
    template <typename Range>
    utf_convert_view<rng::view::all_t<Range>, rng::range_value_t<Range>, char32_t>
    operator()(Range&& range) const
    {
        return {rng::view::all(std::forward<Range>(range))};
    }

    decltype(auto) operator()() const
    {
        return rng::make_pipeable(std::bind(*this));
    }
};

RANGES_INLINE_VARIABLE(rng::view::view<as_utf32_fn>, as_utf32);

#if 0
template <typename Range>
constexpr inline auto as_utf8(Range&& range)
{
    return utf_convert<char>(std::forward<Range>(range));
}

template <typename Range>
constexpr inline auto as_utf16(Range&& range)
{
    return view::utf_convert<char16_t>(std::forward<Range>(range));
}

template <typename Range>
constexpr inline auto as_utf32(Range&& range)
{
    return view::utf_convert<char32_t>(std::forward<Range>(range));
}
#endif

} // end namespace view
} // end namespace ranges
} // end namespace tcb

#endif // TCB_RANGES_VIEW_UTF_CONVERT_HPP
