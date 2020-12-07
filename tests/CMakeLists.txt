macro(NitroTest TEST)
    get_filename_component(TEST_NAME ${TEST} NAME_WE)

    set(TEST_NAME "Nitro.${TEST_NAME}")

    add_executable(${TEST_NAME} ${TEST})
    target_link_libraries(${TEST_NAME} Nitro::core Catch2::Catch2)
    if(CMAKE_C_COMPILER_ID MATCHES "MSVC")
        target_compile_options(${TEST_NAME} PRIVATE /W4)
    else()
        target_compile_options(${TEST_NAME} PRIVATE -Wall -Wextra -pedantic)
    endif()

    add_test(${TEST_NAME} ${TEST_NAME})
endmacro()

add_library(catch2 INTERFACE)
target_include_directories(catch2 SYSTEM INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/Catch/single_include)
add_library(Catch2::Catch2 ALIAS catch2)


if(NOT CMAKE_C_COMPILER_ID MATCHES "MSVC")
    add_library(nitro_test_lib SHARED nitro_test_lib.cpp)

    NitroTest(dl_test.cpp)
    target_sources(Nitro.dl_test PRIVATE dl_test_other.cpp)
    set_tests_properties(Nitro.dl_test PROPERTIES ENVIRONMENT "LD_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}:$ENV{LD_LIBRARY_PATH}")
    target_link_libraries(Nitro.dl_test Nitro::dl)
endif()

NitroTest(env_get_test.cpp)
target_link_libraries(Nitro.env_get_test Nitro::env)
set_tests_properties(Nitro.env_get_test PROPERTIES ENVIRONMENT "TEST_1=THIS_WAS_SET")

NitroTest(better_options_test.cpp)
target_link_libraries(Nitro.better_options_test Nitro::better-options)
set_tests_properties(Nitro.better_options_test PROPERTIES ENVIRONMENT "OPT1=OPT1_VALUE;OPT2=TRUE;OPT3=FALSE;OPT4=OPT4_VALUE0\;OPT4_VALUE1")

NitroTest(tuple_operators_test.cpp)

NitroTest(reverse_test.cpp)

NitroTest(enumerate_test.cpp)

NitroTest(logging_test.cpp)
target_link_libraries(Nitro.logging_test Nitro::log)

NitroTest(string_ref_test.cpp)

NitroTest(catch_test.cpp)

NitroTest(quaint_ptr_test.cpp)

NitroTest(is_callable_test.cpp)

NitroTest(format_test.cpp)

NitroTest(string_test.cpp)
