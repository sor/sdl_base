#pragma once

#include "example_game.hpp"

namespace JanSordid::SDL_Example
{
	struct Entity
	{
		Point position;
	};

	class RoflState final : public MyGameState
	{
		using Base = MyGameState;

	protected:
		struct TileInfo
		{
			Point index;      // Position in the Tileset
			Color color;
			bool  isSolidForPlayers = false;
			bool  isSolidForMinions = false;
			f32   rotation          = 0;
			i8    faction           = -1; // -1 neutral, 0 top-left, 1 bot-right
		};

		static inline HashMap<char, const TileInfo> tileInfos = {
			{ '\0', { { 0,  26 }, { 0,   0,   0,   255 }, false, false, 0, -1 } },
			{ ' ',  { { 0,  26 }, { 0,   0,   0,   255 }, false, false, 0, -1 } },
			{ '0',  { { 0,  26 }, { 0,   0,   0,   255 }, false, false, 0, -1 } },
			{ '1',  { { 0,  26 }, { 0,   0,   0,   255 }, false, false, 0, -1 } },
			{ '2',  { { 0,  26 }, { 0,   0,   0,   255 }, false, false, 0, -1 } },
			{ '3',  { { 0,  26 }, { 0,   0,   0,   255 }, false, false, 0, -1 } },
			{ '4',  { { 0,  26 }, { 0,   0,   0,   255 }, false, false, 0, -1 } },
			{ '5',  { { 0,  26 }, { 0,   0,   0,   255 }, false, false, 0, -1 } },
			{ '6',  { { 0,  26 }, { 0,   0,   0,   255 }, false, false, 0, -1 } },
			{ '7',  { { 0,  26 }, { 0,   0,   0,   255 }, false, false, 0, -1 } },
			{ '8',  { { 0,  26 }, { 0,   0,   0,   255 }, false, false, 0, -1 } },
			{ '9',  { { 0,  26 }, { 0,   0,   0,   255 }, false, false, 0, -1 } },
			{ 'u',  { { 16, 39 }, { 127, 127, 127, 255 }, true,  true, 0, -1 } },
			{ 'U',  { { 17, 39 }, { 127, 127, 127, 255 }, true,  true, 0, -1 } },
			{ 'r',  { { 18, 39 }, { 127, 127, 127, 255 }, true,  true, 0, -1 } },
			{ 'R',  { { 18, 40 }, { 127, 127, 127, 255 }, true,  true, 0, -1 } },
			{ 'd',  { { 18, 41 }, { 127, 127, 127, 255 }, true,  true, 0, -1 } },
			{ 'D',  { { 17, 41 }, { 127, 127, 127, 255 }, true,  true, 0, -1 } },
			{ 'l',  { { 16, 41 }, { 127, 127, 127, 255 }, true,  true, 0, -1 } },
			{ 'L',  { { 16, 40 }, { 127, 127, 127, 255 }, true,  true, 0, -1 } },
			{ 'X',  { { 19, 27 }, { 127, 127, 127, 255 }, true,  true, 0, -1 } },   // Destroyed tower
			{ '-',  { { 1,  29 }, { 156, 107, 48,  255 }, true,  true, 0, -1 } },
			{ '|',  { { 1,  29 }, { 156, 107, 48,  255 }, true,  true, -90, -1 } },
			{ 't',  { { 3,  45 }, { 255, 136, 0,   255 }, true,  true, 0, 0 } },
			{ 'T',  { { 3,  45 }, { 137, 207, 240, 255 }, true,  true, 0, 1 } },
			{ 'f',  { { 14, 36 }, { 255, 136, 0,   255 }, true,  true, 0, 0 } },
			{ 'F',  { { 14, 36 }, { 137, 207, 240, 255 }, true,  true, 0, 1 } },
			{ 'a',  { { 2,  27 }, { 53,  104, 45,  255 }, true,  true, 0, -1 } },
			{ 'A',  { { 3,  27 }, { 53,  104, 45,  255 }, true,  true, 0, -1 } },
			{ 'G',  { { 1,  26 }, { 129, 183, 26,  255 }, false, true, 0, -1 } },
		};

		Owned<Texture>             _tileset;
		Array<Array<Point, 10>, 2> _paths;     // forward for faction 0, backward for faction 1
		// Minions follow path from 0 to 9 or the opposite way around
		DynArray<String> _level = {
			"uUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUr",
			"L        |      GGG                    R",
			"L  f0    |     AA        aa AA         R",
			"L        |    AAAA                     R",
			"L        |     GG a               6    R",
			"L             AA                       R",
			"L------- 1    aa      5      Aa        R",
			"L             aA            AA         R",
			"La     t     AA            GG         aR",
			"LA           Aa           aAA        aAR",
			"La       2  AA            AA  7       aR",
			"LAa        AAa           aA           AR",
			"La         GG            AA     T     aR",
			"L         AA            Aa             R",
			"L        aA      4      aa    8 -------R",
			"L                       AA             R",
			"L    3               a GG     |        R",
			"L                     AAAA    |        R",
			"L         AA aa        AA     |    9F  R",
			"L                    GGG      |        R",
			"lDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDd",
		};

	public:
		// ctor
		using Base::Base;

		void Init() override;
		void Destroy() override;

		bool HandleEvent( const Event & event ) override;
		void Update( u64 frame, u64 totalMSec, f32 deltaT ) override;
		void Render( u64 frame, u64 totalMSec, f32 deltaT ) override;

		Color clearColor() const noexcept override { return Color{ 0, 32, 0, 255 }; }
	};
}
