#pragma once
#include <iostream>

#define PI 3.141592653589793238460

namespace flo {
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
