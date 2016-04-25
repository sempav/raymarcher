/*
   This code isn't mine; I found it somewhere on glsl.heroku.com
   and now can't find out the author for the life of me.
*/

#version 120

#ifdef GL_ES
precision mediump float;
#endif

uniform vec3 camera_pos;
uniform vec3 camera_dir;
uniform vec3 camera_up;
uniform vec3 camera_right;
uniform float camera_eye_dist;
uniform float elapsed_time;

varying vec2 coord2d;

float sphere(in vec3 p, in float radius)
{
    return length(p) - radius;
}

float smin( float a, float b, float k)
{
    float h = clamp(0.5 + 0.5 * (b - a) / k, 0.0, 1.0);
    return mix(b, a, h) - k * h * (1.0 - h);
}


float map(vec3 p)
{
    const int MAX_ITER = 10;
    const float BAILOUT=4.0;
    float Power=6.0;

    vec3 v = p;
    vec3 c = v;

    float r=0.0;
    float d=1.0;
    for(int n=0; n<=MAX_ITER; ++n)
    {
        r = length(v);
        if(r>BAILOUT) break;

        float theta = acos(v.z/r);
        float phi = atan(v.y, v.x);
        d = pow(r,Power-1.0)*Power*d+1.0;

        float zr = pow(r,Power);
        theta = theta*Power;
        phi = phi*Power;
        v = (vec3(sin(theta)*cos(phi), sin(phi)*sin(theta), cos(theta))*zr)+c;
    }
    return 0.5*log(r)*r/d;
}

float DistanceField(vec3 p)
{
    //return smin(map(p), sphere(p, 1.0 / exp(0.1 * elapsed_time)), 0.5);
    return map(p);
}


void main( void )
{
    vec3 point, normal;
    vec3 ray_orig, ray_dir;
    ray_dir = camera_dir * camera_eye_dist + coord2d.x * camera_right + coord2d.y * camera_up;
    ray_dir = normalize(ray_dir);
    ray_orig = camera_pos;

    //vec2 pos = (gl_FragCoord.xy*2.0 - resolution.xy) / resolution.y;
    //vec3 camPos = vec3(cos(time*0.3), sin(time*0.3), 1.5);
    //vec3 camTarget = vec3(0.0, 0.0, 0.0);

    //vec3 camDir = normalize(camTarget-camPos);
    //vec3 camUp  = normalize(vec3(0.0, 1.0, 0.0));
    //vec3 camSide = cross(camDir, camUp);
    //float focus = 1.8;

    //vec3 rayDir = normalize(camSide*pos.x + camUp*pos.y + camDir*focus);
    //vec3 ray = camPos;
    vec3 rayDir = ray_dir;
    vec3 ray = camera_pos;
    float m = 0.0;
    float d = 0.0, total_d = 0.0;
    const int MAX_MARCH = 50;
    const float MAX_DISTANCE = 1000.0;
    for(int i=0; i<MAX_MARCH; ++i) {
        d = DistanceField(ray);
        total_d += d;
        ray += rayDir * d;
        m += 1.0;
        if(d<0.00001) { break; }
        if(total_d>MAX_DISTANCE) { total_d=MAX_DISTANCE; break; }
    }

    float c = (total_d)*0.0001;
    vec4 result = vec4( 1.0-vec3(c, c, c) - vec3(0.025, 0.025, 0.02)*m*0.8, 1.0 );
    gl_FragColor = result;
}

