#include "example_game.hpp"

#include "simpleeditor.hpp"
#include "roflstate.hpp"
#include "mapeditor.hpp"
#include "tgui.hpp"

namespace JanSordid::SDL_Example
{
	constexpr bool DoBenchmark = false;

	MyGame::MyGame()
		: Base( "Example SDL Game", { 640, 360 }, ScalingFactorDynamic, DoBenchmark ? VSyncDisabled : VSyncAdaptive )
	//	: Base( "Example SDL Game", Point { 1280, 960 }, ScalingFactorDynamic, doBenchmark ? 0 : VSyncAdaptive )
	{
		AddStates<
			IntroState,
			PlasmaState,
			SortState,
			CameraState,
			ShooterState,
			EditorState,
			RoflState,
			MapEditorState,
			TGUIState
		>( *this );

		// Set initial State
		PushState( MyGS::Intro );

		IfNotFinal
			SetPerfDrawMode( PerformanceDrawMode::Title );
	}

	bool MyGame::HandleEvent( const Event & event )
	{
		// Global Keybindings, will be checked before the ones in GameState
		if( Base::HandleEvent( event ) )
			return true;

		switch( event.type )
		{
			case SDL_EVENT_KEY_DOWN:
			{
				const SDL_KeyboardEvent & what_key = event.key;

				if( what_key.scancode == SDL_SCANCODE_1 )
				{
					// Both Game and GameState can change the State,
					// but it will only be changed after the current frame
					ReplaceState( MyGS::Intro );
					return true;
				}
				else if( what_key.scancode == SDL_SCANCODE_2 )
				{
					ReplaceState( MyGS::Plasma );
					return true;
				}
				else if( what_key.scancode == SDL_SCANCODE_3 )
				{
					ReplaceState( MyGS::Ball );
					return true;
				}
				else if( what_key.scancode == SDL_SCANCODE_4 )
				{
					ReplaceState( MyGS::Parallax );
					return true;
				}
				else if( what_key.scancode == SDL_SCANCODE_5 )
				{
					ReplaceState( MyGS::Shooter );
					return true;
				}
				else if( what_key.scancode == SDL_SCANCODE_6 )
				{
					ReplaceState( MyGS::Editor );
					return true;
				}
				else if( what_key.scancode == SDL_SCANCODE_7 )
				{
					ReplaceState( MyGS::Rofl );
					return true;
				}
				else if( what_key.scancode == SDL_SCANCODE_8 )
				{
					ReplaceState( MyGS::AdvEditor );
					return true;
				}
				else if( what_key.scancode == SDL_SCANCODE_9 )
				{
					ReplaceState( MyGS::GUI );
					return true;
				}
				else if( what_key.scancode == SDL_SCANCODE_0 )
				{
					ReplaceState( MyGS::Invalid );
					return true;
				}
				break;
			}

			default:
				break;
		}

		return false;
	}

	void MyGame::Update( const f32 deltaT )	{ Base::Update( deltaT ); }

	void MyGame::Render( const f32 deltaT ) { Base::Render( deltaT ); }

#if IMGUI
	void MyGame::RenderUI( const f32 deltaTNeeded ) { Base::RenderUI( deltaTNeeded ); }
#endif
}

int main( int argc, char * argv [] )
{
	using namespace JanSordid;

	try
	{
		std::ios_base::sync_with_stdio( false );

		SDL_Example::MyGame game;
		return game.Run();
	}
	catch( ... )
	{
		Core::print( "Non-SDL Exception has occurred!\n" );

		// put a breakpoint in this line, to prevent the shell from closing
		return 667;
	}
}
