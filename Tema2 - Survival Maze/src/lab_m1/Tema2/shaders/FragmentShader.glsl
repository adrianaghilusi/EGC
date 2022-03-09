#version 330

// Input
 in vec4 frag_color;

  
// Output
layout(location = 0) out vec4 out_color;

void main()
{
    out_color = frag_color;
         if(out_color.a < 0.5){
         discard;
     } 
}

