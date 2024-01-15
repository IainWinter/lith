#include "lith/sketch.h"
#include "lith/buffer.h"
#include "lith/random.h"

#include "lith/plane.h"
#include "lith/capsule.h"

#include "gl/glad.h"

static const float _pi = glm::pi<float>();

struct WaveParams {
	float w; // wavelength w = 2/L where L is the frequency
	float A; // amplitude
	float Q; // steepness Q = [0, 1/(w * A)]
	float phi; // phase speed phi = speed * w
	vec2 D;

	WaveParams(float wavelength, float amplitude, float steepness, float speed, vec2 direction) {
		w = 2.f * _pi / wavelength;
		A = amplitude;
		Q = 1.f / (w * A * 32) * steepness;
		phi = speed * w;
		D = normalize(direction);
	}
};

VertexArray mesh;
ShaderProgram waterShader;
Buffer waveParamBuffer;
std::vector<WaveParams> waves;
mat4 waterModelMatrix;
CameraLens lens;

VertexArray skybox;
ShaderProgram skyboxShader;

VertexArray boat;
ShaderProgram boatShader;

vec3 boatPos = vec3(0, 0, 0);
float boatHeading = 0;

void setupSkybox() {
	skybox = VertexArrayBuilder()
		.buffer(0).data({
			vec2(-1, -1),
			vec2( 1, -1),
			vec2( 1,  1),
			vec2(-1, -1),
			vec2( 1,  1),
			vec2(-1,  1)
		})
		.map(0)
			.attribute(0).type(AttributeTypeFloat, 2)
		.build()
		.upload();

	skyboxShader = ShaderProgramBuilder()
		.vertex(R"(
			#version 430

			layout (location = 0) in vec2 pos;

			out vec3 plane_pos;

			uniform float u_aspect;
			uniform mat3 u_cameraRotation;
			uniform float u_cameraNearClipDistance;

			void main() {
				vec2 uv = pos * vec2(u_aspect, 1.0);

				plane_pos = u_cameraRotation * vec3(uv, -u_cameraNearClipDistance);
				gl_Position = vec4(pos, 0.0, 1.0);
			}
		)")
		.fragment(R"(
			#version 430

			// stolen Implementation of 2012 Hosek-Wilkie skylight model :)

			#define M_PI 3.1415926535897932384626433832795
			#define CIE_X 0
			#define CIE_Y 1
			#define CIE_Z 2

			const float kHosekCoeffsX[] = float[](
				-1.129483, -1.146420, -1.128348, -1.025820, -1.083957, -1.062465, -0.189062, -0.188358, -0.264130, 0.014047,
				-0.260668, -0.151219, -9.065101, 3.309173, 1.223176, -1.187406, 2.207108, 1.042881, 9.659923, -3.127882,
				0.055150, 2.729900, -7.202803, 14.278390, -0.036078, -0.069382, -0.349065, 0.587759, -5.968103, -4.242214,
				0.831436, 0.398711, 1.997784, -0.276114, 2.129455, 0.403810, 0.081817, 0.140058, -0.041237, 0.460263, -0.077895,
				0.199778, 4.768868, 6.283042, -2.251251, 8.305125, -1.137688, 2.814449, 0.633978, 0.526708, 0.948347, 0.394500,
				0.887177, 0.580320
			);

			const float kHosekCoeffsY[] = float[](
				-1.144464, -1.170104, -1.129171, -1.042294, -1.082293, -1.071715, -0.204380, -0.211863, -0.255288,
				0.004450, -0.272306, -0.142657, -10.201880, 4.391405, 0.223830, -0.511603, 2.065076, 1.095351, 10.712470,
				-4.198900, 0.731429, 2.627589, -8.143133, 17.297831, -0.032567, -0.071116, -0.356273, 0.609900, -7.892212,
				-3.851931, 0.786021, 0.389044, 1.881931, -0.126464, 2.142231, 0.436051, 0.068727, 0.102483, -0.030787,
				0.432528, -0.071062, 0.211444, 4.824771, 6.282535, -1.039120, 7.080503, -1.122398, 2.970832, 0.625984,
				0.536569, 0.909630, 0.458365, 0.833851, 0.594439
			);

			const float kHosekCoeffsZ[] = float[](
				-1.353023, -1.624704, -0.798361, -1.266679, -1.009707, -1.075646, -0.481352, -0.799020, 0.141748,
				-0.428898, -0.153775, -0.176875, -31.049200, -21.671249, 9.914841, -5.818701, 3.496378, -1.347762,
				31.401560, 22.463409, -10.815030, 6.986437, -3.013726, 1.989004, -0.009511, -0.011635, -0.012188,
				-0.081807, 0.242115, 0.013758, 0.554203, 0.541575, 0.341139, 1.397403, -0.283193, 1.764810, 0.008135,
				0.026184, -0.061377, 0.201692, 0.030034, 0.133002, 3.136646, 1.139214, 7.445848, -1.275731, 3.702862,
				3.230864, 0.521599, 0.344436, 1.180080, 0.259277, 0.774632, 0.662621
			);

			const float kHosekRadX[] = float[](1.471043, 1.746088, -0.929970, 17.203620, 5.473384, 8.336416);
			const float kHosekRadY[] = float[](1.522034, 1.844545, -1.322862, 19.183820, 5.440769, 8.837119);
			const float kHosekRadZ[] = float[](1.107408, 2.382765, -5.112357, 21.478230, 14.931280, 14.608820);

			float sample_coeff(int channel, int quintic_coeff, int coeff) {    
				int index =  6 * coeff + quintic_coeff;
				if (channel == CIE_X) { return kHosekCoeffsX[index]; }
				if (channel == CIE_Y) { return kHosekCoeffsY[index]; }
				if (channel == CIE_Z) { return kHosekCoeffsZ[index]; }
			}

			float sample_radiance(int channel, int quintic_coeff) {
				int index = quintic_coeff;
				if (channel == CIE_X) { return kHosekRadX[index]; }
				if (channel == CIE_Y) { return kHosekRadY[index]; }
				if (channel == CIE_Z) { return kHosekRadZ[index]; }
			}

			float eval_quintic_bezier(in float[6] control_points, float t) {
				float t2 = t * t;
				float t3 = t2 * t;
				float t4 = t3 * t;
				float t5 = t4 * t;
				
				float t_inv = 1.0 - t;
				float t_inv2 = t_inv * t_inv;
				float t_inv3 = t_inv2 * t_inv;
				float t_inv4 = t_inv3 * t_inv;
				float t_inv5 = t_inv4 * t_inv;
					
				return (
					control_points[0] *             t_inv5 +
					control_points[1] *  5.0 * t  * t_inv4 +
					control_points[2] * 10.0 * t2 * t_inv3 +
					control_points[3] * 10.0 * t3 * t_inv2 +
					control_points[4] *  5.0 * t4 * t_inv  +
					control_points[5] *        t5
				);
			}

			float transform_sun_zenith(float sun_zenith) {
				float elevation = M_PI / 2.0 - sun_zenith;
				return pow(elevation / (M_PI / 2.0), 0.333333);
			}

			void get_control_points(int channel, int coeff, out float[6] control_points) {
				for (int i = 0; i < 6; ++i) control_points[i] = sample_coeff(channel, i, coeff);
			}

			void get_control_points_radiance(int channel, out float[6] control_points) {
				for (int i = 0; i < 6; ++i) control_points[i] = sample_radiance(channel, i);
			}

			void get_coeffs(int channel, float sun_zenith, out float[9] coeffs) {
				float t = transform_sun_zenith(sun_zenith);
				for (int i = 0; i < 9; ++i) {
					float control_points[6]; 
					get_control_points(channel, i, control_points);
					coeffs[i] = eval_quintic_bezier(control_points, t);
				}
			}

			vec3 mean_spectral_radiance(float sun_zenith) {
				vec3 spectral_radiance;
				for (int i = 0; i < 3; ++i) {
					float control_points[6];
					get_control_points_radiance(i, control_points);
					float t = transform_sun_zenith(sun_zenith);
					spectral_radiance[i] = eval_quintic_bezier(control_points, t);
				}

				return spectral_radiance;
			}

			float F(float theta, float gamma, in float[9] coeffs) {
				float A = coeffs[0];
				float B = coeffs[1];
				float C = coeffs[2];
				float D = coeffs[3];
				float E = coeffs[4];
				float F = coeffs[5];
				float G = coeffs[6];
				float H = coeffs[8];
				float I = coeffs[7];
				float chi = (1.0 + pow(cos(gamma), 2.0)) / pow(1.0 + H*H - 2.0 * H * cos(gamma), 1.5);
				
				return (
					(1.0 + A * exp(B / (cos(theta) + 0.01))) *
					(C + D * exp(E * gamma) + F * pow(cos(gamma), 2.0) + G * chi + I * sqrt(cos(theta)))
				);
			}

			vec3 spectral_radiance(float theta, float gamma, float sun_zenith) {
				vec3 XYZ;
				for (int i = 0; i < 3; ++i) {
					float coeffs[9];
					get_coeffs(i, sun_zenith, coeffs);
					XYZ[i] = F(theta, gamma, coeffs);
				}
				return XYZ;
			}

			// Returns angle between two directions defined by zentih and azimuth angles
			float angle(float z1, float a1, float z2, float a2) {
				return acos(
					sin(z1) * cos(a1) * sin(z2) * cos(a2) +
					sin(z1) * sin(a1) * sin(z2) * sin(a2) +
					cos(z1) * cos(z2));
			}

			// Sky is rendered using Hosek-Wilkie skylight model
			vec3 sample_sky(float view_zenith, float view_azimuth, float sun_zenith, float sun_azimuth) {    
				float gamma = angle(view_zenith, view_azimuth, sun_zenith, sun_azimuth);
				float theta = view_zenith;   
				vec3 mean_sr = mean_spectral_radiance(sun_zenith);
				float sun_angular_radius = M_PI / 360.0; // About 0.5 deg
				
				if (gamma > sun_angular_radius) {
					return spectral_radiance(theta, gamma, sun_zenith) * mean_sr;
				} else {
					return vec3(100.0, 100.0, 100.0);
				}
			}

			vec3 XYZ_to_RGB(vec3 XYZ) {
				mat3 XYZ_to_linear = mat3(
					3.24096994, -0.96924364, 0.55630080,
					-1.53738318,  1.8759675, -0.20397696,
					-0.49861076,  0.04155506, 1.05697151
				);

				return XYZ_to_linear * XYZ;
			}

			vec3 expose(vec3 color, float exposure) {
				return vec3(2.0) / (vec3(1.0) + exp(-exposure * color)) - vec3(1.0);
			}

			vec2 to_zenith_azimuth(vec3 v) {
				float zenith = acos(v.y);
				float azimuth = atan(v.x, v.z);
				return vec2(zenith, azimuth);
			}

			vec3 fresnel_schlick(float cosTheta, vec3 F0) {
				return F0 + (max(vec3(1.0), F0) - F0) * pow(1.0 - cosTheta, 5.0);
			}   

			in vec3 plane_pos;

			out vec4 fragColor;

			uniform float u_time;
			uniform vec3 u_sun;

			void main() {
				vec3 sun_dir = u_sun;
				vec2 sun_pos = to_zenith_azimuth(sun_dir);
				
				vec3 sample_dir = normalize(plane_pos);
				
				vec3 fresnel = vec3(1.0);
				if (sample_dir.y < 0.0) {
					vec3 N = vec3(0, 1, 0);
					
					sample_dir = reflect(sample_dir, N);
					if (sample_dir.y < 0.0) {
						sample_dir.y *= -1.;
					}

					// Eval fresnel
					float cos_theta = max(dot(N, sample_dir), 0.0);
					fresnel = fresnel_schlick(cos_theta, vec3(0.04));
				}
				
				vec2 view_zenith_azimuth = to_zenith_azimuth(sample_dir);
				
				vec3 XYZ = fresnel * sample_sky(view_zenith_azimuth.x, view_zenith_azimuth.y, sun_pos.x, sun_pos.y);  
				vec3 RGB = XYZ_to_RGB(XYZ);

				vec3 col = expose(RGB, 0.1);
				
				fragColor = vec4(col, 1.0);
			}
		)")
		.build()
		.compile();
}

