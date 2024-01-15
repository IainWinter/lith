#include "lith/interpolation.h"

LinearInterpolator::LinearInterpolator(vec2 start, vec2 end, float stepSize)
	: cur     (start)
	//, end     (end)
	, curStep (0)
{
	delta = normalize(end - start) * stepSize;
	totalSteps = (int)ceil(distance(start, end) / stepSize) + 1;
}

void LinearInterpolator::next() {
	curStep += 1;
	cur += delta;
}

bool LinearInterpolator::more() const {
	return curStep < totalSteps;
}

vec2 LinearInterpolator::current() const {
	return cur;
}
