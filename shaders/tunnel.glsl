#version 130

uniform vec3 camera_pos;
uniform vec3 camera_dir;
uniform vec3 camera_up;
uniform vec3 camera_right;
uniform float camera_eye_dist;
uniform float elapsed_time;

varying vec2 coord2d;

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

vec3 mandel(in vec2 z0)
{
    int it = 0;
    int max_it = 20;
    vec2 z = z0;
    for (; it < max_it && z.x * z.x + z.y * z.y < 4.0; ++it) {
        vec2 nz = z;
        nz = vec2(z.x * z.x - z.y * z.y + z0.x,
                  2 * z.x * z.y + z0.y);
        z = nz;
    }
    if (it == max_it) {
        return vec3(0.0);
    } else {
        it = max_it - it;
        return heatmap_color4(it, 0, max_it);
    }
}

vec3 texture(in vec2 z0)
{
    z0 = mod(z0 + 1.0, 2.0) - 1.0;
    float ang = elapsed_time;
    ang = 0 * 0.25 * 3.14159;
    float s = sin(ang);
    float c = cos(ang);
    vec2 rz = vec2(z0.x * c - z0.y * s,
                   z0.x * s + z0.y * c);
    z0 = rz;
    //z0 = 2.0 * z0;
    //return mandel(z0);
    if (abs(z0.x) > 0.6f || abs(z0.y) > 0.5f) {
        return vec3(0.0f);
    } else {
        return vec3(1.0f);
    }
}

vec3 get_pixel(in vec2 coord2d)
{
    vec2 center = vec2(0.0);
    //center.x += 0.5 * sin(1.0 * elapsed_time);
    //center.y += 0.5 * sin(2.0 * elapsed_time);
    vec2 size = vec2(1.0, 1.0);
    vec2 z = center + size * coord2d;
    float ang_mod_coeff = 20 * sin(elapsed_time * 0.314);
    vec2 r = vec2(1.0 / length(z) + elapsed_time, elapsed_time +
                                                  ang_mod_coeff * length(z) +
                                                  6 * atan(z.y, z.x) / 3.14159);
    float thresh = 0.3;
    if (length(z) < thresh) {
        r.x = 11.11 * length(z) + elapsed_time;
        r.y = r.y + 0.1 * ang_mod_coeff * 0.2 / length(z) - 0.1 * ang_mod_coeff * 0.2 / thresh;
    }
    return texture(r);
}

void main(void) 
{
    vec3 v = get_pixel(coord2d);
    float eps = 2.0/960.0/2.0;
    vec3 v1 = get_pixel(coord2d + vec2(eps, 0.0));
    vec3 v2 = get_pixel(coord2d + vec2(-eps, 0.0));
    vec3 v3 = get_pixel(coord2d + vec2(0.0, eps));
    vec3 v4 = get_pixel(coord2d + vec2(0.0, -eps));
    gl_FragColor = vec4(0.2 * (v + v1 + v2 + v3 + v4), 1.0);
}
