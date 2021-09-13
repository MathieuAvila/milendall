#include "fps_counter.hxx"
#include "fonts.hxx"
#include <string>

FpsCounter::FpsCounter()
{
    fps_start = std::chrono::steady_clock::now();
}

void FpsCounter::update()
{
    fps_count++;
    auto current_time = std::chrono::steady_clock::now();
    auto fps_elapsed = float(std::chrono::duration_cast<std::chrono::microseconds>(current_time - fps_start).count())
            /(1000.0f*1000.0f);

    if (fps_elapsed > FPS_TIME_COUNT_SEC) {
        fps_start = current_time;
        fps_last_counter = float(fps_count) / FPS_TIME_COUNT_SEC;
        fps_count = 0;
    }
    fontRenderTextBorder("regular", "FPS: " + std::to_string(fps_last_counter), 25.0f, 695.0f,  0.5f,  2, glm::vec3(0.3, 0.7f, 0.9f), glm::vec3(0.1, 0.1f, 0.1f));
}