void setupOcean() {
	Plane plane = MakePlane(1000, 1000);

	for (vec3& p : plane.pos) {
		p *= 1000.f;
	}

	mesh = VertexArrayBuilder()
		.buffer(0).data(pack(plane.pos, plane.uvs))
		.index().data(plane.index)
		.map(0)
			.attribute(0).type(AttributeTypeFloat, 3)
			.attribute(1).type(AttributeTypeFloat, 2)
		.build()
		.upload();

	waterModelMatrix = mat4_cast(angleAxis(_pi / 2.f, vec3(1, 0, 0)));

	waterShader = ShaderProgramBuilder()
		.vertex(R"(
			#version 430
			layout(location = 0) in vec3 pos;
			layout(location = 1) in vec2 uv;

			uniform mat4 u_model;
			uniform mat4 u_view;
			uniform mat4 u_proj;

			uniform float u_time;

			struct WaveParams {
				float w;   // wavelength w = 2/L where L is the frequency
				float A;   // amplitude
				float Q;   // steepness Q = [0, 1/(w * A)]
				float phi; // phase speed phi = speed * w
				vec2 D;    // the direction of the wave
			};

			layout (std430, binding = 0) buffer WaveParamBuffer {
				WaveParams u_waveData[1];
			};

			uniform int u_waveCount = 0;

			out vec2 frag_uv;
			out vec3 frag_pos;
			out vec3 frag_B;
			out vec3 frag_T;
			out vec3 frag_N;

			void main() {
				vec3 P = pos;

				for (int i = 0; i < u_waveCount; i++) {
					WaveParams wave = u_waveData[i];

					float angle = wave.w * dot(wave.D, pos.xy) + wave.phi * u_time; 

					P.xy += wave.Q * wave.A * wave.D * cos(angle);
					P.z += wave.A * sin(angle);
				}

				// vec3 B = vec3(1, 0, 0);
				// vec3 T = vec3(0, 1, 0);
				// vec3 N = vec3(0, 0, 1);

				// for (int i = 0; i < u_waveCount; i++) {
				// 	WaveParams wave = u_waveData[i];

				// 	float WA = wave.w * wave.A;
				// 	float S = sin(wave.w * dot(wave.D, P.xz) + wave.phi * u_time);
				// 	float C = cos(wave.w * dot(wave.D, P.xz) + wave.phi * u_time); // could calc both at the same time

				// 	B += vec3(-wave.Q * wave.D.x * wave.D.x * WA * S,
				// 			  -wave.Q * wave.D.x * wave.D.y * WA * S,
				// 			   wave.D.x * WA * C);

				// 	T += vec3(-wave.Q * wave.D.x * wave.D.y * WA * S,
				// 			  -wave.Q * wave.D.y * wave.D.y * WA * S,
				// 			   wave.D.y * WA * C);

				// 	N += vec3(-wave.D.x * WA * C,
				// 			  -wave.D.y * WA * C,
				// 			  -wave.Q * WA * S);
				// }

				mat3 modelVector = transpose(inverse(mat3(u_model)));
				vec4 worldPos = u_model * vec4(P, 1.0);

				gl_Position = u_proj * u_view * worldPos;
				frag_uv = uv;
				frag_pos = worldPos.xyz;
				// frag_B = normalize(modelVector * B);
				// frag_T = normalize(modelVector * T);
				// frag_N = normalize(modelVector * N);
			}
		)")
		.fragment(R"(
			#version 430

			in vec2 frag_uv;
			in vec3 frag_pos;
			// in vec3 frag_B;
			// in vec3 frag_T;
			// in vec3 frag_N;

			out vec4 outColor;

			uniform vec3 u_cameraPos;
			uniform vec3 u_sunDir;

			vec3 HDR(vec3 color, float exposure) {
				return 1.0 - exp(-color * exposure);
			}

			void main() {
				// for now just calc the normals numerically
				vec3 normal = normalize(cross(dFdx(frag_pos), dFdy(frag_pos)));

				vec3 view_dir = normalize(u_cameraPos - frag_pos);
				float fresnel = 0.02f + 0.98f * pow(1.f - dot(normal, view_dir), 5.f);

				vec3 sky_color = vec3(3.2f, 9.6f, 12.8f);
				vec3 ocean_color = vec3(0.004f, 0.016f, 0.047f);
				float exposure = 0.35f;

				vec3 sky = fresnel * sky_color;
				float diffuse = clamp(dot(normal, normalize(-u_sunDir)), 0.f, 1.f);

				vec3 water = (1.f - fresnel) * ocean_color * sky_color * diffuse;

				vec3 color = sky + water;

				outColor = vec4(HDR(color, exposure), 1.f);
			}
		)")
		.build()
		.compile();
}

