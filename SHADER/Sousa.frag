#version 430

in vec2 pixel;

layout(binding = 0) uniform sampler2D SurfaceMap;

uniform int TAPS;
uniform float stepsize;
uniform float brightness;
uniform float near;
uniform float far;
uniform vec2 lightsource;

float LinearizeDepth(float depth) {
	float z = depth * 2.0 - 1.0; // Texspace [0,1] to NDC [-1,1]
	float tmp = (2.0 * near * far) / (far + near - z * (far - near));
	return tmp / far;
}

// Sousa Sun Shafts
//TODO
// 0) get sun in screenspace
// 1) invert depth map
// 2) radial blur on shadow map
// 3) need 1 or 2 temporary framebuffers
// 4) blur several times and vary radius
// 5) add inverted and blurred depth map on world image
//    soft blend shadows: http://www.pegtop.net/delphi/articles/blendmodes/softlight.htm
// Der Trick ist die Anzahl Samples N zu reduzieren, ohne Qualität zu verlieren.
// 3 Passes mit immer höherer Schrittweite und konstanten 6 Samples


vec4 RadialBlur(sampler2D image) {
	float brightness = .2; // aka exposure
	float blurriness = .5; // aka density in Mitchell's process
	vec2 origin = lightsource;
	vec2 v = pixel - origin; // Lightsource --> Pixel
	float sum = 0.0;
	int N = 10;
	for(int i = 0; i < N; i++) {
		float fraction = float(i) / float(N);
		// "scale" starts at 1.0 and goes toward 0.0
		float scale = 1.0 - blurriness * fraction;
		// "smpl" moves towards origin
		vec2 start = origin;
		vec2 stp = v * scale;
		float smpl = texture(image, start + stp).r;
		sum += smpl;
	}
	return vec4(sum * brightness);
}

vec4 Sousa(sampler2D image) {
	vec2 v = lightsource - pixel;
	float l = length(v);
	vec2 stp = stepsize * v / l;
	float MAX_STEPS = l / stepsize;
	float sum = 0.0;
	vec2 p = pixel;
	for(int i = 0; i < TAPS; i ++) {
		if(i >= MAX_STEPS) break;
		sum += texture(image, p).r;
		p += stp;
	}
	return vec4(sum * brightness);
}

void main() {
	gl_FragColor = Sousa(SurfaceMap);
}