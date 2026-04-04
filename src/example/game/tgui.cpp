#include "tgui.hpp"

#include "example_game.hpp"

namespace JanSordid::SDL_Example
{
	void TGUIState::Init()
	{
		Base::Init();

		_gui.setWindow( _game.window(), _game.renderer() );
		_gui.loadWidgetsFromFile( BasePathGUI "form.tgui" );

		const f32 scale = _game.scalingFactor();
		tgui::getBackend()->setFontScale( scale );
		//tgui::getBackend()->getFontBackend();
		_gui.setRelativeView({ 0, 0, 1 / scale, 1 / scale });
		// The line above and below do the same
		//_gui.setAbsoluteView({ 0, 0, 640, 360 });

		_edit = _gui.get<tgui::EditBox>("EditBox1");
		_edit->getRenderer()->setBackgroundColor( tgui::Color::Red );

		_but = _gui.get<tgui::Button>("Button1");
		_but->onPress( [&]{ print( stdout, "Dynamischer Text {} {}\n", rand(), _edit->getText().toStdString() ); } );
		_but->onPress( [&]{ _but->setText( format( "Dynamischer Text {} {}", rand(), _edit->getText().toStdString() ) ); } );

//		_but = tgui::Button::create();
//		_but->setText( "Hallo Welt" );
//		_but->setPosition( 20, 320 );
//		_but->setSize( 100, 100 );
//		_but->setTextSize( 18 );
//		_gui.add( _but );

		SDL_SetRenderDrawBlendMode( renderer(), SDL_BLENDMODE_BLEND );
	}

	void TGUIState::Destroy()
	{
		Base::Destroy();
	}

	bool TGUIState::Input( const Event & event )
	{
		return _gui.handleEvent( event );
	}

	void TGUIState::Update( const u64 framesSinceStart, const Duration timeSinceStart, const f32 deltaT ) {}

	void TGUIState::Render( const u64 framesSinceStart, const Duration timeSinceStart, const f32 deltaTNeeded )
	{
		SDL_SetRenderDrawColor( renderer(), 255, 0, 0, 255 );
		FRect rect = { (f32)(framesSinceStart % 500), 270, 100, 100 };
		SDL_RenderFillRect( renderer(), &rect );

		_gui.draw();

		SDL_SetRenderDrawColor( renderer(), 0, 255, 0, 191 );
		FRect rect2 = { (f32)(framesSinceStart % 500)+50, 290, 100, 100 };
		SDL_RenderFillRect( renderer(), &rect2 );
	}
}
