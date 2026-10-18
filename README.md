# IRONVEIN | Metal FX Rack

**v0.3 alpha** • VST3 + Windows standalone • JUCE 8

IRONVEIN is a **preset-first guitar rig**. Choose a musical style, play, then
make only the tweaks you need. There's no requirement to understand EQ,
cabinet microphones or tube circuits to get started.

## Install and use (Windows)

1. Open **Actions > Build Windows VST3** and choose the latest successful run.
2. Download the artifact **IRONVEIN-Windows-x64**, extract it, and:
   - Double-click \`Standalone/IRONVEIN.exe\`, **or**
   - Copy \`VST3/IRONVEIN.vst3\` to \`C:\Program Files\Common Files\VST3\`
     and rescan plug-ins in your DAW.
3. For live guitar with a Focusrite interface, select **Options > Audio/MIDI
   Settings > ASIO > Focusrite USB ASIO** in the standalone. Start with
   48 kHz / 128 or 256 sample buffers and increase if audio crackles.
4. Choose your style from the large preset dropdown. Adjust Gain, Tightness,
   and Brightness around their neutral 0 positions if needed.
5. **RESTORE PRESET** returns to the exact built-in rig. **ADVANCED** expands
   scrollable per-module controls, including **Load WAV** for custom cabinets.

### Nine starting rigs

- Norwegian Black Metal: thin, cold and upper-mid focused
- Atmospheric Black Metal: grainy with more breathing room
- Swedish Death Metal: buzzsaw pedal-inspired grind
- Old-School Death Metal: traditional saturated preamp character
- Old-School Thrash: dry, fast and sharp
- Classic Heavy Metal: open, lower-gain amp crunch
- Modern Death Metal: focused multistage high gain
- Modern Metalcore: tight overdrive-boosted preamp
- Slow Crushing Doom: darker, looser and weightier

Preset selection configures amp model, drive, bass cutoff, treble cutoff,
cabinet IR, distortion output, and cabinet output. Three quick tweaks alter
drive (+/-12 dB), tightness (+/-85 Hz), and tone without overwriting the
factory recipe. To restore a rig, press **RESTORE PRESET**. Presets and macro
settings persist in your saved DAW session. Old v0.2 sessions retain their
saved parameters and show **Custom / imported session**.

### Engines, not five variants of the same distortion

The five DSP structures are intentionally different:
1. Raw transistor-inspired clipped fuzz with treble emphasis.
2. Swedish mid-grind pedal with extreme dual-band filtering and saturation.
3. Dynamic, sag-sensitive two-stage old-school preamp.
4. Death metal boost into three clipped, filtered stages.
5. Modern metalcore mid-boosted overdrive into a tightly filtered preamp.

Processing is 4x oversampled, includes latency-matched dry/wet blending and
an individually bypassable convolution cabinet stage with four bundled CC0
impulse responses or your own WAV. Cab simulation should usually be disabled
on non-guitar buses.

**Important:** These are original genre-inspired digital processors, not
verified hardware circuit emulations. The factory sounds are *starting
recipes*, not claims of studio/reference validation. Pickups, DI level,
playing, and monitor setup will affect the result. A passing Windows compile
does not guarantee the tones are enjoyable. Real guitar listening tests and
reference matching remain essential.

### Roadmap

Next: reference-based preset tuning, audio meters and input trim, switch
crossfades and better automation, noise gate, user preset save/rename,
expanded guitar/bass/mix categories, true add/remove/reorder module rack.

### Licensing

Four cabinet WAV IRs are included as CC0 resources from the 0xFX project.
See [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md) for attribution and
JUCE/ASIO licensing notes.
