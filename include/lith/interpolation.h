#pragma once

#include "lith/math.h"

class LinearInterpolator {
public:
	LinearInterpolator(vec2 start, vec2 end, float stepSize);

	void next();
	bool more() const;
	vec2 current() const;

private:
	vec2 cur;
	//vec2 end;
	vec2 delta;

	int curStep;
	int totalSteps;
};