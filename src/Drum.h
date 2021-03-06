
#ifndef _INC_DRUM    //まだ読み込まれていなければ以下の処理をする
#define _INC_DRUM    //すでに読み込まれているという目印をつける

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