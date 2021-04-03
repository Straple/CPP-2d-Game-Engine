#pragma once

#include "../utils.h"
#include "matrix.h"

struct sprite {

    struct pixel_t {
        bool is_draw;
        u32 color;
    };

    matrix<pixel_t> picture;

    sprite() {}

    // считывание спрайта
    sprite(std::string path) {

        std::ifstream input(path);
        u32 height, width;
        input >> height >> width;
        picture = matrix<pixel_t>(height, width);

        if (path.substr(path.size() - 3) == "eng") {
            for (u32 i = 0; i < height; i++) {
                for (u32 j = 0; j < width; j++) {
                    s64 pixel;
                    input >> pixel;
                    if (pixel == -1) {
                        picture[i][j].is_draw = false;
                    }
                    else {
                        picture[i][j].is_draw = true;
                        picture[i][j].color = pixel;
                    }
                }
            }
        }
        else {

            u32 len;
            input >> len;
            while (len--) {
                u32 color, // цвет
                    point_count; // количество точек

                input >> color >> point_count;
                while (point_count--) {
                    u32 x, y;
                    input >> x >> y;
                    picture[y][x].is_draw = true;
                    picture[y][x].color = color;
                }
            }
        }
    }
};
