#pragma once

#include "../global.hpp"

#include <sor/sdl_extgame.hpp>
#include <sor/sdl_shapeops.hpp>

#include <hsnr64/offset.hpp>
#include <hsnr64/palette.hpp>
#include <hsnr64/tilefont.hpp>
#include <hsnr64/tiles.hpp>

namespace JanSordid::SDL_Example
{
	using namespace JanSordid::SDL;

	class MyGame;
	class MyGameState;

	class IntroState;
	class PlasmaState;
	class SortState;
	class CameraState;
	class ShooterState;
	class EditorState;
	class RoflState;
	class MapEditorState;
	class TGUIState;

	// MyGameState-Index
	// Why not in the classes themselves? For reusability!
	enum class MyGS : u8
	{
		Intro = 0,
		Plasma,
		Ball,
		Parallax,
		Shooter,
		Editor,
		Rofl,
		AdvEditor,
		GUI,
		// "Invalid" is a special value, do NOT reproduce in your own game,
		// its only purpose is to show how hard it would be to crash the game
		Invalid,
	};

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wweak-vtables"
	// Only create this intermediary class if you want to add something to ALL of your GameStates,
	// else let them derive from SDL::GameState<MyGame> directly
	// abstract
	class MyGameState : public SDL::GameState<MyGame>
	{
		using Base = GameState;

	protected:
		static constexpr const SDL_Color White = HSNR64::Palette( HSNR64::NamedColor::White );
		static constexpr const SDL_Color Black = HSNR64::Palette( HSNR64::NamedColor::Black );

	public:
		// ctor
		using Base::Base;
	};
#pragma clang diagnostic pop

	class MyGame final : public SDL::Game<MyGameState,MyGS>
	{
		using Base = Game;

	public:
		MyGame();

		bool HandleEvent( const SDL_Event & event ) override;
		void Update( f32 deltaT ) override;
		void Render( f32 deltaT ) override;
		ImGuiOnly(
			void RenderUI( f32 deltaTNeeded ) override; )
	};


	class IntroState final : public MyGameState
	{
		using Base = MyGameState;

	protected:
		HSNR64::TileFont _tf { _game.renderer() };

		Owned<TTF_Font>    _font;
		Owned<SDL_Texture> _image;
		Owned<MIX_Audio>   _music;
		Owned<MIX_Audio>   _sound;
		Owned<MIX_Track>   _trackMusic;
		Owned<MIX_Track>   _trackSound;
		Owned<SDL_Texture> _blendedText;

		SDL_FPoint _blendedTextSize = { 0, 0 };
		u8         _textmode        = 0;

		// Testvars controlled by ImGui
		SDL_FPoint _textStartPoint    = { 20, 30 };
		i32        _colorIndex        = 9;
		bool       _isDarkOutline     = true;
		bool       _showExampleWindow = false;

	public:
		/// Ctors & Dtor
		using Base::Base;

		/// Getters & Setters: non-virtual first, followed by (pure) virtual/override
		[[nodiscard]] constexpr SDL_Color clearColor() const noexcept override { return White; }

		/// Methods: non-virtual first, followed by (pure) virtual/override
		void Init()                     override;
		void Enter( bool isStacking )   override;
		void Exit( bool isStackedUpon ) override;
		void Destroy()                  override;

		bool Input( const SDL_Event & event ) override;
		void Update( u64 framesSinceStart, Duration timeSinceStart, f32 deltaT       ) override;
		void Render( u64 framesSinceStart, Duration timeSinceStart, f32 deltaTNeeded ) override;
		ImGuiOnly(
			void RenderUI( u64 framesSinceStart, Duration timeSinceStart, f32 deltaTNeeded ) override;)

		//bool isFPSLimited() const noexcept override { return false; }
	};

	class PlasmaState final : public MyGameState
	{
		using Base = MyGameState;

	protected:
		static constexpr int Scale = 8;

		Owned<TTF_Font>    _font;
		Owned<SDL_Texture> _blendedText;
		Owned<SDL_Surface> _plasmaSrf;
		Owned<SDL_Texture> _plasmaTex;

		SDL_FPoint _blendedTextSize = { 0, 0 };
		i32        _brightness      = 160;

	public:
		// ctor
		using Base::Base;

		void Init() override;
		void Destroy() override;

		bool StatefulInput() override;
		bool Input( const SDL_Event & event ) override;
		void Update( u64 framesSinceStart, Duration timeSinceStart, f32 deltaT       ) override;
		void Render( u64 framesSinceStart, Duration timeSinceStart, f32 deltaTNeeded ) override;
	};

