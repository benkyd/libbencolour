#include "bencolour/colour.h"
#include <math.h>

// f(y) = m * y^c / (y^c + s), with s and m solved so f(grey) = 0.18 and f(white) = 1. Mirrored by colour.glsl.
bc_tone_t bc_tone_prepare(float lift, float contrast, float white)
{
    const float g0   = 0.18f;
    const float c    = contrast > 0.1f ? contrast : 0.1f;
    const float grey = g0 * exp2f(-lift);
    const float w    = white > grey * 1.01f ? white : grey * 1.01f;
    const float gc   = powf(grey, c);
    const float wc   = powf(w, c);
    // g0 * wc > gc holds whenever white is above grey, which the clamp above guarantees.
    const float s = gc * wc * (1.0f - g0) / (g0 * wc - gc);
    bc_tone_t   t = {c, s, (wc + s) / wc};
    return t;
}

float bc_tone_eval(bc_tone_t t, float y)
{
    if (y <= 0.0f)
        return 0.0f;
    const float yc = powf(y, t.c);
    return t.m * yc / (yc + t.s);
}
