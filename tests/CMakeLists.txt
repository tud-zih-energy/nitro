macro(NitroTest TEST)
    get_filename_component(TEST_NAME ${TEST} NAME_WE)

    add_executable(${TEST_NAME} ${TEST})
    add_test(${TEST_NAME} ${TEST_NAME})
endmacro()

add_library(nitro_test_lib SHARED nitro_test_lib.cpp)

NitroTest(dl_test)
set_tests_properties(dl_test PROPERTIES ENVIRONMENT "LD_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}")
