#include "adapt_sdl_nfd.hpp"

// This following include has a toxic include on SDL_syswm,
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
