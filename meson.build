project('nitro', 'cpp',
  version : '2.0',
  default_options : ['warning_level=3', 'cpp_std=c++17'])

inc_dir = include_directories('include')

nitro_core_dep = declare_dependency(
  include_directories : inc_dir
)

env_lib = static_library('nitro-env', 'src/env/get.cpp', 'src/env/hostname.cpp', 'src/env/process.cpp',
  install : true,
  gnu_symbol_visibility : 'hidden',
  dependencies : nitro_core_dep
)

nitro_env_dep = declare_dependency(
  link_with : env_lib,
  dependencies : nitro_core_dep
)

options_lib = static_library('nitro-options',
  'src/options/parser.cpp',
  'src/options/group.cpp',
  'src/options/option.cpp',
  'src/options/toggle.cpp',
  'src/options/multi_option.cpp',
  install : true,
  dependencies : nitro_env_dep
)

nitro_options_dep = declare_dependency(
  link_with : options_lib,
  dependencies : nitro_env_dep
)

nitro_dl_dep = declare_dependency(
  dependencies : [
    nitro_core_dep,
    meson.get_compiler('cpp').find_library('dl')
])

nitro_dep = declare_dependency(
  dependencies : [nitro_core_dep, nitro_env_dep, nitro_options_dep, nitro_dl_dep]
)

subdir('tests')

install_subdir('include/nitro', install_dir : 'include')

# pkg_mod = import('pkgconfig')
# pkg_mod.generate(
#   name : 'nitro',
#   filebase : 'nitro',
#   description : 'Collection of various, modern C++ libraries and helpers.',
#   subdirs : 'nitro',
#   libraries : env_lib,
#   version : '1.0',
# )
