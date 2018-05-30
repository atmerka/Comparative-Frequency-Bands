/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class CompareFrequencyBandsAudioProcessorEditor : public AudioProcessorEditor,
												public Timer, public Slider::Listener
{
public:
	CompareFrequencyBandsAudioProcessorEditor(CompareFrequencyBandsAudioProcessor&);
    ~CompareFrequencyBandsAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
	CompareFrequencyBandsAudioProcessor& processor;


	//define functions
	void timerCallback() override;
	void sliderValueChanged(Slider* slider) override;

	//images and graphics drawers
	Image meter1;
	Graphics drawer;

	Image meter2;
	Graphics drawer2;

	//and sliders and variabels
	float max;

	Slider frequency;

	double f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CompareFrequencyBandsAudioProcessorEditor)
};
