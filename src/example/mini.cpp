#include "mini.hpp"

#include <hsnr64/offset.hpp>
#include <hsnr64/palette.hpp>

int main( int argc, char * argv [] )
{
	std::ios_base::sync_with_stdio( false );

	NotJanSordid::SDL_MiniExample::MiniGame game;
	return game.Run();
}

namespace NotJanSordid::SDL_MiniExample
{
	MiniGame::MiniGame()
		: Game( "MiniExampleGame", { 640, 360 }, ScalingFactorDynamic, VSyncAdaptive )
	{
		// The same as the _allStates.emplace...
		AddStates<MiniState>( *this ); // State 0
		//_allStates.emplace_back( make_unique<MiniState>( *this ) );

		PushState( 0 );

		IfNotFinal
			SetPerfDrawMode( PerformanceDrawMode::Title );
	}
}
