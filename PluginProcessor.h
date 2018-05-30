/*
==============================================================================

This file was auto-generated!

It contains the basic framework code for a JUCE plugin processor.

==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
/**
*/
class CompareFrequencyBandsAudioProcessor : public AudioProcessor
{
public:
	//==============================================================================
	CompareFrequencyBandsAudioProcessor();
	~CompareFrequencyBandsAudioProcessor();

	//==============================================================================
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
	bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

	void processBlock(AudioBuffer<float>&, MidiBuffer&) override;

	//==============================================================================
	AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override;

	//==============================================================================
	const String getName() const override;

	bool acceptsMidi() const override;
	bool producesMidi() const override;
	bool isMidiEffect() const override;
	double getTailLengthSeconds() const override;

	//==============================================================================
	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram(int index) override;
	const String getProgramName(int index) override;
	void changeProgramName(int index, const String& newName) override;

	//==============================================================================
	void getStateInformation(MemoryBlock& destData) override;
	void setStateInformation(const void* data, int sizeInBytes) override;

	void pushNextSampleIntoFifo(float sample) noexcept;
	void pushNextSampleIntoFifo2(float sample) noexcept;


	//order (complexity of fft)
	enum
	{
		fftOrder = 13,
		fftSize = 1 << fftOrder
	};


	//functions to call values from the editor
	float getFFT(int index);
	float getMaxLevel();
	int getFFTSize();


	float getFFT2(int index);
	float getMaxLevel2();
	int getFFTSize2();

	//fftdata arrays
	float fftData[2 * fftSize];
	float fftData2[2 * fftSize];

	//define fft
	dsp::FFT forwardFFT;

	//booleans for logic
	bool nextFFtBlockReady = false;
	bool nextFFtBlockReady2 = false;
	bool readyToDraw = false;
	bool readyToDraw2 = false;



private:
	//variables
	double currentSampleRate;


	//fifo array for DSP
	float fifo[fftSize];
	int fifoIndex;


	float fifo2[fftSize];
	int fifoIndex2;


	int numberOfBins = fftSize / 2;
	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompareFrequencyBandsAudioProcessor)
};
