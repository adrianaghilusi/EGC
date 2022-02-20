#version 330

// Input
in vec2 texcoord;

// Uniform properties
uniform sampler2D texture_1;
uniform sampler2D texture_2;
// TODO(student): Declare various other uniforms
uniform int remix;

// Output
layout(location = 0) out vec4 out_color;


void main()
{
    // TODO(student): Calculate the out_color using the texture2D() function.

    vec4 ct1 = texture2D(texture_1, texcoord);
	vec4 ct2 = texture2D(texture_2, texcoord);
    if(remix==1){
        out_color = mix(ct1, ct2, 0.5f);
    }
    else{
        out_color=ct1;
    }
    
    if(out_color.a < 0.5){
         discard;
     }
    
}
