#include "tilefont.hpp"

#include <numeric>

//#pragma clang diagnostic push
//#pragma ide diagnostic ignored "cppcoreguidelines-avoid-magic-numbers"

namespace JanSordid::HSNR64
{
	const Array<const CharMetric,256> TileFont::CharMetrics = {
		CharMetric{ 99 },   // NUL
		CharMetric{ 99 },
		CharMetric{ 99 },
		CharMetric{ 99 },
		CharMetric{ 99 },
		CharMetric{ 99 },
		CharMetric{ 99 },
		CharMetric{ 99 },   // Alert
		CharMetric{ 99 },   // Backspace
		CharMetric{},       // Tab  (special handling?)
		CharMetric{},       // Newline
		CharMetric{},       // Vert Tab
		CharMetric{},       // Formfeed / Pagebreak
		CharMetric{},       // Carriage Return (useful to print on one line over and over)
		CharMetric{},
		CharMetric{},
		CharMetric{},
		CharMetric{},
		CharMetric{},
		CharMetric{},
		CharMetric{},
		CharMetric{},
		CharMetric{},
		CharMetric{},
		CharMetric{},
		CharMetric{},
		CharMetric{},
		CharMetric{},       // Escape
		CharMetric{},
		CharMetric{},
		CharMetric{},
		CharMetric{},

		CharMetric{  6,  0,  0, { 2, 1, 0, 0, 0, 0, 1, 2 }, { 2, 1, 0, 0, 0, 0, 1, 2 } },   // Space
		CharMetric{  5, -5,  0, { 3, 0, 0, 0, 1, 1, 1, 3 }, { 3, 0, 0, 0, 1, 1, 1, 3 } },   // !
		CharMetric{ 10, -3, -2, { 0, 0, 0, 1, 3, 3, 3, 3 }, { 0, 0, 0, 1, 3, 3, 3, 3 } },   // "
		CharMetric{ 13, -1,  0, { 3, 2, 2, 0, 0, 2, 2, 3 }, { 3, 2, 2, 0, 0, 2, 2, 3 } },   // #
		CharMetric{ 11, -2,  0, { 3, 3, 0, 1, 2, 0, 3, 3 }, { 3, 3, 0, 2, 0, 1, 3, 3 } },   // $
		CharMetric{ 13, -1,  0, { 3, 0, 0, 0, 0, 0, 0, 3 }, { 3, 0, 0, 0, 0, 0, 0, 3 } },   // %
		CharMetric{ 14, -1,  0, { 3, 1, 0, 1, 1, 0, 1, 3 }, { 5, 3, 2, 3, 3, 0, 0, 2 } },   // &
		CharMetric{  3, -6, -2, { 0, 0, 0, 1, 3, 3, 3, 3 }, { 0, 0, 0, 1, 3, 3, 3, 3 } },   // '
		CharMetric{  7, -4,  0, { 3, 3, 1, 0, 0, 1, 3, 3 }, { 3, 0, 1, 3, 3, 1, 0, 3 } },   // (
		CharMetric{  7, -4,  0, { 3, 0, 1, 3, 3, 1, 0, 3 }, { 3, 3, 1, 0, 0, 1, 3, 3 } },   // )
		CharMetric{ 14, -1,  0, { 6, 6, 6, 2, 0, 0, 2, 6 }, { 6, 6, 6, 2, 0, 0, 2, 6 } },   // *
		CharMetric{ 11, -2,  0, { 6, 6, 6, 4, 0, 0, 4, 6 }, { 6, 6, 6, 4, 0, 0, 4, 6 } },   // +
		CharMetric{  5, -5,  1, { 6, 6, 6, 6, 6, 2, 2, 0 }, { 6, 6, 6, 6, 6, 0, 0, 1 } },   // ,
		CharMetric{ 11, -2,  0, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },   // -
		CharMetric{  3, -6,  0, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },   // .
		CharMetric{ 13, -2,  0, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },   // /
		CharMetric{ 11, -2,  0, { 3, 1, 0, 0, 0, 0, 1, 3 }, { 3, 1, 0, 0, 0, 0, 1, 3 } },   // 0
		CharMetric{ 11, -2,  0, { 3, 3, 3, 2, 3, 3, 0, 3 }, { 3, 3, 3, 3, 3, 3, 0, 3 } },   // 1
		CharMetric{ 11, -2,  0, { 3, 1, 0, 3, 3, 1, 0, 3 }, { 3, 1, 0, 0, 1, 3, 0, 3 } },   // 2
		CharMetric{ 11, -2,  0, { 3, 1, 0, 3, 3, 0, 1, 3 }, { 3, 1, 0, 1, 1, 0, 1, 3 } },   // 3
		CharMetric{ 12, -2,  0, { 3, 3, 2, 0, 0, 3, 3, 3 }, { 3, 2, 2, 2, 0, 1, 2, 3 } },   // 4
		CharMetric{ 11, -2,  0, { 3, 0, 0, 0, 3, 0, 1, 3 }, { 3, 0, 2, 1, 0, 0, 1, 3 } },   // 5
		CharMetric{ 11, -2,  0, { 3, 1, 0, 0, 0, 0, 1, 3 }, { 3, 1, 0, 2, 1, 0, 1, 3 } },   // 6
		CharMetric{ 11, -2,  0, { 3, 0, 0, 3, 3, 3, 3, 3 }, { 3, 0, 0, 1, 3, 3, 3, 3 } },   // 7
		CharMetric{ 11, -2,  0, { 3, 1, 0, 1, 1, 0, 1, 3 }, { 3, 1, 0, 1, 1, 0, 1, 3 } },   // 8
		CharMetric{ 11, -2,  0, { 3, 1, 0, 1, 2, 0, 1, 3 }, { 3, 1, 0, 0, 0, 0, 1, 3 } },   // 9
		CharMetric{  3, -6,  0, { 3, 3, 2, 0, 1, 1, 0, 3 }, { 3, 3, 2, 0, 1, 1, 0, 3 } },   // :
		CharMetric{  5, -4,  1, { 5, 5, 4, 2, 3, 3, 2, 0 }, { 3, 3, 2, 0, 1, 1, 0, 1 } },   // ;
		CharMetric{  9, -3,  0, { 6, 6, 4, 2, 0, 2, 4, 6 }, { 6, 2, 0, 2, 4, 2, 0, 6 } },   // <
		CharMetric{ 11, -2,  0, { 6, 6, 6, 2, 0, 0, 2, 6 }, { 6, 6, 6, 2, 0, 0, 2, 6 } },   // =
		CharMetric{  9, -3,  0, { 6, 2, 0, 2, 4, 2, 0, 6 }, { 6, 6, 4, 2, 0, 2, 4, 6 } },   // >
		CharMetric{ 11, -2,  0, { 6, 1, 0, 1, 4, 4, 4, 6 }, { 6, 1, 0, 0, 2, 4, 4, 6 } },   // ?

		CharMetric{ 14, -1,  1, { 6, 2, 0, 0, 0, 0, 0, 1 }, { 6, 2, 0, 0, 0, 0, 1, 2 } },    // @
		CharMetric{ 11, -2,  0, { 6, 3, 1, 0, 0, 0, 0, 6 }, { 6, 3, 1, 0, 0, 0, 0, 6 }, 10, 0b0000'0000'0001'0010, 0b0001'0010'0000'0000 },
		CharMetric{ 11, -2,  0, { 6, 0, 0, 0, 0, 0, 0, 6 }, { 6, 2, 0, 1, 2, 0, 2, 6 }, 10, 0b0000'0000'0101'0010, 0b0000'0000'0000'0000 },
		CharMetric{ 11, -2,  0, { 6, 1, 0, 0, 0, 0, 1, 6 }, { 6, 1, 0, 4, 4, 0, 1, 6 }, 10, 0b0000'0000'0001'0000, 0b0000'0000'0000'0000 }, // C
		CharMetric{ 11, -2,  0, { 6, 0, 0, 0, 0, 0, 0, 6 }, { 6, 3, 1, 0, 0, 1, 3, 6 },  8, 0b0000'0000'0101'0010, 0b0001'1000'0000'0000 },
		CharMetric{ 11, -2,  0, { 6, 0, 0, 0, 0, 0, 0, 6 }, { 6, 0, 6, 6, 4, 6, 0, 6 },  8, 0b0000'0000'0101'0010, 0b0100'0010'0100'0010 },
		CharMetric{ 11, -2,  0, { 6, 0, 0, 0, 0, 0, 0, 6 }, { 6, 0, 6, 6, 4, 6, 6, 6 }, 10, 0b0000'0000'0100'1010, 0b0100'1000'0100'1000 }, // F
		CharMetric{ 11, -2,  0, { 6, 1, 0, 0, 0, 0, 1, 6 }, { 6, 1, 0, 2, 0, 0, 1, 6 }, 10, 0b0000'0000'0000'0000, 0b0000'1000'0000'0000 }, // G
		CharMetric{ 11, -2,  0, { 6, 0, 0, 0, 0, 0, 0, 6 }, { 6, 0, 0, 0, 0, 0, 0, 6 }, 10, 0b0000'0000'0100'0010, 0b0100'0010'0000'0000 },
		CharMetric{  7, -4,  0, { 6, 0, 2, 2, 2, 2, 0, 5 }, { 6, 0, 2, 2, 2, 2, 0, 6 }, 10, 0b0100'0010'0100'0010, 0b0100'0010'0100'0010 }, // I
		CharMetric{ 11, -2,  0, { 6, 4, 6, 6, 6, 0, 1, 6 }, { 6, 0, 2, 2, 2, 2, 3, 6 },  8, 0b0100'0100'0100'0100, 0b0101'0000'0100'0000 }, // J
		CharMetric{ 11, -2,  0, { 6, 0, 0, 0, 0, 0, 0, 6 }, { 6, 0, 1, 3, 3, 1, 0, 6 }, 10, 0b0000'0000'0100'0010, 0b0100'0010'0100'0010 }, // K
		CharMetric{ 11, -2,  0, { 6, 0, 0, 0, 0, 0, 0, 6 }, { 6, 6, 6, 6, 6, 6, 0, 6 },  8, 0b0000'0000'0100'0010, 0b0100'0010'0000'0010 }, // L
		CharMetric{ 13, -1,  0, { 6, 0, 0, 0, 0, 0, 0, 6 }, { 6, 0, 0, 0, 0, 0, 0, 6 }, 10, 0b0000'0000'0100'0010, 0b0100'0010'0000'0000 }, // M
		CharMetric{ 11, -2,  0, { 6, 0, 0, 0, 0, 0, 0, 6 }, { 6, 0, 0, 0, 0, 0, 0, 6 }, 10, 0b0000'0000'0100'0010, 0b0100'0010'0000'0000 }, // N
		CharMetric{ 11, -2,  0, { 6, 1, 0, 0, 0, 0, 1, 6 }, { 6, 1, 0, 0, 0, 0, 1, 6 }, 99, 0b0000'0000'0000'0000, 0b0000'0000'0000'0000 }, // O
		CharMetric{ 11, -2,  0, { 6, 0, 0, 0, 0, 0, 0, 6 }, { 6, 1, 0, 1, 6, 6, 6, 6 }, 10, 0b0000'0000'0100'0010, 0b0000'0000'0000'0000 }, // P
		CharMetric{ 11, -2,  0, { 6, 1, 0, 0, 0, 0, 1, 5 }, { 6, 1, 0, 0, 0, 0, 1, 0 }, 10, 0b0000'0000'0000'0000, 0b0000'0001'0000'0001 }, // Q
		CharMetric{ 11, -2,  0, { 6, 0, 0, 0, 0, 0, 0, 6 }, { 6, 1, 0, 0, 2, 2, 0, 6 }, 10, 0b0000'0000'0100'0010, 0b0000'0010'0000'0010 }, // R
		CharMetric{ 11, -2,  0, { 6, 1, 0, 0, 3, 0, 1, 6 }, { 6, 1, 0, 3, 1, 0, 1, 6 }, 10, 0b0000'0000'0000'0000, 0b0000'0000'0000'0000 }, // S
		CharMetric{ 11, -2,  0, { 6, 0, 4, 4, 4, 4, 4, 6 }, { 6, 0, 4, 4, 4, 4, 4, 6 }, 10, 0b0100'0000'0100'0000, 0b0100'0000'0100'0000 }, // T
		CharMetric{ 11, -2,  0, { 6, 0, 0, 0, 0, 0, 1, 6 }, { 6, 0, 0, 0, 0, 0, 1, 6 }, 10, 0b0000'0000'0100'0000, 0b0100'0000'0000'0000 }, // U
		CharMetric{ 11, -2,  0, { 6, 0, 0, 0, 0, 1, 3, 6 }, { 6, 0, 0, 0, 0, 1, 3, 6 }, 10, 0b0000'0000'0100'0000, 0b0100'0000'0000'0000 }, // V
		CharMetric{ 13, -1,  0, { 6, 0, 0, 0, 0, 0, 0, 6 }, { 6, 0, 0, 0, 0, 0, 0, 6 }, 10, 0b0000'0000'0100'0010, 0b0100'0010'0000'0000 }, // W
		CharMetric{ 11, -2,  0, { 6, 0, 0, 2, 2, 0, 0, 6 }, { 6, 0, 0, 2, 2, 0, 0, 6 }, 10, 0b0000'0000'0100'0010, 0b0100'0010'0000'0000 }, // X
		CharMetric{ 11, -2,  0, { 6, 0, 0, 2, 4, 4, 4, 6 }, { 6, 0, 0, 2, 4, 4, 4, 6 }, 10, 0b0000'0000'0100'0010, 0b0100'0010'0000'0000 }, // Y
		CharMetric{ 11, -2,  0, { 6, 0, 6, 4, 2, 0, 0, 6 }, { 6, 0, 0, 2, 4, 6, 0, 6 }, 10, 0b0100'0000'0100'0010, 0b0100'0010'0000'0010 }, // Z
		CharMetric{  5, -6, -1, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 2, 2, 2, 2, 0, 0 } },   // [
		CharMetric{ 13, -1,  0, { 3, 0, 1, 3, 4, 4, 4, 4 }, { 4, 4, 4, 4, 3, 1, 0, 3 } },   // \ ...
		CharMetric{  5, -6, -1, { 0, 0, 2, 2, 2, 2, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },   // ]
		CharMetric{ 11, -2, -2, { 3, 1, 0, 3, 4, 4, 4, 4 }, { 3, 1, 0, 3, 4, 4, 4, 4 } },   // ^
		CharMetric{ 11, -2,  0, { 4, 4, 4, 4, 4, 3, 1, 0 }, { 4, 4, 4, 4, 4, 3, 1, 0 } },   // _
		// pr,rp
		CharMetric{  7, -4, -2, { 0, 1, 1, 2, 3, 3, 3, 3 }, { 1, 0, 1, 2, 3, 3, 3, 3 } },    // `
		CharMetric{ 11, -2,  0, { 6, 6, 6, 1, 2, 0, 1, 6 }, { 6, 6, 6, 1, 0, 0, 0, 6 },  6, 0b0001'0000'0001'0000, 0b0000'0010'0000'0000 }, // a
		CharMetric{ 11, -2,  0, { 6, 0, 0, 0, 0, 0, 0, 6 }, { 6, 6, 6, 1, 0, 0, 1, 6 }, 10, 0b0000'0000'0101'0010, 0b0000'0000'0000'0000 }, // b
		CharMetric{ 10, -3,  0, { 6, 6, 6, 1, 0, 0, 1, 6 }, { 6, 6, 6, 0, 5, 5, 0, 6 },  5, 0b0000'0000'0000'0000, 0b0001'0010'0001'0010 }, // c
		CharMetric{ 11, -2,  0, { 6, 6, 6, 1, 0, 0, 1, 6 }, { 6, 0, 0, 0, 0, 0, 0, 6 },  8, 0b0000'0000'0000'0000, 0b0101'0010'0000'0000 }, // d
		CharMetric{ 11, -2,  0, { 6, 6, 6, 1, 0, 0, 1, 6 }, { 6, 6, 6, 1, 0, 2, 1, 6 },  5, 0b0000'0000'0000'0000, 0b0000'0010'0000'0010 }, // e
		CharMetric{  9, -3,  0, { 6, 3, 2, 0, 2, 2, 2, 6 }, { 6, 0, 3, 0, 4, 4, 4, 6 },  3, 0b0001'0000'0001'0000, 0b0101'0000'0101'0000 }, // f
		CharMetric{ 11, -2,  2, { 6, 6, 6, 1, 0, 0, 1, 0 }, { 6, 6, 6, 0, 0, 0, 0, 1 }, 10, 0b0000'0001'0000'0001, 0b0001'0010'0000'0000 }, // g
		CharMetric{ 11, -2,  0, { 6, 0, 0, 0, 0, 0, 0, 6 }, { 6, 6, 6, 1, 0, 0, 0, 6 },  6, 0b0000'0000'0101'0010, 0b0000'0010'0000'0000 }, // h
		CharMetric{  7, -4,  0, { 6, 2, 2, 0, 2, 2, 0, 6 }, { 6, 2, 2, 2, 2, 2, 0, 6 },  3, 0b0001'0010'0001'0010, 0b0001'0010'0000'0010 }, // i
		CharMetric{  9, -2,  2, { 6, 6, 6, 6, 6, 6, 6, 0 }, { 6, 6, 0, 0, 0, 0, 0, 1 },  8, 0b0000'0001'0001'0001, 0b0001'0010'0000'0000 }, // j
		CharMetric{ 10, -3,  0, { 6, 0, 0, 0, 0, 0, 0, 6 }, { 6, 6, 6, 0, 2, 2, 0, 6 },  6, 0b0000'0000'0100'0110, 0b0001'0010'0001'0010 }, // k
		CharMetric{  7, -4,  0, { 6, 0, 2, 2, 2, 2, 0, 6 }, { 6, 2, 2, 2, 2, 2, 0, 6 },  5, 0b0100'0010'0100'0010, 0b0000'0010'0000'0010 }, // l
		CharMetric{ 13, -1,  0, { 6, 6, 6, 0, 0, 0, 0, 6 }, { 6, 6, 6, 1, 0, 0, 0, 6 },  8, 0b0000'0000'0001'0010, 0b0000'0010'0000'0000 }, // m
		CharMetric{ 11, -2,  0, { 6, 6, 6, 0, 0, 0, 0, 6 }, { 6, 6, 6, 1, 0, 0, 0, 6 },  8, 0b0000'0000'0001'0010, 0b0000'0010'0000'0000 }, // n
		CharMetric{ 11, -2,  0, { 6, 6, 6, 1, 0, 0, 1, 6 }, { 6, 6, 6, 1, 0, 0, 1, 6 }, 99, 0b0000'0000'0000'0000, 0b0000'0000'0000'0000 }, // o
		CharMetric{ 11, -2,  2, { 6, 6, 6, 0, 0, 0, 0, 0 }, { 6, 6, 6, 1, 0, 0, 1, 6 }, 10, 0b0000'0000'0001'0010, 0b0000'0000'0000'0000 }, // p
		CharMetric{ 11, -2,  2, { 6, 6, 6, 1, 0, 0, 1, 6 }, { 6, 6, 6, 0, 0, 0, 0, 0 }, 10, 0b0000'0000'0000'0000, 0b0000'0000'0001'0010 }, // q
		CharMetric{ 11, -2,  0, { 6, 6, 6, 0, 0, 0, 0, 6 }, { 6, 6, 6, 1, 0, 6, 6, 6 }, 10, 0b0000'0000'0001'0010, 0b0000'1000'0000'1000 }, // r
		CharMetric{ 11, -2,  0, { 6, 6, 6, 1, 0, 2, 0, 6 }, { 6, 6, 6, 0, 2, 0, 1, 6 },  5, 0b0000'0010'0000'0010, 0b0001'0000'0001'0000 }, // s
		CharMetric{ 10, -3,  0, { 6, 3, 3, 0, 3, 3, 4, 6 }, { 6, 4, 4, 1, 4, 3, 0, 6 },  4, 0b0001'0000'0001'0000, 0b0001'0010'0001'0010 }, // t
		CharMetric{ 11, -2,  0, { 6, 6, 6, 0, 0, 0, 1, 6 }, { 6, 6, 6, 0, 0, 0, 0, 6 },  6, 0b0000'0000'0001'1000, 0b0001'0010'0000'0000 }, // u
		CharMetric{ 11, -2,  0, { 6, 6, 6, 0, 0, 1, 3, 6 }, { 6, 6, 6, 0, 0, 1, 3, 6 },  6, 0b0000'0000'0001'0000, 0b0001'0000'0000'0000 }, // v
		CharMetric{ 13, -1,  0, { 6, 6, 6, 0, 0, 1, 2, 6 }, { 6, 6, 6, 0, 0, 1, 2, 6 },  6, 0b0000'0000'0001'0000, 0b0001'0000'0000'0000 }, // w
		CharMetric{ 11, -2,  0, { 6, 6, 6, 0, 2, 2, 0, 6 }, { 6, 6, 6, 0, 2, 2, 0, 6 }, 10, 0b0000'0000'0001'0010, 0b0001'0010'0000'0000 }, // x
		CharMetric{ 11, -2,  2, { 6, 6, 6, 0, 0, 0, 1, 0 }, { 6, 6, 6, 0, 0, 0, 0, 1 }, 10, 0b0000'0001'0001'0001, 0b0001'0010'0000'0000 }, // y
		CharMetric{ 11, -2,  0, { 6, 6, 6, 0, 4, 2, 0, 6 }, { 6, 6, 6, 0, 2, 4, 0, 6 },  4, 0b0001'0010'0001'0010, 0b0001'0010'0001'0010 }, // z
		CharMetric{  9, -3, -1, { 4, 3, 3, 1, 0, 3, 3, 4 }, { 0, 0, 3, 3, 4, 3, 0, 0 } },   // {
		CharMetric{  3, -6, -1, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },   // |
		CharMetric{  9, -5, -1, { 0, 0, 3, 3, 4, 3, 0, 0 }, { 4, 3, 3, 1, 0, 3, 3, 4 } },   // }
		CharMetric{ 14, -1,  0, { 6, 6, 6, 0, 0, 6, 6, 6 }, { 6, 6, 6, 0, 0, 6, 6, 6 } },   // ~
		CharMetric{ 99,  0,  0, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },   // DEL - 99 width to catch accidental usage

		CharMetric{},
		CharMetric{},
		CharMetric{},
		CharMetric{},
		CharMetric{},
		CharMetric{},
		CharMetric{},
		CharMetric{},
		CharMetric{},
		CharMetric{},
		CharMetric{},
		CharMetric{},
		CharMetric{},
		CharMetric{},
		CharMetric{},
		CharMetric{},
		CharMetric{},
		CharMetric{},
		CharMetric{},
		CharMetric{},
		CharMetric{},
		CharMetric{},
		CharMetric{},
		CharMetric{},
		CharMetric{},
		CharMetric{},
		CharMetric{},
		CharMetric{},
		CharMetric{},
		CharMetric{},
		CharMetric{},
		CharMetric{},

		// TODO: fix all widths, kerning, etc below

		// \u00A0¡¢£¤¥¦§¨©ª«¬\u00AD®¯°±²³´µ¶·¸¹º»¼½¾¿
		CharMetric{  6,  0,  0, { 6, 1, 0, 0, 0, 0, 1, 6 }, { 2, 1, 0, 0, 0, 0, 1, 6 } },   // NBSP
		CharMetric{  5, -5,  0, { 6, 0, 0, 0, 0, 0, 0, 0 }, { 6, 0, 0, 0, 0, 0, 0, 0 } },   // !
		CharMetric{ 10, -3,  0, { 6, 0, 0, 0, 0, 0, 0, 0 }, { 6, 0, 0, 0, 0, 0, 0, 0 } },   // c
		CharMetric{ 13, -1,  0, { 6, 0, 0, 0, 0, 0, 0, 0 }, { 6, 0, 0, 0, 0, 0, 0, 0 } },   // pound
		CharMetric{ 11, -2,  0, { 6, 0, 0, 0, 0, 0, 0, 0 }, { 6, 0, 0, 0, 0, 0, 0, 0 } },   // currency
		CharMetric{ 13, -2,  0, { 6, 0, 0, 0, 0, 0, 0, 0 }, { 6, 0, 0, 0, 0, 0, 0, 0 } },   // Yen
		CharMetric{  5, -6, -1, { 6, 0, 0, 0, 0, 0, 0, 0 }, { 6, 0, 0, 0, 0, 0, 0, 0 } },   // | broken
		CharMetric{ 12, -2,  0, { 6, 0, 0, 0, 0, 0, 0, 0 }, { 6, 0, 0, 0, 0, 0, 0, 0 } },   // §
		CharMetric{  9, -3, -2, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },   // "
		CharMetric{ 14, -1, -1, { 1, 0, 0, 0, 0, 0, 0, 1 }, { 1, 0, 0, 0, 0, 0, 0, 1 } },   // (C)
		CharMetric{  8, -4, -2, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },   // ^a
		CharMetric{ 13, -1,  0, { 0, 2, 0, 0, 0, 0, 2, 6 }, { 0, 0, 0, 3, 0, 0, 0, 6 } },   // <<
		CharMetric{ 11, -2,  0, { 0, 3, 0, 0, 0, 0, 3, 6 }, { 0, 3, 0, 0, 0, 0, 0, 6 } },   // not
		CharMetric{  9, -3,  0, { 0, 3, 0, 0, 0, 0, 3, 6 }, { 0, 3, 0, 0, 0, 0, 3, 6 } },   // shy
		CharMetric{ 14, -1, -1, { 1, 0, 0, 0, 0, 0, 0, 1 }, { 1, 0, 0, 0, 0, 0, 0, 1 } },   // (R)
		CharMetric{ 13, -2,  0, { 0, 2, 4, 6, 6, 6, 6, 6 }, { 0, 2, 4, 6, 6, 6, 6, 6 } },   // up_
		CharMetric{  8, -4, -2, { 0, 0, 0, 1, 3, 6, 6, 6 }, { 0, 0, 0, 1, 3, 6, 6, 6 } },   // degree ^o
		CharMetric{ 11, -2,  0, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },   // plusminus
		CharMetric{  8, -4, -2, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },   // ^2
		CharMetric{  9, -3, -2, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },   // ^3
		CharMetric{  8, -5, -2, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },   // ´
		CharMetric{ 11, -2,  0, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },   // müh u
		CharMetric{ 12, -2,  0, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },   // linebreak P
		CharMetric{  5, -3,  0, { 6, 2, 0, 0, 0, 0, 2, 6 }, { 6, 2, 0, 0, 0, 0, 2, 6 } },   // dot
		CharMetric{ 11, -2,  2, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },   // ,
		CharMetric{  7, -4, -2, { 0, 0, 0, 0, 0, 0, 0, 6 }, { 0, 0, 0, 0, 0, 0, 0, 6 } },   // ^1
		CharMetric{ 10, -3, -2, { 0, 0, 0, 0, 0, 0, 0, 6 }, { 0, 0, 0, 0, 0, 0, 0, 6 } },   // ^o
		CharMetric{ 13, -1,  0, { 0, 0, 0, 3, 0, 0, 0, 6 }, { 0, 2, 0, 0, 0, 0, 2, 6 } },   // >>
		CharMetric{ 14, -1,  0, { 0, 0, 0, 0, 0, 0, 0, 3 }, { 0, 0, 0, 0, 0, 0, 0, 3 } },   // 1/4
		CharMetric{ 14, -1,  0, { 0, 0, 0, 0, 0, 0, 0, 3 }, { 0, 0, 0, 0, 0, 0, 0, 3 } },   // 1/2
		CharMetric{ 14, -1,  0, { 0, 0, 0, 0, 0, 0, 0, 3 }, { 0, 0, 0, 0, 0, 0, 0, 3 } },   // 3/4
		CharMetric{ 11, -2,  0, { 0, 4, 0, 0, 0, 0, 0, 3 }, { 0, 4, 0, 4, 0, 0, 0, 3 } },   // ?

		// ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞß
		CharMetric{ 11, -2, -2, { 0, 1, 0, 0, 0, 0, 0, 1 }, { 0, 1, 0, 0, 0, 0, 0, 1 } },   // À
		CharMetric{ 11, -2, -2, { 0, 3, 0, 0, 0, 0, 0, 6 }, { 0, 3, 0, 0, 0, 0, 0, 6 } },
		CharMetric{ 11, -2, -2, { 0, 0, 0, 0, 0, 0, 0, 6 }, { 0, 2, 0, 0, 0, 0, 0, 6 } },
		CharMetric{ 11, -2, -2, { 0, 1, 0, 0, 0, 0, 0, 6 }, { 0, 1, 0, 0, 0, 0, 0, 6 } },   // Ã
		CharMetric{ 11, -2, -2, { 0, 0, 0, 0, 0, 0, 0, 6 }, { 0, 0, 0, 0, 0, 0, 0, 6 } },   // Ä
		CharMetric{ 11, -2, -2, { 0, 0, 0, 0, 0, 0, 0, 6 }, { 0, 0, 0, 0, 0, 0, 0, 6 } },   // Å
		CharMetric{ 15,  0,  0, { 6, 0, 0, 0, 0, 0, 0, 6 }, { 0, 0, 0, 0, 0, 0, 0, 6 } },   // Æ
		CharMetric{ 11, -2,  0, { 6, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },   // Ç
		CharMetric{ 11, -2, -2, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },   // È
		CharMetric{ 11, -2, -2, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },
		CharMetric{ 11, -2, -2, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },
		CharMetric{ 11, -2, -2, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },   // Ë
		CharMetric{  7, -4, -2, { 0, 0, 0, 2, 0, 0, 0, 3 }, { 0, 0, 0, 2, 0, 0, 0, 3 } },   // I
		CharMetric{  7, -4, -2, { 0, 0, 0, 2, 0, 0, 0, 3 }, { 0, 0, 0, 2, 0, 0, 0, 3 } },   // I
		CharMetric{  7, -4, -2, { 0, 0, 0, 2, 0, 0, 0, 3 }, { 0, 0, 0, 2, 0, 0, 0, 3 } },   // I
		CharMetric{  9, -3, -2, { 0, 0, 0, 3, 0, 0, 1, 4 }, { 0, 0, 0, 3, 0, 0, 1, 4 } },   // I
		CharMetric{ 13, -1,  0, { 6, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },
		CharMetric{ 11, -2, -2, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },   // Njo
		CharMetric{ 11, -2, -2, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },   // O
		CharMetric{ 11, -2, -2, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },
		CharMetric{ 11, -2, -2, { 0, 0, 0, 4, 0, 0, 4, 5 }, { 0, 0, 0, 4, 0, 0, 4, 5 } },
		CharMetric{ 11, -2, -2, { 0, 0, 0, 0, 0, 0, 0, 5 }, { 0, 0, 0, 0, 0, 0, 0, 5 } },
		CharMetric{ 11, -2, -2, { 0, 0, 0, 0, 0, 0, 0, 5 }, { 0, 0, 0, 0, 0, 0, 0, 5 } },   // O
		CharMetric{  9, -3,  0, { 6, 6, 0, 0, 0, 0, 1, 6 }, { 6, 6, 0, 0, 0, 0, 1, 6 } },   // x
		CharMetric{ 11, -2,  0, { 0, 0, 0, 0, 0, 0, 0, 5 }, { 0, 0, 0, 0, 0, 0, 0, 5 } },   // O
		CharMetric{ 11, -2, -2, { 0, 0, 0, 0, 0, 0, 0, 5 }, { 0, 0, 0, 0, 0, 0, 0, 5 } },   // U
		CharMetric{ 11, -2, -2, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },
		CharMetric{ 11, -2, -2, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },
		CharMetric{ 11, -2, -2, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },   // U
		CharMetric{ 11, -2, -2, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },   // Y
		CharMetric{ 11, -2,  0, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },
		CharMetric{ 11, -2,  0, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0 } },

		// àáâãäåæçèéêëìíîïðñòóôõö÷øùúûüýþÿ
		CharMetric{ 11, -2, -1, { 0, 2, 0, 1, 0, 0, 0, 6 }, { 0, 4, 0, 1, 0, 0, 0, 6 },  6, 0b0001'0000'0001'0000, 0b0000'0010'0000'0000 }, // à
		CharMetric{ 11, -2, -1, { 0, 4, 0, 1, 0, 0, 0, 6 }, { 0, 2, 0, 1, 0, 0, 0, 6 },  6, 0b0001'0000'0001'0000, 0b0000'0010'0000'0000 }, // a
		CharMetric{ 11, -2, -1, { 0, 2, 0, 1, 0, 0, 0, 6 }, { 0, 2, 0, 1, 0, 0, 0, 6 },  6, 0b0001'0000'0001'0000, 0b0000'0010'0000'0000 }, // a
		CharMetric{ 11, -2,  0, { 0, 2, 0, 1, 0, 0, 0, 6 }, { 0, 4, 0, 1, 0, 0, 0, 6 },  6, 0b0001'0000'0001'0000, 0b0000'0010'0000'0000 }, // a
		CharMetric{ 11, -2,  0, { 0, 2, 0, 1, 0, 0, 0, 6 }, { 0, 4, 0, 1, 0, 0, 0, 6 },  6, 0b0001'0000'0001'0000, 0b0000'0010'0000'0000 }, // ä
		CharMetric{ 11, -2, -1, { 0, 2, 0, 1, 0, 0, 0, 6 }, { 0, 2, 0, 1, 0, 0, 0, 6 },  6, 0b0001'0000'0001'0000, 0b0000'0010'0000'0000 }, // å
		CharMetric{ 15,  0,  0, { 0, 8, 0, 0, 0, 0, 0, 8 }, { 0, 8, 0, 0, 0, 0, 0, 8 }, 10, 0b0001'0000'0001'0000, 0b0000'0010'0000'0010 }, // æ
		CharMetric{  9, -3,  0, { 0, 4, 0, 0, 0, 0, 0, 2 }, { 0, 4, 0, 0, 0, 0, 0, 2 },  5, 0b0000'0000'0000'0000, 0b0001'0010'0001'0010 }, // ç
		CharMetric{ 11, -2, -1, { 0, 0, 0, 0, 0, 0, 0, 6 }, { 0, 0, 0, 0, 0, 0, 0, 6 },  5, 0b0000'0000'0000'0000, 0b0000'0010'0000'0010 }, // è
		CharMetric{ 11, -2, -1, { 0, 0, 0, 0, 0, 0, 0, 6 }, { 0, 0, 0, 0, 0, 0, 0, 6 },  5, 0b0000'0000'0000'0000, 0b0000'0010'0000'0010 }, // é
		CharMetric{ 11, -2, -1, { 0, 0, 0, 0, 0, 0, 0, 6 }, { 0, 0, 0, 0, 0, 0, 0, 6 },  5, 0b0000'0000'0000'0000, 0b0000'0010'0000'0010 }, // ê
		CharMetric{ 10, -2,  0, { 0, 0, 0, 0, 0, 0, 0, 6 }, { 0, 0, 0, 0, 0, 0, 0, 6 },  5, 0b0000'0000'0000'0000, 0b0000'0010'0000'0010 }, // ë
		CharMetric{  7, -4, -1, { 0, 0, 0, 0, 0, 0, 0, 6 }, { 0, 2, 0, 2, 0, 0, 0, 6 },  3, 0b0001'0010'0001'0010, 0b0001'0010'0000'0010 }, // i
		CharMetric{  7, -4, -1, { 0, 2, 0, 0, 0, 0, 0, 6 }, { 0, 0, 0, 2, 0, 0, 0, 6 },  3, 0b0001'0010'0001'0010, 0b0001'0010'0000'0010 }, // i
		CharMetric{  7, -4, -1, { 0, 0, 0, 0, 0, 0, 0, 6 }, { 0, 0, 0, 2, 0, 0, 0, 6 },  3, 0b0001'0010'0001'0010, 0b0001'0010'0000'0010 }, // i
		CharMetric{  9, -3,  0, { 0, 0, 0, 1, 0, 0, 1, 6 }, { 0, 0, 0, 3, 0, 0, 1, 6 },  3, 0b0001'0010'0001'0010, 0b0001'0010'0000'0010 }, // i
		CharMetric{ 13, -1,  0, { 0, 6, 0, 0, 0, 0, 0, 6 }, { 0, 0, 0, 0, 0, 0, 0, 6 }, 10, 0b0000'0000'0000'0000, 0b0000'0011'0000'0000 }, // d^
		CharMetric{ 11, -2,  0, { 0, 1, 0, 0, 0, 0, 0, 6 }, { 0, 1, 0, 0, 0, 0, 0, 6 },  6, 0b0000'0000'0001'0010, 0b0000'0010'0000'0000 }, // n~
		CharMetric{ 11, -2, -1, { 0, 2, 0, 0, 0, 0, 0, 6 }, { 0, 4, 0, 0, 0, 0, 0, 6 }, 99, 0b0000'0000'0000'0000, 0b0000'0000'0000'0000 }, // o
		CharMetric{ 11, -2, -1, { 0, 4, 0, 0, 0, 0, 0, 6 }, { 0, 2, 0, 0, 0, 0, 0, 6 }, 99, 0b0000'0000'0000'0000, 0b0000'0000'0000'0000 },
		CharMetric{ 11, -2, -1, { 0, 2, 0, 0, 0, 0, 0, 6 }, { 0, 2, 0, 0, 0, 0, 0, 6 }, 99, 0b0000'0000'0000'0000, 0b0000'0000'0000'0000 }, // o
		CharMetric{ 11, -2,  0, { 0, 1, 0, 0, 0, 0, 0, 6 }, { 0, 1, 0, 0, 0, 0, 0, 6 }, 99, 0b0000'0000'0000'0000, 0b0000'0000'0000'0000 },
		CharMetric{ 11, -2,  0, { 0, 1, 0, 0, 0, 0, 0, 6 }, { 0, 1, 0, 0, 0, 0, 0, 6 }, 99, 0b0000'0000'0000'0000, 0b0000'0000'0000'0000 }, // "o
		CharMetric{ 11, -2,  0, { 0, 4, 0, 0, 0, 0, 4, 6 }, { 0, 4, 0, 0, 0, 0, 4, 6 }, 99, 0b0000'0000'0000'0000, 0b0000'0000'0000'0000 }, // div
		CharMetric{ 11, -2,  0, { 0, 6, 0, 0, 0, 0, 0, 6 }, { 0, 6, 0, 0, 0, 0, 0, 6 }, 99, 0b0000'0000'0000'0000, 0b0000'0000'0000'0000 }, // o/
		CharMetric{ 11, -2, -1, { 0, 2, 0, 0, 0, 0, 0, 6 }, { 0, 4, 0, 0, 0, 0, 0, 6 },  6, 0b0000'0000'0001'0000, 0b0001'0010'0000'0000 }, // u`
		CharMetric{ 11, -2, -1, { 0, 4, 0, 0, 0, 0, 0, 6 }, { 0, 2, 0, 0, 0, 0, 0, 6 },  6, 0b0000'0000'0001'0000, 0b0001'0010'0000'0000 }, // u´
		CharMetric{ 11, -2, -1, { 0, 2, 0, 0, 0, 0, 0, 6 }, { 0, 2, 0, 0, 0, 0, 0, 6 },  6, 0b0000'0000'0001'0000, 0b0001'0010'0000'0000 }, // u^
		CharMetric{ 11, -2,  0, { 0, 1, 0, 0, 0, 0, 0, 6 }, { 0, 1, 0, 0, 0, 0, 0, 6 },  6, 0b0000'0000'0001'0000, 0b0001'0010'0000'0000 }, // u"
		CharMetric{ 11, -2,  2, { 0, 4, 0, 0, 0, 0, 0, 0 }, { 0, 2, 0, 0, 0, 0, 0, 1 }, 10, 0b0000'0001'0001'0001, 0b0001'0010'0000'0000 }, // y´
		CharMetric{ 11, -2,  1, { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 6, 0, 0, 0, 0, 0, 6 }, 15, 0b0000'0000'0101'0011, 0b0000'0000'0000'0000 }, // :p
		CharMetric{ 11, -2,  2, { 0, 1, 0, 0, 0, 0, 0, 0 }, { 0, 1, 0, 0, 0, 0, 0, 1 }, 10, 0b0000'0001'0001'0001, 0b0001'0010'0000'0000 }, // y"
	};

	// differentiate between ligatures and just fixing the look, do a second map
	const std::unordered_map<u64, const i8> TileFont::SpecialPairs = {
		// lig
		//{ combineChars( U'f', U'a' ), -4 },
		//{ combineChars( U'c', U't' ), -3 },
		//{ combineChars( U's', U't' ), -3 },
		//{ combineChars( U'T', U'U' ), -2 },
		// near-lig
		//{ combineChars( U'i', U'f' ), -3 },
		//{ combineChars( U'S', U't' ), -1 },
		//{ combineChars( U'e', U'c' ), -1 },

		//{ combineTwoChars( U'i', U't' ), -4 },
		//{ combineChar('T','U'), -2 },
		//{"TU", -2},
		//{"Ty", -3},
		/*
		{"ft", -3},
		{"st", -3},
		{"ct", -3},
		{"ff", -2},
		{"tt", -2},
		{"sf", -2},
		{"fi", -2},
		{"is", -2},
		{"li", -1},
		{"ll", -1},
		{"ei", -1},
		{"ie", -1},
		*/
	//		{"wi", -1},
	//		{"ic", -1},
	//		{"ch", -1},
	//		{"ig", -1},
	//		{"ht", -1},
	//		{"ti", -1},
	//		{"si", -1},
	//		{"le", -1},
	//		{"fa", -1},

	//		{"ij", -6},
	//		{"nj", -6},
	//		{"bj", -6},
	};

	/*
	 * Fonts can merge at: top mid bot low
	 * One or Both must be Pointy to allow the connection
	 * A merge means that the score at this spot is 2 better
	 */

	TileFont::TileFont( Renderer * renderer )
	{
		_renderer = renderer;

		//SDL_DestroyTexture(tiles);
		if( !_tiles )
		{
			Shared<Surface>
			//Surface *
			    tileSurf = IMG_Load( BasePath "asset/graphic/hsnr64.png" );
	//		SDL_SetColorKey( tileSurf, true, SDL_MapRGB( tileSurf->format, 178, 185, 212 ) );
			const u32 paletteIndex = SDL_MapRGB( SDL_GetPixelFormatDetails( tileSurf->format ), SDL_GetSurfacePalette( tileSurf ), 0, 0, 0 );
			SDL_SetSurfaceColorKey( tileSurf, true, paletteIndex );
			_tiles = SDL_CreateTextureFromSurface( renderer, tileSurf );
			//SDL_FreeSurface(tileSurf);
			//tiles = IMG_LoadTexture( renderer, BasePath "asset/graphic/16x16_hsnr64.png" );
			if( !_tiles )
				print( stderr, "IMG_LoadTexture failed: {}\n", SDL_GetError() );
		}
	}

	void TileFont::RenderSingle(
		const String & text,
		const FRect  & dimension,
		const u8       ligatureLevel,
		const u8       spacing,
		const Point  & tilesize,
		RenderFunc     renderFunc )
	{
		//DebugOnly(
		String skipChar; // UTF-8 char build over time, therefore a string
		Rect dst     = { (int)dimension.x, (int)dimension.y, tilesize.x, tilesize.y };
		int  skip    = 0;
		u32  prefix  = 0;
		u32  last_uc = '\n';
		for( const char c : text )
		{
			// ++ skip a lot of Unicode for now
			if( skip )
			{
				IfDebug
				{
					skipChar.push_back( c );
					if( skip == 1 )
						print_once( "Skipped the drawing of char: {}\n", skipChar );
				}

				skip -= 1;
				continue;
			}

			u32 uc = (unsigned char) c; // cast is necessary here
			if( prefix != 0 )
			{
				if( prefix == 0xC2 )
				{
					assert( 0x80 <= uc && uc <= 0xBF );
					// just pass uc through
				}
				else if( prefix == 0xC3 )
				{
					assert( 0x80 <= uc && uc <= 0xBF );
					uc += 0x40;
				}

				prefix = 0;
			}
			else
			{
				const int howManyBytes = TileFont::numBytesInUTF8( c );
				if( howManyBytes > 2 )
				{
					IfDebug
					{
						skipChar = c;
					}

					skip = howManyBytes - 1;
					continue;
				}
				else if( howManyBytes == 2 )
				{
					if( uc == 0xC2
					 || uc == 0xC3 )
					{
						prefix = uc;
						continue;
					}
					else
					{
						// Only C2 and C3 are supported ATM
						assert( false );
					}
				}
				// else if( howManyBytes == 1 )
				//      uc is just ascii
			}
			// -- skip a lot of Unicode for now

			const bool isNewLine = (uc == '\n');
			if( isNewLine || dst.x > dimension.x + dimension.w )
			{
				dst.x   = dimension.x;
				dst.y  += 20;
				last_uc = '\n';

				if( isNewLine )
					continue;
			}

			auto spec = SpecialPairs.find( combineChars( last_uc, uc ) );
			if( spec != SpecialPairs.end() )
			{
				dst.x += spec->second;
			}

			const CharMetric & met_curr = CharMetrics[uc];
			if( spec == SpecialPairs.end() )
			{
				const CharMetric & met_last = CharMetrics[last_uc];
				const int  ligatureProduct    = met_last.ligLevel * met_curr.ligLevel; // max of 65025 (255 * 255)
				const bool isLigaturePossible = ligatureLevel >= ligatureProduct;
				u16 merge = 0;
				if( isLigaturePossible )
				{
					merge = met_last.mergeR & met_curr.mergeL;
					//merge |= (merge >> 4);
				}

				i8 sum[8];
				i8 ligOffs[8] = { 0 };
				bool hadALigature = false;
				for( int i = 0; i < 8; ++i )
				{
					i8 ligatureOffset = 0;
					if( isLigaturePossible )
					{
						const u16  masklo       = 0b1000'0000 >> i;
						const u16  maskhi       = 0b1000'0000'0000'0000 >> i;
						const bool canConnectLo = (merge & masklo) != 0;
						const bool canConnectHi = (merge & maskhi) != 0;

						ligatureOffset = canConnectLo + canConnectHi + (2 * canConnectLo * canConnectHi); // 3 if both pointy, 1 if only one pointy
						//ligatureOffset *= ligatureOffset;
						//ligatureOffset = (canConnect /*&& !hadALigature*/) ? 2 : 0;
						hadALigature  |= ligatureOffset > 0; // only one lig per pair of chars
						// (canConnect ? 2 : 0) == canConnect * 2
					}
					sum[i]     = met_last.right[i] + met_curr.left[i];// + ligatureOffset;
					ligOffs[i] = ligatureOffset;
				}

				if( hadALigature )
				{
					// More than 1 ligature-connect results in not merging them, just bringing them closer together.
					const int sumLigOff = std::accumulate( std::begin( ligOffs ), std::end( ligOffs ), 0 );
					if( sumLigOff > 7 )
					{
						IfDebug
							print_once(
								"Would be double ligature between '{0:c}'({0:d}) and '{1:c}'({1:d}) offset will be halfed\n",
								last_uc, uc );

						for( auto & ligOff : ligOffs )
							ligOff = 1;
					}
				}

				std::transform( std::begin(sum), std::end(sum), std::begin(ligOffs), std::begin(sum), std::plus<>() );

				const auto min_e = *std::min_element( std::begin( sum ), std::end( sum ) );
				dst.x -= min_e;
			}

			// y offset for: g, p, q, y, etc.
			const Point dst_off = (31 < uc && uc < 256) ? Point{ met_curr.xOff, met_curr.yOff } : Point{ 0, 0 };

			constexpr int stride = 32;
			const Point index{ (int) (uc % stride), (int) (uc / stride) };

			// TODO: Do the conversion to f32 inside this whole function
			const FRect src       = toF( index * 16 + toWH( tilesize ) );
			const FRect dst_final = toF( dst + dst_off ); // Angry text: + Point{-2+rand()%5, -1+rand()%3};

			renderFunc( _renderer, _tiles, src, dst_final );

			const u8 width = met_curr.width;
			IfDebug if( width > 16 )
			{
				print_once( "Wider than 16: {}\n", uc );
			}

			dst.x += (31 < uc && uc < 256)
				? width + spacing
				: 16;

			last_uc = uc;
		}
	}

	void TileFont::Render(
		const String & text,
		const FRect  & dimension,
		const Color  & color,
		const Color  & outlineColor )
	{
		constexpr u8    ligatureLevel = 25; // 0 off, 9 only ffi, 25 acst, 130 a lot
		constexpr u8    spacing       = 2;
		constexpr Point tilesize      = { 16, 16 };

		// HACK: Calling TF_RenderSingle two times in part does the exact same work twice, refactor out the common steps to make this more efficient

		SDL_SetTextureAlphaMod( _tiles, 127 );
		SDL_SetTextureColorMod( _tiles, outlineColor.r, outlineColor.g, outlineColor.b );
		RenderSingle( text, dimension, ligatureLevel, spacing, tilesize, RenderOutline );

		SDL_SetTextureAlphaMod( _tiles, SDL_ALPHA_OPAQUE );
		SDL_SetTextureColorMod( _tiles, color.r, color.g, color.b );
		RenderSingle( text, dimension, ligatureLevel, spacing, tilesize, RenderCenter );

		/* early version, keep for now
		Rect dst{ 32, 100, 16, 16 };
		char last_c = ' ';
		for( const char c : string( text ) )
		{
			if( c == '\n' || dst.x > winSize.x - 30 )
			{
				dst.x = 32;
				dst.y += 20;
				last_c = '\n';
				continue;
			}

			auto spec = special_pairs.find( string{ last_c, c } );
			if( spec != special_pairs.end() )
			{
				dst.x += spec->second;
			}

			const int x = c % 32;
			const int y = c / 32;
			const Point dst_off = (31 < c && c < 127) ? Point{ 0, char_metrics[c].yOff } : Point{ 0, 0 };
			const Rect src = (_textmode == 1)
			                 ? Rect{ x * 8, y * 8, 8, 8 }
			                 : Rect{ x * 16, (y + 3) * 16, 16, 16 };
			const Rect dst_final = dst + dst_off;
			SDL_SetTextureColorMod( tiles, 0, 0, 0 );
			for( const Point & pd : shadowOffsets )
			{
				const Rect dst_rect = dst_final + pd;
				SDL_RenderCopy( renderer, tiles, &src, &dst_rect );
			}
			SDL_SetTextureColorMod( tiles, 255, 255, 255 );
			SDL_RenderCopy( renderer, tiles, &src, &dst_final );
			u8 width = char_metrics[c].width;
			if( _textmode == 1 )
				width = ((width + 1) / 2) * 2; // approximately
			dst.x += (31 < c && c < 127) ? width + 2 : 16;

			last_c = c;
		}*/
	}


	/*static*/ void TileFont::RenderOutline( Renderer * renderer, Texture * texture, const FRect & src, const FRect & dst )
	{
		for( const FPoint & offset : HSNR64::ShadowOffset::Rhombus )
		{
			const FRect dst_off = dst + offset;
			SDL_RenderTexture( renderer, texture, &src, &dst_off );
		}
	}

	/*static*/ void TileFont::RenderCenter( Renderer * renderer, Texture * texture, const FRect & src, const FRect & dst )
	{
		SDL_RenderTexture( renderer, texture, &src, &dst );
	}
}

//#pragma clang diagnostic pop
