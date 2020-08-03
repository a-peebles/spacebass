#ifndef __SPACEBASS__
#define __SPACEBASS__

#include "IPlug_include_in_plug_hdr.h"
#include "MIDIReceiver.h"
#include "VoiceManager.h"



class SpaceBass : public IPlug
{
public:
	SpaceBass(IPlugInstanceInfo instanceInfo);
	~SpaceBass();

	void Reset();
	void OnParamChange(int paramIdx);
	void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);
	void ProcessMidiMsg(IMidiMsg* pMsg);
	// Needed for GUI
	// Should return nin-zero if one or more keys played
	inline int GetNumKeys() const { return mMIDIReceiver.getNumKeys(); };
	inline bool GetKeyStatus(int key) const { return mMIDIReceiver.getKeyStatus(key); }
	static const int virtualKeyboardMinimumNoteNumber = 48;
	int lastVirtualKeyboardNoteNumber;

private:
	void CreatePresets();
	MIDIReceiver mMIDIReceiver;
	IControl* mVirtualKeyboard;
	void processVirtualKeyboard();
	void CreateParams();
	void CreateGraphics();
	VoiceManager voiceManager;
};

#endif
