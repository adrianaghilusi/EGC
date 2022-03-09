#version 330

// Input
 in vec2 texcoord;
 in vec4 frag_color;
 in vec4 frag_normal;

 uniform sampler2D texture;

// Output
layout(location = 0) out vec4 out_color;
layout(location = 1) out vec4 out_normal;

void main()
{
    
vec4 ct1 = texture2D(texture, texcoord);
  
        out_color=ct1;
    
    
    if(out_color.a < 0.5){
         discard;
     }
}

