#pragma once

#include "lith/math.h"
#include "lith/bytes.h"
#include "lith/typedef.h"

enum VertexArrayTopology {
	TopologyTriangles,
	TopologyTriangleStrip,
	TopologyTriangleFan,
	TopologyLines,
	TopologyLineStrip,
	TopologyLineLoop,
	TopologyPoints
};

enum VertexArrayAttributeType {
	AttributeTypeFloat,
	AttributeTypeInt
};

GLenum getVertexArrayTopology(VertexArrayTopology topology);
GLenum getVertexArrayAttributeType(VertexArrayAttributeType type);

struct VertexBuffer {
	int id;

	GLenum type;
	GLuint handle;

	ByteVector data;

	bool freeHostAfterUpload;
	bool belongsToInstancedAttribute;
	int uploadedItemCount;

	VertexBuffer();
};

struct VertexArrayAttribute {
	int id;
    int bufferID;
	GLuint instanceStride;
	GLint offset;
	VertexArrayAttributeType type;
	GLenum count;

    VertexBuffer* buffer;
	bool uploaded;

	VertexArrayAttribute();
};

struct VertexArrayData {
	std::vector<VertexBuffer> buffers;
	std::vector<VertexArrayAttribute> attributes;
    VertexBuffer* indexBuffer;
	VertexArrayTopology topology;

	VertexArrayData();

	VertexArrayData(const VertexArrayData& copy);
	VertexArrayData& operator=(const VertexArrayData& copy);

	// force copies
	VertexArrayData(VertexArrayData&&) = delete;
	VertexArrayData& operator=(VertexArrayData&&) = delete;

	VertexBuffer* getBuffer(int bufferID);

	bool hasInstancedAttribute() const;
	bool hasIndexBuffer() const;
	void cacheAttributeBufferLinks();

private:
	void copy_from(const VertexArrayData& copy);
};

class VertexArray {
public:
	VertexArray();
	VertexArray(const VertexArrayData& data);

	VertexBuffer& buffer(int bufferID);
	VertexBuffer& index();

	ByteVector& bufferData(int bufferID);
	ByteVector& indexData();

	const VertexArrayData& internalData() const;

	VertexArray& upload();

	void free();
	void clear();
	void clearInstances();

	void draw();

private:
    VertexArrayData data;

	GLuint handle;

	bool hasInstancedAttribute;
	bool hasIndexBuffer;

	int indexCount;
	int vertexCount;
	int instanceCount;
};

class VertexArrayBuilder {
public:
	VertexArrayBuilder& topology(VertexArrayTopology topology);

	VertexArrayBuilder& index();
	VertexArrayBuilder& buffer(int bufferID);

	VertexArrayBuilder& map(int bufferID);
	VertexArrayBuilder& attribute(int attributeID);

	VertexArrayBuilder& type(VertexArrayAttributeType type, int count);
	VertexArrayBuilder& host();
	VertexArrayBuilder& instanced(int stride = 1);

	VertexArrayBuilder& data(int itemSize);
	VertexArrayBuilder& data(int itemSize, int byteCount, void* bytes);
	VertexArrayBuilder& data(std::initializer_list<int> ints);
	VertexArrayBuilder& data(std::initializer_list<float> floats);
	VertexArrayBuilder& data(std::initializer_list<vec2> vec2s);
	VertexArrayBuilder& data(std::initializer_list<vec3> vec3s);
	VertexArrayBuilder& data(std::initializer_list<vec4> vec4s);

	template<typename _t>
	VertexArrayBuilder& data(const std::vector<_t>& vector) {
		return data((int)sizeof(_t), (int)(vector.size() * sizeof(_t)), (void*)vector.data());
	}

	VertexArray build();

private:
	VertexArrayData building;
    
	VertexBuffer* currentBuffer;
	VertexArrayAttribute* currentAttribute;
    int currentInstanceStride = 0;

	// should add checks to this so the function call order is correct, or a series of interfaces
	// which this class implements.
};

// file format, could implement in simple parser that uses the builder:
// 
// topology triangles
//
// index
// data_ints 0 1 2 0 3 2
//
// buffer 0
// data_init 16 64
// data_floats 0 0 0 1 1 0 1 1
//
// buffer 1 
// data_init 56
// host 
//
// map 0
// attribute 0 f2
// attribute 1 f2
//
// map 1
// instanced
// attribute 2 f3
// attribute 3 f2
// attribute 4 f1
// attribute 5 f1
// attribute 6 f4
// attribute 7 f4