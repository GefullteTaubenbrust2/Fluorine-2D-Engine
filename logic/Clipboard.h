#pragma once
#include <iostream>

namespace flo {
	///<summary>
	/// Get a string stored in the clipboard.
	///</summary>
	///<returns>The text stored in the clipboard.</returns>
	std::string getFromClipboard();

	///<summary>
	/// Put text into the clipboard.
	///</summary>
	///<param name="str">Text to put into the clipboard.</param>
	void copyToClipboard(const std::string& str);
}