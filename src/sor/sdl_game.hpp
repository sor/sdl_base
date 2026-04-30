#pragma once

#include "core.hpp"
#include "sdl_core.hpp"
#include "sdl_smartptr.hpp"

#include "adapt_sdl_imgui.hpp"
#include "component/perftrack.hpp"

namespace JanSordid::SDL
{
	class IGameState;

	template <typename TGame/* = Game<IGameState>*/>
	class GameState;

	class IGame;

	template <typename TGameState/* = IGameState*/, typename TState/* = u8*/>
	class Game;

	//class GenericGameState;
	//class GenericGame;

	// abstract, interface
	// Enter and Exit can be called from being stacked-as-a child or by replaced-by-a sibling
	class IGameState
	{
	public:
		/// Ctors & Dtor
		         IGameState() noexcept = default;
		virtual ~IGameState() noexcept;

		IGameState(              const IGameState &  ) = delete;
		IGameState(                    IGameState && ) = delete;
		IGameState &  operator=( const IGameState &  ) = delete;
		IGameState && operator=(       IGameState && ) = delete;

		/// constexpr Getters, to override for configuration of derived states
		[[nodiscard]] virtual constexpr bool  isFPSLimited()    const noexcept { return true;  } // Limit the FPS of this State
		[[nodiscard]] virtual constexpr bool  isFillingScreen() const noexcept { return true;  } // Care does not work yet: For stacked states, a state that fills the screen will prevent states behind from rendering
		[[nodiscard]] virtual constexpr bool  isEventBubbling() const noexcept { return false; } // Care does not work yet: For stacked states, does this state propagate unhandled events further down the stack
		[[nodiscard]] virtual constexpr bool  isAlwaysUpdated() const noexcept { return false; } // Care does not work yet: For stacked states, does this state need to be updated, even if it is not on top
		[[nodiscard]] virtual constexpr Color clearColor()      const noexcept { return Color { 0, 0, 0, SDL_ALPHA_OPAQUE }; } // With what color shall the screen be cleared

		/// Methods: non-virtual first, followed by (pure) virtual/override
		virtual void Init()                     {} // Called before Enter()ing, can be called multiple times
		virtual void Enter( bool isStacking )   {} // Call when activating the state, either replacing the top state or stacking on top
		virtual void Exit( bool isStackedUpon ) {} // Call when deactivating the state, either being stacked upon or being completely removed from the stack
		virtual void Destroy()                  {} // Could be called after Exit()ing

		// This is mostly being replaced by HandleEvent, only use for stateful inputs, is already pumped.
		// Do only `return true` in overloads if ALL inputs are handled,
		// Care: `return true` here would skip the whole `while(SDL_PollEvent())` loop
		//  and all calls to Game::HandleEvent and GameState::HandleEvent
		virtual constexpr bool StatefulInput() { return false; }

		// Returns a bool to know if the Event was handled, maybe useful in the future if GameStates could be stacked (not possible yet)
		virtual bool Input( const Event & event ) = 0;
		virtual void Update( u64 framesSinceStart, Duration timeSinceStart, f32 deltaT       ) = 0;
		virtual void Render( u64 framesSinceStart, Duration timeSinceStart, f32 deltaTNeeded ) = 0;
		ImGuiOnly(
			virtual void RenderUI( u64 framesSinceStart, Duration timeSinceStart, f32 deltaTNeeded ) {})
	};

	// abstract
	template <typename TGame = Game<IGameState, u8>>
	class GameState : public IGameState
	{
	public:
		using Base = IGameState;

	protected:
		/// Members / Fields
		TGame & _game;

	public:
		/// Ctors & Dtor
		constexpr explicit GameState( TGame & game ) noexcept : _game( game ) {}
		constexpr         ~GameState()               noexcept override = default;

		/// Getters & Setters: non-virtual first, followed by (pure) virtual/override
		[[nodiscard]] constexpr Window   * window()   const noexcept { return _game.window();   }
		[[nodiscard]] constexpr Renderer * renderer() const noexcept { return _game.renderer(); }
		[[nodiscard]] constexpr Mixer    * mixer()    const noexcept { return _game.mixer();    }
	};