void setupBoat() {
	Capsule capsule = MakeCapsule(32, 30, 10);

	for (int i = 0; i < capsule.index.size(); i += 3) {
		std::swap(capsule.index[i + 1], capsule.index[i + 2]);
	}

	boat = VertexArrayBuilder()
		.buffer(0).data(pack(capsule.pos, capsule.uvs))
		.index().data(capsule.index)
		.map(0)
			.attribute(0).type(AttributeTypeFloat, 3)
			.attribute(1).type(AttributeTypeFloat, 2)
		.build()
		.upload();

	boatShader = ShaderProgramBuilder()
		.vertex(R"(
			#version 430
			layout(location = 0) in vec3 pos;
			layout(location = 1) in vec2 uv;

			uniform mat4 u_model;
			uniform mat4 u_view;
			uniform mat4 u_proj;

			out vec2 frag_uv;

			void main() {
				gl_Position = u_proj * u_view * u_model * vec4(pos, 1.0);
				frag_uv = uv;
			}
		)")
		.fragment(R"(
			#version 430

			in vec2 frag_uv;

			out vec4 outColor;

			void main() {
				outColor = vec4(frag_uv, 0.0, 1.0);
			}
		)")
		.build()
		.compile();
}

// for bocyency, just calc some points under the ship
vec3 calcWaveheight(vec3 pos, float time) {
	float height = 0.f;
	vec2 posOnPlane = vec2(pos.x, pos.z);

	for (int i = 0; i < waveParamBuffer.bufferData().count(); i++) {
		WaveParams wave = waveParamBuffer.bufferData().at<WaveParams>(i);
		float angle = wave.w * dot(wave.D, posOnPlane) + wave.phi * time; 

		height += wave.A * sin(angle);
	}

	line(pos.x, pos.y, pos.z, pos.x, pos.y - height, pos.z);

	return vec3(pos.x, pos.y - height, pos.z);
}

