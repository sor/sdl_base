#pragma once

#include "../global.hpp"

#include <sor/sdl_game.hpp>
#include <sor/sdl_shapeops.hpp>

#include <hsnr64/offset.hpp>
#include <hsnr64/palette.hpp>

namespace JanSordid::SDL_Example
{
	using namespace JanSordid;
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
		// "Invalid" is a special value, do NOT reproduce in your own game,
		// its only purpose is to show how hard it would be to crash the game
		Invalid,
	};


	// Only create this intermediary class if you want to add something to ALL of your GameStates,
	// else let them derive from SDL::GameState<MyGame> directly
	// abstract
	class MyGameState : public SDL::GameState<MyGame>
	{
		using Base = GameState;

	protected:
		static constexpr const Color White = HSNR64::Palette( HSNR64::NamedColor::White );
		static constexpr const Color Black = HSNR64::Palette( HSNR64::NamedColor::Black );

	public:
		// ctor
		using Base::Base;
	};

	class MyGame final : public SDL::Game<MyGameState,MyGS>
	{
		using Base = Game;

	public:
		MyGame();

		bool HandleEvent( const Event & event ) override;
	};


	class IntroState final : public MyGameState
	{
		using Base = MyGameState;

	protected:
		Owned<Font>    _font;
		Owned<Texture> _image;
		Owned<Music>   _music;
		Owned<Chunk>   _sound;
		Owned<Texture> _blendedText;

		Point _blendedTextSize = { 0, 0 };
		u8    _textmode        = 0;

		// Testvars controlled by ImGui
		Point _textStartPoint = { 20, 30 };
		i32   _colorIndex     = 9;
		bool  _isDarkOutline  = true;

	public:
		/// Ctors & Dtor
		using Base::Base;

		/// Getters & Setters: non-virtual first, followed by (pure) virtual/override
		[[nodiscard]] constexpr Color clearColor() const noexcept override { return White; }

		/// Methods: non-virtual first, followed by (pure) virtual/override
		void Init()                     override;
		void Enter( bool isStacking )   override;
		void Exit( bool isStackedUpon ) override;
		void Destroy()                  override;

		bool HandleEvent( const Event & event ) override;
		void Update( u64 framesSinceStart, u64 msSinceStart, f32 deltaT       ) override;
		void Render( u64 framesSinceStart, u64 msSinceStart, f32 deltaTNeeded ) override;
		ImGuiOnly(
			void RenderUI( u64 framesSinceStart, u64 msSinceStart, f32 deltaTNeeded ) override;)
	};

	class PlasmaState final : public MyGameState
	{
		using Base = MyGameState;

	protected:
		static constexpr const int Scale = 8;

		Owned<Font>    _font;
		Owned<Texture> _blendedText;
		Owned<Surface> _plasmaSrf;
		Owned<Texture> _plasmaTex;

		Point _blendedTextSize = { 0, 0 };
		i32   _brightness      = 160;

	public:
		// ctor
		using Base::Base;

		void Init() override;
		void Destroy() override;

		bool Input() override;
		bool HandleEvent( const Event & event ) override;
		void Update( u64 framesSinceStart, u64 msSinceStart, f32 deltaT       ) override;
		void Render( u64 framesSinceStart, u64 msSinceStart, f32 deltaTNeeded ) override;
	};

	class SortState final : public MyGameState
	{
		using Base = MyGameState;

	protected:
		struct Ball { f32 x, y, z, w; };

		Owned<Texture> _image;
		Vector<Ball>   _balls;

		bool _isOrdered     = false;
		bool _isTransparent = false;
		bool _isDarkened    = false;

	public:
		// ctor
		using Base::Base;

		void Init() override;
		void Destroy() override;

		bool HandleEvent( const Event & event ) override;
		void Update( u64 framesSinceStart, u64 msSinceStart, f32 deltaT       ) override;
		void Render( u64 framesSinceStart, u64 msSinceStart, f32 deltaTNeeded ) override;
	};

	class CameraState : public MyGameState
	{
		using Base = MyGameState;

	protected:
		static constexpr const Array<FPoint,4> BackgroundStartOffset = {{
			{ 0,    -330 },
			{ -350, -330 },
			{ -450, -900 },
			{ -800, -1500 },
		}};
		static constexpr const Array<FPoint,4> BackgroundFactor = {{
			{ 0.2f, 0.3f },
			{ 0.4f, 0.45f },
			{ 0.8f, 0.8f },
			{ 1.2f, 1.2f },
		}};

		Array<Owned<Texture>,4> _background;
		Array<Point,4>          _backgroundSize; // Is initialized in Init()

		FPoint _mouseOffset      = { 0, 0 };
		FPoint _mouseOffsetEased = { 0, 0 };
		FPoint _direction        = { 0, 0 };
		FPoint _camera           = { 0, 0 };

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

		bool Input() override;
		bool HandleEvent( const Event & event ) override;
		void Update( u64 framesSinceStart, u64 msSinceStart, f32 deltaT       ) override;
		void Render( u64 framesSinceStart, u64 msSinceStart, f32 deltaTNeeded ) override;

		FPoint CalcFluxCam( u64 totalMSec ) const;
		void RenderLayer( Point windowSize, FPoint camPos, int index) const;
	};

	class ShooterState final : public CameraState
	{
		using Base = CameraState;

	protected:
		static constexpr const int SatRadius = 25;

		Owned<Chunk>            _sound;
		Array<Owned<Texture>,4> _projectile;
		//ReuseFPoints rvProjectiles;

		Vector<FPoint>           _enemyProjectiles;
		Vector<FPoint>::iterator _enemyProjReuse;
		int                      _numDeadEnemyProj = 0;

		Vector<FPoint>           _myProjectiles;
		Vector<FPoint>::iterator _myProjReuse;
		int                      _numDeadMyProj = 0;

		Vector<FRect> _enemies = {
			{ 1100, 280, 160, 160 },
			{ 1200, 500,  80,  80 },
			{ 1100, 680,  80,  80 },
		};

		FRect           _player     = { 200, 530, 200, 100 };
		Array<FPoint,5> _satellites = { { { 0, 0 } } };

		f32    _progress      = 0;
		u64    _shootCooldown = 0;

		FPoint _spawnProjectileAt;           // Valid if x and y is >= 0
		u64    _spawnProjectileSoundCD = 0;  //

	public:
		// ctor
		using Base::Base;

		void Init() override;
		void Destroy() override;

		bool HandleEvent( const Event & event ) override;
		void Update( u64 framesSinceStart, u64 msSinceStart, f32 deltaT       ) override;
		void Render( u64 framesSinceStart, u64 msSinceStart, f32 deltaTNeeded ) override;

		//bool isFPSLimited() const noexcept override { return false; }

		[[nodiscard]]
		bool IsProjectileAlive(    const Vector<FPoint>::iterator & it ) const;
		void SpawnEnemyProjectile( const FPoint pos );
		void SpawnMyProjectile(    const FPoint pos );
		void RetireProjectile(     const Vector<FPoint>::iterator & it );
		void RetireMyProjectile(   const Vector<FPoint>::iterator & it );
	};
}
