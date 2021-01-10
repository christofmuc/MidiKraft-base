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
		struct DataFileDescription {
			DataFileType dataFileID_; // Database identifier of data file (e.g. Single, Wave, Tone)
			std::string name; // Display name of the type
			bool canBeRequested; // If you can't request it, it is a write-only type. For example, the Alternate Tunings of the Rev2
			bool canBeSent;  // If you can't send the file type, it is read-only. For example, the global settings of the Rev2
		};
		struct DataFileImportDescription {
			DataStreamType dataStreamID = DataStreamType(0);
			std::string description;
			int startItemNo;
		};

		virtual std::vector<DataFileDescription> dataTypeNames() const = 0;
		virtual std::vector<DataFileImportDescription> dataFileImportChoices() const = 0;
		virtual std::vector<MidiMessage> requestDataItem(int itemNo, DataStreamType dataStreamType) = 0;
		virtual int numberOfMidiMessagesPerStreamType(DataStreamType dataTypeID) const = 0;
		virtual bool isPartOfDataFileStream(const MidiMessage &message, DataStreamType dataTypeID) const = 0;
		virtual bool isDataFile(const MidiMessage &message, DataFileType dataTypeID) const = 0;		
		virtual std::vector<std::shared_ptr<DataFile>> loadData(std::vector<MidiMessage> messages, DataStreamType dataTypeID) const = 0;
		
	};

}

