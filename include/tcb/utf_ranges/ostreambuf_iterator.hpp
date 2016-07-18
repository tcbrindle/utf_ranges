
#ifndef TCB_UTF_RANGES_OSTREAMBUF_ITERATOR_HPP_INCLUDED
#define TCB_UTF_RANGES_OSTREAMBUF_ITERATOR_HPP_INCLUDED

#include <range/v3/range_fwd.hpp>

namespace tcb {
namespace utf_ranges {

template <typename Char = char, typename Traits = std::char_traits <Char>>
struct ostreambuf_iterator {
private:
    std::basic_streambuf<Char, Traits>* sout_;

    struct proxy {
        std::basic_streambuf<Char, Traits>* sout_;

        proxy& operator=(Char t)
        {
            RANGES_ASSERT(sout_);
            sout_->sputc(t);
            return *this;
        }
    };

public:
    using difference_type = std::ptrdiff_t;
    using char_type = Char;
    using traits_type = Traits;

    ostreambuf_iterator() = default;

    ostreambuf_iterator(std::basic_ostream<Char, Traits>& sout) noexcept
            : sout_(sout.rdbuf()) {}

    proxy operator*() const noexcept
    {
        return {sout_};
    }

    ostreambuf_iterator& operator++()
    {
        return *this;
    }

    ostreambuf_iterator operator++(int)
    {
        return *this;
    }
};

} // end namespace utf_ranges
} // end namespace tcb

#endif // TCB_UTF_RANGES_OSTREAMBUF_ITERATOR_HPP_INCLUDED