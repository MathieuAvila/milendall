#pragma once

#include <chrono>

struct FpsCounter {

    const float FPS_TIME_COUNT_SEC = 5.0f;

    int fps_count = 0;
    std::chrono::_V2::steady_clock::time_point  fps_start;
    float fps_last_counter = 0.0f;

    FpsCounter();
    void update();
};
