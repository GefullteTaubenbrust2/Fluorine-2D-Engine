#pragma once

#include "../logic/Types.h"

namespace flo {
    ///<summary>
    ///Sort an array of anything using the radix sort, given each value has a char associated to it, by which to sort.
    ///</summary>
    ///<param name="values">The associated chars by which to sort.</param>
    ///<param name="associated">The actual data to sort.</param>
    ///<param name="count">The amount of elements in both arrays.</param>
    ///<param name="element_size">The size of each element.</param>
    ///<param name="buffer">For the sake of optimization, the user might decide to allocate a char array '(3 + element_size) * count' in size themselves.</param>
	void radixSort(i8* values, i8* associated, const int count, const int element_size, char* buffer = nullptr);
}
