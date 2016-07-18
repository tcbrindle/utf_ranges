
#include <cassert>
#include <chrono>
#include <codecvt>
#include <iostream>
#include <fstream>
#include <locale>
#include <string>

#include "utf8.h"

#include <boost/locale/encoding_utf.hpp>

#include <tcb/utf_ranges/view/utf_convert.hpp>

#include <tcb/utf_ranges/convert.hpp>

using std::string;
using std::u16string;
using std::u32string;

namespace {

struct timer {
    using clock = std::chrono::high_resolution_clock;
    using time_point = typename clock::time_point;

    timer() = default;

    template <typename T = std::chrono::milliseconds>
    T elapsed() const
    {
        return std::chrono::duration_cast<T>(clock::now() - start_);
    }

private:
    time_point start_ = clock::now();
};

template <typename Func, typename Arg>
void time_function_call(Func f, Arg&& arg, int n, string label)
{
    timer t;
    for (int i = 0; i < n; i++) {
        volatile auto res = f(std::forward<Arg>(arg));
    }
    auto e = t.elapsed();
    std::cout << label << " took " << e.count() << "ms\n";
}

/*
 * All six codecvt conversion functions
 */

inline
u16string codecvt_u8_to_u16(const string& u8)
{
    using codecvt = std::codecvt_utf8_utf16<char16_t>;
    return std::wstring_convert<codecvt, char16_t>{}.from_bytes(u8);
}

inline
u32string codecvt_u8_to_u32(const string& u8)
{
    using codecvt = std::codecvt_utf8<char32_t>;
    return std::wstring_convert<codecvt, char32_t>{}.from_bytes(u8);
}

inline
string codecvt_u16_to_u8(const u16string& u16)
{
    using codecvt = std::codecvt_utf8_utf16<char16_t>;
    return std::wstring_convert<codecvt, char16_t>{}.to_bytes(u16);
}

inline
u32string codecvt_u16_to_u32(const u16string& u16)
{
    // You might expect std::codecvt_utf16<char32_t> to convert between
    // char16_t and char32_t, but it does not; rather, it operates on
    // UTF-16 encoded *byte* strings. This is not what we want.
    // We could try to reinterpret_cast<> our way around the problem, but this
    // is ugly and error prone. The easiest way is to do the conversion in two
    // steps, to UTF-8 and then to UTF-32. While this might be "unfair" on
    // codecvt for benchmark purposes, it does rather demonstrate what a
    // terrible API it is.
    const string u8 = codecvt_u16_to_u8(u16);
    return codecvt_u8_to_u32(u8);
}

inline
string codecvt_u32_to_u8(const u32string& u32)
{
    using codecvt = std::codecvt_utf8<char32_t>;
    return std::wstring_convert<codecvt, char32_t>{}.to_bytes(u32);
}

inline
u16string codecvt_u32_to_u16(const u32string& u32)
{
    // As above, to avoid reinterpret_cast<> and trying to pretend that
    // a UTF-16 string is really a UTF-16 byte string, we do this in two steps
    const string u8 = codecvt_u32_to_u8(u32);
    return codecvt_u8_to_u16(u8);
}

/*
 * All six cpputf8 conversion functions
 */

inline
u16string cpputf8_u8_to_u16(const string& u8)
{
    u16string u16;
    utf8::utf8to16(std::begin(u8), std::end(u8), std::back_inserter(u16));
    return u16;
}

inline
u32string cpputf8_u8_to_u32(const string& u8)
{
    u32string u32;
    utf8::utf8to32(std::begin(u8), std::end(u8), std::back_inserter(u32));
    return u32;
}

inline
string cpputf8_u16_to_u8(const u16string& u16)
{
    string u8;
    utf8::utf16to8(std::begin(u16), std::end(u16), std::back_inserter(u8));
    return u8;
}

inline
u32string cpputf8_u16_to_u32(const u16string& u16)
{
    // cpputf8 doesn't support this directly (it is, after all, designed to
    // handle UTF-8), so we need to do it in two steps
    const string u8 = cpputf8_u16_to_u8(u16);
    return cpputf8_u8_to_u32(u8);
}

inline
string cpputf8_u32_to_u8(const u32string& u32)
{
    string u8;
    utf8::utf32to8(std::begin(u32), std::end(u32), std::back_inserter(u8));
    return u8;
}

inline
u16string cpputf8_u32_to_u16(const u32string& u32)
{
    // As above, we need to do this in two steps
    const string u8 = cpputf8_u32_to_u8(u32);
    return cpputf8_u8_to_u16(u8);
}

/*
 * All six Boost.Locale conversion functions
 */

inline
u16string boost_u8_to_u16(const string& u8)
{
    return boost::locale::conv::utf_to_utf<char16_t>(u8);
}

inline
u32string boost_u8_to_u32(const string& u8)
{
    return boost::locale::conv::utf_to_utf<char32_t>(u8);
}

inline
string boost_u16_to_u8(const u16string& u16)
{
    return boost::locale::conv::utf_to_utf<char>(u16);
}

inline
u32string boost_u16_to_u32(const u16string& u16)
{
    return boost::locale::conv::utf_to_utf<char32_t>(u16);
}

inline
string boost_u32_to_u8(const u32string& u32)
{
    return boost::locale::conv::utf_to_utf<char>(u32);
}

inline
u16string boost_u32_to_u16(const u32string& u32)
{
    return boost::locale::conv::utf_to_utf<char16_t>(u32);
}

/*
 * All six range conversion functions
 */

inline
u16string range_u8_to_u16(const string& u8)
{
    return tcb::utf_ranges::to_u16string(u8);
}

inline
u32string range_u8_to_u32(const string& u8)
{
    return tcb::utf_ranges::to_u32string(u8);
}

inline
string range_u16_to_u8(const u16string& u16)
{
    return tcb::utf_ranges::to_u8string(u16);
}

inline
u32string range_u16_to_u32(const u16string& u16)
{
    return tcb::utf_ranges::to_u32string(u16);
}

inline
string range_u32_to_u8(const u32string& u32)
{
    return tcb::utf_ranges::to_u8string(u32);
}

inline
u16string range_u32_to_u16(const u32string& u32)
{
    return tcb::utf_ranges::to_u16string(u32);
}

/*
 * All six range view functions
 */

inline
u16string range_view_u8_to_u16(const string& u8)
{
    return tcb::utf_ranges::view::utf16(u8);
}

inline
u32string range_view_u8_to_u32(const string& u8)
{
    return tcb::utf_ranges::view::utf32(u8);
}

inline
string range_view_u16_to_u8(const u16string& u16)
{
    return tcb::utf_ranges::view::utf8(u16);
}

inline
u32string range_view_u16_to_u32(const u16string& u16)
{
    return tcb::utf_ranges::view::utf32(u16);
}

inline
string range_view_u32_to_u8(const u32string& u32)
{
    return tcb::utf_ranges::view::utf8(u32);
}

inline
u16string range_view_u32_to_u16(const u32string& u32)
{
    return tcb::utf_ranges::view::utf16(u32);
}

} // end anonymous namespace

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cout << "Usage: benchmark UTF8FILE [ITERATIONS]\n";
        return 1;
    }

    const string u8str = [argv] {
        std::ifstream f(argv[1]);
        return string(std::istreambuf_iterator<char>{f},
                      std::istreambuf_iterator<char>{});
    }();

    const u16string u16str = cpputf8_u8_to_u16(u8str);
    const u32string u32str = cpputf8_u8_to_u32(u8str);

    const int num_iterations = argc > 2 ? std::atoi(argv[2]) : 1;


    // UTF-8 to UTF-16
    time_function_call(codecvt_u8_to_u16, u8str, num_iterations, "codecvt u8 to u16");
    time_function_call(cpputf8_u8_to_u16, u8str, num_iterations, "cpputf8 u8 to u16");
    time_function_call(boost_u8_to_u16, u8str, num_iterations, "boost u8 to u16");
    time_function_call(range_u8_to_u16, u8str, num_iterations, "range u8 to u16");
    time_function_call(range_view_u8_to_u16, u8str, num_iterations, "range view u8 to u16");
    std::cout << "\n";

    // UTF-8 to UTF-32
    time_function_call(codecvt_u8_to_u32, u8str, num_iterations, "codecvt u8 to u32");
    time_function_call(cpputf8_u8_to_u32, u8str, num_iterations, "cpputf8 u8 to u32");
    time_function_call(boost_u8_to_u32, u8str, num_iterations, "boost u8 to u32");
    time_function_call(range_u8_to_u32, u8str, num_iterations, "range u8 to u32");
    time_function_call(range_view_u8_to_u32, u8str, num_iterations, "range view u8 to u32");
    std::cout << "\n";

    // UTF-16 to UTF-8
    time_function_call(codecvt_u16_to_u8, u16str, num_iterations, "codecvt u16 to u8");
    time_function_call(cpputf8_u16_to_u8, u16str, num_iterations, "cpputf8 u16 to u8");
    time_function_call(boost_u16_to_u8, u16str, num_iterations, "boost u16 to u8");
    time_function_call(range_u16_to_u8, u16str, num_iterations, "range u16 to u8");
    time_function_call(range_view_u16_to_u8, u16str, num_iterations, "range view u16 to u8");
    std::cout << "\n";

    // UTF-16 to UTF-32
    time_function_call(codecvt_u16_to_u32, u16str, num_iterations, "*codecvt u16 to u32");
    time_function_call(cpputf8_u16_to_u32, u16str, num_iterations, "*cpputf8 u16 to u32");
    time_function_call(boost_u16_to_u32, u16str, num_iterations, "boost u16 to u32");
    time_function_call(range_u16_to_u32, u16str, num_iterations, "range u16 to u32");
    time_function_call(range_view_u16_to_u32, u16str, num_iterations, "range view u16 to u32");
    std::cout << "\n";

    // UTF-32 to UTF-8
    time_function_call(codecvt_u32_to_u8, u32str, num_iterations, "codecvt u32 to u8");
    time_function_call(cpputf8_u32_to_u8, u32str, num_iterations, "cpputf8 u32 to u8");
    time_function_call(boost_u32_to_u8, u32str, num_iterations, "boost u32 to u8");
    time_function_call(range_u32_to_u8, u32str, num_iterations, "range u32 to u8");
    time_function_call(range_view_u32_to_u8, u32str, num_iterations, "range view u32 to u8");
    std::cout << "\n";

    // UTF-32 to UTF-16
    time_function_call(codecvt_u32_to_u16, u32str, num_iterations, "*codecvt u32 to u16");
    time_function_call(cpputf8_u32_to_u16, u32str, num_iterations, "*cpputf8 u32 to u16");
    time_function_call(boost_u32_to_u16, u32str, num_iterations, "boost u32 to u16");
    time_function_call(range_u32_to_u16, u32str, num_iterations, "range u32 to u16");
    time_function_call(range_view_u32_to_u16, u32str, num_iterations, "range view u32 to u16");
}