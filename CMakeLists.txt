cmake_minimum_required(VERSION 3.22)
project(Ironvein VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(JUCE
  GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
  GIT_TAG 8.0.16-security
  GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(Ironvein
  COMPANY_NAME "Ironvein"
  BUNDLE_ID "audio.ironvein.rack"
  PLUGIN_MANUFACTURER_CODE Irvn
  PLUGIN_CODE Iv01
  PRODUCT_NAME "IRONVEIN"
  VERSION 0.1.0
  FORMATS VST3 Standalone
  VST3_CATEGORIES Fx
  IS_SYNTH FALSE
  NEEDS_MIDI_INPUT FALSE
  NEEDS_MIDI_OUTPUT FALSE
  IS_MIDI_EFFECT FALSE
  COPY_PLUGIN_AFTER_BUILD FALSE)

juce_generate_juce_header(Ironvein)

target_sources(Ironvein PRIVATE
  Source/PluginProcessor.h
  Source/PluginProcessor.cpp
  Source/PluginEditor.h
  Source/PluginEditor.cpp
  Source/DSP/EffectModule.h
  Source/DSP/EffectRack.h
  Source/DSP/Waveshaper.h
  Source/DSP/DistortionModule.h
  Source/DSP/DistortionModule.cpp
  Source/UI/ForgeLookAndFeel.h
  Source/UI/ForgeLookAndFeel.cpp
  Source/UI/ModuleCard.h
  Source/UI/ModuleCard.cpp
  Source/UI/DistortionPanel.h
  Source/UI/DistortionPanel.cpp)

target_compile_definitions(Ironvein PUBLIC
  JUCE_WEB_BROWSER=0
  JUCE_USE_CURL=0
  JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(Ironvein PRIVATE
  juce::juce_audio_utils
  juce::juce_dsp
  juce::juce_recommended_config_flags
  juce::juce_recommended_warning_flags)

include(CTest)
if(BUILD_TESTING)
  add_executable(ironvein_waveshaper_tests tests/waveshaper_tests.cpp)
  target_compile_features(ironvein_waveshaper_tests PRIVATE cxx_std_17)
  add_test(NAME waveshaper_modes COMMAND ironvein_waveshaper_tests)
endif()
