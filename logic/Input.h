#pragma once
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../logic/Types.h"

namespace flo {
    ///<summary>
    ///One or more of these states may apply for a given input, not just in this file!
    ///</summary>
	enum InputType {
		none = 0,
		released = 1,
		held = 2,
		hit = 4,
	};

    ///<summary>
    ///Get the state of a given key.
    ///</summary>
    ///<param name="key">The id of the key to check.</param>
    ///<returns>The state of the key.</returns>
	InputType getKey(const unsigned int key);

    ///<summary>
    ///Get the string that has been entered using the keyboard since the input was last checked.
    ///</summary>
    ///<returns>The entered string.</returns>
	//std::string getEntered();

	void setInputString(std::string* str, bool* exit_state);

    ///<summary>
    ///Get the state of a given mouse button.
    ///</summary>
    ///<param name="mouse_button">The mouse button, 0 being left, 1 right and 2 the middle mouse button.</param>
    ///<returns>The success, false being a success.</returns>
	InputType getMouseButton(const unsigned int mouse_button);

    ///<summary>
    ///Get the position of the cursor, in pixels from the top left corner of the frame.
    ///</summary>
    ///<returns>The position.</returns>
	const glm::vec2 getMousePosition();

    ///<summary>
    ///Get the mouse scroll.
    ///</summary>
    ///<returns>The amount of scroll as a vector. Only the y component is expected to be not 0.</returns>
	const glm::vec2 getScroll();

    ///<summary>
    ///Set the keys to monitor for input. The index for these is used by the "getKey()" function.
    ///</summary>
    ///<param name="to_monitor">The keys that are to be monitored, as chars. Use the GLFW_X variables.</param>
    ///<param name="size">The amount of keys in the given array.</param>
	void setInputKeys(const i16* to_monitor, const int size);
}
