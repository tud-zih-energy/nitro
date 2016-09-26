macro(NitroTest TEST)
    get_filename_component(TEST_NAME ${TEST} NAME_WE)

    add_executable(${TEST_NAME} ${TEST})
    add_test(${TEST_NAME} ${TEST_NAME})
endmacro()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Catch/include)

add_library(nitro_test_lib SHARED nitro_test_lib.cpp)

NitroTest(dl_test.cpp)
set_tests_properties(dl_test PROPERTIES ENVIRONMENT "LD_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}:$ENV{LD_LIBRARY_PATH}")
target_link_libraries(dl_test ${CMAKE_DL_LIBS})

NitroTest(env_get_test.cpp)
set_tests_properties(env_get_test PROPERTIES ENVIRONMENT "TEST_1=THIS_WAS_SET")

NitroTest(broken_options_test.cpp)

NitroTest(tuple_operators_test.cpp)
