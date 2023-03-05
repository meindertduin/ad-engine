gpu_sources = files('gpu.h')

subdir('opengl')

project_sources += gpu_sources

gpu_lib = static_library('gpu', gpu_sources, include_directories: inc)
