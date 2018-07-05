#version 440 core
#define s(a, b, t) smoothstep( a, b, t)

// this is how we receive the texture
uniform sampler2DRect tex0;

in vec2 Texcoord;

out vec4 outputColor;

uniform float uTime = 0.5;
uniform vec2 uResolution  = vec2(1280, 720);

float random (in vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(-20.5286,-4.43876)))    *
        50022.3);
}    

float noise (in vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    vec2 u = f * f * (3.0 - 2.0 * f);

    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}

// random number generator vec2 to float
float n21( vec2 p)
{
	p = fract(p*vec2(2345.28, 8721.94));
	p += dot(p, p+34.12);
	return fract(p.x*p.y);
}

// random number generator vec2 to vec2 
vec2 n22( vec2 p)
{
	float n = n21(p);
	return vec2(n, n21(p+n));
}

float vignette(vec2 uv, vec2 dim, float c)
{

	dim = clamp(dim, vec2(0), vec2(.5));
	float topThresh =  .5 + dim.y;
	float bottomThresh = .5 - dim.y ;
	float top = smoothstep(  topThresh, topThresh - c, uv.y);  
	float bottom = smoothstep( bottomThresh, bottomThresh + c,uv.y);  

	float mask = top * bottom;
	
	return mask;
}

void main()
{

	vec2 uv = (Texcoord/uResolution.xy); //texcoord y is flipped
	vec4 texCol = texture(tex0, Texcoord);

	// unflip uv.y
	uv.y *= -1;
	uv.y += 1.;
	float vMask = vignette(uv, vec2(.6), .5);
	vec3 col = vMask*texCol.rgb;

	outputColor  = vec4(col, 1.0);



}
