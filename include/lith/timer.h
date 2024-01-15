#pragma once

// If the timer is being hit inside of a loop
// only add dt once, then not again until reset_if_passed is called

struct lithTimer
{
	float delay;
	float timer;
	bool trap;
	bool pass;

	lithTimer(float delay);

	void reset();
	void reset_if_passed();

	float ratio() const;

	bool passed() const;
	bool trapped(float dt);
	bool passed(float dt);
	bool once(float dt);

	bool once_until_reset(float dt);
	bool passed_until_reset(float dt);
};

struct lithTimerBurst
{
	lithTimer outer;
	lithTimer inner;
	int burstCount;

	int burstCounter = 0;

	lithTimerBurst(float outerDelay, float innerDelay, int burstCount);

	void reset();

	// should use in while loop like trapped
	bool burst(float dt);

	// decrement the burst counter
	void failed_burst();
};