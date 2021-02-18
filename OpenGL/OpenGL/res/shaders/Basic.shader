#shader vertex
#version 330 core

layout(location = 0) in vec4 position;

void main()
{
	glPosition = position;
};

#shader fragment
#version 330 core

#ifdef GL_ES
precision mediump float;
#endif

uniform vec2 u_resolution;
uniform vec4 u_color;
uniform float u_time;

layout(location = 0) out vec4 color;

const int AMOUNT = 6;

void main() {
	vec2 pos = 20.0 * (gl_FragCoord.xy - u_resolution / 2.0) / min(u_resolution.y, u_resolution.x);

	float len;

	for (int i = 0; i < AMOUNT; i++) {
		len = length(vec2(pos.x, pos.y));

		pos.x = pos.x - sin(pos.y + cos(cos(len) * 2.0)) + cos(u_time / 20000.0);
		pos.y = pos.y - cos(pos.x + sin(cos(len) * 2.0)) + sin(u_time / 20000.0);
	}

	color = vec4(cos(len / 2.0), sin(len / 2.0) + .0, sin(len * 0.5) + 0.8, 1.0);

	// we can have a seperate color, obviously. so comment out the gl_FragColor above & uncomment the lines below
	// vec3 color = vec3(cos(len * 3.0), cos(len * 4.0), cos(len * 2.0));
	// gl_FragColor = vec4(color, 1.0);
}

