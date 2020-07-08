#include <stdio.h>
#include <malloc.h>

#include "tobkit/memoryindicator.h"

#define clamp(v, vmin, vmax) (((v) < (vmin)) ? (vmin) : ((v > (vmax)) ? (vmax) : (v)))

/* https://devkitpro.org/viewtopic.php?f=6&t=3057 */

extern u8 *fake_heap_end;
extern u8 *fake_heap_start;

static int getFreeMem() {
	struct mallinfo mi = mallinfo();
	return mi.fordblks + (fake_heap_end - (u8*)sbrk(0));
}

/* ===================== PUBLIC ===================== */

MemoryIndicator::MemoryIndicator(u8 _x, u8 _y, u8 _width, u8 _height, u16 **_vram, bool _visible)
	:Widget(_x, _y, _width, _height, _vram, _visible)
{
	total_ram = getFreeMem(); // only estimate!
}

MemoryIndicator::~MemoryIndicator()
{
}
// Drawing request
void MemoryIndicator::pleaseDraw(void)
{
	draw();
}

/* ===================== PRIVATE ===================== */

#define COL_INDICATOR_OK (RGB15(17,24,16) | BIT(15))
#define COL_INDICATOR_WARNING (RGB15(31,31,0) | BIT(15))
#define COL_INDICATOR_ALERT (RGB15(31,0,0) | BIT(15))

void MemoryIndicator::draw(void)
{
	u32 free_ram = getFreeMem();
	u32 used_ram = total_ram - free_ram;
	
	int boxwidth = clamp((width - 2) * used_ram / total_ram, 0, (u32) (width - 2));
	int percentfull = clamp(100 * used_ram / total_ram, 0, 100);
	
	// Color depends on percentage of used RAM
	u16 col;
	if(percentfull < 62)
		col = COL_INDICATOR_OK;
	else if(percentfull < 78)
		col = interpolateColor(COL_INDICATOR_WARNING, COL_INDICATOR_OK, (percentfull - 62) << 8);
	else if(percentfull < 94)
		col = interpolateColor(COL_INDICATOR_ALERT, COL_INDICATOR_WARNING, (percentfull - 78) << 8);
	else
		col = COL_INDICATOR_ALERT;

	drawBorder(theme->col_outline);
	drawFullBox(1, 1, width-2, height-2, theme->col_light_bg);
	drawFullBox(1, 1, boxwidth, height-2, col);
}
