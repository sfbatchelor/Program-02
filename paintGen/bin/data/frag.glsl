#version 440 core

// this is how we receive the texture
uniform sampler2DRect tex0;

in vec2 Texcoord;

out vec4 outputColor;

uniform float uTime = 0.5;


#define t uTime
#define r vec2 (1920,1000)

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

#define NUM_OCTAVES 8

float fbm ( in vec2 _st) {
    float v = 0.;
    float a = .5;
    vec2 shift = vec2(100.0);
    // Rotate to reduce axial bias5
    mat2 rot = mat2(cos(0.5), sin(0.5),
                    -sin(0.5), cos(0.10));
    for(int i = 0; i < NUM_OCTAVES; ++i) {
        v += a * noise(_st);
        _st = rot * _st * 1.6 + shift;
        a *= 0.67;
    }
    return v;
}

void main()
{
// setup uv (with fbm shake)
    vec2 scaledUV = Texcoord.xy/r;
    scaledUV *= 7;

// one domain distortion
      vec2 q = vec2( fbm( scaledUV + vec2(0.0,0.0) +t*.5),
                     fbm( scaledUV + vec2(5.2,1.3) +t*.2) );

// two domain distortion
      vec2 c = vec2( fbm( scaledUV + 5.0*q + vec2(1.7,9.2) +0.*t),
                     fbm( scaledUV + 5.0*q + vec2(8.3,2.8) +0.3*t) );
// final value
    float f = fbm(scaledUV+c*7.0+t*.5);

// colour based on that value
    vec3 color = vec3(0.0);
    color = mix(vec3(0.91961,0.199608,0.966667),
                vec3(0.166667,0.166667,0.148039),
                clamp(f*f+f*r.x,0.0,1.0));

    color = mix(color,
                vec3(0.8,0.9,0.964706),
                clamp(length(q*r-f),0.0,1.0));

    color = mix(color,
                vec3(0.429,0.781,.91),
                clamp(fbm(vec2(q.y, f*q.x)),0.0,1.0));

    vec3 final  = (f*f*f*.9*f*f+.2*f)*color;
    outputColor = vec4(final, 1.0);
	

	// texture mask 
	vec4 texCol = texture(tex0, Texcoord);
	float maskCoeff = 1. - texCol.x;
	float cutoff = 0.55;
	maskCoeff = smoothstep(cutoff - 0.05, cutoff +0.05, maskCoeff);
	outputColor  = mix(outputColor, texCol, maskCoeff);


	outputColor = texCol;

}
