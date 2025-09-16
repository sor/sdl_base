#pragma once

#include <memory>

#include "sdl_core.hpp"

// It is allowed to specialize std classes, but this might be UB?!
namespace std
{
#define IMPLEMENT_DEFAULT_DELETE( CLASS, DELETE_FUNC )                                       \
	template <> struct default_delete<CLASS> {                                               \
		default_delete() = default;                                                          \
		template <class OTHER> constexpr default_delete( default_delete<OTHER> ) noexcept {} \
		void operator()( CLASS * p ) const noexcept {                                        \
			/*fmt::print("del {}\n",#DELETE_FUNC);*/                                             \
			DELETE_FUNC( p );                                                                \
		}                                                                                    \
	}

	IMPLEMENT_DEFAULT_DELETE( SDL_Window,      SDL_DestroyWindow   );
	IMPLEMENT_DEFAULT_DELETE( SDL_Renderer,    SDL_DestroyRenderer );
	IMPLEMENT_DEFAULT_DELETE( SDL_Texture,     SDL_DestroyTexture  );
	IMPLEMENT_DEFAULT_DELETE( SDL_Surface,     SDL_DestroySurface  );
	IMPLEMENT_DEFAULT_DELETE( SDL_Palette ,    SDL_DestroyPalette  );
	IMPLEMENT_DEFAULT_DELETE( TTF_Font,        TTF_CloseFont       );
	IMPLEMENT_DEFAULT_DELETE( TTF_Text,        TTF_DestroyText     );
	IMPLEMENT_DEFAULT_DELETE( TTF_TextEngine,  TTF_DestroyRendererTextEngine ); // Care: Might be a different kind of Engine
	IMPLEMENT_DEFAULT_DELETE( Mix_Chunk,       Mix_FreeChunk       );
	IMPLEMENT_DEFAULT_DELETE( Mix_Music,       Mix_FreeMusic       );

#undef IMPLEMENT_DEFAULT_DELETE
}

namespace JanSordid::Core
{
	/** CARE:
	 * A deleted templated base func with specialized implementations,
	 * disallows derived types as parameters for those specializations,
	 * which is NOT a problem for most C libraries like SDL
	 */

	/*
	// Usage: auto blah = Uniq( tex ); // blah will be of type UniquePtr<Renderer> with the correct deleter assigned
	template <typename T>
	UniquePtr<T> Uniq( T * p )
#ifdef UNIQ_ONLY_FOR_SDL
	= delete;
#else
	{ return UniquePtr<T> { p }; }
#endif

	template<> inline UniquePtr<SDL_Window>      Uniq(  SDL_Window      * p ) { return UniquePtr<SDL_Window>     { p }; }
	template<> inline UniquePtr<SDL_Renderer>    Uniq(  SDL_Renderer    * p ) { return UniquePtr<SDL_Renderer>   { p }; }
	template<> inline UniquePtr<SDL_Texture>     Uniq(  SDL_Texture     * p ) { return UniquePtr<SDL_Texture>    { p }; }
	template<> inline UniquePtr<SDL_Surface>     Uniq(  SDL_Surface     * p ) { return UniquePtr<SDL_Surface>    { p }; }
	template<> inline UniquePtr<SDL_PixelFormat> Uniq(  SDL_PixelFormat * p ) { return UniquePtr<SDL_PixelFormat>{ p }; }
	template<> inline UniquePtr<SDL_Palette>     Uniq(  SDL_Palette     * p ) { return UniquePtr<SDL_Palette>    { p }; }
	template<> inline UniquePtr<TTF_Font>        Uniq(  TTF_Font        * p ) { return UniquePtr<TTF_Font>       { p }; }
	template<> inline UniquePtr<Mix_Chunk>       Uniq(  Mix_Chunk       * p ) { return UniquePtr<Mix_Chunk>      { p }; }
	template<> inline UniquePtr<Mix_Music>       Uniq(  Mix_Music       * p ) { return UniquePtr<Mix_Music>      { p }; }

	// Usage: auto blah = Share( tex ); // blah will be of type SharedPtr<Texture> with the correct deleter assigned
	template <typename T>
	SharedPtr<T> Share( T * p )
#ifdef SHARE_ONLY_FOR_SDL
	= delete;
#else
	{ return SharedPtr<T> { p }; }
#endif

	template<> inline SharedPtr<SDL_Window>      Share( SDL_Window      * p ) { return SharedPtr<SDL_Window>     { p, &SDL_DestroyWindow   }; }
	template<> inline SharedPtr<SDL_Renderer>    Share( SDL_Renderer    * p ) { return SharedPtr<SDL_Renderer>   { p, &SDL_DestroyRenderer }; }
	template<> inline SharedPtr<SDL_Texture>     Share( SDL_Texture     * p ) { return SharedPtr<SDL_Texture>    { p, &SDL_DestroyTexture  }; }
	template<> inline SharedPtr<SDL_Surface>     Share( SDL_Surface     * p ) { return SharedPtr<SDL_Surface>    { p, &SDL_FreeSurface     }; }
	template<> inline SharedPtr<SDL_PixelFormat> Share( SDL_PixelFormat * p ) { return SharedPtr<SDL_PixelFormat>{ p, &SDL_FreeFormat      }; }
	template<> inline SharedPtr<SDL_Palette>     Share( SDL_Palette     * p ) { return SharedPtr<SDL_Palette>    { p, &SDL_FreePalette     }; }
	template<> inline SharedPtr<TTF_Font>        Share( TTF_Font        * p ) { return SharedPtr<TTF_Font>       { p, &TTF_CloseFont       }; }
	template<> inline SharedPtr<Mix_Chunk>       Share( Mix_Chunk       * p ) { return SharedPtr<Mix_Chunk>      { p, &Mix_FreeChunk       }; }
	template<> inline SharedPtr<Mix_Music>       Share( Mix_Music       * p ) { return SharedPtr<Mix_Music>      { p, &Mix_FreeMusic       }; }
	*/
}
