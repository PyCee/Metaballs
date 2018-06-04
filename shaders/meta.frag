#version 450

layout(location=0) in vec2 frag_position;

layout(location=0) out vec4 frag_color;

layout(binding=0) uniform Metaball_Data {
	vec4 color_and_radius;
	vec2 position;
} ball;
layout(binding = 1) uniform sampler2D base;

float cutoff = 0.5;

void main(){
     vec2 diff = frag_position - ball.position;
     float sq_dist = dot(diff, diff);
     float radius = ball.color_and_radius.w;

     // Calculate color based on fragment distance from point
     //float distance_scale = radius / sq_dist;
     float denom = pow((4.787344 * sq_dist / (radius * radius)) + 0.98545329, 2);
     float distance_scale = (1 / denom) - 0.03;
     

     distance_scale = max(0.0f, min(1.0f, distance_scale));
     vec3 circle_color = vec3(ball.color_and_radius.rgb) * distance_scale;

     // Get color currently in the render-target
     vec2 texture_coords = (frag_position * 0.5f) + vec2(0.5f, 0.5f);
     vec3 base_color = texture(base, texture_coords).rgb;
     
     frag_color = vec4(circle_color.rgb + base_color.rgb*0.9, 1.0f);
     
     /*frag_color = vec4(base_color.r + circle_color.r,
     		//circle_color.g + base_color.g,
		//circle_color.b + base_color.b,
		0.0, 0.0,
		1.0f);*/
		
     //frag_color = vec4(base_color, 1.0f);
     //frag_color = vec4(circle_color, 1.0f);
     
}