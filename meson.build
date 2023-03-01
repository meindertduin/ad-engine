project('ad-enine', 'cpp',
    version: '0.1.0',
    default_options: ['cpp_std=c++20'])

project_sources = []
project_header_files = []

inc = [
    include_directories('source'),
]

subdir('source')

executable('adengine', main_source,
    include_directories: inc)

e = executable('sample_cpp_project_bin',
  main_source,
  include_directories : inc)
