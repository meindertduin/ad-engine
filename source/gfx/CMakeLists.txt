add_library(
        gfx OBJECT
        render_pipeline.cpp
)

target_compile_features(gfx PUBLIC cxx_std_20)
