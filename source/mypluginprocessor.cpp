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
					fWaveType = (float)value;
					break;
				case kLfoFreq:
					fLfoFreq = (float)value;
					fLfoAngle = PI2 * fLfoFreq / data.processContext->sampleRate;
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
					fVolume = 0.6f;
					fWavePhase = 0.f;
					// LFO
					fLfoPhase = 0.f;
					// Envelopes 
					fTimer = 0.0f;

					break;
				case Vst::Event::kNoteOffEvent:
					fVolume = 0.f;
					break;
				}
			}
		}
	}


	//--- Here you have to implement your processing
	Vst::Sample32* outL = data.outputs[0].channelBuffers32[0];
	Vst::Sample32* outR = data.outputs[0].channelBuffers32[1];

	for (int32 i = 0; i < data.numSamples; i++) {

		outL[i] = fWaveLevel * generate(fWavePhase);
		if (true) {
			outL[i] += fLfoLevel * sin(fLfoPhase);
		}
		outL[i] *= fVolume;
		outR[i] = outL[i];

		//Update Phase 
		fWavePhase += fDeltaAngle;
		fLfoPhase += fLfoAngle;
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
	
	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API MMS_SynthProcessor::getState (IBStream* state)
{
	// here we need to save the model
	IBStreamer streamer (state, kLittleEndian);

	return kResultOk;
}

//------------------------------------------------------------------------
} // namespace MyCompanyName

// Waveform Generation Functions 

float rect(float phase) {

	if (phase <= PI2 / 2) {
		return 1;
	}
	else {
		return 0;
	}
}

float tri(float phase)
{
	if (phase < PI2 / 2) {
		return (1 - (phase * 1 / PI2));
	}
	else {
		return (-1 + (phase * 1 / PI2));
	}
}

float saw(float phase)
{

	return 1 - (phase * 2 / PI2);
}

float generate(float phase) {

	//for testing before GUI
	int type = 0;
	if (phase >= PI2) {
		phase = phase - trunc(phase / PI2) * PI2;
	}
	switch (type)
	{
	case 0:
		return sin(phase);
		break;
	case 1:
		return rect(phase);
		break;
	case 2:
		return tri(phase);
		break;
	case 3:
		return saw(phase);
		break;
	default:
		return 0.f;
		break;
	}

}