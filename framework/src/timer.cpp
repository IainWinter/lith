#include "lith/timer.h"

lithTimer::lithTimer(float delay)
	: delay (delay)
	, timer (0.f)
	, trap  (false)
	, pass  (false)
{}

void lithTimer::reset() {
	timer = 0;
}

float lithTimer::ratio() const {
	float r = timer / delay;

	if (r < 0) return 0;
	if (r > 1) return 1;

	return r;
}

bool lithTimer::passed() const {
	return timer > delay;
}

bool lithTimer::trapped(float dt) {
	if (!trap) {
		timer += dt;
	}

	bool p = timer > delay;
	trap = p;

	if (p) {
		timer -= delay;
	}

	return p;
}

bool lithTimer::passed(float dt) {
	timer += dt;
	bool p = timer > delay;

	if (p) {
		timer = 0.f;
	}

	return p;
}

bool lithTimer::once(float dt) {
	timer += dt;
	return timer > delay;
}

bool lithTimer::once_until_reset(float dt) {
	if (!pass) {
		timer += dt;
	}

	pass = true;

	return timer > delay;
}

bool lithTimer::passed_until_reset(float dt) {
	timer += dt;
	return timer > delay;
}

void lithTimer::reset_if_passed() {
	if (timer > delay) {
		timer = 0;
	}

	pass = false;
}

lithTimerBurst::lithTimerBurst(float outerDelay, float innerDelay, int burstCount)
	: outer        (outerDelay)
	, inner        (innerDelay)
	, burstCount   (burstCount)
	, burstCounter (0)
{}

void lithTimerBurst::reset() {
	outer.reset();
	inner.reset();
}

bool lithTimerBurst::burst(float dt) {
	bool innerPassed = false;
	if (outer.once(dt)) {
		innerPassed = inner.passed(dt);
		if (innerPassed) {
			burstCounter += 1;
			if (burstCounter >= burstCount) {
				burstCounter = 0;
				outer.reset();
			}
		}
	}

	return innerPassed;
}

void lithTimerBurst::failed_burst() {
	burstCounter -= 1;
}