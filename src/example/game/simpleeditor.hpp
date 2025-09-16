#pragma once

#include "example_game.hpp"

namespace JanSordid::SDL_Example
{
	class EditorState : public MyGameState
	{
		using Base = MyGameState;

		constexpr const static Array<Color,8> BaseColors = {
			Color{ 0,   0,   0,   255 },
			Color{ 255, 0,   0,   255 },
			Color{ 0,   255, 0,   255 },
			Color{ 255, 255, 0,   255 },
			Color{ 0,   0,   255, 255 },
			Color{ 255, 0,   255, 255 },
			Color{ 0,   255, 255, 255 },
			Color{ 255, 255, 255, 255 },
		};

		Owned<Font>    _font;
		Owned<Texture> _tileSet;

		using WorldState = Array<Array<int, 40>, 20>; // TODO: These sizes here also need to be manually configured, expose dynamic sizing via ImGUI

		const bool _doGenerateEmptyMap = true; // Set this to false if you already load a map
		WorldState _worldState1;
		WorldState _worldState2;

		/**
		 * This is how it looks if you already preload the map:
		 *
			const bool doGenerateEmptyMap = false;
			WorldState worldState1 =
			{{
				 { 55,64,64,... },
				 ...
				 { 55,55,55,... }
			}};
			// worldState2 does not need to be loaded
		**/

		WorldState
			* _currState = &_worldState1,
			* _nextState = &_worldState2;

		Point  _tileSetSize;
		Point  _tileSize;
		Point  _tileCount;
		FPoint _camera;
		Point  _pickedIdx    = Point{ 0, 0 };
		i32    _mapScale     = 2;
		i32    _paletteScale = 2; // Is set to _game.scalingFactor() in Init()
		bool   _isPainting   = false;
		bool   _isPanning    = false;
		bool   _showGrid     = false;

		constexpr const static u64 UpdateDeltaTimeMS = 100;
		u64   _nextUpdateTimeMS = 0;

	public:
		// ctor
		using Base::Base;

		void Init() override;
		void Destroy() override;

		bool HandleEvent( const Event & event ) override;
		void Update( u64 framesSinceStart, u64 msSinceStart, f32 deltaT ) override;
		void Render( u64 framesSinceStart, u64 msSinceStart, f32 deltaTNeeded ) override;

		constexpr Color clearColor() const noexcept override { return HSNR64::Palette( HSNR64::NamedColor::Grey40 ); }
	};
}
