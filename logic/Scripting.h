#pragma once
#include <map>
#include <vector>
#include <iostream>

namespace flo {

	#define GENERATE_TYPE(type) flo::Type(flo::uniqueCode<type>(), sizeof(type))
	#define SET_OUTPUT(output_var_name, output_type, output) *((output_type*)output_var_name) = output
	#define READ_START int READ_COUNTER = 0;
	#define READ_INPUT(input_var_name, arg_type) *((arg_type*)(input_var_name + READ_COUNTER)); READ_COUNTER += sizeof(arg_type); 

	template<typename T>
	constexpr unsigned int uniqueCode();

	struct String {
		char* data;
		int size;

		String(char* data, int size);

		std::string convert();

		void dispose();
	};

	struct Type {
		unsigned int type_signature = 0;
		size_t type_size = 0;

		Type() = default;

		Type(unsigned int type_signature, size_t type_size);
	};

	extern const Type type_string, type_int, type_float;

	struct Function {
		void(*function)(char*, char*);
		Type return_type;
		std::vector<Type> arg_types;
		size_t mem_required = 0;

		Function(void(*function)(char*, char*), Type return_type, std::vector<Type> arg_types);
	};

	struct ScriptInterpreter {
		std::map<std::string, Function> functions;
		unsigned int cursor_offset;
		std::string script;
		bool error;

		ScriptInterpreter() = default;

		void readArgument(char* data, Type type);

		void runFunction(char* data, Function& f);

		void runScrip(std::string& script);

		void throwError(std::string error);

		void registerFunction(Function function, std::string name);
	};
}