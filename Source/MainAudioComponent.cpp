
#include <iostream>
#include "MainAudioComponent.hpp"

// To investigate...
//#include "kiss_fft.h"

using namespace std;

void AudioChangeListener::changeListenerCallback(ChangeBroadcaster *source)
{
    std::cout << "AudioChangeListener::changeListenerCallback on called !" << std::endl;
    if (source == getMACSingleton()->m_adm)
    {
        std::cout << "changeListenerCallback on m_adm called !" << std::endl;
    }
}

/*
void AudioFormatReaderSourceHook::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
    std::cout << "AudioFormatReaderSourceHook::getNextAudioBlock called !" << std::endl;
    AudioFormatReaderSource::getNextAudioBlock(bufferToFill);
}

void AudioFormatReaderSourceHook::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    std::cout << "AudioFormatReaderSourceHook::samplesPerBlockExpected = " << samplesPerBlockExpected << ", sampleRate = " << sampleRate << std::endl;
    AudioFormatReaderSource::prepareToPlay(samplesPerBlockExpected, sampleRate);
}
*/

void AudioTransportSourceHook::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
    std::cout << "AudioTransportSourceHook::getNextAudioBlock called !" << std::endl;
    //AudioTransportSource::getNextAudioBlock(bufferToFill); // Useless

    getMACSingleton()->m_afrs->getNextAudioBlock(bufferToFill); // Chain to the real sound handler, get the sound data !

    // Send the results back to the main thread... We will fix the eventual locking issue later :)
    if (getMACSingleton()->m_mtc)
    {
        jassert(bufferToFill.buffer->getNumSamples() == 512);
        int numSamples = bufferToFill.buffer->getNumSamples(); // Should be 512 here !

        // Process FFT on the buffer here !
        juce::uint8 left[8], right[8];
        juce::FFT mathStuff(8, false);

        for (int channel = 0; channel < 2; ++channel)
        {
            float* channelData = bufferToFill.buffer->getWritePointer(channel);
            float* freqArray = new float[numSamples * 2];
            memcpy(freqArray, channelData, numSamples * sizeof(float)); // fills the first half of the array with the signal

            mathStuff.performRealOnlyForwardTransform(freqArray); // In place !

            //mathStuff.performRealOnlyInverseTransform(freqArray);
            //bufferToFill.buffer->copyFrom(channel, 0, freqArray, numSamples);

            if (channel == 0)
            {
                // Left
            }
            else
            {
                // Right
            }

            delete[] freqArray;
        }

        (*(getMACSingleton()->m_mtc))(left, right);
    }
}

void AudioTransportSourceHook::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    std::cout << "AudioTransportSourceHook::samplesPerBlockExpected = " << samplesPerBlockExpected << ", sampleRate = " << sampleRate << std::endl;
    AudioTransportSource::prepareToPlay(samplesPerBlockExpected, sampleRate);
}

MainAudioComponent* mac = nullptr;
void initMACSingleton(const std::string& fileInput, MainThreadCallback* callback)
{
    // MainThreadCallback mtc; This compile but what is the meaning ?

    mac = new MainAudioComponent(fileInput);
    mac->m_mtc = callback;
}

MainAudioComponent* getMACSingleton()
{
    return mac;
}

//MainAudioComponent::MainAudioComponent() : m_state(Stopped)
MainAudioComponent::MainAudioComponent(const std::string& fileInput)
{
    cout << "MainAudioComponent : Entering ctor fileInput = " << fileInput << endl;

	File file(File::getCurrentWorkingDirectory().getChildFile(juce::String(fileInput)));
	cout << "file.getFullPathName() = " << file.getFullPathName() << endl;

	cout << "step 0" << endl;
	m_acl = new AudioChangeListener();
	if (!m_acl)
	{
        cout << "Oups : m_acl is null" << endl;
        return;
	}

	cout << "step 1" << endl;
	m_afm = new AudioFormatManager();
	if (!m_afm)
	{
        cout << "Oups : m_afm is null" << endl;
        return;
	}
    m_afm->registerBasicFormats(); // Care : the returned list include mod under Windows but not under Linux !!!
    cout << "m_afm->getWildcardForAllFormats() = " << m_afm->getWildcardForAllFormats() << endl;

    cout << "step 2" << endl;
	m_afrs = new AudioFormatReaderSource(m_afm->createReaderFor(file), true);
	if (!m_afrs)
	{
        cout << "Oups : m_afrs is null" << endl;
        return;
	}

	cout << "step 3" << endl;
	m_adm = new AudioDeviceManager();
	if (!m_adm)
	{
        cout << "Oups : m_adm is null" << endl;
        return;
	}
    m_adm->initialiseWithDefaultDevices(0, 2); // Here we don't choose any specific output device

    cout << "step 4" << endl;
    m_aiod = m_adm->getCurrentAudioDevice(); // Needed ! See the stop()
    if (!m_aiod)
    {
        cout << "Oups : m_aiod is null" << endl;
        return;
    }

    cout << "step 5" << endl;
    m_asp = new AudioSourcePlayer();
    if (!m_asp)
    {
        std::cout << "Oups : m_asp is null" << endl; // Very weird case :)
        return;
    }

    cout << "step 6" << endl;
    m_atsh = new AudioTransportSourceHook();
    if (!m_atsh)
    {
        std::cout << "Oups : m_atph is null" << endl;
        return;
    }

    //cout << "step 7" << endl;
    //m_asp->setSource(m_afrs);
    cout << "step 7 bis" << endl;
    m_asp->setSource(m_atsh); // Replace the m_afrs !
    cout << "step 8" << endl;
    m_adm->addChangeListener(m_acl); // Does not work :(
    cout << "step 9" << endl;
    m_adm->addAudioCallback(m_asp); // Start audio ! On speaker for now... We have to tweak the output...
    cout << "step A" << endl;
    //m_adm.addAudioCallback(?);

    //TODO : needed ?
    //m_transportSource.addChangeListener();

    // Code behind setAudioChannels(0, 2) in AudioAppComponent
    //String audioError = m_deviceManager.initialise(0, 2, nullptr, true);
    //jassert(audioError.isEmpty());
    //m_deviceManager.addAudioCallback(&m_audioSourcePlayer);

    //TODO : needed ?
    //m_audioSourcePlayer.setSource(this);
}

