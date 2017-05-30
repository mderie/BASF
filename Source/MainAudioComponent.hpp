
#ifndef MAIN_AUDIO_COMPONENT
#define MAIN_AUDIO_COMPONENT

#include <string>

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

class AudioChangeListener : public ChangeListener
{
public:
    void changeListenerCallback(ChangeBroadcaster *source) override;
};

/*
class AudioFormatReaderSourceHook : public AudioFormatReaderSource
{
public:
    virtual void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    virtual void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override; // The override keyword is optional (C++11)
};
*/

class AudioTransportSourceHook : public AudioTransportSource
{
public:
    virtual void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    virtual void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override; // The override keyword is optional (C++11)
};

/*
class AudioDeviceManagerHook : public AudioDeviceManager
{
public:
};
*/

/*
class AudioSourcePlayerHook : public AudioSourcePlayer
{
public:
};
*/

typedef void (MainThreadCallback)(const juce::uint8[], const juce::uint8[]);

class MainAudioComponent // : public AudioSource // An not AudioAppComponent !... Actually we should not inherit from anything :)
{
public:
	MainAudioComponent(const std::string& fileInput);
    ~MainAudioComponent();

    float m_lTime = 0.f;

    AudioChangeListener* m_acl = nullptr;
    MainThreadCallback* m_mtc = nullptr;

    // TODO: Convert all ptr to instance ? Or the opposit... Move also all this back to private !!!
    // See https://www.juce.com/doc/tutorial_playing_sound_files
    //AudioSourcePlayer m_audioSourcePlayer;
    AudioFormatManager* m_afm = nullptr;
    AudioIODevice* m_aiod = nullptr;
    AudioFormatReaderSource* m_afrs = nullptr; // Inherit from AudioSource !
    //AudioFormatReaderSourceHook* m_afrsh = nullptr;
    AudioTransportSourceHook* m_atsh = nullptr; // Inherit from AudioSource and ChangeBroadcaster !
    AudioDeviceManager* m_adm = nullptr;
    AudioSourcePlayer* m_asp = nullptr; // Wrapper class to continuously stream audio from an audio source to an AudioIODevice

    /* Should be all this private ? Needed ? Need a timer again ? What about the callback / listener ?
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    */

    void changeListenerCallback(ChangeBroadcaster* source);
    void play();// const std::string& what);
    void stop();

private:
    int m_counter;
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
};

void initMACSingleton(const std::string& fileInput, MainThreadCallback* callback);
MainAudioComponent* getMACSingleton();

#endif // MAIN_AUDIO_COMPONENT
