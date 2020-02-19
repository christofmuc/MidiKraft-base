/*
   Copyright (c) 2019 Christof Ruch. All rights reserved.

   Dual licensed: Distributed under Affero GPL license by default, an MIT license is available for purchase
*/

#include "AutoDetection.h"

#include "MidiController.h"
#include "Logger.h"
#include "Settings.h"

#include "FindSynthOnMidiNetwork.h"

#include <boost/format.hpp>

namespace midikraft {

	const char
		*kChannel = "channel",
		*kInput = "input",
		*kOutput = "output";

	std::string midiSetupKey(DiscoverableDevice *synth, std::string const &trait) {
		return (boost::format("%s-%s") % synth->getName() % trait).str();
	}

	void AutoDetection::autoconfigure(std::vector<std::shared_ptr<SimpleDiscoverableDevice>> &allSynths, ProgressHandler *progressHandler)
	{
		// For all devices that are discoverable, run the find method
		for (auto synthHolder : allSynths) {
			if (synthHolder) {
				findSynth(synthHolder.get(), progressHandler);
			}
		}
		listenerToAllFound(allSynths);
		sendChangeMessage();
	}

	void AutoDetection::quickconfigure(std::vector<std::shared_ptr<SimpleDiscoverableDevice>> &allSynths)
	{
		// For all devices that are discoverable, run the find method
		for (auto &synthHolder : allSynths) {
			if (synthHolder) {
				auto synth = synthHolder.get();
				// Load the synthesizer setup from the settings file
				std::string channelString = Settings::instance().get(midiSetupKey(synth, kChannel));
				if (!channelString.empty()) {
					int channel = std::atoi(channelString.c_str());
					if (channel >= 0 && channel < 16) {
						std::string input = Settings::instance().get(midiSetupKey(synth, kInput));
						std::string output = Settings::instance().get(midiSetupKey(synth, kOutput));
						if (!input.empty() && !output.empty()) {
							synth->setCurrentChannelZeroBased(input, output, channel);
							if (!checkSynth(synth)) {
								SimpleLogger::instance()->postMessage(
									(boost::format("Lost communication with %s on channel %d of device %s - please rerun autoconfigure!")
										% synth->getName() % synth->channel().toOneBasedInt() % synth->midiOutput()).str());
								synth->setChannel(MidiChannel::invalidChannel());
							}
						}
					}
				}
			}
		}
		listenerToAllFound(allSynths);
		sendChangeMessage();
	}

	void AutoDetection::findSynth(SimpleDiscoverableDevice *synth, ProgressHandler *progressHandler) {
		auto locations = FindSynthOnMidiNetwork::detectSynth(MidiController::instance(), *synth, progressHandler);
		if (locations.size() > 0) {
			for (auto loc : locations) {
				SimpleLogger::instance()->postMessage((boost::format("Found %s on channel %d replying on device %s when sending to %s")
					% synth->getName() % (loc.midiChannel + 1) % loc.inputName % loc.outputName).str());
			}

			// Select the last location (the first one might be the "All" devices which we don't want to address the devices individually)
			size_t loc = locations.size() - 1;
			synth->setCurrentChannelZeroBased(locations[loc].inputName, locations[loc].outputName, locations[loc].midiChannel);

			// Additionally, we want to persist this knowhow in the user settings file!
			Settings::instance().set(midiSetupKey(synth, kChannel), (boost::format("%d") % synth->channel().toZeroBasedInt()).str());
			Settings::instance().set(midiSetupKey(synth, kInput), synth->midiInput());
			Settings::instance().set(midiSetupKey(synth, kOutput), synth->midiOutput());
		}
		else {
			// Ups
			SimpleLogger::instance()->postMessage((boost::format("No %s could be detected - is it turned on?") % synth->getName()).str());
		}
	}

	bool AutoDetection::checkSynth(SimpleDiscoverableDevice *synth) {
		// This is the fast version of the FindSynthOnMidiNetwork routine - just a single pass to see if the synth responds
		IsSynth callback(*synth);

		MidiController::HandlerHandle handler = MidiController::makeOneHandle();
		MidiController::instance()->addMessageHandler(handler, [&](MidiInput *source, MidiMessage const &midimessage) {
			callback.handleIncomingMidiMessage(source, midimessage);
		});

		MidiController::instance()->enableMidiInput(synth->midiInput());

		// Send the detect message
		auto detectMessage = synth->deviceDetect(synth->channel().toZeroBasedInt());
		MidiController::instance()->getMidiOutput(synth->midiOutput())->sendMessageNow(detectMessage);

		// Sleep as long as the synth thinks is enough
		Thread::sleep(synth->deviceDetectSleepMS());

		// Check if we found it
		bool ok = false;
		for (auto found : callback.locations()) {
			if (found.inputName == synth->midiInput() && found.midiChannel == synth->channel().toZeroBasedInt()) {
				ok = true;
			}
		}
		MidiController::instance()->disableMidiInput(synth->midiInput());
		MidiController::instance()->removeMessageHandler(handler);
		return ok;
	}

	void AutoDetection::listenerToAllFound(std::vector<std::shared_ptr<SimpleDiscoverableDevice>> &allSynths) {
		// Listen to all detected synths
		for (auto synth : allSynths) {
			if (synth->channel().isValid()) {
				MidiController::instance()->enableMidiInput(synth->midiInput());
			}
		}
	}

}