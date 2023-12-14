#include "lith/clock.h"
#include <chrono>

using lithclock = std::chrono::high_resolution_clock;
using lithtimepoint = std::chrono::time_point<lithclock >;
using lithduration = lithclock::duration;

static lithtimepoint chrono_start = lithclock::now();
static lithtimepoint chrono_now = lithclock::now();
static lithduration  chrono_delta = lithclock::duration::zero();

static size_t ticks = 0;

static float time_scale = 1.0f;
static float time_fixed = 0.02f;

static float total_time = 0.0f;
static float total_time_scaled = 0.0f;

static float current_delta = 0.0f;
static float current_delta_scaled = 0.0f;
static float current_fixed_scaled = 0.0f;

float lithDeltaTime() {
	return current_delta_scaled;
}

float lithTotalTime() {
	return total_time_scaled;
}

float lithGetTime() {
    return lithclock::now().time_since_epoch().count() / 1000000000.0f;
}

void lithUpdateTime() {
    ticks++;

    current_delta = chrono_delta.count() / 1000000000.0f;
    current_delta_scaled = time_scale * current_delta;
    current_fixed_scaled = time_scale * time_fixed;

    total_time += current_delta;
    total_time_scaled += current_delta_scaled;

    chrono_delta = lithclock::now() - chrono_now;
    chrono_now = lithclock::now();
}