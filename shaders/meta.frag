#version 450

layout(location=0) in vec2 frag_position;

layout(location=0) out vec4 frag_color;

layout(binding=0) uniform Metaball_Data {
	vec2 position;
	float radius;
	vec3 color;
} ball;

float r = 0.2;

void main(){
     
	vec2 diff = frag_position - ball.position;
	float sq_dist = dot(diff, diff);
	frag_color = vec4(vec3(sq_dist < (ball.radius * ball.radius)), 0.0);
     
     
     //frag_color = vec4(vec2(frag_position.x), 0.0, 1.0);   
}