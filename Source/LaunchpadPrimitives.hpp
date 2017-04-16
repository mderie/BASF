
#ifndef LAUNCHPAD_PRIMITIVES
#define LAUNCHPAD_PRIMITIVES

#include "../JuceLibraryCode/JuceHeader.h" // Must be in first position
// Not needed :-)
//#include "../JuceLibraryCode/modules/juce_audio_basics/juce_audio_basics.h"
//#include "../JuceLibraryCode/modules/juce_audio_devices/juce_audio_devices.h"

class LP
{
private:
	mutable juce::MidiOutput* m_out; // Actually we just need a three bytes sender...
	bool m_visual;
	bool m_yx;
	bool m_db; // Use Double Buffering :)
	int m_offsetBuffer; // We could use a boolean but we may need more than...
	mutable int m_buffers[8 * 8 * 2]; // ...Two buffers !-)

public:
	LP(juce::MidiOutput* out) : m_out(out), m_yx(false), m_db(false), m_offsetBuffer(0) { Init(); } // Review this !

	static const int CL_NONE = 0x00; //TODO: Rename all color code with CL prefix !

	static const int RED_LOW = 0x01;
	static const int RED_MED = 0x02;
	static const int RED_HIGH = 0x03;

	static const int GREEN_LOW = (0x10 * 0x01);
	static const int GREEN_MED = (0x10 * 0x02);
	static const int GREEN_HIGH = (0x10 * 0x03);

	// There are two leds behind each pad !
	static const int AMBER_LOW = GREEN_LOW + RED_LOW;
	static const int AMBER_HIGH = GREEN_HIGH + RED_HIGH;
	static const int YELLOW = GREEN_HIGH + RED_MED; // Strange...
	static const int MIXED = GREEN_MED + RED_MED;

	// All modes are mutually exclusive... Write one SetMode() method or three separate ones ?
	static const int NORMAL_MODE = 0x0C;
	static const int FLASH_MODE = 0x08;
	static const int DBUFFER_MODE = 0x00;

	static const int BTN_1 = 0x68;
	static const int BTN_2 = 0x69;
	static const int BTN_3 = 0x6A;
	static const int BTN_4 = 0x6B;
	static const int BTN_5 = 0x6C;
	static const int BTN_6 = 0x6D;
	static const int BTN_7 = 0x6E;
	static const int BTN_8 = 0x6F;

	static const int BTN_A = 0x08;
	static const int BTN_B = 0x18;
	static const int BTN_C = 0x28;
	static const int BTN_D = 0x38;
	static const int BTN_E = 0x48;
	static const int BTN_F = 0x58;
	static const int BTN_G = 0x68;
	static const int BTN_H = 0x78;

	void Init(); // bool bufferMode ? Call clear screen ?

	//TODO: Make all methods static ? Then quid of the ctor ? Move the ctor code in Init !
	void ClearScreen() const;

	// X (horizontal) & Y(vertical) ranges are from 0 to 7
	void SetPixel(const int x, const int y, const int color) const;
};

#endif // LAUNCHPAD_PRIMITIVES
