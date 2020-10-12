#pragma once

#include <random>

namespace flo {
	template <typename Trng = std::mt19937>
	class Rng
	{
	private:
		std::random_device randomDevice;
		std::mt19937 mt;

	public:
		Rng(unsigned int seed)
			: randomDevice(), mt(seed)
		{}

		Rng()
			: randomDevice(), mt(randomDevice())
		{}

		template <bool is_integral, typename T> struct dist_selector;

		template <typename T> struct dist_selector<true, T> {
			using type = typename std::uniform_int_distribution<T>;
		};
		template <typename T> struct dist_selector<false, T> {
			using type = typename std::uniform_real_distribution<T>;
		};

		template<typename T>
		T next(T min = std::numeric_limits<T>::min(),
			T max = std::numeric_limits<T>::max())
		{
			typename dist_selector<std::is_integral<T>::value, T>::type dist(min, max);
			return dist(mt);
		}

		template<typename T>
		T gaussian(T mean, T sigma) {
			std::normal_distribution<T> d(mean, sigma);
			return d(randomDevice);
		}
	};

	extern Rng<> random;
}