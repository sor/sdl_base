#include "sdl_core.hpp"

// This one has a toxic include on SDL_syswm,
// which pollutes global namespace with a lot of senseless X11 typedefs
// therefore only include in this .cpp file
#include <nfd_sdl2.h>

// Monkey-patching NFD
namespace NFD
{
	// SDL_Window as pointer because it's the case for all other functions as well, else by reference would be preferred
	// Return by value intentional
	Window GetNativeWindowFromSDLWindow( SDL_Window * sdlWindow )
	{
		Window nativeWindow;
		NFD_GetNativeWindowFromSDLWindow( sdlWindow, &nativeWindow ); // Intentionally discard return value
		return nativeWindow;
	}
}

static_assert( std::is_same_v<JanSordid::Core::i8,   Sint8>  );
static_assert( std::is_same_v<JanSordid::Core::i16,  Sint16> );
static_assert( std::is_same_v<JanSordid::Core::i32,  Sint32> );
static_assert( std::is_same_v<JanSordid::Core::i64,  Sint64> );
static_assert( std::is_same_v<JanSordid::Core::u8,   Uint8>  );
static_assert( std::is_same_v<JanSordid::Core::u16,  Uint16> );
static_assert( std::is_same_v<JanSordid::Core::u32,  Uint32> );
static_assert( std::is_same_v<JanSordid::Core::u64,  Uint64> );
