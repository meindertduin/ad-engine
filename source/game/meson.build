game_sources = files(
    'universe.cpp',
    'render_world.cpp',
    'transform.cpp',
    'terrain.cpp',
    'terrain_generator.cpp',
    'tile.cpp',
    'tile_set.cpp',
    'node.cpp',
    'scene.cpp'
)
project_sources += game_sources

engine_lib = static_library('engine', game_sources, include_directories: inc)
