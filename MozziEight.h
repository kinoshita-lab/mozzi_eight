#ifndef MOZZIEIGHT_H
#define MOZZIEIGHT_H

#include <stdint.h>
#include <Arduino.h>
#include <MozziGuts.h>
#include <Oscil.h>
#include <LowPassFilter.h>
#include <tables/square_no_alias_2048_int8.h> // table for Oscils to play

#include "pitches.h"

class MozziEight
{
public:

	// mozzi configuration
	enum {
		CONTROL_RATE = 64,
	};

	// constants for this class
	enum {
		PITCH_AD_INPUT_PIN = 0,
		MAX_STEP_AD_INPUT_PIN = 1,
		FILTER_CUTOFF_AD_INPUT_PIN = 2,
		FILTER_RESONANCE_AD_INPUT_PIN = 3,
		DECAY_AD_INPUT_PIN = 4,
		
    
		SEL_0 = 2,
		SEL_1 = 3,
		SEL_2 = 4,
    
		STEP_MAX = 8,
	};

	// constants for scale("Dorian only so far...")
	static const uint8_t NumberOfNotes = 43;
	static const uint16_t NoteNumbers[NumberOfNotes] =
	{ 0, 
	  NOTE_C2, NOTE_D2,  NOTE_DS2, NOTE_F2, NOTE_G2, NOTE_A2, NOTE_AS2, 
	  NOTE_C3, NOTE_D3,  NOTE_DS3, NOTE_F3, NOTE_G3, NOTE_A3, NOTE_AS3,
	  NOTE_C4, NOTE_D4,  NOTE_DS4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_AS4,
	  NOTE_C5, NOTE_D5,  NOTE_DS5, NOTE_F5, NOTE_G5, NOTE_A5, NOTE_AS5,
	  NOTE_C6, NOTE_D6,  NOTE_DS6, NOTE_F6, NOTE_G6, NOTE_A6, NOTE_AS6,
	  NOTE_C7, NOTE_D7,  NOTE_DS7, NOTE_F7, NOTE_G7, NOTE_A7, NOTE_AS7,
	};


	MozziEight() :
		counter(0),
		maxStep(STEP_MAX),
		durationCounter(0),
		maxDuration(1024),
		updateBpm(false) {
        pinMode(SEL_0, OUTPUT);
        pinMode(SEL_1, OUTPUT);
        pinMode(SEL_2, OUTPUT);
	}

	void bpmTick() {
		counter++;
    
		if (counter >= maxStep) {
			counter = 0;
		}

		// select step knob via 74hc4051
		digitalWrite(SEL_0, counter & 0x01);
		digitalWrite(SEL_1, counter & 0x02);
		digitalWrite(SEL_2, counter & 0x04);
	}
				
	void readAdValue() {
		const uint16_t pitchAdValue
			= mozziAnalogRead(PITCH_AD_INPUT_PIN);
		const uint8_t noteIndex
			= map(pitchAdValue, 0, 1023, 0, NumberOfNotes);
		
		lastRead = static_cast< int >(NoteNumbers[noteIndex]);   

		const uint16_t steps
			= 1 + ((mozziAnalogRead(MAX_STEP_AD_INPUT_PIN))>> 7);
		setCounterMax(steps);
	}
  
	uint16_t getLastRead() const {
		return lastRead;
	}
  
	bool isUpdateBpm() const {
		return updateBpm;
	}
  
	void canselUpdateBpm() {
		updateBpm = false;
	}
  
	void setCounterMax(const uint8_t count) {
		maxStep = count;
		
		if (counter > maxStep) {
			counter = 0;
		}
	}
	void setDurationMax(const int32_t maxCount) {
		maxDuration = maxCount;
	}

	void resetDuration() {
		durationCounter = 0;
	}

	int32_t getMaxDuration() const {
		return maxDuration;
	}

	int32_t getDurationCounter() const {
		return durationCounter;
	}

	void incrementDurationCounter() {
		durationCounter++;
	}

	void updateControl() {
		const int cutoff = mozziAnalogRead(FILTER_CUTOFF_AD_INPUT_PIN) >> 2;
		const int resonance = mozziAnalogRead(FILTER_RESONANCE_AD_INPUT_PIN) >> 2;

		lpf.setResonance(resonance);
		lpf.setCutoffFreq(cutoff);
		lpf2.setResonance(resonance);
		lpf2.setCutoffFreq(cutoff);
	}
			

	int updateAudio() {
		const int oscout = osc.next();
		const int filterout = lpf2.next(lpf.next(oscout >> 1) >> 2) >> 2;
		int8_t gain = 1;
	
		incrementDurationCounter();

		if (durationCounter >= maxDuration()) {
			gain = 0;
		}
	
		return filterout * gain;
	}
	
protected:

private:

	uint8_t counter;
	uint8_t maxStep;
	uint16_t lastRead;
	bool updateBpm;
	int32_t durationCounter;
	int32_t maxDuration;
	Oscil< SQUARE_NO_ALIAS_2048_NUM_CELLS, AUDIO_RATE > osc(SQUARE_NO_ALIAS_2048_DATA);
	LowPassFilter lpf;
	LowPassFilter lpf2;
};






#endif /* MOZZIEIGHT_H */
