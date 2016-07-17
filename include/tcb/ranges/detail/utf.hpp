//
//  Copyright (c) 2009-2011 Artyom Beilis (Tonkikh)
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef TCB_RANGES_DETAIL_UTF_HPP_INCLUDED
#define TCB_RANGES_DETAIL_UTF_HPP_INCLUDED

#include <cstdint>

namespace tcb {
namespace ranges {
namespace detail {

/// \cond INTERNAL
#ifdef __GNUC__
#   define BOOST_LOCALE_LIKELY(x)   __builtin_expect((x),1)
#   define BOOST_LOCALE_UNLIKELY(x) __builtin_expect((x),0)
#else
#   define BOOST_LOCALE_LIKELY(x)   (x)
#   define BOOST_LOCALE_UNLIKELY(x) (x)
#endif
/// \endcond

///
/// \brief The integral type that can hold a Unicode code point
///
typedef char32_t code_point;

///
/// \brief Special constant that defines illegal code point
///
static constexpr code_point illegal = 0xFFFFFFFFu;

///
/// \brief Special constant that defines incomplete code point
///
static constexpr code_point incomplete = 0xFFFFFFFEu;

///
/// \brief the function checks if \a v is a valid code point
///
inline constexpr bool is_valid_codepoint(code_point v)
{
    if (v > 0x10FFFF)
        return false;
    if (0xD800 <= v && v <= 0xDFFF) // surrogates
        return false;
    return true;
}

template <typename CharType, int size = sizeof(CharType)>
struct utf_traits;

template <typename CharType>
struct encoded_chars {
public:
    constexpr encoded_chars() = default;

    constexpr encoded_chars(CharType _1)
            : chars_{{_1}}, size_{1} {}

    constexpr encoded_chars(CharType _1, CharType _2)
            : chars_{{_1, _2}}, size_{2} {}

    constexpr encoded_chars(CharType _1, CharType _2, CharType _3)
            : chars_{{_1, _2, _3}}, size_{3} {}

    constexpr encoded_chars(CharType _1, CharType _2, CharType _3, CharType _4)
            : chars_{{_1, _2, _3, _4}}, size_{4} {}

    constexpr int size() const noexcept { return size_; }

    constexpr CharType operator[](int i) const noexcept { return chars_[i]; }

    friend constexpr bool operator==(const encoded_chars& lhs,
                                     const encoded_chars& rhs)
    {
        return std::equal(std::begin(lhs.chars_),
                          std::begin(lhs.chars_) + lhs.size_,
                          std::begin(rhs.chars_),
                          std::begin(rhs.chars_) + rhs.size_);
    }

private:
    std::array<CharType, 4 / sizeof(CharType)> chars_{{}};
    int size_ = 0;
};

template <typename CharType>
struct utf_traits<CharType, 1> {

    typedef CharType char_type;

    static constexpr int trail_length(char_type ci)
    {
        unsigned char c = ci;
        if (c < 128)
            return 0;
        if (BOOST_LOCALE_UNLIKELY(c < 194))
            return -1;
        if (c < 224)
            return 1;
        if (c < 240)
            return 2;
        if (BOOST_LOCALE_LIKELY(c <= 244))
            return 3;
        return -1;
    }

    static constexpr int max_width = 4;

    static constexpr int width(code_point value)
    {
        if (value <= 0x7F) {
            return 1;
        }
        else if (value <= 0x7FF) {
            return 2;
        }
        else if (BOOST_LOCALE_LIKELY(value <= 0xFFFF)) {
            return 3;
        }
        else {
            return 4;
        }
    }

    static constexpr bool is_trail(char_type ci)
    {
        unsigned char c = ci;
        return (c & 0xC0) == 0x80;
    }

    static constexpr bool is_lead(char_type ci)
    {
        return !is_trail(ci);
    }

