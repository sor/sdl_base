#pragma once

#include "global.hpp"

#include "sor/sdl_game.hpp"

// This is from JanSordid, the different namespace is just here to show how to use it from not within JanSordid namespace
namespace NotJanSordid::SDL_MiniExample
{
	// For MYLL: "using namespace" should provide access for the inside of the current scope,
	// not for accessing e.g. SDL_MiniExample::Window from the outside (which came from JanSordid::SDL::Window)
	using namespace JanSordid::SDL;
	// Also possible to do this, but then everything SDL needs prefix SDL::
	//namespace SDL = JanSordid::SDL;

	class MiniGame : public Game<> //Game<IGameState,u8>
	{
	public:
		/// Ctors & Dtor
		MiniGame();
	};

	class MiniState : public GameState<MiniGame>
	{
	public:
		/// Ctors & Dtor
		using GameState::GameState; // "Copy" the ctor from GameState

		f32 _colorRolling = 0.0f;

		/// Getters & Setters: non-virtual first, followed by (pure) virtual/override
		[[nodiscard]] Color clearColor() const noexcept override {
			return {
				(u8) (128.0f + sin( _colorRolling * 0.002f ) * 128.0f),
				(u8) (128.0f + sin( _colorRolling * 0.003f ) * 128.0f),
				(u8) (128.0f + sin( _colorRolling * 0.005f ) * 128.0f),
				255
			};
		}

		/// Methods: non-virtual first, followed by (pure) virtual/override
		bool HandleEvent( const Event & event                                   ) override { return false; }
		void Update( u64 framesSinceStart, u64 msSinceStart, float deltaT       ) override { _colorRolling = (f32)msSinceStart; }
		void Render( u64 framesSinceStart, u64 msSinceStart, float deltaTNeeded ) override {}

		//bool isFPSLimited() const noexcept override { return false; }
	};
}
