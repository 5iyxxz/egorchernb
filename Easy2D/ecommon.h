#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include <functional>

namespace e2d
{

typedef std::wstring EString;

template<typename T>
using EVector = std::vector<T>;

struct EWindowStyle
{
	EWindowStyle()
	{
		ICON_ID = 0;
		NO_CLOSE = false;
		NO_MINI_SIZE = false;
		TOP_MOST = false;
	}

	EWindowStyle(
		LPCTSTR ICON_ID
	)
	{
		this->ICON_ID = ICON_ID;
		NO_CLOSE = false;
		NO_MINI_SIZE = false;
		TOP_MOST = false;
	}

	EWindowStyle(
		LPCTSTR ICON_ID,
		bool NO_CLOSE,
		bool NO_MINI_SIZE,
		bool TOP_MOST
	)
	{
		this->ICON_ID = ICON_ID;
		this->NO_CLOSE = NO_CLOSE;
		this->NO_MINI_SIZE = NO_MINI_SIZE;
		this->TOP_MOST = TOP_MOST;
	}

	LPCTSTR ICON_ID;	/* 程序图标 ID */
	bool NO_CLOSE;		/* 禁用关闭按钮 */
	bool NO_MINI_SIZE;	/* 禁用最小化按钮 */
	bool TOP_MOST;		/* 窗口置顶 */
};

struct EPoint
{
	EPoint()
	{
		x = 0;
		y = 0;
	}

	EPoint(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	EPoint operator + (EPoint const & p)
	{
		return EPoint(x + p.x, y + p.y);
	}

	EPoint operator - (EPoint const & p)
	{
		return EPoint(x - p.x, y - p.y);
	}

	float x;
	float y;
};

typedef EPoint EVec;

struct ESize
{
	ESize()
	{
		width = 0;
		height = 0;
	}

	ESize(float width, float height)
	{
		this->width = width;
		this->height = height;
	}

	ESize operator + (ESize const & size)
	{
		return ESize(width + size.width, height + size.height);
	}

	ESize operator - (ESize const & size)
	{
		return ESize(width - size.width, height - size.height);
	}

	float width;
	float height;
};


// 定时器回调函数（参数为该定时器被调用的次数，从 0 开始）
typedef std::function<void(int)> TIMER_CALLBACK;
// 按钮点击回调函数
typedef std::function<void()> BUTTON_CLICK_CALLBACK;
// 按键消息监听回调函数
typedef std::function<void()> KEY_LISTENER_CALLBACK;
// 鼠标消息监听回调函数
typedef std::function<void()> MOUSE_LISTENER_CALLBACK;
// 鼠标点击消息监听回调函数（参数为点击位置）
typedef std::function<void(EPoint mousePos)> MOUSE_CLICK_LISTENER_CALLBACK;
// 鼠标按下消息监听回调函数（参数为按下位置）
typedef MOUSE_CLICK_LISTENER_CALLBACK MOUSE_PRESS_LISTENER_CALLBACK;
// 鼠标双击消息监听回调函数（参数为双击位置）
typedef MOUSE_CLICK_LISTENER_CALLBACK MOUSE_DBLCLK_LISTENER_CALLBACK;
// 鼠标拖动消息监听函数（参数为拖动前位置和拖动后位置）
typedef std::function<void(EPoint begin, EPoint end)> MOUSE_DRAG_LISTENER_CALLBACK;



class EColor
{
public:

	EColor()
	{
		value = VALUE::WHITE;
	}

	EColor(int color)
	{
		value = VALUE(color);
	}
	
