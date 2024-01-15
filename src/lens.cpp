#include "lith/lens.h"

CameraLens::CameraLens()
	: position (0, 0, 0)
	, rotation (1, 0, 0, 0)
	, height   (0)
	, width    (0)
	, aspect   (1)
	, near     (0)
	, far      (0)
	, ortho    (false)
{}

mat4 CameraLens::GetViewMatrix() const {
	return translate(toMat4(-rotation), -position);
}

mat4 CameraLens::GetProjectionMatrix() const {
	float w = height * aspect;

	return ortho 
		? glm::ortho(-w / 2, w / 2, -height / 2, height / 2, near, far)
		: glm::perspective(fovy, aspect, near, far);
}

vec3 CameraLens::GetViewDirection() const {
	return rotation * vec3(0, 0, -1);
}

float CameraLens::GetNearClipPlaneDistance() const {
	return ortho
		? near
		: 1.f / tan(fovy / 2.f);
}

CameraLens& CameraLens::SetView(vec3 position, quat rotation) {
	this->position = position;
	this->rotation = rotation;
	return *this;
}

CameraLens& CameraLens::SetView(vec2 position, float rotation) {
	this->position = vec3(position, 0.f);
	this->rotation = angleAxis(rotation, vec3(0, 0, 1));
	return *this;
}

CameraLens& CameraLens::SetOrthographic(float height, float aspect, float near, float far) {
	this->height = height;
	this->aspect = aspect;
	this->width = height * aspect;
	this->near = near;
	this->far = far;
	this->ortho = true;

	return *this;
}

CameraLens& CameraLens::SetPerspective(float fovy, float aspect, float near, float far) {
	this->fovy = fovy;
	this->fovx = fovy * aspect;
	this->aspect = aspect;
	this->near = near;
	this->far = far;
	this->ortho = false;

	return *this;
}

vec2 CameraLens::ScreenSize() const {
	return vec2(height * aspect, height);
}

vec2 CameraLens::ScreenToWorld2D(vec2 screen) const {
	vec2 realDim = ScreenSize();
	vec2 pos = vec2(position.x, position.y) + screen * realDim - realDim / 2.f;

	return vec2(pos.x, pos.y);
}

vec2 CameraLens::World2DToScreen(vec2 world2D) const {
	vec2 realDim = ScreenSize();
	vec2 pos = world2D - vec2(position);

	return (pos + realDim) / realDim / 2.f;
}

CameraLens lens_Orthographic(float height, float aspect, float near, float far) {
	return CameraLens().SetOrthographic(height, aspect, near, far);
}

CameraLens lens_Perspective(float fov, float aspect, float near, float far) {
	return CameraLens().SetPerspective(fov, aspect, near, far);
}