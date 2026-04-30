#include "perftrack.hpp"

#if USE_IMGUI

#include <imgui.h>
#include <implot.h>

namespace JanSordid::SDL
{
	void PerformanceTrackerComponent::RenderUI( const f32 deltaTNeeded )
	{
		_frameTimesTotal[_frameTimesIndex]    = deltaTNeeded * 1000;

		const u8 nextFrameTimeIndex           = (_frameTimesIndex + 1) % FrameTimesIndexCount;
		_frameTimesTotal[nextFrameTimeIndex]  = NAN;
		_frameTimesUpdate[nextFrameTimeIndex] = NAN;
		_frameTimesRender[nextFrameTimeIndex] = NAN;
		_frameTimesRendUI[nextFrameTimeIndex] = NAN;
		_frameTimesDeltaT[nextFrameTimeIndex] = NAN;

		if( _frameTimesMode == 1 ) {
			ImGui::Begin( "Frames", 0, ImGuiWindowFlags_NoTitleBar
			                           | ImGuiWindowFlags_NoResize
			                           // | ImGuiWindowFlags_NoSavedSettings
			                           | ImGuiWindowFlags_NoFocusOnAppearing );
			ImGui::TextColored(
				{1, 0.8, 0.4, 1},
				"Total: %6.2fms\nUpdate: %5.2fms\nRender: %5.2fms\nRendUI: %5.2fms\nDeltaT: %5.2fms",
				_frameTimesTotal[_frameTimesIndex],
				_frameTimesUpdate[_frameTimesIndex],
				_frameTimesRender[_frameTimesIndex],
				_frameTimesRendUI[_frameTimesIndex],
				_frameTimesDeltaT[_frameTimesIndex]
			);
			ImGui::End();
		}
		else if( _frameTimesMode == 2 )
		{
			ImGui::Begin( "Frametime", 0, ImGuiWindowFlags_NoTitleBar
			                              | ImGuiWindowFlags_NoResize
			                              // | ImGuiWindowFlags_NoSavedSettings
			                              | ImGuiWindowFlags_NoFocusOnAppearing );
			//			ImGui::PlotLines( "##Frame Times", _frameTimes.data(), (int)_frameTimes.size(), 0, nullptr, 0, 20, ImVec2{ (f32)_frameTimes.size()*2, 140 } );
			//			ImPlot::SetNextAxesToFit();
			ImPlot::SetNextAxesLimits( 0, (f64)_frameTimesTotal.size(), 0.1, 40, ImPlotCond_Once );
			if( ImPlot::BeginPlot( "##Frame Times Plot", ImVec2{ (f32)_frameTimesTotal.size() * 2, 120 }, ImPlotFlags_NoInputs | ImPlotFlags_NoFrame | /*ImPlotFlags_NoChild |*/ ImPlotFlags_NoTitle | ImPlotFlags_NoLegend | ImPlotFlags_NoMenus | ImPlotFlags_NoBoxSelect ) )
			{
				ImPlot::SetupAxisScale( ImAxis_Y1, ImPlotScale_SymLog );
				ImPlot::PlotLine( "Total",  _frameTimesTotal.data(),  (int)_frameTimesTotal.size() );
				ImPlot::PlotLine( "Update", _frameTimesUpdate.data(), (int)_frameTimesUpdate.size() );
				ImPlot::PlotLine( "Render", _frameTimesRender.data(), (int)_frameTimesRender.size() );
				ImPlot::PlotLine( "DeltaT", _frameTimesDeltaT.data(), (int)_frameTimesDeltaT.size() );
				ImPlot::PlotLine( "RendUI", _frameTimesRendUI.data(), (int)_frameTimesRendUI.size() );
				//ImPlot::PlotLine( "My Line Plot", x_data, y_data, 1000 );
				ImPlot::EndPlot();
			}
			ImGui::End();
		}

		if( _isFrameTimeRecording )
			_frameTimesIndex = nextFrameTimeIndex;
	}
}

#endif
