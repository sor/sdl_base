#include "example_game.hpp"

#include <hsnr64/offset.hpp>
#include <hsnr64/palette.hpp>

#include <hsnr64/tilefont.hpp>

namespace JanSordid::SDL_Example
{
	void IntroState::Init()
	{
		Base::Init();

		if( !_font )
		{
			_font = TTF_OpenFont( BasePath "asset/font/RobotoSlab-Bold.ttf", (int)(10.0f * _game.scalingFactor()) );
			TTF_SetFontHinting( _font, TTF_HINTING_LIGHT );
			if( !_font )
				print( stderr, "TTF_OpenFont failed: {}\n", SDL_GetError() );
		}

		if( !_image )
		{
			_image = IMG_LoadTexture( renderer(), BasePath "asset/graphic/background.png" );
			if( !_image )
				print( stderr, "IMG_LoadTexture failed: {}\n", SDL_GetError() );
		}

		if( !_music )
		{
			_music = Mix_LoadMUS( BasePath "asset/music/severance.ogg" );
			if( !_music )
				print( stderr, "Mix_LoadMUS failed: {}\n", SDL_GetError() );
		}

		if( !_sound )
		{
			_sound = Mix_LoadWAV( BasePath "asset/sound/pew.wav" );
			if( !_sound )
				print( stderr, "Mix_LoadWAV failed: {}\n", SDL_GetError() );
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
			case SDL_EVENT_KEY_DOWN:
			{
				const auto & key = event.key;

				if( key.scancode == SDL_SCANCODE_F1 && event.key.repeat == 0 )
				{
					if( Mix_PausedMusic() )
						Mix_ResumeMusic();
					else
						Mix_PauseMusic();
				}
				else if( key.scancode == SDL_SCANCODE_F2 && event.key.repeat == 0 )
				{
					if( Mix_VolumeMusic( -1 ) == MIX_MAX_VOLUME )
						Mix_VolumeMusic( 0 );
					else
						Mix_VolumeMusic( MIX_MAX_VOLUME );
				}
				else if( key.scancode == SDL_SCANCODE_F3 && event.key.repeat == 0 )
				{
					Mix_PlayChannel( -1, _sound, 0 );
				}
				else if( key.scancode == SDL_SCANCODE_F4 && event.key.repeat == 0 )
				{
					_textmode = (_textmode + 1) % 2;
				}
				else if( key.scancode == SDL_SCANCODE_F9 )
				{
					// crash/shutdown, since State #6 does not exist
					_game.ReplaceState( MyGS::Invalid );
				}
				else if( key.scancode == SDL_SCANCODE_ESCAPE )
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

			case SDL_EVENT_MOUSE_BUTTON_DOWN:
				//game.SetNextState( 1 );
				break;

			default:
				break;
		}

		return false;
	}

	void IntroState::Update( const u64 framesSinceStart, const u64 msSinceStart, const f32 deltaT )
	{
	}

	void IntroState::Render( const u64 framesSinceStart, const u64 msSinceStart, const f32 deltaTNeeded )
	{
		Point windowSize;
		SDL_GetWindowSize( window(), &windowSize.x, &windowSize.y );

		{
			const FRect dst_rect = { 0, 0, (f32)windowSize.x, (f32)windowSize.y };
			SDL_RenderTexture( renderer(), _image, EntireFRect, &dst_rect /* same result as EntireRect */ );
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
				//"\n\nPress any key to continue!"
				"\n\n!\"#$%&'()*+,-./0123456789:;<=>? c*8 uzu"
				"\n@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
				"\n`abcdefghijklmnopqrstuvwxyz{|}~"
				"\n\u00A0!¡c¢E£¤Y¥¦S§¨©ª«¬\u00AD®¯°C±n²³´uµ¶·¸¹º»¼½¾?¿"
				"\nAÀÁÂÃÄ¨ÅÆCÇEÈÉÊËÌÍÎÏIDÐÑNOÒÓÔÕÖ×OØUÙÚÛÜYÝÞß"
				"\naàáâãäåæcçeèéêëiìíîïdðnñoòóôõö÷oøuùúûüyýþyÿ"
				"\ninjektion! enjoy major Heij project Stobject farbe kd aes real tw";

			const Color outlineColor = _isDarkOutline
				? Black
				: White;

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

					Owned<Surface> surf = TTF_RenderText_Blended_Wrapped( _font, text, 0, White, windowSize.x - 2 * _textStartPoint.x );
					_blendedText = SDL_CreateTextureFromSurface( renderer(), surf );

					SDL_GetTextureSize( _blendedText, &_blendedTextSize.x, &_blendedTextSize.y );
				}

				SDL_SetTextureColorMod( _blendedText, outlineColor.r, outlineColor.g, outlineColor.b );

				for( const FPoint & offset : HSNR64::ShadowOffset::Rhombus )
				{
					const FRect dst_rect = FRect{ _textStartPoint.x, _textStartPoint.y, _blendedTextSize.x, _blendedTextSize.y } + offset;
					SDL_RenderTexture( renderer(), _blendedText, EntireFRect, &dst_rect );
				}

				const Color & color = HSNR64::Palette( _colorIndex );
				SDL_SetTextureColorMod( _blendedText, color.r, color.g, color.b );
				const FRect dst_rect = { _textStartPoint.x, _textStartPoint.y, _blendedTextSize.x, _blendedTextSize.y };
				SDL_RenderTexture( renderer(), _blendedText, EntireFRect, &dst_rect );
			}
			else
			{
				FRect dimension { _textStartPoint.x, _textStartPoint.y, windowSize.x - (32 + _textStartPoint.x), 9999 };
				_tf.Render( text, dimension, HSNR64::Palette( _colorIndex ), outlineColor );
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

		if( ImGui::SliderFloat( "int", &_textStartPoint.x, 0, windowSize.x / 2 ) && autoUpdate )
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

		auto needsLinebreak = []( int i )
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
		ImGui::NewLine(); // Undo last SameLine
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();
		ImGui::PopID();

		if( ImGui::Button( "Open" ) ) [[unlikely]]
		{
			NFD::UniquePath   path;
			const NFD::Result result = NFD::OpenDialog( path, NFD::EmptyFilter, 0, NFD::EmptyPath );    // Freezes execution of the Game

			if( result == NFD::Result::NFD_OKAY )
				print( "Success! Path is {0}\n", path.get() );
		}

		ImGui::SameLine();
		if( ImGui::Button( "OpenMultiple" ) ) [[unlikely]]
		{
			NFD::UniquePathSet paths;
			const NFD::Result  result = NFD::OpenDialogMultiple( paths, NFD::EmptyFilter, 0, NFD::EmptyPath );     // Freezes execution of the Game

			if( result == NFD::Result::NFD_OKAY ) {
				nfdpathsetsize_t count;
				if( NFD::PathSet::Count( paths, count ) == NFD::Result::NFD_OKAY ) {
					for( uint i = 0; i < count ; ++i ) {
						NFD::UniquePathSetPath path;
						if( NFD::PathSet::GetPath( paths, i, path ) == NFD::Result::NFD_OKAY ) {
							print( "Success! Paths are {0}\n", path.get() );
						}
					}
				}
			}
		}

		ImGui::SameLine();
		if( ImGui::Button( "Save" ) ) [[unlikely]]
		{
			NFD::UniquePath   path;
			const NFD::Result result = NFD::SaveDialog( path, NFD::EmptyFilter, 0, NFD::EmptyPath, NFD::EmptyName ); // Freezes execution of the Game
			//	const NFD::Result result = NFD::SaveDialog();   // The same as above

			if( result == NFD::Result::NFD_OKAY )
				print( "Success! Path is {0}\n", path.get() );
		}

		ImGui::SameLine();
		if( ImGui::Button( "PickFolder" ) ) [[unlikely]]
		{
			NFD::UniquePath   path;
			const NFD::Result result = NFD::PickFolder( path, NFD::EmptyPath );     // Freezes execution of the Game

			if( result == NFD::Result::NFD_OKAY )
				print( "Success! Path is {0}\n", path.get() );
		}

		ImGui::End();
	}

#endif

}
