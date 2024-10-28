//
// Created by ianpo on 28/10/2024.
//

#include "Physics/Math.hpp"

using namespace FYC::Literal;

namespace FYC
{
	namespace Math
	{
		Real Dot(const Vec2 &a, const Vec2 &b)
		{
			return a.x * b.x + a.y * b.y;
		}

		Real MagnitudeSqr(const Vec2& vec)
		{
			return Dot(vec, vec);
		}

		Real Magnitude(const Vec2& vec)
		{
			return std::sqrt(MagnitudeSqr(vec));
		}

		Vec2 Normalize(const Vec2& vec)
		{
			return vec / Magnitude(vec);
		}

		void NormalizeInPlace(Vec2& vec)
		{
			vec /= Magnitude(vec);
		}

		Real Determinant(const Mat2x2& matrix) {
			return matrix.c00 * matrix.c11 - matrix.c10 * matrix.c01;
		}

		Mat2x2 Inverse(const Mat2x2 &matrix) {
			Real inv = 1_r / Determinant(matrix);
			Mat2x2 result {
				 matrix.c11, -matrix.c10,
				-matrix.c01,  matrix.c00,
				};
			result *= inv;
			return result;
		}

		Mat2x2 Transpose(const Mat2x2 &matrix)
		{
			return Mat2x2 {
				matrix.c00, matrix.c01,
				matrix.c10, matrix.c11,
			};
		}

		Mat2x2 Rotation(Real rad)
		{
			return Mat2x2
			{
				std::cos(rad), -std::sin(rad),
				std::sin(rad),  std::cos(rad)
			};
		}
	}
} // FYC