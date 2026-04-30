#include "sdl_game.hpp"
#include "sdl_shapeops.hpp"

#include "adapt_nfd.hpp"

namespace JanSordid::SDL
{
	// Explicit template instantiation in .cpp file
	//template class           Game<IGameState,u8>;
	//template class GameState<Game<IGameState,u8>>;

	// We want one out of line virtual definition, to not have vtables in every TU
	IGameState::~IGameState() noexcept = default;

	IGame::IGame( const char * const windowTitle, const Point requestedSize, f32 scalingFactor, const int vSync ) noexcept
		: _windowTitle{ windowTitle },
		  _vSync{ vSync }
	{
		if( !SDL_Init( SDL_INIT_EVERYTHING ) )
		{
			print( stderr, "SDL_Init failed: {}\n", SDL_GetError() );
			exit( 1 );
		}

		//SDL_GL_SetAttribute( SDL_GL_MULTISAMPLEBUFFERS, 1 );
		//SDL_GL_SetAttribute( SDL_GL_MULTISAMPLESAMPLES, 4 );

		if( !TTF_Init() )
		{
			print( stderr, "TTF_Init failed: {}\n", SDL_GetError() );
			exit( 2 );
		}

		if( !MIX_Init() )
		{
			print( stderr, "Mix_Init failed: {}\n", SDL_GetError() );
			exit( 4 );
		}

		// Recalculate scalingFactor dynamically
		if( scalingFactor == ScalingFactorDynamic )
		{
			const SDL_DisplayID     displayID   = SDL_GetPrimaryDisplay();
			const SDL_DisplayMode * displayMode = SDL_GetDesktopDisplayMode( displayID );
			const FPoint            factor      = toF( Point{ displayMode->w, displayMode->h } ) / toF( requestedSize );
			scalingFactor                       = std::max( 1.0f, std::floor( std::min( factor.x, factor.y ) - 0.2f ) );

			print( "Scaling Factor was calculated to be: {}\n", scalingFactor );
		}

		if( scalingFactor != NoScaling )
		{
			// TODO: test if this works as intended
			const bool isIntegerScaling = (scalingFactor == nearbyintf( scalingFactor ));
			if( isIntegerScaling )
			{
//				SDL_RenderSetIntegerScale( _renderer, true );
//				SDL_SetRenderLogicalPresentation( _renderer, requestedSize.x, requestedSize.y, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE );

//				SDL_SetRenderLogicalPresentation( _renderer, 640, 360, SDL_RendererLogicalPresentation::SDL_LOGICAL_PRESENTATION_INTEGER_SCALE );
			}
			// HACK: Does not work because ImGui would not work then, it in fact turns this here off :(
			//SDL_SetHint( SDL_HINT_MOUSE_RELATIVE_SCALING, "1" );
			//SDL_RenderSetLogicalSize( _renderer, requestedSize.x, requestedSize.y );
		}
		_scalingFactor       = scalingFactor;
		_requestedSizeScaled = toI( toF( requestedSize ) * _scalingFactor );

		const NFD::Result nfdInit = NFD::Init();
		if( nfdInit != NFD_OKAY )
		{
			print( stderr, "Native File Dialog (NFD) could not be initialized: {}\n", NFD::GetError() );
			exit( 8 );
		}

		ImGuiOnly( CreateImGui(); )

		// TODO: Maybe remove this from in here
		CreateWindowEtc();
	}

	IGame::~IGame() noexcept
	{
		_mixer.release(); // RAII is sad

		DestroyWindowEtc();

		ImGuiOnly( DestroyImGui(); )

		NFD::Quit();

		MIX_Quit();

		while( TTF_WasInit() )
			TTF_Quit();

		constexpr u32 AllInit = 0;
		if( SDL_WasInit( AllInit ) )
			SDL_Quit();
	}

