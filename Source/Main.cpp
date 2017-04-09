/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
//#include "LaunchpadPrimitives.hpp"

#include <iostream>

int matrix[8][8];

// See c:\laboratory\tutorial_playing_sound_files\Source\MainComponent.cpp
AudioFormatManager formatManager;
ScopedPointer<AudioFormatReaderSource> readerSource;
AudioTransportSource transportSource;
//TransportState state;

// juce::MidiOutput* midiLP = NULL;

using namespace std;

class BPMTimer : public juce::HighResolutionTimer
{
public:
	void hiResTimerCallback() override;
};

void BPMTimer::hiResTimerCallback()
{
	return;
}

BPMTimer* bpmTimer = NULL;

void Play(const string& what)
{

	return;
}

void DisplayMatrixOnConsole()
{
	system("cls"); // Nothing in the modern C++ to clear tge console (except ncxurse maybe)

	for (int j = 0; j < 8; j++)
	{
		for (int i = 0; i < 8; i++)
		{
			cout << matrix[j][i];
		}
		cout << endl;
	}
	return;
}

void ClearMatrix()
{
	for (int j = 0; j < 8; j++)
	{
		for (int i = 0; i < 8; i++)
		{
			matrix[j][i] = 0;
		}
	}
}

void DisplayMatrixOnLaunchpad()
{
	// Clear it

	// Push the matrix content

	return;
}

void POC()
{
	ClearMatrix();
	for (int i = 0; i < 10; i++)
	{
		DisplayMatrixOnConsole();
		matrix[1][1] = 1;
		matrix[6][6] = 1;
		DisplayMatrixOnConsole();
		matrix[1][1] = 0;
		matrix[6][6] = 0;
	}
}

//==============================================================================
int main (int argc, char* argv[])
{
	cout << "Welcome to BASF (= Bare Audio Spectrogram Fourier)" << endl;

	if (argc != 2)
	{
		cout << "Usage : " << endl;
		cout << "basf audio_filename" << endl;
		return -1;
	}

	//play(argv[1]);
	//play("z:\008_vwx\white stripes\Elephant [2003]\the white stripes - elephant - 14 - it's true that we love one another.mp3");

	POC();

	return 0;
}
