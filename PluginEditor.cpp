/*
==============================================================================

This file was auto-generated!

It contains the basic framework code for a JUCE plugin editor.

==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CompareFrequencyBandsAudioProcessorEditor::CompareFrequencyBandsAudioProcessorEditor(CompareFrequencyBandsAudioProcessor& p)
	: AudioProcessorEditor(&p), processor(p), drawer(meter1),
	drawer2(meter2)
	, meter1(Image::RGB, 400 / 3, 400 * 9 / 10, true)
	, meter2(Image::RGB, 400 / 3, 400 * 9 / 10, true)
{
	//set timercallback to start
	startTimerHz(100);

	//drawing frequency slider
	addAndMakeVisible(frequency);
	frequency.setRange(20.0, 20000.0, 1.0);
	frequency.setSliderStyle(Slider::LinearBarVertical);
	frequency.setValue(1000.0);
	frequency.setSkewFactor(0.3, false);
	frequency.addListener(this);
	frequency.setColour(0x1001200, Colour(35, 196, 204));
	frequency.setColour(0x1001300, Colour(35, 196, 204));
	frequency.setColour(0x1001310, Colour(35, 196, 204));

	//set plug in to resizable
	AudioProcessorEditor::setResizable(true, true);
	AudioProcessorEditor::setResizeLimits(100, 100, 1000, 600);
	setSize(400, 400);

}

CompareFrequencyBandsAudioProcessorEditor::~CompareFrequencyBandsAudioProcessorEditor()
{
}

//==============================================================================
void CompareFrequencyBandsAudioProcessorEditor::paint(Graphics& g)
{
	//fill background Dark Blue
	g.fillAll(Colour(0, 1, 69));

	//change colour
	g.setColour(Colours::white);

	//draw channel labels at top
	g.drawText("Channel", getWidth()*.01, getHeight()*.01, getWidth() / 3, getHeight() / 10, Justification::centred, true);
	g.drawText("Sidechain", getWidth() * 2 / 3, getHeight()*.01, getWidth() / 3, getHeight() / 10, Justification::centred, true);
	g.drawText("Frequency", getWidth() * 1 / 3, getHeight()*.01, getWidth() / 3, getHeight() / 10, Justification::centred, true);

	//separate sections with 3 lines
	g.drawLine(0.0, getHeight()*0.1, getWidth(), getHeight()*0.1);
	g.drawLine(getWidth() / 3, 0.0f, getWidth() / 3, getHeight());
	g.drawLine(getWidth() * 2 / 3, 0.0f, getWidth() * 2 / 3, getHeight());

	// what is deemed negative infinity
	float const negInf = -12.0f;

	//max level in the editor
	float e_maxLevel = 1.0f;

	//get size of FFT array from the processor
	//deems number of bins, frequency etc
	int fftSize = processor.getFFTSize();


	/*
	//============================================================================================================
	//OLD WAY TO SCALE FREQUENCY VALUES (DOESN"T NEED LOG SCALE BECAUSE FREQUENCY DIRECTLY CORROLATES TO BINS
	//============================================================================================================
	//get frequency value from slider
	//log scale
	float  logF = std::log10(f);

	//jlimit - ensure values are between 0 and 4
	float logLimF = jlimit(0.0f, 4.0f, logF);

	//get the corresponding bin number
	float scaleF = jmap(logLimF, 0.0f, 4.0f, 0.0f, 1024.0f);

	//ensure thisBin is within fftSize/2
	int	thisBin = jlimit(0, fftSize / 2, (int)scaleF);
	*/

	//scale frequency value from the slider (f) to correspond to the relevant bin
	int thisBin = (f / 4.8779296875);
	//ensure values are within fftarray
	thisBin = jlimit(0, fftSize / 2, thisBin);


	//print bin to output
	String msg;
	msg << "Bin No:" << thisBin << newLine;
	Logger::getCurrentLogger()->writeToLog(msg);



	//draw image 1
	//store meter 1 dimensions
	float meter1Width = meter1.getWidth();
	float meter1Height = meter1.getHeight();

	//draw the image and paint black
	g.drawImageAt(meter1, 0.0f, getHeight()*0.1f);
	drawer.fillAll(Colour(0, 1, 69));


	//get the corresponding amplitude from the bin
	auto data = processor.getFFT(thisBin);

	/*
	//print data to logger
	//String msg;
	msg << "Data: " << data << newLine;
	Logger::getCurrentLogger()->writeToLog(msg);
	*/

	//convert to dB
	auto dB = 20 * std::log10f(data);



	//convert that to values between 0 and "silence"
	if (dB< negInf)
	{
		dB = negInf;
	}

	/*
	//get max dB level
	if (dB>max)
	{
	auto	max = dB;
	}
	String db;
	db << "max= :" << max << newLine;
	Logger::getCurrentLogger()->writeToLog(db);

	*/

	//scale this to image height
	auto level = jmap(dB, negInf, 12.0f, meter1Height, 0.0f);

	//draw a rectangle with height equal to this gain
	//set colour
	drawer.setColour(Colour(160, 255, 131));

	//define rect
	Rectangle<int> volume(0.0f, level, meter1Width, meter1Height - level);
	//draw block rectangle
	drawer.fillRect(volume);

	//draw image 2
	//store meter1 dimensions
	float meter2Width = meter2.getWidth();
	float meter2Height = meter2.getHeight();


	//draw and paint black
	g.drawImageAt(meter2, getWidth() * 2 / 3, getHeight()*0.1f);
	drawer2.fillAll(Colour(0, 1, 69));

	//get the corresponding amplitude from the bin
	auto data2 = processor.getFFT2(thisBin);


	//convert to dB
	auto dB2 = 20 * std::log10f(data2);


	//convert that to values between 0 and -silence
	if (dB2 < negInf)
	{
		dB2 = negInf;
	}



	//scale this to image height
	auto level2 = jmap(dB2, negInf, 12.0f, meter2Height, 0.0f);

	//draw a rectangle with height equal to this gain
	//set colour
	drawer2.setColour(Colour(160, 255, 131));

	//define rect
	Rectangle<int> volume2(0.0, level2, meter2Width, meter2Height - level2);
	//draw second meter
	drawer2.fillRect(volume2);
}

void CompareFrequencyBandsAudioProcessorEditor::resized()
{
	frequency.setBounds(getWidth() / 3, getHeight()*0.1, getWidth() / 3, getHeight()*.9);

	//for some reason when the below two lines of code are implemented, the meters won't draw 
	//meter1 = meter1.rescaled(getWidth()/4, getHeight()*.7, Graphics::lowResamplingQuality);
	//meter2 = meter2.rescaled(getWidth() / 4, getHeight()*.7, Graphics::lowResamplingQuality);

}

void CompareFrequencyBandsAudioProcessorEditor::timerCallback()
{
	//repaint at a fast rate
	repaint();
}

void CompareFrequencyBandsAudioProcessorEditor::sliderValueChanged(Slider* slider)
{
	//get f value form slider
	f = frequency.getValue();
	/*
	String msg;
	msg << "F= :" << f << newLine;
	Logger::getCurrentLogger()->writeToLog(msg);
	*/
}