
add_executable(utf8_to_utf16be utf8_to_utf16be.cpp)
target_include_directories(utf8_to_utf16be PRIVATE
        ${RANGE_INCLUDE_DIR}
        ${Boost_INCLUDE_DIR})