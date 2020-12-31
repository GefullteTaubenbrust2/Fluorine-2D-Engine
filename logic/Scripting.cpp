#include "Scripting.h"
#include <typeindex>

namespace flo {
	const Type type_string = GENERATE_TYPE(String), type_int = GENERATE_TYPE(int), type_float = GENERATE_TYPE(float);

	String::String(char* data, int size) :
	data(data), size(size) {

	}

	String::String(std::string& str) :
	data(new char[str.size()]), size(str.size()) {
		std::copy(str.data(), str.data() + str.size(), data);
	}

	std::string String::convert() {
		std::string result(data, size);
		if (function_generated) dispose();
		return result;
	}

	void String::dispose() {
		delete[] data;
	}

	Type::Type(size_t type_signature, size_t type_size) :
	type_signature(type_signature), type_size(type_size) {

	}

	Function::Function(void(*function)(char*, char*), Type return_type, std::vector<Type> arg_types) :
	function(function), return_type(return_type), arg_types(arg_types) {
		for (int i = 0; i < arg_types.size(); ++i) mem_required += arg_types[i].type_size;
	}

	Variable::Variable(void* value_ptr, Type type, bool is_const) :
	value_ptr(value_ptr), type(type), is_const(is_const) {

	}

	Invocation::Invocation(Function& function, char* output_data) :
	function(function.function), memory(new char[function.mem_required]), output_data(output_data) {

	}

	void Invocation::run() {
		function(memory, output_data);
	}

	void Invocation::dispose() {
		delete[] memory;
	}

	Setter::Setter(Variable variable) :
	variable((char*)variable.value_ptr), value(new char[variable.type.type_size]), type(variable.type) {
		
	}

	void Setter::setValue() {
		std::copy(value, value + type.type_size, variable);
	}

	void Setter::dispose() {
		delete[] value;
	}

	void ConditionalStatement::dispose() {
		if (unique_allocation) delete condition;
	}

	VariableStorage::VariableStorage(Variable var, char* data) :
	type(var.type), value_ptr(var.value_ptr), data(data) {

	}

	void VariableStorage::setValue() {
		char* val = (char*)value_ptr;
		std::copy(val, val + type.type_size, data);
	}

