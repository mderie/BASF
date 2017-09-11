/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include <iostream>
#include <iomanip>
#include <fstream>
// This supposed to fix the M_PI "undeclared identifier" issue
// But does not work :( https://msdn.microsoft.com/en-us/library/4hwaceh6(v=vs.110).aspx
// #define _USE_MATH_DEFINES

#ifdef WIN32
const double M_PI = 3.14159265;
#endif

#include <cmath>
#include <string>
#include <stdint.h> // For the uint8
#include <iso646.h> // For or support

#include "../JuceLibraryCode/JuceHeader.h"
#include "LaunchpadPrimitives.hpp"
#include "MainAudioComponent.hpp"

#define REFRESH_RATE 100 // Ten per seconds
#define MAX_REFRESH_COUNT 100 // Ten seconds (coze it is equal to 10 * 10 :)

juce::MidiInput* midiInput = NULL; // LP !
juce::MidiOutput* midiLP = NULL;

int matrix[8][8]; // (Row, Col)
LP* lp = nullptr;
bool done = false;
bool nousb = false;

using namespace std;

//typedef unsigned char[8] spectres;

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
	if (static_cast<int>(*(p + 2)) == 0)
	{
		return;
		// s_keyPressed = true; // Check this !?!
	}
	//int key = (static_cast<int>(*(p + 1)));
	cout << "Any key pressed :)" << endl;
	// done = true;
}

// On Screen (console) stuff --------------------------------------------------

void displayMatrixOnConsole()
{
//    return;
// Nothing in the modern C++ to clear the console (except ncurse maybe)
#ifdef WIN32
    system("cls");
#else
	system("clear");
#endif
	for (int j = 0; j < 8; j++)
	{
		for (int i = 0; i < 8; i++)
		{
			cout << setw(4) << matrix[j][i] ;
		}
		cout << endl;
	}
	return;
}

void clearMatrix()
{
	for (int j = 0; j < 8; j++) // Row
	{
		for (int i = 0; i < 8; i++) // Col
		{
			matrix[j][i] = 0;
		}
	}
}

void displayMatrixOnLaunchpad()
{
	// Clear it
	lp->ClearScreen();

	// Push the matrix content... Really need to check the fast way of refreshing the entire LP
	for (int j = 0; j < 8; j++)
	{
		for (int i = 0; i < 8; i++)
		{
			lp->SetPixel(i, j, matrix[j][i]); // [j,i] does not compile, but produce a weird error message (invalid conversion from int* to int)
		}
	}
}

void UpdateMatrix(const juce::uint8 left[], const juce::uint8 right[])
{
    //cout << "Entering/leaving UpdateMatrix !" << endl;
    //return;

    for (int i = 0; i < 8; i++) // Column
    {
        //cout << "i = " << i << ", left[i] = " << int(left[i]) << ", right[i] = " << int(right[i]) << endl;

        if (left[i] > 0)
        {
            matrix[3][i] = LP::GREEN_HIGH;
            if (left[i] > 64)
            {
                matrix[2][i] = LP::GREEN_HIGH;
                if (left[i] > 128)
                {
                    matrix[1][i] = LP::YELLOW;
                    if (left[i] > 192)
                    {
                        matrix[0][i] = LP::RED_HIGH;
                    }
                    else
                    {
                        matrix[0][i] = LP::CL_NONE;
                    }
                }
                else
                {
                    matrix[1][i] = LP::CL_NONE;
                    matrix[0][i] = LP::CL_NONE;
                }
            }
            else
            {
                matrix[2][i] = LP::CL_NONE;
                matrix[1][i] = LP::CL_NONE;
                matrix[0][i] = LP::CL_NONE;
            }
        }
        else
        {
            matrix[3][i] = LP::CL_NONE;
            matrix[2][i] = LP::CL_NONE;
            matrix[1][i] = LP::CL_NONE;
            matrix[0][i] = LP::CL_NONE;
        }

        if (right[i] > 0)
        {
            matrix[7][i] = LP::GREEN_HIGH;
            if (right[i] > 64)
            {
                matrix[6][i] = LP::GREEN_HIGH;
                if (right[i] > 128)
                {
                    matrix[5][i] = LP::YELLOW;
                    if (right[i] > 192)
                    {
                        matrix[4][i] = LP::RED_HIGH;
                    }
                    else
                    {
                        matrix[4][i] = LP::CL_NONE;
                    }
                }
                else
                {
                    matrix[5][i] = LP::CL_NONE;
                    matrix[4][i] = LP::CL_NONE;
                }
            }
            else
            {
                matrix[6][i] = LP::CL_NONE;
                matrix[5][i] = LP::CL_NONE;
                matrix[4][i] = LP::CL_NONE;
            }
        }
        else
        {
            matrix[7][i] = LP::CL_NONE;
            matrix[6][i] = LP::CL_NONE;
            matrix[5][i] = LP::CL_NONE;
            matrix[4][i] = LP::CL_NONE;
        }
    }
}

