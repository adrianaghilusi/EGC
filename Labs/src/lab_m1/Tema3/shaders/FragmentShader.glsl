#version 330

// Input
in vec2 texcoord;
in vec3 world_position;
in vec3 world_normal;

// Uniforms for light properties
uniform vec3 light_position;
uniform vec3 c;
uniform vec3 light_direction;
uniform vec3 eye_position;
uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;
uniform sampler2D texture;
uniform vec3 object_color;
uniform int type_of_light;
uniform float time;
uniform float cut_off_angle;
uniform vec3 spot_position1;
uniform vec3 spot_position2;
uniform vec3 spot_position3;
uniform vec3 spot_position4;
uniform vec3 spot_position5;
uniform vec3 spot_direction;

// Output
layout(location = 0) out vec4 out_color;


void main()
{
	vec3 color;


	vec3 color1;
	float d = distance(world_position, light_position);

	if (type_of_light < 0) {

		float att = 0.f;
		if (d < 2) {
			att = pow(2 - d, 2);
		}
		if (att != 0) {
			color1 = object_color * att * c;
		}
		else {
			color1 = object_color * c;
		}

		color = color1;
	}


	if (type_of_light == 1) {
		float cut_off = radians(cut_off_angle);
		vec3 N = normalize(world_normal);


		vec3 L = normalize(spot_position1 - world_position);
		vec3 V = normalize(eye_position - world_position);
		vec3 H = normalize(L + V);
		vec3 R = reflect(-L, N);

		float spot_light = dot(-L, spot_direction);
		float diffuse_light = material_kd * 1 * max(dot(world_normal, L), 0);
		float specular_light = 0;

		if (diffuse_light > 0)
		{
			specular_light = material_ks * 0.1 * pow(max(dot(V, R), 0), material_shininess);
		}


		if (spot_light > cos(cut_off)) {

			float linear_att = (spot_light - cos(cut_off)) / (1.0f - cos(cut_off));
			float light_att_factor = pow(linear_att, 8);
			color += vec3(1, 0, 0) * (light_att_factor * (diffuse_light + specular_light));
		}
		else {
			color += vec3(1, 0, 0);
		}

		L = normalize(spot_position2 - world_position);
		V = normalize(eye_position - world_position);
		H = normalize(L + V);
		R = reflect(-L, N);

		spot_light = dot(-L, spot_direction);
		diffuse_light = material_kd * 1 * max(dot(world_normal, L), 0);
		specular_light = 0;

		if (diffuse_light > 0)
		{
			specular_light = material_ks * 0.1 * pow(max(dot(V, R), 0), material_shininess);
		}


		if (spot_light > cos(cut_off)) {

			float linear_att = (spot_light - cos(cut_off)) / (1.0f - cos(cut_off));
			float light_att_factor = pow(linear_att, 8);
			color += vec3(0, 1, 0) * (light_att_factor * (diffuse_light + specular_light));
		}
		else {
			color += vec3(0, 1, 0);
		}
		L = normalize(spot_position3 - world_position);
		V = normalize(eye_position - world_position);
		H = normalize(L + V);
		R = reflect(-L, N);

		spot_light = dot(-L, spot_direction);
		diffuse_light = material_kd * 1 * max(dot(world_normal, L), 0);
		specular_light = 0;

		if (diffuse_light > 0)
		{
			specular_light = material_ks * 0.1 * pow(max(dot(V, R), 0), material_shininess);
		}


		if (spot_light > cos(cut_off)) {

			float linear_att = (spot_light - cos(cut_off)) / (1.0f - cos(cut_off));
			float light_att_factor = pow(linear_att, 8);
			color += vec3(0, 0, 1) * (light_att_factor * (diffuse_light + specular_light));
		}
		else {
			color += vec3(0, 0, 1);
		}
		L = normalize(spot_position4 - world_position);
		V = normalize(eye_position - world_position);
		H = normalize(L + V);
		R = reflect(-L, N);

		spot_light = dot(-L, spot_direction);
		diffuse_light = material_kd * 1 * max(dot(world_normal, L), 0);
		specular_light = 0;

		if (diffuse_light > 0)
		{
			specular_light = material_ks * 0.1 * pow(max(dot(V, R), 0), material_shininess);
		}


		if (spot_light > cos(cut_off)) {

			float linear_att = (spot_light - cos(cut_off)) / (1.0f - cos(cut_off));
			float light_att_factor = pow(linear_att, 8);
			color += vec3(1, 1, 0) * (light_att_factor * (diffuse_light + specular_light));
		}
		else {
			color += vec3(1, 1, 0);
		}
		L = normalize(spot_position5 - world_position);
		V = normalize(eye_position - world_position);
		H = normalize(L + V);
		R = reflect(-L, N);

		spot_light = dot(-L, spot_direction);
		diffuse_light = material_kd * 1 * max(dot(world_normal, L), 0);
		specular_light = 0;

		if (diffuse_light > 0)
		{
			specular_light = material_ks * 0.1 * pow(max(dot(V, R), 0), material_shininess);
		}


		if (spot_light > cos(cut_off)) {

			float linear_att = (spot_light - cos(cut_off)) / (1.0f - cos(cut_off));
			float light_att_factor = pow(linear_att, 8);
			color += vec3(0, 1, 1) * (light_att_factor * (diffuse_light + specular_light));
		}
		else {
			color += vec3(0, 1, 1);
		}
	}

	if (type_of_light == 2) {
		vec3 light_dir = world_position - vec3(4, 4, 4);

		// texcoord este coordonata de textura utilizata pentru esantionare
		vec2 texcoord;
		texcoord.x = (1.0 / (2 * 3.14159)) * atan(light_dir.x, light_dir.z)+0.5;
		texcoord.y = (1.0 / 3.14159) * acos(light_dir.y / length(light_dir));
		
		// color este culoarea corespunzatoare pozitiei world_position
		vec4 tex = texture2D(texture, texcoord);
		color += tex.xyz;
	}

	out_color = vec4(color, 1);

}
