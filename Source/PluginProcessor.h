#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include "PhoenixDSP.h"

class SomethingForTheMixBussProcessor : public juce::AudioProcessor
{
public:
    SomethingForTheMixBussProcessor();
    ~SomethingForTheMixBussProcessor() override = default;

    const juce::String getName() const override { return "Something For The Mix Buss"; }

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    bool hasEditor() const override;
    juce::AudioProcessorEditor* createEditor() override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int) override;
    const juce::String getProgramName(int) override;
    void changeProgramName(int, const juce::String&) override;

    void getStateInformation(juce::MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

private:
    PhoenixChannel left, right;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SomethingForTheMixBussProcessor)
};
