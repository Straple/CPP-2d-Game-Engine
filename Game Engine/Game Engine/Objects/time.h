#pragma once

#include <windows.h>
#include "../utils.h"

// ������ ������� ���������� ����������
static inline u64 get_performance_frequency() {
    LARGE_INTEGER perf;

    if (!QueryPerformanceFrequency(&perf)) {
        throw "call to QueryPerformanceFrequency fails";
    }

    return perf.QuadPart;
}

const point_t performance_frequency = get_performance_frequency();

// ������ ������� ���
static inline u64 get_ticks() {
    LARGE_INTEGER ticks;

    if (!QueryPerformanceCounter(&ticks)) {
        throw "call to QueryPerformanceCounter fails";
    }

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