void setup() {
	setupSkybox();
	setupOcean();
	setupBoat();

	size(1000, 1000);
	lens = lens_Perspective(1.2f, 1.f, 0.1f, 4000.f);
	lens.position = vec3(0, 0, 0);

	createAxis("Mouse Delta")
		.Map(MOUSE_VEL_X, vec2(1, 0))
		.Map(MOUSE_VEL_Y, vec2(0, 1));

	createAxis("Mouse Right")
		.MapButton(MOUSE_RIGHT);

	for (int i = 0; i < 32; i++) {
		float r = rand_fmm(0.1, 100.f);
		vec2 dir = rand_2fcn(1.);

		WaveParams wave = WaveParams(r, r/50.f, r/100.f, 2.f, dir);
		waves.push_back(wave);
	}

	waveParamBuffer = BufferBuilder()
		.type(BufferTypeShaderStorage)
		.data(waves)
		.build()
		.upload();
}

void cameraMove(CameraLens& lens) {
	vec3 move = vec3(0);

	if (keyDown[Key_W])          move.z += 1;
	if (keyDown[Key_S])          move.z -= 1;
	if (keyDown[Key_A])          move.x -= 1;
	if (keyDown[Key_D])          move.x += 1;
	if (keyDown[Key_Space])      move.y += 1;
	if (keyDown[Key_Left_Shift]) move.y -= 1;

	vec2 mouse = axis("Mouse Delta") * 0.8f;

	if (length(mouse) > 100) {
		return;
	}

	vec3 localR  = vec3(move.x,      0,       0) * lens.rotation;
	vec3 globalU = vec3(0,      move.y,       0);
	vec3 localF  = vec3(0,           0, -move.z) * lens.rotation;

	if (length(localR) > 0.f) localR = normalize(vec3(localR.x, 0, localR.z));
	if (length(localF) > 0.f) localF = normalize(vec3(localF.x, 0, localF.z));

	quat deltaP = angleAxis( mouse.x * deltaTime, vec3(0, 1, 0));
	quat deltaY = angleAxis(-mouse.y * deltaTime, vec3(1, 0, 0) * lens.rotation);

	lens.position += (localR + globalU + localF) * deltaTime * 100.f;
	lens.rotation *= deltaY * deltaP;
}

