//
// Created by ianpo on 28/10/2024.
//

#pragma once

namespace FYC {

#ifdef FYC_DOUBLE
	#define REAL_DECIMAL_DIG	DBL_DECIMAL_DIG		// # of decimal digits of rounding precision
	#define REAL_DIG			DBL_DIG				// # of decimal digits of precision
	#define REAL_EPSILON		DBL_EPSILON			// smallest such that 1.0+DBL_EPSILON != 1.0
	#define REAL_HAS_SUBNORM	DBL_HAS_SUBNORM		// type does support subnormal numbers
	#define REAL_GUARD			FLT_GUARD
	#define REAL_MANT_DIG		DBL_MANT_DIG		// # of bits in mantissa
	#define REAL_MAX			DBL_MAX				// max value
	#define REAL_MAX_10_EXP		DBL_MAX_10_EXP		// max decimal exponent
	#define REAL_MAX_EXP		DBL_MAX_EXP			// max binary exponent
	#define REAL_MIN			DBL_MIN				// min normalized positive value
	#define REAL_MIN_10_EXP		DBL_MIN_10_EXP		// min decimal exponent
	#define REAL_MIN_EXP		DBL_MIN_EXP			// min binary exponent
	#define REAL_NORMALIZE		FLT_NORMALIZE
	#define REAL_RADIX			DBL_RADIX			// exponent radix
	#define REAL_TRUE_MIN		DBL_TRUE_MIN		// min positive value

	using Real = double;
#else
	#define REAL_DECIMAL_DIG	FLT_DECIMAL_DIG		// # of decimal digits of rounding precision
	#define REAL_DIG			FLT_DIG				// # of decimal digits of precision
	#define REAL_EPSILON		FLT_EPSILON			// smallest such that 1.0+FLT_EPSILON != 1.0
	#define REAL_HAS_SUBNORM	FLT_HAS_SUBNORM		// type does support subnormal numbers
	#define REAL_GUARD			FLT_GUARD
	#define REAL_MANT_DIG		FLT_MANT_DIG		// # of bits in mantissa
	#define REAL_MAX			FLT_MAX				// max value
	#define REAL_MAX_10_EXP		FLT_MAX_10_EXP		// max decimal exponent
	#define REAL_MAX_EXP		FLT_MAX_EXP			// max binary exponent
	#define REAL_MIN			FLT_MIN				// min normalized positive value
	#define REAL_MIN_10_EXP		FLT_MIN_10_EXP		// min decimal exponent
	#define REAL_MIN_EXP		FLT_MIN_EXP			// min binary exponent
	#define REAL_NORMALIZE		FLT_NORMALIZE
	#define REAL_RADIX			FLT_RADIX			// exponent radix
	#define REAL_TRUE_MIN		FLT_TRUE_MIN		// min positive value

	using Real = float;
#endif

	namespace Literal
	{
		constexpr Real operator ""_r(unsigned long long int value) {return Real(value);}
		constexpr Real operator ""_r(long double value) {return Real(value);}
	}

	struct Vec2
	{
		union
		{
			struct {Real x, y;};
			Real data[2];
		};

		Vec2() : x(0), y(0) {}
		explicit Vec2(Real value) : x(value), y(value) {}
		Vec2(Real x, Real y) : x(x), y(y) {}
		Vec2(const Vec2 &other) : x(other.x), y(other.y) {}
		~Vec2() = default;

		[[nodiscard]] Vec2 operator -() const {return {-x, -y};}

		Vec2& operator +=(const Vec2& other) {x += other.x; y += other.y; return *this;}
		[[nodiscard]] Vec2 operator +(Vec2 other) const {other += *this; return other;}

		Vec2& operator -=(const Vec2& other) {*this += -other; return *this;}
		[[nodiscard]] Vec2 operator -(const Vec2& other) const {Vec2 result(*this); result -= other; return result;}

		Vec2& operator +=(Real value) {*this += Vec2(value); return *this;}
		[[nodiscard]] Vec2 operator +(Real value) const {Vec2 result{*this}; result += Vec2{value}; return result;}

		Vec2& operator -=(Real value) {*this += -value; return *this;}
		[[nodiscard]] Vec2 operator -(Real value) const {Vec2 result{*this}; result -= value; return result;}

		Vec2& operator *=(Real value) {x *= value; y *= value; return *this;}
		[[nodiscard]] Vec2 operator *(Real value) const {Vec2 result{*this}; result *= value; return result;}

		Vec2& operator /=(Real value) {Real inv = Real(1) / value; *this *= inv; return *this;}
		[[nodiscard]] Vec2 operator /(Real value) const {Vec2 result{*this}; result /= value; return result;}

		[[nodiscard]] Real& operator[](const unsigned int index) {return data[index];}
		[[nodiscard]] const Real& operator[](const unsigned int index) const  {return data[index];}
	};

	struct Mat2x2
	{
		union
		{
			struct {Real c00, c01, c10, c11;};
			Vec2 columns[2];
			Real data[4];
		};

