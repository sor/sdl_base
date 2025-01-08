#include "mini.hpp"

#include <hsnr64/offset.hpp>
#include <hsnr64/palette.hpp>

extern "C"
int main( int argc, char * argv [] )
{
	std::ios_base::sync_with_stdio( false );

	NotJanSordid::SDL_MiniExample::MiniGame game;
	return game.Run();
}

namespace NotJanSordid::SDL_MiniExample
{
	MiniGame::MiniGame()
		: Game( "MiniExampleGame", -1, Point{ 640, 360 }, false )
	{
		// The same as the _allStates.emplace...
		AddStates<MiniState>( *this );
		//_allStates.emplace_back( make_unique<MiniState>( *this ) );

		PushState( 0 );

		IfNotFinal
			SetPerfDrawMode( PerformanceDrawMode::Title );
	}
}
