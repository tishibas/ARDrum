#include <stdio.h>
#include <windows.h>
#include <sys/types.h>
#include <time.h>
#include <WinDef.h>
#include <MIDIIO.h>
#include "Drum.h"
#include "tone.h"

Drum::Drum(MIDIOut * pMIDIOut){
	this->pMIDIOut = pMIDIOut;
}

Drum::Drum(void){
	wchar_t szDeviceName[32];
	long lRet;

    if ((lRet = MIDIOut_GetDeviceName (0, szDeviceName, 32)) == 0) {
	 	printf ("利用できるMIDI出力デバイスはありません。\n");
		throw 1;//適当
	}

	if ((this->pMIDIOut = MIDIOut_Open (szDeviceName)) == NULL) {
	 	printf ("MIDI出力デバイス「%s」を開けません。\n", szDeviceName);
		throw 1;//適当
	}
}

Drum::~Drum(void){
	MIDIOut_Close (this->pMIDIOut);
}

void Drum::send(char status, char data1, char data2){
		unsigned char byMessage[3] = {status, data1, data2};
		MIDIOut_PutMIDIMessage (this->pMIDIOut, byMessage, 3);
}

void Drum::tataku(tone t){
	this->send(0x99, (char)t, 100);
}

void Drum::tataku(tone t, int velocity){
	this->send(0x99, (char)t, velocity);
}



