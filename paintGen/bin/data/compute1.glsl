#version 440

struct Particle {
	vec4  pos;
	vec4 colour;
	float endTime;
	float speed;
	float strength;
};

layout(std140, binding=0) buffer particle{
    Particle p[];
};

layout(std140, binding=1) buffer indices{
	uint idx[];
};

 
layout(binding=0) uniform sampler2DRect src;
layout(binding=1) uniform sampler2DRect prev;
layout(rgba32f,binding=1) uniform image2D dst;

uniform float timeLastFrame;
uniform float elapsedTime;

uniform float max_speed;
uniform float min_speed;

uniform float max_strength;
uniform float min_strength;

uniform float max_lifetime;
uniform float min_lifetime;

uniform vec4 colour_a;

uniform int num_particles;

#define WIDTH 1200
#define HEIGHT 1600


float hash(float seed)
{
	return fract(sin(seed)*590932);
}

layout(local_size_x = 10) in;
void main(){

	vec4 imageCol = texture(src, ivec2(gl_GlobalInvocationID.xy));
	float endTime = p[gl_GlobalInvocationID.x].endTime;
	//reset particle if needed
	if(endTime <= elapsedTime)
	{
		float randVal = hash(elapsedTime + gl_GlobalInvocationID.x);
		p[gl_GlobalInvocationID.x].endTime = elapsedTime + mix(min_lifetime, max_lifetime, randVal); 
		randVal = hash(randVal*213545);
		p[gl_GlobalInvocationID.x].pos.x = mix(0, WIDTH, randVal);
		randVal = hash(randVal*3245);
		p[gl_GlobalInvocationID.x].pos.y = mix(0, HEIGHT, randVal);
		p[gl_GlobalInvocationID.x].pos.z = -10;
		p[gl_GlobalInvocationID.x].pos.w = 1;
		randVal = hash(randVal*24875);
		p[gl_GlobalInvocationID.x].speed = mix(min_speed, max_speed, randVal);
		randVal = hash(randVal*90245);
		p[gl_GlobalInvocationID.x].strength = mix(min_strength, max_strength, randVal);
	}

	ivec2 pos = ivec2( p[gl_GlobalInvocationID.x].pos.xy);
	int str = int(ceil(p[gl_GlobalInvocationID.x].strength));
	int halfStr = int(ceil(.5*str));

	// paint in a circle around pos
	vec4 dstCol = texture(prev, pos);
	dstCol = mix(dstCol, colour_a, 0.01);
	for(float x = pos.x - halfStr; x <= pos.x + halfStr; x++)
	{
		for(float y = pos.y - halfStr; y <= pos.y + halfStr; y++)
		{
			vec2 uv = vec2(x, y);
			float dist =  distance(ivec2(x,y) , pos);
			float coeff = dist/halfStr;
			dstCol *= coeff;
			imageStore(dst, ivec2(x,y), dstCol);
		}
	}

}