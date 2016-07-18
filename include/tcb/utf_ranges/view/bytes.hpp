
#ifndef TCB_UTF_RANGES_VIEW_BYTES_HPP_INCLUDED
#define TCB_UTF_RANGES_VIEW_BYTES_HPP_INCLUDED

#include <range/v3/view_adaptor.hpp>
#include <range/v3/view/view.hpp>

namespace tcb {
namespace utf_ranges {

namespace rng = ::ranges::v3;
using rng::static_const;

template <typename Rng>
class bytes_view : public rng::view_adaptor<bytes_view<Rng>, Rng>
{
private:
    using value_type = rng::range_value_t<Rng>;
    using byte = unsigned char;

    friend rng::range_access;

    struct adaptor : rng::adaptor_base
    {
        adaptor() = default;

        adaptor(const bytes_view& b)
        {
            if (b.mutable_base().begin() != b.mutable_base().end()) {
                fill_buffer(b.mutable_base().begin());
            }
        }

        void fill_buffer(rng::range_iterator_t<Rng> it)
        {
            const value_type t = *it;
            std::copy(reinterpret_cast<const byte*>(&t),
                      reinterpret_cast<const byte*>(&t) + sizeof(value_type),
                      buf_.begin());
            idx_ = 0;
        }


        byte get(rng::range_iterator_t<Rng>) const {
            return buf_[idx_++];
        }

        void next(rng::range_iterator_t<Rng>& it) {
            if (idx_ == sizeof(value_type)) {
                fill_buffer(++it);
            }
        }

        std::array<byte, sizeof(value_type)> buf_{{}};
        mutable int idx_ = 0;
    };

public:

    bytes_view() = default;

    bytes_view(Rng range)
            : rng::view_adaptor<bytes_view, Rng>(std::move(range))
    {}

    adaptor begin_adaptor() const { return adaptor{*this}; }
};

namespace view {

struct bytes_fn {

    template <typename Rng>
    bytes_view<rng::view::all_t<Rng>> operator()(Rng&& range) const
    {
        return {rng::view::all(std::forward<Rng>(range))};
    }

    decltype(auto) operator()() const
    {
        rng::make_pipeable(std::bind(*this));
    }

};

RANGES_INLINE_VARIABLE(rng::view::view<bytes_fn>, bytes);

} // end namespace view
} // end namespace utf_ranges
} // end namespace tcb

#endif // TCB_UTF_RANGES_VIEW_BYTES_HPP_INCLUDED
