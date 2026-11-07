#include "bencolour/colour.h"
#include <math.h>

float bc_srgb_eotf(float v)
{
    if (v <= 0.0f)
        return 0.0f;
    if (v >= 1.0f)
        return 1.0f;
    return (v <= 0.04045f) ? v / 12.92f : powf((v + 0.055f) / 1.055f, 2.4f);
}

float bc_srgb_oetf(float v)
{
    if (v <= 0.0f)
        return 0.0f;
    if (v >= 1.0f)
        return 1.0f;
    return (v <= 0.0031308f) ? 12.92f * v : 1.055f * powf(v, 1.0f / 2.4f) - 0.055f;
}

float bc_luminance(bc_linear_rgb_t in)
{
    return 0.2126f * in.c[0] + 0.7152f * in.c[1] + 0.0722f * in.c[2];
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
