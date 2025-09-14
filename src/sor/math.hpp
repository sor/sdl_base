#pragma once

#include <bit>
#include "core.hpp"

namespace JanSordid::Core
{
	template <typename T>
	constexpr bool IsPowerOfTwo( T x )
	{
		// TODO: Should be replaced by std::has_single_bit
		//return std::has_single_bit( x );

		// v == 2`x
		static_assert( std::is_integral_v<T>, "Integral type required" );
		return x && ((x & (x - 1)) == 0);
	}

	template <int N, typename T>
	constexpr T RoundUpMultiple( T x )
	{
		static_assert( N > 0, "The multiple N must be greater than 0" );

		if constexpr( std::is_signed_v<T> )
			Assert( x >= 0 );

		if constexpr( IsPowerOfTwo( N ) )
			// TODO: Should be partially replaced by std::bit_ceil
			return (x + (N - 1)) & -N;
		else
			return ((x + (N - 1)) / N) * N;
	}
}
