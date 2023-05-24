//------------------------------------------------------------------------
// Copyright(c) 2023 JKU.
//------------------------------------------------------------------------

#pragma once

#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/vst/vsttypes.h"

namespace MyCompanyName {
//------------------------------------------------------------------------
static const Steinberg::FUID kMMS_SynthProcessorUID (0xF9A6E051, 0xD4625A9D, 0x96958C38, 0xFFAC4D31);
static const Steinberg::FUID kMMS_SynthControllerUID (0x01F3DF32, 0x2AF05889, 0xB33809CF, 0x63A2A19C);

#define MMS_SynthVST3Category "Instrument"

//------------------------------------------------------------------------
} // namespace MyCompanyName
