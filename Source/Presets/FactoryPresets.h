#pragma once
#include <array>

namespace ironvein
{
// A preset is a complete, repeatable starting signal chain, not just a label
// for a clipping curve. Tune these using DI recordings and references later.
struct FactoryPreset
{
    const char* name;
    const char* description;
    const char* recommendedUse;
    int engine; // 0 black, 1 chainsaw, 2 old-school, 3 death, 4 modern.
    float driveDb;
    float tightHz;
    float toneHz;
    float outputDb;
    int cabinet; // 0 open, 1 closed, 2 straight, 3 slant
    float cabinetDb;
};

inline constexpr std::array<FactoryPreset, 9> factoryPresets {{
    {"Norwegian Black Metal",
     "Raw, icy upper mids and a deliberately abrasive edge.",
     "Fast tremolo riffs / bridge pickup / stacked rhythm tracks",
     0, 14.0f, 205.0f, 10700.0f, -13.0f, 3, -5.0f},
    {"Atmospheric Black Metal",
     "Open, grainy and less crowded, leaving room for layers.",
     "Melodic washes / wide layers / bridge pickup",
     0, 8.0f, 92.0f, 9000.0f, -9.0f, 0, -6.0f},
    {"Swedish Death Metal",
     "Buzzsaw midrange with a brutally forward pedal grind.",
     "Stockholm-style rhythm / tremolo and blunt riffs",
     1, 12.0f, 95.0f, 8200.0f, -14.0f, 2, -7.0f},
    {"Old-School Death Metal",
     "Thick, gritty and less polished than contemporary metal.",
     "Crunchy riffs / chunky palm mutes / power chords",
     3, 10.0f, 92.0f, 7400.0f, -13.0f, 2, -7.0f},
    {"Old-School Thrash",
     "Dry attack, snappy low end and sharp riff articulation.",
     "Fast downpicking / gallops / bridge humbuckers",
     2, 13.0f, 125.0f, 9300.0f, -9.0f, 2, -5.0f},
    {"Classic Heavy Metal",
     "Mid-forward amp crunch with room for pick dynamics.",
     "Traditional heavy metal / power metal rhythm / leads",
     2, 7.0f, 68.0f, 9000.0f, -6.0f, 1, -4.0f},
    {"Modern Death Metal",
     "Dense multi-stage preamp with a firm, controlled attack.",
     "Tight rhythm guitars / high-output bridge pickups",
     3, 16.0f, 165.0f, 7500.0f, -14.0f, 2, -7.0f},
    {"Modern Metalcore",
     "Focused low-end and an aggressive, compressed pick edge.",
     "Chugs / percussive rhythms / dropped tunings",
     4, 15.0f, 180.0f, 7600.0f, -13.0f, 3, -6.0f},
    {"Slow Crushing Doom",
     "Loose bottom end and slow, thick saturation.",
     "Slow riffs / lower tunings / sustained chords",
     2, 19.0f, 47.0f, 4600.0f, -15.0f, 1, -6.0f}
}};
}
