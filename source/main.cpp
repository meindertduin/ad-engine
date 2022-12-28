/*
 * Copyright 2011-2020 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
 */

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

auto main() -> int {
    bgfx::PlatformData pd;

    bgfx::init();

    bgfx::shutdown();
    return 0;
}