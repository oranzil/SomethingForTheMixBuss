#include "PluginProcessor.h"

SomethingForTheMixBussProcessor::SomethingForTheMixBussProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput ("Input",  juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{
}

void SomethingForTheMixBussProcessor::prepareToPlay(double sampleRate, int /*samplesPerBlock*/)
{
    left.prepare(sampleRate);
    right.prepare(sampleRate);
}

void SomethingForTheMixBussProcessor::releaseResources()
{
    left.reset();
    right.reset();
}

bool SomethingForTheMixBussProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    const auto& mainOut = layouts.getMainOutputChannelSet();
    if (mainOut != juce::AudioChannelSet::stereo())
        return false;
    if (layouts.getMainInputChannelSet() != mainOut)
        return false;
    return true;
}

void SomethingForTheMixBussProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                                    juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    constexpr double kInputGain  = 1.0;
    constexpr double kOutputGain = 0.83562888266817;  // 10^(-1.55/20)

    const int n = buffer.getNumSamples();

    if (buffer.getNumChannels() >= 1)
    {
        auto* L = buffer.getWritePointer(0);
        for (int i = 0; i < n; ++i)
        {
            double in  = static_cast<double>(L[i]) * kInputGain;
            double out = left.process(in) * kOutputGain;
            L[i] = static_cast<float>(out);
        }
    }
    if (buffer.getNumChannels() >= 2)
    {
        auto* R = buffer.getWritePointer(1);
        for (int i = 0; i < n; ++i)
        {
            double in  = static_cast<double>(R[i]) * kInputGain;
            double out = right.process(in) * kOutputGain;
            R[i] = static_cast<float>(out);
        }
    }
}

// Minimal blank editor — exists only to suppress the host's generic parameter view.
// No controls: this plugin has a fixed sound and nothing to adjust.
struct SomethingForTheMixBussEditor : public juce::AudioProcessorEditor
{
    explicit SomethingForTheMixBussEditor (SomethingForTheMixBussProcessor& p)
        : AudioProcessorEditor (p) { setSize (300, 80); }

    void paint (juce::Graphics& g) override { g.fillAll (juce::Colour (0xff1a1a1a)); }
    void resized() override {}
};

juce::AudioProcessorEditor* SomethingForTheMixBussProcessor::createEditor()
{
    return new SomethingForTheMixBussEditor (*this);
}

bool SomethingForTheMixBussProcessor::acceptsMidi() const                  { return false; }
bool SomethingForTheMixBussProcessor::producesMidi() const                 { return false; }
double SomethingForTheMixBussProcessor::getTailLengthSeconds() const       { return 0.0; }

int SomethingForTheMixBussProcessor::getNumPrograms()                      { return 1; }
int SomethingForTheMixBussProcessor::getCurrentProgram()                   { return 0; }
void SomethingForTheMixBussProcessor::setCurrentProgram(int)               {}
const juce::String SomethingForTheMixBussProcessor::getProgramName(int)    { return {}; }
void SomethingForTheMixBussProcessor::changeProgramName(int, const juce::String&) {}

void SomethingForTheMixBussProcessor::getStateInformation(juce::MemoryBlock&) {}
void SomethingForTheMixBussProcessor::setStateInformation(const void*, int)    {}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SomethingForTheMixBussProcessor();
}
