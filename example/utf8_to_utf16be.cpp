
#include <iostream>
#include <fstream>

#include <tcb/ranges/view.hpp>
#include <range/v3/istream_range.hpp>
#include <range/v3/view/bounded.hpp>

namespace rng = ::ranges::v3;
namespace utf = ::tcb::ranges;

int main(int argc, char** argv)
{
    if (argc < 3) {
        std::cout << "Usage:\n"
                  << "utf8_to_utf16be INFILE OUTFILE\n"
                  << "\n"
                  << "Converts a UTF-8 encoded file to big-endian UTF-16.\n";
        return 1;
    }

    std::ifstream in_file{argv[1], std::ios::binary};
    std::ofstream out_file{argv[2], std::ios::binary};

    std::string src(std::istreambuf_iterator<char>{in_file},
                    std::istreambuf_iterator<char>{});

    //auto src =

    auto view = src
            | utf::view::consume_bom // Remove UTF-8 "BOM" if present
            | utf::view::as_utf16    // Convert to UTF-16
            | utf::view::add_bom     // Prepend UTF-16 BOM to start of range
            | utf::view::endian_convert<boost::endian::order::big>; // Convert to big-endian

    std::u16string out_str = view;

    out_file.write(reinterpret_cast<const char*>(out_str.data()),
                   out_str.size() * sizeof(char16_t));
}