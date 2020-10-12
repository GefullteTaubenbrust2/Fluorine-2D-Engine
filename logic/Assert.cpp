#include "Assert.h"
#include <iostream>

namespace flo {
	bool assert(bool condition) {
		if (!condition) {
			std::cerr << "An assertion has failed. Continuing to run the program will likely result in unexpected behavior.\n";
			system("pause");
		}
		return !condition;
	}
}