	enum VALUE
	{
		ALICE_BLUE = 0xF0F8FF,
		ANTIQUE_WHITE = 0xFAEBD7,
		AQUA = 0x00FFFF,
		AQUAMARINE = 0x7FFFD4,
		AZURE = 0xF0FFFF,
		BEIGE = 0xF5F5DC,
		BISQUE = 0xFFE4C4,
		BLACK = 0x000000,
		BLANCHED_ALMOND = 0xFFEBCD,
		BLUE = 0x0000FF,
		BLUE_VIOLET = 0x8A2BE2,
		BROWN = 0xA52A2A,
		BURLY_WOOD = 0xDEB887,
		CADET_BLUE = 0x5F9EA0,
		CHARTREUSE = 0x7FFF00,
		CHOCOLATE = 0xD2691E,
		CORAL = 0xFF7F50,
		CORNFLOWER_BLUE = 0x6495ED,
		CORNSILK = 0xFFF8DC,
		CRIMSON = 0xDC143C,
		CYAN = 0x00FFFF,
		DARK_BLUE = 0x00008B,
		DARK_CYAN = 0x008B8B,
		DARK_GOLDENROD = 0xB8860B,
		DARK_GRAY = 0xA9A9A9,
		DARK_GREEN = 0x006400,
		DARK_KHAKI = 0xBDB76B,
		DARK_MAGENTA = 0x8B008B,
		DARK_OLIVE_GREEN = 0x556B2F,
		DARK_ORANGE = 0xFF8C00,
		DARK_ORCHID = 0x9932CC,
		DARK_RED = 0x8B0000,
		DARK_SALMON = 0xE9967A,
		DARK_SEA_GREEN = 0x8FBC8F,
		DARK_SLATE_BLUE = 0x483D8B,
		DARK_SLATE_GRAY = 0x2F4F4F,
		DARK_TURQUOISE = 0x00CED1,
		DARK_VIOLET = 0x9400D3,
		DEEP_PINK = 0xFF1493,
		DEEP_SKY_BLUE = 0x00BFFF,
		DIM_GRAY = 0x696969,
		DODGER_BLUE = 0x1E90FF,
		FIREBRICK = 0xB22222,
		FLORAL_WHITE = 0xFFFAF0,
		FOREST_GREEN = 0x228B22,
		FUCHSIA = 0xFF00FF,
		GAINSCORO = 0xDCDCDC,
		GHOST_WHITE = 0xF8F8FF,
		GOLD = 0xFFD700,
		GOLDENROD = 0xDAA520,
		GRAY = 0x808080,
		GREEN = 0x008000,
		GREEN_YELLOW = 0xADFF2F,
		HONEYDEW = 0xF0FFF0,
		HOT_PINK = 0xFF69B4,
		INDIAN_RED = 0xCD5C5C,
		INDIGO = 0x4B0082,
		IVORY = 0xFFFFF0,
		KHAKI = 0xF0E68C,
		LAVENDER = 0xE6E6FA,
		LAVENDER_BLUSH = 0xFFF0F5,
		LAWN_GREEN = 0x7CFC00,
		LEMON_CHIFFON = 0xFFFACD,
		LIGHT_BLUE = 0xADD8E6,
		LIGHT_CORAL = 0xF08080,
		LIGHT_CYAN = 0xE0FFFF,
		LIGHT_GOLDENROD_YELLOW = 0xFAFAD2,
		LIGHT_GREEN = 0x90EE90,
		LIGHT_GRAY = 0xD3D3D3,
		LIGHT_PINK = 0xFFB6C1,
		LIGHT_SALMON = 0xFFA07A,
		LIGHT_SEA_GREEN = 0x20B2AA,
		LIGHT_SKY_BLUE = 0x87CEFA,
		LIGHT_SLATE_GRAY = 0x778899,
		LIGHT_STEEL_BLUE = 0xB0C4DE,
		LIGHT_YELLOW = 0xFFFFE0,
		LIME = 0x00FF00,
		LIME_GREEN = 0x32CD32,
		LINEN = 0xFAF0E6,
		MAGENTA = 0xFF00FF,
		MAROON = 0x800000,
		MEDIUM_AQUAMARINE = 0x66CDAA,
		MEDIUM_BLUE = 0x0000CD,
		MEDIUM_ORCHID = 0xBA55D3,
		MEDIUM_PURPLE = 0x9370DB,
		MEDIUM_SEA_GREEN = 0x3CB371,
		MEDIUM_SLATE_BLUE = 0x7B68EE,
		MEDIUM_SPRING_GREEN = 0x00FA9A,
		MEDIUM_TURQUOISE = 0x48D1CC,
		MEDUIM_VIOLET_RED = 0xC71585,
		MIDNIGHT_BLUE = 0x191970,
		MINT_CREAM = 0xF5FFFA,
		MISTY_ROSE = 0xFFE4E1,
		MOCCASIN = 0xFFE4B5,
		NAVAJO_WHITE = 0xFFDEAD,
		NAVY = 0x000080,
		OLD_LACE = 0xFDF5E6,
		OLIVE = 0x808000,
		OLIVE_DRAB = 0x6B8E23,
		ORANGE = 0xFFA500,
		ORANGE_RED = 0xFF4500,
		ORCHID = 0xDA70D6,
		PALE_GOLDENROD = 0xEEE8AA,
		PALE_GREEN = 0x98FB98,
		PALE_TURQUOISE = 0xAFEEEE,
		PALE_VIOLET_RED = 0xDB7093,
		PAPAYA_WHIP = 0xFFEFD5,
		PEACH_PUFF = 0xFFDAB9,
		PERU = 0xCD853F,
		PINK = 0xFFC0CB,
		PLUM = 0xDDA0DD,
		POWDER_BLUE = 0xB0E0E6,
		PURPLE = 0x800080,
		RED = 0xFF0000,
		ROSY_BROWN = 0xBC8F8F,
		ROYAL_BLUE = 0x4169E1,
		SADDLE_BROWN = 0x8B4513,
		SALMON = 0xFA8072,
		SANDY_BROWN = 0xF4A460,
		SEA_GREEN = 0x2E8B57,
		SEA_SHELL = 0xFFF5EE,
		SIENNA = 0xA0522D,
		SILVER = 0xC0C0C0,
		SKY_BLUE = 0x87CEEB,
		SLATE_BLUE = 0x6A5ACD,
		SLATE_GRAY = 0x708090,
		SNOW = 0xFFFAFA,
		SPRING_GREEN = 0x00FF7F,
		STEEL_BLUE = 0x4682B4,
		TAN = 0xD2B48C,
		TEAL = 0x008080,
		THISTLE = 0xD8BFD8,
		TOMATO = 0xFF6347,
		TURQUOISE = 0x40E0D0,
		VIOLET = 0xEE82EE,
		WHEAT = 0xF5DEB3,
		WHITE = 0xFFFFFF,
		WHITE_SMOKE = 0xF5F5F5,
		YELLOW = 0xFFFF00,
		YELLOW_GREEN = 0x9ACD32
	};

	VALUE value;
};


class EFontWeight
{
public:

	EFontWeight()
	{
		value = VALUE::REGULAR;
	}

	EFontWeight(int fontWeight)
	{
		value = VALUE(fontWeight);
	}

	enum VALUE
	{
		THIN = 100,
		EXTRA_LIGHT = 200,
		ULTRA_LIGHT = 200,
		LIGHT = 300,
		SEMI_LIGHT = 350,
		NORMAL = 400,
		REGULAR = 400,
		MEDIUM = 500,
		DEMI_BOLD = 600,
		SEMI_BOLD = 600,
		BOLD = 700,
		EXTRA_BOLD = 800,
		ULTRA_BOLD = 800,
		BLACK = 900,
		HEAVY = 900,
		EXTRA_BLACK = 950,
		ULTRA_BLACK = 950
	};

	VALUE value;
};

}