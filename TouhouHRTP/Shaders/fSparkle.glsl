#version 450 core
out vec4 FragColor;
in vec2 Coords;

uniform float time;
uniform float range;
uniform vec3 uCol;

mat2 Rot(float a)
{
	float c = cos(a);
	float s = sin(a);
	return mat2(c, -s, s, c);
}

float Hash21(vec2 p)
{
	p = fract(p*vec2(123.35, 456.21));
	p += dot(p, p+45.32);
	return fract(p.x*p.y);
}

float Star(vec2 uv, float flare)
{
	float d = length(uv);
	float m = .2/d;
	
	float rays = max(0., 1. - abs(uv.x*uv.y*500.));
	m += rays*flare;
	
	uv *= Rot(3.1415/4.);
	
	rays = max(0., 1. - abs(uv.x*uv.y*500.));
	m += rays*.3*flare;

	m *= smoothstep(1., .2, d);


	return m;
}

vec4 Stars(vec2 uv)
{
	vec3 col = vec3(0.);
	vec2 gv = fract(uv)-.5;
	vec2 id = floor(uv);
	float alpha = 0.;
	
	for (int y=-1; y<=1; y++)
		for (int x=-1; x<=1; x++)
		{
			vec2 offs = vec2(x,y);
			
			float n = Hash21(id+offs);
			float size = min(1., fract(n*321.85)+.3);
			
			float star = Star(gv-offs-vec2(fract(sin(n*time)*2.12), fract(n*time))+.1, smoothstep(.5, 1., size));
			//.425
			vec3 color = sin(uCol*fract(n*472.123)*range)*.5+.5;
			// .4 .1 .6
			star *= sin(time*20.+n*6.2831)*.5+1.;
			
			alpha += star*size;
			col += star*size*color;
		}

	return vec4(col, alpha);
}

void main()
{
	vec2 uv = Coords;
	uv *= .06;

	vec4 col = vec4(0);
	
	col += Stars(uv);
	col *= 1. - smoothstep(.9, 1.2, length(uv));

	FragColor = vec4(col);
}