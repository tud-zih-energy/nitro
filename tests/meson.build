# We use Catch2 as testing framework
catch2_dep = dependency('catch2', fallback : ['catch2', 'catch2_dep'])
catch_main = library('catch_main', 'catch_main.cpp',
  dependencies : catch2_dep
)
catch_main_obj = catch_main.extract_objects('catch_main.cpp')

# the test for nitro::env
env_test_exe = executable('nitro_env_test', 'env_get_test.cpp',
  objects : catch_main_obj,
  dependencies : [ nitro_env_dep, catch2_dep ]
)
test('nitro_env_test', env_test_exe, env : ['TEST_1=THIS_WAS_SET'])

# the test for nitro::options
options_test_exe = executable('nitro_options_test', 'options_test.cpp',
  objects : catch_main_obj,
  dependencies : [ nitro_options_dep, catch2_dep ]
)
test('nitro_options_test', options_test_exe,
  env : [
    'OPT1=OPT1_VALUE',
    'OPT2=TRUE',
    'OPT3=FALSE',
    'OPT4=OPT4_VALUE0;OPT4_VALUE1'
])

# the test for nitro::dl
dl_test_exe = executable('nitro_dl_test', 'dl_test.cpp', 'dl_test_other.cpp',
  objects : catch_main_obj,
  dependencies : [ nitro_dl_dep, catch2_dep ],
  export_dynamic : true
)
dl_test_lib = shared_library('nitro_test_lib', 'nitro_test_lib.cpp')
test('nitro_dl_test', dl_test_exe, env : ['LD_LIBRARY_PATH=tests'])

# the test for nitro::lang::enumerate
enumerate_test_exe = executable('nitro_enumerate_test', 'enumerate_test.cpp',
  objects : catch_main_obj,
  dependencies : [ nitro_core_dep, catch2_dep ]
)
test('nitro_enumerate_test', enumerate_test_exe)

# the test for nitro::meta::is_callable
callable_test_exe = executable('nitro_callable_test', 'is_callable_test.cpp',
  objects : catch_main_obj,
  dependencies : [ nitro_core_dep, catch2_dep ]
)
test('nitro_callable_test', callable_test_exe)

# the test for nitro::format
format_test_exe = executable('nitro_format_test', 'format_test.cpp',
  objects : catch_main_obj,
  dependencies : [ nitro_core_dep, catch2_dep ]
)
test('nitro_format_test', format_test_exe)

# the test for nitro::lang::reverse
reverse_test_exe = executable('nitro_reverse_test', 'reverse_test.cpp',
  objects : catch_main_obj,
  dependencies : [ nitro_core_dep, catch2_dep ]
)
test('nitro_reverse_test', reverse_test_exe)

# the test for nitro::lang::string
string_test_exe = executable('nitro_string_test', 'string_test.cpp',
  objects : catch_main_obj,
  dependencies : [ nitro_core_dep, catch2_dep ]
)
test('nitro_string_test', string_test_exe)

# the test for nitro::logging
logging_test_exe = executable('nitro_logging_test', 'logging_test.cpp',
  objects : catch_main_obj,
  dependencies : [ nitro_core_dep, catch2_dep ],
  cpp_args : '-DNITRO_LOG_MIN_SEVERITY=debug'
)
test('nitro_logging_test', logging_test_exe)