	void IGame::CreateWindowEtc()
	{
		_window = SDL_CreateWindow(
			_windowTitle,
			_requestedSizeScaled.x,
			_requestedSizeScaled.y,
			SDL_WINDOW_OPENGL /*| SDL_WINDOW_HIGH_PIXEL_DENSITY*/ );
		if( _window == nullptr )
		{
			print( stderr, "Window could not be created: {}\n", SDL_GetError() );
			exit( 6 );
		}

//		_renderer = SDL_CreateRenderer(
//			_window,
//			nullptr);

		SDL_PropertiesID props = SDL_CreateProperties();
		SDL_SetPointerProperty( props, SDL_PROP_RENDERER_CREATE_WINDOW_POINTER,       _window );
		SDL_SetNumberProperty(  props, SDL_PROP_RENDERER_CREATE_PRESENT_VSYNC_NUMBER, _vSync  );
		_renderer = SDL_CreateRendererWithProperties( props );
		if( _renderer == nullptr )
		{
			print( stderr, "Renderer could not be created: {}\n", SDL_GetError() );
			exit( 7 );
		}

		_mixer = MIX_CreateMixerDevice( SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr );
		if( _mixer == nullptr )
		{
			print( stderr, "MIX_CreateMixerDevice failed: {}\n", SDL_GetError() );
			exit( 5 );
		}

		ImGuiOnly(
			ImGui_ImplSDL3_InitForSDLRenderer( _window, _renderer );
			ImGui_ImplSDLRenderer3_Init( _renderer ); )
	}

	void IGame::DestroyWindowEtc()
	{
		if( _renderer != nullptr )
		{
			SDL_DestroyRenderer( _renderer );
			_renderer = nullptr;
		}

		if( _window != nullptr )
		{
			SDL_DestroyWindow( _window );
			_window = nullptr;
		}
	}

#if USE_IMGUI

	void IGame::CreateImGui()
	{
		//SDL_GLContext gl_context = SDL_GL_CreateContext( window );
		//SDL_GL_MakeCurrent( window, gl_context );
		//SDL_GL_SetSwapInterval( 1 ); // Enable vsync

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImPlot::CreateContext();
		ImGuiIO & io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.Fonts->AddFontDefault();

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();

		// Setup Platform/Renderer backends
		//ImGui_ImplSDL3_InitForSDLRenderer( _window, _renderer );
		//ImGui_ImplSDLRenderer3_Init( _renderer );

		/*
		// Decide GL+GLSL versions
	#if defined(IMGUI_IMPL_OPENGL_ES2)
		const char * glsl_version = "#version 100";
	#elif defined(__APPLE__)
		const char * glsl_version = "#version 150";
	#else
		const char * glsl_version = "#version 130";
	#endif
		// Setup Platform/Renderer backends
		SDL_GLContext gl_context = SDL_GL_GetCurrentContext();
		ImGui_ImplSDL2_InitForOpenGL( window, gl_context );
		ImGui_ImplOpenGL3_Init( glsl_version );
		*/
	}

	void IGame::DestroyImGui()
	{
		ImGui_ImplSDLRenderer3_Shutdown();
		ImGui_ImplSDL3_Shutdown();

		ImPlot::DestroyContext();
		ImGui::DestroyContext();
	}

#endif

	void IGame::Input()
	{
		SDL_PumpEvents();

		if( currentState().StatefulInput() )
			return;

		Event event;
		while( SDL_PollEvent( &event ) )
		{
			// First try if Game wants to handle the input (globals), then pass it to the GameState
			const bool hasGameHandledEvent = HandleEvent( event );
			if( !hasGameHandledEvent )
			{
				// This call returns a bool to tell if the Event was expected and handled,
				// TODO: if false, then this return could bubble up to a stacked GameState (future feature)
				currentState().Input( event );
			}
		}
	}

	void IGame::Update( const f32 deltaT )
	{
		currentState().Update( _framesSinceStart, _timeSinceStart, deltaT );
	}

	void IGame::RenderClear()
	{
		const Color clear = currentState().clearColor();
		SDL_SetRenderDrawColor( _renderer, clear.r, clear.g, clear.b, clear.a );
		SDL_RenderClear( _renderer );
	}

	void IGame::RenderPresent()
	{
		SDL_RenderPresent( _renderer );
	}

