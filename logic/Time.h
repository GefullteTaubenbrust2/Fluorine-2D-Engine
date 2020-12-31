#pragma once
#include <chrono>

namespace flo {
	/// <summary> Units of time measurement </summary>
	enum TimeUnit {
		nanosecond = 0,
		microsecond = 1,
		millisecond = 2,
		second = 3
	};

	/// <summary> A basic struct for measuring time
	struct Time {
	protected:
		double milliseconds = 0;

	public:
		Time() = default;

		/// <summary> Convert an amount of time to a Time object </summary>
		/// <param name="amount"> The amount of units to convert to a Time object </param>
		/// <param name="unit"> The time unit to be used </param>
		Time(double amount, TimeUnit unit) {
			switch (unit) {
			case nanosecond :
				milliseconds = amount / 1000000;
				break;
			case microsecond :
				milliseconds = amount / 1000;
				break;
			case millisecond :
				milliseconds = amount;
				break;
			case second :
				milliseconds = amount * 1000;
				break;
			}
		}

		/// <summary> Convert seconds to a Time object </summary>
		/// <param name="seconds The amount of seconds
		explicit Time(double seconds) : milliseconds(seconds * 1000) {}

		double asSeconds() {
			return milliseconds / 1000;
		}

		double asMilliseconds() {
			return milliseconds;
		}

		double asMicroseconds() {
			return milliseconds * 1000;
		}
		
		double asNanoSeconds() {
			return milliseconds * 1000000;
		}

		Time operator+(const Time time) {
			return Time(milliseconds + time.milliseconds, millisecond);
		}

		Time operator-(const Time time) {
			return Time(milliseconds - time.milliseconds, millisecond);
		}

		Time operator*(const double right) {
			return Time(milliseconds * right, millisecond);
		}

		Time operator/(const double right) {
			return Time(milliseconds / right, millisecond);
		}

		bool operator==(const Time right) {
			return milliseconds == right.milliseconds;
		}

		bool operator>=(const Time right) {
			return milliseconds >= right.milliseconds;
		}

		bool operator<=(const Time right) {
			return milliseconds <= right.milliseconds;
		}

		bool operator>(const Time right) {
			return milliseconds > right.milliseconds;
		}

		bool operator<(const Time right) {
			return milliseconds < right.milliseconds;
		}

		/// <summary> Convert a Time object to seconds by casting </summary>
		operator double() const {
			return milliseconds / 1000;
		}
	};

	/// <summary> A simple struct for measuring time intervals </summary>
	/// \see Time
	struct Stopclock {
	protected:
		std::chrono::time_point<std::chrono::high_resolution_clock> start;

	public:
		/// <summary> Construct and start the stopclock </summary>
		Stopclock() {
			start = std::chrono::high_resolution_clock::now();
		}

		/// <summary> If this function is called, the stopclock measures its time from that point on </summary>
		void reset() {
			start = std::chrono::high_resolution_clock::now();
		}

		/// <summary> This function does not actually stop the stopclock, but measures the time since it was started or last reset </summary>
		/// <returns> The time since it was started or last reset </returns>
		/// \see reset
		Time stop() {
			return Time(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start).count(), nanosecond);
		}
	};
}