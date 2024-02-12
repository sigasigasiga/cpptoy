project('toy', 'cpp', default_options : [ 'cpp_std=c++23' ])

sources = [
  'src/main.cpp',
]

executable(
  meson.project_name(),
  sources,
  include_directories: ['src/'],
  cpp_pch: 'src/shared_pch.hpp'
)
