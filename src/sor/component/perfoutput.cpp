#include "perfoutput.hpp"

namespace JanSordid::SDL
{
	void PerformanceOutputComponent::Output( SDL_Window * const window, const u64 framesSinceStart, const Duration timeSinceStart, const Duration needed )
	{
		_accumulatedNeeded += needed;

		const Duration passedTime = timeSinceStart - _lastInfoTime;

		switch( _mode )
		{
			case DrawMode::Title:
				if( passedTime > 250ms )
				{
					const std::string avgms = format( "{}ms", AverageMSecPerFrame( framesSinceStart ) );
					SDL_SetWindowTitle( window, avgms.c_str() );
					Reset( framesSinceStart, timeSinceStart );
				}
				break;

			case DrawMode::OStream:
				if( passedTime > 1000ms )
				{
					print( "{}ms", AverageMSecPerFrame( framesSinceStart ) );
					Reset( framesSinceStart, timeSinceStart );
				}
				break;

			case DrawMode::Graph:
			case DrawMode::None:
			default: break;
		}
	}

	float PerformanceOutputComponent::AverageMSecPerFrame( const u64 framesSinceStart ) const
	{
		const u64 passedFrames = framesSinceStart - _lastInfoFrame + 1;
		return toFMilliSec( _accumulatedNeeded / passedFrames );
	}

	void PerformanceOutputComponent::Reset( const u64 framesSinceStart, const Duration timeSinceStart )
	{
		_lastInfoFrame     = framesSinceStart;
		_lastInfoTime      = timeSinceStart;
		_accumulatedNeeded = Duration::zero();
	}
}