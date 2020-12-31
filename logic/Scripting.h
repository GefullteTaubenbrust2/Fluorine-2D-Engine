#pragma once
#include <map>
#include <vector>
#include <iostream>
#include "Types.h"

#include <glm/glm.hpp>

namespace flo {

	#define GENERATE_TYPE(type) flo::Type(flo::uniqueCode<type>(), sizeof(type))
	#define SET_OUTPUT(output_var_name, output_type, output) *((output_type*)output_var_name) = output
	#define READ_START int READ_COUNTER = 0;
	#define READ_INPUT(input_var_name, arg_type) *((arg_type*)(input_var_name + READ_COUNTER)); READ_COUNTER += sizeof(arg_type); 

	///<summary>
	/// A struct that should be used in favour of std::string when scripting.
	///</summary>
	struct String {
		char* data;
		int size;
		bool function_generated = true;

		///<summary>
		/// Construct a String.
		///</summary>
		///<param name="data">The data of the string.</param>
		///<param name="size">The length of the string.</param>
		String(char* data, int size);

		///<summary>
		/// Construct a String from a std::string.
		///</summary>
		String(std::string& str);

		///<summary>
		/// Convert the String to a std::string.
		///</summary>
		///<returns>A std::string with the same content.</returns>
		std::string convert();

		///<summary>
		/// Destroy the allocated contents. This must be called for every construction to prevent memory leaks.
		///</summary>
		void dispose();
	};

	///<summary>
	/// A struct for handling types in scripting.
	///</summary>
	struct Type {
		size_t type_signature = 0;
		size_t type_size = 0;

		Type() = default;

		///<summary>
		/// Construct a Type. Use GENERATE_TYPE() instead, as it is safe.
		///</summary>
		Type(size_t type_signature, size_t type_size);
	};

	extern const Type type_string, type_int, type_float;

	///<summary>
	/// A struct for handling functions in scripting.
	///</summary>
	struct Function {
		void(*function)(char*, char*);
		Type return_type;
		std::vector<Type> arg_types;
		size_t mem_required = 0;

		///<summary>
		/// Construct a function.
		///</summary>
		///<param name="function">A pointer to the actual function in code. The first char* is input, the second is output.</param>
		///<param name="return_type">The function's return type.</param>
		///<param name="arg_types">The types of the arguments.</param>
		Function(void(*function)(char*, char*), Type return_type, std::vector<Type> arg_types);
	};

	///<summary>
	/// A struct for handling variables and constants in scripting.
	///</summary>
	struct Variable {
		void* value_ptr;
		Type type;
		bool is_const;
		
		///<summary>
		/// Construct a Variable.
		///</summary>
		///<param name="value_ptr">A pointer to the actual value.</param>
		///<param name="type">The type of the variable.</param>
		///<param name="is_const">Should the script treat the variable as a constant?</param>
		Variable(void* value_ptr, Type type, bool is_const);
	};

	struct Invocation {
		void(*function)(char*, char*);
		char* memory;
		char* output_data;

		Invocation(Function& function, char* output_data);

		void run();

		void dispose();
	};

	struct Setter {
		char* variable;
		char* value;
		Type type;

		Setter(Variable variable);

		void setValue();

		void dispose();
	};

	struct VariableStorage {
		void* value_ptr;
		Type type;
		char* data;

		VariableStorage(Variable var, char* data);

		void setValue();
	};

	struct ConditionalStatement {
		bool* condition;
		bool unique_allocation = true;
		uint true_jump, false_jump;

		ConditionalStatement() = default;

		void dispose();
	};

	///<summary>
	/// An executable script.
	///</summary>
	struct Script {
		std::vector<Invocation> code;
		std::vector<VariableStorage> variable_storages;
		std::vector<Setter> setters;
		std::vector<ConditionalStatement> conditionals;
		std::vector<String> strings;
		std::vector<Variable> custom_variables;
		std::vector<glm::ivec2> read_order;
		std::map<std::string, Variable> variables;
		bool corrected_execution = true;
		bool build_error = true;

		///<summary>
		/// Clear the contents of the script.
		///</summary>
		void clearScript();

		///<summary>
		/// Run the script.
		///</summary>
		void runScript();
	};

	///<summary>
	/// A struct for interpreting scripts.
	///</summary>
	struct ScriptInterpreter {
	protected:
		std::map<std::string, Function> functions;
		std::map<std::string, Variable> variables;
		std::map<std::string, Type> variable_types;
		uint cursor_offset;
		std::string script;
		bool error;
		Script* currently_interpreting;
		std::vector<bool*> if_hierarchy;
		int if_level;

	public:
		ScriptInterpreter() = default;

		///<summary>
		/// Interpret a script.
		///</summary>
		///<param name="code">The source code.</param>
		///<param name="script">A reference to the script to initialize.</param>
		///<param name="print_debug_info">When set to true, information about the created script as well as error messages will be output.</param>
		void interpretScript(std::string& code, Script& script, bool print_debug_info = false);

		///<summary>
		/// Register a function.
		///</summary>
		///<param name="function">The function.</param>
		///<param name="name">The name of the function in code.</param>
		void registerFunction(Function function, std::string name);

		///<summary>
		/// Register a variable.
		///</summary>
		///<param name="variable">The variable.</param>
		///<param name="name">The name of the variable in code.</param>
		void registerVariable(Variable variable, std::string name);

		///<summary>
		/// Register a type.
		///</summary>
		///<param name="type">The type.</param>
		///<param name="name">The name of the type in code.</param>
		void registerType(Type type, std::string name);

	protected:
		void readArgument(char* data, Type type);

		void interpretFunction(char* data, Function& f);

		void interpretSetter(Variable& setter);

		void interpretDeclaration(Type type);

		void interpretIf();

		void interpretElse();

		void interpretWhile();

		void throwError(std::string error);
	};
}