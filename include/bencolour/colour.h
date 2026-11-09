#ifndef BENCOLOUR_COLOUR_H
#define BENCOLOUR_COLOUR_H

#ifndef BC_API
#if defined(_WIN32) && defined(BENCOLOUR_SHARED)
#ifdef BENCOLOUR_BUILDING_LIB
#define BC_API __declspec(dllexport)
#else
#define BC_API __declspec(dllimport)
#endif
#elif defined(__GNUC__) || defined(__clang__)
#define BC_API __attribute__((visibility("default")))
#else
#define BC_API
#endif
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    // The loaded library's version, "MAJOR.MINOR.PATCH".
    BC_API const char* bc_version(void);

    // distinct C types so the compiler catches passing OKLab where RGB is wanted
    typedef struct
    {
        float c[3];
    } bc_rgb_t;
    typedef struct
    {
        float c[3];
    } bc_linear_rgb_t;
    typedef struct
    {
        float c[3];
    } bc_oklab_t;
    // OKLab in polar form: L, chroma, hue in degrees
    typedef struct
    {
        float c[3];
    } bc_oklch_t;

    // sRGB <-> linear, IEC 61966-2-1 piecewise, inputs clamped to [0, 1]
    BC_API bc_linear_rgb_t bc_rgb_to_linear_rgb(bc_rgb_t in);
    BC_API bc_rgb_t        bc_linear_rgb_to_rgb(bc_linear_rgb_t in);

    // Single-channel form of the same curve, for callers converting one
    // value at a time (a UI swatch, a scalar luminance) rather than a
    // full RGB triplet.
    BC_API float bc_srgb_eotf(float v); // sRGB -> linear
    BC_API float bc_srgb_oetf(float v); // linear -> sRGB

    // Rec.709 relative luminance of linear sRGB
    BC_API float bc_luminance(bc_linear_rgb_t in);

    // linear sRGB <-> OKLab (Ottosson 2020)
    BC_API bc_oklab_t      bc_linear_rgb_to_oklab(bc_linear_rgb_t in);
    BC_API bc_linear_rgb_t bc_oklab_to_linear_rgb(bc_oklab_t in);
    BC_API bc_oklab_t      bc_oklch_to_oklab(bc_oklch_t in);

    // perceptually uniform mix; t clamped to [0, 1], result is linear sRGB
    BC_API bc_linear_rgb_t bc_lerp_oklab(bc_linear_rgb_t a, bc_linear_rgb_t b, float t);

    // Planckian locus, Kim 2002 polynomial, 1500..15000 K
    // gains: (R/G, B/G) of the illuminant at T_kelvin, G = 1
    // cct:   T from cam_mul ratios r = cam_mul[R]/cam_mul[G], b = cam_mul[B]/cam_mul[G]
    BC_API void  bc_planckian_gains(float T_kelvin, float* r_out, float* b_out);
    BC_API float bc_planckian_cct(float r_ratio, float b_ratio);

    // Base tone curve on linear luminance: a log-logistic through midgrey (0.18), reaching 1.0 at `white` with a
    // non-zero slope so the top stops keep gradation; `contrast` is the log-log slope at midgrey. `toe` (0..1) deepens
    // the shadows: up to 0.4 extra stops of slope per stop below midgrey, easing in so there is no kink, nothing above
    // midgrey (midgrey itself darkens by at most 0.2 stop). Prepare once per parameter set, evaluate per sample.
    typedef struct
    {
        float c, s, m, toe;
    } bc_tone_t;
    BC_API bc_tone_t bc_tone_prepare(float contrast, float white, float toe);
    BC_API float     bc_tone_eval(bc_tone_t t, float y);

#ifdef __cplusplus
}
#endif

#endif
