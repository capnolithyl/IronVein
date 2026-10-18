# IRONVEIN | Modular FX Rack

**IRONVEIN v0.1** is a metal-focused audio effect insert for mono/stereo mixer tracks and buses, written in C++ and JUCE 8. It builds as a **Windows x64 VST3** and a **standalone desktop application**. No build tools are needed on your computer when using the GitHub Actions output.

> Experimental alpha. The numerical waveshaper tests are included; do not treat a successful compile as a substitute for DAW validation or listening tests.

## Easiest installation (Windows)

1. Open this repository's **Actions** tab, then click the latest successful **Build Windows VST3** run (on main or an approved PR).
2. Under **Artifacts**, download **IRONVEIN-Windows-x64** and extract it.
3. Copy the **entire** `VST3/IRONVEIN.vst3` directory into `C:\Program Files\Common Files\VST3\`.
4. Restart or rescan VST3 plug-ins in your DAW, then insert **IRONVEIN** on a track or bus.

You can also run `Standalone/IRONVEIN.exe` to test without a DAW. GitHub artifacts expire after 30 days; stable builds can be published separately as Releases later.

## Features

- Reusable module card with bypass and editable effect controls; first installed module is **Distortion**.
- Four modes: **Warm** (saturation), **Hard** (clipping), **Razor** (asymmetric texture) and **Obliterate** (extreme saturation).
- Drive (+0–48 dB), Tight pre-high-pass (35–350 Hz), Tone post-low-pass (1.8–16 kHz), Mix (0–100%) and Output (-30 to +12 dB).
- Four-times oversampled processing, DC rejection, delayed dry path, smoothed adjustments and reported latency.
- Automatable DAW parameters and session state; mono/stereo audio.

The GUI currently has *one* functional card. Adding, deleting, and reordering multiple modules is future work, not a v0.1 feature. The algorithms here are waveshapers, **not** guitar amp or cabinet emulation. For DI guitar, use an amp/cab sim or IR loader after distortion. Exercise caution on the master bus because there is no limiter yet.

## Build from source

Requirements: Visual Studio 2022 with Desktop development with C++, Windows SDK, CMake, Git.

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release --target Ironvein_VST3 Ironvein_Standalone --parallel 2
```

Or run `build-windows.bat`. JUCE is fetched from the fixed `8.0.16-security` tag at configure time. VST3 build output is in `build/Ironvein_artefacts/Release/VST3`. The repository also runs `ctest` on mathematical distortion tests in CI.

## Distortion starting points (experimental, not vetted industry presets)

| Name | Mode | Drive | Tight | Tone | Mix | Output |
|---|---|---:|---:|---:|---:|---:|
| A Little Bite | Warm | 4 dB | 45 Hz | 14 kHz | 25% | 0 dB |
| Old-School Riff Crunch | Hard | 15 dB | 85 Hz | 9 kHz | 85% | -8 dB |
| Tight Modern Rhythm | Razor | 25 dB | 130 Hz | 6.5 kHz | 100% | -12 dB |
| Full Goblin Mode | Obliterate | 38 dB | 180 Hz | 4.5 kHz | 100% | -16 dB |

These are examples only, **not yet loadable presets**.

## Roadmap

1. Plug-in validation and host compatibility tests, loudness/peak metering, UI refinement.
2. Dynamic module rack with add/remove/reorder, saved chain layout and independent bypass.
3. EQ, compressor, delay, reverb, cab impulse response support.
4. Documented, testable preset library organized by Guitar, Bass, Drums, Mix, Master and Experimental.

## Licensing

JUCE is downloaded separately and governed by its own license. Choose an appropriate JUCE license before distribution or commercialization; third-party DSP and impulse response assets have their own terms. Project source currently has no separately granted redistributable license.
