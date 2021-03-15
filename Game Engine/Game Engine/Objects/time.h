#pragma once

#include <windows.h>
#include "../utils.h"

// вернет частоту обновления устройства
static inline u64 get_performance_frequency() {
    LARGE_INTEGER perf;

    if (!QueryPerformanceFrequency(&perf)) {
        throw "call to QueryPerformanceFrequency fails";
    }

    return perf.QuadPart;
}

const point_t performance_frequency = get_performance_frequency();

// вернет текущий тик
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

    // вернет время после reset|default constructor в секундах
    point_t time() const {

        // разница тиков делить на частоту
        return (get_ticks() - start_tick) / performance_frequency;
    }

    // обновит указатель времени
    void reset() {
        start_tick = get_ticks();
    }

    // вернет тик начала отсчета
    u64 get_tick() const {
        return start_tick;
    }
};