    template <typename Iterator, typename Sentinel>
    static constexpr code_point decode(Iterator& p, Sentinel e)
    {
        if (BOOST_LOCALE_UNLIKELY(p == e))
            return incomplete;

        unsigned char lead = *p++;

        // First byte is fully validated here
        int trail_size = trail_length(lead);

        if (BOOST_LOCALE_UNLIKELY(trail_size < 0))
            return illegal;

        //
        // Ok as only ASCII may be of size = 0
        // also optimize for ASCII text
        //
        if (trail_size == 0)
            return lead;

        code_point c = lead & ((1 << (6 - trail_size)) - 1);

        // Read the rest
        unsigned char tmp{};
        switch (trail_size) {
        case 3:
            if (BOOST_LOCALE_UNLIKELY(p == e))
                return incomplete;
            tmp = *p++;
            if (!is_trail(tmp))
                return illegal;
            c = (c << 6) | (tmp & 0x3F);
        case 2:
            if (BOOST_LOCALE_UNLIKELY(p == e))
                return incomplete;
            tmp = *p++;
            if (!is_trail(tmp))
                return illegal;
            c = (c << 6) | (tmp & 0x3F);
        case 1:
            if (BOOST_LOCALE_UNLIKELY(p == e))
                return incomplete;
            tmp = *p++;
            if (!is_trail(tmp))
                return illegal;
            c = (c << 6) | (tmp & 0x3F);
        }

        // Check code point validity: no surrogates and
        // valid range
        if (BOOST_LOCALE_UNLIKELY(!is_valid_codepoint(c)))
            return illegal;

        // make sure it is the most compact representation
        if (BOOST_LOCALE_UNLIKELY(width(c) != trail_size + 1))
            return illegal;

        return c;

    }

    template <typename Iterator>
    static constexpr code_point decode_valid(Iterator& p)
    {
        unsigned char lead = *p++;
        if (lead < 192)
            return lead;

        int trail_size = 0;

        if (lead < 224)
            trail_size = 1;
        else if (BOOST_LOCALE_LIKELY(lead < 240)) // non-BMP rare
            trail_size = 2;
        else
            trail_size = 3;

        code_point c = lead & ((1 << (6 - trail_size)) - 1);

        switch (trail_size) {
        case 3:
            c = (c << 6) | (static_cast<unsigned char>(*p++) & 0x3F);
        case 2:
            c = (c << 6) | (static_cast<unsigned char>(*p++) & 0x3F);
        case 1:
            c = (c << 6) | (static_cast<unsigned char>(*p++) & 0x3F);
        }

        return c;
    }

    template <typename Iterator>
    static constexpr Iterator encode(code_point value, Iterator out)
    {
        if (value <= 0x7F) {
            *out++ = static_cast<char_type>(value);
        }
        else if (value <= 0x7FF) {
            *out++ = static_cast<char_type>((value >> 6) | 0xC0);
            *out++ = static_cast<char_type>((value & 0x3F) | 0x80);
        }
        else if (BOOST_LOCALE_LIKELY(value <= 0xFFFF)) {
            *out++ = static_cast<char_type>((value >> 12) | 0xE0);
            *out++ = static_cast<char_type>(((value >> 6) & 0x3F) | 0x80);
            *out++ = static_cast<char_type>((value & 0x3F) | 0x80);
        }
        else {
            *out++ = static_cast<char_type>((value >> 18) | 0xF0);
            *out++ = static_cast<char_type>(((value >> 12) & 0x3F) | 0x80);
            *out++ = static_cast<char_type>(((value >> 6) & 0x3F) | 0x80);
            *out++ = static_cast<char_type>((value & 0x3F) | 0x80);
        }
        return out;
    }

    static constexpr encoded_chars<CharType> encode(code_point value)
    {
        if (value <= 0x7F) {
            return {static_cast<char_type>(value)};
        }
        else if (value <= 0x7FF) {
            return {static_cast<char_type>((value >> 6) | 0xC0),
                    static_cast<char_type>((value & 0x3F) | 0x80)};
        }
        else if (BOOST_LOCALE_LIKELY(value <= 0xFFFF)) {
            return {static_cast<char_type>((value >> 12) | 0xE0),
                    static_cast<char_type>(((value >> 6) & 0x3F) | 0x80),
                    static_cast<char_type>((value & 0x3F) | 0x80)};
        }
        else {
            return {static_cast<char_type>((value >> 18) | 0xF0),
                    static_cast<char_type>(((value >> 12) & 0x3F) | 0x80),
                    static_cast<char_type>(((value >> 6) & 0x3F) | 0x80),
                    static_cast<char_type>((value & 0x3F) | 0x80)};
        }
    }

}; // utf8

template <typename CharType>
struct utf_traits<CharType, 2> {
    typedef CharType char_type;

