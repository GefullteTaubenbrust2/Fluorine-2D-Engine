#pragma once
#include <iostream>

#define PI 3.141592653589793238460

namespace flo {
	/**
	<summary> A generic two-dimensional vector, along with the associated math.
	*/
	/*template<typename Type>
	struct Vector2 {
		Type x = 0, y = 0;

		Vector2() = default;
		
		Vector2(const Type x, const Type y);

		Vector2<Type> operator+(const Vector2<Type> right);

		Vector2<Type> operator-(const Vector2<Type> right);

		Vector2<Type> operator*(const Type right);

		Vector2<Type> operator/(const Type right);

		Type cross(const Vector2<Type> right);

		Type dot(const Vector2<Type> right);

		Type abs();

		double theta();
	};*/

	/**
	<summary> A very simplistic 3D vector
	*/
	/*template<typename Type>
	struct Vector3 {
		Type x = 0, y = 0, z = 0;

		Vector3() = default;

		Vector3(const Type x, const Type y, const Type z);

		Vector3<Type> operator+(const Vector3<Type> right);

		Vector3<Type> operator-(const Vector3<Type> right);

		Vector3<Type> operator*(const Type right);

		Vector3<Type> operator/(const Type right);

		Vector3<Type> cross(const Vector3<Type> right);

		Type dot(const Vector2<Type> right);

		Type abs();
	};*/

	/**
	<summary> A generic complex number, along with the associated math.</summary>
	*/
	template<typename Type>
	struct Complex {
		Type real = 0, imag = 0;

		Complex() = default;

		Complex(const Type real, const Type imag);

		Complex<Type> operator+(const Complex<Type> right);

		Complex<Type> operator-(const Complex<Type> right);

		Complex<Type> operator*(const Complex<Type> right);

		Complex<Type> operator/(const Complex<Type> right);

		Complex<Type> operator*(const Type right);

		Complex<Type> operator/(const Type right);

		Type abs();

		double theta();
	};

	/*
	typedef Vector2<int> Vector2i;
	typedef Vector2<double> Vector2d;
	typedef Vector2<float> Vector2f;

	typedef Vector3<int> Vector3i;
	typedef Vector3<double> Vector3d;
	typedef Vector3<float> Vector3f;
	*/
}

/*
<summary> Definitions
*/
namespace flo {
	//Vector definitions

	/*
	template<typename Type>
	Vector2<Type>::Vector2(const Type x, const Type y) : x(x), y(y) {}

	template<typename Type>
	Vector2<Type> Vector2<Type>::operator+(const Vector2<Type> right) {
		return Vector2(x + right.x, y + right.y);
	}

	template<typename Type>
	Vector2<Type> Vector2<Type>::operator-(const Vector2<Type> right) {
		return Vector2(x - right.x, y - right.y);
	}

	template<typename Type>
	Vector2<Type> Vector2<Type>::operator*(const Type right) {
		return Vector2(x * right, y * right);
	}

	template<typename Type>
	Vector2<Type> Vector2<Type>::operator/(const Type right) {
		return Vector2(x / right, y / right);
	}

	template<typename Type>
	Type Vector2<Type>::dot(const Vector2<Type> right) {
		return x * right.x + y * right.y;
	}

	template<typename Type>
	Type Vector2<Type>::cross(const Vector2<Type> right) {
		return x * right.y - y * right.x;
	}

	template<typename Type>
	Type Vector2<Type>::abs() {
		return std::sqrt(x*x + y*y);
	}

	template<typename Type>
	double Vector2<Type>::theta() {
		return std::atan2(y, x);
	}

	//3D vector definitions

	template<typename Type>
	Vector3<Type>::Vector3(const Type x, const Type y, const Type z) :
	x(x), y(y), z(z) {}

	template<typename Type>
	Vector3<Type> Vector3<Type>::operator+(const Vector3<Type> right) {
		return Vector3<Type>(x + right.x, y + right.y, z + right.z);
	}

	template<typename Type>
	Vector3<Type> Vector3<Type>::operator-(const Vector3<Type> right) {
		return Vector3<Type>(x - right.x, y - right.y, z - right.z);
	}

	template<typename Type>
	Vector3<Type> Vector3<Type>::operator*(const Type right) {
		return Vector3<Type>(x * right, y * right, z * right);
	}

	template<typename Type>
	Vector3<Type> Vector3<Type>::operator/(const Type right) {
		return Vector3<Type>(x / right, y / right, z / right);
	}

	template<typename Type>
	Vector3<Type> Vector3<Type>::cross(const Vector3<Type> right) {
		return Vector3(y * right.z - z * right.y, z * right.x - x * right.z, x * right.y - y * right.x);
	}

	template<typename Type>
	Type Vector3<Type>::dot(const Vector2<Type> right) {
		return x * right.x + y * right.y + z * right.z;
	}

	template<typename Type>
	Type Vector3<Type>::abs() {
		return std::sqrt(x*x + y*y + z*z);
	}
	*/

	//Complex definitions

	template<typename Type>
	Complex<Type>::Complex(const Type real, const Type imag) : real(real), imag(imag) {}

	template<typename Type>
	Complex<Type> Complex<Type>::operator+(const Complex<Type> right) {
		return Complex(real + right.real, imag + right.imag);
	}

	template<typename Type>
	Complex<Type> Complex<Type>::operator-(const Complex<Type> right) {
		return Complex(real - right.real, imag - right.imag);
	}

	template<typename Type>
	Complex<Type> Complex<Type>::operator*(const Complex<Type> right) {
		return Complex(real*right.real - imag*right.imag, real*right.imag + imag*right.real);
	}

	template<typename Type>
	Complex<Type> Complex<Type>::operator/(const Complex<Type> right) {
		return Complex(real / right.real + imag / right.imag, -real / right.imag + imag / right.real);
	}

	template<typename Type>
	Complex<Type> Complex<Type>::operator*(const Type right) {
		return Complex(real * right, imag * right);
	}

	template<typename Type>
	Complex<Type> Complex<Type>::operator/(const Type right) {
		return Complex(real / right, imag / right);
	}

	template<typename Type>
	Type Complex<Type>::abs() {
		return std::sqrt(real*real + imag*imag);
	}

	template<typename Type>
	double Complex<Type>::theta() {
		return std::atan2(imag, real);
	}
}
