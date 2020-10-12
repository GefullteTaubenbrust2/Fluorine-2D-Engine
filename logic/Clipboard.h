#pragma once
#include <iostream>

namespace flo {
	std::string getFromClipboard();

	void copyToClipboard(std::string str);
}