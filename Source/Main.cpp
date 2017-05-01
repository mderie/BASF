/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include <iostream>
#include <string>

#include "../JuceLibraryCode/JuceHeader.h"
#include "LaunchpadPrimitives.hpp"
#include "MainAudioComponent.hpp"

juce::MidiInput* midiInput = NULL; // LP !
juce::MidiOutput* midiLP = NULL;

int matrix[8][8];
LP* lp = NULL;
bool done = false;

using namespace std;

// TIMER stuff ----------------------------------------------------------------

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

// Midi callback stuff --------------------------------------------------------

class LPCallback : public juce::MidiInputCallback
{
public:
	void handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message) override;
	//static bool KeyPressed() { return s_keyPressed; }
	//static void ResetKeyPressed() { s_keyPressed = false; }
private:
	//static bool s_keyPressed;
};

void LPCallback::handleIncomingMidiMessage(juce::MidiInput* source, const juce::MidiMessage& message)
{
	std::cout << "Incomming message :" << std::endl;
	const juce::uint8* p = message.getRawData();
	for (int i = 0; i < message.getRawDataSize(); i++)
	{
		std::cout << std::hex << static_cast<int>(*(p + i)) << std::endl;
	}

	// Test if it is something else than a key release code
	if (static_cast<int>(*(p + 2)) != 0)
	{
		return;
		// s_keyPressed = true; // Check this !?!
	}
	int key = (static_cast<int>(*(p + 1)));
	done = true; // "any" key pressed :)
}

// On Screen (console) stuff --------------------------------------------------

void displayMatrixOnConsole()
{
// Nothing in the modern C++ to clear the console (except ncurse maybe)
#ifdef WINDOWS
    system("cls");
#else
	system("clear");
#endif
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

void clearMatrix()
{
	for (int j = 0; j < 8; j++)
	{
		for (int i = 0; i < 8; i++)
		{
			matrix[j][i] = 0;
		}
	}
}

void displayMatrixOnLaunchpad()
{
	// Clear it

	// Push the matrix content

	return;
}

void POC()
{
	clearMatrix();
	for (int i = 0; i < 10; i++)
	{
		displayMatrixOnConsole();
		matrix[1][1] = 1;
		matrix[6][6] = 1;
		displayMatrixOnConsole();
		matrix[1][1] = 0;
		matrix[6][6] = 0;
	}
}

// Code taken almost as is from JUCE play some file tutorial ------------------
MainAudioComponent *mac = NULL;

/*
void stop()
{
    changeState(Stopping);
}

void play(const string& what)
{
    mac = new MainContentComponent();

    File file(what);
    AudioFormatReader* reader = formatManager.createReaderFor (file);

    if (reader != nullptr)
    {
        ScopedPointer<AudioFormatReaderSource> newSource = new AudioFormatReaderSource (reader, true);
        transportSource.setSource (newSource, 0, nullptr, reader->sampleRate);
        readerSource = newSource.release();
    }

	return;
}
*/

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

  int i, lpOutIndex, lpInIndex;
	LPCallback lpcb;
	string dummy;

	// Now get the LP for output

  std::cout << "Names of all the midi output device (lp)" << std::endl;
	juce::StringArray outputDevices = juce::MidiOutput::getDevices();
	i = 0;
	lpOutIndex = -1;
	lpInIndex = -1;

	// Microsoft specific ! https://msdn.microsoft.com/en-us/library/ms177202.aspx
	// for each (juce::String item in outputDevices)
    for (auto s : outputDevices)
	{
		if (s.contains("Launchpad"))
		{
			lpOutIndex = i;
		}

		std::cout << "index " << i++ << " : " << s << std::endl;
	}

	if (lpOutIndex == -1)
	{
		lpOutIndex = 1; // Fine tune this default value !
	}

	std::cout << "About to use " << lpOutIndex << " as index... Press ENTER or CTRL-C to stop !" << std::endl;
	std::getline(std::cin, dummy);

	midiLP = juce::MidiOutput::openDevice(lpOutIndex);
	lp = new LP(midiLP);
	lp->ClearScreen();

    // Get again the LP but for input this time

	std::cout << "Names of all the midi input device (lp again)" << std::endl;
	juce::StringArray inputDevices = juce::MidiInput::getDevices();
	i = 0;
	for (auto s : inputDevices)
	{
		if (s.contains("Launchpad"))
		{
			lpInIndex = i;
		}

		std::cout << "index " << i++ << " : " << s << std::endl;
	}

	if (lpInIndex == -1)
	{
		lpInIndex = 1; // Fine tune this default value !
	}

	std::cout << "About to use" << lpInIndex << " as index... Press ENTER or CTRL-C to stop !" << std::endl;
	std::getline(std::cin, dummy);

	midiInput = juce::MidiInput::openDevice(lpInIndex, &lpcb);
	midiInput->start();

	mac->play(argv[1]);

	// main loop :)
    while (!done)
	{
	}

	// POC();

	// Stop all !
	mac->stop();
	delete mac;

	midiInput->stop();
    delete midiInput;

	lp->ClearScreen();
	delete lp;

	return 0;
}
