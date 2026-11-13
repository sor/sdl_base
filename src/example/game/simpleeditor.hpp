#pragma once

#include "example_game.hpp"

namespace JanSordid::SDL_Example
{
	class EditorState : public MyGameState
	{
		using Base = MyGameState;

		constexpr static std::array<SDL_Color,8> BaseColors = {
			SDL_Color{ 0,   0,   0,   255 },
			SDL_Color{ 255, 0,   0,   255 },
			SDL_Color{ 0,   255, 0,   255 },
			SDL_Color{ 255, 255, 0,   255 },
			SDL_Color{ 0,   0,   255, 255 },
			SDL_Color{ 255, 0,   255, 255 },
			SDL_Color{ 0,   255, 255, 255 },
			SDL_Color{ 255, 255, 255, 255 },
		};

		Owned<TTF_Font>    _font;
		Owned<SDL_Texture> _tileSet;

		using WorldState = std::array<std::array<int, 40>, 20>; // TODO: These sizes here also need to be manually configured, expose dynamic sizing via ImGUI

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

		SDL_Point  _tileSetSize;
		SDL_Point  _tileSize;
		SDL_Point  _tileCount;
		SDL_FPoint _camera;
		SDL_Point  _pickedIdx    = SDL_Point{ 0, 0 };
		i32        _mapScale     = 2;
		i32        _paletteScale = 2; // Is set to _game.scalingFactor() in Init()
		bool       _isPainting   = false;
		bool       _isPanning    = false;
		bool       _showGrid     = false;

		constexpr static Duration UpdateDeltaTime = 100ms;

		Duration _nextUpdateTime = {};

	public:
		// ctor
		using Base::Base;

		void Init() override;
		void Destroy() override;

		bool Input( const SDL_Event & event ) override;
		void Update( u64 framesSinceStart, Duration timeSinceStart, f32 deltaT       ) override;
		void Render( u64 framesSinceStart, Duration timeSinceStart, f32 deltaTNeeded ) override;

		constexpr SDL_Color clearColor() const noexcept override { return HSNR64::Palette( HSNR64::NamedColor::Grey40 ); }
	};
}
