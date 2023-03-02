#include <cstdio>

#include "math/size.h"
#include "engine/window.h"

constexpr int WIDTH = 640;
constexpr int HEIGHT = 480;

int main(int argc, char **argv) {
    WindowOptions options { .title = "Application", .size = { WIDTH, HEIGHT } };
    AdWindow window { options };

    math::Size2D size { 1, 2 };
    printf("%d %d\n", size.width(), size.height());

    return 0;
}