/*
   Copyright (c) 2019 Christof Ruch. All rights reserved.

   Dual licensed: Distributed under Affero GPL license by default, an MIT license is available for purchase
*/

#include "Synth.h"

#include "Patch.h"
#include "MidiController.h"
#include "MidiHelpers.h"
#include "Logger.h"

#include "EditBufferCapability.h"
#include "ProgramDumpCapability.h"
#include "BankDumpCapability.h"
#include "DataFileLoadCapability.h"
#include "StreamDumpCapability.h"

#include <boost/format.hpp>

namespace midikraft {

	Synth::PatchData Synth::filterVoiceRelevantData(std::shared_ptr<DataFile> unfilteredData) const
	{
		// The default implementation does nothing, i.e. all bytes are relevant for the sound of the patch
		// That would be the case e.g. for the Korg DW8000 or the Kawai K3, which have not even a patch name in the patch data
		return unfilteredData->data();
	}

	TPatchVector Synth::loadSysex(std::vector<MidiMessage> const &sysexMessages)
	{
		TPatchVector result;
		// Now that we have a list of messages, let's see if there are (hopefully) any patches between them
		int patchNo = 0;
		auto editBufferSynth = dynamic_cast<EditBufferCapability *>(this);
		auto programDumpSynth = dynamic_cast<ProgramDumpCabability *>(this);
		auto bankDumpSynth = dynamic_cast<BankDumpCapability*>(this);
		auto dataFileLoadSynth = dynamic_cast<DataFileLoadCapability*>(this);
		auto streamDumpSynth = dynamic_cast<StreamDumpCapability*>(this);
		if (streamDumpSynth) {
			// The stream dump synth loads all at once
			result = streamDumpSynth->loadStreamDump(sysexMessages);
		}
		else {
			// The other Synth types load message by message
			for (auto message : sysexMessages) {
				if (editBufferSynth && editBufferSynth->isEditBufferDump(message)) {
					auto patch = editBufferSynth->patchFromSysex(message);
					if (patch) {
						result.push_back(patch);
						// As these are edit buffers, I would not expect the patchNumber to be set by the loading routine
						patch->setPatchNumber(MidiProgramNumber::fromZeroBase(patchNo));
					}
					else {
						Logger::writeToLog((boost::format("Error decoding edit buffer dump for patch %d, skipping it") % patchNo).str());
					}
					patchNo++;
				}
				else if (programDumpSynth && programDumpSynth->isSingleProgramDump(message)) {
					auto patch = programDumpSynth->patchFromProgramDumpSysex(message);
					if (patch) {
						result.push_back(patch);
					}
					else {
						Logger::writeToLog((boost::format("Error decoding edit buffer dump for patch %d, skipping it") % patchNo).str());
					}
					patchNo++;
				}
				else if (bankDumpSynth && bankDumpSynth->isBankDump(message)) {
					auto morePatches = bankDumpSynth->patchesFromSysexBank(message);
					Logger::writeToLog((boost::format("Loaded bank dump with %d patches") % morePatches.size()).str());
					std::copy(morePatches.begin(), morePatches.end(), std::back_inserter(result));
				}
				else if (dataFileLoadSynth) {
					// Should test all data file types!
					for (int dataType = 0; dataType < dataFileLoadSynth->dataTypeNames().size(); dataType++) {
						if (dataFileLoadSynth->isDataFile(message, dataType)) {
							// Hit, we can load this
							auto items = dataFileLoadSynth->loadData({ message }, dataType);
							std::copy(items.begin(), items.end(), std::back_inserter(result));
						}
					}
				}
				else {
					// The way I ended up here was to load the ZIP of the Pro3 factory programs, and that includes the weird macOS resource fork
					// with a syx extension, wrongly getting interpreted as a real sysex file.
					SimpleLogger::instance()->postMessage("Ignoring sysex message found, not implemented: " + message.getDescription());
				}
			}
		}

		return result;
	}

	void Synth::saveSysex(std::string const &filename, std::vector<juce::MidiMessage> messages)
	{
		File outFile = File::createFileWithoutCheckingPath(filename);
		FileOutputStream outStream(outFile);

		for (auto message : messages) {
			outStream.write(message.getRawData(), message.getRawDataSize());
		}
	}

	void Synth::sendPatchToSynth(MidiController *controller, SimpleLogger *logger, std::shared_ptr<DataFile> dataFile)
	{
		std::vector<MidiMessage> messages;
		auto realPatch = std::dynamic_pointer_cast<midikraft::Patch>(dataFile);
		if (realPatch) {
			// Default implementation is to just shoot it to the Midi output and hope for the best, no handshake is done
			auto editBufferCapability = dynamic_cast<EditBufferCapability *>(this);
			auto programDumpCapability = dynamic_cast<ProgramDumpCabability *>(this);
			if (editBufferCapability) {
				messages = editBufferCapability->patchToSysex(*realPatch);
			}
			else if (programDumpCapability) {
				messages = programDumpCapability->patchToProgramDumpSysex(*realPatch);
			}
			else {
				SimpleLogger::instance()->postMessage("Program error - unknown strategy to send patch out to synth");
			}
		}
		else {
			auto dfcl = dynamic_cast<DataFileSendCapability*>(this);
			if (dfcl) {
				messages = dfcl->dataFileToMessages(dataFile);
			}
			else {
				SimpleLogger::instance()->postMessage("Program error - Synth has DataFile as patch, but did not implement DataFileSendCapability");
			}
		}
		auto midiLocation = dynamic_cast<MidiLocationCapability *>(this);
		if (midiLocation && !messages.empty()) {
			logger->postMessage((boost::format("Sending patch %s to %s") % dataFile->name() % getName()).str());
			controller->enableMidiOutput(midiLocation->midiOutput());
			controller->getMidiOutput(midiLocation->midiOutput())->sendBlockOfMessagesNow(MidiHelpers::bufferFromMessages(messages));
		}
	}

}
