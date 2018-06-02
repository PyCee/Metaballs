#version 450

layout(location=0) in vec2 vert_position;

layout(location=0) out vec2 frag_position;

void main(){
     frag_position = vert_position;
     gl_Position = vec4(vert_position, 0.0, 1.0);
}