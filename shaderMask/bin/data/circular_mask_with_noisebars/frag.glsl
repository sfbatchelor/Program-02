#version 440 core
#define s(a, b, t) smoothstep( a, b, t)

// this is how we receive the texture
uniform sampler2DRect tex0;

in vec2 Texcoord;

out vec4 outputColor;

uniform float uTime = 0.5;
uniform vec2 uResolution  = vec2(1200,1600);

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

// nearest distance between a point and a line from startLine to endLine 
float distLine(vec2 p , vec2 startLine, vec2 endLine)
{
	vec2 pa = p - startLine;
	vec2 ba  = endLine - startLine;
	float t = clamp ( dot(pa, ba)/dot(ba, ba), 0., 1.0);
	return length(pa - ba*t);
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

// get position of point dependent on it's id
vec2 getPos( vec2 id, vec2 offset)
{
	vec2 n = n22(id + offset) * uTime;
	return offset + sin(n)*.4;
}

// draws a line between points
float line( vec2 p , vec2 a, vec2 b)
{
	float d = distLine(p, a, b);
	float m = s(.04, .01, d);
	m *= s(1.2, .8, length(a-b));
	return m;
}

vec4 plexus( vec2 uv)
{
	//vec2 uv = Texcoord/uResolution.xy;
//	float d = distLine(uv, vec2(0), vec2(1));

	uv *= 14;
	vec2 gv = fract(uv)- .5;
	vec2 id = floor(uv);

	vec2 pos = getPos(id, vec2(0));
	float d = length(gv - pos);
	float m = 1;s(.1, .05, d);

	// collect cells in kernel
	vec2 p[9];
	int i = 0;
	for( float y = -1.; y <= 1.; y++)
	{
		for( float x = -1.; x <= 1.; x++)
		{
			p[i++]= getPos(id, vec2 (x, y));
		}
	}
	// iterate through cells
	for ( int i =0; i  <9; i++)
	{
		m -= line( gv, p[4], p[i]);
		vec2 j = (p[i] - gv) *20.;
		float sparkle = 1. / dot(j,j); 
		//m -= sparkle * (sin(uTime +p[i].x*30) *.5 +.5);
	}

	m -= line( gv, p[1], p[3]);
	m -= line( gv, p[1], p[5]);
	m -= line( gv, p[7], p[3]);
	m -= line( gv, p[7], p[5]);

	vec4  col = vec4(vec3(m), 1);
	//col.rg = id *.04;
	//if(gv.x > .48|| gv.y > .48) col = vec3(1,0,0);
	col.rgb = 1. - col.rgb;
	return col;

}



vec4 shapes( vec2 uv)
{
	//vec2 uv = Texcoord/uResolution.xy;
//	float d = distLine(uv, vec2(0), vec2(1));

	uv *= 3;
	uv.y *= 15;

	vec2 offset = getPos(vec2(1), vec2(0));
	vec2 gv = fract(uv+offset)- .5;
	vec2 id = floor(uv+offset);
	vec2 pos = getPos(id, vec2(0));
	float d = length(gv - pos);
	float m = 1;s(.1, .05, d);

	float freq = sin(fract(uTime/3*id.x))*.000000001*(pow(id.y -1,1));
	float n = n21(vec2( freq, uv.y));

	vec4  col = vec4(n,n,n, 1);
	//col.rg = id *.04;
	//if(gv.x > .48|| gv.y > .48) col = vec4(1,0,0, 1);
	//col = 1. - col;
	float a = n21(vec2(d));
	//col.a = pow(a,1);
	//if(id.x > a || id.y >a) col.w = 0;
	//if(id.x < a-1 || id.y <a-1) col.w = 0;
	return col;

}


void main()
{

	//vec2 uv = Texcoord/uResolution.xy;
	vec2 uv = (Texcoord-.5*uResolution.xy)/uResolution.y;


	vec2 pos = vec2(0.5)-uv;
	vec2 offset = getPos(vec2(1), vec2(0));
	float r = length(pos+offset)*2.0;
	float a = atan(pos.y,pos.x);
    float f = cos(a*0.05);



//	vec3 col = plexus(uv);
	vec4 plexus = plexus(uv);
	vec4 shapes = shapes(vec2(.5)-uv );
	float mask = ( 1.-smoothstep(f,f+0.02,r) );
	vec4 col = mix(plexus, shapes, mask); 



	outputColor  = col;

}
