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

		std::array<f32,FrameTimesIndexCount> _frameTimesTotal;
		std::array<f32,FrameTimesIndexCount> _frameTimesUpdate;
		std::array<f32,FrameTimesIndexCount> _frameTimesRender;
		std::array<f32,FrameTimesIndexCount> _frameTimesRendUI;
		std::array<f32,FrameTimesIndexCount> _frameTimesDeltaT;

		Duration _startTimeUpdate;
		Duration _startTimeRender;
		Duration _startTimeRendUI;

		u8   _frameTimesIndex      = 0;
		u8   _frameTimesMode       = 0;
		bool _isFrameTimeRecording = true;

	public:
		PerformanceTrackerComponent()
		{
			_frameTimesTotal.fill( NAN );
			_frameTimesUpdate.fill( NAN );
			_frameTimesRender.fill( NAN );
			_frameTimesRendUI.fill( NAN );
			_frameTimesDeltaT.fill( NAN );
		}

		void RenderUI( const f32 deltaTNeeded );

		[[nodiscard]] Duration UpdateDuration() const { return now() - _startTimeUpdate; }
		[[nodiscard]] Duration RenderDuration() const { return now() - _startTimeRender; }
		[[nodiscard]] Duration RendUIDuration() const { return now() - _startTimeRendUI; }

		void StartUpdateTiming() { _startTimeUpdate = now(); }
		void StartRenderTiming() { _startTimeRender = now(); }
		void StartRendUITiming() { _startTimeRendUI = now(); }
		void RecordDeltaTDuration( const f32 deltaT ) { _frameTimesDeltaT[_frameTimesIndex] = deltaT * 1000.0f; }
		void EndUpdateTiming() { _frameTimesUpdate[_frameTimesIndex] = toFMilliSec( UpdateDuration() ); }
		void EndRenderTiming() { _frameTimesRender[_frameTimesIndex] = toFMilliSec( RenderDuration() ); }
		void EndRendUITiming() { _frameTimesRendUI[_frameTimesIndex] = toFMilliSec( RendUIDuration() ); }

		void ToggleRecording() { _isFrameTimeRecording = !_isFrameTimeRecording; }
		void ToggleMode()      { _frameTimesMode = (_frameTimesMode + 1) % FrameTimesModeCount; }
	};
}