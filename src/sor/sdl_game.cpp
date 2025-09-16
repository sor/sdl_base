#include "sdl_game.hpp"
#include "sdl_shapeops.hpp"

#include "adapt_nfd.hpp"

namespace JanSordid::SDL
{
	// Explicit template instantiation in .cpp file
	//template class           Game<IGameState,u8>;
	//template class GameState<Game<IGameState,u8>>;

	IGame::IGame( const char * windowTitle, float scalingFactor, const Point logicalSize, const bool vSync ) noexcept
	{
		if( !SDL_Init( SDL_INIT_EVERYTHING ) )
		{
			print( stderr, "SDL_Init failed: {}\n", SDL_GetError() );
			exit( 1 );
		}

		if( !TTF_Init() )
		{
			print( stderr, "TTF_Init failed: {}\n", SDL_GetError() );
			exit( 2 );
		}

		constexpr MIX_InitFlags mixFlags = (MIX_InitFlags) (MIX_INIT_MP3 | MIX_INIT_OGG);
		if( Mix_Init( mixFlags ) != mixFlags )
		{
			print( stderr, "Mix_Init failed: {}\n", SDL_GetError() );
			exit( 4 );
		}

		if( !Mix_OpenAudio( 0, nullptr ) )
		{
			print( stderr, "Mix_OpenAudio failed: {}\n", SDL_GetError() );
			exit( 5 );
		}

		// Recalculate scalingFactor dynamically
		if( scalingFactor == -1.0f )
		{
			const SDL_DisplayID     displayID   = SDL_GetPrimaryDisplay();
			const SDL_DisplayMode * displayMode = SDL_GetDesktopDisplayMode( displayID );
			const FPoint factor = toF( Point{displayMode->w, displayMode->h } ) / toF( logicalSize );
			scalingFactor = std::max( 1.0f, std::floor( std::min( factor.x, factor.y ) - 0.2f ) );

			print( "Scaling Factor was calculated to be: {}\n", scalingFactor );
		}

		Point requestedSize = toI( toF( logicalSize ) * scalingFactor );
		_window = SDL_CreateWindow(
			windowTitle,
			requestedSize.x,
			requestedSize.y,
			SDL_WINDOW_OPENGL );

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
		SDL_SetNumberProperty(  props, SDL_PROP_RENDERER_CREATE_PRESENT_VSYNC_NUMBER, vSync ? SDL_RENDERER_VSYNC_ADAPTIVE : 0 );
		_renderer = SDL_CreateRendererWithProperties( props );

		if( _renderer == nullptr )
		{
			print( stderr, "Renderer could not be created: {}\n", SDL_GetError() );
			exit( 7 );
		}

		if( scalingFactor != 1.0f )
		{
			// TODO: test if this works as intended
			const bool isIntegerScaling = (scalingFactor == nearbyintf( scalingFactor ));
			if( isIntegerScaling )
			{
//				SDL_RenderSetIntegerScale( _renderer, true );
//				SDL_SetRenderLogicalPresentation( _renderer, logicalSize.x, logicalSize.y, SDL_LOGICAL_PRESENTATION_INTEGER_SCALE );

				//SDL_SetRenderLogicalPresentation( _renderer, 640, 360, SDL_RendererLogicalPresentation::SDL_LOGICAL_PRESENTATION_INTEGER_SCALE );
			}
			// HACK: Does not work because ImGui would not work then, it in fact turns this here off :(
			//SDL_SetHint( SDL_HINT_MOUSE_RELATIVE_SCALING, "1" );
			//SDL_RenderSetLogicalSize( _renderer, logicalSize.x, logicalSize.y );
		}
		_scalingFactor = scalingFactor;

		const NFD::Result nfdInit = NFD::Init();
		if( nfdInit != NFD_OKAY )
		{
			print( stderr, "Native File Dialog (NFD) could not be initialized: {}\n", NFD::GetError() );
			exit( 8 );
		}

		ImGuiOnly( CreateImGui( _renderer, _window ); )
	}

	IGame::~IGame() noexcept
	{
		NFD::Quit();

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

		while( TTF_WasInit() )
			TTF_Quit();

		constexpr u32 AllInit = 0;
		if( SDL_WasInit( AllInit ) )
			SDL_Quit();
	}

	void IGame::Input()
	{
		SDL_PumpEvents();

		if( currentState().Input() )
			return;

		Event event;
		while( SDL_PollEvent( &event ) )
		{
			// First try if BaseGame wants to handle the input (globals), then pass it to the GameState
			if( HandleEvent( event ) )
			{
				continue;
			}
			else
			{
				// Returns a bool to tell if the Event was expected and handled, if false, then this return could bubble up to a stacked GameState (future feature)
				currentState().HandleEvent( event );
			}
			// TODO: same as `if !game.HE then curr.HE` ?
		}
	}

	void IGame::Update( const f32 deltaT )
	{
		currentState().Update( _framesSinceStart, _msSinceStart, deltaT );
	}

