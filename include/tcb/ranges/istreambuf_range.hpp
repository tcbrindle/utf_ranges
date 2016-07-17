
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
using rng::static_const;

template<typename CharT = char, typename Traits = std::char_traits<CharT>>
struct istreambuf_range
  : rng::view_facade<istreambuf_range<CharT, Traits>, rng::unknown>
{
private:
    friend rng::range_access;
    std::basic_streambuf<CharT, Traits> *sbin_ = nullptr;
    bool done_ = false;
    typename Traits::int_type obj_{};

    struct cursor
    {
    private:
        istreambuf_range *rng_ = nullptr;
    public:
        cursor() = default;
        explicit cursor(istreambuf_range &rng)
          : rng_(&rng)
        {}

        void next()
        {
            rng_->next();
        }

        CharT get() const noexcept
        {
            return Traits::to_char_type(rng_->obj_);
        }

        bool done() const
        {
            return rng_->done_;
        }

    };

    void next()
    {
        obj_ = sbin_->sbumpc();
        if (obj_ == Traits::eof()) {
            done_ = true;
        }
    }

    cursor begin_cursor()
    {
        return cursor{*this};
    }

public:
    istreambuf_range() = default;

    istreambuf_range(std::basic_istream<CharT, Traits>& sin)
      : sbin_(sin.rdbuf())
    {
        next(); // prime the pump
    }
};


struct istreambuf_fn
{
    template <typename CharT, typename Traits>
    istreambuf_range<CharT, Traits>
    operator()(std::basic_istream<CharT, Traits>& sin) const
    {
        return {sin};
    }
};

RANGES_INLINE_VARIABLE(istreambuf_fn, istreambuf);


} // end namespace ranges
} // end namespace tcb

#endif
