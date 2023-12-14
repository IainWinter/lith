#include "lith/capsule.h"
#include <cfloat>

static const float pi = 3.1415927f;

Capsule MakeCapsule(int resolution, float height, float radius) {
	if (resolution < 2) {
		return {};
	}

	// Almost same generation as UV sphere but we force the
	// lat count to be odd so it can be split evenly

	int latCount = resolution;
	int lonCount = resolution;

	if (latCount % 2 == 0) {
		latCount++;
	}

	// Each longitudinal count makes two triangles (6 indices) for every
	// lateral count except for the top and bottom poles, which only make
	// one triangle per longitudinal count.

	// UV maps require two vertices with the same position, but different UVs
	// so we need counts + 1.

	const int totalIndexCount = 6 * (latCount - 1) * lonCount;
	const int totalVertexCount = (latCount + 1) * (lonCount + 1);
	const float latStep = pi / latCount;
	const float lonStep = 2 * pi / lonCount;
	const float zOffset = clamp(height / 2.0f - radius, 0.0f, FLT_MAX);

	Capsule capsule;

	capsule.index.resize(totalIndexCount);
	capsule.pos.resize(totalVertexCount);
	capsule.uvs.resize(totalVertexCount);

	int currentVertex = 0;
	int currentIndex = 0;

	for (int lat = 0; lat <= latCount; lat++) {
		float offset = lat > latCount / 2 ? zOffset : -zOffset;

		for (int lon = 0; lon <= lonCount; lon++) {
			capsule.pos[currentVertex] = vec3(
				cos(lon * lonStep) * sin(lat * latStep) * radius,
				sin(lon * lonStep) * sin(lat * latStep) * radius,
				cos(lat * latStep - pi) * radius + offset
			);

			// UVs are almost the same as UV sphere, but V needs to be scaled
			// to fit the height

			capsule.uvs[currentVertex] = vec2(
				(float)lon / lonCount,
				capsule.pos[currentVertex].z / (radius * 2 + height) + 0.5f
			);

			currentVertex += 1;
		}
	}

	// Top cap
	//
	// One triangle connects the first lateral layer to the second per longitudinal count.
	// Even though the top points all have the same position, their UVs are different,
	// so each triangle uses a different point.
	//
	//          -------- lonCount -------->
	//                      lon
	//    |                  *
	//    |                / | \.
	//    1             /    |    \.
	//    |          /       |       \.
	//    |       /                     \.
	//   \ /     *------------*------------*
	//          v          v + 1      (v + 1) + 1

	int v = lonCount + 1;
	for (int lon = 0; lon < lonCount; lon++) {
		capsule.index[currentIndex++] = lon;
		capsule.index[currentIndex++] = v;
		capsule.index[currentIndex++] = v + 1;

		v += 1;
	}

	// Middle
	//
	// Each lateral layer has 2 triangles for every longitudinal count.
	//
	//          -------- lonCount -------->
	//          v          v + 1      (v + 1) + 1
	//    |     *------------*------------*
	//    |     |          / |          / |
	// latCount |       /    |       /    |
	//    |     |    /       |    /       |
	//    |     | /          | /          |
	//   \ /    *------------*------------*
	//      v + lc + 1   v + lc + 2   (v + 1) + lc + 2

	v = lonCount + 1;
	for (int lat = 1; lat < latCount - 1; lat++) {
		for (int lon = 0; lon < lonCount; lon++) {
			capsule.index[currentIndex++] = v;
			capsule.index[currentIndex++] = v + lonCount + 1;
			capsule.index[currentIndex++] = v + 1;

			capsule.index[currentIndex++] = v + 1;
			capsule.index[currentIndex++] = v + lonCount + 1;
			capsule.index[currentIndex++] = v + lonCount + 2;

			v += 1;
		}

		v += 1;
	}

	// Bottom cap

	// Same as top cap, but reversed.
	//
	//          -------- lonCount -------->
	//          v          v + 1      (v + 1) + 1
	//    |     *------------*------------*
	//    |       \          |          /
	//    1          \       |       /
	//    |             \    |    /
	//    |                \ | /
	//   \ /                 *
	//                   v + lc + 1

	for (int lon = 0; lon < lonCount; lon++) {
		capsule.index[currentIndex++] = v;
		capsule.index[currentIndex++] = v + lonCount + 1;
		capsule.index[currentIndex++] = v + 1;

		v += 1;
	}

	return capsule;
}