	class SortState final : public MyGameState
	{
		using Base = MyGameState;

	protected:
		struct Ball { f32 x, y, z, w; };

		Owned<SDL_Texture> _image;
		DynArray<Ball> _balls;

		bool _isOrdered     = false;
		bool _isTransparent = false;
		bool _isDarkened    = false;

	public:
		// ctor
		using Base::Base;

		void Init() override;
		void Destroy() override;

		bool Input( const SDL_Event & event ) override;
		void Update( u64 framesSinceStart, Duration timeSinceStart, f32 deltaT       ) override;
		void Render( u64 framesSinceStart, Duration timeSinceStart, f32 deltaTNeeded ) override;
	};

	class CameraState : public MyGameState
	{
		using Base = MyGameState;

	protected:
		static constexpr Array<SDL_FPoint,4> BackgroundStartOffset = {{
			{ 0,    -330 },
			{ -350, -330 },
			{ -450, -900 },
			{ -800, -1500 },
		}};
		static constexpr Array<SDL_FPoint,4> BackgroundFactor = {{
			{ 0.2f, 0.3f },
			{ 0.4f, 0.45f },
			{ 0.8f, 0.8f },
			{ 1.2f, 1.2f },
		}};

		Array<Owned<SDL_Texture>,4> _background;
		Array<SDL_FPoint,4>         _backgroundSize; // Is initialized in Init()

		SDL_FPoint _mouseOffset      = { 0, 0 };
		SDL_FPoint _mouseOffsetEased = { 0, 0 };
		SDL_FPoint _direction        = { 0, 0 };
		SDL_FPoint _camera           = { 0, 0 };

		Array<bool,4> _isBackgroundVisible = {
			true,
			true,
			true,
			true,
		};
		bool _isInverted = false;
		bool _isEased    = true;
		bool _isFlux     = true;

	public:
		// ctor
		using Base::Base;

		void Init() override;
		void Destroy() override;

		bool StatefulInput() override;
		bool Input( const SDL_Event & event ) override;
		void Update( u64 framesSinceStart, Duration timeSinceStart, f32 deltaT       ) override;
		void Render( u64 framesSinceStart, Duration timeSinceStart, f32 deltaTNeeded ) override;

		[[nodiscard]] SDL_FPoint CalcFluxCam( Duration timeSinceStart ) const;
		void RenderLayer( SDL_Point windowSize, SDL_FPoint camPos, int index ) const;
	};

	class ShooterState final : public CameraState
	{
		using Base = CameraState;

	protected:
		static constexpr int SatRadius = 25;

		Owned<MIX_Audio>               _sound;
		Array<Owned<MIX_Track>,8>      _tracksSound;
		Array<Owned<SDL_Texture>,4>    _projectile;
		//ReuseFPoints rvProjectiles;

		usize                          _currTrack = 0;

		DynArray<SDL_FPoint>           _enemyProjectiles;
		DynArray<SDL_FPoint>::iterator _enemyProjReuse;
		int                            _numDeadEnemyProj = 0;

		DynArray<SDL_FPoint>           _myProjectiles;
		DynArray<SDL_FPoint>::iterator _myProjReuse;
		int                            _numDeadMyProj = 0;

		DynArray<SDL_FRect> _enemies = {
			{ 1100, 280, 160, 160 },
			{ 1200, 500,  80,  80 },
			{ 1100, 680,  80,  80 },
		};

		SDL_FRect           _player     = { 200, 530, 200, 100 };
		Array<SDL_FPoint,5> _satellites = { { { 0, 0 } } };

		f32      _progress      = 0;
		Duration _shootCooldown = {};

		SDL_FPoint _spawnProjectileAt;           // Valid if x and y is >= 0
		Duration   _spawnProjectileSoundCD = {};

	public:
		// ctor
		using Base::Base;

		void Init() override;
		void Destroy() override;

		bool Input( const SDL_Event & event ) override;
		void Update( u64 framesSinceStart, Duration timeSinceStart, f32 deltaT       ) override;
		void Render( u64 framesSinceStart, Duration timeSinceStart, f32 deltaTNeeded ) override;

		//bool isFPSLimited() const noexcept override { return false; }

		[[nodiscard]]
		bool IsProjectileAlive(    const DynArray<SDL_FPoint>::iterator & it ) const;
		void SpawnEnemyProjectile( SDL_FPoint pos );
		void SpawnMyProjectile(    SDL_FPoint pos );
		void RetireProjectile(     const DynArray<SDL_FPoint>::iterator & it );
		void RetireMyProjectile(   const DynArray<SDL_FPoint>::iterator & it );
	};
}
