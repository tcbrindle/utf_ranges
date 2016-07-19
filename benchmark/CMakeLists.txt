

find_package(Boost COMPONENTS locale)

if (Boost_FOUND)
    add_executable(benchmark benchmark.cpp)

    target_include_directories(benchmark PRIVATE
        ${RANGE_INCLUDE_DIR}
        ${Boost_INCLUDE_DIR}
    )
else()
    message("Boost.Locale not found, skipping benchmark target")
endif()