#version 450

layout(location=0) in vec2 frag_position;

layout(location=0) out vec4 frag_color;

layout(binding=0) uniform Metaball_Data {
	vec4 color_and_radius;
	vec2 position;
} ball;
layout(binding = 1) uniform sampler2D base;

in vec4 gl_FragCoord;

void main(){
     float r = 0.3;
     vec2 diff = frag_position - ball.position;
     float sq_dist = dot(diff, diff);
     float sq_radius = ball.color_and_radius.w * ball.color_and_radius.w;
     
     float scale = float(sq_dist <= sq_radius);
     //float scale = (sq_radius - sq_dist) / sq_radius;
     
     frag_color = vec4(texture(base, (frag_position / 2.0) +0.5).rgb +
     vec3(ball.color_and_radius.xyz) * scale, 0.0);
     
}