// GLSL mirror of colour.h: the same constants and maths, for shaders. Pulled in with
// #include "bencolour/colour.glsl" (glslc -I <libbencolour>/include).
#ifndef BENCOLOUR_COLOUR_GLSL
#define BENCOLOUR_COLOUR_GLSL

// sRGB <-> linear, IEC 61966-2-1 piecewise, inputs clamped to [0, 1]
float bc_srgb_eotf(float v) {
    v = clamp(v, 0.0, 1.0);
    return v <= 0.04045 ? v / 12.92 : pow((v + 0.055) / 1.055, 2.4);
}

float bc_srgb_oetf(float v) {
    v = clamp(v, 0.0, 1.0);
    return v <= 0.0031308 ? v * 12.92 : 1.055 * pow(v, 1.0 / 2.4) - 0.055;
}

vec3 bc_srgb_oetf(vec3 v) {
    return vec3(bc_srgb_oetf(v.r), bc_srgb_oetf(v.g), bc_srgb_oetf(v.b));
}

// Rec.709 relative luminance of linear sRGB
float bc_luminance(vec3 lin) {
    return dot(lin, vec3(0.2126, 0.7152, 0.0722));
}

// linear sRGB -> CIE XYZ, D65
vec3 bc_linear_rgb_to_xyz(vec3 lin) {
    return vec3(0.4124564 * lin.r + 0.3575761 * lin.g + 0.1804375 * lin.b,
                0.2126729 * lin.r + 0.7151522 * lin.g + 0.0721750 * lin.b,
                0.0193339 * lin.r + 0.1191920 * lin.g + 0.9503041 * lin.b);
}

// OKLab (Ottosson 2020) in two halves, so a stage that treats negative LMS differently keeps the same matrices:
// linear sRGB -> LMS, then cube-rooted LMS -> Lab.
vec3 bc_oklab_lms(vec3 lin) {
    return vec3(0.4122214708 * lin.r + 0.5363325363 * lin.g + 0.0514459929 * lin.b,
                0.2119034982 * lin.r + 0.6806995451 * lin.g + 0.1073969566 * lin.b,
                0.0883024619 * lin.r + 0.2817188376 * lin.g + 0.6299787005 * lin.b);
}

vec3 bc_oklab_from_lms_cbrt(vec3 c) {
    return vec3(0.2104542553 * c.x + 0.7936177850 * c.y - 0.0040720468 * c.z,
                1.9779984951 * c.x - 2.4285922050 * c.y + 0.4505937099 * c.z,
                0.0259040371 * c.x + 0.7827717662 * c.y - 0.8086757660 * c.z);
}

// Sign-preserving cube root, as colour.h's cbrtf, so negative LMS round-trips. Per channel, not one vec3 pow:
// Metal compiles the two differently and the pipeline's hashes are locked to the scalar form.
vec3 bc_linear_rgb_to_oklab(vec3 lin) {
    vec3 lms = bc_oklab_lms(lin);
    return bc_oklab_from_lms_cbrt(vec3(sign(lms.x) * pow(abs(lms.x), 1.0 / 3.0),
                                       sign(lms.y) * pow(abs(lms.y), 1.0 / 3.0),
                                       sign(lms.z) * pow(abs(lms.z), 1.0 / 3.0)));
}

vec3 bc_oklab_to_linear_rgb(vec3 lab) {
    float lp = lab.x + 0.3963377774 * lab.y + 0.2158037573 * lab.z;
    float mp = lab.x - 0.1055613458 * lab.y - 0.0638541728 * lab.z;
    float sp = lab.x - 0.0894841775 * lab.y - 1.2914855480 * lab.z;
    float l = lp * lp * lp, m = mp * mp * mp, s = sp * sp * sp;
    return vec3(4.0767416621 * l - 3.3077115913 * m + 0.2309699292 * s,
                -1.2684380046 * l + 2.6097574011 * m - 0.3413193965 * s,
                -0.0041960863 * l - 0.7034186147 * m + 1.7076147010 * s);
}

// HSV saturation: (max - min) / max, 0 for black
float bc_hsv_saturation(vec3 c) {
    float mx = max(max(c.r, c.g), c.b);
    float mn = min(min(c.r, c.g), c.b);
    return mx > 1e-5 ? (mx - mn) / mx : 0.0;
}


// GLSL only (no colour.h counterpart). Sub-LSB TPDF dither from a coordinate hash, stable frame to frame: two uniforms
// per channel give a triangular (-1, 1) LSB at 8 bits, and per-channel offsets make it colour noise, less visible than
// luma noise.
float bc_hash21(vec2 p) {
    p = fract(p * vec2(123.34, 456.21));
    p += dot(p, p + 78.233);
    return fract(p.x * p.y);
}
vec3 bc_tpdf_dither_rgb(vec2 coord) {
    float r1 = bc_hash21(coord + vec2(0.123, 0.456));
    float r2 = bc_hash21(coord + vec2(7.890, 1.230));
    float g1 = bc_hash21(coord + vec2(3.210, 6.540));
    float g2 = bc_hash21(coord + vec2(9.870, 0.420));
    float b1 = bc_hash21(coord + vec2(2.710, 5.830));
    float b2 = bc_hash21(coord + vec2(4.660, 8.910));
    return vec3(r1 - r2, g1 - g2, b1 - b2) * (1.0 / 255.0);
}

#endif
