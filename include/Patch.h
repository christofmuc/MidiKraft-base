/*
   Copyright (c) 2019 Christof Ruch. All rights reserved.

   Dual licensed: Distributed under Affero GPL license by default, an MIT license is available for purchase
*/

#pragma once

#include "Synth.h"
#include "Tag.h"

namespace midikraft {

	class SynthParameterDefinition;

	class DataFile {
	public:
		DataFile(int dataTypeID);
		DataFile(int dataTypeID, Synth::PatchData const &patchdata);

		int dataTypeID() const;

		virtual std::string name() const = 0;

		// Direct byte access functions
		virtual void setData(Synth::PatchData const &data);
		virtual void setDataFromSysex(MidiMessage const &message);
		virtual Synth::PatchData const &data() const;
		virtual int at(int sysExIndex) const;
		virtual void setAt(int sysExIndex, uint8 value);

	protected:
		// Just any ID you want to give it
		int dataTypeID_;

		// Direct byte storage
		Synth::PatchData data_;
	};

	class Patch : public DataFile {
	public:
		using DataFile::DataFile;

		virtual MidiProgramNumber patchNumber() const = 0;

		// For patch comparison
		static Synth::PatchData blankOut(std::vector<Range<int>> const &blankoutZones, Synth::PatchData const &inputData);
	};

}
