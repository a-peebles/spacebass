#include "Oscillator.h"

double Oscillator::mSampleRate = 44100.0;

/**
 Sets whether the oscillator uses a sine, saw,square or triangle wave

*/
void Oscillator::setMode(OscillatorMode mode) {
	mOscillatorMode = mode;
}

/**
Sets the frequency of the oscillator and uses updateIncrement() 
to update mPhaseIncrement as it is dependent on mFrequency
*/
void Oscillator::setFrequency(double frequency) {
	mFrequency = frequency;
	updateIncrement();
}

/**
Sets the sample rate of the oscillator and uses updateIncrement()
to update mPhaseIncrement as it is dependent on mSampleRate
*/
void Oscillator::setSampleRate(double sampleRate) {
	mSampleRate = sampleRate;
	updateIncrement();
}
/**

*/
void Oscillator::updateIncrement() {
	double pitchModAsFrequency = pow(2.0, fabs(mPitchMod) * 14.0) - 1;
	if (mPitchMod < 0) {
		pitchModAsFrequency = -pitchModAsFrequency;
	}
	double calculatedFrequency = fmin(fmax(mFrequency + pitchModAsFrequency, 0), mSampleRate / 2.0);
	mPhaseIncrement = calculatedFrequency * 2 * mPI / mSampleRate;
}
/**
	
Generates different types of oscillator waves based on the selected option. 
Called everytime ProcessDoubleReplacing is called.
*/
void Oscillator::generate(double* buffer, int nFrames) {
	const double twoPI = 2 * mPI;
	switch (mOscillatorMode) {
	case OSCILLATOR_MODE_SINE:
		// Used to generate sine wave
		for (int i = 0; i < nFrames; i++) {
			buffer[i] = sin(mPhase);
			// Incrementing mPhase so it's inbetween 0 and twoPI
			mPhase += mPhaseIncrement;
			while (mPhase >= twoPI) {
				mPhase -= twoPI;
			}
		}
		break;

		/**
		mPhase goes from 0 upwards, and jumps back to 0 when it reaches twoPI.
		(mPhase / twoPI) goes from 0 upwards and jumps back to 0 when it reaches 1.
		(2.0 * mPhase / twoPI) goes from 0 up and jumps back at 2.
		When mPhase is 0, the expression 1.0 - (2.0 * mPhase / twoPI) is 1.
		While mPhase goes upwards, the expression goes downwards and jumps back to 1 when it reaches -1.
		*/
	case OSCILLATOR_MODE_SAW:
		for (int i = 0; i < nFrames; i++) {
			buffer[i] = 1.0 - (2.0 * mPhase / twoPI);
			mPhase += mPhaseIncrement;
			while (mPhase >= twoPI) {
				mPhase -= twoPI;
			}
		}
		break;
	case OSCILLATOR_MODE_SQUARE:
		for (int i = 0; i < nFrames; i++) {
			// first half of cycle has values of 1
			if (mPhase <= mPI) {
				buffer[i] = 1.0;
			}
			// second half of cycle is has values of -1
			else {
				buffer[i] = -1.0;
			}
			mPhase += mPhaseIncrement;
			while (mPhase >= twoPI) {
				mPhase -= twoPI;
			}
		}
		break;
	case OSCILLATOR_MODE_TRIANGLE:
		for (int i = 0; i < nFrames; i++) {
			// calculation for upwards saw wave
			double value = -1.0 - (2.0 * mPhase / twoPI);
			/** Take absolute values of saw subtract .5 
			to make waveform close to 0  and multiply by 2 to get between -1 and 1 */
			buffer[i] = 2.0 *(fabs(value) - 0.5);
			mPhase += mPhaseIncrement;
			while (mPhase >= twoPI) {
				mPhase -= twoPI;
			}

		}
		break;
	}
}
double Oscillator::nextSample() {
	double value = naiveWaveformForMode(mOscillatorMode);
	mPhase += mPhaseIncrement;
	while(mPhase >= twoPI){
		mPhase -= twoPI;
	}
	return value;
}
void Oscillator::setPitchMod(double amount) {
	mPitchMod = amount;
	updateIncrement();
}
/**
Used on every sample to get audio data.
*/
double Oscillator::naiveWaveformForMode(OscillatorMode mode) {
	double value;
	switch (mode) {
	case OSCILLATOR_MODE_SINE:
		// generates sine wave
		value = sin(mPhase);
		break;
	case OSCILLATOR_MODE_SAW:
		// generates saw wave using twoPI constant
		value = (2.0 * mPhase / twoPI) - 1.0;
		break;
	case OSCILLATOR_MODE_SQUARE:
		// generates square wave
		if (mPhase < mPI) {
			value = 1.0;
		}
		else {
			value = -1.0;
		}
		break;
	case OSCILLATOR_MODE_TRIANGLE:
		// generates triangle wave
		value = -1.0 + (2.0 * mPhase / twoPI);
		value = 2.0 * (fabs(value) - 0.5);
		break;
	default:
		break;
	}
	return value;
}