MainAudioComponent::~MainAudioComponent()
{
    cout << "Entering dtor" << endl;

    if (m_atsh)
    {
        cout << "About to delete m_atsh" << endl;
        delete m_atsh;
    }

    if (m_asp)
    {
        cout << "About to delete m_asp" << endl;
        delete m_asp; // BOUM !?!
    }

    // Needed ?
    //if (m_aiod)
    //{
    //    delete m_aiod;
    //}

    if (m_afrs)
    {
        cout << "About to delete m_afrs" << endl;
        delete m_afrs;
    }

    cout << "About to call m_adm->closeAudioDevice()" << endl;
    m_adm->closeAudioDevice();
    if (m_adm)
    {
        cout << "About to delete m_adm" << endl;
        delete m_adm;
    }

	if (m_afrs)
	{
        cout << "About to delete m_afrs" << endl;
        // delete m_afrs; // BOUM !!!
	}

	if (m_afm)
	{
        cout << "About to delete m_afm" << endl;
        delete m_afm;
	}

	if (m_acl)
	{
        cout << "About to delete m_acl" << endl;
        delete m_acl;
	}

    //TODO : Do we need to something about m_readerSource ? Not used to play with scoped ptr...

    //m_transportSource.releaseResources();

    // Code behind shutdownAudio() in AudioAppComponent
    //m_audioSourcePlayer.setSource(nullptr);
    //m_deviceManager.removeAudioCallback(&m_audioSourcePlayer);
    //m_deviceManager.closeAudioDevice();
}

/*
void MainAudioComponent::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
    if (m_readerSource == nullptr)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    m_transportSource.getNextAudioBlock(bufferToFill);
}
*/

void MainAudioComponent::changeListenerCallback(ChangeBroadcaster* source)
{
/*
    if (source == &m_transportSource)
    {
        if (m_transportSource.isPlaying())
        {
            changeState(Playing);
        }
        else
        {
            changeState(Stopped); // Or Stopping ?
        }
    }
*/
}

/*
void MainAudioComponent::changeState(TransportState newState)
{
    if (m_state != newState)
    {
        m_state = newState;

        switch (m_state)
        {
            case Starting:
                std::cout << "state = Starting" << std::endl;
                m_transportSource.start();
                break;

            case Playing:
                std::cout << "state = Playing" << std::endl;
                break;

            case Stopping:
                std::cout << "state = Stopping" << std::endl;
                m_transportSource.stop();
                break;

            case Stopped:
                std::cout << "state = Stopped" << std::endl;
                m_transportSource.setPosition(0.0);
                break;
        }
    }
}
*/

// Useless npw
void MainAudioComponent::play()//const std::string& what)
{
	/*
    File file(File::getCurrentWorkingDirectory().getChildFile(juce::String(what)));
    std::cout << "file.getFullPathName() = " << file.getFullPathName() << std::endl;
	*/

	//m_afrs->getNextAudioBlock()

	while (m_afrs->getNextReadPosition() < m_afrs->getTotalLength())
	{
		Thread::sleep(100);
	}
}

/*
void MainAudioComponent::play(const std::string& what)
{
    File file(File::getCurrentWorkingDirectory().getChildFile(juce::String(what)));
    std::cout << "file.getFullPathName() = " << file.getFullPathName() << std::endl;
    AudioFormatReader* reader = m_formatManager.createReaderFor(file);

    if (reader != nullptr)
    {
        ScopedPointer<AudioFormatReaderSource> newSource = new AudioFormatReaderSource(reader, true);
        m_transportSource.setSource(newSource, 0, nullptr, reader->sampleRate);
        m_readerSource = newSource.release();
    }

    changeState(Starting);
    m_transportSource.prepareToPlay(512, 44100); // Check the validity of the first parameter !
}
*/

void MainAudioComponent::stop()
{
    //changeState(Stopping);
	//m_afrs->setNextReadPosition(m_afrs->getTotalLength());
	// m_aiod->stop();

    cout << "About to call m_aiod->stop()" << endl;
    m_aiod->stop();


    // By doing this we will call changeListenerCallback !!!
    //cout << "About to call m_adm->sendSynchronousChangeMessage()" << endl;
	//m_adm->sendSynchronousChangeMessage(); // Needed ?
}
