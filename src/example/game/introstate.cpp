#include "example_game.hpp"

#include <hsnr64/offset.hpp>
#include <hsnr64/palette.hpp>
//#include <tilefont.h>

namespace JanSordid::SDL_Example
{
	void IntroState::Init()
	{
		Base::Init();

		if( !_font )
		{
			_font = TTF_OpenFont( BasePath "asset/font/RobotoSlab-Bold.ttf", 8.0f * _game.scalingFactor() );
			TTF_SetFontHinting( _font, TTF_HINTING_LIGHT );
			if( !_font )
				print( stderr, "TTF_OpenFont failed: {}\n", TTF_GetError() );
		}

		if( !_image )
		{
			_image = IMG_LoadTexture( renderer(), BasePath "asset/graphic/background.png" );
			if( !_image )
				print( stderr, "IMG_LoadTexture failed: {}\n", IMG_GetError() );
		}

		if( !_music )
		{
			_music = Mix_LoadMUS( BasePath "asset/music/severance.ogg" );
			if( !_music )
				print( stderr, "Mix_LoadMUS failed: {}\n", Mix_GetError() );
		}

		if( !_sound )
		{
			_sound = Mix_LoadWAV( BasePath "asset/sound/pew.wav" );
			if( !_sound )
				print( stderr, "Mix_LoadWAV failed: {}\n", Mix_GetError() );
		}
	}

	void IntroState::Enter( bool stacking )
	{
		Base::Enter( stacking );

		if( Mix_GetMusicPosition( _music ) <= 0.001 )
			Mix_PlayMusic( _music, -1 );
		else if( Mix_PausedMusic() )
			Mix_ResumeMusic();
	}

	void IntroState::Exit( bool stacking )
	{
		if( !Mix_PausedMusic() )
			Mix_PauseMusic();

		Base::Exit( stacking );
	}

	void IntroState::Destroy()
	{
		// Keep everything loaded/allocated is also an option
		/*
		TTF_CloseFont( font );
		SDL_DestroyTexture( image );
		SDL_DestroyTexture( _blendedText );
		font = nullptr;
		image = nullptr;
		_blendedText = nullptr;
		*/
		Base::Destroy();
	}

	bool IntroState::HandleEvent( const Event & event )
	{
		switch( event.type )
		{
			case SDL_KEYDOWN:
			{
				const Keysym & what_key = event.key.keysym;

				if( what_key.scancode == SDL_SCANCODE_F1 && event.key.repeat == 0 )
				{
					if( Mix_PausedMusic() )
						Mix_ResumeMusic();
					else
						Mix_PauseMusic();
				}
				else if( what_key.scancode == SDL_SCANCODE_F2 && event.key.repeat == 0 )
				{
					if( Mix_VolumeMusic( -1 ) == MIX_MAX_VOLUME )
						Mix_VolumeMusic( 0 );
					else
						Mix_VolumeMusic( MIX_MAX_VOLUME );
				}
				else if( what_key.scancode == SDL_SCANCODE_F3 && event.key.repeat == 0 )
				{
					Mix_PlayChannel( -1, _sound, 0 );
				}
				else if( what_key.scancode == SDL_SCANCODE_F4 && event.key.repeat == 0 )
				{
					_textmode = (_textmode + 1) % 2;
				}
				else if( what_key.scancode == SDL_SCANCODE_F9 )
				{
					// crash/shutdown, since State #6 does not exist
					_game.ReplaceState( MyGS::Invalid );
				}
				else if( what_key.scancode == SDL_SCANCODE_ESCAPE )
				{
					_game.ReplaceState( MyGS::Intro );
				}
				else
				{
					return false; // Not handled
				}

				return true; // Confusing control flow: Handled by all but the else case

				break;
			}

			case SDL_MOUSEBUTTONDOWN:
				//game.SetNextState( 1 );
				break;

			default: break;
		}

		return false;
	}

	void IntroState::Update( const u64 frame, const u64 totalMSec, const f32 deltaT )
	{
	}

