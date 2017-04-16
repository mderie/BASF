
#include "MainAudioComponent.hpp"

MainAudioComponent::MainAudioComponent() : m_state(Stopped)
{
    m_formatManager.registerBasicFormats();
    m_transportSource.addChangeListener (this);
    setAudioChannels (0, 2);
}

MainAudioComponent::~MainAudioComponent()
{
    shutdownAudio();
}

void MainAudioComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    m_transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainAudioComponent::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
    if (m_readerSource == nullptr)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    m_transportSource.getNextAudioBlock (bufferToFill);
}

void MainAudioComponent::releaseResources()
{
    m_transportSource.releaseResources();
}

void MainAudioComponent::changeListenerCallback (ChangeBroadcaster* source)
{
    if (source == &m_transportSource)
    {
        if (m_transportSource.isPlaying())
        {
            changeState(Playing);
        }
        else
        {
            changeState(Stopped);
        }
    }
}

void MainAudioComponent::changeState(TransportState newState)
{
    if (m_state != newState)
    {
        m_state = newState;

        switch (m_state)
        {
            case Stopped:
                m_transportSource.setPosition(0.0);
                break;

            case Starting:
                m_transportSource.start();
                break;

            case Playing:
                break;

            case Stopping:
                m_transportSource.stop();
                break;
        }
    }
}

void MainAudioComponent::play(const std::string& what)
{
    File file(what);
    AudioFormatReader* reader = m_formatManager.createReaderFor (file);

    if (reader != nullptr)
    {
        ScopedPointer<AudioFormatReaderSource> newSource = new AudioFormatReaderSource(reader, true);
        m_transportSource.setSource (newSource, 0, nullptr, reader->sampleRate);
        m_readerSource = newSource.release();
    }
}

void MainAudioComponent::stop()
{
    changeState(Stopping);
}
