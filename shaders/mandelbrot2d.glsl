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
    int max_it = 255;
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
        return heatmap_color4(it, 0, max_it);
    }
}

void main(void) 
{
    float zoom = 2.0 + exp(elapsed_time);
    vec2 center = vec2(-0.77, 0.1 - 3e-6);
    //vec2 center = vec2(-1.48, 0.001);
    vec2 size = vec2(3.0, 3.0) / zoom;
    vec2 z = center + size * coord2d;
    vec2 disp = size * 1.001;
    disp.x *= cos(elapsed_time) - sin(elapsed_time);
    disp.y *= sin(elapsed_time) + cos(elapsed_time);
    vec3 col1 = mandel(z + vec2(disp.x, disp.y));
    vec3 col2 = mandel(z + vec2(-disp.x, disp.y));
    vec3 col3 = mandel(z + vec2(-disp.x, -disp.y));
    vec3 col4 = mandel(z + vec2(disp.x, -disp.y));
    gl_FragColor = vec4(0.25 * (col1 + col2 + col3 + col4), 1.0);
}