vec3 rotateXZ(vec3 xz, float angle) {
	float s = sin(angle);
	float c = cos(angle);

	return vec3(xz.x * c - xz.z * s, xz.y, xz.x * s + xz.z * c);
}

mat4 moveBoatOld() {
	vec3 boatHeadingVector = vec3(cos(boatHeading - _pi/2), 0.f, sin(boatHeading - _pi/2));

	if (keyDown[Key_Up])    boatPos += boatHeadingVector * deltaTime * 10.f;
	if (keyDown[Key_Down])  boatPos -= boatHeadingVector * deltaTime * 10.f;
	if (keyDown[Key_Right]) boatHeading += deltaTime * 1.f;
	if (keyDown[Key_Left])  boatHeading -= deltaTime * 1.f;

	vec3 hullSize = vec3(10, 10, 30) / 2.f;

	vec3 h00 = calcWaveheight(boatPos + rotateXZ(vec3(-1, 0, -1) * hullSize, boatHeading), totalTime);
	vec3 h01 = calcWaveheight(boatPos + rotateXZ(vec3(-1, 0,  1) * hullSize, boatHeading), totalTime);
	vec3 h10 = calcWaveheight(boatPos + rotateXZ(vec3( 1, 0, -1) * hullSize, boatHeading), totalTime);
	vec3 h11 = calcWaveheight(boatPos + rotateXZ(vec3( 1, 0,  1) * hullSize, boatHeading), totalTime);

	stroke(255);
	line(h00.x, h00.y, h00.z, h01.x, h01.y, h01.z);
	line(h00.x, h00.y, h00.z, h10.x, h10.y, h10.z);
	line(h01.x, h01.y, h01.z, h11.x, h11.y, h11.z);
	line(h10.x, h10.y, h10.z, h11.x, h11.y, h11.z);

	float havg = (h00.y + h01.y + h10.y + h11.y) / 4.f;

	// get the pitch
	vec3 front = (h00 + h01) / 2.f;
	vec3 back  = (h10 + h11) / 2.f;
	vec3 left  = (h00 + h10) / 2.f;
	vec3 right = (h01 + h11) / 2.f;

	vec3 forwardAxis = normalize(front - back);
	vec3 rightAxis   = normalize(right - left);

	stroke(255, 200, 200);
	line(front.x, front.y, front.z, back.x, back.y, back.z);
	line(left.x, left.y, left.z, right.x, right.y, right.z);

	stroke(0, 255, 0);
	line(boatPos.x, boatPos.y + 3, boatPos.z, boatPos.x + boatHeadingVector.x * 5, boatPos.y + boatHeadingVector.y * 5 + 3, boatPos.z + boatHeadingVector.z * 5);
	stroke(0, 0, 255);
	line(boatPos.x, boatPos.y + 3, boatPos.z, boatPos.x + forwardAxis.x * 5, boatPos.y + forwardAxis.y * 5 + 3, boatPos.z + forwardAxis.z * 5);
	stroke(255, 0, 0);
	line(boatPos.x, boatPos.y + 3, boatPos.z, boatPos.x + rightAxis.x * 5, boatPos.y + rightAxis.y * 5 + 3, boatPos.z + rightAxis.z * 5);

	stroke(100, 100, 255);
	line(boatPos.x, boatPos.y + 3, boatPos.z, boatPos.x + forwardAxis.x * 5, boatPos.y + 3, boatPos.z + forwardAxis.z * 5);

	stroke(255, 100, 100);
	line(boatPos.x, boatPos.y + 3, boatPos.z, boatPos.x + rightAxis.x * 5, boatPos.y + 3, boatPos.z + rightAxis.z * 5);

	float pitch = atan( (front.y - back.y) / hullSize.z );
	float roll  = atan( (right.y - left.y) / hullSize.x );

	quat rot = angleAxis(-boatHeading, vec3(0, 1, 0))
	         * angleAxis(-pitch, vec3(1, 0, 0))
	         * angleAxis(-roll, vec3(0, 0, 1));

	vec3 boatDisplayPos = vec3(boatPos.x, havg, boatPos.z);
	vec3 boatScale = vec3(1);
	quat boatRotation = rot;

	mat4 boatModelMatrix = mat4_cast(boatRotation);
	boatModelMatrix[0] *= boatScale.x;
	boatModelMatrix[1] *= boatScale.y;
	boatModelMatrix[2] *= boatScale.z;
	boatModelMatrix[3] = vec4(boatDisplayPos, 1);

	return boatModelMatrix;
}

