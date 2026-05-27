cmake_minimum_required(VERSION 3.22)
project(SomethingForTheMixBuss VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "" FORCE)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "" FORCE)

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.7
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(JUCE)

# Patch JUCE: mark the auto-generated VST3 bypass parameter as kIsHidden so
# hosts don't show it as a visible slider in their generic parameter UI.
FetchContent_GetProperties(JUCE SOURCE_DIR _juce_src)
set(_vst3_wrapper "${_juce_src}/modules/juce_audio_plugin_client/juce_audio_plugin_client_VST3.cpp")
set(_bypass_hidden_stamp "${CMAKE_BINARY_DIR}/bypass_hidden.stamp")
if(NOT EXISTS "${_bypass_hidden_stamp}")
    file(READ "${_vst3_wrapper}" _vst3_src)
    string(REPLACE
        "info.flags |= Vst::ParameterInfo::kIsBypass;"
        "info.flags |= Vst::ParameterInfo::kIsBypass | Vst::ParameterInfo::kIsHidden;"
        _vst3_src_patched "${_vst3_src}")
    file(WRITE "${_vst3_wrapper}" "${_vst3_src_patched}")
    file(TOUCH "${_bypass_hidden_stamp}")
endif()

juce_add_plugin(SomethingForTheMixBuss
    COMPANY_NAME                    "Oranzil"
    BUNDLE_ID                       "com.oranzil.somethingforthemixbuss"
    PLUGIN_MANUFACTURER_CODE        Ornz
    PLUGIN_CODE                     Sftm
    FORMATS                         AU VST3
    PRODUCT_NAME                    "Something For The Mix Buss"
    IS_SYNTH                        FALSE
    NEEDS_MIDI_INPUT                FALSE
    NEEDS_MIDI_OUTPUT               FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS     FALSE
    COPY_PLUGIN_AFTER_BUILD         TRUE
)

juce_add_binary_data(SomethingForTheMixBussBinaryData SOURCES
    Resources/cookie.png
)

target_sources(SomethingForTheMixBuss PRIVATE
    Source/PluginProcessor.cpp
)

target_include_directories(SomethingForTheMixBuss PRIVATE Source)

target_compile_definitions(SomethingForTheMixBuss PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(SomethingForTheMixBuss PRIVATE
    SomethingForTheMixBussBinaryData
    juce::juce_audio_utils
    juce::juce_audio_plugin_client
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)
