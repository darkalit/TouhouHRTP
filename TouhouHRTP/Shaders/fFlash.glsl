#version 450 core
out vec4 FragColor;
in vec2 Coords;

uniform float time;

mat2 Rot(float a)
{
	float c = cos(a);
	float s = sin(a);
	return mat2(c, -s, s, c);
}

void main()
{
    vec2 uv = Coords / 15.;
    float anim = uv.x;
    //uv *= Rot(3.1415 / 4.);
    vec2 R = vec2(.02, .2);
    vec2 P = uv;
    vec3 col = vec3(0.);
    float m = abs(uv.x) + .3;
    //col += m;
    float n = abs(uv.x) - .4;
    //col += n;
    P.y *= R.x / R.y;
    float e = dot(P, P);
    //col += e;
    //anim = sin(time) * 2.;
	//col += smoothstep(.5, 1.1, length(vec2(uv.x * .4, uv.y)));
	col += abs(0.1 / smoothstep(.5, 1.1, length(vec2(uv.x * .4, uv.y))));
    //col = 1. - col;
    n = abs(uv.x - time) * 1.;
    col -= n;
	
	//abs(0.05 / smoothstep(.2, .4, length(uv)) * 2.)
    //col *= abs(0.05 / (sin( uv.x + sin(uv.y+time)* 0.3 ) * 20.0) );
	FragColor = vec4(col, col.x);
}