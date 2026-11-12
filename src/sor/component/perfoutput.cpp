#include "perfoutput.hpp"

namespace JanSordid::SDL
{
	void PerformanceOutputComponent::Output( SDL_Window * const window, const u64 framesSinceStart, const TimePoint current, const Duration needed )
	{
		_accumulatedNeeded += needed;

		const Duration passedTime = current - _lastInfoTime;

		switch( _mode )
		{
			case DrawMode::Title:
				if( passedTime > 250ms )
				{
					const String avgms = format( "{}ms", AverageMSecPerFrame( framesSinceStart ) );
					SDL_SetWindowTitle( window, avgms.c_str() );
					Reset( framesSinceStart, current );
				}
				break;

			case DrawMode::OStream:
				if( passedTime > 1000ms )
				{
					print( "{}ms", AverageMSecPerFrame( framesSinceStart ) );
					Reset( framesSinceStart, current );
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

	void PerformanceOutputComponent::Reset( const u64 framesSinceStart, const TimePoint current )
	{
		_lastInfoFrame    = framesSinceStart;
		_lastInfoTime     = current;
		_accumulatedNeeded = Duration::zero();
	}
}