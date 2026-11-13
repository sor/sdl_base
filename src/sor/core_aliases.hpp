#pragma once

namespace JanSordid::Core
{
	/// Aliases of a lot of std:: for easy usage

//	namespace ComplexLiterals    { using namespace std::complex_literals;     }
	namespace Chrono             { using namespace std::chrono;               }
	namespace ChronoLiterals     { using namespace std::chrono_literals;      }
	namespace Numbers            { using namespace std::numbers;              }
	namespace StringLiterals     { using namespace std::string_literals;      }
	namespace StringViewLiterals { using namespace std::string_view_literals; }

	// Classes / Structs
	using String        = std::string;
	using StringView    = std::string_view;

	using NanoSec       = Chrono::nanoseconds;                     // Nanoseconds
	using MilliSec      = Chrono::milliseconds;                    // Milliseconds
	using Sec           = Chrono::seconds;                         // Seconds
	using FNanoSec      = Chrono::duration<f32, NanoSec::period>;  // Nanoseconds as float
	using FMilliSec     = Chrono::duration<f32, MilliSec::period>; // Milliseconds as float
	using FSec          = Chrono::duration<f32, Sec::period>;      // Seconds as float
	// SDL provides the timestamps (nanoseconds since its epoch, see Clock in sdl_core.hpp), chrono the durations
	// A TimePoint is therefore just a Duration relative to SDL's epoch
	using TimePoint     = NanoSec;
	using Duration      = NanoSec;

	using File          = std::FILE;

	// Math Functions
	using std::min, std::max;
	using std::abs, std::sin, std::cos;
	using std::floor, std::ceil, std::round, std::lround, std::llround;

	// Chrono Functions
	using Chrono::duration_cast;

	// Functions
	using std::move, std::forward; // Clang does not like this, decide for yourself
	using std::make_unique, std::make_shared;

	// Templates
	template<typename T, usize Size>            using Array     = std::array<T, Size>;
	template<typename T>                        using DynArray  = std::vector<T>;
//	template<typename T>                        using Vector    = std::vector<T>;
	template<typename T>                        using HashSet   = std::unordered_set<T>;
	template<typename TKey, typename TValue>    using HashMap   = std::unordered_map<TKey,TValue>;

	template<typename T>                                           using RawPtr    = T*;
	template<typename T, typename TDel = std::default_delete<T>>   using UniquePtr = std::unique_ptr<T,TDel>;
	template<typename T>                                           using SharedPtr = std::shared_ptr<T>;
	template<typename T>                                           using WeakPtr   = std::weak_ptr<T>;
}
