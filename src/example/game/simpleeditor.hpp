#pragma once

#include "example_game.hpp"

namespace JanSordid::SDL_Example
{
	class EditorState : public MyGameState
	{
		using Base = MyGameState;

		constexpr const static int   TileSize  = 32;        // Needs to be manually configured (or the TileCount)
		constexpr const static Point TileCount = { 9, 11 }; // TODO: This could be automatically determined

		constexpr const static Color BaseColors[] = {
			Color( 0,   0,   0,   255 ),
			Color( 255, 0,   0,   255 ),
			Color( 0,   255, 0,   255 ),
			Color( 255, 255, 0,   255 ),
			Color( 0,   0,   255, 255 ),
			Color( 255, 0,   255, 255 ),
			Color( 0,   255, 255, 255 ),
			Color( 255, 255, 255, 255 ),
		};

		Owned<Font>    font;
		Owned<Texture> tileSet;

		using WorldState = std::array<std::array<int, 40>, 20>; // These sizes here also need to be manually configured

		const bool doGenerateEmptyMap = true; // Set this to false if you already load a map
		WorldState worldState1;
		WorldState worldState2;

		/**
		 * This is how it looks if you already preload the map:
		 *
			const bool doGenerateEmptyMap = false;
			WorldState worldState1 =
			{{
				 { 55,55,55,... },
				 { 55,64,64,... },
				 ...
				 { 55,55,55,... }
			}};
			// worldState2 does not need to be loaded
		**/

		WorldState
			* currState = &worldState1,
			* nextState = &worldState2;

		Point camera;
		Point tileSetSize;
		Point pickedIdx  = Point{ 0, 0 };
		bool  isPainting = false;
		bool  isPanning  = false;
		bool  showGrid   = false;

	public:
		// ctor
		using Base::Base;

		void Init() override;
		void Destroy() override;

		bool HandleEvent( const Event & event ) override;
		void Update( u64 framesSinceStart, u64 msSinceStart, f32 deltaT ) override;
		void Render( u64 framesSinceStart, u64 msSinceStart, f32 deltaTNeeded ) override;

		constexpr Color clearColor() const noexcept override { return Color{ 90, 90, 90, SDL_ALPHA_OPAQUE }; }
	};
}