	// abstract, pseudo interface (contains fields)
	class IGame
	{
	protected:
		/// Types
		enum class NextStateOp : u8
		{
			None = 0,
			Replace,    // Modifies the GameStateIndex on top of the stack
			Push,       //   Adds a new GameStateIndex on top of the stack
			Pop,        //  Removes the GameStateIndex on top of the stack
		};

		static constexpr f32 ScalingFactorDynamic = -1;
		static constexpr f32 NoScaling            =  1;
		static constexpr int VSyncDisabled        = SDL_RENDERER_VSYNC_DISABLED;
		static constexpr int VSyncAdaptive        = SDL_RENDERER_VSYNC_ADAPTIVE;

		/// Members / Fields
		Owned<Window>   _window;
		Owned<Renderer> _renderer;
		Owned<Mixer>    _mixer;
		u64             _framesSinceStart = 0;
		Duration        _timeSinceStart   = Duration::zero();

		// TODO: This needs to become a queue to work with multiple changes in one frame
		DynArray<u8>    _stateStack;      // Not a std::stack since it's necessary to look inside without de-stacking
		u8              _stateNextVal;
		NextStateOp     _stateNextOp      = NextStateOp::None;

		f32  _scalingFactor;
		bool _isRunning = true;

		Point        _requestedSizeScaled;
		const char * _windowTitle;
		int          _vSync;

		ImGuiOnly(
			PerformanceTrackerComponent _perfTracker;)

	public:
		/// Ctors & Dtor
		// scalingFactor:
		// ScalingFactorDynamic = -1.0f (or unspecified) for autoscaling,
		// NoScaling            =  1.0f for "no" scaling,
		//  integer floating point values for pixel perfect scaling
		explicit IGame(
			const char * windowTitle   = "SDL Game",
			const Point  requestedSize = { 640, 360 },
			const f32    scalingFactor = ScalingFactorDynamic,
			const int    vSync         = VSyncAdaptive ) noexcept;
		virtual ~IGame() noexcept;

		IGame(              const IGame &  ) = delete;
		IGame(                    IGame && ) = delete;
		IGame &  operator=( const IGame &  ) = delete;
		IGame && operator=(       IGame && ) = delete;

		/// Getters & Setters: non-virtual first, followed by (pure) virtual/override
		[[nodiscard]]           Window   * window()            const noexcept { return _window.get();   } // even though this is a pointer, it is usually not null
		[[nodiscard]]           Renderer * renderer()          const noexcept { return _renderer.get(); } // even though this is a pointer, it is usually not null
		[[nodiscard]]           Mixer    * mixer()             const noexcept { return _mixer.get();    } // even though this is a pointer, it is usually not null
		[[nodiscard]] constexpr bool       isRunning()         const noexcept { return _isRunning;  }
		[[nodiscard]] constexpr f32        scalingFactor()     const noexcept { return _scalingFactor; }
		[[nodiscard]] constexpr bool       isStateChanging()   const noexcept { return _stateNextOp != NextStateOp::None; }
		[[nodiscard]] constexpr u8         currentStateIndex() const          { Assert( !_stateStack.empty() ); return _stateStack.back(); }

		[[nodiscard]] virtual constexpr       IGameState &  currentState()                  = 0;
		[[nodiscard]] virtual constexpr const IGameState &  currentState()   const          = 0;
		[[nodiscard]] virtual constexpr       usize         numberOfStates() const noexcept = 0;

		[[nodiscard]] virtual constexpr bool  isStateIndexValid( u8 index ) const noexcept
		{
			return (usize)index < numberOfStates();
		}

		/// Methods: non-virtual first, followed by (pure) virtual/override
		// Care: Currently not possible to Push multiple States in one Frame
		void PushState( u8 state );
		// Care: Currently not possible to Pop multiple States in one Frame (might quickly be problematic)
		void PopState();
		void ReplaceState( u8 state );
	protected:
		void ChangeState();
	private:
		void CreateWindowEtc();
		void DestroyWindowEtc();
		ImGuiOnly(
			void CreateImGui();
			void DestroyImGui();)

		void RenderClear();
		void RenderPresent();

	public:
		virtual int  Run();
		virtual void Input();
		virtual bool HandleEvent( const Event & event );
		virtual void Update( f32 deltaT );
		virtual void Render( f32 deltaTNeeded );
		ImGuiOnly(
			virtual void RenderUI( f32 deltaTNeeded ); )


