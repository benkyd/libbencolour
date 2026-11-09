#include "bencolour/colour.h"
#include <math.h>

// The toe's ease, in stops: how gradually it reaches its full slope below midgrey.
static const float kToeEase = 0.5f;

// f(y) = m * y^c / (y^c + s), with s and m solved so f(0.18) = 0.18 and f(white) = 1. Mirrored by colour.glsl.
bc_tone_t bc_tone_prepare(float contrast, float white, float toe)
{
    const float g0 = 0.18f;
    const float c  = contrast > 0.1f ? contrast : 0.1f;
    const float w  = white > g0 * 1.01f ? white : g0 * 1.01f;
    const float gc = powf(g0, c);
    const float wc = powf(w, c);
    // g0 * wc > gc holds whenever white is above midgrey, which the clamp above guarantees.
    const float s = gc * wc * (1.0f - g0) / (g0 * wc - gc);
    bc_tone_t   t = {c, s, (wc + s) / wc, 0.4f * (toe < 0.0f ? 0.0f : (toe > 1.0f ? 1.0f : toe))};
    return t;
}

float bc_tone_eval(bc_tone_t t, float y)
{
    if (y <= 0.0f)
        return 0.0f;
    const float yc  = powf(y, t.c);
    float       out = t.m * yc / (yc + t.s);
    if (t.toe > 0.0f)
    {
        // A soft min(stops below midgrey, 0) times the extra slope, applied in stops.
        const float x = log2f(y / 0.18f);
        out *= exp2f(-t.toe * kToeEase * log2f(1.0f + exp2f(-x / kToeEase)));
    }
    return out;
}
