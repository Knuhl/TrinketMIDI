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

#include "vusbmidi.h"   // V-USB MIDI definitions & routines
#include "trinketusb.h" // Trinket oscillator calibration & USB init
#include "usbmidi.h"    // usbmidi*() functions

int main(void) {
    /*
    https://learn.adafruit.com/introducing-trinket/pinouts
    (alle digital rw)
    PB0 = #0
    PB1 = #1 This pin is also connected to the onboard LED
    PB2 = #2 Analog Read, Digital rw
    PB3 = #3 USB
    PB4 = #4 USB

    https://de.wikibooks.org/wiki/C-Programmierung_mit_AVR-GCC/_IO-Ports

    Datenrichtung DDRx. Für Port B 1 = PB1 also DDRB
    0 = Input
    1 = Output

    PB0 Eingang, PB1 Ausgang:
    DDRB &= ~(1 << PB0);
    DDRB |= (1 << PB1);

    Wenn ein PIN ein Eingang ist, Pull-Up-Wiederstand über PORTx (PORTB)
    Pull-Up zieht den Pin auf high wenn nichts angeschlossen ist
    Eingabepin, der nicht beschaltet ist, sollte mit Pull-Up Wiederstand betrieben werden, sonst evtl. Querstrom
    0 = Pull-Up aus
    1 = Pull-Up ein
    DDRB &= ~(1 << PB1); //Pull-Up an PB1 an
    PORTB &= ~(1 << PB1); //aus

    Wenn ein PIN ein Ausgang ist, setzen über PORTx:
    PORTB |= (1 << PB0);  //PB0 im PORTB setzen
    PORTB &= ~(1 << PB0); //PB0 im PORTB löschen

    Schaltzustand lesen über PINx
    if(PINB & (1 << PB1)) {
        //PB1 ist gesetzt
    }

    _BV(x) ist Makro für (1 << (x))

    sleep mit util/delay.h -> _delay_ms(1);
    */

    uchar pedaldown=0;
    unsigned debounce=0;

    wdt_enable(WDTO_1S);
    // includes setting speed to 16MHz
    trinketUsbBegin();

    PORTB |= _BV(PB2); // Pullup on pedal
    // PORTB |= _BV(PB2); // Pullup on button
    DDRB |= _BV(PB1); // LED pin as output

    while(1) {
        // PINB |= _BV(PB1); // Toggle LED
        wdt_reset();
        usbPoll();

        if (debounce) debounce--;
        else if (!(PINB & _BV(PB2)) != pedaldown) { // pedal state change
            pedaldown = !pedaldown; // pedaldown to reflect current state
            //note, velocity
            //http://www.somascape.org/midi/basic/notes.html
            if (pedaldown) {
                PINB |= _BV(PB1); // LED on
                usbmidiNoteOn(60, 127);
            }
            else {
                PINB &= ~_BV(PB1); // LED off
                usbmidiNoteOff(60, 0);
            }
            debounce = 1000;
        }

        usbmidiSend();
    }

    return 0;
}
