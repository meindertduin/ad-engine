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

# ---- Declare executable ----

add_executable(${PROJECT_NAME} source/main.cpp)

set(HEADERS include)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer -D__extern_always_inline=inline -D_XOPEN_SOURCE_EXTENDED")

# --- Install submodules

target_include_directories(${PROJECT_NAME} PUBLIC external/bgfx/include external/bimg/include external/bx/include)

target_link_directories(${PROJECT_NAME} PRIVATE external/bgfx/.build/linux64_gcc/bin/)

target_link_libraries(${PROJECT_NAME}
        GL X11 ${CMAKE_DL_LIBS} pthread
        bgfxRelease bimgRelease bxRelease
        )