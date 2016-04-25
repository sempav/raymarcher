#version 130

#define eps 1e-5
#define min_step 1e-5
#define NORMAL_EPS 1e-5

//#define DRAW_PLANE
#define PLANE_Y 0.0

//#define REFLECTION
//#define SECOND_REFLECTION

uniform vec3 camera_pos;
uniform vec3 camera_dir;
uniform vec3 camera_up;
uniform vec3 camera_right;
uniform float camera_eye_dist;
uniform float elapsed_time;

varying vec2 coord2d;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

struct Ray {
    vec3 dir;
    vec3 origin;
    vec3 dir_inv;
};

struct Distance {
    float dist;
    vec3 color;
};

struct Intersection {
    vec3 p;
    vec3 color;
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
    float da = sdBox(p.xyz,vec3(1.0/0.0, 1.0, 1.0));
    float db = sdBox(p.yzx,vec3(1.0, 1.0/0.0, 1.0));
    float dc = sdBox(p.zxy,vec3(1.0, 1.0, 1.0/0.0));
    return min(da, min(db, dc));
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

float dot2(in vec3 v) {
    return dot(v,v);
}

float udTriangle( vec3 p, vec3 a, vec3 b, vec3 c )
{
    vec3 ba = b - a; vec3 pa = p - a;
    vec3 cb = c - b; vec3 pb = p - b;
    vec3 ac = a - c; vec3 pc = p - c;
    vec3 nor = cross( ba, ac );

    return sqrt(
            (sign(dot(cross(ba,nor),pa)) +
             sign(dot(cross(cb,nor),pb)) +
             sign(dot(cross(ac,nor),pc)) < 2.0)
            ?
            min(min(
                dot2(ba*clamp(dot(ba,pa)/dot2(ba),0.0,1.0)-pa),
                dot2(cb*clamp(dot(cb,pb)/dot2(cb),0.0,1.0)-pb)),
                dot2(ac*clamp(dot(ac,pc)/dot2(ac),0.0,1.0)-pc))
            :
            dot(nor,pa)*dot(nor,pa)/dot2(nor) );
}

vec3 swirl(vec3 p, float ang)
{
    float c = cos(ang * p.y);
    float s = sin(ang * p.y);
    mat2  m = mat2(c, -s, s, c);
    return vec3(m * p.xz, p.y);
}

vec3 rotateX(vec3 p, float ang)
{
    float c = cos(ang);
    float s = sin(ang);
    mat2  m = mat2(c, -s, s, c);
    return vec3(p.x, m * p.yz);
}

vec3 rotateX(vec3 p, float c, float s)
{
    mat2  m = mat2(c, -s, s, c);
    return vec3(p.x, m * p.yz);
}

vec3 rotateY(vec3 p, float ang)
{
    float c = cos(ang);
    float s = sin(ang);
    mat2  m = mat2(c, -s, s, c);
    return vec3(m * p.xz, p.y).xzy;
}

vec3 rotateY(vec3 p, float c, float s)
{
    mat2  m = mat2(c, -s, s, c);
    return vec3(m * p.xz, p.y).xzy;
}

vec3 rotateZ(vec3 p, float ang)
{
    float c = cos(ang);
    float s = sin(ang);
    mat2  m = mat2(c, -s, s, c);
    return vec3(m * p.xy, p.z);
}

vec3 rotateZ(vec3 p, float c, float s)
{
    mat2  m = mat2(c, -s, s, c);
    return vec3(m * p.xy, p.z);
}

float smin( float a, float b, float k )
{
    float h = clamp(0.5 + 0.5 * (b - a) / k, 0.0, 1.0);
    return mix(b, a, h) - k * h * (1.0 - h);
}

float iq_menger(in vec3 p)
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

float sierpinski(in vec3 p)
{
    vec3 a1 = vec3(1,1,1);
    vec3 a2 = vec3(-1,-1,1);
    vec3 a3 = vec3(1,-1,-1);
    vec3 a4 = vec3(-1,1,-1);

    vec3 c;
    int n = 0;
    float dist, d;
    int iterations = 8;
    float scale = 2.0;

    while (n < iterations) {
        // reflect across tetrahedron's symmetry planes
        if(p.x+p.y<0) p.xy = -p.yx;
        if(p.x+p.z<0) p.xz = -p.zx;
        if(p.y+p.z<0) p.zy = -p.yz;
        p = p*scale - a1*(scale-1.0);
        //p = a1 + rotateX(p - a1, 0.2 * sin(elapsed_time * n));

        n++;
    }

    return (length(p) - 1.6) * pow(scale, float(-n));
}

float kifs_menger(in vec3 p)
{
    vec3 a1 = vec3(1,1,1);

    vec3 c;
    int n = 0;
    float dist, d;
    int iterations = 4;
    float scale = 3;

    float cs = cos(0.05 + 0.05 * sin(elapsed_time));
    float sn = sin(0.05 + 0.05 * sin(elapsed_time));

    while (n < iterations) {
        // reflect across symmetry planes
        p = rotateX(p, cs, sn);

        p = abs(p);
        if (p.x - p.y < 0) {
            p.xy = p.yx;
        }
        if (p.x - p.z < 0) {
            p.xz = p.zx;
        }
        if (p.y - p.z < 0) {
            p.yz = p.zy;
        }

        int cnt = int(p.x < 1.0/3.0) + int(p.y < 1.0/3.0) + int(p.z < 1.0/3.0);
        if (cnt == 1) {
            p = abs(p - 1.0/3.0) + 1.0/3.0;
        }

        p = scale*p-a1*(scale-1.0);
        if (p.z < -0.5 * a1.z * (scale - 1.0)) {
            p.z += a1.z * (scale - 1.0); 
        }
        //p = p*scale - a1*(scale-1.0);
        //p = a1 + rotateX(p - a1, 0.2 * sin(elapsed_time * n));

        n++;
    }

    return (length(p) - 2.0) * pow(scale, float(-n));
}

float spincube(in vec3 p)
{
    float c = cos(0.5 * elapsed_time);
    float s = sin(0.5 * elapsed_time);

    vec3 rp = rotateY(p, c, s);
    rp = rotateX(rp, c, s);
    rp = rotateZ(rp, c, s);

    float d = sdBox(rp, vec3(3.1));
    if (d > 0.1) {
        return d;
    }

    vec3 mrp = mod(rp + 0.5, 1.0) - 0.5;
    d = max(d, sdBox(mrp, vec3(0.4)));
    //d = max(d, sphere(mrp, 0.2));
    d = max(d, -sdBox(rp, vec3(0.5)));
    return d;
}

float DistanceField(in vec3 p, out vec3 color)
{
    color = vec3(1.0f);
    return kifs_menger(p);

    float c = cos(1.0 * elapsed_time);
    float s = sin(1.0 * elapsed_time);
    float c2 = cos(0.5 * elapsed_time);
    float s2 = sin(0.5 * elapsed_time);
    p = rotateX(p, c, s);
    p = rotateY(p, c, s);
    p = rotateZ(p, c2, s2);

    //return kifs_menger(p);
    vec3 chess = 0.5 + 0.5 * sign(p - floor(p) - 0.5);
    color = vec3(mod(chess.x + chess.y + chess.z, 2.0));
    return sdTorus(p, vec2(3.0, 1.0));

    //float d = spincube(p);
    //float s = sphere(p, 6.0);
    //s = max(s, sdPlane(p, 0.0));
    //s = max(s, -sphere(p, 5.9));
    //d = min(d, s);
    //return d;

    //float t = -sphere(p, 10.0);
    //float t = sphere(p, 3.1);
    //t = max(t, -sdPlane(-p, 0.0));
    //t = max(t, -sphere(p - vec3(0.0, 0.1, 0.0), 3.1));
    //return min(t, d);

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

    
    //float r = sphere(p, 3.0);
    //r = max(r, -sphere(p, 2.995));
    //vec3 np = p;
    ////np.xy += 0.1 * elapsed_time;
    //np.x += 0.11 + 0.05 * sin(2 + 1.57 * elapsed_time);
    //np.y += 0.11 + 0.03 * sin(1 + 2.34 * elapsed_time);
    //np.z += 0.11 + 0.05 * sin(2 * elapsed_time);
    //r = max(r, -sdBox(mod(np + 0.77, 1.44) - vec3(0.77), vec3(1.4)));
    //r = min(r, sphere(p, 0.7 + 0.1 * sin(elapsed_time) + 0.05 * pow(sin(2 * elapsed_time), 2)));
    //return r;
    

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
    vec3 color;
    float i = DistanceField(p, color);
    float d = udPlane(p, PLANE_Y);
    if (d < i) {
        return d;
    } else {
        return i;
    }
}

Intersection GetIntersect(in vec3 ro, in vec3 rd, in float mint, in float maxt, out float occl)
{
    vec3 color;
    float h;
    occl = 0.0;
    for (float t = mint; t < maxt; ) {
        h = DistanceField(ro + t * rd, color);
        if (h < eps) {
            return Intersection(ro + t * rd, color);
        }
        t += h;
        occl += 1.0 / (maxt - mint);
    }
    return Intersection(vec3(0.0), vec3(0.0));
}

Intersection GetIntersectWithPlane(in vec3 ro, in vec3 rd, in float mint, in float maxt, out float occl)
{
    vec3 color;
    float h;
    occl = 0.0;

    float plane_t = (PLANE_Y - ro.y) / rd.y;
    if (plane_t > 0) {
        maxt = min(maxt, plane_t);
    }

    for (float t = mint; t < maxt; ) {
        h = DistanceField(ro + t * rd, color);
        if (h < eps) {
            return Intersection(ro + t * rd, color);
        }
        t += h;
        occl += 1.0 / (maxt - mint);
    }

    if (plane_t > 0) {
        return Intersection(ro + plane_t * rd, vec3(1.0));
    } else {
        return Intersection(vec3(0.0), vec3(0.0));
    }
}

vec3 point_normal(in vec3 p)
{
    vec3 color;
    float d = DistanceField(p, color);
    return normalize(vec3(DistanceField(p + vec3(NORMAL_EPS, 0, 0), color) - d,
                          DistanceField(p + vec3(0, NORMAL_EPS, 0), color) - d,
                          DistanceField(p + vec3(0, 0, NORMAL_EPS), color) - d));
}

float shadow(in vec3 ro, in vec3 rd, float mint, float maxt, float k)
{
    vec3 color;
    float res = 1.0;
    for(float t=mint; t < maxt; )
    {
        float h = DistanceField(ro + rd * t, color);
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

vec4 get_pixel_color(in vec3 ray_orig, in vec3 ray_dir)
{
    vec4 result_color = vec4(0);

    float occl;
    vec3 point, normal, mtl_color;
    Intersection i;
#ifdef DRAW_PLANE
    i = GetIntersectWithPlane(ray_orig, ray_dir, 0.01, 400, occl);
#else
    i = GetIntersect(ray_orig, ray_dir, 0.01, 400, occl);
#endif
    point = i.p;
    mtl_color = i.color;

#ifdef DRAW_PLANE
    if (abs(point.y - PLANE_Y) < eps && length(point) > eps) {
        float d = DistanceField(point);
        result_color = vec4(heatmap_color4(-d, -10, -0.01), 1);
        if (abs(d - floor(d)) < 2e-2) {
            result_color = vec4(vec3(0), 1);
        }
    }
    else
#endif
    if (point != vec3(0)) {

        normal = point_normal(point);
        //vec3 light = vec3(0.0 + 2.0 * sin(elapsed_time), 5.0f, -0.0 - 2.0 * cos(elapsed_time));
        vec3 light = camera_pos;
        //vec3 light = vec3(0.0, 0.0, 0.0);
        vec3 light_dir = normalize(light - point);
        float dotp_diffuse = clamp(dot(light_dir, normal), 0, 1);
        vec3 color = mtl_color;
        color *= dotp_diffuse;

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
        //color *= shadow(point, light_dir, 0.01, length(light - point), 100);
        color = clamp(color, 0.0, 1.0);

        result_color = vec4(color, 1);
    }

    return result_color;
}

void main(void) 
{
    vec3 ray_orig, ray_dir;
    ray_orig = camera_pos;

    ray_dir = camera_dir * camera_eye_dist + coord2d.x * camera_right + coord2d.y * camera_up;
    ray_dir  = normalize(ray_dir);

    gl_FragColor = get_pixel_color(ray_orig, ray_dir);
}
