game_sources = files(
    'scene.cpp',
    'render_world.cpp',
    'transform.cpp',
    'terrain.cpp',
    'terrain_generator.cpp',
    'tile.cpp',
    'tile_set.cpp',
)
project_sources += game_sources

engine_lib = static_library('engine', game_sources, include_directories: inc)
