#include "lith/target.h"
#include "gl/glad.h"
#include <algorithm>

bool isColorAttachment(TargetAttachmentType attachment) {
	if (attachment == TargetAttachmentDepth) {
		return false;
	}

	return true;
}

GLenum getAttachmentType(TargetAttachmentType attachment) {
	switch (attachment) {
		case TargetAttachmentColor0: return GL_COLOR_ATTACHMENT0;
		case TargetAttachmentColor1: return GL_COLOR_ATTACHMENT1;
		case TargetAttachmentColor2: return GL_COLOR_ATTACHMENT2;
		case TargetAttachmentColor3: return GL_COLOR_ATTACHMENT3;
		case TargetAttachmentDepth:  return GL_DEPTH_ATTACHMENT;
	}

	throw nullptr;
}

void useScreenTarget() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

TargetData::TargetData() 
	: width  (0)
	, height (0)
{}

TargetAttachment* TargetData::getAttachment(TargetAttachmentType attachment) {
	auto itr = std::find_if(attachments.begin(), attachments.end(),
		[attachment](const auto& x) { return x.attachment == attachment; });

	return itr != attachments.end()
		? &*itr
		: nullptr;
}

Target::Target()
	: handle (0)
{}

Target::Target(const TargetData& data) 
	: data   (data)
	, handle (0)
{}

void Target::swapAttachment(TargetAttachmentType attachment, TextureInterface* texture) {
	TargetAttachment* current = data.getAttachment(attachment);

	if (current) {
		if (current->isOwnedByTarget) {
			current->texture->free();
			delete current->texture;
		}

		current->texture = texture;
	}
}

TextureInterface* Target::get(TargetAttachmentType attachment) {
	TargetAttachment* a = data.getAttachment(attachment);
	return a ? a->texture : nullptr;
}

void Target::upload() {
	if (handle == 0) {
		glGenFramebuffers(1, &handle);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, handle);

	std::vector<GLenum> drawBuffers;

	for (TargetAttachment& attachment : data.attachments) {
		attachment.texture->upload();

		GLenum attachmentType = getAttachmentType(attachment.attachment);
		int textureHandle = attachment.texture->getHandle();

		glFramebufferTexture(GL_FRAMEBUFFER, attachmentType, textureHandle, 0);

		if (attachment.isColorAttachment) {
			drawBuffers.push_back(attachmentType);
		}
	}

	glDrawBuffers(drawBuffers.size(), drawBuffers.data());
}

void Target::free() {
	glDeleteFramebuffers(1, &handle);
	handle = 0;

	for (const auto& attachment : data.attachments) {
		if (attachment.isOwnedByTarget) {
			attachment.texture->free();
			delete attachment.texture;
		}
	}
	data.attachments.clear();
}

void Target::use() {
	glBindFramebuffer(GL_FRAMEBUFFER, handle);
}

TargetBuilder& TargetBuilder::size(int width, int height) {
	building.width = width;
	building.height = height;

	return *this;
}

TargetBuilder& TargetBuilder::attach(TargetAttachmentType attachment, TextureInterface* texture) {
	TargetAttachment data;
	data.attachment = attachment;
	data.texture = texture;
	data.isColorAttachment = isColorAttachment(attachment);
	data.isOwnedByTarget = false;

	building.attachments.push_back(data);

	return *this;
}

TargetBuilder& TargetBuilder::attach(TargetAttachmentType attachment, TextureFormat format) {
	Texture* tex = new Texture();
	tex->source(format, building.width, building.height);

	TargetAttachment data;
	data.attachment = attachment;
	data.texture = tex;
	data.isColorAttachment = isColorAttachment(attachment);
	data.isOwnedByTarget = true;

	building.attachments.push_back(data);

	return *this;
}

Target TargetBuilder::build() {
	return Target(building);
}
