
add_executable(unicode_ranges_test
    bom_test.cpp
    bytes_test.cpp
    catch_main.cpp
    endian_test.cpp
    istreambuf_range_test.cpp
    line_end_transform_test.cpp
    ostreambuf_iterator_test.cpp
    view_test.cpp
    )

target_include_directories(unicode_ranges_test PRIVATE ${RANGE_INCLUDE_DIR})
