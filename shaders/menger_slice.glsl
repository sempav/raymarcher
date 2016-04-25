#version 130

#define eps 1e-5

uniform vec3 camera_pos;
uniform vec3 camera_dir;
uniform vec3 camera_up;
uniform vec3 camera_right;
uniform float camera_eye_dist;
uniform float elapsed_time;

varying vec2 coord2d;

struct Ray {
    vec3 dir;
    vec3 origin;
    vec3 dir_inv;
};

float sphere(in vec3 p, in float radius)
{
    return length(p) - radius;
}

float udBox(in vec3 p, in vec3 b)
{
    return length(max(abs(p) - 0.5 * b, 0.0));
}

float sdBox(in vec3 p, in vec3 b)
{
    vec3 d = abs(p) - 0.5 * b;
    return min(max(d.x,max(d.y,d.z)),0.0) + length(max(d,0.0));
}

float sdTorus(vec3 p, vec2 t)
{
    vec2 q = vec2(length(p.xy)-t.x, p.z);
    return length(q) - t.y;
}

float sdCylinder(vec3 p, vec3 c)
{
    return length(p.xy - c.xy) - c.z;
}

float myCylinder(vec3 p, float r, float h)
{
    return max(length(p.xy) - r, abs(p.z) - h);
}

float infCylinderZ(vec3 p, float r)
{
    return length(p.xy) - r;
}

float sdCross(in vec3 p)
{
    float da = sdBox(p.xyz,vec3(1.0/0.0,1.0,1.0));
    float db = sdBox(p.yzx,vec3(1.0,1.0/0.0,1.0));
    float dc = sdBox(p.zxy,vec3(1.0,1.0,1.0/0.0));
    return min(da,min(db,dc));
}

float sdPlane(vec3 p, float h)
{
    return p.y - h;
}

float udPlane(vec3 p, float h)
{
    return abs(p.y - h);
}

float sdPlaneGeneral(vec3 p, vec3 a)
{
    return dot(p, normalize(a));
}

vec3 rotate(vec3 p, float ang)
{
    float c = cos(ang * p.y);
    float s = sin(ang * p.y);
    mat2  m = mat2(c, -s, s, c);
    return vec3(m * p.xz, p.y);
}

float smin( float a, float b, float k )
{
    float h = clamp(0.5 + 0.5 * (b - a) / k, 0.0, 1.0);
    return mix(b, a, h) - k * h * (1.0 - h);
}

float menger(in vec3 p)
{
    float d = max(sdBox(p, vec3(1.0)), -sdPlaneGeneral(p, vec3(1.0, 1.0, 1.0)));
    d = max(d, -sdCross(3.0 * p) / 3.0);
    float mult = 1.0;
    for (int iter = 1; iter < 5; ++iter) {
        mult *= 3.0;
        vec3 modp = mod(mult * p + vec3(0.5), 1.0) - 0.5;
        float c = sdCross(3.0 * modp) / 3.0;
        d = max(d, -c / mult);
    }
    return d;
}

float DistanceField(in vec3 p)
{
    return menger(p);
}

vec3 GetIntersect(in vec3 ro, in vec3 rd, in float mint, in float maxt, out float occl)
{
    float h;
    occl = 0.0;
    for (float t = mint; t < maxt; ) {
        h = DistanceField(ro + t * rd);
        if (h < eps) {
            return ro + t * rd;
        }
        t += h;
        occl += 1.0 / (maxt - mint);
    }
    return vec3(0, 0, 0);
}

vec3 point_normal(in vec3 p)
{
#define NORMAL_EPS 1e-5
    float d = DistanceField(p);
    return normalize(vec3(DistanceField(p + vec3(NORMAL_EPS, 0, 0)) - d,
                          DistanceField(p + vec3(0, NORMAL_EPS, 0)) - d,
                          DistanceField(p + vec3(0, 0, NORMAL_EPS)) - d));
}

float shadow(in vec3 ro, in vec3 rd, float mint, float maxt, float k)
{
    float res = 1.0;
    for(float t=mint; t < maxt; )
    {
        float h = DistanceField(ro + rd * t);
        if(h < eps) {
            return 0.0;
        }
        res = min(res, k * h / t);
        t += h;
    }
    return res;
}

float norclamp(float x, float l, float h)
{
    return (clamp(x, l, h) - l) / (h - l);
}

//  L 0 1 2 H
// r 0 0 / 1
// g / 1 1 \
// b 1 \ 0 0
vec3 heatmap_color4(float val, float lo, float hi)
{
    if (val > hi || val < lo) {
        return vec3(1.0, 0.0, 1.0);
    }
    val = clamp(val, lo, hi);
    float m[3];
    for (int i = 0; i < 3; ++i) {
        m[i] = ((3 - i) * lo + (1 + i) * hi) / 4;
    }

    float b = 1.0 - norclamp(val, m[0], m[1]);
    float g = norclamp(val,   lo, m[0]) * (1.0 - norclamp(val, m[2],   hi));
    float r = norclamp(val, m[1], m[2]);
    vec3 col = vec3(r, g, b);
    return col;
    if (b == 1.0) {
        return vec3(1.0, 0.0, 1.0);
    }

    float speed = 1.5;
    float t = speed * mod(elapsed_time, 3.0 / speed);
    if (t < 1.0) {
        return vec3(col.r);
    } else if (t < 2.0) {
        return vec3(col.g);
    } else {
        return vec3(col.b);
    }
}

void main(void) 
{
    vec3 point, normal;
    vec3 ray_orig, ray_dir;
    ray_dir = camera_dir * camera_eye_dist + coord2d.x * camera_right + coord2d.y * camera_up;
    ray_dir = normalize(ray_dir);
    ray_orig = camera_pos;

    float occl;
    point = GetIntersect(ray_orig, ray_dir, 0.01, 100, occl);
    if (point == vec3(0)) {
        gl_FragColor = vec4(0);
        return;
    }

    normal = point_normal(point);
    vec3  light = camera_pos;
    vec3  light_dir = normalize(light - point);
    float dotp_diffuse = clamp(dot(light_dir, normal), 0, 1);

    //vec3 color = heatmap_color4(length(point), 0.5, 0.5 * 1.75);
    vec3 color = vec3(0.8);

    gl_FragColor = vec4(color * vec3(dotp_diffuse), 1);
}
