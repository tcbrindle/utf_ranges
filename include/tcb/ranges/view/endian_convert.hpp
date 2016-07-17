
#pragma once

#include <boost/endian/conversion.hpp>
#include <range/v3/view_adaptor.hpp>
#include <range/v3/view/transform.hpp>

namespace tcb {
namespace ranges {

namespace rng = ::ranges::v3;
using rng::static_const;
using rng::operator|;

namespace detail {

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

}
}
}
