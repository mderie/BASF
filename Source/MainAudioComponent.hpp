
#ifndef MAIN_AUDIO_COMPONENT
#define MAIN_AUDIO_COMPONENT

#include "../JuceLibraryCode/JuceHeader.h"

/*
https://forum.juce.com/t/how-to-use-getnextaudioblock-method/15580/4

The audio signal flows from its audio source to the standard audio output,
under control of  JUCE-Audio-thread. The function getNextAudioBlock()
is called internally by the JUCE-Audio-thread, in order to make available the signal to you,
if you want to apply any DSP on it. Actually, this function acts as a *window* to you,
for each part of the signal. The AudioSourceChannelInfo structure,
contains all you need for any your process (current part of audio signal, number of samples contained in the buffer...)
*/

class MainAudioComponent // : public AudioSource // An not AudioAppComponent !... Actually we should not inherit from anything :)
{
public:
    MainAudioComponent();
    ~MainAudioComponent();

    /* Should be all this private ? Needed ? Need a timer again ? What about the callback / listener ?
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    void changeListenerCallback(ChangeBroadcaster* source);
    */
    void play(const std::string& what);
    void stop();

private:
    // Just usefull to keep track of the various state...
    /*
    enum TransportState
    {
        Stopped,
        Starting,
        Playing,
        Stopping
    };
    void changeState(TransportState newState);
    TransportState m_state;
    */

    // See https://www.juce.com/doc/tutorial_playing_sound_files
    //AudioSourcePlayer m_audioSourcePlayer;
    AudioFormatManager m_afm;
    AudioIODevice* m_aiod = nullptr;
    AudioFormatReaderSource* m_afrs = nullptr; // Inherit from AudioSource !
    // AudioTransportSource m_transportSource; // Inherit from AudioSource and ChangeBroadcaster !
    AudioDeviceManager m_adm;
    AudioSourcePlayer* m_asp = nullptr; // TODO: Convert all ptr to instance ? Or the opposit...
};

#endif // MAIN_AUDIO_COMPONENT
