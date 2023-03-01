#include <cstdio>

#include "math/size.h"

int main() {
    math::Size2D size { 1, 2 };
    printf("%d %d\n", size.width(), size.height());
    return 0;
}