    // See RFC 2781
    static constexpr bool is_first_surrogate(uint16_t x)
    {
        return 0xD800 <= x && x <= 0xDBFF;
    }

    static constexpr bool is_second_surrogate(uint16_t x)
    {
        return 0xDC00 <= x && x <= 0xDFFF;
    }

    static constexpr code_point combine_surrogate(uint16_t w1, uint16_t w2)
    {
        return ((code_point(w1 & 0x3FF) << 10) | (w2 & 0x3FF)) + 0x10000;
    }

    static constexpr int trail_length(char_type c)
    {
        if (is_first_surrogate(c))
            return 1;
        if (is_second_surrogate(c))
            return -1;
        return 0;
    }

    ///
    /// Returns true if c is trail code unit, always false for UTF-32
    ///
    static constexpr bool is_trail(char_type c)
    {
        return is_second_surrogate(c);
    }

    ///
    /// Returns true if c is lead code unit, always true of UTF-32
    ///
    static constexpr bool is_lead(char_type c)
    {
        return !is_second_surrogate(c);
    }

    template <typename It, typename S>
    static constexpr code_point decode(It& current, S last)
    {
        if (BOOST_LOCALE_UNLIKELY(current == last))
            return incomplete;
        uint16_t w1 = *current++;
        if (BOOST_LOCALE_LIKELY(w1 < 0xD800 || 0xDFFF < w1)) {
            return w1;
        }
        if (w1 > 0xDBFF)
            return illegal;
        if (current == last)
            return incomplete;
        uint16_t w2 = *current++;
        if (w2 < 0xDC00 || 0xDFFF < w2)
            return illegal;
        return combine_surrogate(w1, w2);
    }

    template <typename It>
    static constexpr code_point decode_valid(It& current)
    {
        uint16_t w1 = *current++;
        if (BOOST_LOCALE_LIKELY(w1 < 0xD800 || 0xDFFF < w1)) {
            return w1;
        }
        uint16_t w2 = *current++;
        return combine_surrogate(w1, w2);
    }

    static const int max_width = 2;

    static constexpr int width(code_point u)
    {
        return u >= 0x10000 ? 2 : 1;
    }

    template <typename It>
    static constexpr It encode(code_point u, It out)
    {
        if (BOOST_LOCALE_LIKELY(u <= 0xFFFF)) {
            *out++ = static_cast<char_type>(u);
        }
        else {
            u -= 0x10000;
            *out++ = static_cast<char_type>(0xD800 | (u >> 10));
            *out++ = static_cast<char_type>(0xDC00 | (u & 0x3FF));
        }
        return out;
    }

    static constexpr encoded_chars<CharType> encode(code_point u)
    {
        if (BOOST_LOCALE_LIKELY(u <= 0xFFFF)) {
            return {static_cast<char_type>(u)};
        }
        else {
            u -= 0x10000;
            return {static_cast<char_type>(0xD800 | (u >> 10)),
                    static_cast<char_type>(0xDC00 | (u & 0x3FF))};
        }
    }
}; // utf16;


template <typename CharType>
struct utf_traits<CharType, 4> {
    typedef CharType char_type;

    static constexpr int trail_length(char_type c)
    {
        if (is_valid_codepoint(c))
            return 0;
        return -1;
    }

    static constexpr bool is_trail(char_type /*c*/)
    {
        return false;
    }

    static constexpr bool is_lead(char_type /*c*/)
    {
        return true;
    }

    template <typename It>
    static constexpr code_point decode_valid(It& current)
    {
        return *current++;
    }

    template <typename It, typename S>
    static constexpr code_point decode(It& current, S last)
    {
        if (BOOST_LOCALE_UNLIKELY(current == last))
            return incomplete;
        code_point c = *current++;
        if (BOOST_LOCALE_UNLIKELY(!is_valid_codepoint(c)))
            return illegal;
        return c;
    }

    static constexpr int max_width = 1;

    static constexpr int width(code_point /*u*/)
    {
        return 1;
    }

    template <typename It>
    static constexpr It encode(code_point u, It out)
    {
        *out++ = static_cast<char_type>(u);
        return out;
    }

    static constexpr encoded_chars<char_type> encode(code_point u)
    {
        return {static_cast<char_type>(u)};
    }

}; // utf32

} // end namespace detail
} // end namespace ranges
} // end namespace tcb

#endif

// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4

