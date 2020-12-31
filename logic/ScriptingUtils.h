#pragma once
#include "Scripting.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace flo {
	//functions:
	//float sin(float)
	//float cos(float)
	//float tan(float)
	//float pow(float, float)
	//float min(float, float)
	//float max(float, float)
	//float step(float, float)
	//int int(float)
	//float float(int)
	//vec2 vec2(float, float)
	//vec3 vec3(float, float, float)
	//vec4 vec4(float, float, float, float)
	//float add1(float, float)
	//vec2 add2(vec2, vec2)
	//vec3 add3(vec3, vec3)
	//vec4 add4(vec4, vec4)
	//float subtract1(float, float)
	//vec2 subtract2(vec2, vec2)
	//vec3 subtract3(vec3, vec3)
	//vec4 subtract4(vec4, vec4)
	//float multiply1(float, float)
	//vec2 multiply2(vec2, vec2)
	//vec3 multiply3(vec3, vec3)
	//vec4 multiply4(vec4, vec4)
	//vec2 multiply2(vec2, float)
	//vec3 multiply3(vec3, float)
	//vec4 multiply4(vec4, float)
	//string join(string, string)
	//void print(string)
	//void print1(float)
	//void print2(vec2)
	//void print3(vec3)
	//void print4(vec4)
	//float get2(vec2, int)
	//float get3(vec3, int)
	//float get4(vec4, int)
	//bool equals(float, float)
	//bool greater(float, float)
	//bool less(float, float)
	//bool not(bool)
	//bool or(bool, bool)
	//bool and(bool, bool)
	//const float pi
	//const float e
	//const bool true
	//const bool false
	///<summary>
	/// Register utility functionalities to an Interpreter, most of which is math-related.
	///</summary>
	void registerUtilFunctions(ScriptInterpreter& interpreter);
}