void POCAndExit()
{
	clearMatrix();
	/*
	for (int i = 0; i < 10; i++)
	{
		matrix[1][1] = 1;
		matrix[6][6] = 1;
		displayMatrixOnConsole();
		matrix[1][1] = 0;
		matrix[6][6] = 0;
		displayMatrixOnConsole();
    }
    */

    juce::uint8 left[8], right[8];
    left[0] = 1;
    left[1] = 1;
    left[2] = 40;
    left[3] = 40;
    left[4] = 80;
    left[5] = 80;
    left[6] = 120;
    left[7] = 120;
    right[0] = 0;
    right[1] = 0;
    right[2] = 100;
    right[3] = 100;
    right[4] = 0;
    right[5] = 0;
    right[6] = 200;
    right[7] = 200;
    UpdateMatrix(left, right);
    displayMatrixOnConsole();
    exit(0);
}

// Code taken almost as is from JUCE play some file tutorial ------------------
//MainAudioComponent *mac = NULL;

// For the POC : just wait a few seconds, fake the user input ------------------

class BPMTimer : public juce::HighResolutionTimer
{
private:
    int m_count = 1;
public:
	void hiResTimerCallback() override;
	int maxCount = MAX_REFRESH_COUNT;
};

void BPMTimer::hiResTimerCallback()
{
    if (m_count++ > maxCount)
    {
        done = true;
        this->stopTimer();
    }

    if (nousb)
    {
        displayMatrixOnConsole();
    }
    else
    {
        displayMatrixOnLaunchpad();
    }
};

BPMTimer* bpmTimer = NULL;

void SpectrumPOCAndExit()
{
	//juce::FFT mathStuff(3, false); // 2^3 = 8 bars !
	juce::dsp::FFT mathStuff(3);
	float sine[512 * 2];
	ofstream outf;
	outf.open("out.dat");
	float x; // JUCE seems to works with float instead of double... For perf ?

	outf << "sin(x)" << endl;
	for (int i = 0; i < 512; i++)
	{
		x = static_cast<float>((i - 256) / (2 * M_PI) + 1); // Stay positive !-)
        sine[i] = std::sin(x);
        outf << "[i = " << i << "] x = " << x << " ==> sin(x) = " << sine[i] << endl;
	}
	for (int i = 0; i < 512; i++)
	{
        sine[i + 512] = 0;
	}
	mathStuff.performFrequencyOnlyForwardTransform(sine);
	outf << "fft(x)" << endl;
	for (int i = 0; i < mathStuff.getSize() ; i++) // getSize returns 2^(ctor parameter)
	{
        outf << sine[i] << " and in byte (range 0-7 ?) " << int(sine[i]) << endl;
	}

	outf << "sin(x)/2" << endl;
	for (int i = 0; i < 512; i++)
	{
		x = static_cast<float>((i - 256) / (2 * M_PI) + 1); // Stay positive !-)
        sine[i] = std::sin(x) / 2;
        outf << "[i = " << i << "] x = " << x << " ==> sin(x) / 2 = " << sine[i] << endl;
	}
	for (int i = 0; i < 512; i++)
	{
        sine[i + 512] = 0;
	}
	mathStuff.performFrequencyOnlyForwardTransform(sine);
	outf << "fft(x)" << endl;
	for (int i = 0; i < mathStuff.getSize(); i++)
	{
        outf << sine[i] << endl;
	}

	outf << "sin(x*2)" << endl;
	for (int i = 0; i < 512; i++)
	{
		x = static_cast<float>((i - 256) / (2 * M_PI) + 1); // Stay positive !-)
        sine[i] = std::sin(x * 2);
        outf << "[i = " << i << "] x = " << x << " ==> sin(x * 2) = " << sine[i] << endl;
	}
	for (int i = 0; i < 512; i++)
	{
        sine[i + 512] = 0;
	}
	mathStuff.performFrequencyOnlyForwardTransform(sine);
	outf << "fft(x)" << endl;
	for (int i = 0; i < mathStuff.getSize(); i++)
	{
        outf << sine[i] << endl;
	}

	/*
    outf << "mean(x)" << endl;
    x = 0.0;
    for (int i = 0; i < 256; i++)
	{
        x += sine[i];
        if ((i > 0) && (i % 32 == 0))
        {
            outf << "i = " << i << "; x = " << x / 32 << endl;
            x = 0.0f;
        }
        outf << "i = " << i << "; x = " << x / 32 << endl;
	}
	outf << "i = 256; x = " << x / 32 << endl;
	*/
	outf.close();
	exit(0);
}

