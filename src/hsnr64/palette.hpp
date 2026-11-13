#pragma once

#include "sor/core.hpp"
#include "sor/sdl_core.hpp"

namespace JanSordid::HSNR64
{
	using namespace JanSordid::SDL;

	enum class NamedColor : u8
	{
		Transparent = 0,// In reality this is pink or cyan, but when used as index, the framework will treat it as transparent
		Black = 1,      // Not totally black (should it be?)
		Grey10,         // Charcoal,
		Grey20,         // MidnightBlue,
		Grey30,         // SteelBlue,
		Grey40,         // SlateGray,
		Grey50,         // SlateBlue,
		Grey62,         // BlueGray,       // BaliHai
		Grey75,         // LightSteelBlue, // Casper
		Grey88,         // LightSlateGray, // LinkWater
		White = 10,
		Red25, RedWine = Red25,
		Red50,          // Crimson, Siren
		Red75,          // Monza
		Red,            // Scarlet, Vermilion
		OrangeRed,      // ? BlazeOrange
		Orange,         // OrangePeel, Goldenrod
		Gold,           // Gold
		Yellow,
		LimeYellow,
		Lime, LimeGreen = Lime,
		MalachiteGreen, // Harlequin
		Green,
		ForestGreen,
		DarkForest,
		Teal = 37,

		// TODO: Continue the named color list above when feeling creative

		/// To use Alpha0 use the SDL_Color named "Transparent" at Index 0
		Alpha25  = 0b0000'0000, // This is kind of used by default... maybe Alpha100 would make more sense by default?
		Alpha50  = 0b0100'0000,
		Alpha75  = 0b1000'0000,
		Alpha100 = 0b1100'0000,
		Opaque   = Alpha100,

		ColorMask = 0b0011'1111,
		AlphaMask = 0b1100'0000,
		AlphaFill = 0b0011'1111,
	};

