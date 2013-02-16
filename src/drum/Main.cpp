#include <stdio.h>
#include <windows.h>
#include <sys/types.h>
#include <time.h>
#include <WinDef.h>
#include <MIDIIO.h>

#include "Drum.h"


void send(MIDIOut* pMIDIOut, char status, char data1, char data2){
	unsigned char byMessage[3] = {status, data1, data2};
	MIDIOut_PutMIDIMessage (pMIDIOut, byMessage, 3);
}

enum keyEvent { NOT_PRESSED = 0, KEYDOWN = 1, PRESSING = 2, KEYUP = 3 };

void getKeyState(keyEvent * keymap){
	for(char c = 0x10; c <= 'Z'; c++){
		int pressing = GetAsyncKeyState(c);

		switch(keymap[c]){
		case NOT_PRESSED: keymap[c] = pressing ? KEYDOWN  : NOT_PRESSED; break;
		case KEYDOWN:     keymap[c] = pressing ? PRESSING : KEYUP; break;
		case PRESSING:    keymap[c] = pressing ? PRESSING : KEYUP; break;
		case KEYUP:       keymap[c] = NOT_PRESSED;break;
		default:break;
		}
	}
}

int main () {
	keyEvent keymap[255] = { (keyEvent)0 };
	Drum * drum = new Drum();
	int loop = 1;

	while(loop){
		getKeyState(keymap);
		
		for(unsigned int i = 0; i < 255; i++){
			if(keymap[i] == KEYDOWN){
				printf("%d(%c) keydown\n",i, i);
				drum->send(0x90, i, 100);
			}
			if(keymap[i] == KEYUP){
				printf("%d(%c) keyup\n",i, i);
				drum->send(0x90, i, 0);
			}
		}

		//èIóπ
		if(keymap[VK_ESCAPE] == KEYDOWN){ loop = 0;}

		Sleep( 1 );
	}


	return 1;
}
