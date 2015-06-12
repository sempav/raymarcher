#version 130

#define eps 1e-4
#define min_step 1e-6
#define NORMAL_EPS 1e-4

//#define DRAW_PLANE
#define PLANE_Y 0.0

#define REFLECTION
#define SECOND_REFLECTION

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
    float g = norclamp(val,   lo, m[0]) * (1.0 - norclamp(val, m[2], hi));
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
    float d = sdBox(p, vec3(1.0));
    if (d > 0.1) {
        return d;
    }
    d = max(d, -sdCross(3.0 * p) / 3.0);
    float mult = 1.0;
    for (int iter = 1; iter < 5; ++iter) {
        mult *= 3.0;
        vec3 modp = mod(mult * p + vec3(0.5), 1.0) - 0.5;
        float c = sdCross(3.0 * modp) / 3.0;
        d = max(d, -c / mult);
        if (d * mult > 0.1) {
            return d;
        }
    }
    return d;
}


float DistanceField(in vec3 p)
{
    return sphere(mod(p + 1.5, 3.0) - 1.5, 0.5);
    //return sdTorus(p, vec2(5.0, 1.0));
    //return menger(p);
    //float d = sphere(p, 2.0);
    //if (d < 1.0) {
    //    float o1 = 10 * sin(elapsed_time);
    //    float o2 = 10 * sin(1.3 * elapsed_time + 1);
    //    float o3 = 10 * sin(1.1 * elapsed_time + 0.3);
    //    d += 0.1 * sin(p.x * 10 + o1) * sin(p.y * 10 + o2) * sin(p.z * 10 + o3);
    //}
    //d = min(d, sdPlane(p, -5.0 + sin(elapsed_time)));
    //return d;

    /*
    float r = sphere(p, 1.0);
    r = max(r, -sphere(p, 0.995));
    vec3 np = p;
    //np.xy += 0.1 * elapsed_time;
    np.x += 0.11 + 0.05 * sin(2 + 1.57 * elapsed_time);
    np.y += 0.11 + 0.03 * sin(1 + 2.34 * elapsed_time);
    np.z += 0.11 + 0.05 * sin(2 * elapsed_time);
    r = max(r, -sdBox(mod(np + 0.11, 0.22) - vec3(0.11), vec3(0.2)));
    r = min(r, sphere(p, 0.9));
    return r;
*/
    /*
    float r = 100000;
    for (int i = 0; i < 8; ++i) {
        r = min(r, sphere(p - vec3(i * 1.0, sin(2 * elapsed_time + i), sin(2 * elapsed_time + i) + cos(3 * elapsed_time + i)), 0.5));
    }
    return r;
    */
/*
    float ang = atan(p.z, p.x);
    float rad = sqrt(p.x * p.x + p.z * p.z);
    float delta_phi = (2 * 3.14159) / 8;
    //obj to the left
    float obj_phi = floor(ang/delta_phi) * delta_phi;
    float obj_rad = 3.0;
    vec3 obj = vec3(obj_rad * cos(obj_phi),
                    cos(obj_phi),
                    obj_rad * sin(obj_phi));
    float d = sphere(p - obj, (1.0));
    //obj to the right
    obj_phi = ceil(ang/delta_phi) * delta_phi;
    obj = vec3(obj_rad * cos(obj_phi),
               cos(obj_phi),
               obj_rad * sin(obj_phi));
    d = min(d, sphere(p - obj, (1.0)));

    d = min(d, sdPlane(p, -5.0));
    //d = min(d, sphere(p, 2.0));
    return d;
*/
}

