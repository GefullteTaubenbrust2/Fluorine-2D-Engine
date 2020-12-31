#include "ScriptingUtils.h"

namespace flo {

	using namespace glm;

	namespace functions {

		void sin(char* in, char* out) {
			READ_START;
			float a = READ_INPUT(in, float);
			SET_OUTPUT(out, float, std::sin(a));
		}

		void cos(char* in, char* out) {
			READ_START;
			float a = READ_INPUT(in, float);
			SET_OUTPUT(out, float, std::cos(a));
		}

		void tan(char* in, char* out) {
			READ_START;
			float a = READ_INPUT(in, float);
			SET_OUTPUT(out, float, std::tan(a));
		}

		void pow(char* in, char* out) {
			READ_START;
			float a = READ_INPUT(in, float);
			float b = READ_INPUT(in, float);
			SET_OUTPUT(out, float, std::pow(a, b));
		}

		void min(char* in, char* out) {
			READ_START;
			float a = READ_INPUT(in, float);
			float b = READ_INPUT(in, float);
			SET_OUTPUT(out, float, a < b ? a : b);
		}

		void max(char* in, char* out) {
			READ_START;
			float a = READ_INPUT(in, float);
			float b = READ_INPUT(in, float);
			SET_OUTPUT(out, float, a > b ? a : b);
		}

		void step(char* in, char* out) {
			READ_START;
			float a = READ_INPUT(in, float);
			float b = READ_INPUT(in, float);
			SET_OUTPUT(out, float, a > b ? 1 : 0);
		}

		void cint(char* in, char* out) {
			READ_START;
			float a = READ_INPUT(in, float);
			SET_OUTPUT(out, int, (int)a);
		}

		void cfloat(char* in, char* out) {
			READ_START;
			int a = READ_INPUT(in, int);
			SET_OUTPUT(out, float, (float)a);
		}

		void cvec2(char* in, char* out) {
			READ_START;
			float a = READ_INPUT(in, float);
			float b = READ_INPUT(in, float);
			SET_OUTPUT(out, vec2, vec2(a, b));
		}

		void cvec3(char* in, char* out) {
			READ_START;
			float a = READ_INPUT(in, float);
			float b = READ_INPUT(in, float);
			float c = READ_INPUT(in, float);
			SET_OUTPUT(out, vec3, vec3(a, b, c));
		}

		void cvec4(char* in, char* out) {
			READ_START;
			float a = READ_INPUT(in, float);
			float b = READ_INPUT(in, float);
			float c = READ_INPUT(in, float);
			float d = READ_INPUT(in, float);
			SET_OUTPUT(out, vec4, vec4(a, b, c, d));
		}

		#define TWO_ELEMENT_OPERATION(operation_name, type, operation) void operation_name(char* in, char* out) { READ_START; type a = READ_INPUT(in, type); type b = READ_INPUT(in, type); SET_OUTPUT(out, type, operation); }
		#define TWO_TYPE_OPERATION(operation_name, typea, typeb, operation) void operation_name(char* in, char* out) { READ_START; typea a = READ_INPUT(in, typea); typeb b = READ_INPUT(in, typeb); SET_OUTPUT(out, typea, operation); }
		#define RETURN_VARIED_TWO_ELEMENT_OP(operation_name, typea, typeb, operation) void operation_name(char* in, char* out) { READ_START; typea a = READ_INPUT(in, typea); typea b = READ_INPUT(in, typea); SET_OUTPUT(out, typeb, operation); }

		TWO_ELEMENT_OPERATION(addi, int, a + b)
		TWO_ELEMENT_OPERATION(add1, float, a + b)
		TWO_ELEMENT_OPERATION(add2, vec2, a + b)
		TWO_ELEMENT_OPERATION(add3, vec3, a + b)
		TWO_ELEMENT_OPERATION(add4, vec4, a + b)

		TWO_ELEMENT_OPERATION(subtracti, int, a - b)
		TWO_ELEMENT_OPERATION(subtract1, float, a - b)
		TWO_ELEMENT_OPERATION(subtract2, vec2, a - b)
		TWO_ELEMENT_OPERATION(subtract3, vec3, a - b)
		TWO_ELEMENT_OPERATION(subtract4, vec4, a - b)

