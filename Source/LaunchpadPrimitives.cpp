
//#include <ciso646> // MSVC is not ISO C++ compliant, once again :(
#include "LaunchpadPrimitives.hpp"

void LP::Init()
{
	m_out->sendMessageNow(juce::MidiMessage(0xB0, 0x00, 0x01)); // X-Y Layout
}

void LP::ClearScreen() const
{
	m_out->sendMessageNow(juce::MidiMessage(0xB0, 0x00, 0x00));
}

// x for col, y for row !
void LP::SetPixel(const int x, const int y, const int color) const
{
	//if ((x < 0) or (x > 7) or (y < 0) or (y > 7))
	if ((x < 0) || (x > 7) || (y < 0) || (y > 7))
	{
		return; // Easy clipping !
	}

	m_out->sendMessageNow(juce::MidiMessage(0x90, (y * 0x10) + x, color));
}
