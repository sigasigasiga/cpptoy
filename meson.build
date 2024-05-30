# macos cmdline
# ```
# CC=clang CXX=clang++ LDFLAGS="-L/opt/homebrew/opt/llvm/lib/c++ -Wl,-rpath,/opt/homebrew/opt/llvm/lib/c++" meson setup bin
# ```
project('toy', 'cpp', default_options : [ 'cpp_std=c++23' ])

sources = [
  'src/app.cpp',
]

deps = [
  dependency('fmt'),
  dependency('range-v3'),
  dependency('tl-optional'),
  dependency(
    'boost',
    # modules: ['program_options'],
    required: false
  )
]

executable(
  meson.project_name(),
  sources,
  include_directories: ['src/'],
  dependencies: deps,
  cpp_pch: 'src/shared_pch.hpp'
)
