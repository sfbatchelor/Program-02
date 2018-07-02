#version 440 core

// this is how we receive the texture
uniform sampler2DRect tex0;

in vec2 Texcoord;

out vec4 outputColor;

uniform float uTime = 0.5;


#define t uTime
#define res vec2 (1560, 2130)


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
    float v = 0.0;
    float a = 0.5;
    vec2 shift = vec2(100.0);
    // Rotate to reduce axial bias
    mat2 rot = mat2(cos(0.5), sin(0.5),
                    -sin(0.5), cos(0.50));
    for (int i = 0; i < NUM_OCTAVES; ++i) {
        v += a * noise(_st);
        _st = rot * _st * 2.0 + shift;
        a *= 0.5;
    }
    return v;
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
    color = mix(vec3(0.701961,0.619608,0.266667),
                vec3(0.966667,0.266667,0.398039),
                clamp((f*f)*4.0,0.0,1.0));
    color = mix(color,
                vec3(0.876,0.3,0.164706),
                clamp(length(q),0.0,1.0));
    color = mix(color,
                vec3(0.666667,.4,1),
                clamp(length(r.x),0.0,1.0));
	color = (f*f*f+.6*f*f+.5*f)*color;
	return color;
}

void main() {
    vec2 st = (Texcoord.xy-.5*res.xy)/res.y;
    vec3 color= vec3(0);

	for(float i=0; i<1.; i+= 1./4.)
	{
		float z = fract(i+uTime*.1);
		float size = mix(5., .1, z);
		float fade = smoothstep(0., .5, z)*smoothstep(1., .8,z);
		color += layer(st*size+i)*fade*.5;
	}



    outputColor = vec4(color,1.);
}