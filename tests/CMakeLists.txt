macro(NitroTest TEST)
    get_filename_component(TEST_NAME ${TEST} NAME_WE)

    set(TEST_NAME "Nitro.${TEST_NAME}")

    add_executable(${TEST_NAME} ${TEST})
    target_link_libraries(${TEST_NAME} Nitro::core)
    if(CMAKE_C_COMPILER_ID MATCHES "MSVC")
        target_compile_options(${TEST_NAME} PRIVATE /W4)
    else()
        target_compile_options(${TEST_NAME} PRIVATE -Wall -Wextra -pedantic)
    endif()

    add_test(${TEST_NAME} ${TEST_NAME})
endmacro()

include_directories(SYSTEM ${CMAKE_CURRENT_SOURCE_DIR}/Catch/include)

if(NOT CMAKE_C_COMPILER_ID MATCHES "MSVC")
    add_library(nitro_test_lib SHARED nitro_test_lib.cpp)

    NitroTest(dl_test.cpp)
    target_sources(Nitro.dl_test PRIVATE dl_test_other.cpp)
    set_tests_properties(Nitro.dl_test PROPERTIES ENVIRONMENT "LD_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}:$ENV{LD_LIBRARY_PATH}")
    target_link_libraries(Nitro.dl_test Nitro::dl)

    NitroTest(jiffy_test.cpp)
    target_link_libraries(Nitro.jiffy_test Nitro::jiffy)

    NitroTest(logging_jiffy_test.cpp)
    target_link_libraries(Nitro.logging_jiffy_test Nitro::log Nitro::jiffy)
endif()

NitroTest(env_get_test.cpp)
target_link_libraries(Nitro.env_get_test Nitro::env)
set_tests_properties(Nitro.env_get_test PROPERTIES ENVIRONMENT "TEST_1=THIS_WAS_SET")

NitroTest(broken_options_test.cpp)

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
