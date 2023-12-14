#include "lith/mesh.h"
#include "lith/log.h"
#include "gl/glad.h"
#include <algorithm>

GLenum getVertexArrayTopology(VertexArrayTopology topology) {
	switch (topology) {
		case TopologyTriangles:     return GL_TRIANGLES;
		case TopologyTriangleStrip: return GL_TRIANGLE_STRIP;
		case TopologyTriangleFan:   return GL_TRIANGLE_FAN;
		case TopologyLines:         return GL_LINES;
		case TopologyLineStrip:     return GL_LINE_STRIP;
		case TopologyLineLoop:      return GL_LINE_LOOP;
		case TopologyPoints:        return GL_POINTS;
	}

	throw nullptr;
}

GLenum getVertexArrayAttributeType(VertexArrayAttributeType type) {
	switch (type) {
		case AttributeTypeFloat: return GL_FLOAT;
		case AttributeTypeInt:   return GL_INT;
	}

	throw nullptr;
}

VertexBuffer::VertexBuffer()
	: id                          (0)
	, type                        (0)
	, handle                      (0)
	, freeHostAfterUpload         (false)
	, belongsToInstancedAttribute (false)
	, uploadedItemCount           (0)
{}

VertexArrayAttribute::VertexArrayAttribute()
	: id             (0)
	, bufferID       (0)
	, instanceStride (0)
	, offset         (0)
	, type           (AttributeTypeFloat)
	, count          (0)
	, buffer         (nullptr)
	, uploaded       (false)
{}

VertexArrayData::VertexArrayData()
	: buffers     ()
	, attributes  ()
	, indexBuffer (nullptr)
	, topology    (TopologyTriangles)
{}

VertexArrayData::VertexArrayData(const VertexArrayData& copy) {
	copy_from(copy);
}

VertexArrayData& VertexArrayData::operator=(const VertexArrayData& copy) {
	copy_from(copy);
	return *this;
}

void VertexArrayData::copy_from(const VertexArrayData& copy) {
	buffers = copy.buffers;
	attributes = copy.attributes;
	topology = copy.topology;
	indexBuffer = nullptr;

	cacheAttributeBufferLinks();
}

VertexBuffer* VertexArrayData::getBuffer(int bufferID) {
	if (indexBuffer && bufferID == -1) {
		return indexBuffer;
	}

	auto itr = std::find_if(buffers.begin(), buffers.end(), 
		[bufferID](const auto& buffer){ return buffer.id == bufferID; });

    return itr != buffers.end()
        ? &*itr
        : nullptr;
}

bool VertexArrayData::hasInstancedAttribute() const {
    return std::any_of(attributes.begin(), attributes.end(),
        [](const auto& attribute) { return attribute.instanceStride > 0; });
}

bool VertexArrayData::hasIndexBuffer() const {
    return std::any_of(buffers.begin(), buffers.end(),
        [](const auto& buffer) { return buffer.id == -1; });
}

void VertexArrayData::cacheAttributeBufferLinks() {
    for (VertexArrayAttribute& attribute : attributes) {
        attribute.buffer = getBuffer(attribute.bufferID);
    }
        
    indexBuffer = getBuffer(-1);
}

VertexArray::VertexArray()
    : handle                (0)
    , hasInstancedAttribute (0)
    , hasIndexBuffer        (0)
    , indexCount            (0)
    , vertexCount           (0)
    , instanceCount         (0)
{}

VertexArray::VertexArray(const VertexArrayData& data)
	: data          (data)
    , handle        (0)
    , indexCount    (0)
    , vertexCount   (0)
    , instanceCount (0)
{
	hasInstancedAttribute = data.hasInstancedAttribute();
	hasIndexBuffer = data.hasIndexBuffer();
}

VertexBuffer& VertexArray::buffer(int bufferID) {
	return *data.getBuffer(bufferID);
}

VertexBuffer& VertexArray::index() {
	return buffer(-1);
}

ByteVector& VertexArray::bufferData(int bufferID) {
	return buffer(bufferID).data;
}

ByteVector& VertexArray::indexData() {
	return bufferData(-1);
}

const VertexArrayData& VertexArray::internalData() const {
	return data;
}

VertexArray& VertexArray::upload() {
    // issue is that the a.buffer is an invalid pointer
        
    if (handle == 0) {
        glGenVertexArrays(1, &handle);
    }
        
    glBindVertexArray(handle);
		
	for (VertexBuffer& b : data.buffers) {
        if (b.handle == 0) {
			glGenBuffers(1, &b.handle);
            glBindBuffer(b.type, b.handle);
            glBufferData(b.type, b.data.size(), b.data.data(), GL_DYNAMIC_DRAW); // hint doesn't matter apparently
                
            b.uploadedItemCount = b.data.count();
                
            if (b.freeHostAfterUpload) {
                b.data.clear();
            }
		}
            
        // update existing buffers which are on CPU
        // could put dirty flag in data
        else if (!b.freeHostAfterUpload) {
            glBindBuffer(b.type, b.handle);
            glBufferData(b.type, b.data.size(), b.data.data(), GL_DYNAMIC_DRAW); // use sub data
                
            b.uploadedItemCount = b.data.count();
        }
	}

	if (data.attributes.size() > 0) {
		vertexCount = INT_MAX;
		instanceCount = INT_MAX;
	}

	for (VertexArrayAttribute& a : data.attributes) {
		if (!a.uploaded) {
			a.uploaded = true;

			intptr_t offset = (intptr_t)a.offset;
			void* offsetPtr = (void*)offset;

			GLenum attributeType = getVertexArrayAttributeType(a.type);

			glBindBuffer(a.buffer->type, a.buffer->handle);
			glEnableVertexAttribArray(a.id);
			glVertexAttribPointer(a.id, a.count, attributeType, GL_FALSE, a.buffer->data.stride(), offsetPtr);
			glVertexAttribDivisor(a.id, a.instanceStride);
		}

		if (a.instanceStride == 0) {
			int count = a.buffer->uploadedItemCount;
			vertexCount = min(vertexCount, count);
		}

		else {
			int count = a.buffer->data.count() / a.instanceStride;
			instanceCount = min(instanceCount, count);
		}
	}

    if (data.indexBuffer) {
        indexCount = data.indexBuffer->uploadedItemCount;
    }
            
    // unbind so future binds don't effect this VAO
    glBindVertexArray(0);
        
	return *this;
}

