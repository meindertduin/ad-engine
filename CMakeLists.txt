cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
        ad-engine
        VERSION 0.1.0
        DESCRIPTION "A 2D game engine"
        HOMEPAGE_URL "https://example.com/"
        LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Declare Library ----

# include(source/gfx/project-is-top-level.cmake)

# ---- Declare executable ----

add_executable(ad-engine_exe source/main.cpp)
add_executable(ad-engine::exe ALIAS ad-engine_exe)

set_property(TARGET ad-engine_exe PROPERTY OUTPUT_NAME ad-engine)

target_compile_features(ad-engine_exe PRIVATE cxx_std_20)

# target_link_libraries(ad-engine_exe PRIVATE gfx)
