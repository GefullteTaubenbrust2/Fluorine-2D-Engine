#include "Scripting.h"
#include <typeindex>

namespace flo {
	template<typename T>
	constexpr unsigned int uniqueCode() {
		return std::type_index(typeid(T)).hash_code();
	}

	const Type type_string = GENERATE_TYPE(String), type_int = GENERATE_TYPE(int), type_float = GENERATE_TYPE(float);

	String::String(char* data, int size) :
	data(data), size(size) {

	}

	std::string String::convert() {
		return std::string(data, size);
	}

	void String::dispose() {
		delete[] data;
	}

	Type::Type(unsigned int type_signature, size_t type_size) :
	type_signature(type_signature), type_size(type_size) {

	}

	Function::Function(void(*function)(char*, char*), Type return_type, std::vector<Type> arg_types) :
	function(function), return_type(return_type), arg_types(arg_types) {
		for (int i = 0; i < arg_types.size(); ++i) mem_required += arg_types[i].type_size;
	}

	bool is_whitespace(char c) {
		return c == ' ' || c == '\n' || c == c == '\t';
	}

	bool is_starting_char(char c) {
		return c == '_' || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
	}

	bool is_text(char c) {
		return (c >= '0' && c <= '9') || c == '_' || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
	}

	bool is_numberf(char c) {
		return (c >= '0' && c <= '9') || c == '.' || c == '-';
	}

	bool is_numberi(char c) {
		return (c >= '0' && c <= '9') || c == '-';
	}

	void ScriptInterpreter::throwError(std::string error) {
		std::cerr << "An error has occured running the script: " << error << '\n';
		ScriptInterpreter::error = true;
	}

	void ScriptInterpreter::readArgument(char* data, Type type) {
		//Interpret String
		if (type.type_signature == type_string.type_signature) {
			int start_offset = cursor_offset;
			for (; cursor_offset <= script.size(); ++cursor_offset) {
				if (cursor_offset == script.size()) {
					throwError("No string found where it was a required argument.");
					return;
				}
				char c = script[cursor_offset];
				if (!is_whitespace(c)) {
					if (script[cursor_offset] == '"') {
						start_offset = cursor_offset + 1;
						++cursor_offset;
						break;
					}
					else if (is_starting_char(script[cursor_offset])) goto read_as_function;
					else {
						throwError("Not a valid string.");
						return;
					}
				}
			}
			int end_offset = start_offset;
			for (; cursor_offset <= script.size(); ++cursor_offset) {
				if (cursor_offset == script.size()) {
					throwError("String is never terminated.");
					return;
				}
				if (script[cursor_offset] == '"') {
					end_offset = cursor_offset;
					break;
				}
			}
			++cursor_offset;
			std::string result = script.substr(start_offset, end_offset - start_offset);
			if (data) {
				char* result_data = new char[result.size()];
				std::copy(result.data(), result.data() + result.size(), result_data);
				String& dstr = *((String*)data);
				dstr.data = result_data;
				dstr.size = result.size();
			}
		} 
		//Interpret Integer
		else if (type.type_signature == type_int.type_signature) {
			int start_offset = cursor_offset;
			for (; cursor_offset <= script.size(); ++cursor_offset) {
				if (cursor_offset == script.size()) {
					throwError("No integer found where it was a required argument.");
					return;
				}
				char c = script[cursor_offset];
				if (!is_whitespace(c)) {
					if (is_numberi(c)) {
						start_offset = cursor_offset;
						break;
					}
					else if (is_starting_char(c)) goto read_as_function;
					else {
						throwError("Not a valid integer.");
						return;
					}
				}
			}
			int end_offset = start_offset;
			for (; cursor_offset < script.size(); ++cursor_offset) {
				char c = script[cursor_offset];
				if (!is_numberi(c)) {
					end_offset = cursor_offset;
					break;
				}
			}
			std::string substr = script.substr(start_offset, end_offset - start_offset);
			int result = std::atoi(substr.data());
			if (data) *((int*)data) = result;
		}
		//Interpret Float
		else if (type.type_signature == type_float.type_signature) {
			int start_offset = cursor_offset;
			for (; cursor_offset <= script.size(); ++cursor_offset) {
				if (cursor_offset == script.size()) {
					throwError("No float found where it was a required argument.");
					return;
				}
				char c = script[cursor_offset];
				if (!is_whitespace(c)) {
					if (is_numberf(c)) {
						start_offset = cursor_offset;
						break;
					}
					else if (is_starting_char(c)) goto read_as_function;
					else {
						throwError("Not a valid float.");
						return;
					}
				}
			}
			int end_offset = start_offset;
			for (; cursor_offset < script.size(); ++cursor_offset) {
				char c = script[cursor_offset];
				if (!is_numberf(c)) {
					end_offset = cursor_offset;
					break;
				}
			}
			std::string substr = script.substr(start_offset, end_offset - start_offset);
			float result = std::atof(substr.data());
			if (data) *((float*)data) = result;
		}
		//Interpret Function
		else {
			read_as_function:
			int start_offset = cursor_offset;
			for (; cursor_offset < script.size(); ++cursor_offset) {
				char c = script[cursor_offset];
				if (!is_whitespace(c)) {
					if (is_starting_char(c)) {
						start_offset = cursor_offset;
						break;
					}
					else {
						throwError("Not a valid function name.");
						return;
					}
				}
			}
			int end_offset = start_offset;
			for (; cursor_offset < script.size(); ++cursor_offset) {
				char c = script[cursor_offset];
				if (!is_text(c)) {
					if (c != '(') {
						throwError("Not a valid syntax for a function.");
						return;
					}
					end_offset = cursor_offset;
					break;
				}
			}
			++cursor_offset;
			std::string substr = script.substr(start_offset, end_offset - start_offset);
			auto iter = functions.find(substr);
			if (iter == functions.end()) {
				throwError("Not a valid function name.");
				return;
			}
			runFunction(data, iter->second);
		}
	}

	void ScriptInterpreter::runFunction(char* data, Function& function) {
		char* memory = new char[function.mem_required];
		char* mem = memory;
		for (int i = 0; i < function.arg_types.size(); ++i) {
			Type type = function.arg_types[i];
			readArgument(mem, type);

			if (error) {
				delete[] memory;
				return;
			}

			bool final_arg = i == function.arg_types.size() - 1;
			for (; cursor_offset <= script.size(); ++i) {
				if (cursor_offset == script.size()) {
					throwError("Function call is missing a separator [,] or terminating bracket [)].");
					return;
				}
				char c = script[cursor_offset];
				if (!is_whitespace(c)) {
					char separator = final_arg ? ')' : ',';
					if (c == separator) {
						++cursor_offset;
						break;
					}
					else {
						throwError("Invalid characters found in function call.");
						return;
					}
				}
			}

			mem += type.type_size;
		}
		function.function(memory, data);
		delete[] memory;
	}

	void ScriptInterpreter::runScrip(std::string& script) {
		cursor_offset = 0;
		error = false;
		ScriptInterpreter::script = script;
		while (cursor_offset < script.size()) {
			readArgument(nullptr, Type());
			if (error) return;
		}
	}

	void ScriptInterpreter::registerFunction(Function function, std::string name) {
		functions.insert(std::make_pair(name, function));
	}
}