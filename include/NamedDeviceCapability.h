/*
   Copyright (c) 2019 Christof Ruch. All rights reserved.

   Dual licensed: Distributed under Affero GPL license by default, an MIT license is available for purchase
*/

#pragma once

#include <string>

namespace midikraft {

	class NamedDeviceCapability {
	public:
		virtual std::string getName() const = 0;
	};

}
