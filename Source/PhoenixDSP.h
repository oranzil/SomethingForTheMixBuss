#pragma once
#include <cmath>

// Direct translation of JClones_Phoenix JSFX (v1.1.0), baked to:
//   Type = Dark Essence (4), Brightness = Sapphire (2),
//   Process = 37.98 %, Auto Gain = Off.
struct PhoenixChannel
{
    void prepare(double sampleRate)
    {
        sr_scale = 1.0 / std::floor(sampleRate / 44100.0);
        hpf_k = 0.375  * sr_scale;
        lpf_k = 0.5625 * sr_scale;
        reset();
    }

    void reset()
    {
        s      = 0.0;
        prev_x = 0.0;
    }

    // sat_type == 2: Gaussian reconstructed from Phoenix's lookup table.
    // EEL2 `(x*b)^5` preserves sign; std::abs on the result of bx^5 matches.
    static double sat(double x)
    {
        constexpr double a = 2.052;
        constexpr double b = 1.350;
        const double bx  = x * b;
        const double bx2 = bx * bx;
        const double bx5 = bx2 * bx2 * bx;   // bx^5, sign preserved
        return x * a / std::exp(std::abs(bx5));
    }

    // Phoenix_processSample with Type=4, g0=false, sat_type=2, auto_gain off.
    double process(double x)
    {
        // processing = kProcessing * kA3 = 0.3798 * 0.375
        constexpr double kProcessingScaled = 0.3798 * 0.375;
        constexpr double kF1  = 0.75;
        constexpr double kP20 = 0.5625;
        constexpr double kP24 = 0.0125;

        const double x1 = hpf_k * x + (x - prev_x);
        const double x2 = x1 * kF1 + x1;          // x1 * 1.75

        // g0 == false → x3 = x  (raw input, not x2)
        // model_type != 3 → x4 = sat(x2), not sat(x2 * processing)
        const double x4 = sat(x2);
        const double x5 = sat(x4 * kProcessingScaled * kP20 + x);

        prev_x = x;
        s += (x5 - s) * lpf_k;

        // model_type != 3 → no y *= 0.5
        double y = kProcessingScaled * (s - x * kP24);
        y += x;
        // auto_gain off → no multiply
        return y;
    }

    double sr_scale = 1.0;
    double hpf_k    = 0.0;
    double lpf_k    = 0.0;
    double s        = 0.0;
    double prev_x   = 0.0;
};
