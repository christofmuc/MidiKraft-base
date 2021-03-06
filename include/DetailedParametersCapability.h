/*
   Copyright (c) 2020 Christof Ruch. All rights reserved.

   Dual licensed: Distributed under Affero GPL license by default, an MIT license is available for purchase
*/

#pragma once

#include "SynthParameterDefinition.h"

namespace midikraft {

	class DetailedParametersCapability {
	public:
		virtual std::vector<std::shared_ptr<SynthParameterDefinition>> allParameterDefinitions() const = 0;
	};

}
