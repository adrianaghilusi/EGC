#version 330

// Input
in vec2 texcoord;

// Uniform properties
uniform sampler2D texture;
// TODO(student): Declare various other uniforms

// Output
layout(location = 0) out vec4 out_color;


void main()
{

    vec4 ct1 = texture2D(texture, texcoord);
  
        out_color=ct1;
    
    
    if(out_color.a < 0.5){
         discard;
     }
    
}
