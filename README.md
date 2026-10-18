# IRONVEIN | Modular Metal FX Rack

**v0.2 alpha** | JUCE 8 | VST3 + Windows standalone

A metal-oriented guitar distortion processor with five original digital
voicings and a separate convolution cabinet IR module. Designed to grow into
an insertable suite for guitar tracks and, with cab simulation bypassed, buses.

## Install (Windows)

1. Open the latest **successful** `Build Windows VST3` workflow in **Actions**.
2. Download its **IRONVEIN-Windows-x64** artifact and extract it.
3. For a DAW, copy the entire `VST3/IRONVEIN.vst3` directory to
   `C:\Program Files\Common Files\VST3\` and rescan plug-ins.
4. Or run `Standalone/IRONVEIN.exe` for direct guitar playing.

### Fixing crackling and live guitar latency (Focusrite)

The standalone **v0.2 build enables native ASIO** (`JUCE_ASIO=1`). In the
standalone's **Options > Audio/MIDI Settings** menu, select **ASIO** as device
type, then **Focusrite USB ASIO** (requires the Focusrite manufacturer's
Windows ASIO driver). Try 48 kHz and 128 or 256 samples, then increase to
512 if you hear dropouts. Hardware monitoring, monitoring via DAW, and
Windows shared audio may sound doubled or delayed; disable direct-monitor
when listening to the effect only. Start with low volume and drive ~10 dB.

In the v0.1 screenshots the available Windows Audio setting at 44.1 kHz,
441 samples shows a **10 ms nominal audio buffer**, not total round-trip
latency. If ASIO isn't available yet, try Windows Audio (Low Latency Mode).
These changes cannot correct all Focusrite/USB/driver configuration issues.

## Effects

### Module 1: Distortion

- **Raw Black Metal**: sharp, brittle distortion with pre-emphasised treble.
- **Swedish Chainsaw**: deliberately exaggerated high-midrange grind;
  *HM-2 inspired*, not circuit-perfect modelling.
- **Old-School Heavy**: softer, dynamic clipping and pronounced low mids.
- **Modern Death Metal**: cascaded saturation, punchy upper mids and bass cut.
- **Modern Metalcore**: intense, tight filtering and sharply defined attack.

All five use genuinely different nonlinear transfer functions and pre/post EQ
profiles. **Drive is 0–48 dB**: 48 dB will saturate almost anything and make
all modes sound similar. Start at **6–18 dB**, then compare at matched output.
Controls: Drive, Tight, Tone, Mix, Output. Distortion remains 4x oversampled.

### Module 2: Cabinet IR

- Four *real audio data* bundled CC0 cabinet impulse responses: 1x12 Open,
  2x12 Closed, 4x12 Straight, 4x12 Slant.
- Browse and load your own **mono or stereo WAV IR**, max 32 MB (processed
  as mono IR by default); selection and custom file path are saved in the session.
- Convolution internally resamples IRs to the host's current sample rate.
- Bypass individually for non-guitar tracks; **cabinet simulation is not a
  general-purpose mastering effect.**

See [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md) for cabinet sources and
license details. Cab and distortion are individual rack DSP modules and UI cards.

### Important caveats

This is an **experimental alpha** and neither a physically accurate guitar
amp/circuit simulation nor a substitute for a dedicated tube amp model. No
noise gate, limiter, input/output meters, full dynamic rack editor or loadable
factory presets yet. These are planned improvements. Distortion character
changes are not crossfaded; avoid switching modes during critical takes.
A saved custom IR path can become invalid if the WAV is moved.

## Build from source

Install Visual Studio 2022 C++ Desktop tools, CMake, and Git, then run
`build-windows.bat`. The build fetches pinned JUCE 8.0.16-security; Windows
builds enable JUCE ASIO support. See third-party notices for SDK license
considerations.

## Next

Noise gate, signal meters and oversampling options; dynamic add/remove/reorder
rack; EQ, compressor, delay, reverb; documented preset library.
