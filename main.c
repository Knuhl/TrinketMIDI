/*
This file is part of TrinketMIDI. See README.md for licencing details.

TrinketMIDI is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

TrinketMIDI is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with TrinketMIDI.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <avr/power.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "usbconfig.h"
#include "usbdrv/usbdrv.h"

#include "vusbmidi.h"
#include "trinketusb.h"
#include "usbmidi.h"

int main(void) {
    uchar keydown=0;
    unsigned debounce=0;

    wdt_enable(WDTO_1S);
    usbBegin();

    PORTB |= _BV(PB0); // Pullup on button

    while(1) {
        wdt_reset();
        usbPoll();

        if(debounce) debounce--;
        else if(!(PINB & _BV(PB0)) != keydown) { // button state change
            keydown = !keydown; // keydown to reflect current state
            if(keydown)
                usbmidiNoteOn(60, 50);
            else
                usbmidiNoteOff(60, 0);
            debounce = 1000;
        }

        usbmidiSend();
    }

    return 0;
}
