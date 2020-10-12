#pragma once
#include <fstream>

namespace flo {
	std::istream& safeGetline(std::istream& is, std::string& t);
}