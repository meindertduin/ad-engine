game_sources = files(
    'scene.cpp',
    'render_world.cpp',
    'transform.cpp'
)
project_sources += game_sources

engine_lib = static_library('engine', game_sources, include_directories: inc)
