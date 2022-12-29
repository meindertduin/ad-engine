./external/bgfx/.build/linux64_gcc/bin/shadercRelease \
    -f ./shaders/v_simple.sc \
    -o ./shaders/build/v_simple.bin \
    --varyingdef ./shaders/varying.def.sc \
    --platform linux \
    --type vertex \
    --verbose \
    -i external/bgfx/src

./external/bgfx/.build/linux64_gcc/bin/shadercRelease \
    -f ./shaders/f_simple.sc \
    -o ./shaders/build/f_simple.bin \
    --varyingdef ./shaders/varying.def.sc \
    --platform linux \
    --type fragment \
    --verbose \
    -i external/bgfx/src
