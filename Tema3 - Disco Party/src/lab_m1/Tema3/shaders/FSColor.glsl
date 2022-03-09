#version 330

// Input
in vec4 frag_color;

uniform int type_of_light;
// Output
layout(location = 0) out vec4 out_color;

void main()
{
	if (type_of_light == 1) {
		out_color = vec4(frag_color.x, frag_color.y, frag_color.z, 0.5);
		if (out_color.a < 0.5) {
			discard;
		}

	}

}

