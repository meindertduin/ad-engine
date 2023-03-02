engine_sources = files(
    'logging.cpp',
    'hash.cpp',
    'window.cpp',
    'file_reader.cpp',
    #'allocator.cpp',
    #'engine.cpp',
    #'application.cpp',
)
project_sources += engine_sources

engine_lib = static_library('lua', engine_sources, include_directories: inc)
