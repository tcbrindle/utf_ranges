
#ifndef TCB_UTF_RANGES_VIEW_ENDIAN_CONVERT_HPP_INCLUDED
#define TCB_UTF_RANGES_VIEW_ENDIAN_CONVERT_HPP_INCLUDED

#include <boost/endian/conversion.hpp>
#include <range/v3/view_adaptor.hpp>
#include <range/v3/view/transform.hpp>

namespace tcb {
namespace utf_ranges {

namespace rng = ::ranges::v3;
using rng::static_const;
using rng::operator|;

namespace detail {

// This stuff is necessary because Boost.Endian doesn't seem to handle
// byte-swapping character types very well. [unsigned] char should just be
// returned unaltered, but instead it gets promoted to int, byte-swapped and
// converted back to char, which means that it is always zero. The same happens
// to char16_t. To get around this, we wrap values in a swap_wrapper<> struct,
// with "overloads" for the endian_reverse functions for char and char16_t
// (actually function that get found by the Boost library via ADL). All other
// types get forwarded to the regular boost conversion function.

template <typename T>
struct swap_wrapper {
    T value;
};

template <typename T>
swap_wrapper<T> make_swap_wrapper(T t)
{
    return swap_wrapper<T>{t};
}

template <typename T>
swap_wrapper<T> endian_reverse(swap_wrapper<T> s) noexcept
{
    return swap_wrapper<T>{boost::endian::endian_reverse(s.value)};
}

inline swap_wrapper<char> endian_reverse(swap_wrapper<char> s) noexcept
{
    return s;
}

inline swap_wrapper<char16_t> endian_reverse(swap_wrapper<char16_t> s) noexcept
{
    return make_swap_wrapper(
            static_cast<char16_t>(
                boost::endian::endian_reverse(static_cast<std::uint16_t>(s.value)))
    );
}

} // end namespace detail

namespace view {

template <boost::endian::order DestOrder>
struct endian_convert_fn {
    template <typename Range>
    auto operator()(Range&& range,
                    boost::endian::order src_order = boost::endian::order::native) const
    {
        const auto swapper = [src_order] (auto c){
            return boost::endian::conditional_reverse(detail::make_swap_wrapper(c),
                                                      src_order, DestOrder).value;
        };
        return rng::view::transform(std::forward<Range>(range),
                                    std::move(swapper));
    }

    decltype(auto) operator()(boost::endian::order src_endian = boost::endian::order::native) const
    {
        return rng::make_pipeable(std::bind(*this, std::placeholders::_1,
                                            rng::protect(src_endian)));
    }
};

inline namespace
{
    template <boost::endian::order DestOrder = boost::endian::order::native>
    constexpr auto& endian_convert = static_const<rng::view::view<endian_convert_fn<DestOrder>>>::value;
}

} // end namespace view
} // end namespace utf_ranges
} // end namespace tcb

#endif // TCB_UTF_RANGES_VIEW_ENDIAN_CONVERT_HPP_INCLUDED
