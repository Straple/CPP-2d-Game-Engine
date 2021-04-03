#pragma once

#include "../utils.h"

#include "matrix.h"

//u8* build_dp_blend() {
//    u8* dp = new u8[256 * 256];
//
//    for (u32 alpha = 0; alpha <= 0xff; alpha++) {
//        for (u32 c = 0; c <= 0xff; c++) {
//            dp[(alpha << 8) + c] = alpha * c / 0xff;
//        }
//    }
//    return dp;
//}

// 0xAARRGGBB
// [a, r, g, b]
// [0..255]
struct Color {

    u8 b; // blue
    u8 g; // green
    u8 r; // red
    u8 a; // alpha

#define calc_color(alpha, c) (static_cast<u16>(alpha) * c / 255)


    Color() {
        r = g = b = a = 0;
    }
    Color(u8 red, u8 green, u8 blue, u8 alpha = 0xff) {
        r = calc_color(alpha, red);
        g = calc_color(alpha, green);
        b = calc_color(alpha, blue);
        a = alpha;
    }
    Color(u32 rgb, u8 alpha) {
        *reinterpret_cast<u32*>(this) = rgb;

        r = calc_color(alpha, r);
        g = calc_color(alpha, g);
        b = calc_color(alpha, b);
        a = alpha;
    }
    Color(u32 rgba) {
        *reinterpret_cast<u32*>(this) = rgba;

        r = calc_color(a, r);
        g = calc_color(a, g);
        b = calc_color(a, b);
    }

    // накладывает на этот цвет color
    Color combine(const Color& color) const {

        u8 inv_alpha = 0xff - color.a;

        Color result;
        result.a = 0xff - calc_color(inv_alpha, 0xff - a);
        result.r = color.r + calc_color(inv_alpha, r);
        result.g = color.g + calc_color(inv_alpha, g);
        result.b = color.b + calc_color(inv_alpha, b);

        return result;
    }

#undef calc_color

    operator u32() const {
        return static_cast<u32>(r << 16) + (g << 8) + b;
    }
};