mat4 moveBoat() {
	vec3 boatHeadingVector = vec3(cos(boatHeading - _pi/2), 0.f, sin(boatHeading - _pi/2));

	if (keyDown[Key_Up])    boatPos += boatHeadingVector * deltaTime * 10.f;
	if (keyDown[Key_Down])  boatPos -= boatHeadingVector * deltaTime * 10.f;
	if (keyDown[Key_Right]) boatHeading += deltaTime * 1.f;
	if (keyDown[Key_Left])  boatHeading -= deltaTime * 1.f;

	vec3 hullSize = vec3(10, 10, 30) / 2.f;

	vec3 front = calcWaveheight(boatPos + rotateXZ(vec3(0, 0, -1) * hullSize, boatHeading), totalTime);
	vec3 back  = calcWaveheight(boatPos + rotateXZ(vec3(0, 0,  1) * hullSize, boatHeading), totalTime);

	float heightAvg = (front.y + back.y) / 2.f;

	float height = front.y - back.y;
	float length = hullSize.z;

	float angle = atan(height / length);

	vec3 boatDisplayPos = vec3(boatPos.x, heightAvg, boatPos.z);
	vec3 boatScale = vec3(1);
	
	quat boatRotation = angleAxis(-boatHeading, vec3(0, 1, 0)) 
					  * angleAxis(angle, vec3(1, 0, 0));

	mat4 boatModelMatrix = mat4_cast(boatRotation);
	boatModelMatrix[0] *= boatScale.x;
	boatModelMatrix[1] *= boatScale.y;
	boatModelMatrix[2] *= boatScale.z;
	boatModelMatrix[3] = vec4(boatDisplayPos, 1);

	return boatModelMatrix;
}