	void IntroState::Render( const u64 frame, const u64 totalMSec, const f32 deltaTNeeded )
	{
		Point windowSize;
		SDL_GetWindowSize( window(), &windowSize.x, &windowSize.y );

		{
			const Rect dst_rect = { 0, 0, windowSize.x, windowSize.y };
			SDL_RenderCopy( renderer(), _image, EntireRect, &dst_rect /* same result as EntireRect */ );
		}

		// Poor persons benchmark
		//for (uint x = 0; x < 100; ++x)
		{
			constexpr const char * text =
				"                                          --== Introscreen of my Super Mega Gamey Game 3000 ==--\n\n"
				"Dies ist ein Typoblindtext. An ihm kann man sehen, ob alle Buchstaben da sind und wie sie aussehen. "
				"Manchmal benutzt man Worte wie Hamburgefonts, Rafgenduks oder Handgloves, um Schriften zu testen. "
				"Manchmal Sätze, die alle Buchstaben des Alphabets enthalten - man nennt diese Sätze »Pangrams«. "
				"Sehr bekannt ist dieser: The quick brown fox jumps over the lazy old dog. "
				"Oft werden in Typoblindtexte auch fremdsprachige Satzteile eingebaut (AVAIL® and Wefox™ are testing aussi la Kerning), um die Wirkung in anderen Sprachen zu testen. "
				"In Lateinisch sieht zum Beispiel fast jede Schrift gut aus. Quod erat demonstrandum. "
				"Seit 1975 fehlen in den meisten Testtexten die Zahlen, weswegen nach TypoGb. §.204 ab dem Jahr 2034 Zahlen in 86 der Texte zur Pflicht werden. "
				"Nichteinhaltung wird mit bis zu 245 € oder 368 $ bestraft. Genauso wichtig in sind mittlerweile auch Âçcèñtë, die in neueren Schriften aber fast immer enthalten sind. "
				"Ein wichtiges aber schwierig zu integrierendes Feld sind OpenType-Funktionalitäten. "
				"Je nach Software und Voreinstellungen können eingebaute Kapitälchen, Kerning oder Ligaturen (sehr pfiffig) nicht richtig dargestellt werden."
				"\n\nRoyality free music by Karl Casey @ White Bat Audio"
				"\n  - Press [F1] to (un)pause music."
				"\n  - Press [F2] to (un)mute music."
				"\nSource: https://www.youtube.com/watch?v=aFITtvK64B4"
				"\n\nPress any key to continue!"
				"\n\n!\"#$%&'()*+,-./0123456789:;<=>? c*8 uzu"
				"\n@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
				"\n`abcdefghijklmnopqrstuvwxyz{|}~"
				"\n\u00A0!¡c¢E£¤Y¥¦S§¨©ª«¬\u00AD®¯°C±n²³´uµ¶·¸¹º»¼½¾?¿"
				"\nAÀÁÂÃÄ¨ÅÆCÇEÈÉÊËÌÍÎÏIDÐÑNOÒÓÔÕÖ×OØUÙÚÛÜYÝÞß"
				"\naàáâãäåæcçeèéêëiìíîïdðnñoòóôõö÷oøuùúûüyýþyÿ"
				"\ninjektion! enjoy major Heij project Stobject farbe kd aes real tw";

			const Color outlineColor = _isDarkOutline
				? Color {   0,   0,   0, SDL_ALPHA_OPAQUE }
				: Color { 255, 255, 255, SDL_ALPHA_OPAQUE };

			// Draw the text on top
			// _textmode == 0 is True Type (currently cached)
			// _textmode == 1 is Bitmap Font (currently uncached)
			if( _textmode == 0 )
			{
				// Comment out to disable the cache. Uses 5ms without / 20 ms with harfbuzz
				if( _blendedText == nullptr )
				{
					if( _blendedText != nullptr )
						SDL_DestroyTexture( _blendedText );

					Surface * surf = TTF_RenderUTF8_Blended_Wrapped( _font, text, white, windowSize.x - _p.x );
					_blendedText = SDL_CreateTextureFromSurface( renderer(), surf );
					SDL_FreeSurface( surf );

					u32 fmt;
					int access;
					SDL_QueryTexture( _blendedText, &fmt, &access, &_blendedTextSize.x, &_blendedTextSize.y );
				}

				SDL_SetTextureColorMod( _blendedText, outlineColor.r, outlineColor.g, outlineColor.b );

				for( const Point & offset : HSNR64::ShadowOffset::Rhombus )
				{
					const Rect dst_rect = Rect {_p.x, _p.y, _blendedTextSize.x, _blendedTextSize.y } + offset;
					SDL_RenderCopy( renderer(), _blendedText, EntireRect, &dst_rect );
				}

				const Color & color = HSNR64::Palette( _colorIndex );
				SDL_SetTextureColorMod( _blendedText, color.r, color.g, color.b );
				const Rect dst_rect = { _p.x, _p.y, _blendedTextSize.x, _blendedTextSize.y };
				SDL_RenderCopy( renderer(), _blendedText, EntireRect, &dst_rect );
			}
			else
			{
				/*
				TileFont::TF_Init( renderer() );

				Rect dimension { _p.x, _p.y, winSize.x - (32 + _p.x), 9999 };
				TileFont::TF_Render( renderer(), text, dimension, HSNR64::Palette( _colorIndex ), outlineColor );
				*/
			}
		}
	}

#ifdef IMGUI

