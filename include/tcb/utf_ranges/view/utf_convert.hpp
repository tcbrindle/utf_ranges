
#ifndef TCB_UTF_RANGES_VIEW_UTF_CONVERT_HPP_INCLUDED
#define TCB_UTF_RANGES_VIEW_UTF_CONVERT_HPP_INCLUDED

#include <range/v3/view_facade.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/view.hpp>

#include <tcb/utf_ranges/detail/utf.hpp>

namespace tcb {
namespace utf_ranges {

namespace rng = ::ranges::v3;
using rng::static_const;

template <typename Range, typename InCharT, typename OutCharT>
class utf_convert_view
        : public rng::view_facade<utf_convert_view<Range, InCharT, OutCharT>, rng::unknown> {
    struct cursor {
        cursor() = default;

        cursor(utf_convert_view& parent)
                : first_(rng::begin(parent.range_)),
                  last_(rng::end(parent.range_))
        {
            if (first_ != last_) {
                char32_t c = detail::utf_traits<InCharT>::decode(first_, last_);
                next_chars_ = detail::utf_traits<OutCharT>::encode(c);
            }
        }

        cursor(const utf_convert_view& parent)
                : first_(rng::begin(parent.range_)),
                  last_(rng::end(parent.range_))
        {
            if (first_ != last_) {
                char32_t c = detail::utf_traits<InCharT>::decode(first_, last_);
                next_chars_ = detail::utf_traits<OutCharT>::encode(c);
            }
        }

        void next()
        {
            if (++idx_ == next_chars_.size() && first_ != last_) {
                char32_t c = detail::utf_traits<InCharT>::decode(first_, last_);
                next_chars_ = detail::utf_traits<OutCharT>::encode(c);
                idx_ = 0;
            }
        }

        OutCharT get() const
        {
            return next_chars_[idx_];
        }

        bool done() const
        {
            return first_ == last_ && idx_ == next_chars_.size();
        }

        bool equal(const cursor& other) const
        {
            return std::tie(next_chars_) ==
                    std::tie(other.next_chars_);
        }

        detail::encoded_chars<OutCharT> next_chars_;
        char idx_ = 0;
        rng::range_iterator_t<Range> first_{};
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

namespace view {

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

struct utf8_fn {
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

RANGES_INLINE_VARIABLE(rng::view::view<utf8_fn>, utf8);

struct utf16_fn {
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

RANGES_INLINE_VARIABLE(rng::view::view<utf16_fn>, utf16);

struct utf32_fn {
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

RANGES_INLINE_VARIABLE(rng::view::view<utf32_fn>, utf32);

} // end namespace view
} // end namespace utf_ranges
} // end namespace tcb

#endif // TCB_UTF_RANGES_VIEW_UTF_CONVERT_HPP_INCLUDED