	bool is_whitespace(char c) {
		return c == ' ' || c == '\n' || c == '\t';
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

	int scmin(int a, int b) {
		return a < b ? a : b;
	}

	int scmax(int a, int b) {
		return a > b ? a : b;
	}

	void ScriptInterpreter::throwError(std::string error) {
		currently_interpreting->build_error = true;
		std::cerr << "An error has occured interpreting the script: " << error << '\n';
		ScriptInterpreter::error = true;
		int s0 = scmax(cursor_offset - 20, 0);
		int a0 = scmin(cursor_offset, script.size() - 1);
		int e0 = scmin(cursor_offset + 1, script.size());
		int e1 = scmin(cursor_offset + 20, script.size()) + 1;
		std::cerr << "Location of the error: \n";
		std::cerr << script.substr(s0, a0 - s0) << " >>> " << script[a0] << " <<< " << script.substr(e0, e1 - e0) << '\n';
	}

	void ScriptInterpreter::readArgument(char* data, Type type) {
		//Skip forward to qualifiers
		for (; cursor_offset <= script.size(); ++cursor_offset) {
			if (cursor_offset == script.size()) {
				return;
			}
			if (!is_whitespace(script[cursor_offset])) break;
		}

		//Check for conditionals
		if (script[cursor_offset] == '#') {
			int if_pos = glm::min(script.size(), (size)cursor_offset + 3);
			int else_pos = glm::min(script.size(), (size)cursor_offset + 5);
			int while_pos = glm::min(script.size(), (size)cursor_offset + 8);
			std::string if_string = script.substr(cursor_offset, if_pos - cursor_offset);
			std::string else_string = script.substr(cursor_offset, else_pos - cursor_offset);
			std::string while_string = script.substr(cursor_offset, while_pos - cursor_offset);

			if (if_string == "#if") {
				cursor_offset += 3;
				if (cursor_offset == script.size()) {
					throwError("If statement is not completed.");
					return;
				}
				if (is_whitespace(script[cursor_offset])) {
					interpretIf();
					return;
				}
			}
			else if (else_string == "#else") {
				cursor_offset += 5;
				if (cursor_offset == script.size()) {
					throwError("Else statement is not completed.");
					return;
				}
				interpretElse();
				return;
			}
			else if (while_string == "#dowhile") {
				cursor_offset += 8;
				if (cursor_offset == script.size()) {
					throwError("While statement is not completed.");
					return;
				}
				interpretWhile();
				return;
			}
			throwError("Not a valid statement.");
			return;
		}

		//Interpret String
		if (type.type_signature == type_string.type_signature) {
			int start_offset = cursor_offset;
			char c = script[cursor_offset];
			if (script[cursor_offset] == '"') {
				start_offset = cursor_offset + 1;
				++cursor_offset;
			}
			else if (is_starting_char(script[cursor_offset])) goto read_as_function;
			else {
				throwError("Not a valid string.");
				return;
			}
			int end_offset = start_offset;
			bool fwdsl = false;
			for (; cursor_offset <= script.size(); ++cursor_offset) {
				if (cursor_offset == script.size()) {
					throwError("String is never terminated.");
					return;
				}
				if (script[cursor_offset] == '"' && !fwdsl) {
					end_offset = cursor_offset;
					break;
				}
				fwdsl = false;
				if (script[cursor_offset] == '\\') fwdsl = true;
			}
			++cursor_offset;
			std::string result = script.substr(start_offset, end_offset - start_offset);
			std::string corrected = "";
			fwdsl = false;
			for (int i = 0; i < result.size(); ++i) {
				if (!fwdsl) {
					if (result[i] == '\\') {
						fwdsl = true;
						continue;
					}
					corrected += result[i];
				}
				else {
					switch (result[i]) {
					case 'n':
						corrected += '\n';
						break;
					case '\\':
						corrected += '\\';
						break;
					case '"':
						corrected += '"';
						break;
					case 't':
						corrected += '\t';
						break;
					}
					fwdsl = false;
				}
			}
			if (data) {
				String& dstr = *((String*)data);
				dstr.data = new char[corrected.size()];
				std::copy(corrected.data(), corrected.data() + corrected.size(), dstr.data);
				dstr.size = corrected.size();
				dstr.function_generated = false;
				currently_interpreting->strings.push_back(dstr);
			}
		} 
		//Interpret Integer
		else if (type.type_signature == type_int.type_signature) {
			int start_offset = cursor_offset;
			char c = script[cursor_offset];
			if (is_numberi(c)) {
				start_offset = cursor_offset;
			}
			else if (is_starting_char(c)) goto read_as_function;
			else {
				throwError("Not a valid integer.");
				return;
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
			char c = script[cursor_offset];
			if (is_numberf(c)) {
				start_offset = cursor_offset;
			}
			else if (is_starting_char(c)) goto read_as_function;
			else {
				throwError("Not a valid float.");
				return;
			}
			int end_offset = start_offset;
			for (; cursor_offset < script.size(); ++cursor_offset) {
				char c = script[cursor_offset];
				if (!is_numberf(c)) {
					end_offset = cursor_offset;
					break;
				}
			}
			end_offset = cursor_offset;
			std::string substr = script.substr(start_offset, end_offset - start_offset);
			float result = std::atof(substr.data());
			if (data) *((float*)data) = result;
		}
		//Interpret as operation
		else {
			read_as_function:
			int start_offset = cursor_offset;
			char c = script[cursor_offset];
			if (is_starting_char(c)) {
				start_offset = cursor_offset;
			}
			else {
				throwError("Not a valid name.");
				return;
			}
			if (cursor_offset == script.size()) return;
			bool is_function = false;
			int end_offset = start_offset;
			for (; cursor_offset < script.size(); ++cursor_offset) {
				char c = script[cursor_offset];
				if (!is_text(c)) {
					end_offset = cursor_offset;
					if (c == '(') {
						is_function = true;
					}
					break;
				}
			}
			std::string substr = script.substr(start_offset, end_offset - start_offset);
			//Interpret as function
			if (is_function) {
				++cursor_offset;
				auto iter = functions.find(substr);
				if (iter == functions.end()) {
					throwError("Not a valid function name.");
					return;
				}
				if (iter->second.return_type.type_signature != type.type_signature) {
					throwError("The argument provided has an incompatible type.");
					return;
				}
				interpretFunction(data, iter->second);
			}
			else {
				bool is_argument = type.type_signature;
				bool is_setter = false;
				for (int i = cursor_offset; i < script.size(); ++i) {
					char c = script[i];
					if (!is_whitespace(c) && !is_argument) {
						if (c == '=') is_setter = true;
						else if (is_starting_char(c)) {
							auto iter = variable_types.find(substr);
							if (iter == variable_types.end()) {
								throwError("Not a valid type name.");
								return;
							}
							interpretDeclaration(iter->second);
							return;
						}
						break;
					}
				}

				//Interpret as variable
				if (!is_setter) {
					auto iter = currently_interpreting->variables.find(substr);
					if (iter == currently_interpreting->variables.end()) {
						throwError("Not a valid variable name.");
						return;
					}
					if (iter->second.type.type_signature != type.type_signature) {
						throwError("The argument provided has an incompatible type.");
						return;
					}
					currently_interpreting->read_order.push_back(glm::ivec2(1, currently_interpreting->variable_storages.size()));
					currently_interpreting->variable_storages.push_back(VariableStorage(iter->second, data));
				}
				//Interpret as setter
				else {
					auto iter = currently_interpreting->variables.find(substr);
					if (iter == currently_interpreting->variables.end()) {
						throwError("Not a valid variable name.");
						return;
					}
					interpretSetter(iter->second);
				}
			}
		}
	}

	void ScriptInterpreter::interpretIf() {
		ConditionalStatement statement;
		statement.condition = new bool;
		if (if_hierarchy.size() <= if_level) if_hierarchy.push_back(statement.condition);
		else if_hierarchy[if_level] = statement.condition;
		++if_level;

		readArgument((char*)statement.condition, GENERATE_TYPE(bool));

		if (error) return;

		currently_interpreting->read_order.push_back(glm::ivec2(3, currently_interpreting->conditionals.size()));
		currently_interpreting->conditionals.push_back(statement);
		int statement_index = currently_interpreting->conditionals.size() - 1;

		currently_interpreting->conditionals[statement_index].true_jump = currently_interpreting->read_order.size();

		for (; cursor_offset <= script.size(); ++cursor_offset) {
			if (cursor_offset == script.size()) {
				throwError("If statement is not followed by a curly bracket [{].");
				return;
			}
			char c = script[cursor_offset];
			if (!is_whitespace(c)) {
				if (script[cursor_offset] == '{') break;
				else {
					throwError("Illegal symbols found.");
					return;
				}
			}
		}
		++cursor_offset;

		for (; cursor_offset <= script.size(); ++cursor_offset) {
			if (cursor_offset == script.size()) {
				throwError("If statement is not terminated by a curly bracket [}].");
				return;
			}
			char c = script[cursor_offset];
			if (!is_whitespace(c)) {
				if (c == '}') {
					currently_interpreting->conditionals[statement_index].false_jump = currently_interpreting->read_order.size();
					++cursor_offset;
					break;
				}
				else {
					readArgument(nullptr, Type());
					--cursor_offset;
					if (error) return;
				}
			}
		}

		--if_level;
	}

	void ScriptInterpreter::interpretWhile() {
		ConditionalStatement statement;
		statement.condition = new bool;

		statement.true_jump = currently_interpreting->read_order.size();

		int start_pos = cursor_offset;

		if (error) return;

		for (; cursor_offset <= script.size(); ++cursor_offset) {
			if (cursor_offset == script.size()) {
				throwError("While statement is not followed by a curly bracket [{].");
				return;
			}
			char c = script[cursor_offset];
			if (script[cursor_offset] == '{') break;
		}
		++cursor_offset;

		for (; cursor_offset <= script.size(); ++cursor_offset) {
			if (cursor_offset == script.size()) {
				throwError("While statement is not terminated by a curly bracket [}].");
				return;
			}
			char c = script[cursor_offset];
			if (!is_whitespace(c)) {
				if (c == '}') {
					++cursor_offset;
					break;
				}
				else {
					readArgument(nullptr, Type());
					--cursor_offset;
					if (error) return;
				}
			}
		}

		int end_position = cursor_offset;

		cursor_offset = start_pos;
		readArgument((char*)statement.condition, GENERATE_TYPE(bool));
		cursor_offset = end_position;

		statement.false_jump = currently_interpreting->read_order.size() + 1;

		currently_interpreting->read_order.push_back(glm::ivec2(3, currently_interpreting->conditionals.size()));
		currently_interpreting->conditionals.push_back(statement);
	}

	void ScriptInterpreter::interpretElse() {
		if (!(if_hierarchy.size() > if_level && if_hierarchy[if_level])) {
			throwError("No if statement that corresponds to this else statement can be found.");
			return;
		}

		ConditionalStatement statement;
		statement.unique_allocation = false;
		statement.condition = if_hierarchy[if_level];

		for (; cursor_offset <= script.size(); ++cursor_offset) {
			if (cursor_offset == script.size()) {
				throwError("Else statement is not followed by a curly bracket [{].");
				return;
			}
			char c = script[cursor_offset];
			if (!is_whitespace(c)) {
				if (script[cursor_offset] == '{') break;
				else {
					throwError("Illegal symbols found.");
					return;
				}
			}
		}
		++cursor_offset;

		currently_interpreting->read_order.push_back(glm::ivec2(3, currently_interpreting->conditionals.size()));
		currently_interpreting->conditionals.push_back(statement);
		int statement_index = currently_interpreting->conditionals.size() - 1;

		currently_interpreting->conditionals[statement_index].false_jump = currently_interpreting->read_order.size();

		for (; cursor_offset <= script.size(); ++cursor_offset) {
			if (cursor_offset == script.size()) {
				throwError("Else statement is not terminated by a curly bracket [}].");
				return;
			}
			char c = script[cursor_offset];
			if (!is_whitespace(c)) {
				if (c == '}') {
					currently_interpreting->conditionals[statement_index].true_jump = currently_interpreting->read_order.size();
					++cursor_offset;
					break;
				}
				else {
					readArgument(nullptr, Type());
					--cursor_offset;
					if (error) return;
				}
			}
		}

		if_hierarchy[if_level] = nullptr;
	}

	void ScriptInterpreter::interpretDeclaration(Type type) {
		int start_offset = cursor_offset;
		for (; cursor_offset < script.size(); ++cursor_offset) {
			char c = script[cursor_offset];
			if (!is_whitespace(c)) {
				if (is_starting_char(c)) {
					start_offset = cursor_offset;
					break;
				}
				else {
					throwError("Not a valid variable name. Choose a different alias.");
					return;
				}
			}
		}
		if (cursor_offset == script.size()) return;
		bool is_function = false;
		int end_offset = start_offset;
		for (; cursor_offset < script.size(); ++cursor_offset) {
			char c = script[cursor_offset];
			if (!is_text(c)) {
				end_offset = cursor_offset;
				break;
			}
		}
		std::string substr = script.substr(start_offset, end_offset - start_offset);

		Variable var = Variable(new char[type.type_size], type, false);
		currently_interpreting->custom_variables.push_back(var);
		currently_interpreting->variables.insert(std::make_pair(substr, var));

		interpretSetter(var);
	}

	void ScriptInterpreter::interpretSetter(Variable& var) {
		bool validated = false;
		for (; cursor_offset <= script.size(); ++cursor_offset) {
			if (cursor_offset == script.size()) {
				throwError("Set operation is incomplete");
				return;
			}
			char c = script[cursor_offset];
			if (c == '=') {
				if (!validated) validated = true;
				else {
					throwError("Two equals [=] symbols found; this is illegal.");
					return;
				}
			}
			else if (!is_whitespace(c)) break;
		}
		if (!validated) {
			throwError("Set operation requires equals [=] symbol.");
			return;
		}
		if (var.is_const) {
			throwError("Cannot change the value of a constant.");
			return;
		}
		Setter setter = Setter(var);
		char* mem = setter.value;
		Type type = var.type;

		readArgument(mem, type);

		if (error) {
			setter.dispose();
			return;
		}

		currently_interpreting->read_order.push_back(glm::ivec2(2, currently_interpreting->setters.size()));
		currently_interpreting->setters.push_back(setter);
	}

	void ScriptInterpreter::interpretFunction(char* data, Function& function) {
		Invocation invoc = Invocation(function, data);
		char* mem = invoc.memory;
		for (int i = 0; i < function.arg_types.size(); ++i) {
			Type type = function.arg_types[i];
			readArgument(mem, type);

			if (error) {
				invoc.dispose();
				return;
			}

			bool final_arg = i == function.arg_types.size() - 1;
			for (; cursor_offset <= script.size(); ++cursor_offset) {
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
		
		if (function.arg_types.size() == 0) {
			for (; cursor_offset <= script.size(); ++cursor_offset) {
				if (cursor_offset == script.size()) {
					throwError("Function call is missing a separator [,] or terminating bracket [)].");
					return;
				}
				char c = script[cursor_offset];
				if (!is_whitespace(c)) {
					if (c == ')') {
						++cursor_offset;
						break;
					}
					else {
						throwError("Invalid characters found in function call.");
						return;
					}
				}
			}
		}

		currently_interpreting->read_order.push_back(glm::ivec2(0, currently_interpreting->code.size()));
		currently_interpreting->code.push_back(invoc);
	}

	void ScriptInterpreter::interpretScript(std::string& script, Script& sc, bool print_debug_info) {
		currently_interpreting = &sc;
		sc.clearScript();
		sc.variables = variables;
		sc.build_error = false;
		cursor_offset = 0;
		error = false;
		if_hierarchy.clear();
		if_level = 0;
		ScriptInterpreter::script = script;
		while (cursor_offset < script.size()) {
			readArgument(nullptr, Type());
			if (error) {
				sc.clearScript();
				return;
			}
		}
		std::cout << "Successfully interpreted script!\n";

		if (print_debug_info) {
			std::cout << "\n Debug Data:\n___________________________________\n\n";
			std::cout << " Functions to be run: " << sc.code.size() << '\n';
			std::cout << " Variables to be read: " << sc.variable_storages.size() << '\n';
			std::cout << " Setters to be run: " << sc.setters.size() << '\n';
			std::cout << " Conditional statements: " << sc.conditionals.size() << '\n';
			std::cout << " Number of allocated variables: " << sc.variables.size() << '\n';
			std::cout << " Number of custom variables: " << sc.custom_variables.size() << '\n';
			std::cout << " Total number of operations: " << sc.read_order.size() << "\n\n";
		}
	}

	void Script::runScript() {
		if (build_error) {
			std::cerr << "The script is in a bad state and cannot be executed.\n";
			return;
		}
		if (corrected_execution) {
			for (int i = 0; i < read_order.size(); ++i) {
				int id = read_order[i].x;
				int index = read_order[i].y;
				switch(id) {
				case 0:
					code[index].run();
					break;
				case 1:
					variable_storages[index].setValue();
					break;
				case 2:
					setters[index].setValue();
					break;
				case 3:
					ConditionalStatement & cs = conditionals[index];
					if (*cs.condition) i = cs.true_jump - 1;
					else i = cs.false_jump - 1;
				}
			}
		}
		else {
			for (int i = 0; i < variable_storages.size(); ++i) {
				variable_storages[i].setValue();
			}
			for (int i = 0; i < code.size(); ++i) {
				code[i].run();
			}
			for (int i = 0; i < setters.size(); ++i) {
				setters[i].setValue();
			}
		}
	}

	void Script::clearScript() {
		for (int i = 0; i < code.size(); ++i) {
			code[i].dispose();
		}
		for (int i = 0; i < strings.size(); ++i) {
			strings[i].dispose();
		}
		for (int i = 0; i < setters.size(); ++i) {
			setters[i].dispose();
		}
		for (int i = 0; i < custom_variables.size(); ++i) {
			delete[] custom_variables[i].value_ptr;
		}
		for (int i = 0; i < conditionals.size(); ++i) {
			conditionals[i].dispose();
		}
		code.clear();
		custom_variables.clear();
		setters.clear();
		variable_storages.clear();
		strings.clear();
		read_order.clear();
		conditionals.clear();
	}

	void ScriptInterpreter::registerFunction(Function function, std::string name) {
		functions.insert(std::make_pair(name, function));
	}

	void ScriptInterpreter::registerVariable(Variable variable, std::string name) {
		variables.insert(std::make_pair(name, variable));
	}

	void ScriptInterpreter::registerType(Type type, std::string name) {
		variable_types.insert(std::make_pair(name, type));
	}
}