//------------------------------------------------------------------------
// Copyright(c) 2023 JKU.
//------------------------------------------------------------------------

#include "mypluginprocessor.h"
#include "myplugincids.h"

#include "base/source/fstreamer.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"

#include "public.sdk/source/vst/hosting/eventlist.h"

using namespace Steinberg;

namespace MyCompanyName {
//------------------------------------------------------------------------
// MMS_SynthProcessor
//------------------------------------------------------------------------
MMS_SynthProcessor::MMS_SynthProcessor ()
{
	//--- set the wanted controller for our processor
	setControllerClass (kMMS_SynthControllerUID);
}

//------------------------------------------------------------------------
MMS_SynthProcessor::~MMS_SynthProcessor ()
{}

//------------------------------------------------------------------------
tresult PLUGIN_API MMS_SynthProcessor::initialize (FUnknown* context)
{
	// Here the Plug-in will be instantiated
	
	//---always initialize the parent-------
	tresult result = AudioEffect::initialize (context);
	// if everything Ok, continue
	if (result != kResultOk)
	{
		return result;
	}

	//--- create Audio IO ------
	addAudioInput (STR16 ("Stereo In"), Steinberg::Vst::SpeakerArr::kStereo);
	addAudioOutput (STR16 ("Stereo Out"), Steinberg::Vst::SpeakerArr::kStereo);

	/* If you don't need an event bus, you can remove the next line */
	addEventInput (STR16 ("Event In"), 1);

	

	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API MMS_SynthProcessor::terminate ()
{
	// Here the Plug-in will be de-instantiated, last possibility to remove some memory!
	
	//---do not forget to call parent ------
	return AudioEffect::terminate ();
}

//------------------------------------------------------------------------
tresult PLUGIN_API MMS_SynthProcessor::setActive (TBool state)
{
	//--- called when the Plug-in is enable/disable (On/Off) -----
	return AudioEffect::setActive (state);
}

//------------------------------------------------------------------------
tresult PLUGIN_API MMS_SynthProcessor::process (Vst::ProcessData& data)
{
	//--- First : Read inputs parameter changes-----------

	if (data.inputParameterChanges)
	{
		int32 numParamsChanged = data.inputParameterChanges->getParameterCount();
		for (int32 index = 0; index < numParamsChanged; index++)
		{
			if (auto* paramQueue = data.inputParameterChanges->getParameterData(index))
			{
				Vst::ParamValue value;
				int32 sampleOffset;
				int32 numPoints = paramQueue->getPointCount();
				paramQueue->getPoint(numPoints - 1, sampleOffset, value);
				switch (paramQueue->getParameterId())
				{
				case kWaveLevel:
					fWaveLevel = (float)value;
					break;
				case kWaveType:
					iWaveType = float (value);
					break;

				case kLevelAttack:
					fLevelAttack = (float)value*500;//0 to 1 times 500ms
					break;
				case kLevelDecay:
					fLevelDecay = (float)value * 500;//0 to 1 times 500ms
					break;
				case kLevelSustain:
					fLevelSustain = (float)value;
					break;
				case kLevelRelease:
					fLevelRelease = (float)value * 500;//0 to 1 times 500ms
					break;
				case kLfoActive:
					bLfoActive = (bool)value;
					break;				
				case kLfoFreq:
					fLfoFreq = (float)value*20;
					fLfoAngle = PI2 * fLfoFreq / data.processContext->sampleRate;
					break;
				case kLfoLevel:
					fLfoLevel = (float)value*0.5;
					break;
				}
			}
		}
	}
	
	

	Vst::IEventList* events = data.inputEvents;
	if (events != NULL) {
		int32 numEvent = events->getEventCount();
		for (int32 i = 0; i < numEvent; i++) {
			Vst::Event event;
			if (events->getEvent(i, event) == kResultOk) {
				switch (event.type) {
				case Vst::Event::kNoteOnEvent:
					// Waveform
					fFrequency = 440.0f * pow(2.0f, ((float)event.noteOn.pitch - 69) / 12.0f);
					fDeltaAngle = PI2 * fFrequency / data.processContext->sampleRate;
					fLevelAttackDelta = 1 / ((fLevelAttack / 1000) * data.processContext->sampleRate);
					fLevelDecayDelta = (1 - fLevelSustain) / ((fLevelDecay / 1000) * data.processContext->sampleRate);
					fLevelReleaseDelta = fLevelSustain / ((fLevelRelease / 1000) * data.processContext->sampleRate);
					fVolume = 0.6f;
					fWavePhase = 0.f;
					// LFO
					fLfoAngle = PI2*fLfoFreq /  data.processContext->sampleRate;
					//fLfoPhase = 0.f;
					// Envelopes 
					iSampleCounter = 0;
					noteOn = true;
					
					break;
				case Vst::Event::kNoteOffEvent:
					noteOn = false;
					iSampleCounter = 0;
					break;
				}
			}
		}
	}


	//--- Here you have to implement your processing
	Vst::Sample32* outL = data.outputs[0].channelBuffers32[0];
	Vst::Sample32* outR = data.outputs[0].channelBuffers32[1];
	Vst::Sample32* waveSamples = new Vst::Sample32[data.numSamples];
	Vst::Sample32* lfoSamples = new Vst::Sample32[data.numSamples];
	Vst::Sample32* envSamples = new Vst::Sample32[data.numSamples];

	for (int32 i = 0; i < data.numSamples; i++) {

		waveSamples[i] = fWaveLevel * generate(fWavePhase);
		lfoSamples[i] =0.5+ ( fLfoLevel *sin(fLfoPhase));
		envSamples[i] = env(iSampleCounter);
		
		
		//Update Phase 
		fWavePhase += fDeltaAngle;
		fLfoPhase += fLfoAngle;
		iSampleCounter++;
	}

	for (int32 i = 0; i < data.numSamples; i++) {


		outL[i] = fVolume *  (waveSamples[i] * envSamples[i]);
		if (!bLfoActive) {
			outL[i] *= lfoSamples[i];
		}
		outR[i] = outL[i];
	}


	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API MMS_SynthProcessor::setupProcessing (Vst::ProcessSetup& newSetup)
{
	//--- called before any processing ----
	return AudioEffect::setupProcessing (newSetup);
}

//------------------------------------------------------------------------
tresult PLUGIN_API MMS_SynthProcessor::canProcessSampleSize (int32 symbolicSampleSize)
{
	// by default kSample32 is supported
	if (symbolicSampleSize == Vst::kSample32)
		return kResultTrue;

	// disable the following comment if your processing support kSample64
	/* if (symbolicSampleSize == Vst::kSample64)
		return kResultTrue; */

	return kResultFalse;
}

//------------------------------------------------------------------------
tresult PLUGIN_API MMS_SynthProcessor::setState (IBStream* state)
{
	// called when we load a preset, the model has to be reloaded
	IBStreamer streamer (state, kLittleEndian);

	float fval;
	//int16 ival;
	bool bval;

	if (streamer.readFloat(fval) == false) {
		return kResultFalse;
	}
	iWaveType = fval;

	if (streamer.readFloat(fval) == false) {
		return kResultFalse;
	}
	fWaveLevel = fval;

	if (streamer.readFloat(fval) == false) {
		return kResultFalse;
	}
	fLevelAttack = fval;
	if (streamer.readFloat(fval) == false) {
		return kResultFalse;
	}
	fLevelDecay = fval;

	if (streamer.readFloat(fval) == false) {
		return kResultFalse;
	}
	fLevelSustain = fval;

	if (streamer.readFloat(fval) == false) {
		return kResultFalse;
	}
	fLevelRelease = fval;

	/*
	if (streamer.readFloat(fval) == false) {
		return kResultFalse;
	}
	fFilterAttack = fval;

	if (streamer.readFloat(fval) == false) {
		return kResultFalse;
	}
	fFilterDecay = fval;

	if (streamer.readFloat(fval) == false) {
		return kResultFalse;
	}
	fFilterSustain = fval;

	if (streamer.readFloat(fval) == false) {
		return kResultFalse;
	}
	fFilterRelease = fval;

	if (streamer.readInt16(ival) == false) {
		return kResultFalse;
	}
	iFilterType = ival;

	*/
	if (streamer.readFloat(fval) == false) {
		return kResultFalse;
	}
	fLfoFreq = fval;

	if (streamer.readFloat(fval) == false) {
		return kResultFalse;
	}
	fLfoLevel = fval;

	if (streamer.readBool(bval) == false) {
		return kResultFalse;
	}
	bLfoActive = bval;

	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API MMS_SynthProcessor::getState (IBStream* state)
{
	// here we need to save the model
	IBStreamer streamer (state, kLittleEndian);
	streamer.writeFloat(iWaveType);
	streamer.writeFloat(fWaveLevel);
	streamer.writeFloat(fLevelAttack);
	streamer.writeFloat(fLevelDecay);
	streamer.writeFloat(fLevelSustain);
	streamer.writeFloat(fLevelRelease);
	/*
	streamer.writeFloat(fWaveLevel);
	streamer.writeFloat(fWaveLevel);
	streamer.writeFloat(fWaveLevel);
	streamer.writeFloat(fWaveLevel);
	streamer.writeInt16(iWaveType);
	streamer.writeFloat(fWaveLevel);
	streamer.writeFloat(fWaveLevel);
	*/
	streamer.writeFloat(fLfoFreq);
	streamer.writeFloat(fLfoLevel);
	streamer.writeBool(bLfoActive);

	return kResultOk;
}

//------------------------------------------------------------------------
// Envelope Generator
float MMS_SynthProcessor::env(int sample)
{
	float retVal = 0;
	if (noteOn) {
		if (sample < 1/fLevelAttackDelta) {
			retVal = sample * fLevelAttackDelta;
			if (retVal <= 1) {
				return retVal;
			}
			else {
				return 1;
			}
		}
		else if (sample < ((1 / fLevelAttackDelta)) + ((1 - fLevelSustain) / (fLevelDecayDelta))) {
			return 1 - ((sample - (1 / fLevelAttackDelta)) * fLevelDecayDelta);
		}
		else {
			return fLevelSustain;
		}
	}
	else {
		if (sample < fLevelSustain / fLevelReleaseDelta) {
			return fLevelSustain - fLevelReleaseDelta * sample;
		}
		else {
			return 0;
		}
	}

	
}
// Waveform Generation Functions 
float MMS_SynthProcessor::generate(float phase)
{
	
	//for testing before GUI
	int16 type = iWaveType;



	if (phase >= PI2) {
		phase = phase - trunc(phase / PI2) * PI2;
	}

	if(iWaveType <0.15){
		return sin(phase);
	}
	if (iWaveType < 0.50) {
		return rect(phase);
	}
	if (iWaveType < 0.80) {
		return tri(phase);
	}
	return saw(phase);
}

float MMS_SynthProcessor::rect(float phase) {

	if (phase <= PI2 / 2) {
		return 1;
	}
	else {
		return 0;
	}
}

float MMS_SynthProcessor::tri(float phase)
{
	if (phase < PI2 / 2) {
		return (1 - (phase * 1 / PI2));
	}
	else {
		return (-1 + (phase * 1 / PI2));
	}
}

float MMS_SynthProcessor::saw(float phase)
{

	return 1 - (phase * 2 / PI2);
}

} // namespace MyCompanyName



