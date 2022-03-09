#version 330

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;
layout(location = 3) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform vec4 Color;
uniform float time;

// Output
out vec4 frag_color;
out vec3 frag_normal;
out vec2 texcoord;


void main()
{
  
   texcoord = v_texture_coord;
   vec3 p2 = v_position;
   frag_color = Color;
    
    vec4 pos = vec4(v_position,1);
	float scale = 0.4;
    float frequency =3;
	float amt = scale * 0.5 * sin(v_position.y * frequency * time);
	vec4 dis = vec4(v_normal, 1.0);
	pos = vec4(v_position,1) + amt * dis;
    if(pos.y >0){
        pos.y -= (cos(time+3.14)+sin(time+3.14))/2 +time/100;
    }
    else{
    pos.y = 0;
    }
    if(pos.x <= v_position.x +0.5 && pos.x > v_position.x -0.5){
        pos.x -=sin(time+3.14)/2 +time/100;
    }
    else{
    pos.x = 0;
    }
    if(pos.z <= v_position.z +0.5 && pos.z > v_position.z -0.5){
        pos.z -=cos(time+3.14)/2 +time/100;
    }
    else{
    pos.z = 0;
    }
    
   gl_Position = Projection * View * Model * pos;
}
