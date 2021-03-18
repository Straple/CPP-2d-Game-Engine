#pragma once

#include "../utils.h"

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
        r = u32(red) * alpha / 255;
        g = u32(green) * alpha / 255;
        b = u32(blue) * alpha / 255;
        a = alpha;
    }
    Color(u32 color, u8 alpha) {
        *reinterpret_cast<u32*>(this) = color;

        a = alpha;

        r = u32(r) * alpha / 255;
        g = u32(g) * alpha / 255;
        b = u32(b) * alpha / 255;
    }
    Color(u32 color) {
        *reinterpret_cast<u32*>(this) = color;
    }

    // накладывает на этот цвет color
    Color combine(const Color& color) const {
        if (color.a == 255) { // полностью не прозрачен
            return color;
        }
        else {
            Color result;

            u32 inv_alpha = 255 - color.a;

            result.a = 255 - (inv_alpha * (255 - a) / 255);

            result.r = color.r + (inv_alpha * r / 255);
            result.g = color.g + (inv_alpha * g / 255);
            result.b = color.b + (inv_alpha * b / 255);


            return result;
        }
    }

    operator u32() const {
        return (static_cast<u32>(r << 16) + (g << 8) + b);
    }
};