	void IGame::Render( const f32 deltaTNeeded )
	{
		const Color clear = currentState().clearColor();
		SDL_SetRenderDrawColor( _renderer, clear.r, clear.g, clear.b, clear.a );
		SDL_RenderClear( _renderer );

		// This is placed before the GameState::Render call, to also allow calls to ImGui inside Render (although most ImGui calls should be in RenderUI)
	#if IMGUI
		//FPoint oldScale;
		//SDL_RenderGetScale(_renderer, &oldScale.x, &oldScale.y);
		//const ImGuiIO & io = ImGui::GetIO();
		//SDL_SetRenderScale( _renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y );
		ImGui_ImplSDLRenderer3_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();
		//SDL_RenderSetScale( _renderer, oldScale.x, oldScale.y );
	#endif

		currentState().Render( _framesSinceStart, _msSinceStart, deltaTNeeded );

		ImGuiOnly( RenderUI( deltaTNeeded ); )

		SDL_RenderPresent( _renderer );
	}

	#if IMGUI

	void IGame::RenderUI( const f32 deltaTNeeded )
	{
		currentState().RenderUI( _framesSinceStart, _msSinceStart, deltaTNeeded );

		ImGui::Render();
		ImGui_ImplSDLRenderer3_RenderDrawData( ImGui::GetDrawData(), _renderer );
	}

	void IGame::CreateImGui( Renderer * renderer, Window * window )
	{
		//SDL_GLContext gl_context = SDL_GL_CreateContext( window );
		//SDL_GL_MakeCurrent( window, gl_context );
		//SDL_GL_SetSwapInterval( 1 ); // Enable vsync

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO & io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();

		// Setup Platform/Renderer backends
		ImGui_ImplSDL3_InitForSDLRenderer( window, renderer );
		ImGui_ImplSDLRenderer3_Init( renderer );

		io.Fonts->AddFontDefault();
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

	#endif

	// Returns if the event has been handled
	bool IGame::HandleEvent( const Event & event )
	{
	#ifdef IMGUI
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
				const auto & key_event = event.key;

				if( (key_event.mod & SDL_KMOD_ALT) &&
				    (key_event.scancode == SDL_SCANCODE_F4) )
				{
					Event next_event = { .type = SDL_EVENT_QUIT };
					SDL_PushEvent( &next_event );
					return true;
				}
	#ifdef IMGUI
				else if( io.WantCaptureKeyboard )
				{
					return true;
				}
	#endif
				break;
			}

			case SDL_EVENT_KEY_UP:
	#ifdef IMGUI
				if( io.WantCaptureKeyboard )
					return true;
	#endif
				break;

			case SDL_EVENT_MOUSE_BUTTON_DOWN:
			case SDL_EVENT_MOUSE_BUTTON_UP:
			case SDL_EVENT_MOUSE_WHEEL:
	#ifdef IMGUI
				if( io.WantCaptureMouse )
					return true;
	#endif
				break;
		}

		return false;
	}

	int IGame::Run()
	{
		Duration  deltaTDur       = Duration::zero();
		Duration  deltaTDurNeeded = Duration::zero();   // How much time was really necessary
		TimePoint start;

		while( isRunning() )
		{
			start = Clock::now();

			      f32 deltaT       = std::chrono::duration<float>( deltaTDur       ).count();
			const f32 deltaTNeeded = std::chrono::duration<float>( deltaTDurNeeded ).count();

			// TODO: Make it possible to opt out / configure this
			constexpr f32 fpsLimit    = 30.0f;           // slowest frame rate in frames per second
			constexpr f32 deltaTLimit = 1.0f / fpsLimit; // slowest frame rate in delta time
			if( deltaT > deltaTLimit ) [[unlikely]]
			{
				IfNotFinal
					print( stderr, "Less than {} fps ({:.4}ms per frame), actually {:.4}ms, game will progress slower than realtime to stay consistent.\n",
					       fpsLimit, deltaTLimit * 1000.0f, deltaT * 1000.0f );

				deltaT = deltaTLimit;
			}

			OutputPerformanceInfo( start, deltaTDurNeeded );

			ChangeState();
			SDL_assert( currentStateIndex() >= 0 );

			// The difference to last frame is usually 16-17 at 60Hz, 10 at 100Hz, 8-9 at 120Hz, 6-*7* at 144Hz
			_msSinceStart = SDL_GetTicks();

			// Main loop trinity
			{
				Input();

				Update( deltaT );

				Render( deltaTNeeded );
			}

			deltaTDurNeeded = Clock::now() - start;

			// With VSync this should not be needed and only Delay( 0 );
			if( currentState().isFPSLimited() )
			{
				using namespace std::chrono_literals;

				const Duration dur = std::max( Duration::zero(), 16ms - deltaTDurNeeded );
				const u32 ms = static_cast<u32>( std::chrono::duration_cast<std::chrono::milliseconds>( dur ).count() );
				if( ms > 0 )
				{
					//print( "need to sleep {}\n", ms );
				}
				// TODO: Change towards SDL_DelayNS (or SDL_DelayPrecise only if precision is paramount)
				SDL_Delay( ms );
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
		return std::chrono::duration<float>( _accumulatedNeeded / passedFrames ).count() * 1000.0f;
	}

	void IGame::ResetPerformanceInfo( const TimePoint current )
	{
		_lastPerfInfoFrame = _framesSinceStart;
		_lastPerfInfoTime  = current;
		_accumulatedNeeded = Duration::zero();
	}

	void IGame::OutputPerformanceInfo( const TimePoint current, const Duration needed )
	{
		using namespace ChronoLiterals;

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
