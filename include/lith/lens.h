#pragma once

#include "lith/math.h"
#include "glm/gtx/quaternion.hpp"

class CameraLens
{
public:
	vec3 position;
	quat rotation;
	
	union { float fovy, height; };
	union { float fovx, width; };

	float aspect;
	float near;
	float far;

	// is this camera lens orthographic or perspective
	bool ortho;

public:
	CameraLens();

	mat4 GetViewMatrix() const;
	mat4 GetProjectionMatrix() const;

	vec3 GetViewDirection() const;
	float GetNearClipPlaneDistance() const;

	CameraLens& SetView(vec3 position, quat rotation);
	CameraLens& SetView(vec2 position, float rotation);

	CameraLens& SetOrthographic(float height, float aspect, float near, float far);
	CameraLens& SetPerspective(float fov, float aspect, float near, float far);

	// returns (aspect * height, height)
	vec2 ScreenSize() const;

	// assumes 2d, should just put in real rotation math
	vec2 ScreenToWorld2D(vec2 screen) const;
	vec2 World2DToScreen(vec2 world2D) const;
};

CameraLens lens_Orthographic(float height, float aspect, float near, float far);
CameraLens lens_Perspective(float fov, float aspect, float near, float far);