#include "lith/uvsphere.h"

static const float pi = 3.1415927f;

UVSphere MakeUVSphere(int latCount, int lonCount) {
	if (latCount < 2 || lonCount < 2) {
		return {};
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
 
	UVSphere sphere;

	sphere.index.resize(totalIndexCount);
	sphere.pos.resize(totalVertexCount);
	sphere.uvs.resize(totalVertexCount);
 
	int currentVertex = 0;
	int currentIndex = 0;

	for (int lat = 0; lat <= latCount; lat++) {
		for (int lon = 0; lon <= lonCount; lon++) {
			sphere.pos[currentVertex] = vec3(
				cos(lon * lonStep) * sin(lat * latStep),
				sin(lon * lonStep) * sin(lat * latStep),
				cos(lat * latStep - pi)
			);

			sphere.uvs[currentVertex] = vec2(
				(float)lon / lonCount,
				(float)lat / latCount
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
	//    |                / | \
	//    1             /    |    \
	//    |          /       |       \
	//    |       /                     \
	//   \ /     *------------*------------*
	//           v          v + 1        v + 2
 
	int v = lonCount + 1;
	for (int lon = 0; lon < lonCount; lon++) {
		sphere.index[currentIndex++] = lon;
		sphere.index[currentIndex++] = v;
		sphere.index[currentIndex++] = v + 1;

		v += 1;
	}

	// Middle
	//
	// Each lateral layer has 2 triangles for every longitudinal count.
	//
	//          -------- lonCount -------->
	//          v          v + 1        v + 2
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
			sphere.index[currentIndex++] = v;
			sphere.index[currentIndex++] = v + lonCount + 1;
			sphere.index[currentIndex++] = v + 1;
 
			sphere.index[currentIndex++] = v + 1;
			sphere.index[currentIndex++] = v + lonCount + 1;
			sphere.index[currentIndex++] = v + lonCount + 2;

			v += 1;
		}

		v += 1;
	}

	// Bottom cap

	// Same as top cap, but reversed.
	//
	//          -------- lonCount -------->
	//          v          v + 1        v + 2
	//    |     *------------*------------*
	//    |       \          |          /
	//    1          \       |       /
	//    |             \    |    /
	//    |                \ | /
	//   \ /                 *
	//                   v + lc + 1
 
	for (int lon = 0; lon < lonCount; lon++) {
		sphere.index[currentIndex++] = v;
		sphere.index[currentIndex++] = v + lonCount + 1;
		sphere.index[currentIndex++] = v + 1;

		v += 1;
	}
 
	return sphere;
}