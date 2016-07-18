
#ifndef TCB_UTF_RANGES_VIEW_LINE_END_TRANSFORM_HPP_INCLUDED
#define TCB_UTF_RANGES_VIEW_LINE_END_TRANSFORM_HPP_INCLUDED

#include <range/v3/view_adaptor.hpp>
#include <tcb/utf_ranges/view/utf_convert.hpp>

namespace tcb {
namespace utf_ranges {

namespace rng = ::ranges::v3;

namespace view {

template <typename Rng>
class line_end_transform_view
        : public rng::view_adaptor<line_end_transform_view<Rng>, Rng>
{
private:
    friend rng::range_access;

    struct adaptor : rng::adaptor_base {
        adaptor() = default;

        char32_t get(rng::range_iterator_t<Rng> it) const
        {
            char32_t c = *it;
            switch (c) {
            case U'\u0085': // Next line (NEL)
            case U'\u000B': // Vertical tab (VT)
            case U'\u000C': // Form feed (FF)
            case U'\u2028': // Line separator (LS)
            case U'\u2029': // Paragraph separator (PS)
                c = U'\n';
                break;
            case U'\u000D': // Carriage return (CR)
                c = U'\n';
                // If next character is LF, skip it
                skip_next_ = (*++it == U'\u000A');
                break;
            }

            return c;
        }

        void next(rng::range_iterator_t<Rng>& it)
        {
            ++it;
            if (skip_next_) {
                ++it;
                skip_next_ = false;
            }
        }

        bool equal(const adaptor& other) const
        {
            return skip_next_ == other.skip_next_;
        }

        mutable bool skip_next_ = false;
    };

public:

    adaptor begin_adaptor() const { return adaptor{}; }

    line_end_transform_view() = default;

    line_end_transform_view(Rng rng)
        : rng::view_adaptor<line_end_transform_view, Rng>{std::move(rng)}
    {}
};

template <typename Rng>
auto line_end_transform(Rng&& range)
{
    // Convert to UTF-32 and then back again
    using CharT = rng::range_value_t<Rng>;
    using R = decltype(rng::view::all(as_utf32(std::forward<Rng>(range))));
    return utf_convert<CharT>(
        line_end_transform_view<R>{rng::view::all(as_utf32(std::forward<Rng>(range)))}
    );
}


} // end namespace view
} // end namespace utf_ranges
} // end namespace tcb

#endif // TCB_UTF_RANGES_VIEW_LINE_END_TRANSFORM_HPP_INCLUDED
