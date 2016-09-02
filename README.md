
# utf_ranges

A collection of Unicode utilities for C++ using [Range-V3](https://github.com/ericniebler/range-v3)

This header-only library contains facilities for transforming between UTF-8, UTF-16 and UTF-32 encoded strings (eagerly and lazily), as well as dealing with byte-order marks and transforming line endings.

## Example

A quick overview is best supplied by an example. The following reads a UTF-8 encoded input stream and outputs a UTF-16BE byte stream with byte-order mark:

```cpp
    namespace rng = ::ranges::v3;
    namespace utf = ::tcb::utf_ranges;

    std::ifstream in_file{"input_file.utf8.txt", std::ios::binary};
    std::ofstream out_file{"output_file.utf16be.txt", std::ios::binary};

    auto view = utf::istreambuf(in_file) // Read range from input stream
            | utf::view::consume_bom     // Remove UTF-8 "BOM" if present
            | utf::view::utf16           // Convert to UTF-16
            | utf::view::add_bom         // Prepend UTF-16 BOM to start of range
            | utf::view::endian_convert<boost::endian::order::big> // Convert to big-endian
            | utf::view::bytes;          // Write to disk as bytes

    rng::copy(view, utf::ostreambuf_iterator<char>{out_file}); // Do the copy
```

(see example/utf8_to_utf16be.cpp for the full code).

## Conversions

For "eager" encoding conversions, the library broadly follows the API specified in [Beman Dawes' proposed Unicode conversion library](https://github.com/Beman/unicode/tree/std-proposal), albeit (currently) with simplified error handling (invalid Unicode characters are simply replaced by the Unicode replacement character U+FFFD). The actual conversion uses code taken from Boost.Locale.

To convert a range of characters between UTF-8, UTF-16 or UTF-32, use the `tcb::utf_ranges::utf_convert()` function. This takes an `InputRange` with a value type that is an arithmetic type of size 1, 2 or 4 bytes (for UTF-8, UTF-16 and UTF-32 respectively), and an `OutputIterator` with a value type similarly defined. For example:

```cpp
std::string in = u8"Hello world";
std::u16string out;
// Note that the output type cannot be determined automatically, so must be specified
tcb::utf_ranges::utf_convert<char16_t>(in, std::back_inserter(out));
```

To tranform directly to a new string, the `to_utf_string()` function is supplied:

```cpp
std::u16string in = u"Hello world";
std::string out = tcb::utf_ranges::to_utf_string<char>(in);
```

Convenience functions `to_u8string()`, `to_u16string()`, `to_u32string()` and `to_wstring()` are also provided (but please don't use the last one):

```cpp
std::u32string in = U"Hello world";
std::u16string out = tcb::utf_ranges::to_u16string(in);
```

## Views

If you're familiar with Range-V3, you'll know that views perform lazy transformations on a given range -- that is, conversion is done one element at a time when the view is iterated over.

### Encoding conversions

This library provides views which lazily perform the same transformations as above. For consistency with Range-V3, these are in the `view::` sub-namespace.

```cpp
std::u16string in = u"Hello world";

auto view = tcb::utf_ranges::view::utf8(in);

ranges::v3::copy(view, std::ostream_iterator<char>(std::cout));
```

There are similar `utf16` and `utf32` views

### Endian transformations

For UTF-16 and UTF-32, the library provides views which perform byte-swapping between native-, big- and little-ending representations, using code from Boost. The output endianness is specifed by a template parameter, and the input endianness is passed as an argument to the constructor. Both default to `boost::endian::native`. For example:

```
std::u16string in = u"Hello world"; // native endian
auto view = tcb::utf_ranges::view::endian_convert<boost::endian::order::big>(in);
std::vector<std::int16_t> out = view; // Copy byte-swapped values to vector
```

### Byte order mark handling

The library provides two views for dealing with "byte order marks", that is, the Unicode non-breaking space character U+FEFF which is often placed at the start of files to allow the endianness to be detected.

To detect a byte-order mark, using the `consume_bom` view:

```
std::u16string in = u"\uFEFFHello world"; // native-endian UTF-16 with BOM
auto view = tcb::utf_ranges::view::consume_bom(in);
std::u16string out = view; // copy to new string with BOM removed
```

As suggested by the name, the byte order mark is removed if present. If a BOM is found an has non-native endianness, endian conversion is automatically performed -- that is, the output of the view will always be native-endian. For UTF-8, if a BOM is detected it is simply removed. If no BOM is present, the string is assumed to be native-endian (for UTF-16 and -32), and is passed through unchanged.

To place a byte-order mark at the start of a string, use the `add_bom` view:

```
std::u16string in = u"Hello world";
auto view = tcb::utf_ranges::view::add_bom(in);
std::u16string out = view; // copy to new string, with BOM prepended
```

### Line ending transformation

Unicode specifies eight possible line endings, and recommends that these are converted to the machine native line ending representation on input. In C++, the native representation is "\n". The `line_end_transform` view performs such a conversion. For example:

```cpp
std::string in = u8"Hello world\r\n"; // Windows-style
std::string out = tcb::utf_ranges::view::line_end_transform(in);
assert(out == "Hello world\n");
```

### Chaining views

As with Range-V3, `operator|` is overloaded for views, allowing them to be easily concatenated together, as in the example above.

## Licence

This library is provided under the Boost licence. See LICENCE_1_0.txt for details.