float DistanceFieldWithPlane(vec3 p)
{
    return min(DistanceField(p), udPlane(p, PLANE_Y));
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

vec3 GetIntersectWithPlane(in vec3 ro, in vec3 rd, in float mint, in float maxt, out float occl)
{
    float h;
    occl = 0.0;

    float plane_t = (PLANE_Y - ro.y) / rd.y;
    if (plane_t > 0) {
        maxt = min(maxt, plane_t);
    }

    for (float t = mint; t < maxt; ) {
        h = DistanceField(ro + t * rd);
        if (h < eps) {
            return ro + t * rd;
        }
        t += h;
        occl += 1.0 / (maxt - mint);
    }

    if (plane_t > 0) {
        return ro + plane_t * rd;
    } else {
        return vec3(0, 0, 0);
    }
}

vec3 point_normal(in vec3 p)
{
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

vec3 texture(vec3 p)
{
    return vec3(1.0);
   //return 0.5 + 0.5 * sin(10 * p);
    float value = sin(9 * p.x) + sin(9 * p.y) + sin(9 * p.z);
    return 0.5 + 0.5 * sign(vec3(value, value, -1.0));
}

void main(void) 
{
    vec3 point, normal;
    vec3 ray_orig, ray_dir;
    ray_dir = camera_dir * camera_eye_dist + coord2d.x * camera_right + coord2d.y * camera_up;
    ray_dir = normalize(ray_dir);
    ray_orig = camera_pos;

    float occl;
#ifdef DRAW_PLANE
    point = GetIntersectWithPlane(ray_orig, ray_dir, 0.01, 400, occl);
#else
    point = GetIntersect(ray_orig, ray_dir, 0.01, 400, occl);
#endif

    gl_FragColor = vec4(0);

#ifdef DRAW_PLANE
    if (abs(point.y - PLANE_Y) < eps && length(point) > eps) {
        float d = DistanceField(point);
        gl_FragColor = vec4(heatmap_color4(-d, -10, -0.01), 1);
        if (abs(d - floor(d)) < 2e-2) {
            gl_FragColor = vec4(vec3(0), 1);
        }
    }
    else
#endif
    if (point != vec3(0)) {

        normal = point_normal(point);
        //vec3 light = vec3(5.0 + 2.0 * sin(elapsed_time), 5.0f, -5.0 - 2.0 * cos(elapsed_time));
        vec3 light = camera_pos;
        vec3 light_dir = normalize(light - point);
        float dotp_diffuse = clamp(dot(light_dir, normal), 0, 1);
        vec3 color = texture(point);
        color *= dotp_diffuse;// * shadow(point, light_dir, 0.01, 100, 10);

        float specularCoefficient = 0.0;
        float material_shininess = 20.0;
        if (material_shininess > 0.0) {
            specularCoefficient = pow(max(0.0, dot(-ray_dir,
                                                   reflect(-light_dir, normal))), 
                                      material_shininess);
        }
        vec3 specular = specularCoefficient * vec3(1.0);

#ifdef REFLECTION
        // first reflection
        vec3 fst_color = vec3(0.0);
        vec3 fst_dir = reflect(ray_dir, normal);
        vec3 fst_point = GetIntersect(point, fst_dir, 0.01, 80, occl);
        if (fst_point != vec3(0)) {
            vec3 fst_normal = point_normal(fst_point);
            vec3 fst_light_dir = normalize(light - fst_point);
            float fst_diffuse = clamp(dot(fst_light_dir, fst_normal), 0, 1);
            fst_color = texture(fst_point);
            fst_color *= fst_diffuse;

#ifdef SECOND_REFLECTION
            vec3 snd_dir = reflect(fst_dir, fst_normal);
            vec3 snd_point = GetIntersect(fst_point, snd_dir, 0.01, 30, occl);
            vec3 snd_color = texture(snd_point);
            if (snd_point != vec3(0)) {
                vec3 snd_normal = point_normal(snd_point);
                vec3 snd_light_dir = normalize(light - snd_point);
                float snd_diffuse = clamp(dot(snd_light_dir, snd_normal), 0, 1);
                snd_color *= snd_diffuse;
            }

            fst_color = 0.5 * fst_color + 0.5 * snd_color;
#endif //SECOND_REFLECTION
            //if (material_shininess > 0.0) {
            //    specularCoefficient = pow(max(0.0, dot(-new_dir,
            //                    reflect(-new_light_dir, new_normal))), 
            //            material_shininess);
            //}
            //vec3 new_specular = specularCoefficient * vec3(1.0);
            //fst_color += new_specular;
        }

        color = 0.5 * color + 0.5 * fst_color;
#endif //REFLECTION
        color += specular;
        gl_FragColor = vec4(color, 1);
    }
}
