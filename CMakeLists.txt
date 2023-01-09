cmake_minimum_required(VERSION 3.14)

project(
        ad-engine
        VERSION 0.1.0
        DESCRIPTION "A 2D game engine"
        HOMEPAGE_URL "https://example.com/"
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)

# ---- Declare Library ----

add_subdirectory(source/engine)
add_subdirectory(source/gfx)

# ---- Declare executable ----

add_executable(${PROJECT_NAME}
        source/main.cpp
        ${ENGINE_SOURCES}
        ${GFX_SOURCES}
        source/math/size.h source/platform/gcc.h)

set(HEADERS include)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer -D__extern_always_inline=inline -D_XOPEN_SOURCE_EXTENDED -DBX_CONFIG_DEBUG=0")

# --- Install submodules

target_include_directories(${PROJECT_NAME} PRIVATE source/)
target_link_directories(${PROJECT_NAME} PRIVATE source/engine/ source/gfx/)

target_include_directories(${PROJECT_NAME} PUBLIC external/bgfx/include external/bimg/include external/bx/include)

target_link_directories(${PROJECT_NAME} PRIVATE external/bgfx/.build/linux64_gcc/bin/)

target_link_libraries(${PROJECT_NAME}
        bgfxRelease bimgRelease bxRelease
        GL X11 SDL2 dl pthread rt
        )