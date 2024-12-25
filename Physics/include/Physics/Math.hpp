//
// Created by ianpo on 28/10/2024.
//

#pragma once

namespace FYC {

#ifdef FYC_DOUBLE
	using Real = double;
#else
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

		[[nodiscard]] Real& operator[](unsigned int index) {return data[index];}
		[[nodiscard]] const Real& operator[](unsigned int index) const  {return data[index];}
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

		Real Dot(const Vec2& a, const Vec2& b);
		Real MagnitudeSqr(const Vec2& vec);
		Real Magnitude(const Vec2& vec);
		Vec2 Normalize(const Vec2& vec);
		void NormalizeInPlace(Vec2& vec);

		Real Determinant(const Mat2x2& matrix);
		Mat2x2 Inverse(const Mat2x2& matrix);
		Mat2x2 Transpose(const Mat2x2& matrix);

		Mat2x2 Rotation(Real radianAngle);
	}
} // FYC
