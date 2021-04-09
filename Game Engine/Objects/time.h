#pragma once

#include <windows.h>
#include "../utils.h"

// ������ ������� ���������� ����������
u64 get_performance_frequency() {
    LARGE_INTEGER perf;

    ASSERT(QueryPerformanceFrequency(&perf), "call to QueryPerformanceFrequency fails");

    return perf.QuadPart;
}

const point_t performance_frequency = get_performance_frequency();

// ������ ������� ���
u64 get_ticks() {
    LARGE_INTEGER ticks;

    ASSERT(QueryPerformanceCounter(&ticks), "call to QueryPerformanceCounter fails");

    return ticks.QuadPart;
}

class Timer {

    u64 start_tick;

public:

    Timer() {
        reset();
    }

    // ������ ����� ����� reset|default constructor � ��������
    point_t time() const {

        // ������� ����� ������ �� �������
        return (get_ticks() - start_tick) / performance_frequency;
    }

    // ������� ��������� �������
    void reset() {
        start_tick = get_ticks();
    }

    // ������ ��� ������ �������
    u64 get_tick() const {
        return start_tick;
    }
};
