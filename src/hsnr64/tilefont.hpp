#pragma once

#include "sor/core.hpp"
#include "sor/sdl_core.hpp"
#include "sor/sdl_shapeops.hpp"

#include "offset.hpp"

//#pragma clang diagnostic push
//#pragma ide diagnostic ignored "cppcoreguidelines-avoid-magic-numbers"

namespace JanSordid::HSNR64
{
	using namespace JanSordid::SDL;

	struct CharMetric
	{
		u8  width    = 16;
		i8  xOff     = 0;
		i8  yOff     = 0;
		i8  left [8] = { 0, 0, 0, 0, 0, 0, 0, 0 };  // TODO: add 3 values for the belly, example: a (left lower), e (right high), d (left full)
		i8  right[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
		u8  ligLevel = 255;    // higher => less likely; both chars will be multiplied together to form the final number
		u16 mergeL   = 0b0000'0000'0000'0000;  // pointy / round
		u16 mergeR   = 0b0000'0000'0000'0000;  // round  / pointy
	};

	// TODO: Move this sparsely stored and separate from CharMetric
	struct CharLig
	{
		// leave "level" in CharMetric, so we already know from there if this here exists
		u8  level   = 255;                   // higher => less likely; both chars will be multiplied together to form the final number
		u16 merge_l = 0b0000'0000'0000'0000; // pointy / round
		u16 merge_r = 0b0000'0000'0000'0000; // round  / pointy
	};

	// TODO: The typesetting algo should just output an array of CharChains, which can be used regardless of backend, with no added linebreaks yet
	struct CharChain
	{
		u32 uc; // "unicode" codepoint, not really, but the index into the tileset
		u8  width;
		u8  left;
		i8  xoff;
		i8  yoff;
	};

	class TileFont
	{
		/*
		 * Fonts can merge at: top mid bot low
		 * One or Both must be Pointy to allow the connection
		 * A merge means that the score at this spot is 2 better
		 */

		/*** TODO:
		 * Handle U+25XX & U+1FBXX
		 * Shift all the characters in the sheet back to the middle, to enable usage of simple monospaced text and add offsets here to still enable proper kerning.
		 * 	 Done (partially or completely?)
		 * Move the layouting towards using CharChain, which would also work outside of SDL etc.
		 * Support specials like Tab, Shy, NBSP, etc.
		 * Fill all the values of CharMetric table properly
		 * https://www.utf8-chartable.de/
		 ***/

		static const Array<const CharMetric, 256> CharMetrics;
		static const HashMap<u64, const i8>       SpecialPairs; // TODO: To differentiate between ligatures and just fixing the look, add a second map

		static constexpr u64 combineChars( char32_t last, char32_t curr )
		{
			return (((u64) last) << 32) | (u64) curr;
		}

		static constexpr int numBytesInUTF8( const char c )
		{
			     if( (c & 0b10000000) == 0          ) return 1;
			else if( (c & 0b11100000) == 0b11000000 ) return 2;
			else if( (c & 0b11110000) == 0b11100000 ) return 3;
			else if( (c & 0b11111000) == 0b11110000 ) return 4;
			else return -1; // Invalid UTF-8 sequence
		}

		Owned<Texture> _tiles;
		Renderer     * _renderer;

	public:
		TileFont( Renderer * renderer );

		void Render(
			const String & text,
			const FRect  & dimension,
			const Color  & color,
			const Color  & outlineColor = { 0, 0, 0, SDL_ALPHA_OPAQUE } );

	private:
		using RenderFunc = void ( Renderer * renderer, Texture * texture, const FRect & src, const FRect & dst );

		// These are function declares ;)
		static RenderFunc RenderOutline;
		static RenderFunc RenderCenter;

		void RenderSingle(
			const String & text,
			const FRect  & dimension,
			const u8       ligatureLevel,
			const u8       spacing,
			const Point  & tilesize,
			RenderFunc     renderFunc );
	};
}

//#pragma clang diagnostic pop
