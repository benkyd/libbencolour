#include "bencolour/colour.h"

#define TABLE_MIN_K 1500.0f
#define TABLE_STEP 100.0f
#define TABLE_N 136

static float kTable[TABLE_N][2];
static int kTableReady = 0;

// Kim et al. 2002, T (Kelvin) -> CIE 1931 (x, y)
static void kelvin_to_xy(float T, float* xo, float* yo)
{
    float T2 = T * T;
    float T3 = T2 * T;
    float x;
    if (T <= 4000.0f)
    {
        x = -0.2661239e9f / T3 - 0.2343589e6f / T2 + 0.8776956e3f / T + 0.179910f;
        float x2 = x * x;
        float x3 = x2 * x;
        *yo = -1.1063814f * x3 - 1.34811020f * x2 + 2.18555832f * x - 0.20219683f;
    }
    else
    {
        x = -3.0258469e9f / T3 + 2.1070379e6f / T2 + 0.2226347e3f / T + 0.240390f;
        float x2 = x * x;
        float x3 = x2 * x;
        *yo = 3.0817580f * x3 - 5.87338670f * x2 + 3.75112997f * x - 0.37001483f;
    }
    *xo = x;
}

static void table_init(void)
{
    for (int i = 0; i < TABLE_N; i++)
    {
        float T = TABLE_MIN_K + i * TABLE_STEP;
        float x, y;
        kelvin_to_xy(T, &x, &y);
        float z = 1.0f - x - y;

        // D65-adapted XYZ -> linear sRGB; absolute scale drops out under G=1
        float R = 3.2404542f * x - 1.5371385f * y - 0.4985314f * z;
        float G = -0.9692660f * x + 1.8760108f * y + 0.0415560f * z;
        float B = 0.0556434f * x - 0.2040259f * y + 1.0572252f * z;

        if (R < 0.0f)
            R = 0.0f;
        if (G < 0.0f)
            G = 0.0f;
        if (B < 0.0f)
            B = 0.0f;

        if (G > 0.0f)
        {
            kTable[i][0] = R / G;
            kTable[i][1] = B / G;
        }
        else
        {
            kTable[i][0] = 1.0f;
            kTable[i][1] = 1.0f;
        }
    }
    kTableReady = 1;
}

// concurrent first calls write the same values into the same slots - benign
static void ensure_table(void)
{
    if (!kTableReady)
        table_init();
}

void bc_planckian_gains(float T_kelvin, float* r_out, float* b_out)
{
    ensure_table();

    if (T_kelvin < TABLE_MIN_K)
        T_kelvin = TABLE_MIN_K;
    if (T_kelvin > TABLE_MIN_K + (TABLE_N - 1) * TABLE_STEP)
        T_kelvin = TABLE_MIN_K + (TABLE_N - 1) * TABLE_STEP;

    float idx_f = (T_kelvin - TABLE_MIN_K) / TABLE_STEP;
    int idx = (int)idx_f;
    if (idx >= TABLE_N - 1)
        idx = TABLE_N - 2;
    float t = idx_f - (float)idx;

    *r_out = kTable[idx][0] * (1.0f - t) + kTable[idx + 1][0] * t;
    *b_out = kTable[idx][1] * (1.0f - t) + kTable[idx + 1][1] * t;
}

float bc_planckian_cct(float r_ratio, float b_ratio)
{
    if (r_ratio <= 0.0f || b_ratio <= 0.0f)
        return 6504.0f;

    ensure_table();

    // illuminant ratio is the inverse of the cam_mul gain that neutralises it
    float r_illum = 1.0f / r_ratio;
    float b_illum = 1.0f / b_ratio;

    float best_dist = 1e30f;
    int best = 0;
    for (int i = 0; i < TABLE_N; i++)
    {
        float dr = kTable[i][0] - r_illum;
        float db = kTable[i][1] - b_illum;
        float d = dr * dr + db * db;
        if (d < best_dist)
        {
            best_dist = d;
            best = i;
        }
    }

    if (best == 0)
        return TABLE_MIN_K;
    if (best == TABLE_N - 1)
        return TABLE_MIN_K + (TABLE_N - 1) * TABLE_STEP;

    float d_prev = (kTable[best - 1][0] - r_illum) * (kTable[best - 1][0] - r_illum) +
                   (kTable[best - 1][1] - b_illum) * (kTable[best - 1][1] - b_illum);
    float d_next = (kTable[best + 1][0] - r_illum) * (kTable[best + 1][0] - r_illum) +
                   (kTable[best + 1][1] - b_illum) * (kTable[best + 1][1] - b_illum);

    float T_best = TABLE_MIN_K + best * TABLE_STEP;
    // +1e-6 keeps NaN out if a future table grows near-duplicate rows
    if (d_prev < d_next)
        return T_best - TABLE_STEP * best_dist / (best_dist + d_prev + 1e-6f);
    else
        return T_best + TABLE_STEP * best_dist / (best_dist + d_next + 1e-6f);
}
