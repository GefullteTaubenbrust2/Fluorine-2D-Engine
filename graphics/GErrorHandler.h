#pragma once

#if 0
#define graphics_throw_error(error) setError(); std::cerr << error << " has occured in line " << __LINE__ << " of the file " << __FILE__ << '\n';
#define graphics_throw_external(error) setError(); std::cerr << error << " has occured externally in line " << __LINE__ << " of the file " << __FILE__ << '\n';
#define graphics_check_error() if (glGetError() != GL_NO_ERROR) { graphics_throw_error("An openGL error"); }
#define graphics_check_external() if (glGetError() != GL_NO_ERROR) { graphics_throw_external("An openGL error"); }
#elif 1
#define graphics_throw_error(error) setError(); std::cerr << error << " has occured in line " << __LINE__ << " of the file " << __FILE__ << '\n';
#define graphics_throw_external(error) setError(); std::cerr << error << " has occured externally in line " << __LINE__ << " of the file " << __FILE__ << '\n';
#define graphics_check_error() if (getErrorCode() != GL_NO_ERROR) { graphics_throw_error("OpenGL Error " <<  readErrorCode()); }
#define graphics_check_external() if (getErrorCode() != GL_NO_ERROR) { graphics_throw_external("OpenGL Error " <<  readErrorCode()); }
#else
#define graphics_throw_error(error) 
#define graphics_throw_external(error) 
#define graphics_check_error 
#define graphics_check_external 
#endif

namespace fgr {
	void setError();

	int readErrorCode();

	int getErrorCode();

	bool errorThrown();

	/// <summary> A function to call if an error occurs </summary>
	//extern void(*error_event)();
}