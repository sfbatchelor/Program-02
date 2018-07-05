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

// random number generator vec2 to float repeating but efficent
float n21( vec2 p)
{
	p = fract(p*vec2(485.28, 871.94));
	p += dot(p, p+346.12);
	return fract(p.x*p.y);
}

// random number generator vec2 to vec2 repeating but efficent
vec2 n22( vec2 p)
{
	float n = n21(p);
	return vec2(n, n21(p+n));
}


float circle(in vec2 _st, in float _radius, in float c){
	float ar = uResolution.x/ uResolution.y;
    vec2 dist = _st - vec2(.5 *ar, .5);
	return 1.-smoothstep(_radius,
                         _radius+(_radius*c),
                         dot(dist,dist)*2.0);
}

float vignette(vec2 uv, vec2 dim, vec2 c)
{

	//dim = clamp(dim, vec2(0), vec2(.5));
	float topThresh =  .5 + dim.y;
	float bottomThresh = .5 - dim.y ;
	float top = smoothstep(  topThresh, topThresh - c.y, uv.y);  
	float bottom = smoothstep( bottomThresh, bottomThresh + c.y,uv.y);  
	float cir = circle(uv, dim.x, c.x*3);

	float mask = top * bottom * cir;
	mask /= pow(mask, .1); // little bit of a pow curve for spice
	
	return mask;
}

void main()
{


	vec2 uv = (Texcoord-.5)/uResolution.y; //texcoord y is flipped
	// unflip uv.y
	uv.y *= -1;
	uv.y += 1;
	vec4 texCol = texture(tex0, Texcoord);


	//b&w
	texCol.rgb = vec3(length(texCol.rgb)/3.);

	// vingette
	float vMask = vignette(uv, vec2(.5,.5), vec2(.8, .5) );
	//vec3 col = vec3(1 - vMask);
	vec3 col = vMask*texCol.rgb;


	//noise
	uv *= 300;
	vec2 id = floor(uv);
	float rand = n21(id+uTime*.000002);
	float n = noise(vec2(id.x * rand, id.y+uTime*2));
	col *= vec3(n);

	outputColor  = vec4(col, 1.0);



}
