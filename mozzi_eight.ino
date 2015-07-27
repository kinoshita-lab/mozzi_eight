/**
 @file
 @author kazuki saita(Breadboard Band)
*/

// define it for free-run mode

#define FREERUN_MODE 

#include <stdint.h>
#include "MozziEight.h"

#ifdef FREERUN_MODE
#include <MsTimer2.h>

void msTimer2Callback();
#endif

MozziEight mozziEight;

void msTimer2Callback()
{
	mozziEight.tick();
	mozziEight.readAdValue();
	osc.setFreq(static_cast< int > (mozziEight.getLastRead()));
	const int decayTime = mozziAnalogRead(StepPitchSelector::DECAY_AD_INPUT_PIN) << 1;
	mozziEight.setDurationMax(decayTime);
	mozziEight.resetDuration();
}

void setup()
{
	startMozzi(CONTROL_RATE);
	
	lpf.setResonance(200);
	lpf2.setResonance(200);
  
  
	osc.setFreq(440);
	
	MsTimer2::set(120, msTimer2Callback);
	MsTimer2::start();
}



void loop()
{
	audioHook();
}

void updateControl() 
{

}

int updateAudio()
{
return mozziEight.updateAudio();
}
