#include "bencolour/colour.h"
#include <math.h>

bc_oklab_t bc_linear_rgb_to_oklab(bc_linear_rgb_t in)
{
    float r = in.c[0], g = in.c[1], b = in.c[2];

    float l = 0.4122214708f * r + 0.5363325363f * g + 0.0514459929f * b;
    float m = 0.2119034982f * r + 0.6806995451f * g + 0.1073969566f * b;
    float s = 0.0883024619f * r + 0.2817188376f * g + 0.6299787005f * b;

    // cbrtf is sign-preserving so negative LMS round-trips
    float lp = cbrtf(l);
    float mp = cbrtf(m);
    float sp = cbrtf(s);

    bc_oklab_t out;
    out.c[0] = 0.2104542553f * lp + 0.7936177850f * mp - 0.0040720468f * sp;
    out.c[1] = 1.9779984951f * lp - 2.4285922050f * mp + 0.4505937099f * sp;
    out.c[2] = 0.0259040371f * lp + 0.7827717662f * mp - 0.8086757660f * sp;
    return out;
}

bc_linear_rgb_t bc_oklab_to_linear_rgb(bc_oklab_t in)
{
    float L = in.c[0], a = in.c[1], b = in.c[2];

    float lp = L + 0.3963377774f * a + 0.2158037573f * b;
    float mp = L - 0.1055613458f * a - 0.0638541728f * b;
    float sp = L - 0.0894841775f * a - 1.2914855480f * b;

    float l = lp * lp * lp;
    float m = mp * mp * mp;
    float s = sp * sp * sp;

    bc_linear_rgb_t out;
    out.c[0] = 4.0767416621f * l - 3.3077115913f * m + 0.2309699292f * s;
    out.c[1] = -1.2684380046f * l + 2.6097574011f * m - 0.3413193965f * s;
    out.c[2] = -0.0041960863f * l - 0.7034186147f * m + 1.7076147010f * s;
    return out;
}

bc_oklab_t bc_oklch_to_oklab(bc_oklch_t in)
{
    const float h = in.c[2] * (3.14159265358979f / 180.0f);
    bc_oklab_t out;
    out.c[0] = in.c[0];
    out.c[1] = in.c[1] * cosf(h);
    out.c[2] = in.c[1] * sinf(h);
    return out;
}

bc_linear_rgb_t bc_lerp_oklab(bc_linear_rgb_t a, bc_linear_rgb_t b, float t)
{
    if (t <= 0.0f)
        return a;
    if (t >= 1.0f)
        return b;
    bc_oklab_t la = bc_linear_rgb_to_oklab(a);
    bc_oklab_t lb = bc_linear_rgb_to_oklab(b);
    bc_oklab_t mix;
    mix.c[0] = la.c[0] + (lb.c[0] - la.c[0]) * t;
    mix.c[1] = la.c[1] + (lb.c[1] - la.c[1]) * t;
    mix.c[2] = la.c[2] + (lb.c[2] - la.c[2]) * t;
    return bc_oklab_to_linear_rgb(mix);
}
