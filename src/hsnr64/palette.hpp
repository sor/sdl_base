#pragma once

#include "sor/core.hpp"
#include "sor/sdl_core.hpp"

/*
 * ~ Jan Sordid's ~
 *     ~ Hybrid Sprite Navigator Revolution 64 ~
 *                             ~ Editor / Framework ~
 */
namespace JanSordid::HSNR64
{
	using namespace JanSordid::SDL;

	enum class NamedColorIndex : u8 {
		Transparent = 0,// In reality this is pink or cyan, but when used as index, the framework will treat it as transparent
		Black = 1,      // Not totally black
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
	};

	// Color[64]
	// var Color[NamedColorIndex, 64]
	constexpr const static inline Array<Color, 64> palette = {
		Color{  255,  0,    255, SDL_ALPHA_OPAQUE /*SDL_ALPHA_TRANSPARENT*/ },   // this is the don't-draw-pink
	//	Color{  0,    255,  255, SDL_ALPHA_TRANSPARENT },   // this is the don't-draw-cyan
		Color{  1,    7,    10,  SDL_ALPHA_OPAQUE },    // Black
		Color{  19,   16,   20,  SDL_ALPHA_OPAQUE },
		Color{  33,   34,   57,  SDL_ALPHA_OPAQUE },
		Color{  49,   57,   68,  SDL_ALPHA_OPAQUE },
		Color{  73,   84,   101, SDL_ALPHA_OPAQUE },
		Color{  106,  116,  147, SDL_ALPHA_OPAQUE },    // Grey 50%
		Color{  138,  148,  179, SDL_ALPHA_OPAQUE },    // Grey 63%
		Color{  178,  185,  212, SDL_ALPHA_OPAQUE },    // Grey 77%
		Color{  217,  226,  242, SDL_ALPHA_OPAQUE },    // Grey 90%
		Color{  255,  255,  255, SDL_ALPHA_OPAQUE },    // White
		Color{  62,   21,   39,  SDL_ALPHA_OPAQUE },
		Color{  124,  0,    42,  SDL_ALPHA_OPAQUE },
		Color{  193,  0,    18,  SDL_ALPHA_OPAQUE },
		Color{  236,  37,   2,   SDL_ALPHA_OPAQUE },    // Red
		Color{  255,  94,   1,   SDL_ALPHA_OPAQUE },
		Color{  255,  158,  0,   SDL_ALPHA_OPAQUE },    // Orange
		Color{  255,  212,  0,   SDL_ALPHA_OPAQUE },
		Color{  254,  254,  0,   SDL_ALPHA_OPAQUE },    // Yellow
		Color{  212,  244,  26,  SDL_ALPHA_OPAQUE },
		Color{  143,  223,  0,   SDL_ALPHA_OPAQUE },    // Lime
		Color{  57,   196,  0,   SDL_ALPHA_OPAQUE },    // Malach
		Color{  1,    163,  0,   SDL_ALPHA_OPAQUE },    // Green
		Color{  0,    125,  46,  SDL_ALPHA_OPAQUE },    // ForestGreen
		Color{  4,    78,   48,  SDL_ALPHA_OPAQUE },    // DarkForestGreen
		Color{  20,   36,   36,  SDL_ALPHA_OPAQUE },    // Aztec            ///
		Color{  23,   28,   64,  SDL_ALPHA_OPAQUE },
		Color{  5,    54,   114, SDL_ALPHA_OPAQUE },
		Color{  14,   90,   218, SDL_ALPHA_OPAQUE },
		Color{  9,    162,  234, SDL_ALPHA_OPAQUE },
		Color{  17,   218,  202, SDL_ALPHA_OPAQUE },
		Color{  147,  255,  218, SDL_ALPHA_OPAQUE },
		Color{  196,  249,  221, SDL_ALPHA_OPAQUE },
		Color{  129,  224,  183, SDL_ALPHA_OPAQUE },
		Color{  71,   178,  138, SDL_ALPHA_OPAQUE },
		Color{  6,    134,  97,  SDL_ALPHA_OPAQUE },
		Color{  5,    105,  72,  SDL_ALPHA_OPAQUE },
		Color{  54,   127,  135, SDL_ALPHA_OPAQUE },
		Color{  68,   143,  200, SDL_ALPHA_OPAQUE },
		Color{  128,  155,  241, SDL_ALPHA_OPAQUE },
		Color{  180,  190,  255, SDL_ALPHA_OPAQUE },
		Color{  253,  244,  177, SDL_ALPHA_OPAQUE },
		Color{  245,  210,  159, SDL_ALPHA_OPAQUE },
		Color{  255,  156,  142, SDL_ALPHA_OPAQUE },
		Color{  246,  92,   103, SDL_ALPHA_OPAQUE },
		Color{  199,  58,   161, SDL_ALPHA_OPAQUE },
		Color{  131,  47,   139, SDL_ALPHA_OPAQUE },
		Color{  64,   50,   89,  SDL_ALPHA_OPAQUE },
		Color{  33,   29,   23,  SDL_ALPHA_OPAQUE },
		Color{  53,   42,   39,  SDL_ALPHA_OPAQUE },
		Color{  119,  61,   52,  SDL_ALPHA_OPAQUE },
		Color{  196,  111,  46,  SDL_ALPHA_OPAQUE },
		Color{  228,  161,  70,  SDL_ALPHA_OPAQUE },
		Color{  243,  178,  157, SDL_ALPHA_OPAQUE },
		Color{  194,  114,  97,  SDL_ALPHA_OPAQUE },
		Color{  150,  78,   78,  SDL_ALPHA_OPAQUE },
		Color{  98,   45,   56,  SDL_ALPHA_OPAQUE },
		Color{  69,   42,   52,  SDL_ALPHA_OPAQUE },
		Color{  67,   56,   52,  SDL_ALPHA_OPAQUE },
		Color{  93,   78,   64,  SDL_ALPHA_OPAQUE },
		Color{  126,  102,  78,  SDL_ALPHA_OPAQUE },
		Color{  164,  133,  85,  SDL_ALPHA_OPAQUE },
		Color{  200,  175,  129, SDL_ALPHA_OPAQUE },
		Color{  253,  214,  177, SDL_ALPHA_OPAQUE },
	};

	constexpr const Color & Palette( NamedColorIndex index )
	{
		Assert((u8)index < 64);
		return palette[(u8)index];
	}

	constexpr const Color & Palette( u8 index )
	{
		Assert((u8)index < 64);
		return palette[index];
	}


	enum class NColor : u32
	{
		Transparent = ToU32( palette[0] ),
	};
}
