#include <cstdio>

#include "math/size.h"
#include "engine/logging.h"

int main(int argc, char **argv) {
    math::Size2D size { 1, 2 };
    printf("%d %d\n", size.width(), size.height());
    Logger::info("Hello, world!");

    return 0;
}