		Mat2x2() : c00(1), c01(0), c10(0), c11(1) {}
		explicit Mat2x2(Real value) : c00(value), c01(value), c10(value), c11(value) {}
		Mat2x2(Real c_00, Real c_10, Real c_01, Real c_11) : c00(c_00), c01(c_01), c10(c_10), c11(c_11) {}
		Mat2x2(const Vec2& col1, const Vec2& col2) : columns{col1, col2} {}
		Mat2x2(const Mat2x2& other) : c00(other.c00), c01(other.c01), c10(other.c10), c11(other.c11) {}
		~Mat2x2() = default;

		Mat2x2& operator +=(Real value) {c00 += value; c01 += value; c10 += value; c11 += value; return *this;}
		[[nodiscard]] Mat2x2 operator +(Real value) const {Mat2x2 other(*this); other += value; return other;}

		Mat2x2& operator -=(Real other) {*this += -other; return *this;}
		[[nodiscard]] Mat2x2 operator -(Real value) const {Mat2x2 other(*this); other -= value; return other;}

		Mat2x2& operator *=(Real value) {c00 *= value; c01 *= value; c10 *= value; c11 *= value; return *this;}
		[[nodiscard]] Mat2x2 operator *=(Real value) const {Mat2x2 other(*this); other *= value; return other;}

		Mat2x2& operator /=(Real value) {Real inv = Real(1)/value; *this *= inv; return *this;}
		[[nodiscard]] Mat2x2 operator /=(Real value) const {Mat2x2 other(*this); other /= value; return other;}

		Vec2 operator *(const Vec2& other) const {return Vec2{other.x * c00 + other.y * c10, other.x * c01 + other.y * c11};}

		Mat2x2 operator-() const {return {-c00, -c10, -c01, -c11};}

		Mat2x2& operator +=(const Mat2x2& other) {c00 += other.c00; c01 += other.c01; c10 += other.c10; c11 += other.c11; return *this;}
		[[nodiscard]] Mat2x2 operator +(Mat2x2 other) const {other += *this; return other;}

		Mat2x2& operator -=(const Mat2x2& other) {*this += -other; return *this;}
		[[nodiscard]] Mat2x2 operator -(Mat2x2 other) const {other -= *this; return other;}

		Mat2x2 operator *(const Mat2x2& other) const {
			return Mat2x2 {
					c00 * other.c00 + c10 * other.c01, c00 * other.c10 + c10 * other.c11,
					c01 * other.c00 + c11 * other.c01, c01 * other.c10 + c11 * other.c11,
			};
		}
		Mat2x2& operator *=(const Mat2x2& other) { Mat2x2 result = *this * other; *this = result; return *this; }


		[[nodiscard]] Vec2 GetCol(unsigned int col) const {return columns[col];}
		[[nodiscard]] Vec2 GetRow(unsigned int row) const {return {columns[0][row],columns[1][row]};}

		void SetCol(unsigned int col, Vec2 colData) {columns[col] = colData;}
		void SetRow(unsigned int row, Vec2 rowData) {columns[0][row] = rowData[0]; columns[1][row] = rowData[1];}

		[[nodiscard]] Vec2& operator[](unsigned int col) {return columns[col];}
		[[nodiscard]] const Vec2& operator[](unsigned int col) const {return columns[col];}

		[[nodiscard]] Real& operator()(unsigned int col, unsigned int row) {return columns[col][row];}
		[[nodiscard]] const Real& operator()(unsigned int col, unsigned int row) const {return columns[col][row];}

		[[nodiscard]] Real& operator()(unsigned int index) {return data[index];}
		[[nodiscard]] const Real& operator()(unsigned int index) const {return data[index];}
	};

	using Mat2 = Mat2x2;

	namespace Math
	{
		inline static constexpr Real tau {6.28318530717958647692};
		inline static constexpr Real pi  {3.14159265358979323846};
		inline static constexpr Real phi {1.61803398874989484820};
		inline static constexpr Real deg2rad {pi / static_cast<Real>(180)};
		inline static constexpr Real rad2deg {static_cast<Real>(180) / pi};

		[[nodiscard]] Real Sign(Real value);
		[[nodiscard]] Real Dot(const Vec2& a, const Vec2& b);
		[[nodiscard]] Real MagnitudeSqr(const Vec2& vec);
		[[nodiscard]] Real Magnitude(const Vec2& vec);
		[[nodiscard]] Vec2 Normalize(const Vec2& vec);
		void NormalizeInPlace(Vec2& vec);

		[[nodiscard]] Real Determinant(const Mat2x2& matrix);
		[[nodiscard]] Mat2x2 Inverse(const Mat2x2& matrix);
		[[nodiscard]] Mat2x2 Transpose(const Mat2x2& matrix);

		[[nodiscard]] Mat2x2 Rotation(Real radianAngle);
	}
} // FYC
