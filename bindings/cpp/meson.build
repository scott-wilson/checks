project(
  'cppopenchecks',
  'cpp',
  version: '0.1.0',
  default_options: ['warning_level=3', 'cpp_std=c++17'],
)

cargo_toml = meson.project_source_root() / '../c/Cargo.toml'
cargo = find_program('cargo')
cargo_wrapper = find_program(meson.project_source_root() / '../c/meson/cargo_wrapper.py', native: true)
cc = meson.get_compiler('cpp')
host_system = host_machine.system()
is_msvc_style = cc.get_argument_syntax() == 'msvc' or cc.get_define('_MSC_VER') != ''
release_mode = get_option('debug') ? 'debug' : 'release'
build_tests = get_option('tests')
build_fuzz = get_option('fuzz')

if host_system == 'windows'
  lib_prefix = is_msvc_style ? '' : 'lib'
  dynamic_ext = 'dll'
  static_ext = is_msvc_style ? 'lib' : 'a'
elif host_system in ['darwin', 'ios']
  lib_prefix = 'lib'
  dynamic_ext = 'dylib'
  static_ext = 'a'
else
  lib_prefix = 'lib'
  dynamic_ext = 'so'
  static_ext = 'a'
endif

subdir('include')

# HACK: This is copy of the C custom target, but just the static part of the library.
# Doing this until there's a better solution for having subprojects outside of the expected Meson way of doing things.
rust_c_artifacts = custom_target(
  meson.project_name(),
  build_by_default: true,
  output: ['@0@copenchecks.@1@'.format(lib_prefix, static_ext)],
  console: true,
  install: false,
  command: [
    cargo_wrapper,
    'build',
    cargo,
    '--release-mode', release_mode,
    '--manifest-path', cargo_toml,
    '--project-build-root', meson.project_build_root(),
    '--current-build-dir', meson.current_build_dir(),
  ],
)
copenchecks_include_dirs = include_directories('../c/include')
copenchecks_dep = declare_dependency(
  include_directories: copenchecks_include_dirs,
  link_with: rust_c_artifacts[0],
  version: meson.project_version(),
)

cppopenchecks_lib = library(
  meson.project_name(),
  'src' / 'core.cpp',
  install: true,
  include_directories: include_dirs,
  dependencies: [copenchecks_dep],
)

pkg = import('pkgconfig')
pkg.generate(
  name: meson.project_name(),
  description: 'A simple checks framework',
  libraries: [cppopenchecks_lib],
)

cmake = import('cmake')
cmake.write_basic_package_version_file(
  name: meson.project_name(),
  version: meson.project_version(),
)
conf = configuration_data()
conf.set_quoted(
  'PACKAGE_INCLUDE_INSTALL_DIR',
  '${PACKAGE_PREFIX_DIR}' / get_option('includedir'),
)
cmake.configure_package_config_file(
  name: meson.project_name(),
  input: 'cmake/cppopenchecks.cmake.in',
  configuration: conf,
)

cppopenchecks_dep = declare_dependency(
  include_directories: include_directories('include'),
  link_with: cppopenchecks_lib,
)

if build_tests
  subdir('tests')
endif

if build_fuzz
  subdir('fuzz')
endif
