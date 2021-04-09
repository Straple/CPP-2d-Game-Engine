#pragma once

#include "dot.h"
#include "../../utils.h"


// a, b, c
// ax * by + c = 0
class Line {
    point_t a, b, c;
    // a, b �������������!

    // �� ��������� ������ a, b, c �������
    // ����� �������� ������� � �������

public:

    point_t get_a() const {
        return a;
    }
    point_t get_b() const {
        return b;
    }
    point_t get_c() const {
        return c;
    }

    Line() {
        a = b = c = 0;
    }
    Line(const dot& begin, const dot& end) {
        a = begin.y - end.y;
        b = end.x - begin.x;
        // normalize
        {
            dot ab = dot(a, b).normalize();
            a = ab.x;
            b = ab.y;
        }
        c = -a * begin.x - b * begin.y;
    }
    Line(point_t A, point_t B, point_t C) {
        a = A;
        b = B;
        c = C;
        // normalize
        {
            point_t d = dot(a, b).getLen();
            if (d != 0) {
                a /= d;
                b /= d;
                c /= d;
            }
        }
    }

    // ���������� ������������� �� �����
    Line getPerp(const dot& point) const {
        point_t A = -b, B = a;
        point_t C = -A * point.x - B * point.y;
        return Line(A, B, C);
    }

    // ���������� ������������ ������ �� ���������� dist
    // ���� ��� ����� ������������, �� ������ � ������ �������
    Line getParallel(point_t dist) const {
        return Line(a, b, c + dist /* * sqrt(a * a + b * b)*/);
    }

    // ���������� ��������������� ������ ������ ���������� �� mult
    dot getVector(point_t mult = 1) const {
        return dot(-b, a)/*.normalize()*/ * mult;
    }

    // ���������� ����� ����������� ���� ������
    dot intersect(const Line& Rhs) const {
        point_t x, y;
        // ax + by + c = 0
        // by = -c - ax
        // y  = (-ax - c) / b

        if (Rhs.b != 0) {
            x = (b * Rhs.c / Rhs.b - c) / (a - b * Rhs.a / Rhs.b);
            y = (-x * Rhs.a - Rhs.c) / Rhs.b;
        }
        else {
            x = -Rhs.c / Rhs.a;
            y = (-x * a - c) / b;
        }
        return dot(x, y);
    }

    // ���������� ����� ����������� ��������������
    dot perpIntersect(const dot& point) const {
        return intersect(getPerp(point));
    }

    // �������� ����� �� ������
    std::vector<dot> reflection(const std::vector<dot>& Dots) const {
        std::vector<dot> Result(Dots.size());
        for (int i = 0; i < Result.size(); i++) {
            Result[i] = Dots[i] + (perpIntersect(Dots[i]) - Dots[i]) * 2;
        }
        return Result;
    }

    // ���������� ����� ��������������
    point_t dist(const dot& point) const {
        return abs(a * point.x + b * point.y + c) /* / std::sqrt(a * a + b * b)*/;
    }
    // ���������� ���������� ����� ������������� �������
    point_t dist(const Line& parallel) const {
        return abs(c - parallel.c) /* / sqrt(a * a + b * b)*/;
    }

    bool isParallel(const Line& Rhs) const {
        return a * Rhs.b - b * Rhs.a == 0;
    }
    bool isPerp(const Line& Rhs) const {
        return a * Rhs.a + b * Rhs.b == 0;
    }
    bool ison(const dot& point) const {
        return a * point.x + b * point.y + c == 0;
    }

    bool operator == (const Line& Rhs) const {
        // 1 1 1
        // 1 0 -1
        // 0 1 -1
        // 0 0 1
        point_t sign;
        if ((a < 0) ^ (Rhs.a < 0)) {
            sign = -1;
        }
        else {
            sign = 1;
        }

        return (a == sign * Rhs.a) && (b == sign * Rhs.b) && (c == sign * Rhs.c);
    }
};
