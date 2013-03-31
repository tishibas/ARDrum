
#ifndef _INC_DRUM    //‚Ü‚¾“Ç‚İ‚Ü‚ê‚Ä‚¢‚È‚¯‚ê‚ÎˆÈ‰º‚Ìˆ—‚ğ‚·‚é
#define _INC_DRUM    //‚·‚Å‚É“Ç‚İ‚Ü‚ê‚Ä‚¢‚é‚Æ‚¢‚¤–Úˆó‚ğ‚Â‚¯‚é

#include <MIDIIO.h>
#include "tone.h"


class Drum
{
public:
	Drum(void);
	Drum(MIDIOut*);
	~Drum(void);
	void tataku(tone);
	void tataku(tone, int);
	void send(char, char, char);
private:
	MIDIOut* pMIDIOut;

};

#endif