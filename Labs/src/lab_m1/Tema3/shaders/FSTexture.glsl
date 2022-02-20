#version 330

// Input
in vec2 texcoord;
in vec3 world_position;
in vec3 world_normal;

// Uniform properties

uniform vec3 light_pos;
uniform int type_of_light;
uniform int time;
uniform sampler2D texture_1;



// Output
layout(location = 0) out vec4 out_color;


void main()
{
	vec3 light_dir = world_position - light_pos;
	vec2 newTex = texcoord;
	

	if (type_of_light == 2) {
		vec4 ct1 = texture2D(texture_1, newTex);

		out_color = ct1;


		if (out_color.a < 0.5) {
			discard;
		}
	}


}
