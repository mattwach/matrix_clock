# matrix_clock
> "I DON’T EVEN SEE THE CODE ANYMORE" - The Matrix.  

![matrix clock](images/matrix_clock.jpg)

Shown above is a clock that presents the current time on an 64x32 RGB LED
matrix.  The clock has other selectable modes, including these:

![matrix clock](images/matrix_clock2.jpg)
![matrix clock](images/matrix_clock3.jpg)

The firware also support different hardware configurations (different LED and
clock hardware), included is a 8x8 "dotstar" model which I used originally:

![matrix clock](images/ds8x8/matrix_clock.jpg)

This document focuses on the 62x32-based model, which I prefer.  If you
want to learn more details on the 8x8 build, I have a separate document
for it [here](README_dotstar_8x8.md).

In the upcoming sections, I explain how to build the clock.  I also
explain how to modify the design in different ways.

# Parts List

Here are the parts I used to build the 64x32 version.  You can of course modify
the design to use different ones:

   * Microcontroller: [Raspberry PI Pico ($4)](https://www.adafruit.com/product/4864)
   * [LED Matrix 62x32: ($30)](https://www.amazon.com/waveshare-Displaying-Animation-Adjustable-Brightness/dp/B0B3W1PFY6)
     Note there are many variants available.
   * Clock: [DS3231-based RTC module ($4)](https://www.amazon.com/HiLetgo-AT24C32-Arduino-Without-Battery/dp/B00LX3V7F0)
   * Buttons: [2 12x12 pushbuttons for control (<$1)](https://www.amazon.com/WOWOONE-12x12x7-3-Tactile-Momentary-Assortment/dp/B08JLWTQ3C),
     1 6x3 SMD reset button (optional)
   * Connectors:
     * One 8x2 female pin header to interface the Pico PCB with the matrix.
       Most displays include an interface cable that I am not using in the PCB
       design.
     * A couple of angled male pin headers to connect to buttons and power.
   * 1 220uF capacitor to reduce 5V noise
   * PCB for assembly (however you want to build it.  maybe something like
     [this](https://www.amazon.com/DEYUE-Double-sided-Prototyping-Solder-able-Protoboards/dp/B07FFDFLZ3))

Optional:

The LED display as-designed is powered directly from the Pico USB power pin
(VDD).  This reduces part count and complexity.  The downside is that the LED
can pull a lot of current if every pixel is set to white (around 3A).  The
firmware included never does this and thus pulls between 100-400 MA (measured).
The optional part is a current protection device (the NCP380) which will limit
the max current to the LED to 500 mA, protecting the Pico and potentially the
USB power source from being overloaded.  In my opinion this is important if you
are experimenting with your own custom firmware modifications.  If you are just
sticking with the stock ones, I have already checked that the current draw is
not excessive and you can skip the protection hardware and save a bit of money.
Here are the needed parts:

   * NCP380
   * Two additional 1uf capacitors for stability.  I am using SMD but ceramic
     through-hole are also an option.
   * A 50k pullup resistor for the FLAG pin of the NCP380.  This allows the
     NCP380 to tell the Pico that it is limiting current which the pico will
     then report to you by activating its onboard LED.

# Reading the clock

Here is my scheme for showing the time, which you can easily modify to your
preference.

Numbers are represented by colors.  At the time of writing, they are as follows:

![numbers](images/numbers.jpg)

> Colors are defined in [src/colors.c](src/colors.c) if you would like to edit them.

As for the individual digits in the time, these are represented by the speed at
which the matrix points fall:

| digit               | speed  |
|---------------------|--------|
| Hour - ones place   | slow   |
| Minute - tens place | medium |
| Minute - ones place | fast   |

Here are some examples:

| 12h time | 24h time | Slow point | Medium Point | Fast Point |
|----------|----------|------------|--------------|------------|
| 12:00 AM | 00:00    | brown      | brown        | brown      |
| 12:15 AM | 00:15    | brown      | red          | blue       |
| 1:37 PM  | 13:37    | yellow     | yellow       | magenta    |
| 11:37 PM | 23:37    | yellow     | yellow       | magenta    |

The falling points do not show the tens place of the hor becuase I thought it
made the clock harder to read.  Also there are not many options in the tens
place (0, 1 or 2).  If you want the tens place, you can select one of the
available display modes that shows it.

# Feature Overview

This is a quick tour of the built in-features

## Power On

When you plug in the clock via any USB power source you will get a matrix clock
display that indicates the current time as-reported by the internal clock module.

![matrix clock 2](images/matrix_clock4.jpg)

## Setting the Time

The clock has two buttons on it's side: "set" and "increment". Pressing "set"
goes into time change mode where you can use "set" and "increment" to change
each time digit.

![numbers](images/numbers.jpg)

> You can also set the clock via the USB connection.

## Changing the Display Mode

If you press the "increment" button while the matrix is showing, the clock will
cycle through display modes.  Currently defined modes include:

* **Matrix With Time** The default
* **Matrix Without Time** For those who want a challenge
* **Number cascade**, Numbers fall instead of points with hours being "closer"
  (larger/faster) than minutes.
* **Off -**  Useful if you want a darkened room.

## Console Configuration

If you plug the clock into a computer, you can run a terminal emulator program
to access additional settings.  In Linux, I use the "minicom" terminal emulator
and my command looks like this:

```bash
minicom -b 115200 -P /dev/ttyUSB0
```

> The serial device (`/dev/ttyUSB0`) can vary between computers and operating
> systems. 

Once started, you will see something like this:

![minicom](images/minicom.jpg)

By default, `minicom` turns on local echo, making my typed characters appear
twice.  `minicom` allows this to be turned off with `ctrl-a e`.

Type `help` or `?` for help.  Basic options include:

   * Changing LED brightness
   * Changing the time
   * Changing the poweron display mode
   * Simulating hardware button presses
   * Setting a sleep/wake time where the clock LEDs will automatically turn off.

# Build Instructions

## Electronics

Here is the schematic for the 32x64 version:

![schematic](images/schematic.png)

and the schematic for the 8x8 dotstar version:

![schematic](images/schematic.png)

Here is a description on major components and their purpose:

  - Pi Pico: This is the main controller
  - RTC Clock: This provides time to the Pico via an I2C connection.  You
    could use other solutions here, including attempting to use the PI Pico
    to track time but also GPS, radio-based time, or internet time using a
    Pico W.  In terms of the I2C connection, note that the RTC clock is powered
    by 3.3V instead of the specified 5V.  This is the easiest way to make the
    clock compatible with the 3.3V maximum voltage of the PI Pico.  If your RTC
    really does need 5V, there are solutions but the details are not the focus
    of this document.  Thus I suggest doing a Google search for
    "3.3 to 5V I2C" to learn more.
  - 64x32 matrix: This is also a 5V part but can usually be
    controlled from the 3.3V outputs of the PI Pico, even though the LED is
    being *powered* by 5V - mine definitely works in this configuration. All
    connections are high-impedance passive inputs so the Pico does not have to
    concern itself with the 5V nature of the LED panel.  An easy way to tell is
    to see if any reviews are running the panel directly from a 3.3V part, such
    as the PI Pico, Raspberry PI, ARM, or ESP series.
  - 220u capacitor.  The actual value is not important other that "big".  The
    purpose of the cap is to support the power requirements of the LED, which
    can be a "noisy" load, needing near zero current one moment and 300 mA the
    next.
  - Support for buttons.  Select, increment and reset as described earlier.
  - NCP380 current limiting chip (optional).  This chip provides insurance for
    the case where you one day decide to change the firmware to light up many
    LEDs and draw too much current.  If you really want to do support high
    current draw, you'll need to rework the power design so that the LED panel
    can be directly powered from a capable (3A+) source.

> If you don't us use the NCP380, simply connect the 5V directly to the LED.
> Keep the 220u capacitor for noise purposes.

![opened clock](images/back_of_clock.jpg)

## Firmware

> TLDR: If you don't want to build the firmware (`matrix_clock_led_32x64.uf2`) 
yourself, I have a couple of precompiled versions under the
[firmware](firmware) directory.

Source files (written in C) are  provided in the [src/](src) directory.  If you
have never built PI Pico firmware before, run through the official [Getting
started with the Raspberry Pi
PICO](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf)
documentation first to get the needed tools up-and-running on your system.

The instructions for building the clock firmware are the same as the guide
linked above, but I created a small/simple [bootstrap.sh](src/bootstrap.sh)
script to lessen the number to needed steps:

```bash
./bootstrap.sh 
cd build/
make -j
```

I encourage you to view the contents of [bootstrap.sh](src/bootstrap.sh)
  yourself instead of blindly running it.  You'll see something like this:

```bash
$ cat bootstrap.sh 
#!/bin/bash

if [ ! -f pico_uart_console/CMakeLists.txt ]; then
  git submodule init
  git submodule update
fi

rm -rf build
mkdir build
cd build
cmake ..
```

The two major things done here are getting some dependencies (submodules) and
setting up a new build environment under `build/`

If all went well above, you will have a `build/matrix_clock.uf2` file available
to load onto a PI Pico.

# Hardware build

TODO

# Cusomization Guide

This part of the documentation points you in the right direction if you want to modify the project.

## Using Different LED Hardware

The file [src/led_matrix.h](src/led_matrix.h), contains the interface your
hardware driver will be called with.
[src/led_matrix_dotstar.c](src/led_matrix_dotstar.c) implements it for the 8x8
dotstar case.  To use your own, create a new `.c` file (such as
`src/led_matrix_neopixel.c` and change `CMakeLists.txt` to point to your new
file instead of `led_matrix_dotstar.c`.  The main function you'll be
implementing is:

```c
#define LED_MATRIX_WIDTH 8
#define LED_MATRIX_HEIGHT 8

// render the matrix.
// data is of the form 0xIIRRGGBB where
// II -> Intensity (brightness)
// RR -> Red
// GG -> Green
// BB -> Blue
//
// Values are 0-255
// format is horizontal rows.  The first pixel
// sent is the southwest corner of the matrix.
void led_matrix_render(uint32_t* data);
```

again, look at [src/led_matrix_dotstar.c](src/led_matrix_dotstar.c) for a concrete exmaple of an implementation of `led_matrix_render`.  It's really not much code for the dotstar case.

## Using Different Clock Hardware

Instead of an RTC, you might want to go with a radio-based clock, a GPS clock, or [keeping the time with the Pi Pico](https://raspberrypi.github.io/pico-sdk-doxygen/group__hardware__rtc.html) (requiring the user to set the time whenever the power is removed).

[src/clock.h](src/clock.h) gives the interface:

```c
// gets the current time in HHMM format (0000 - 2359)
// For example, if 1354 is returned, the time is 13:54 (or
// 1:34 PM if you prefer)
uint16_t clock_get_time();

// Sets the time.  For example: time_hhmm = 1234 would set the
// time to 12:34.  Note that 24h time is always used so 1234
// is 12:34 PM and 0034 is 12:34 AM.
void clock_set_time(uint16_t time_hhmm);
```

and [src/clock.c](src/clock.c) gives a concrete implementation for the RTC module.

## Changing The Display Rendering

There are two files and one directory to consider.  The first file is [src/clock_render.c](src/clock_render.c).  Specifically, this section:

```c
struct DisplayMode display_modes[] = {
  {"normal", matrix_render},  // This entry will be the default power-on mode
  {"number_cascade", number_cascade_render},
  {"number_fade", fade_render},
  {"binary_grid", binary_grid_render},
  {"off", blank_render},  // always put this entry at the end of the list
};
```

This section lists the display modes, names them and provides a function pointer to each one.  If you were to remove a line from this array, or reorder the lines, you would see the corresponding change when you build/load new firmware.  In order for the function pointers to have any meaning, you'll need to include the corresponding files:

```c
#include "render/blank.h"
#include "render/fade.h"
#include "render/matrix.h"
#include "render/number_cascade.h"
#include "render/number_grid.h"
```

Note that, by convention, all rendering functions are defined in the [src/render](src/render) directory.  Here is an example [src/render/blank.c](src/render/blank.c):

```c
void blank_render(
    uint32_t* led,
    uint32_t frame_index,
    uint16_t time_hhmm,
    const struct ClockSettings* settings) {
  memset(
    led,
    0,
    LED_MATRIX_WIDTH * LED_MATRIX_HEIGHT * sizeof(uint32_t));
}
```

This simple function simply clears the `led` array to zero which effectively turns off the display.

Showing actual patterns will generally require use of the other parameters, such as `time_hhmm`.

The `frame_index` parameter can be useful for initilization.  It is always set to `0` when the display mode is made active.

The `settings` structure has several potentially-useful fields but especially useful is getting the user preference for LED brightness:

```c
  const uint8_t br = brightness_step_to_brightness(settings);
```

This value (0-255), would then be used as a baseline brightness when turning on leds in `led`

## Changing the Console

The file `src/clock_settings.c` contains all of the commands available to the
USB shell.  Adding your own is straight-forward but I suggest skimming through
https://github.com/mattwach/pico_uart_console to get up-to-speed on how the
console works first.

## Using a Different Microcontroller

The hardware requirements for the microcontroller are not demanding for this project (as implemented anyway).  Going with AVR, ESP, STM, PIC or something else would all work perfectly fine.  The main difficulty will be adapting the firmware, specifically:

* The CMake build system
* Access to the PI Pico SDK

To work around these, I would suggest copying the source files to a new project folder, use the "default" build system for your microcontroller and addressing the compiler errors one by one.  Remember that since PI Picos are only $4 and are readily available (at the time I write this) the motivation for going through this effort might not be immediately obivous to others.

