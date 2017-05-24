
#include <iostream>
#include "MainAudioComponent.hpp"

//MainAudioComponent::MainAudioComponent() : m_state(Stopped)
MainAudioComponent::MainAudioComponent(std::string fileInput)
{
	File file(File::getCurrentWorkingDirectory().getChildFile(juce::String(fileInput)));
    m_afm.registerBasicFormats();
	m_afrs = new AudioFormatReaderSource(m_afm.createReaderFor(file), true);
    m_adm.initialiseWithDefaultDevices(0, 2);
    m_aiod = m_adm.getCurrentAudioDevice(); // Needed later ?

    if (!m_aiod)
    {
        std::cout << "Oups : m_aiod is null" << std::endl;
    }

    if (!m_afrs)
    {
        std::cout << "Oups : m_afrs is null" << std::endl;
    }

    m_asp = new AudioSourcePlayer();
    if (!m_asp)
    {
        std::cout << "Oups : m_asp is null" << std::endl; // Very weird case :)
    }
    m_asp->setSource(m_afrs);
    m_adm.addAudioCallback(m_asp); // Start audio !

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
    if (m_afrs)
    {
        delete m_afrs;
    }

    // Needed ?
    //if (m_aiod)
    //{
    //    delete m_aiod;
    //}

    if (m_asp)
    {
        delete m_asp;
    }

    m_adm.closeAudioDevice();

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

/*
void MainAudioComponent::changeListenerCallback(ChangeBroadcaster* source)
{
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
}
*/

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

// Useless now
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
	// Does not work since the play is not needed...
	m_afrs->setNextReadPosition(m_afrs->getTotalLength());
}
