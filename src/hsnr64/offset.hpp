#pragma once

#include "sor/core.hpp"
#include "sor/sdl_core.hpp"

//#include "sor/sdl_shapeops.hpp"

namespace JanSordid::HSNR64
{
	using namespace JanSordid::SDL;

	namespace ShadowOffset
	{
		//using namespace JanSordid::SDL::PointLiterals;

		/**
		 * For Rhombus / Diamond shaped shadows
		 * (single pixels might be problematic, if they are singular in both dimensions)
		 *    x
		 *   x x
		 *  x o x
		 *   x x
		 *    x
		 */
		static constexpr const Array<FPoint, 8> Rhombus = {
			//*
			FPoint { -1, -1 },
			FPoint { +1, -1 },
			FPoint { -1, +1 },
			FPoint { +1, +1 },
			FPoint { +0, +2 },
			FPoint { +2, +0 },
			FPoint { +0, -2 },
			FPoint { -2, +0 },
			/*/
			1.0_up   + 1.0_left,
			1.0_up   + 1.0_right,
			1.0_down + 1.0_left,
			1.0_down + 1.0_right,
			2.0_right,
			2.0_left,
			2.0_up,
			2.0_down,
			//*/
		};

		/**
		 * Square shaped shadow
		 * (extra safe and not as bold as the Rhombus)
		 *
		 *   xxx
		 *   xox
		 *   xxx
		 */

		static constexpr const Array<FPoint, 8> Square = {
			//*
			FPoint { -1, -1 },
			FPoint { +1, -1 },
			FPoint { -1, +1 },
			FPoint { +1, +1 },
			FPoint { +0, +1 },
			FPoint { +1, +0 },
			FPoint { +0, -1 },
			FPoint { -1, +0 },
			/*/
			1.0_up   + 1.0_left,
			1.0_up   + 1.0_right,
			1.0_down + 1.0_left,
			1.0_down + 1.0_right,
			1.0_right,
			1.0_left,
			1.0_up,
			1.0_down,
			//*/
		};

		/**
		 * Plus shaped shadow
		 *
		 *    x
		 *   xox
		 *    x
		 */

		static constexpr const Array<FPoint, 4> Plus = {
			//*
			FPoint { +0, +1 },
			FPoint { +1, +0 },
			FPoint { +0, -1 },
			FPoint { -1, +0 },
			/*/
			1.0_right,
			1.0_left,
			1.0_up,
			1.0_down,
			//*/
		};

		/**
		 * Cross (X) shaped shadow
		 *
		 *   x x
		 *    o
		 *   x x
		 */
		static constexpr const Array<FPoint, 4> Cross = {
			//*
			FPoint { -1, -1 },
			FPoint { +1, -1 },
			FPoint { -1, +1 },
			FPoint { +1, +1 },
			/*/
			1.0_up   + 1.0_left,
			1.0_up   + 1.0_right,
			1.0_down + 1.0_left,
			1.0_down + 1.0_right,
			//*/
		};
	}
}
