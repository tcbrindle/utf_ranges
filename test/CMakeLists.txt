
add_executable(utf_ranges_test
    bom_test.cpp
    bytes_test.cpp
    catch_main.cpp
    endian_test.cpp
    istreambuf_range_test.cpp
    line_end_transform_test.cpp
    ostreambuf_iterator_test.cpp
    utf_convert_view_test.cpp
    )

target_include_directories(utf_ranges_test PRIVATE
        ${RANGE_INCLUDE_DIR}
        ${Boost_INCLUDE_DIR}
        )
