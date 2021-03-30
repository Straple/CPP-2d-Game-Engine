#pragma once

#include "../utils.h"

#include "matrix.h"

matrix<u8> build_dp_blend() {
    matrix<u8> dp(256, 256);

    for (u32 alpha = 0; alpha <= 0xff; alpha++) {
        for (u32 c = 0; c <= 0xff; c++) {
            dp[alpha][c] = alpha * c / 0xff;
        }
    }
    return dp;
}

// d[alpha][c] = alpha * c / 255
static const matrix<u8> dp_blend = build_dp_blend();

// 0xAARRGGBB
// [a, r, g, b]
// [0..255]
struct Color {

    u8 b; // blue
    u8 g; // green
    u8 r; // red
    u8 a; // alpha


    Color() {
        r = g = b = 0;
        a = 255;
    }
    Color(u8 red, u8 green, u8 blue, u8 alpha = 255) {
        r = dp_blend[alpha][red];
        g = dp_blend[alpha][green];
        b = dp_blend[alpha][blue];;
        a = alpha;
    }
    Color(u32 color, u8 alpha) {
        *reinterpret_cast<u32*>(this) = color;

        a = alpha;

        r = dp_blend[alpha][r];
        g = dp_blend[alpha][g];
        b = dp_blend[alpha][b];
    }
    Color(u32 color) {
        *reinterpret_cast<u32*>(this) = color;
    }

    // накладывает на этот цвет color
    Color combine(const Color& color) const {

        u8 inv_alpha = 0xff - color.a;

        return 
             static_cast<u32>(color.b + dp_blend[inv_alpha][b]) +         // b
            (static_cast<u32>(color.g + dp_blend[inv_alpha][g]) << 8) +   // g
            (static_cast<u32>(color.r + dp_blend[inv_alpha][r]) << 16) +  // r
            (static_cast<u32>(0xff - dp_blend[inv_alpha][0xff - a]) << 24); // a
    }

    operator u32() const {
        return static_cast<u32>(r << 16) + (g << 8) + b;
    }
};