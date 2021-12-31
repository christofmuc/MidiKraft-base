/*
   Copyright (c) 2019 Christof Ruch. All rights reserved.

   Dual licensed: Distributed under Affero GPL license by default, an MIT license is available for purchase
*/

#pragma once

#include "DiscoverableDevice.h"

#include "MidiLocationCapability.h"

namespace midikraft {

	class SimpleDiscoverableDevice : public DiscoverableDevice, public MidiLocationCapability {
	public:
		virtual void setCurrentChannelZeroBased(juce::MidiDeviceInfo const &midiInput, juce::MidiDeviceInfo const &midiOutput, int midiChannel);
		virtual void setChannel(MidiChannel newChannel);
		virtual void setInput(juce::MidiDeviceInfo const &midiInput); // To override auto-detection
		virtual void setOutput(juce::MidiDeviceInfo const &midiOutput); // To override auto-detection
		
		// DiscoverableDevice
		virtual bool wasDetected() const override;
		virtual void setWasDetected(bool wasDetected) override;

		// MidiLocationCapability
		virtual juce::MidiDeviceInfo midiInput() const override;
		virtual juce::MidiDeviceInfo midiOutput() const override;
		virtual MidiChannel channel() const override;

	private:
		MidiChannel channel_ = MidiChannel::invalidChannel();
		bool wasDetected_ = false;
		juce::MidiDeviceInfo midiInput_;
		juce::MidiDeviceInfo midiOutput_;
	};

}