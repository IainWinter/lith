#include "lith/quad.h"

static VertexArray quad;

void lithDrawFullScreenQuad() {
    quad.draw();
}

struct QuadVertex {
    float x, y, u, v;
};

void lithInitFullScreenQuad() {
    QuadVertex verts[4] = {
        { -1, -1, 0, 0 },
        {  1, -1, 1, 0 },
        {  1,  1, 1, 1 },
        { -1,  1, 0, 1 },
    };

    int index[6] = { 
        0, 1, 2, 
        0, 2, 3
    };

    quad = VertexArrayBuilder()
        .topology(TopologyTriangles)
        .buffer(0)
            .data(sizeof(QuadVertex), sizeof(verts), verts)
        .index()
            .data(sizeof(int), sizeof(index), index)
        .map(0)
            .attribute(0).type(AttributeTypeFloat, 2)
            .attribute(1).type(AttributeTypeFloat, 2)
        .build()
        .upload();
}
