#include "mini.hpp"

#include "hsnr64/offset.hpp"

extern "C"
int main( int argc, char * argv [] )
{
	std::ios_base::sync_with_stdio( false );

	NotJanSordid::SDL_MiniExample::MiniGame game;
	return game.Run();
}

namespace NotJanSordid::SDL_MiniExample
{
	MiniGame::MiniGame() : Game( "MiniExampleGame", -1, Point{640,360}, false )
	{
		_allStates.emplace_back( make_unique<MiniState>( *this ) );
		PushState( 0 );
		SetPerfDrawMode(PerformanceDrawMode::Title);
	}

#ifdef IMGUI
	void MiniState::RenderUI( const u64 frame, const u64 totalMSec, const f32 deltaTNeeded )
	{
		// ImGui Demo
		static bool show_demo_window = true;
		ImGui::SetNextWindowCollapsed( true, ImGuiCond_Once );
		ImGui::ShowDemoWindow( &show_demo_window );

		//const Point & winSize = game.GetWindowSize();

		static bool autoUpdate = false;
		static bool drawColorNumber = false;
		//ImGuiIO & io = ImGui::GetIO();
		ImGui::Begin( "Introstate", nullptr, ImGuiWindowFlags_NoFocusOnAppearing );
		if( frame == 0 ) // Do not focus this new window
			ImGui::SetWindowFocus( nullptr );

		if( ImGui::SliderInt( "int", &_p.x, 0, 320 ) && autoUpdate )
			_blendedText = nullptr;

		ImGui::Checkbox( "Auto-Redraw", &autoUpdate );
		ImGui::SameLine();
		ImGui::Checkbox( "Dark Outline", &_isDarkOutline );

		if( ImGui::Button(
			"Redraw" ) )                            // Buttons return true when clicked (most widgets return true when edited/activated)
			_blendedText = nullptr;

		if( ImGui::SliderInt( "color index", &_colorIndex, 0, 63 ) )
		{}

		static int charSelect = 65;
		ImGui::Text( "'%c'", (char) charSelect );
		ImGui::SameLine();
		if( ImGui::SliderInt( "charSel", &charSelect, 0, 255 ) )
		{}
//
//		TileFont::CharMetric & m = TileFont::char_metrics[charSelect];
//		u8 min = 0,
//			max = 8;
//		ImGui::SliderScalarN( "cavity left", ImGuiDataType_U8, m.left, 8, &min, &max );
//		ImGui::SliderScalarN( "cavity right", ImGuiDataType_U8, m.right, 8, &min, &max );

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
			const SDL_Color & color = JanSordid::HSNR64::Palette( i );
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
		ImGui::NewLine(); // undo last SameLine
		ImGui::PopStyleColor();
		ImGui::PopStyleVar();
		ImGui::PopID();

		if( ImGui::Button( "Open" ) )
		{
			NFD::Guard g;
			NFD::UniquePath path;
			const NFD::Result result = NFD::OpenDialog( path );    // Freezes execution of the Game

			if( result == NFD::Result::NFD_OKAY )
				print( "Success! Path is {0}\n", path.get() );
		}

		ImGui::End();
	}
#endif
}
