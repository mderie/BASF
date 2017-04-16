
#ifndef MAIN_AUDIO_COMPONENT
#define MAIN_AUDIO_COMPONENT

#include "../JuceLibraryCode/JuceHeader.h"

class MainAudioComponent : public AudioAppComponent, public ChangeListener
{
public:
    MainAudioComponent();
    ~MainAudioComponent();

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    void changeListenerCallback(ChangeBroadcaster* source) override;
    void play(const std::string& what);
    void stop();

private:
    enum TransportState
    {
        Stopped,
        Starting,
        Playing,
        Stopping
    };

    void changeState (TransportState newState);

    AudioFormatManager m_formatManager;
    ScopedPointer<AudioFormatReaderSource> m_readerSource;
    AudioTransportSource m_transportSource;
    TransportState m_state;
};

#endif // MAIN_AUDIO_COMPONENT
