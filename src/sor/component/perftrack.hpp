#pragma once

#include "../sdl_core.hpp"
#include "../sdl_smartptr.hpp"
#include "../adapt_sdl_imgui.hpp"

namespace JanSordid::SDL
{
	class PerformanceTrackerComponent
	{
		static constexpr u8 FrameTimesIndexCount = 128;
		static constexpr u8 FrameTimesModeCount  = 3;

		Array<f32,FrameTimesIndexCount> _frameTimesTotal;
		Array<f32,FrameTimesIndexCount> _frameTimesUpdate;
		Array<f32,FrameTimesIndexCount> _frameTimesRender;
		Array<f32,FrameTimesIndexCount> _frameTimesRendUI;
		Array<f32,FrameTimesIndexCount> _frameTimesDeltaT;

		TimePoint _startTimeUpdate;
		TimePoint _startTimeRender;
		TimePoint _startTimeRendUI;

		u8   _frameTimesIndex      = 0;
		u8   _frameTimesMode       = 0;
		bool _isFrameTimeRecording = true;

	public:
		void RenderUI( const f32 deltaTNeeded );

		[[nodiscard]] Duration UpdateDuration() const { return Clock::now() - _startTimeUpdate; }
		[[nodiscard]] Duration RenderDuration() const { return Clock::now() - _startTimeRender; }
		[[nodiscard]] Duration RendUIDuration() const { return Clock::now() - _startTimeRendUI; }

		void StartUpdateTiming() { _startTimeUpdate = Clock::now(); }
		void StartRenderTiming() { _startTimeRender = Clock::now(); }
		void StartRendUITiming() { _startTimeRendUI = Clock::now(); }
		void RecordDeltaTDuration( const f32 deltaT ) { _frameTimesDeltaT[_frameTimesIndex] = deltaT * 1000.0f; }
		void EndUpdateTiming() { _frameTimesUpdate[_frameTimesIndex] = duration_cast<FMilliSec>( UpdateDuration() ).count(); }
		void EndRenderTiming() { _frameTimesRender[_frameTimesIndex] = duration_cast<FMilliSec>( RenderDuration() ).count(); }
		void EndRendUITiming() { _frameTimesRendUI[_frameTimesIndex] = duration_cast<FMilliSec>( RendUIDuration() ).count(); }

		void ToggleRecording() { _isFrameTimeRecording = !_isFrameTimeRecording; }
		void ToggleMode()      { _frameTimesMode = (_frameTimesMode + 1) % FrameTimesModeCount; }
	};
}