	// SDL_Color[64]
	// Idea for Myll, typed indexer: var SDL_Color[NamedColor, 64]
	constexpr static inline std::array<SDL_Color, 64> palette = {
		SDL_Color{  255,  0,    255, SDL_ALPHA_OPAQUE /*SDL_ALPHA_TRANSPARENT*/ },   // this is the don't-draw-pink
	//	SDL_Color{  0,    255,  255, SDL_ALPHA_TRANSPARENT },   // this is the don't-draw-cyan
		SDL_Color{  1,    7,    10,  SDL_ALPHA_OPAQUE },    // Black
		SDL_Color{  19,   16,   20,  SDL_ALPHA_OPAQUE },
		SDL_Color{  33,   34,   57,  SDL_ALPHA_OPAQUE },
		SDL_Color{  49,   57,   68,  SDL_ALPHA_OPAQUE },
		SDL_Color{  73,   84,   101, SDL_ALPHA_OPAQUE },
		SDL_Color{  106,  116,  147, SDL_ALPHA_OPAQUE },    // Grey 50%
		SDL_Color{  138,  148,  179, SDL_ALPHA_OPAQUE },    // Grey 63%
		SDL_Color{  178,  185,  212, SDL_ALPHA_OPAQUE },    // Grey 77%
		SDL_Color{  217,  226,  242, SDL_ALPHA_OPAQUE },    // Grey 90%
		SDL_Color{  255,  255,  255, SDL_ALPHA_OPAQUE },    // White
		SDL_Color{  62,   21,   39,  SDL_ALPHA_OPAQUE },
		SDL_Color{  124,  0,    42,  SDL_ALPHA_OPAQUE },
		SDL_Color{  193,  0,    18,  SDL_ALPHA_OPAQUE },
		SDL_Color{  236,  37,   2,   SDL_ALPHA_OPAQUE },    // Red
		SDL_Color{  255,  94,   1,   SDL_ALPHA_OPAQUE },
		SDL_Color{  255,  158,  0,   SDL_ALPHA_OPAQUE },    // Orange
		SDL_Color{  255,  212,  0,   SDL_ALPHA_OPAQUE },
		SDL_Color{  254,  254,  0,   SDL_ALPHA_OPAQUE },    // Yellow
		SDL_Color{  212,  244,  26,  SDL_ALPHA_OPAQUE },
		SDL_Color{  143,  223,  0,   SDL_ALPHA_OPAQUE },    // Lime
		SDL_Color{  57,   196,  0,   SDL_ALPHA_OPAQUE },    // Malach
		SDL_Color{  1,    163,  0,   SDL_ALPHA_OPAQUE },    // Green
		SDL_Color{  0,    125,  46,  SDL_ALPHA_OPAQUE },    // ForestGreen
		SDL_Color{  4,    78,   48,  SDL_ALPHA_OPAQUE },    // DarkForestGreen
		SDL_Color{  20,   36,   36,  SDL_ALPHA_OPAQUE },    // Aztec            ///
		SDL_Color{  23,   28,   64,  SDL_ALPHA_OPAQUE },
		SDL_Color{  5,    54,   114, SDL_ALPHA_OPAQUE },
		SDL_Color{  14,   90,   218, SDL_ALPHA_OPAQUE },
		SDL_Color{  9,    162,  234, SDL_ALPHA_OPAQUE },
		SDL_Color{  17,   218,  202, SDL_ALPHA_OPAQUE },
		SDL_Color{  147,  255,  218, SDL_ALPHA_OPAQUE },
		SDL_Color{  196,  249,  221, SDL_ALPHA_OPAQUE },
		SDL_Color{  129,  224,  183, SDL_ALPHA_OPAQUE },
		SDL_Color{  71,   178,  138, SDL_ALPHA_OPAQUE },
		SDL_Color{  6,    134,  97,  SDL_ALPHA_OPAQUE },
		SDL_Color{  5,    105,  72,  SDL_ALPHA_OPAQUE },
		SDL_Color{  54,   127,  135, SDL_ALPHA_OPAQUE },
		SDL_Color{  68,   143,  200, SDL_ALPHA_OPAQUE },
		SDL_Color{  128,  155,  241, SDL_ALPHA_OPAQUE },
		SDL_Color{  180,  190,  255, SDL_ALPHA_OPAQUE },
		SDL_Color{  253,  244,  177, SDL_ALPHA_OPAQUE },
		SDL_Color{  245,  210,  159, SDL_ALPHA_OPAQUE },
		SDL_Color{  255,  156,  142, SDL_ALPHA_OPAQUE },
		SDL_Color{  246,  92,   103, SDL_ALPHA_OPAQUE },
		SDL_Color{  199,  58,   161, SDL_ALPHA_OPAQUE },
		SDL_Color{  131,  47,   139, SDL_ALPHA_OPAQUE },
		SDL_Color{  64,   50,   89,  SDL_ALPHA_OPAQUE },
		SDL_Color{  33,   29,   23,  SDL_ALPHA_OPAQUE },
		SDL_Color{  53,   42,   39,  SDL_ALPHA_OPAQUE },
		SDL_Color{  119,  61,   52,  SDL_ALPHA_OPAQUE },
		SDL_Color{  196,  111,  46,  SDL_ALPHA_OPAQUE },
		SDL_Color{  228,  161,  70,  SDL_ALPHA_OPAQUE },
		SDL_Color{  243,  178,  157, SDL_ALPHA_OPAQUE },
		SDL_Color{  194,  114,  97,  SDL_ALPHA_OPAQUE },
		SDL_Color{  150,  78,   78,  SDL_ALPHA_OPAQUE },
		SDL_Color{  98,   45,   56,  SDL_ALPHA_OPAQUE },
		SDL_Color{  69,   42,   52,  SDL_ALPHA_OPAQUE },
		SDL_Color{  67,   56,   52,  SDL_ALPHA_OPAQUE },
		SDL_Color{  93,   78,   64,  SDL_ALPHA_OPAQUE },
		SDL_Color{  126,  102,  78,  SDL_ALPHA_OPAQUE },
		SDL_Color{  164,  133,  85,  SDL_ALPHA_OPAQUE },
		SDL_Color{  200,  175,  129, SDL_ALPHA_OPAQUE },
		SDL_Color{  253,  214,  177, SDL_ALPHA_OPAQUE },
	};

	static constexpr bool ColorNeedsLinebreak( int i ) noexcept
	{
		return i == 10 || i == 25 || i == 40 || i == 52;
	};

	constexpr const SDL_Color & Palette( NamedColor index )
	{
		Assert((u8)index < 64);
		return palette[(u8)index];
	}

	constexpr const SDL_Color & Palette( u8 index )
	{
		Assert((u8)index < 64);
		return palette[index];
	}


	enum class NColor : u32
	{
		Transparent = ToU32( palette[0] ),
	};
}
