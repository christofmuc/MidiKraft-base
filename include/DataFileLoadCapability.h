/*
   Copyright (c) 2020 Christof Ruch. All rights reserved.

   Dual licensed: Distributed under Affero GPL license by default, an MIT license is available for purchase
*/

#pragma once

#include "JuceHeader.h"

#include "Patch.h"

namespace midikraft {

	class DataFileType {
	public:
		explicit DataFileType(int typeID) : dataTypeID_(typeID) { }

		int asInt() const { return dataTypeID_;  }

	private:
		int dataTypeID_;
	};

	class DataStreamType {
	public:
		explicit DataStreamType(int typeID) : dataStreamType_(typeID) { }

		int asInt() const { return dataStreamType_; }

	private:
		int dataStreamType_;
	};

	class DataFileLoadCapability {
	public:
		class DataFileDescription {
		public:
			DataFileDescription(DataFileType t, std::string const &name) : dataFileID_(t), name(name) {}

			DataFileType dataFileID_; // Database identifier of data file (e.g. Single, Wave, Tone)
			std::string name; // Display name of the type
		};
		struct DataFileImportDescription {
			DataFileImportDescription() : startItemNo(0) {}
			DataFileImportDescription(DataStreamType dataType, std::string const &description, int startIndex) : dataStreamID(dataType), description(description), startItemNo(startIndex) {}
			DataStreamType dataStreamID = DataStreamType(0);
			std::string description;
			int startItemNo;
		};

		// Not sure for what the following two items are
		virtual std::vector<DataFileDescription> dataTypeNames() const = 0;
		virtual bool isDataFile(const MidiMessage &message, DataFileType dataTypeID) const = 0;

		// This certainly is the generalized version of the StreamLoadCapability
		virtual std::vector<DataFileImportDescription> dataFileImportChoices() const = 0;
		virtual std::vector<MidiMessage> requestDataItem(int itemNo, DataStreamType dataStreamType) = 0;
		virtual int numberOfMidiMessagesPerStreamType(DataStreamType dataTypeID) const = 0;
		virtual bool isPartOfDataFileStream(const MidiMessage &message, DataStreamType dataTypeID) const = 0;
		virtual bool isStreamComplete(std::vector<MidiMessage> const &messages, DataStreamType streamType) const = 0;
		virtual bool shouldStreamAdvance(std::vector<MidiMessage> const &messages, DataStreamType streamType) const = 0;
		virtual std::vector<std::shared_ptr<DataFile>> loadData(std::vector<MidiMessage> messages, DataStreamType dataTypeID) const = 0;
		
	};


	class SingleMessageDataFileLoadCapability : public DataFileLoadCapability {
	public:
		virtual int numberOfMidiMessagesPerStreamType(DataStreamType dataTypeID) const override{
			ignoreUnused(dataTypeID);
			return 1;
		}

		virtual bool isStreamComplete(std::vector<MidiMessage> const &messages, DataStreamType streamType) const override {
			for (auto message : messages) {
				if (isPartOfDataFileStream(message, streamType)) {
					return true;
				}
			}
			return false;
		}

		virtual bool shouldStreamAdvance(std::vector<MidiMessage> const &messages, DataStreamType streamType) const override {
			ignoreUnused(messages, streamType);
			return false;
		}
	};
}

