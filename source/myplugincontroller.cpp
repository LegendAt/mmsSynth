//------------------------------------------------------------------------
// Copyright(c) 2023 JKU.
//------------------------------------------------------------------------

#include "myplugincontroller.h"
#include "myplugincids.h"
#include "vstgui/plugin-bindings/vst3editor.h"

#include "pluginterfaces/base/ibstream.h"
#include "base/source/fstreamer.h"

using namespace Steinberg;

namespace MyCompanyName {

//------------------------------------------------------------------------
// MMS_SynthController Implementation
//------------------------------------------------------------------------
tresult PLUGIN_API MMS_SynthController::initialize (FUnknown* context)
{
	// Here the Plug-in will be instantiated

	//---do not forget to call parent ------
	tresult result = EditControllerEx1::initialize (context);
	if (result != kResultOk)
	{
		return result;
	}

	// Here you could register some parameters

	setKnobMode(Vst::kLinearMode);
	parameters.addParameter(STR16("OSC_Type"), nullptr, 3, default_WaveType, Vst::ParameterInfo::kCanAutomate, kWaveType);
	parameters.addParameter(STR16("OSC_Level"), nullptr, 0, default_WaveLevel, Vst::ParameterInfo::kCanAutomate, kWaveLevel);
	parameters.addParameter(STR16("LevelAttack"), nullptr, 0, default_LevelAttack , Vst::ParameterInfo::kCanAutomate, kLevelAttack);
	parameters.addParameter(STR16("LevelDecay"), nullptr, 0, default_LevelDecay, Vst::ParameterInfo::kCanAutomate, kLevelDecay);
	parameters.addParameter(STR16("LevelSustain"), nullptr, 0, default_LevelSustain, Vst::ParameterInfo::kCanAutomate, kLevelSustain);
	parameters.addParameter(STR16("LevelRelease"), nullptr, 0, default_LevelRelease, Vst::ParameterInfo::kCanAutomate, kLevelRelease);
	//parameters.addParameter(STR16("LevelActive"), nullptr, 0, default_WaveType, Vst::ParameterInfo::kCanAutomate, kLevelActive);
	parameters.addParameter(STR16("LFO_Freq"), nullptr, 0, default_LfoFreq, Vst::ParameterInfo::kCanAutomate, kLfoFreq);
	parameters.addParameter(STR16("LFO_Level"), nullptr, 0, default_LfoLevel, Vst::ParameterInfo::kCanAutomate, kLfoLevel);
	parameters.addParameter(STR16("LFO_Active"), nullptr, 0, default_LfoActive, Vst::ParameterInfo::kCanAutomate, kLfoActive);

	return result;
}

//------------------------------------------------------------------------
tresult PLUGIN_API MMS_SynthController::terminate ()
{
	// Here the Plug-in will be de-instantiated, last possibility to remove some memory!

	//---do not forget to call parent ------
	return EditControllerEx1::terminate ();
}

//------------------------------------------------------------------------
tresult PLUGIN_API MMS_SynthController::setComponentState (IBStream* state)
{
	// Here you get the state of the component (Processor part)
	if (!state)
		return kResultFalse;
	IBStreamer streamer(state, kLittleEndian);

	float fval;
	int16 ival;
	bool bval;

	if (streamer.readInt16(ival) == false) {
		return kResultFalse;
	}
	setParamNormalized(kWaveType, ival);

	if (streamer.readFloat(fval) == false) {
		return kResultFalse;
	}
	setParamNormalized(kWaveLevel, fval);

	if (streamer.readFloat(fval) == false) {
		return kResultFalse;
	}
	setParamNormalized(kLevelAttack, fval);

	if (streamer.readFloat(fval) == false) {
		return kResultFalse;
	}
	setParamNormalized(kLevelDecay, fval);

	if (streamer.readFloat(fval) == false) {
		return kResultFalse;
	}
	setParamNormalized(kLevelSustain, fval);

	if (streamer.readFloat(fval) == false) {
		return kResultFalse;
	}
	setParamNormalized(kLevelRelease, fval);

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
	setParamNormalized(kLfoFreq, fval);

	if (streamer.readFloat(fval) == false) {
		return kResultFalse;
	}
	setParamNormalized(kLfoLevel, fval);

	if (streamer.readBool(bval) == false) {
		return kResultFalse;
	}
	setParamNormalized(kLfoActive, bval);


	return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API MMS_SynthController::setState (IBStream* state)
{
	// Here you get the state of the controller

	return kResultTrue;
}

//------------------------------------------------------------------------
tresult PLUGIN_API MMS_SynthController::getState (IBStream* state)
{
	// Here you are asked to deliver the state of the controller (if needed)
	// Note: the real state of your plug-in is saved in the processor

	return kResultTrue;
}

//------------------------------------------------------------------------
IPlugView* PLUGIN_API MMS_SynthController::createView (FIDString name)
{
	// Here the Host wants to open your editor (if you have one)
	if (FIDStringsEqual (name, Vst::ViewType::kEditor))
	{
		// create your editor here and return a IPlugView ptr of it
		auto* view = new VSTGUI::VST3Editor (this, "view", "myplugineditor.uidesc");
		return view;
	}
	return nullptr;
}

//------------------------------------------------------------------------
tresult PLUGIN_API MMS_SynthController::setParamNormalized (Vst::ParamID tag, Vst::ParamValue value)
{
	// called by host to update your parameters
	tresult result = EditControllerEx1::setParamNormalized (tag, value);
	return result;
}

//------------------------------------------------------------------------
tresult PLUGIN_API MMS_SynthController::getParamStringByValue (Vst::ParamID tag, Vst::ParamValue valueNormalized, Vst::String128 string)
{
	// called by host to get a string for given normalized value of a specific parameter
	// (without having to set the value!)
	return EditControllerEx1::getParamStringByValue (tag, valueNormalized, string);
}

//------------------------------------------------------------------------
tresult PLUGIN_API MMS_SynthController::getParamValueByString (Vst::ParamID tag, Vst::TChar* string, Vst::ParamValue& valueNormalized)
{
	// called by host to get a normalized value from a string representation of a specific parameter
	// (without having to set the value!)
	return EditControllerEx1::getParamValueByString (tag, string, valueNormalized);
}

//------------------------------------------------------------------------
} // namespace MyCompanyName
