#pragma once

#include "sor/core.hpp"
#include "sor/sdl_core.hpp"

/*
 * ~ Jan Sordid's ~
 *     ~ Hybrid Sprite Navigator Revolution 64 ~
 *                             ~ Editor / Framework ~
 */
namespace JanSordid::HSNR64
{
	using namespace JanSordid::SDL;

	namespace ShadowOffset
	{
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
			1_up   + 1_left,
			1_up   + 1_right,
			1_down + 1_left,
			1_down + 1_right,
			2_right,
			2_left,
			2_up,
			2_down,
			*/
		};

		/**
		 * Square shaped shadow
		 * (extra safe and not as bold as the Rhombus)
		 *
		 *   xxx
		 *   xox
		 *   xxx
		 */

		static constexpr const Array<Point, 8> Square = {
			//*
			Point { -1, -1 },
			Point { +1, -1 },
			Point { -1, +1 },
			Point { +1, +1 },
			Point { +0, +1 },
			Point { +1, +0 },
			Point { +0, -1 },
			Point { -1, +0 },
			/*/
			1_up   + 1_left,
			1_up   + 1_right,
			1_down + 1_left,
			1_down + 1_right,
			1_right,
			1_left,
			1_up,
			1_down,
			*/
		};

		/**
		 * Plus shaped shadow
		 *
		 *    x
		 *   xox
		 *    x
		 */

		static constexpr const Array<Point, 4> Plus = {
			//*
			Point { +0, +1 },
			Point { +1, +0 },
			Point { +0, -1 },
			Point { -1, +0 },
			/*/
			1_right,
			1_left,
			1_up,
			1_down,
			*/
		};

		/**
		 * Cross (X) shaped shadow
		 *
		 *   x x
		 *    o
		 *   x x
		 */
		static constexpr const Array<Point, 4> Cross = {
			//*
			Point { -1, -1 },
			Point { +1, -1 },
			Point { -1, +1 },
			Point { +1, +1 },
			/*/
			1_up   + 1_left,
			1_up   + 1_right,
			1_down + 1_left,
			1_down + 1_right,
			*/
		};
	}
}