void VertexArray::free() {
	for (VertexBuffer& b : data.buffers) {
		glDeleteBuffers(1, &b.handle);
	}
		
	data.buffers.clear();
	data.attributes.clear();

	glDeleteVertexArrays(1, &handle);
	handle = 0;
}

void VertexArray::clear() {
	for (VertexBuffer& b : data.buffers) {
		b.data.clear();
	}
}

void VertexArray::clearInstances() {
	for (VertexBuffer& b : data.buffers) {
		if (b.belongsToInstancedAttribute) {
			b.data.clear();
		}
	}
}

void VertexArray::draw() {
	glBindVertexArray(handle);

	GLenum topology = getVertexArrayTopology(data.topology);

	if (hasInstancedAttribute) {
		if (hasIndexBuffer) { glDrawElementsInstanced(topology, indexCount, GL_UNSIGNED_INT, nullptr, instanceCount); }
		else                { glDrawArraysInstanced(topology, 0, vertexCount, instanceCount); }
	}

	else {
		if (hasIndexBuffer) { glDrawElements(topology, indexCount, GL_UNSIGNED_INT, nullptr); }
		else                { glDrawArrays(topology, 0, vertexCount); }
	}
}

VertexArrayBuilder& VertexArrayBuilder::topology(VertexArrayTopology topology) {
	building.topology = topology;
        
    return *this;
}

VertexArrayBuilder& VertexArrayBuilder::index() {
	VertexBuffer& buffer = building.buffers.emplace_back();
	buffer.id = -1;
	buffer.type = GL_ELEMENT_ARRAY_BUFFER;

	currentBuffer = &buffer;

	return *this;
}

VertexArrayBuilder& VertexArrayBuilder::buffer(int bufferID) {
	VertexBuffer& buffer = building.buffers.emplace_back();
	buffer.id = bufferID;
	buffer.type = GL_ARRAY_BUFFER;

	currentBuffer = &buffer;

	return *this;
}

VertexArrayBuilder& VertexArrayBuilder::map(int bufferID) {
	currentBuffer = building.getBuffer(bufferID);
    currentAttribute = nullptr;
    currentInstanceStride = 0;

	return *this;
}

VertexArrayBuilder& VertexArrayBuilder::attribute(int attributeID) {
	// calculate offset before currentAttribute is invalidated by emplace_back
	int offset = currentAttribute
		? currentAttribute->offset + currentAttribute->count * 4 // todo: get type size
		: 0;

	VertexArrayAttribute& attribute = building.attributes.emplace_back();
	attribute.id = attributeID;
	attribute.bufferID = currentBuffer->id;
    attribute.instanceStride = currentInstanceStride;
	attribute.offset = offset;

	currentAttribute = &attribute;

	return *this;
}

VertexArrayBuilder& VertexArrayBuilder::type(VertexArrayAttributeType type, int count) {
	currentAttribute->type = type;
	currentAttribute->count = count;
	return *this;
}

VertexArrayBuilder& VertexArrayBuilder::host() {
	currentBuffer->freeHostAfterUpload = false;
	return *this;
}

VertexArrayBuilder& VertexArrayBuilder::instanced(int stride) {
	currentInstanceStride = stride;
	currentBuffer->belongsToInstancedAttribute = true;
	return *this;
}

VertexArrayBuilder& VertexArrayBuilder::data(int itemSize) {
	currentBuffer->data = ByteVector(itemSize);
	return *this;
}

VertexArrayBuilder& VertexArrayBuilder::data(int itemSize, int byteCount, void* bytes) {
	currentBuffer->data.setRaw(itemSize, byteCount, bytes);
	return *this;
}

VertexArrayBuilder& VertexArrayBuilder::data(std::initializer_list<int> ints) {
	currentBuffer->data = ByteVector(ints);
	return *this;
}

VertexArrayBuilder& VertexArrayBuilder::data(std::initializer_list<float> floats) {
	currentBuffer->data = ByteVector(floats);
	return *this;
}

VertexArrayBuilder& VertexArrayBuilder::data(std::initializer_list<vec2> vec2s) {
	currentBuffer->data = ByteVector(vec2s);
	return *this;
}

VertexArrayBuilder& VertexArrayBuilder::data(std::initializer_list<vec3> vec3s) {
	currentBuffer->data = ByteVector(vec3s);
	return *this;
}

VertexArrayBuilder& VertexArrayBuilder::data(std::initializer_list<vec4> vec4s) {
	currentBuffer->data = ByteVector(vec4s);
	return *this;
}

VertexArray VertexArrayBuilder::build() {
	return VertexArray(building);
}
