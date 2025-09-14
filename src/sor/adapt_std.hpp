#pragma once

/// Polyfills for unavailable and disabled functionality

#if defined( __cpp_lib_format )
	#include <format>
#else
	#include <fmt/format.h> // https://fmt.dev/latest/index.html
#endif

#if defined( __cpp_lib_print )
	#include <print>
#else
	#include <fmt/base.h>   // https://fmt.dev/latest/index.html
#endif

#if ! defined( __cpp_rtti )
	// If -fno-rtti, try replacing dynamic_cast with static_cast
	// If this works, then the original code should probably be changed
	#define dynamic_cast static_cast
#endif

#if ! defined( __cpp_exceptions )
	// If -fno-exceptions, transform error handling code to work without it.
	#define try      if constexpr( true )
	#define catch(X) if constexpr( false )
	#define throw    /* HACK: Come up with a solution for throw */
#endif

namespace std
{
#if ! defined( __cpp_lib_to_underlying )
	template<typename TEnum>
	constexpr std::underlying_type_t<TEnum> to_underlying( TEnum e ) noexcept
	{
		return static_cast<std::underlying_type_t<TEnum>>( e );
	}
#endif
}

namespace JanSordid::Core
{
	// String formatting
	#if defined( __cpp_lib_format )
		using std::format, std::format_string;
	#else
		using fmt::format, fmt::format_string;
	#endif

	#if defined( __cpp_lib_print )
		using std::print, std::println;
	#else
		using fmt::print, fmt::println;
	#endif
}
