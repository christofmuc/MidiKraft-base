/*
   Copyright (c) 2020 Christof Ruch. All rights reserved.

   Dual licensed: Distributed under Affero GPL license by default, an MIT license is available for purchase
*/

#pragma once

#include "SynthParameterDefinition.h"

namespace midikraft {

	class BidirectionalSyncCapability {
		public:
		virtual bool determineParameterChangeFromSysex(std::vector<juce::MidiMessage> const& messages, std::shared_ptr<SynthParameterDefinition> *outParam, int& outValue) = 0;
	};

}
