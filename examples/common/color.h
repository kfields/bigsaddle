#pragma once

#include <cstdint>

struct ColorRgba {
    ColorRgba() : r(255), g(255), b(255), a(255) {}
    ColorRgba(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a)
        : r(_r), g(_g), b(_b), a(_a) {}
    ColorRgba(uint32_t _c) : c(_c) {}
    ColorRgba(float _r, float _g, float _b, float _a)
        : r(int(_r * 255.0f)), g(int(_g * 255.0f)), b(int(_b * 255.0f)), a(int(_a * 255.0f)) {}
    ColorRgba(const float* arr)
        : ColorRgba(arr[0], arr[1], arr[2], arr[3]) {}

    union {
        struct {
            uint8_t r;
            uint8_t g;
            uint8_t b;
            uint8_t a;
        };
        uint32_t c;
    };
};
