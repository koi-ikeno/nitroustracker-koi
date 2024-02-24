/*
 * NitroTracker - An FT2-style tracker for the Nintendo DS
 *
 *                                by Tobias Weyand (0xtob)
 *
 * http://nitrotracker.tobw.net
 * http://code.google.com/p/nitrotracker
 */

/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <nds.h>

#include <ntxm/ntxm7.h>

extern "C" {
  #include "ntxm/linear_freq_table.h"
}

#ifdef WIFI
#include <dswifi7.h>
#endif

NTXM7 *ntxm7 = 0;

static volatile bool exitflag = false;
extern bool ntxm_recording;

int vcount;
touchPosition first,tempPos;

void VcountHandler() {
	if(ntxm_recording == true)
		return;

	inputGetAndSend();
}


void VblankHandler(void)
{
#ifdef WIFI
	if(ntxm_recording == false)
		Wifi_Update(); // update wireless in vblank
#endif
}

// This is the callback for the timer
void ntxmTimerHandler(void) {
	ntxm7->timerHandler();
}

void powerButtonHandler(void) {
	exitflag = true;
}

//---------------------------------------------------------------------------------
void enableSound() {
//---------------------------------------------------------------------------------
    powerOn(POWER_SOUND);
    writePowerManagement(PM_CONTROL_REG, ( readPowerManagement(PM_CONTROL_REG) & ~PM_SOUND_MUTE ) | PM_SOUND_AMP );
    REG_SOUNDCNT = SOUND_ENABLE;
    REG_MASTER_VOLUME = 127;
}

//---------------------------------------------------------------------------------
int main(int argc, char ** argv) {
//---------------------------------------------------------------------------------
	enableSound();
	readUserSettings();
	ledBlink(0);
	touchInit();
	irqInit();
	SetYtrigger(80);
	irqSet(IRQ_VCOUNT, VcountHandler);
	irqSet(IRQ_VBLANK, VblankHandler);
	fifoInit();
#ifdef WIFI
	installWifiFIFO();
#endif
	installSystemFIFO();

	irqEnable(IRQ_VBLANK | IRQ_VCOUNT);
#ifdef WIFI
	irqEnable(IRQ_NETWORK);
#endif
#ifdef BLOCKSDS
	initClockIRQTimer(3);
#else
	rtcReset();
#endif

	// Create ntxm player
	ntxm7 = new NTXM7(ntxmTimerHandler);

	// Initialize NTXM update timer
	irqSet(IRQ_TIMER0, ntxmTimerHandler);
	irqEnable(IRQ_TIMER0);

	setPowerButtonCB(powerButtonHandler);

	// Keep the ARM7 out of main RAM
	while (!exitflag) {
		swiWaitForVBlank();
	}

	return 0;
}
