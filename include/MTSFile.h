/*
   Copyright (c) 2020 Christof Ruch. All rights reserved.

   Dual licensed: Distributed under Affero GPL license by default, an MIT license is available for purchase
*/

#pragma once

#include "JuceHeader.h"

#include "Patch.h"
#include "MidiTuning.h"
#include "StoredPatchNameCapability.h"

namespace midikraft {

	class MTSFile : public DataFile, public StoredPatchNameCapability {
	public:
		MTSFile(int dataTypeID_, Synth::PatchData const &data) : DataFile(dataTypeID_, data) {}

		virtual std::string name() const override;
		virtual void setName(std::string const& name) override;

		std::vector<MidiMessage> createMidiMessagesFromDataFile(MidiProgramNumber placeToStore);

	};

}


