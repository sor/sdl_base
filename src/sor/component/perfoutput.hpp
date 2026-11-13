#pragma once

#include "../sdl_core.hpp"

namespace JanSordid::SDL
{
	// Parallel to PerformanceTrackerComponent: instead of recording timings,
	// this one accumulates the really needed frame duration and reports the
	// average msec per frame, see: http://renderingpipeline.com/2013/02/fps-vs-msecframe/
	class PerformanceOutputComponent
	{
	public:
		enum class DrawMode : u8
		{
			None,
			Title   = 0b001, // averaged and only updated every 250ms, written into the window title
			OStream = 0b010, // averaged and only updated every 1000ms, printed via print
			Graph   = 0b100, // no-op here, the graph lives in PerformanceTrackerComponent (F11)
		};

		void SetMode( DrawMode mode ) noexcept { _mode = mode; }

		// Call once per frame, e.g. at the top of the game loop
		void Output( SDL_Window * window, u64 framesSinceStart, Duration timeSinceStart, Duration needed );

	private:
		[[nodiscard]] float AverageMSecPerFrame( u64 framesSinceStart ) const;
		void                Reset( u64 framesSinceStart, Duration timeSinceStart );

		Duration  _lastInfoTime      = now();
		Duration  _accumulatedNeeded = Duration::zero();
		u64       _lastInfoFrame     = 0;
		DrawMode  _mode              = DrawMode::None;
	};
}