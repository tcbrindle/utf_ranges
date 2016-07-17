
//
//  istreambuf_range.hpp
//  Based on range-v3/istream_range.hpp
//  Copyright Eric Niebler 2013-2014
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef TCB_RANGES_ISTREAMBUF_RANGE_HPP
#define TCB_RANGES_ISTREAMBUF_RANGE_HPP

#include <istream>
#include <range/v3/range_fwd.hpp>
#include <range/v3/view_facade.hpp>
#include <range/v3/utility/semiregular.hpp>
#include <range/v3/utility/static_const.hpp>

namespace tcb {
namespace ranges {

namespace rng = ::ranges::v3;

template<typename Val, typename CharT = char, typename Traits = std::char_traits<CharT>>
struct istreambuf_range
  : rng::view_facade<istreambuf_range<Val, CharT, Traits>, rng::unknown>
{
private:
    friend rng::range_access;
    std::basic_streambuf<CharT, Traits> *sin_;
    rng::semiregular_t<Val> obj_;
    bool done_ = false;

    struct cursor
    {
    private:
        istreambuf_range *rng_;
    public:
        cursor() = default;
        explicit cursor(istreambuf_range &rng)
          : rng_(&rng)
        {}
        void next()
        {
            rng_->next();
        }
        Val &get() const noexcept
        {
            return rng_->cached();
        }
        bool done() const
        {
            return rng_->done_;
        }
        Val && move() const noexcept
        {
            return rng::detail::move(rng_->cached());
        }

    };
    void next()
    {
        if (cached() == Traits::eof()) {
            done_ = true;
        }
        else {
            cached() = sin_->sbumpc();
        }
    }

    cursor begin_cursor()
    {
        return cursor{*this};
    }

    istreambuf_range(std::basic_streambuf<CharT, Traits>* sin, Val *)
      : sin_(sin), obj_{}
    {}
    istreambuf_range(std::basic_streambuf<CharT, Traits>* sin, rng::semiregular<Val> *)
      : sin_(sin), obj_{rng::in_place}
    {}
public:
    istreambuf_range() = default;
    istreambuf_range(std::basic_istream<CharT, Traits>& sin)
      : istreambuf_range(sin.rdbuf(), rng::_nullptr_v<rng::semiregular_t<Val>>())
    {
        next(); // prime the pump
    }
    Val & cached() noexcept
    {
        return obj_;
    }
};

#if !RANGES_CXX_VARIABLE_TEMPLATES
template<typename Val, typename CharT, typename Traits>
istreambuf_range<Val> istreambuf(std::basic_istream<CharT, Traits> & sin)
{
    CONCEPT_ASSERT_MSG(rng::DefaultConstructible<Val>(),
       "Only DefaultConstructible types are extractable from streams.");
    return istreambuf_range<Val>{sin};
}
#else
template<typename Val, typename CharT, typename Traits,
        CONCEPT_REQUIRES_(rng::DefaultConstructible<Val>())>
struct istreambuf_fn
{
    istreambuf_range<Val> operator()(std::basic_istream<CharT, Traits>& sin) const
    {
        return istreambuf_range<Val>{sin};
    }
};

#if RANGES_CXX_INLINE_VARIABLES < RANGES_CXX_INLINE_VARIABLES_17
inline namespace
{
    template <typename Val, typename CharT = char, typename Traits = std::char_traits<CharT>>
    constexpr auto& istreambuf = static_const<istreambuf_fn<Val, CharT, Traits>>::value;
}
#else  // RANGES_CXX_INLINE_VARIABLES >= RANGES_CXX_INLINE_VARIABLES_17
inline namespace function_objects
{
    template <typename Val, typename CharT = char, typename Traits = std::char_traits<CharT>>
    inline constexpr istreambuf_fn<Val, CharT, Traits> istreambuf{};
}
#endif  // RANGES_CXX_INLINE_VARIABLES

#endif  // RANGES_CXX_VARIABLE_TEMPLATES
    /// @}

} // end namespace ranges
} // end namespace tcb

#endif
