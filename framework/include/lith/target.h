#pragma once

#include "lith/texture.h"
#include <vector>

enum TargetAttachmentType {
	TargetAttachmentColor0,
	TargetAttachmentColor1,
	TargetAttachmentColor2,
	TargetAttachmentColor3,

	TargetAttachmentDepth
};

bool isColorAttachment(TargetAttachmentType attachment);
GLenum getAttachmentType(TargetAttachmentType attachment);

// Reset the framebuffer to the screen
void useScreenTarget();

struct TargetAttachment {
	TargetAttachmentType attachment;
	TextureInterface* texture;

	bool isOwnedByTarget;
	bool isColorAttachment;
};

struct TargetData {
	std::vector<TargetAttachment> attachments;

	int width;
	int height;

	TargetData();

	TargetAttachment* getAttachment(TargetAttachmentType attachment);
};

class Target {
public:
	Target();
	Target(const TargetData& data);

	// Swap out the texture linked to an attachment.
	void swapAttachment(TargetAttachmentType attachment, TextureInterface* texture);

	// Return the texture linked for an attachment, or nullptr if one does not exist.
	TextureInterface* get(TargetAttachmentType attachment);

	void upload();
	void download();
	void free();
	void use();

private:
	TargetData data;

	GLuint handle;
};

class TargetBuilder {
public:
	// Set the size of this render target. This will be the size of all owned textures.
	TargetBuilder& size(int width, int height);

	// Link a texture to a target.
	// The target does not own the texture, and will not free it.
	TargetBuilder& attach(TargetAttachmentType attachment, TextureInterface* texture);

	// Create a new texture and link it to the target.
	// The target owns the texture, and will free it.
	TargetBuilder& attach(TargetAttachmentType attachment, TextureFormat format);

	Target build();

private:
	TargetData building;
};