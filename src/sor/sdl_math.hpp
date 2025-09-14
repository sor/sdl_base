#pragma once

#include "sdl_core.hpp"

namespace JanSordid::SDL
{
	// TODO: Is there really a benefit to have X and Y be template parameters except for the static_asserts?

	// LtR, TtB - Left to Right, Top to Bottom
	template <uint X, uint Y>
	constexpr uint IndexPack( const Point p )
	{
		static_assert( 1 <= X && 1 <= Y, "Tileset dimension sizes need to be 1 or greater" );

		Assert( 0 <= p.x && p.x < X
		     && 0 <= p.y && p.y < Y && "Point x and y must be in valid range" );

		return p.x + p.y * X;
	}

	// LtR, TtB - Left to Right, Top to Bottom
	template <uint X, uint Y>
	constexpr uint IndexPackClamped( const Point p )
	{
		static_assert( 1 <= X && 1 <= Y, "Tileset dimension sizes need to be 1 or greater" );

		Assert( 0 <= p.x
		     && 0 <= p.y && "Point x and y must be in valid range" );

		return (p.x + p.y * X) % (X * Y);
	}

	// LtR, TtB - Left to Right, Top to Bottom
	template <uint X, uint Y>
	constexpr Point IndexUnpack( const uint v )
	{
		static_assert( 1 <= X && 1 <= Y, "Tileset dimension sizes need to be 1 or greater" );

		Assert( v < (X * Y) && "v must be in valid range" );

		return { (int)(v % X), (int)(v / X) };
	}

	// LtR, TtB - Left to Right, Top to Bottom
	template <uint X, uint Y>
	constexpr Point IndexUnpackClamped( const uint v )
	{
		static_assert( 1 <= X && 1 <= Y, "Tileset dimension sizes need to be 1 or greater" );

		return { (int)(v % X), (int)((v / X) % Y) };
	}
}
