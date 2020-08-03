#include "MIDIReceiver.h"

/** 
Called on every sample while generating an audio buffer. 
As long as there are messages in the queue it processes 
and removes them from the front. 
But only for MIDI messages whose mOffset isn't 
greater than the current offset. Keeping relative timing intact.



*/
void MIDIReceiver::advance() {
	while (!mMidiQueue.Empty()){
		IMidiMsg* midiMessage = mMidiQueue.Peek();
		if (midiMessage->mOffset > mOffset) break;

		IMidiMsg::EStatusMsg status = midiMessage->StatusMsg();
		int noteNumber = midiMessage->NoteNumber();
		int velocity = midiMessage->Velocity();
		// Only on/off messages in the queue
		if (status == IMidiMsg::kNoteOn && velocity) {
			if (mKeyStatus[noteNumber] == false) {
				mKeyStatus[noteNumber] = true;
				mNumKeys += 1;
				noteOn(noteNumber, velocity);
			}
		}
		else {

			if (mKeyStatus[noteNumber] == true) {
				mKeyStatus[noteNumber] = false;
				mNumKeys -= 1;
				noteOff(noteNumber, velocity);
			}

		}
		mMidiQueue.Remove();
	}
	mOffset++;
}
/**
Called when a MIDI message is to be received. 
Select note on/off messages and adds them to the mMidiQueue.
*/
void MIDIReceiver::onMessageReceived(IMidiMsg* midiMessage) {
	IMidiMsg::EStatusMsg status = midiMessage->StatusMsg();
	//Only concerned about which notes are triggered
	if (status == IMidiMsg::kNoteOn || status == IMidiMsg::kNoteOff) {
		mMidiQueue.Add(midiMessage);
	}
}