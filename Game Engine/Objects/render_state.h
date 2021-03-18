#pragma once

#include <windows.h>
#include "../utils.h"

struct Render_state {
    u32 height, width;

    Color* render_memory; // ������ ��� �������

    BITMAPINFO bitmap_info;

    // ��������� �������� ����
    void resize(u32 width, u32 height) {
        this->width = width;
        this->height = height;

        // ��������� � ������������� ������
        {
            if (render_memory) {
                delete[] render_memory;
            }
            u64 size = static_cast<u64>(width) * height;
            render_memory = new Color[size];
        }

        // ���������� bmiHeader
        {
            auto& header = bitmap_info.bmiHeader;
            header.biSize = sizeof(header);
            header.biWidth = width;
            header.biHeight = height;
            header.biPlanes = 1;
            header.biBitCount = 32;
            header.biCompression = BI_RGB;
        }
    }

};