void draw() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	float time = totalTime;// * (keyDown[Key_Q] ? 40.f : 1.f);

	lens.aspect = (float)width / (float)height;

	vec3 sinDir = vec3(.0, fmodf(totalTime/100.f, 1.f), -1);

	if (skyboxShader.use()) {
		skyboxShader.setf("u_aspect", lens.aspect);
		skyboxShader.setf("u_time", time);
		skyboxShader.setf9("u_cameraRotation", inverse(mat3_cast(lens.rotation)));
		skyboxShader.setf("u_cameraNearClipDistance", lens.GetNearClipPlaneDistance());
		skyboxShader.setf3("u_sun", sinDir);

		skybox.draw();
	}

	glClear(GL_DEPTH_BUFFER_BIT);

	if (waterShader.use()) {
		waterShader.setf16("u_model", waterModelMatrix);
		waterShader.setf16("u_proj", lens.GetProjectionMatrix());
		waterShader.setf16("u_view", lens.GetViewMatrix());
		waterShader.setf("u_time", time);

		waterShader.seti("u_waveCount", waveParamBuffer.bufferData().count());
		waveParamBuffer.bindBase(0);

		waterShader.setf3("u_cameraPos", lens.position);
		waterShader.setf3("u_sunDir", sinDir);

		mesh.draw();
	}

	mat4 boatModelMatrix = moveBoat();

	if (boatShader.use()) {
		boatShader.setf16("u_model", boatModelMatrix);
		boatShader.setf16("u_proj", lens.GetProjectionMatrix());
		boatShader.setf16("u_view", lens.GetViewMatrix());

		boat.draw();
	}

	if (button("Mouse Right")) {
		cameraMove(lens);
	}

	camera(lens);

	// draw an axis gizmo
	stroke(255, 0, 0);
	line(0, 0, 0, 10, 0, 0);
	stroke(0, 255, 0);
	line(0, 0, 0, 0, 10, 0);
	stroke(0, 0, 255);
	line(0, 0, 0, 0, 0, 10);

	text("WASD while Right Click to move", 0, 0);
}