		/// Performance
		/// TODO: refactor into a game-component class

	public:
		// All output msec per frame, see: http://renderingpipeline.com/2013/02/fps-vs-msecframe/
		enum class PerformanceDrawMode : u8
		{
			None,
			Title   = 0b001, // averaged and only updated every 250ms
			OStream = 0b010, // averaged and only updated every 1000ms
			Graph   = 0b100, // draw as imgui graph
		};

		void SetPerfDrawMode( PerformanceDrawMode mode ) noexcept { _perfDrawMode = mode; }

	protected:
		TimePoint           _lastPerfInfoTime  = Clock::now();
		Duration            _accumulatedNeeded = Duration::zero();
		u64                 _lastPerfInfoFrame = 0;
		PerformanceDrawMode _perfDrawMode      = PerformanceDrawMode::None;

		[[nodiscard]]
		float AverageMSecPerFrame() const;
		void  ResetPerformanceInfo( TimePoint current );
		void  OutputPerformanceInfo( TimePoint current, Duration needed );
	};

	template <typename TGameState = IGameState, typename TState = u8>
	class Game : public IGame
	{
		using Base = IGame;

	protected:
		/// Members / Fields
		DynArray<Owned<TGameState>> _allStates;

	public:
		/// Ctors & Dtor
		using Base::Base;

		/// Getters & Setters: non-virtual first, followed by (pure) virtual/override
		[[nodiscard]]           TGameState & currentState()                  override { return *_allStates[currentStateIndex()]; }
		[[nodiscard]] const     TGameState & currentState()   const          override { return *_allStates[currentStateIndex()]; }
		[[nodiscard]] constexpr usize        numberOfStates() const noexcept override { return _allStates.size(); }

		/// Methods: non-virtual first, followed by (pure) virtual/override
		template <typename... TS, typename TC>
		requires (sizeof...(TS) == 0) // Pre-C++20: typename std::enable_if_t<sizeof...(TS) == 0>
		constexpr void AddStates( TC & game ) {}

		template <typename T, typename... TS, typename TC>
		constexpr void AddStates( TC & game )
		{
			//_allStates.emplace_back( make_unique<T>( (typename T::Game*)*this ) );
			//AddStates<TS...>( static_cast<typename T::Game*>(*this) );
			_allStates.emplace_back( make_unique<T>( game ) );
			AddStates<TS...>( game );
		}

		void ReplaceState( TState state ) { Base::ReplaceState( (u8)state ); }
		void PushState(    TState state ) { Base::PushState(    (u8)state ); }
		//void PopState()                   { Base::PopState(); }
	};

	// Reference to explicit template instantiation in the .cpp file
	//extern template class Game<IGameState,u8>;
	//extern template class GameState<Game<IGameState,u8>>;

	/*
	// non template version
	class GenericGame : public IGame
	{
		using Base = IGame;

	protected:
		/// Members / Fields
		DynArray<Owned<IGameState>> _allStates;

	public:
		/// Ctors & Dtor
		using Base::Base;

		/// Getters & Setters: non-virtual first, followed by (pure) virtual/override
		[[nodiscard]]       IGameState & currentState()                  override { return *_allStates[currentStateIndex()]; }
		[[nodiscard]] const IGameState & currentState()   const          override { return *_allStates[currentStateIndex()]; }
		[[nodiscard]]       usize        numberOfStates() const noexcept override { return _allStates.size(); }
	};

	// abstract
	class GenericGameState : public IGameState
	{
	public:
		using Base = IGameState;

	protected:
		/// Members / Fields
		GenericGame & _game;

	public:
		/// Ctors & Dtor
		using Base::Base;
		//explicit GenericGameState( GenericGame && game ) = delete; // prevent taking an rvalue of Game
		explicit GenericGameState( GenericGame &  game ) : _game( game ) {}
		virtual ~GenericGameState() noexcept = default;

		/// Getters & Setters: non-virtual first, followed by (pure) virtual/override
		[[nodiscard]] Window   * window()   const noexcept { return _game.window();   }
		[[nodiscard]] Renderer * renderer() const noexcept { return _game.renderer(); }
	};
	*/
}
