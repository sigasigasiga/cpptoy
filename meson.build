# macos cmdline
# ```
# meson setup --cross-file brew-cross.ini bin
# ```
# 
# build benchmark
# meson setup -Dbenchmark=enabled bin
project('toy', 'cpp', default_options : [ 'cpp_std=c++23' ])

deps = [
  dependency('siga-util'),
  dependency('fmt'),
  dependency('range-v3'),
  dependency('tl-optional'),
  dependency(
    'boost',
    # modules: ['program_options'],
    required: false
  ),
]

executable(
  meson.project_name(),
  sources: ['src/app.cpp'],
  include_directories: ['src/'],
  dependencies: deps,
  cpp_pch: 'src/shared_pch.hpp'
)

if get_option('benchmark').enabled()
  executable(
    'benchmark-toy',
    sources: ['src/benchmark.cpp'],
    include_directories: ['src/'],
    dependencies: deps + dependency('benchmark-main'),
    cpp_pch: 'src/shared_pch.hpp'
  )
endif
