/*
   Copyright (c) 2020 Christof Ruch. All rights reserved.

   Dual licensed: Distributed under Affero GPL license by default, an MIT license is available for purchase
*/

#pragma once

namespace midikraft {

	class ProgressHandler {
	public:
		virtual bool shouldAbort() const = 0;
		virtual void setProgressPercentage(double zeroToOne) = 0;
		virtual void onSuccess() = 0;
		virtual void onCancel() = 0;
	};

}
