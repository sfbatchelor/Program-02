#version 440 core

// this is how we receive the texture
uniform sampler2DRect tex0;

in vec2 Texcoord;

out vec4 outputColor;

uniform float uTime = 0.8;


#define t uTime
#define res vec2 (1560, 2130)

#define grey vec4(63./255., 63./255., 61./255., 1.)
#define black vec4(3./255., 3./255., 3./255., 1.)
#define white vec4(244./255., 244./255., 240./255., 1.)
#define lightGrey vec4(141./255., 140./255., 132./255., 1.)
#define lightBlack vec4(24./255., 24./255., 24./255., 1.)
#define PI 3.14159265359
const float EPSILON = 0.0001;

struct ray
{
    vec3 o,  d;
};


float random (in vec2 _st) {
    return fract(sin(dot(_st.xy,
                         vec2(12.9898,78.233)))*
        43758.5453123);
}

float noise (in vec2 _st) {
    vec2 i = floor(_st);
    vec2 f = fract(_st);

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

#define NUM_OCTAVES 5

float fbm ( in vec2 _st) {
    float v = 1.0;
    float a = 0.25;
    vec2 shift = vec2(100.0);
    // Rotate to reduce axial bias
    mat2 rot = mat2(cos(0.01), sin(0.01),
                    -sin(0.01), cos(0.010));
    for (int i = 0; i < NUM_OCTAVES; ++i) {
        v += a * noise(_st);
        _st = rot * _st * .25 + shift;
        a *=  7.5;
    }
    return v;
}


ray getRay(vec2 uv, vec3 camPos, vec3 lookAt, float zoom)
{


    ray a;
    a.o = camPos;
    vec3 forward = normalize(lookAt -camPos);
    vec3 right = cross(vec3(0,1,0),forward);
    vec3 up = cross(forward, right);
    vec3 c = a.o +forward*zoom;
    vec3 i = c + uv.x*right + uv.y*up;
    a.d = normalize(i-a.o);
    return a ;

}


vec3 layer(vec2 st)
{
  vec3 color = vec3(0.0);

    vec2 q = vec2(0.);
    q.x = fbm( st + 0.00*uTime);
    q.y = fbm( st + vec2(1.0));
    vec2 r = vec2(0.);
    r.x = fbm( st + 7.0*q + vec2(10.7,9.2)+ 1.15*uTime);
    r.y = fbm( st + 10.0*q + vec2(98.3,92.8)+ 2.126*uTime);
    float f = fbm(st+r);
    color = mix(vec3(0.201961,0.619608,0.266667),
                vec3(0.166667,0.666667,0.698039),
                clamp((f*f)*4.0,0.0,1.0));
    color = mix(color,
                vec3(0.876,0.6,0.644706),
                clamp(length(q),0.0,1.0));
    color = mix(color,
                vec3(0.166667,.3,.2),
                clamp(length(r.x),0.0,1.0));
	color = (f*f*f+.6*f*f+.5*f)*color;
	return color;
}

float sdSphere(vec3 p, vec3 pos, float radius)
{
    return length(p - pos) - radius;
}

float distMap(vec3 p)
{

	float d = sdSphere(p, vec3(0.0), 80.);

	return d;
}


// returns the normal direction of a point in the scene by taking samples around the point of size EPSILON.
vec3 worldNormal(vec3 p) {
    return normalize(vec3(
        distMap(vec3(p.x + EPSILON, p.y, p.z)) -  distMap(vec3(p.x - EPSILON, p.y, p.z)),
        distMap(vec3(p.x, p.y + EPSILON, p.z)) -  distMap(vec3(p.x, p.y - EPSILON, p.z)),
        distMap(vec3(p.x, p.y, p.z  + EPSILON)) - distMap(vec3(p.x, p.y, p.z - EPSILON))
    ));
}

// returns distance from ro (ray-origin) in the rd (ray-direction) direction. Returns -1 if no hit.
float worldIntersect(vec3 ro, vec3 rd )
{
    for(float t=0.0; t<50000.0; )
    {
        float h = distMap(ro + rd*vec3(t));
        if( h<0.0001 )
            return t;
        t += h.x;
    }
    return float(-1.0);
}

int worldSteps(vec3 ro, vec3 rd)
{
	int steps = 0;
    for(float t=0.0; t<50000.0; )
    {
        float h = distMap(ro + rd*vec3(t));
        if( h<0.0001 )
            break;
        t += h.x;
		steps += 1;
    }
    return steps;
}



vec4 backgroundNoiseLayer(vec2 st)
{

	vec2 lookup = Texcoord;
	vec2 p = vec2(0);
	p.x = fbm(lookup+uTime*140.001);
	p.y = fbm(lookup+uTime*300.2334);
	vec2 q =vec2(0);
	q.x = fbm(lookup + p*.5 + 0.15-uTime * 27);
	q.y = fbm(lookup + p*.5 + 10.15-uTime * 97);
	vec2 m =vec2(0);
	m.x = fbm(lookup + q*.5 + 230.15-uTime * 927);
	m.y = fbm(lookup + q*.5 + 10.15-uTime * 297);
	lookup = m;
	vec4 texCol = texture(tex0, lookup);

	vec4 col = vec4(0);
	col = pow(texCol,vec4(1.1));
	col = mix(lightBlack, black, length(col.b));
	return col;

}

void main() {

    vec2 st = (Texcoord.xy-1*res.xy)/res.y;
    vec3 colour= vec3(0);
	colour = backgroundNoiseLayer(Texcoord).rgb;

   float zoom = 4.75;
   vec3 camPos = vec3( -22.5, -96,-606.659);
   vec3 lookAt = camPos;
   lookAt.z += 1.;
   ray r  = getRay(st, camPos, lookAt, zoom);
	for(int i = 0; i < 10; i++)
	{

   			float intersect = worldIntersect( r.o, r.d);
			if (intersect != -1.) 	//if hit
			{
				//colour += worldNormal(r.o + r.d*vec3(intersect));
				vec2 lookup = Texcoord;
				lookup *= .7;
				lookup.x -= 200;
				vec4 texCol = texture(tex0, lookup );
				float maskCoeff =  texCol.z;
				float cutoff = 0.88;
				maskCoeff = smoothstep(cutoff - 0.1, cutoff +0.1, maskCoeff);
				texCol.rgb = mix(colour, texCol.rgb, maskCoeff);

				vec3 mask = worldSteps(r.o, r.d)* vec3(.018,0.018,0.018)*noise(vec2(uTime));
				colour = mix(colour, texCol.rgb, mask);

			}
	}





    outputColor = vec4(colour,1.);


}