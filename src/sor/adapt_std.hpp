#pragma once

/// Polyfills for unavailable or disabled functionality

#include <version> // Needed for the __cpp_lib_* defines
#include <utility>

#ifdef __cpp_lib_format
//#if __has_include(<format>)
	#include <format>
#else
	#include <fmt/format.h> // https://fmt.dev/latest/index.html
#endif

#ifdef __cpp_lib_print
//#if __has_include(<print>) // C++20 has the include file, but it does not define the functions we want
	#include <print>
#else
	#include <fmt/base.h>   // https://fmt.dev/latest/index.html
#endif

#ifndef __cpp_rtti
	// If -fno-rtti, try replacing dynamic_cast with static_cast
	// If this works, then the original code should probably be changed
	#define dynamic_cast static_cast
#endif

#ifndef __cpp_exceptions
	// If -fno-exceptions, transform error handling code to work without it.
	#define try      if constexpr( true )
	#define catch(X) if constexpr( false )
	#define throw    /* HACK: Come up with a solution for throw */
#endif


namespace JanSordid::Core
{
#ifdef __cpp_lib_to_underlying
	using std::to_underlying;
#else
	template<typename TEnum>
	[[nodiscard]]
	constexpr std::underlying_type_t<TEnum>
	to_underlying( TEnum e ) noexcept
	{
		return static_cast<std::underlying_type_t<TEnum>>( e );
	}
#endif

#ifdef __cpp_lib_format
	using std::format, std::format_string;

	// Exists in fmt:: but not in std::
	template <class... Args>
	[[nodiscard]] inline std::system_error
	system_error( const int error_code, const format_string<Args...> fmt, Args&&... args )
	{
		auto ec = std::error_code(error_code, std::generic_category());
		return std::system_error( ec, format( fmt, std::forward<Args>(args)... ) );
	}
#else
	using fmt::format, fmt::format_string, fmt::system_error;
#endif

#ifdef __cpp_lib_print
	using std::print, std::println;
#else
	using fmt::print, fmt::println;
#endif
}
