/******************************************************************************

 Copyright (c) 2015, Focusrite Audio Engineering Ltd.
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.

 * Neither the name of Focusrite Audio Engineering Ltd., nor the names of its
 contributors may be used to endorse or promote products derived from
 this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 *****************************************************************************/

//______________________________________________________________________________
//
// Headers
//______________________________________________________________________________

#include "app.h"

#define _32TH  6
#define _16TH  12
#define _8TH  24
#define _4TH  48
#define _HALVE  96
#define _WHOLE  192

static u8 tick = 0;
static u8 interval = _16TH;
// buffer to store pad states for flash save
#define BUTTON_COUNT 100

u8 pressed_state[BUTTON_COUNT] = {0};
u8 prev_pressed_state[BUTTON_COUNT] = {0};

//______________________________________________________________________________

void app_surface_event(u8 type, u8 index, u8 value)
{
    switch (type)
    {
        case  TYPEPAD:
        {
            pressed_state[index] = value;
        }
        break;

        case TYPESETUP:
        {
            if (value)
            {
                // save button states to flash (reload them by power cycling the hardware!)
                hal_write_flash(0, pressed_state, BUTTON_COUNT);
            }
        }
        break;
    }
}

//______________________________________________________________________________
static void trigger_notes(void)
{
    for (u8 i = 0; i<BUTTON_COUNT; i++) {
        u8 pressure = pressed_state[i];
        if (pressure) {
            hal_send_midi(USBMIDI, NOTEON, i, pressure);
        } else if (prev_pressed_state[i]) {
            hal_send_midi(USBMIDI, NOTEOFF, i, 0);
        }
        prev_pressed_state[i] = pressed_state[i];
    }
}

static void midi_timing_event(void)
{
    tick++;
    if (tick % interval == 0) {
        trigger_notes();
    }
    if (tick == _WHOLE) {
        tick = 0;
    }
}

void app_midi_event(u8 port, u8 status, u8 d1, u8 d2)
{
    // example - MIDI interface functionality for USB "MIDI" port -> DIN port
    if (port == USBMIDI)
    {
        hal_send_midi(DINMIDI, status, d1, d2);
    }

    // // example -MIDI interface functionality for DIN -> USB "MIDI" port port
    if (port == DINMIDI)
    {
        hal_send_midi(USBMIDI, status, d1, d2);
    }

    if (status == 0xFA) // start
    {
        tick = 0;
    }

    if (status == 0xF8) // timing
    {
        midi_timing_event();
    }

}

//______________________________________________________________________________

void app_sysex_event(u8 port, u8 * data, u16 count)
{
}

//______________________________________________________________________________

void app_aftertouch_event(u8 index, u8 value)
{
    pressed_state[index] = value;
}

//______________________________________________________________________________

void app_cable_event(u8 type, u8 value)
{
}

//______________________________________________________________________________

void app_timer_event()
{
}

//______________________________________________________________________________

void app_init(const u16 *adc_raw)
{
}
