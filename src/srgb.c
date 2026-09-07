#include "bencolour/colour.h"
#include <math.h>

float bc_srgb_eotf(float v)
{
    float s = copysignf(1.0f, v);
    float a = fabsf(v);
    float out = (a <= 0.04045f) ? a / 12.92f : powf((a + 0.055f) / 1.055f, 2.4f);
    return s * out;
}

float bc_srgb_oetf(float v)
{
    float s = copysignf(1.0f, v);
    float a = fabsf(v);
    float out = (a <= 0.0031308f) ? 12.92f * a : 1.055f * powf(a, 1.0f / 2.4f) - 0.055f;
    return s * out;
}

bc_linear_rgb_t bc_rgb_to_linear_rgb(bc_rgb_t in)
{
    bc_linear_rgb_t out;
    out.c[0] = bc_srgb_eotf(in.c[0]);
    out.c[1] = bc_srgb_eotf(in.c[1]);
    out.c[2] = bc_srgb_eotf(in.c[2]);
    return out;
}

bc_rgb_t bc_linear_rgb_to_rgb(bc_linear_rgb_t in)
{
    bc_rgb_t out;
    out.c[0] = bc_srgb_oetf(in.c[0]);
    out.c[1] = bc_srgb_oetf(in.c[1]);
    out.c[2] = bc_srgb_oetf(in.c[2]);
    return out;
}