		TWO_ELEMENT_OPERATION(multiplyi, int, a * b)
		TWO_ELEMENT_OPERATION(multiply1, float, a * b)
		TWO_ELEMENT_OPERATION(multiply2, vec2, a * b)
		TWO_ELEMENT_OPERATION(multiply3, vec3, a * b)
		TWO_ELEMENT_OPERATION(multiply4, vec4, a * b)

		TWO_TYPE_OPERATION(multiply2f, vec2, float, a * b)
		TWO_TYPE_OPERATION(multiply3f, vec3, float, a * b)
		TWO_TYPE_OPERATION(multiply4f, vec4, float, a * b)

		TWO_ELEMENT_OPERATION(dividei, int, a + b)
		TWO_ELEMENT_OPERATION(divide1, float, a + b)

		TWO_ELEMENT_OPERATION(logical_or, bool, a | b)
		TWO_ELEMENT_OPERATION(logical_and, bool, a & b)

		RETURN_VARIED_TWO_ELEMENT_OP(greater, float, bool, a > b)
		RETURN_VARIED_TWO_ELEMENT_OP(less, float, bool, a < b)
		RETURN_VARIED_TWO_ELEMENT_OP(equals, float, bool, a == b)

		void logical_not(char* in, char* out) {
			READ_START;
			bool a = READ_INPUT(in, bool);
			SET_OUTPUT(out, bool, !a);
		}

		void abs1(char* in, char* out) {
			READ_START;
			float a = READ_INPUT(in, float);
			SET_OUTPUT(out, float, abs(a));
		}

		void abs2(char* in, char* out) {
			READ_START;
			vec2 a = READ_INPUT(in, vec2);
			SET_OUTPUT(out, float, length(a));
		}

		void abs3(char* in, char* out) {
			READ_START;
			vec3 a = READ_INPUT(in, vec4);
			SET_OUTPUT(out, float, length(a));
		}

		void abs4(char* in, char* out) {
			READ_START;
			vec4 a = READ_INPUT(in, vec4);
			SET_OUTPUT(out, float, length(a));
		}

		void print(char* in, char* out) {
			READ_START;
			String str = READ_INPUT(in, String);
			std::cout << str.convert();
		}

		void print1(char* in, char* out) {
			READ_START;
			float str = READ_INPUT(in, float);
			std::cout << str << '\n';
		}

		void print2(char* in, char* out) {
			READ_START;
			vec2 str = READ_INPUT(in, vec2);
			std::cout << "{ " << str.x << ", " << str.y << " }\n";
		}

		void print3(char* in, char* out) {
			READ_START;
			vec3 str = READ_INPUT(in, vec3);
			std::cout << "{ " << str.x << ", " << str.y << ", " << str.z << " }\n";
		}

		void print4(char* in, char* out) {
			READ_START;
			vec4 str = READ_INPUT(in, vec4);
			std::cout << "{ " << str.x << ", " << str.y << ", " << str.z << ", " << str.w << " }\n";
		}

		void get2(char* in, char* out) {
			READ_START;
			vec2 v = READ_INPUT(in, vec2);
			int n = READ_INPUT(in, int);
			SET_OUTPUT(out, float, v[n - 1]);
		}

		void get3(char* in, char* out) {
			READ_START;
			vec3 v = READ_INPUT(in, vec3);
			int n = READ_INPUT(in, int);
			SET_OUTPUT(out, float, v[n - 1]);
		}

		void get4(char* in, char* out) {
			READ_START;
			vec4 v = READ_INPUT(in, vec4);
			int n = READ_INPUT(in, int);
			SET_OUTPUT(out, float, v[n - 1]);
		}

		void join(char* in, char* out) {
			READ_START;
			String a = READ_INPUT(in, String);
			String b = READ_INPUT(in, String);
			std::string str = a.convert() + b.convert();
			SET_OUTPUT(out, String, String(str));
		}

		float pi = 3.141592653589;

		float e = 2.718281828;

		bool b_true = true;

		bool b_false = false;

