#pragma once

#include "../utils.h"
#include "color.h"

// ������ ���������� ��� ����������
class quad_tree_render {
    struct node {
        char type; // 0 - ������, 1 - ����, 2 - need push
        Color color;

        node() {
            type = 1;
        }
        node(const Color& color, char type) {
            this->color = color;
            this->type = type;
        }

        void update(const Color& new_color) {
            if (type != 0) { // ������� ���� ��� ��������� � �������������
                color = color.combine(new_color);
            }
            else {
                color = new_color;
                type = 2; // ���� ��� ���-�� ����, ������� ���� ���� ��������� � ����������
            }
        }
    };

    u32 height, width;
    node* tree;


    void push(u32 v, u32 left, u32 right, u32 top, u32 bottom) {
        // ����� ����������� � !(��� ��������� ������� ��� ������ � ��������)
        if (tree[v].type != 0 && !(left >= right && top >= bottom)) {
            if (tree[v].type == 1) { // ��� ����

                if (left == right) {
                    // ��� ������������ �����
                    tree[(v << 2) + 1] = tree[(v << 2) + 3] = tree[v];
                }
                else if (top == bottom) {
                    // ��� �������������� �����
                    tree[(v << 2) + 1] = tree[(v << 2) + 2] = tree[v];
                }
                else {
                    // ��� ���� �������������
                    for (u32 i = 1; i <= 4; i++) {
                        tree[(v << 2) + i] = tree[v];
                    }
                }
            }
            else if (tree[v].type == 2) { // ����� �����������
                if (left == right) {
                    // ��� ������������ �����
                    tree[(v << 2) + 1].update(tree[v].color);
                    tree[(v << 2) + 3].update(tree[v].color);
                }
                else if (top == bottom) {
                    // ��� �������������� �����
                    tree[(v << 2) + 1].update(tree[v].color);
                    tree[(v << 2) + 2].update(tree[v].color);
                }
                else {
                    // ��� ���� �������������
                    for (u32 i = 1; i <= 4; i++) {
                        tree[(v << 2) + i].update(tree[v].color);
                    }
                }
            }

            tree[v].type = 0; // ������ ��� ������ ����
        }
    }

#define cur_quest qleft, qright, qtop, qbottom, color // ������� ������� ������� � ����

#define top_left (v << 2) + 1, left, xmid, top, ymid // ������� � ������ �������� ����

#define bottom_left (v << 2) + 3, left, xmid, ymid + 1, bottom // ������� � ������ ������� ����

#define top_right (v << 2) + 2, xmid + 1, right, top, ymid // ������� � ������� �������� ����

#define bottom_right (v << 2) + 4, xmid + 1, right, ymid + 1, bottom // ������� � ������� ������� ����

    void paint(u32 v, // vertex
        u32 left, u32 right, // x border
        u32 top, u32 bottom, // y border
        u32 qleft, u32 qright, // quest x border
        u32 qtop, u32 qbottom, // quest y border
        const Color& color) { // color

        if (left == qleft && right == qright &&  // x
            top == qtop && bottom == qbottom) {// y

            // ���� � ���� ������� ��������� ����������� �����
            if (tree[v].type != 0) {
                tree[v].color = tree[v].color.combine(color);
            }
            else {
                tree[v].color = color;
                if (color.a == 255) {
                    tree[v].type = 1; // is leaf
                }
                else {
                    tree[v].type = 2; // need push
                }
            }
        }
        else {
            push(v, left, right, top, bottom);

            u32 xmid = (left + right) >> 1; // ����� �� x
            u32 ymid = (top + bottom) >> 1; // ����� �� y

            if (qright <= xmid) { // left

                if (qbottom <= ymid) { // top
                    // top left
                    paint(top_left, cur_quest);
                }
                else if (ymid < qtop) { // bottom
                    // bottom left
                    paint(bottom_left, cur_quest);
                }
                else { // top and bottom
                    paint(top_left, qleft, qright, qtop, ymid, color);
                    paint(bottom_left, qleft, qright, ymid + 1, qbottom, color);
                }
            }
            else if (xmid < qleft) {  // right

                if (qbottom <= ymid) { // top
                    // top left
                    paint(top_right, cur_quest);
                }
                else if (ymid < qtop) { // bottom
                    // bottom left
                    paint(bottom_right, cur_quest);
                }
                else { // top and bottom
                    paint(top_right, qleft, qright, qtop, ymid, color);
                    paint(bottom_right, qleft, qright, ymid + 1, qbottom, color);
                }
            }
            else { // left and right

                if (qbottom <= ymid) { // top_left and top_right
                    paint(top_left, qleft, xmid, qtop, qbottom, color);
                    paint(top_right, xmid + 1, qright, qtop, qbottom, color);
                }
                else if (ymid < qtop) { // bottom_left and bottom_right
                    paint(bottom_left, qleft, xmid, qtop, qbottom, color);
                    paint(bottom_right, xmid + 1, qright, qtop, qbottom, color);
                }
                else { // all
                    paint(top_left, qleft, xmid, qtop, ymid, color);
                    paint(top_right, xmid + 1, qright, qtop, ymid, color);

                    paint(bottom_left, qleft, xmid, ymid + 1, qbottom, color);
                    paint(bottom_right, xmid + 1, qright, ymid + 1, qbottom, color);
                }
            }
        }
    }


    void copy_colors(u32 v, // vertex
        u32 left, u32 right, // x border
        u32 top, u32 bottom, // y border
        u32* dest) { // ���� ����������

        if (tree[v].type == 1) {
            // ������ � ����. �� ��������� ������ ������ ������������� � �����-�� ����

            u32 color = static_cast<u32>(tree[v].color);
            u32 len = right - left + 1;

            dest += top * width + left;
            for (; top <= bottom; top++) {
                for (u32 k = 0; k < len; k++) {
                    dest[k] = color;
                }
                dest += width;
            }
        }
        else {
            push(v, left, right, top, bottom);

            u32 xmid = (left + right) >> 1; // ����� �� x
            u32 ymid = (top + bottom) >> 1; // ����� �� y

            copy_colors(top_left, dest);
            copy_colors(top_right, dest);
            copy_colors(bottom_left, dest);
            copy_colors(bottom_right, dest);
        }
    }

    void zeroing() {
        height = width = 0;
        tree = nullptr;
    }

public:

    void clear() {
        if (tree) {
            delete[] tree;
        }
        height = width = 0;
    }

    quad_tree_render() {
        height = width = 0;
        tree = nullptr;
    }
    ~quad_tree_render() {
        clear();
    }

    void resize(u32 height, u32 width) {
        clear();

        this->height = height;
        this->width = width;

        u64 len = std::max(round_two(height), round_two(width));
        tree = new node[4 * len * len];
    }

    void paint(u32 left, u32 right, u32 top, u32 bottom, const Color& color) {
        paint(1, 0, width - 1, 0, height - 1, left, right, top, bottom, color);
    }

    void copy_colors(u32* dest) {
        copy_colors(1, 0, width - 1, 0, height - 1, dest);
    }
};
