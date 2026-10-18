#pragma once
#include <algorithm>
#include <array>
#include <cmath>

// An independent, JUCE-free DSP building block. These are original
// analogue-inspired structures, not circuit-accurate commercial amplifier clones.
namespace ironvein::amp
{
constexpr float pi2 = 6.28318530718f;

inline float saturate (float x) noexcept { return std::tanh (x); }
inline float diode (float x) noexcept
{
    return x >= 0.0f ? std::tanh (1.1f * x) : 0.82f * std::tanh (1.43f * x);
}
inline float limit (float x, float maxValue) noexcept
{
    return std::clamp (x, -maxValue, maxValue);
}
inline float alpha (float hz, float sampleRate) noexcept
{
    return 1.0f - std::exp (-pi2 * std::clamp (hz, 10.0f, sampleRate * 0.42f) / sampleRate);
}

struct State
{
    std::array<float, 12> lp {};
    float envelope = 0.0f;
    float dcInput = 0.0f;
    float dcOutput = 0.0f;
};

struct Coefficients
{
    float sub = 0.0f, bass = 0.0f, lowMid = 0.0f;
    float mid = 0.0f, presence = 0.0f, treble = 0.0f;
    float interstage = 0.0f, tone = 0.0f;
    float envelopeAttack = 0.0f, envelopeRelease = 0.0f;
    float dc = 0.0f;
};

inline Coefficients makeCoefficients (int model, float tightHz, float toneHz, float sampleRate)
{
    // Each topology uses its own fixed interstage frequencies; the user controls
    // introduce further bass cut and treble damping without destroying the topology.
    const float nativeSub[5] = {245.0f, 95.0f, 65.0f, 135.0f, 190.0f};
    const float bass[5] = {410.0f, 420.0f, 560.0f, 460.0f, 680.0f};
    const float lowMid[5] = {950.0f, 870.0f, 680.0f, 1100.0f, 950.0f};
    const float mid[5] = {2000.0f, 2400.0f, 1700.0f, 2800.0f, 3350.0f};
    const float upper[5] = {4700.0f, 3600.0f, 3300.0f, 4000.0f, 4700.0f};
    const float cap[5] = {12500.0f, 9000.0f, 7900.0f, 8500.0f, 8300.0f};
    model = std::clamp (model, 0, 4);
    const auto hz = [sampleRate] (float f) { return alpha (f, sampleRate); };
    return {
        hz (std::max (nativeSub[model], tightHz)),
        hz (bass[model]),
        hz (lowMid[model]),
        hz (mid[model]),
        hz (upper[model]),
        hz (cap[model]),
        hz (model == 4 ? 590.0f : model == 3 ? 260.0f : 110.0f),
        hz (std::min (cap[model], toneHz)),
        hz (20.0f),
        hz (1.4f),
        std::exp (-pi2 * 12.0f / sampleRate)
    };
}

inline float lowpass (float x, float c, float& memory) noexcept
{
    memory += c * (x - memory);
    return memory;
}
inline float highpass (float x, float c, float& memory) noexcept
{
    return x - lowpass (x, c, memory);
}

// The five processors below intentionally use DIFFERENT circuits and stage
// orderings. Guitar-pick dynamics, clipping knee, band emphasis, and interstage
// filtering all differ, not simply an EQ curve attached to one waveshaper.
inline float process (int model, float x, State& s, const Coefficients& c) noexcept
{
    auto& z = s.lp;
    float y = 0.0f;
    switch (model)
    {
        case 0: // Raw black metal: trebly transistor fuzz / gated square edges.
        {
            const float thin = highpass (x, c.sub, z[0]);
            const float upper = highpass (thin, c.mid, z[1]);
            const float fuzz = limit (thin * 1.95f + upper * 1.6f, 0.58f);
            const float serrated = fuzz * (0.92f + 0.08f * std::cos (6.0f * thin));
            // Limited low mids and a forward harsh midband.
            y = 0.76f * serrated - 0.18f * lowpass (serrated, c.bass, z[2]);
            break;
        }
        case 1: // Swedish chainsaw: clipping pedal followed by extreme two-band EQ.
        {
            const float hp = highpass (x, c.sub, z[0]);
            const float low = lowpass (hp, c.bass, z[1]);
            const float mids = lowpass (hp, c.mid, z[2]) - low;
            const float upper = hp - lowpass (hp, c.mid, z[3]);
            const float driven = 0.85f * hp + 3.3f * mids + 2.0f * upper;
            const float pedal = limit (diode (2.6f * driven), 0.62f);
            const float grinderLow = lowpass (pedal, c.lowMid, z[4]);
            const float grinderHigh = pedal - lowpass (pedal, c.presence, z[5]);
            const float grind = pedal + 0.9f * (pedal - grinderLow) + 0.75f * grinderHigh;
            y = 0.64f * saturate (1.35f * grind);
            break;
        }
        case 2: // Old-school: two dynamic triode-ish stages and slow sag.
        {
            const float hp = highpass (x, c.sub, z[0]);
            const float body = lowpass (hp, c.lowMid, z[1]);
            const float v1 = diode (1.28f * (0.72f * hp + 0.55f * body));
            const float inter = highpass (v1, c.interstage, z[2]);
            const float envelopeInput = std::abs (inter);
            s.envelope += (envelopeInput > s.envelope ? c.envelopeAttack : c.envelopeRelease)
                           * (envelopeInput - s.envelope);
            const float sag = 1.0f / (1.0f + 0.67f * s.envelope);
            const float v2 = diode (1.8f * inter * sag);
            y = 0.83f * v2 + 0.11f * inter;
            break;
        }
        case 3: // Death metal: boost -> clip -> filtered 3-stage high gain.
        {
            const float hp = highpass (x, c.sub, z[0]);
            const float pedal = diode (1.38f * hp);
            const float first = saturate (2.3f * pedal);
            const float between = highpass (first, c.interstage, z[1]);
            const float second = diode (2.5f * between);
            const float dense = highpass (second, c.bass, z[2]);
            const float third = saturate (2.45f * dense);
            const float presence = third - lowpass (third, c.presence, z[3]);
            y = 0.72f * third + 0.22f * presence;
            break;
        }
        case 4: // Modern metalcore: mid-hump overdrive -> tight multistage preamp.
        {
            const float hp = highpass (x, c.sub, z[0]);
            const float bottom = lowpass (hp, c.bass, z[1]);
            const float middle = lowpass (hp, c.presence, z[2]) - bottom;
            const float boost = hp + 1.55f * middle - 0.7f * bottom;
            const float overdrive = saturate (1.6f * boost);
            const float stage1 = diode (1.7f * overdrive);
            const float thinned = highpass (stage1, c.interstage, z[3]);
            const float stage2 = limit (2.4f * thinned, 0.85f);
            const float finalStage = saturate (2.15f * highpass (stage2, c.bass, z[4]));
            const float pick = finalStage - lowpass (finalStage, c.mid, z[5]);
            y = 0.74f * finalStage + 0.18f * pick;
            break;
        }
        default:
            y = saturate (x);
            break;
    }
    // Audio-safe DC removal, followed by the voicing-specific treble cap.
    const float filtered = y - s.dcInput + c.dc * s.dcOutput;
    s.dcInput = y;
    s.dcOutput = filtered;
    return lowpass (filtered, c.tone, z[11]);
}
}
