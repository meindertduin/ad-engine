project('ad-enine', 'cpp',
    version: '0.1.0',
    default_options: ['cpp_std=c++20', 'default_library=static'])

project_sources = []
project_header_files = []

inc = [
    include_directories('source'),
    include_directories('external/xxh3'),
    include_directories('external/stb'),
]

sdl2_dep = dependency('sdl2')
lua_dep = dependency('lua')
glm_dep = dependency('glm')
glew_dep = dependency('glew')
jsoncpp_dep = dependency('jsoncpp')

subdir('source')
subdir('external')

bin_dep_libs = [fast_wfc_lib]

executable('adengine', project_sources,
    include_directories: inc,
    dependencies: [sdl2_dep, lua_dep, glm_dep, glew_dep, jsoncpp_dep],
    link_with: bin_dep_libs)