	void IGame::Render( const f32 deltaTNeeded )
	{
		// This is placed before the GameState::Render call, to also allow calls to ImGui inside Render (although most ImGui calls should be in RenderUI)
#if USE_IMGUI
		//FPoint oldScale;
		//SDL_RenderGetScale(_renderer, &oldScale.x, &oldScale.y);
		//const ImGuiIO & io = ImGui::GetIO();
		//SDL_SetRenderScale( _renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y );
		ImGui_ImplSDLRenderer3_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();
		//SDL_RenderSetScale( _renderer, oldScale.x, oldScale.y );
#endif

		currentState().Render( _framesSinceStart, _timeSinceStart, deltaTNeeded );
	}

#if USE_IMGUI

	void IGame::RenderUI( const f32 deltaTNeeded )
	{
		_perfTracker.RenderUI( deltaTNeeded );

		currentState().RenderUI( _framesSinceStart, _timeSinceStart, deltaTNeeded );

		ImGui::Render();
		ImGui_ImplSDLRenderer3_RenderDrawData( ImGui::GetDrawData(), _renderer );
	}

#endif

	// Returns if the event has been handled
	bool IGame::HandleEvent( const Event & event )
	{
#ifdef USE_IMGUI
		const ImGuiIO & io = ImGui::GetIO();
		ImGui_ImplSDL3_ProcessEvent( &event );
#endif

		switch( event.type )
		{
			case SDL_EVENT_QUIT:
				_isRunning = false;
				return true;
				break;

			case SDL_EVENT_KEY_DOWN:
			{
				const auto & what_key = event.key;

				if( (what_key.mod & SDL_KMOD_ALT) &&
				    (what_key.scancode == SDL_SCANCODE_F4) )
				{
					Event next_event = { .type = SDL_EVENT_QUIT };
					SDL_PushEvent( &next_event );
					return true;
				}
#if USE_IMGUI
				else if( what_key.scancode == SDL_SCANCODE_F11 && !event.key.repeat )
				{
					//         F11 -> toggle display of frame time graph
					// Shift + F11 -> toggle recoding of frame time
					if( what_key.mod & SDL_KMOD_SHIFT )
						_perfTracker.ToggleRecording();
					else
						_perfTracker.ToggleMode();

					return true;
				}
				else if( io.WantCaptureKeyboard )
				{
					return true;
				}
#endif
				break;
			}

			case SDL_EVENT_KEY_UP:
	#if USE_IMGUI
				if( io.WantCaptureKeyboard )
					return true;
	#endif
				break;

			case SDL_EVENT_MOUSE_BUTTON_DOWN:
			case SDL_EVENT_MOUSE_BUTTON_UP:
			case SDL_EVENT_MOUSE_WHEEL:
	#if USE_IMGUI
				if( io.WantCaptureMouse )
					return true;
	#endif
				break;
		}

		return false;
	}

	int IGame::Run()
	{
		Duration deltaTDur       = {};
		Duration deltaTDurNeeded = {};   // How much time was really necessary

		while( isRunning() )
		{
			const TimePoint start = Clock::now();

			      f32 deltaT       = duration_cast<FSec>( deltaTDur       ).count();
			const f32 deltaTNeeded = duration_cast<FSec>( deltaTDurNeeded ).count();

			// TODO: Make it possible to opt out and configure this
			constexpr f32 fpsLimit    = 30.0f;           // slowest frame rate in frames per second
			constexpr f32 deltaTLimit = 1.0f / fpsLimit; // slowest frame rate in delta time
			if( deltaT > deltaTLimit ) [[unlikely]]
			{
				IfNotFinal
					print( stderr,
						"Less than {} fps ({:.4}ms per frame), actually {:.4}ms, game will progress slower than realtime to stay consistent.\n",
					    fpsLimit, deltaTLimit * 1000.0f, deltaT * 1000.0f );

				deltaT = deltaTLimit;
			}

			OutputPerformanceInfo( start, deltaTDurNeeded );

			ChangeState();
			SDL_assert( currentStateIndex() >= 0 );

			// The difference to last frame is usually
			//  33'333'333ns at 30Hz,
			//  16'666'666ns at 60Hz,
			//  10'000'000ns at 100Hz,
			//   8'333'333ns at 120Hz,
			//   6'944'444ns at 144Hz...
			_timeSinceStart = NanoSec{ SDL_GetTicksNS() };

			// Main loop trinity
			{
				Input();

#if USE_IMGUI
				_perfTracker.RecordDeltaTDuration( deltaT );

				_perfTracker.StartUpdateTiming();
#endif
				Update( deltaT );
#if USE_IMGUI
				_perfTracker.EndUpdateTiming();

				_perfTracker.StartRenderTiming();
#endif
				RenderClear();
				Render( deltaTNeeded );
#if USE_IMGUI
				_perfTracker.EndRenderTiming();

				_perfTracker.StartRendUITiming();
				RenderUI( deltaTNeeded );
				_perfTracker.EndRendUITiming();
#endif
				RenderPresent();
			}

			deltaTDurNeeded = Clock::now() - start;

			// With VSync this should not be needed and only Delay( 0 );
			if( currentState().isFPSLimited() )
			{
				//using namespace ChronoLiterals;

				const Duration dur = std::max( Duration::zero(), 16ms - deltaTDurNeeded );
				if( dur > 1ns )
				{
					const i64 ns = duration_cast<NanoSec>( dur ).count();
					//print( "need to sleep {}\n", ms );
					// TODO: Change towards SDL_DelayPrecise if precision is paramount
					SDL_DelayNS( ns );
				}
				deltaTDur = Clock::now() - start;
			}
			else
			{
				deltaTDur = deltaTDurNeeded;
			}

			++_framesSinceStart;
		}
		return 0;
	}

