# macos cmdline
# ```
# CC=clang CXX=clang++ LDFLAGS="-L/opt/homebrew/opt/llvm/lib/c++ -Wl,-rpath,/opt/homebrew/opt/llvm/lib/c++" meson setup bin
# ```
project('toy', 'cpp', default_options : [ 'cpp_std=c++23' ])

boost_dep = dependency(
  'boost',
  # modules: ['program_options'],
  required: false
)
fmt_dep = dependency('fmt')
ranges_dep = dependency('range-v3')

sources = [
  'src/app.cpp',
]

executable(
  meson.project_name(),
  sources,
  include_directories: ['src/'],
  dependencies: [boost_dep, fmt_dep, ranges_dep],
  cpp_pch: 'src/shared_pch.hpp'
)
