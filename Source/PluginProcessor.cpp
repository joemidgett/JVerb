/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
JVerbAudioProcessor::JVerbAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

JVerbAudioProcessor::~JVerbAudioProcessor()
{
}

//==============================================================================
const juce::String JVerbAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool JVerbAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool JVerbAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool JVerbAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double JVerbAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int JVerbAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int JVerbAudioProcessor::getCurrentProgram()
{
    return 0;
}

void JVerbAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String JVerbAudioProcessor::getProgramName (int index)
{
    return {};
}

void JVerbAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void JVerbAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    reverb.reset(sampleRate);
}

void JVerbAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool JVerbAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void JVerbAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }
}

//==============================================================================
bool JVerbAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* JVerbAudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
    // return new JVerbAudioProcessorEditor (*this);
}

//==============================================================================
void JVerbAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void JVerbAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout JVerbAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>("kRT",
        "Reverb Time",
        juce::NormalisableRange<float>(0.0, 1.0, 0.01, 1.0),
        0.9));

    layout.add(std::make_unique<juce::AudioParameterFloat>("lpf_g",
        "Damping",
        juce::NormalisableRange<float>(0.0, 0.5, 0.01, 1.0),
        0.3));

    layout.add(std::make_unique<juce::AudioParameterFloat>("lowShelf_fc",
        "Low Shelf Filter Cutoff",
        juce::NormalisableRange<float>(20.0, 2000.0, 1.0, 1.0),
        150.0));

    layout.add(std::make_unique<juce::AudioParameterFloat>("lowShelfBoostCut_dB",
        "Low Shelf Gain",
        juce::NormalisableRange<float>(-20.0, 20.0, 0.01, 1.0),
        -20.0));

    layout.add(std::make_unique<juce::AudioParameterFloat>("highShelf_fc",
        "High Shelf Filter Cutoff",
        juce::NormalisableRange<float>(1000.0, 5000.0, 1.0, 1.0),
        4000.0));

    layout.add(std::make_unique<juce::AudioParameterFloat>("highShelfBoostCut_dB",
        "High Shelf Gain",
        juce::NormalisableRange<float>(-20.0, 20.0, 0.01, 1.0),
        -6.0));

    layout.add(std::make_unique<juce::AudioParameterFloat>("preDelay_mSec",
        "Pre Delay",
        juce::NormalisableRange<float>(0, 100.0, 0.01, 1.0),
        25.0));

    layout.add(std::make_unique<juce::AudioParameterFloat>("wetLevel_dB",
        "Wet Level",
        juce::NormalisableRange<float>(-60.0, 12.0, 0.01, 1.0),
        -12.0));

    layout.add(std::make_unique<juce::AudioParameterFloat>("dryLevel_db",
        "Dry Level",
        juce::NormalisableRange<float>(-60.0, 12.0, 0.01, 1.0),
        0.0));

    layout.add(std::make_unique<juce::AudioParameterFloat>("apfDelayMax_mSec",
        "Max Allpass Filter Delay",
        juce::NormalisableRange<float>(0.0, 100.0, 0.01, 1.0),
        33.0));

    layout.add(std::make_unique<juce::AudioParameterFloat>("apfDelayWeight_Pct",
        "Allpass Filter Delay Weight",
        juce::NormalisableRange<float>(1.0, 100.0, 0.01, 1.0),
        85.0));

    layout.add(std::make_unique<juce::AudioParameterFloat>("fixedDelayMax_mSec",
        "Max Fixed Delay",
        juce::NormalisableRange<float>(0.0, 100.0, 0.01, 1.0),
        81.0));

    layout.add(std::make_unique<juce::AudioParameterFloat>("fixedDelayWeight_Pct",
        "Allpass Filter Fixed Delay Weight",
        juce::NormalisableRange<float>(1.0, 100.0, 0.01, 1.0),
        100.0));

    layout.add(std::make_unique<juce::AudioParameterChoice>("density",
        "Density",
        juce::StringArray("Thick", "Thin"),
        0));

    return layout;
}

//==============================================================================
void JVerbAudioProcessor::updateParameters()
{
    ReverbTankParameters params = reverb.getParameters();

    params.kRT = *apvts.getRawParameterValue("kRT");
    params.lpf_g = *apvts.getRawParameterValue("lpf_g");

    params.lowShelf_fc = *apvts.getRawParameterValue("lowShelf_fc");
    params.lowShelfBoostCut_dB = *apvts.getRawParameterValue("lowShelfBoostCut_dB");

    params.highShelf_fc = *apvts.getRawParameterValue("highShelf_fc");
    params.highShelfBoostCut_dB = *apvts.getRawParameterValue("highShelfBoostCut_dB");

    params.preDelayTime_mSec = *apvts.getRawParameterValue("preDelayTime_mSec");
    params.wetLevel_dB = *apvts.getRawParameterValue("wetLevel_dB");
    params.dryLevel_dB = *apvts.getRawParameterValue("dryLevel_dB");

    // Tweakers
    params.apfDelayMax_mSec = *apvts.getRawParameterValue("apfDelayMax_mSec");
    params.apfDelayWeight_Pct = *apvts.getRawParameterValue("apfDelayWeight_Pct");
    params.fixeDelayMax_mSec = *apvts.getRawParameterValue("fixeDelayMax_mSec");
    params.fixeDelayWeight_Pct = *apvts.getRawParameterValue("fixeDelayWeight_Pct");
    params.density = convertIntToEnum((int)*apvts.getRawParameterValue("density"), reverbDensity);

    reverb.setParameters(params);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new JVerbAudioProcessor();
}
