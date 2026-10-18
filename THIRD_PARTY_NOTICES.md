# Third-party notices

## Cabinet impulse responses (CC0 / public domain)

The following four WAV cabinet IRs are redistributed unchanged from **0xFX**, by
[averagenative](https://github.com/averagenative/0xFX), which identifies its
four bundled cabinet responses as **CC0 / public domain**:

- `resources/irs/Cab_Open_1x12.wav` (original `1x12_open.wav`)
- `resources/irs/Cab_Closed_2x12.wav` (original `2x12_closed.wav`)
- `resources/irs/Cab_Straight_4x12.wav` (original `4x12_straight.wav`)
- `resources/irs/Cab_Slant_4x12.wav` (original `4x12_slant.wav`)

Original resources: https://github.com/averagenative/0xFX/tree/master/resources/ir/bundled

The original 0xFX software has an MIT license. These specific assets are
identified separately as CC0 in its public README. IRs are loaded internally
as audio data, not linked as third-party DSP code.

## JUCE and ASIO

JUCE 8 is fetched during the CMake build and used according to its
open-source (AGPLv3) or commercial licensing options.
See https://juce.com/legal/juce-8-licence/ .

On Windows the standalone application enables `JUCE_ASIO=1`. This includes
interfaces to Steinberg ASIO SDK code, which is offered under either GPLv3
or a proprietary Steinberg license, with corresponding distribution terms.
See `juce_audio_devices/native/asio/LICENSE.txt` in the fetched JUCE version.

Before distributing or commercially licensing IRONVEIN more broadly,
confirm compatibility of all licenses and choose a project source license.
The current binary is an experimental personal-use alpha.
