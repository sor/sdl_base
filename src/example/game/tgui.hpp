#pragma once

#include "example_game.hpp"

#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SDL-Renderer.hpp>

namespace JanSordid::SDL_Example
{
	class TGUIState final : public MyGameState
	{
		using Base = MyGameState;

	protected:
		tgui::Gui          _gui;
		tgui::Button::Ptr  _but;
		tgui::EditBox::Ptr _edit;

	public:
		/// Ctors & Dtor
		using Base::Base;

		void Init() override;
		void Destroy() override;

		bool Input( const SDL_Event & event ) override;
		void Update( u64 framesSinceStart, Duration timeSinceStart, f32 deltaT       ) override;
		void Render( u64 framesSinceStart, Duration timeSinceStart, f32 deltaTNeeded ) override;

		SDL_Color clearColor() const noexcept override{ return SDL_Color{191,191,191,255}; }
	};
}
