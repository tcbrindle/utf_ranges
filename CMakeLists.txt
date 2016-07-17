
cmake_minimum_required(VERSION 3.1)

project(range-utf-utils CXX)

set(CMAKE_CXX_STANDARD 14)

if (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Werror")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
endif() # UNIX

if (WIN32)
    if (CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Werror")
    endif()
    add_definitions("-DNOMINMAX")
endif()

find_package(Boost COMPONENTS system REQUIRED)

include_directories(include)
set(RANGE_INCLUDE_DIR "${range-utf-utils_SOURCE_DIR}/external/range-v3/include")

add_subdirectory(benchmark)
add_subdirectory(example)
add_subdirectory(test)




