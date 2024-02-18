project('toy', 'cpp', default_options : [ 'cpp_std=c++23' ])

boost_dep = dependency('boost')
fmt_dep = dependency('fmt')
ranges_dep = dependency('range-v3')

sources = [
  'src/main.cpp',
]

executable(
  meson.project_name(),
  sources,
  include_directories: ['src/'],
  dependencies: [boost_dep, fmt_dep, ranges_dep],
  cpp_pch: 'src/shared_pch.hpp'
)
