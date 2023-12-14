#include "lith/plane.h"

Plane MakePlane(int xCount, int yCount) {
	// For each count in either direction, two triangles are added to the mesh.
	// The xCount and yCount are the number of quads, not vertices, so one more vertex is needed in each direction.

	if (xCount == 0 || yCount == 0) {
		return {};
	}

	const int totalIndexCount = 6 * xCount * yCount;
	const int totalVertexCount = (xCount + 1) * (yCount + 1);

	const vec3 offset = vec3(-1, -1, 0); // Make the plane span (-1, -1, 0) to (1, 1, 0)
	const float xStep = 2.0f / xCount;
	const float yStep = 2.0f / yCount;

	const float uStep = 1.0f / xCount; // But UVs always span (0, 0) to (1, 1)
	const float vStep = 1.0f / yCount;

	Plane plane;

	plane.index.resize(totalIndexCount);
	plane.pos.resize(totalVertexCount);
	plane.uvs.resize(totalVertexCount);

	int currentVertex = 0;
	int currentIndex = 0;

	for (int x = 0; x <= xCount; x++) {
		for (int y = 0; y <= yCount; y++) {
			plane.pos[currentVertex] = vec3(x * xStep, y * yStep, 0) + offset;
			plane.uvs[currentVertex] = vec2(x * uStep, y * vStep);

			currentVertex += 1;
		}
	}

	//        --------- xCount --------->
	//        v          v + 1        v + 2
	//    |   *------------*------------*
	//    |   |          / |          / |
	// yCount |       /    |       /    |
	//    |   |    /       |    /       |
	//   \ /  | /          | /          |
	//        *------------*------------*
	//    v + yc + 1   v + yc + 2   v + yc + 3

	for (int v = 0; v < totalVertexCount - yCount- 2; v++) {
		// If at the final row, jump to next column
		if ((v + 1) % (yCount + 1) == 0) {
			v++;
		}

		plane.index[currentIndex++] = v;
		plane.index[currentIndex++] = v + 1;
		plane.index[currentIndex++] = v + yCount + 1;

		plane.index[currentIndex++] = v + 1;
		plane.index[currentIndex++] = v + yCount + 2;
		plane.index[currentIndex++] = v + yCount + 1;
	}

	return plane;
}