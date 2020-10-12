#include "GErrorHandler.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace fgr {
	bool error_thrown = 0;
	int errorCode = 0;

	int getErrorCode() {
		errorCode = glGetError();
		return errorCode;
	}

	int readErrorCode() {
		return errorCode;
	}

	void setError() {
		error_thrown = true;
	}

	bool errorThrown() {
		return error_thrown;
	}

	//void(*error_event)();
}