// The main (dish) ------------------------------------------------------------
int main (int argc, char* argv[])
{
	cout << "Welcome to BASF (= Bare Audio Spectrogram Fourier)" << endl;

	//SpectrumPOCAndExit();
	//POCAndExit();

	if ((argc < 2) or (argc > 3))
	{
		cout << "Usage : " << endl;
		cout << "basf audio_filename [nousb]" << endl;
		return -1;
	}

	int i, lpOutIndex, lpInIndex;
	LPCallback lpcb;
	string dummy;

	if (argc == 3)
	{
        if (strcmp(argv[2], "nousb") == 0)
        {
            nousb = true;
        }
        else
        {
            cout << "Usage : " << endl;
            cout << "basf audio_filename [nousb]" << endl;
            return -1;
        }
	}

	juce::String fullFilename = File::getCurrentWorkingDirectory().getChildFile(argv[1]).getFullPathName();
	//cout << "fullFilename = " << fullFilename << endl; // Care, strange construction to get path... Into non std string :(

	if (!nousb)
	{
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

        std::cout << "About to use " << lpInIndex << " as index... Press ENTER or CTRL-C to stop !" << std::endl;
        std::getline(std::cin, dummy);

        midiInput = juce::MidiInput::openDevice(lpInIndex, &lpcb);
        midiInput->start();
	}

	cout << "About to create the timer" << endl;

	bpmTimer = new BPMTimer();
	bpmTimer->startTimer(REFRESH_RATE); // Five screen updates per second

	cout << "About to create the ugly singleton" << endl;

	//mac = new MainAudioComponent(argv[1]); // Start immediately !!!
	//mac->play(); // For now play does not return until the end of the song !!! But use thread::sleep

	initMACSingleton(argv[1], UpdateMatrix); // Beurk kind of global singleton with constructor... Start immediately !!!
	cout << "About to enter the main loop" << endl;

	// main loop :)
    while (!done)
	{
	}

	// Stop all !
	bpmTimer->stopTimer();
	delete bpmTimer;

    if (!nousb)
	{
        cout << "About to clean the usb stuff !" << endl;
        midiInput->stop();
        delete midiInput;

        lp->ClearScreen(); // We still need midiLP for this...
        // Does not exist !-)
        //midiLP->stop();
        delete midiLP;
        delete lp;
    }

    cout << "About to stop !" << endl;
	getMACSingleton()->stop();
	cout << "About to call dtor !" << endl;
	delete getMACSingleton(); // Argh !-)
	cout << "Dead here ?" << endl;

    cout << "That's all folks !" << endl;

	return 0;
}