	float IGame::AverageMSecPerFrame() const
	{
		const u64 passedFrames = _framesSinceStart - _lastPerfInfoFrame + 1;
		return duration_cast<FMilliSec>( _accumulatedNeeded / passedFrames ).count();
	}

	void IGame::ResetPerformanceInfo( const TimePoint current )
	{
		_lastPerfInfoFrame = _framesSinceStart;
		_lastPerfInfoTime  = current;
		_accumulatedNeeded = Duration::zero();
	}

	void IGame::OutputPerformanceInfo( const TimePoint current, const Duration needed )
	{
		//using namespace ChronoLiterals;

		_accumulatedNeeded += needed;

		const Duration passedTime = current - _lastPerfInfoTime;

		switch( _perfDrawMode )
		{
			case PerformanceDrawMode::Title:
				if( passedTime > 250ms )
				{
					const std::string avgms = format( "{}ms", AverageMSecPerFrame() );
					SDL_SetWindowTitle( _window, avgms.c_str() );
					ResetPerformanceInfo( current );
				}
				break;

			case PerformanceDrawMode::OStream:
				if( passedTime > 1000ms )
				{
					print( "{}ms", AverageMSecPerFrame() );
					ResetPerformanceInfo( current );
				}
				break;

			case PerformanceDrawMode::Graph:
			case PerformanceDrawMode::None:
			default: break;
		}
	}

	void IGame::PushState( const u8 state )
	{
		_stateNextOp = _stateNextOp == NextStateOp::Pop
			? NextStateOp::Replace
			: NextStateOp::Push;

		_stateNextVal = state;
	}

	void IGame::PopState()
	{
		assert( !_stateStack.empty() );
		_stateNextOp = NextStateOp::Pop;
	}

	void IGame::ReplaceState( const u8 state )
	{
		PopState();
		PushState( state );
	}

	void IGame::ChangeState()
	{
		if( _stateNextOp == NextStateOp::None )
		{
			//assert( false && "Change called with nextOp == None" );
			return;
		}

		if( _stateNextOp == NextStateOp::Pop
		 || _stateNextOp == NextStateOp::Replace )
		{
			currentState().Exit( _stateNextOp == NextStateOp::Pop );

			// When to call this?
			//currentState().Destroy();

			_stateStack.pop_back();
		}

		if( _stateNextOp == NextStateOp::Push
		 || _stateNextOp == NextStateOp::Replace )
		{
			assert( isStateIndexValid( _stateNextVal ) && "Invalid State" );

			_stateStack.push_back( _stateNextVal );

			currentState().Init();

			currentState().Enter( _stateNextOp == NextStateOp::Push );
		}

		_stateNextOp  = NextStateOp::None;
		_stateNextVal = 0;
	}
}
