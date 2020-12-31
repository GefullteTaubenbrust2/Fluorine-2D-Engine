#pragma once
#include <typeindex>

#ifdef _WIN64
typedef long long i64;
typedef int i32;
typedef short i16;
typedef char i8;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef unsigned long long size;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long long ulong;
#elif defined _WIN32
typedef long long i64;
typedef int i32;
typedef short i16;
typedef char i8;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef unsigned int size;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long long ulong;
#endif

typedef size_t typehash;

namespace flo {
	///<summary>
	/// Generate a unique hash for a type.
	///</summary>
	template<typename T>
	constexpr typehash uniqueCode() {
		return std::type_index(typeid(T)).hash_code();
	}

#define TYPEHASH(type) flo::uniqueCode<type>()
#define TYPE_VECTOR(...) std::vector<typehash>{ __VA_ARGS__ }
}

#define INVALID_INDEX 0xffffffff