		#define	REGISTRATION(func_name, out_type, ...) interpreter.registerFunction(Function(functions::func_name, out_type, std::vector<Type>{__VA_ARGS__}), #func_name)
		#define	REGISTRATION_VARIED(func_name, alias, out_type, ...) interpreter.registerFunction(Function(functions::func_name, out_type, std::vector<Type>{__VA_ARGS__}), #alias)

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
		//vec2 multiply2f(vec2, float)
		//vec3 multiply3f(vec3, float)
		//vec4 multiply4f(vec4, float)
		//void print(string)
		//void print1(float)
		//void print2(vec2)
		//void print3(vec3)
		//void print4(vec4)
	}

	void registerUtilFunctions(ScriptInterpreter& interpreter) {
		REGISTRATION(sin, GENERATE_TYPE(float), GENERATE_TYPE(float));
		REGISTRATION(cos, GENERATE_TYPE(float), GENERATE_TYPE(float));
		REGISTRATION(tan, GENERATE_TYPE(float), GENERATE_TYPE(float));

		REGISTRATION(pow, GENERATE_TYPE(float), GENERATE_TYPE(float), GENERATE_TYPE(float));
		REGISTRATION(max, GENERATE_TYPE(float), GENERATE_TYPE(float), GENERATE_TYPE(float));
		REGISTRATION(min, GENERATE_TYPE(float), GENERATE_TYPE(float), GENERATE_TYPE(float));
		REGISTRATION(step, GENERATE_TYPE(float), GENERATE_TYPE(float), GENERATE_TYPE(float));

		REGISTRATION_VARIED(cint, int, GENERATE_TYPE(int), GENERATE_TYPE(float));
		REGISTRATION_VARIED(cfloat, float, GENERATE_TYPE(float), GENERATE_TYPE(int));
		REGISTRATION_VARIED(cvec2, vec2, GENERATE_TYPE(vec2), GENERATE_TYPE(float), GENERATE_TYPE(float));
		REGISTRATION_VARIED(cvec3, vec3, GENERATE_TYPE(vec3), GENERATE_TYPE(float), GENERATE_TYPE(float), GENERATE_TYPE(float));
		REGISTRATION_VARIED(cvec4, vec4, GENERATE_TYPE(vec4), GENERATE_TYPE(float), GENERATE_TYPE(float), GENERATE_TYPE(float), GENERATE_TYPE(float));

		REGISTRATION_VARIED(logical_or, or, GENERATE_TYPE(bool), GENERATE_TYPE(bool), GENERATE_TYPE(bool));
		REGISTRATION_VARIED(logical_and, and , GENERATE_TYPE(bool), GENERATE_TYPE(bool), GENERATE_TYPE(bool));
		REGISTRATION_VARIED(logical_not, not , GENERATE_TYPE(bool), GENERATE_TYPE(bool));

		REGISTRATION(greater, GENERATE_TYPE(bool), GENERATE_TYPE(float), GENERATE_TYPE(float));
		REGISTRATION(less, GENERATE_TYPE(bool), GENERATE_TYPE(float), GENERATE_TYPE(float));
		REGISTRATION(equals, GENERATE_TYPE(bool), GENERATE_TYPE(float), GENERATE_TYPE(float));

		REGISTRATION(abs1, GENERATE_TYPE(float), GENERATE_TYPE(float));
		REGISTRATION(abs2, GENERATE_TYPE(float), GENERATE_TYPE(vec2));
		REGISTRATION(abs3, GENERATE_TYPE(float), GENERATE_TYPE(vec3));
		REGISTRATION(abs4, GENERATE_TYPE(float), GENERATE_TYPE(vec4));

		REGISTRATION(addi, GENERATE_TYPE(int), GENERATE_TYPE(int), GENERATE_TYPE(int));
		REGISTRATION(add1, GENERATE_TYPE(float), GENERATE_TYPE(float), GENERATE_TYPE(float));
		REGISTRATION(add2, GENERATE_TYPE(vec2), GENERATE_TYPE(vec2), GENERATE_TYPE(vec2));
		REGISTRATION(add3, GENERATE_TYPE(vec3), GENERATE_TYPE(vec3), GENERATE_TYPE(vec3));
		REGISTRATION(add4, GENERATE_TYPE(vec4), GENERATE_TYPE(vec4), GENERATE_TYPE(vec4));

		REGISTRATION(subtracti, GENERATE_TYPE(int), GENERATE_TYPE(int), GENERATE_TYPE(int));
		REGISTRATION(subtract1, GENERATE_TYPE(float), GENERATE_TYPE(float), GENERATE_TYPE(float));
		REGISTRATION(subtract2, GENERATE_TYPE(vec2), GENERATE_TYPE(vec2), GENERATE_TYPE(vec2));
		REGISTRATION(subtract3, GENERATE_TYPE(vec3), GENERATE_TYPE(vec3), GENERATE_TYPE(vec3));
		REGISTRATION(subtract4, GENERATE_TYPE(vec4), GENERATE_TYPE(vec4), GENERATE_TYPE(vec4));

		REGISTRATION(multiplyi, GENERATE_TYPE(int), GENERATE_TYPE(int), GENERATE_TYPE(int));
		REGISTRATION(multiply1, GENERATE_TYPE(float), GENERATE_TYPE(float), GENERATE_TYPE(float));
		REGISTRATION(multiply2, GENERATE_TYPE(vec2), GENERATE_TYPE(vec2), GENERATE_TYPE(vec2));
		REGISTRATION(multiply3, GENERATE_TYPE(vec3), GENERATE_TYPE(vec3), GENERATE_TYPE(vec3));
		REGISTRATION(multiply4, GENERATE_TYPE(vec4), GENERATE_TYPE(vec4), GENERATE_TYPE(vec4));

		REGISTRATION(multiply2f, GENERATE_TYPE(vec2), GENERATE_TYPE(vec2), GENERATE_TYPE(float));
		REGISTRATION(multiply3f, GENERATE_TYPE(vec3), GENERATE_TYPE(vec3), GENERATE_TYPE(float));
		REGISTRATION(multiply4f, GENERATE_TYPE(vec4), GENERATE_TYPE(vec4), GENERATE_TYPE(float));

		REGISTRATION(dividei, GENERATE_TYPE(int), GENERATE_TYPE(int), GENERATE_TYPE(int));
		REGISTRATION(divide1, GENERATE_TYPE(float), GENERATE_TYPE(float), GENERATE_TYPE(float));

		REGISTRATION(join, GENERATE_TYPE(String), GENERATE_TYPE(String), GENERATE_TYPE(String));

		REGISTRATION(print, Type(), GENERATE_TYPE(String));
		REGISTRATION(print1, Type(), GENERATE_TYPE(float));
		REGISTRATION(print2, Type(), GENERATE_TYPE(vec2));
		REGISTRATION(print3, Type(), GENERATE_TYPE(vec3));
		REGISTRATION(print4, Type(), GENERATE_TYPE(vec4));

		REGISTRATION(get2, GENERATE_TYPE(float), GENERATE_TYPE(vec2), GENERATE_TYPE(int));
		REGISTRATION(get3, GENERATE_TYPE(float), GENERATE_TYPE(vec3), GENERATE_TYPE(int));
		REGISTRATION(get4, GENERATE_TYPE(float), GENERATE_TYPE(vec4), GENERATE_TYPE(int));

		interpreter.registerVariable(Variable(&functions::pi, GENERATE_TYPE(float), true), "pi");
		interpreter.registerVariable(Variable(&functions::e, GENERATE_TYPE(float), true), "e");
		interpreter.registerVariable(Variable(&functions::b_true, GENERATE_TYPE(bool), true), "true");
		interpreter.registerVariable(Variable(&functions::b_false, GENERATE_TYPE(bool), true), "false");

		interpreter.registerType(GENERATE_TYPE(float), "float");
		interpreter.registerType(GENERATE_TYPE(int), "int");
		interpreter.registerType(GENERATE_TYPE(String), "string");
		interpreter.registerType(GENERATE_TYPE(vec2), "vec2");
		interpreter.registerType(GENERATE_TYPE(vec3), "vec3");
		interpreter.registerType(GENERATE_TYPE(vec4), "vec4");
		interpreter.registerType(GENERATE_TYPE(bool), "bool");
	}
}