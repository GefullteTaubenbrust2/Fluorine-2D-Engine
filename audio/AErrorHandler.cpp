#include "AErrorHandler.h"

namespace fau {
	bool error_thrown = 0;

	void setError() {
		error_thrown = true;
	}

	bool errorThrown() {
		return error_thrown;
	}

	//void(*error_event)();
}