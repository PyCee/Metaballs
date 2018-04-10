#version 450

layout(location=0) in vec2 vert_position;

layout(location=0) out vec2 frag_position;
// TODO: this will not work as is
void main(){
     frag_position = vert_position;
}