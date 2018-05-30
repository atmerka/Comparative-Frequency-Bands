/*
==============================================================================

This file was auto-generated!

It contains the basic framework code for a JUCE plugin processor.

==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CompareFrequencyBandsAudioProcessor::CompareFrequencyBandsAudioProcessor()
	: forwardFFT(fftOrder), fifoIndex(0), fifoIndex2(0), currentSampleRate(0.0f),

	AudioProcessor(BusesProperties().withInput("Main", AudioChannelSet::stereo())
		.withInput("Sidechain", AudioChannelSet::stereo())
		.withOutput("Main", AudioChannelSet::stereo())

	)

{
}

CompareFrequencyBandsAudioProcessor::~CompareFrequencyBandsAudioProcessor()
{
}

//==============================================================================
const String CompareFrequencyBandsAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool CompareFrequencyBandsAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
	return true;
#else
	return false;
#endif
}

bool CompareFrequencyBandsAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
	return true;
#else
	return false;
#endif
}

bool CompareFrequencyBandsAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
	return true;
#else
	return false;
#endif
}

double CompareFrequencyBandsAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int CompareFrequencyBandsAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
				// so this should be at least 1, even if you're not really implementing programs.
}

int CompareFrequencyBandsAudioProcessor::getCurrentProgram()
{
	return 0;
}

void CompareFrequencyBandsAudioProcessor::setCurrentProgram(int index)
{
}

const String CompareFrequencyBandsAudioProcessor::getProgramName(int index)
{
	return {};
}

void CompareFrequencyBandsAudioProcessor::changeProgramName(int index, const String& newName)
{
}

//==============================================================================
void CompareFrequencyBandsAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
	// Use this method as the place to do any pre-playback
	// initialisation that you need..
}

void CompareFrequencyBandsAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CompareFrequencyBandsAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
	ignoreUnused(layouts);
	return true;
#else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
		&& layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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

void CompareFrequencyBandsAudioProcessor::processBlock(AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
	//define two audio buffers
	AudioSampleBuffer mainInputOutput = getBusBuffer(buffer, true, 0);

	AudioSampleBuffer sideChain = getBusBuffer(buffer, true, 1);

	ScopedNoDenormals noDenormals;

	auto mainInputChannels = mainInputOutput.getNumChannels();
	auto mainOutputChannels = mainInputOutput.getNumChannels();

	auto sidchainInputChannels = sideChain.getNumChannels();

	//clear main
	for (auto i = mainInputChannels; i < mainOutputChannels; ++i)
		mainInputOutput.clear(i, 0, mainInputOutput.getNumSamples());

	//clear sidechain

	//sideChain.clear(1, 0, sideChain.getNumSamples());


	//get main channels data and send each sample to fft
	for (int channel = 0; channel < mainInputOutput.getNumChannels(); ++channel)
	{
		auto* channelData = mainInputOutput.getWritePointer(channel);


		for (auto sample = 0; sample < mainInputOutput.getNumSamples(); ++sample)
		{
			float const samp = channelData[sample];


			pushNextSampleIntoFifo(samp);
		}
	}

	//get sidechain input data and send each sample to fft
	for (int channel = 0; channel < sideChain.getNumChannels(); ++channel)
	{
		auto* channelData2 = sideChain.getWritePointer(channel);


		for (auto sample = 0; sample < mainInputOutput.getNumSamples(); ++sample)
		{
			float const samp = channelData2[sample];

			pushNextSampleIntoFifo2(samp);
		}
	}


}

//==============================================================================
bool CompareFrequencyBandsAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* CompareFrequencyBandsAudioProcessor::createEditor()
{
	return new CompareFrequencyBandsAudioProcessorEditor(*this);
}

//==============================================================================
void CompareFrequencyBandsAudioProcessor::getStateInformation(MemoryBlock& destData)
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.
}

void CompareFrequencyBandsAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.
}

void CompareFrequencyBandsAudioProcessor::pushNextSampleIntoFifo(float sample) noexcept
{
	//if the index has reached the end 
	if (fifoIndex == fftSize)
	{
		//if next block wasn't already ready
		if (!nextFFtBlockReady)
		{
			zeromem(fftData, sizeof(fftData));//clears fftData
			memcpy(fftData, fifo, sizeof(fifo));//copies memory from fifo to fftData
			nextFFtBlockReady = true;//ready to perform FFT
		}
		fifoIndex = 0;//resests the array counter on fifo to 0
	}
	//if next block is ready
	if (nextFFtBlockReady)
	{
		//perform fft calculations
		forwardFFT.performFrequencyOnlyForwardTransform(fftData);
		nextFFtBlockReady = false;
		readyToDraw = true;
	}

	fifo[fifoIndex++] = sample;//information from sample gets pushed into fifo at array point fifoIndex.

}


void CompareFrequencyBandsAudioProcessor::pushNextSampleIntoFifo2(float sample) noexcept
{
	//Ok, so here we are filling fifo with sample data. When fifo gets full, that data
	//gets copied to the first half of fftData. fifo then gets rewritten by next set of data.
	//Also to make the loop work fftData needs to be cleared


	//if the index has reached the end 
	if (fifoIndex2 == fftSize)
	{
		//if next block wasn't already ready
		if (!nextFFtBlockReady)
		{
			zeromem(fftData2, sizeof(fftData2));//clears fftData
			memcpy(fftData2, fifo2, sizeof(fifo2));//copies memory from fifo to fftData
			nextFFtBlockReady2 = true;//ready to perform FFT
		}
		fifoIndex2 = 0;//resests the array counter on fifo to 0
	}
	//if next block is ready
	if (nextFFtBlockReady2)
	{
		//perform fft calculations
		forwardFFT.performFrequencyOnlyForwardTransform(fftData2);
		nextFFtBlockReady2 = false;
		readyToDraw2 = true;
	}

	fifo2[fifoIndex2++] = sample;//information from sample gets pushed into fifo at array point fifoIndex.

}




int CompareFrequencyBandsAudioProcessor::getFFTSize()
{
	return fftSize;
}


int CompareFrequencyBandsAudioProcessor::getFFTSize2()
{
	return fftSize;
}



float CompareFrequencyBandsAudioProcessor::getFFT(int index)
{
	return fftData[index];
}


float CompareFrequencyBandsAudioProcessor::getFFT2(int index)
{
	return fftData2[index];
}



float CompareFrequencyBandsAudioProcessor::getMaxLevel()
{
	juce::Range<float> maxAndMin = FloatVectorOperations::findMinAndMax(fftData, fftSize);

	return maxAndMin.getEnd();
}


float CompareFrequencyBandsAudioProcessor::getMaxLevel2()
{
	juce::Range<float> maxAndMin = FloatVectorOperations::findMinAndMax(fftData2, fftSize);

	return maxAndMin.getEnd();
}


//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new CompareFrequencyBandsAudioProcessor();
}
