/*
   Copyright (c) 2019 Christof Ruch. All rights reserved.

   Dual licensed: Distributed under Affero GPL license by default, an MIT license is available for purchase
*/

#pragma once

#include "JuceHeader.h"

#include "MidiChannel.h"
#include "TypedNamedValue.h"

#include <map>

namespace midikraft {

	class DataFile;
	class Synth;

	class SynthParameterDefinition {
	public:
		//TODO - should I use inheritance? Or a capability interface again?
		enum class ParamType {
			INT,
			INT_ARRAY,
			LOOKUP,
			LOOKUP_ARRAY
		};

		// Synth parameters are usually integers, or arrays of integers (e.g. gated sequencer)
		virtual ParamType type() const = 0;
		virtual std::string name() const = 0; // Short name
		virtual std::string description() const = 0; // Long name / descriptive name

		virtual std::string valueInPatchToText(DataFile const &patch) const = 0;
	};

	class SynthIntValueParameterCapability {
	public:
		virtual int minValue() const = 0;
		virtual int maxValue() const = 0;
		virtual int sysexIndex() const = 0;
	};

	class SynthIntParameterCapability : public virtual SynthIntValueParameterCapability {
	public:
		//! Use this function to retrieve the integer value in the patch
		virtual bool valueInPatch(DataFile const &patch, int &outValue) const = 0;
		//! Use this function to set the integer value in the patch
		virtual void setInPatch(DataFile &patch, int value) const = 0;
	};

	class SynthVectorParameterCapability : public virtual SynthIntValueParameterCapability {
	public:
		virtual int endSysexIndex() const = 0;
		//! Use this function to retrieve the integer array value in the patch
		virtual bool valueInPatch(DataFile const &patch, std::vector<int> &outValue) const = 0;
		virtual void setInPatch(DataFile &patch, std::vector<int> value) const = 0;
	};

	class SynthLookupParameterCapability {
	public:
		virtual std::string valueAsText(int value) const = 0;
	};

	class SynthMultiLayerParameterCapability {
	public:
		virtual void setTargetLayer(int layerNo) = 0;
		virtual int getTargetLayer() const = 0;
		virtual void setSourceLayer(int layerNo) = 0;
		virtual int getSourceLayer() const = 0;
	};

	class SynthParameterLiveEditCapability { 
	public:
		// This is for live editing!
		virtual std::vector<MidiMessage> setValueMessages(std::shared_ptr<DataFile> const patch, Synth const *synth) const = 0;
	};

	class SynthParameterActiveDetectionCapability {
	public:
		//! A parameter is deemed active when its value can have any meaningful effect on the sound produced
		// This is very useful - if implemented - to thin out the number of parameters that need to be read to understand a patch
		virtual bool isActive(DataFile const *patch) const = 0;
	};

	class SynthParameterEditorCapability {
	public:
		//! This synth parameter allows to create a TypedNamedValue that can be used to edit it via a UI editor
		virtual std::shared_ptr<TypedNamedValue> makeTypedNamedValue() = 0;
	};

	//! Use this to indicate that the parameter can be mapped to CC or NRPN messages
	// Even if the synth doesn't understand CC or NRPN, this function can be used to map general controllers to the synth
	class SynthParameterControllerMappingCapability {
	public:
		virtual bool messagesMatchParameter(std::vector<juce::MidiMessage> const& messages, int &outNewValue) const = 0;
		virtual std::vector<juce::MidiMessage> createParameterMessages(int newValue, MidiChannel channel) const = 0;
	};

}