	void IntroState::RenderUI( const u64 frame, const u64 totalMSec, const f32 deltaTNeeded )
	{
		// ImGui Demo
		static bool show_demo_window = true;
		ImGui::SetNextWindowCollapsed( true, ImGuiCond_Once );
		ImGui::ShowDemoWindow( &show_demo_window );

		Point windowSize;
		SDL_GetWindowSize( window(), &windowSize.x, &windowSize.y );

		static bool autoUpdate = false;
		static bool drawColorNumber = false;
		//ImGuiIO & io = ImGui::GetIO();
		ImGui::Begin( "Introstate", nullptr, ImGuiWindowFlags_NoFocusOnAppearing );
		if( frame == 0 ) // Do not focus this new window
			ImGui::SetWindowFocus( nullptr );

		if( ImGui::SliderInt( "int", &_p.x, 0, windowSize.x/2 ) && autoUpdate )
			_blendedText = nullptr;

		ImGui::Checkbox( "Auto-Redraw", &autoUpdate );
		ImGui::SameLine();
		ImGui::Checkbox( "Dark Outline", &_isDarkOutline );

		if( ImGui::Button( "Redraw" ) ) // Buttons return true when clicked (most widgets return true when edited/activated)
			_blendedText = nullptr;

		if( ImGui::SliderInt( "color index", &_colorIndex, 0, 63 ) ) {}

		static int charSelect = 65;
		ImGui::Text( "'%c'", (char) charSelect );
		ImGui::SameLine();
		if( ImGui::SliderInt( "charSel", &charSelect, 0, 255 ) ) {}

		/*
		TileFont::CharMetric & m = TileFont::char_metrics[charSelect];
		u8  min = 0,
			max = 8;
		ImGui::SliderScalarN( "cavity left", ImGuiDataType_U8, m.left, 8, &min, &max );
		ImGui::SliderScalarN( "cavity right", ImGuiDataType_U8, m.right, 8, &min, &max );
		*/

		ImGui::Checkbox( "Draw Color Number", &drawColorNumber );

		ImGui::PushID( "fg color" );
		ImGui::PushStyleVar( ImGuiStyleVar_FrameBorderSize, 1 );
		// CARE: ImU32 as color is 0xAABBGGRR - opposite of what might be expected
		ImGui::PushStyleColor( ImGuiCol_Border, 0xAAFFFFFF );
		constexpr fmt::format_string<int>
			withNumber( "{:02}" ),
			withoutNumber( "  ##{:02}" );
		const fmt::format_string<int> & fmt = drawColorNumber
			? withNumber
			: withoutNumber;

		auto needsLinebreak =[]( int i )
		{
			return i == 10 || i == 25 || i == 40 || i == 52;
		};

		for( int i = 0; i < 64; ++i )
		{
			const SDL_Color & color = HSNR64::Palette( i );
			const ImU32 pcol = ToU32( color );
			//const ImU32 pcol = std::bit_cast<ImU32>( hsnr64::Palette[i] );
			//Color color = hsnr64::Palette[i];
			ImGui::PushStyleColor( ImGuiCol_Button, pcol );
			ImGui::PushStyleColor( ImGuiCol_Text, pcol ^ 0x00808080 );
			if( ImGui::Button( format( fmt::runtime( fmt ), i ).c_str() ) )
				_colorIndex = i;
			ImGui::PopStyleColor( 2 );
			//ImGui::ColorButton( format( "color{}", i ).c_str(), *((ImVec4*)&hsnr64::Palette[i]), ImGuiColorEditFlags_Uint8 );
			//if(i%10 != 0)
			if( !needsLinebreak( i ) )
				ImGui::SameLine();
		}
		ImGui::NewLine(); // undo last SameLine
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();
		ImGui::PopID();

		if( ImGui::Button( "Open" ) )
		{
			nfdwindowhandle_t nw;
			NFD::GetNativeWindowFromSDLWindow( window(), &nw );

			NFD::UniquePath path;
			const NFD::Result result = NFD::OpenDialog( path, NFD::EmptyFilter, 0, NFD::EmptyDefaultPath, nw );    // Freezes execution of the Game

			if( result == NFD::Result::NFD_OKAY )
				print( "Success! Path is {0}\n", path.get() );
		}
		ImGui::SameLine();
		if( ImGui::Button( "OpenMultiple" ) )
		{
			NFD::UniquePathSet paths;
			const NFD::Result result = NFD::OpenDialogMultiple( paths, NFD::EmptyFilter, 0, NFD::EmptyDefaultPath );     // Freezes execution of the Game

			// TODO
			//if( result == NFD_OKAY )
			//	print( "Success! Path is {0}\n", fmt::join( paths.get(), ", " ) );
		}
		ImGui::SameLine();
		if( ImGui::Button( "Save" ) )
		{
			NFD::UniquePath path;
			const NFD::Result result = NFD::SaveDialog( path, NFD::EmptyFilter, 0, NFD::EmptyDefaultPath );     // Freezes execution of the Game
			//	const NFD::Result result = NFD::SaveDialog();   // The same as above

			if( result == NFD::Result::NFD_OKAY )
				print( "Success! Path is {0}\n", path.get() );
		}
		ImGui::SameLine();
		if( ImGui::Button( "PickFolder" ) )
		{
			NFD::UniquePath path;
			const NFD::Result result = NFD::PickFolder( path, NFD::EmptyDefaultPath );     // Freezes execution of the Game

			if( result == NFD_OKAY )
				print( "Success! Path is {0}\n", path.get() );
		}

		ImGui::End();
	}

#endif

}
