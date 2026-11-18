# Rest Pause Tracker

A small gym device that tells you when to start your next set. Instead of a fixed rest
timer, it watches your heart rate from a BLE chest strap. It waits until your heart rate has
recovered from the last set **and** your chosen minimum rest time has passed.

Built on an ESP32-C3 with an OLED display, an RGB status LED and two buttons, in a
3D-printed enclosure.

## How it works

After a set your heart rate drops quickly at first, then slows down and levels off. The
device models this as an exponential decay:

```
HR(t) = c + a · e^(-t / τ)
```

1. Heart rate is sampled at 1 Hz into a ring buffer. The buffer also keeps 60 s from *before*
   the button press, because you usually press it after putting the weights down, when the
   peak has already passed.
2. The peak is found and confirmed once HR stays at least 3 bpm below it for 5 s.
3. The model is fitted with a log transform and linear regression. The asymptote `c` is
   unknown, so a range of candidate values below the lowest HR is tried and the best fit is
   kept.
4. Recovery ratio `p = (peak − current) / (peak − c)`. HR counts as **ready** when `p ≥ 0.90`.
5. Fallback: if the fit fails, HR is ready once the slope over the last 20 s is close to zero
   (plateau).

## Hardware

| Part | Details | Pin (GPIO) |
|---|---|---|
| MCU | Seeed Studio XIAO ESP32-C3 | – |
| Heart rate | Any BLE chest strap with the Heart Rate Service (0x180D) | – |
| Display | 1.3" OLED 128×64, SH1106, I²C 0x3C | SDA 6, SCL 7 |
| Status LED | 5 mm WS2811 RGB LED | 5 |
| Button A | to GND, internal pull-up | 2 |
| Button B | to GND, internal pull-up | 3 |
| Battery | 500 mAh LiPo, 2× 220 kΩ voltage divider | 4 (ADC) |

Note: GPIO 2 is a strapping pin, so don't hold button A during boot.

Enclosure: OpenSCAD source in [enclosure/kryt.scad](enclosure/kryt.scad), printable STLs in [enclosure/stl/](enclosure/stl/).

## Usage

| State | LED | Display | Buttons |
|---|---|---|---|
| **SETUP** | blue | minimum rest time, battery % | A = start workout, B = +15 s (2:00–4:00, wraps around), hold B = forget paired strap |
| **READY** | green | inverted "GO!" | A = set finished → start rest, hold B = end workout |
| **REST_RED** | red | elapsed rest time | hold A = skip rest, hold B = end workout |
| **REST_ORANGE** | orange | waiting for HR or for the time | hold A = skip rest, hold B = end workout |
| **SUMMARY** | purple | sets, total time, average rest | any button → SETUP |

The rest ends (→ READY) only when **both** conditions hold: HR is ready and the minimum rest
time has passed. REST_ORANGE means exactly one of them is still missing.

The first strap found is paired and its address saved to NVS, so the device reconnects to it
automatically.

## Building

[PlatformIO](https://platformio.org/) project, Arduino framework.

```sh
pio run -t upload
pio device monitor        # 115200 baud
```

Libraries (pulled in automatically): NimBLE-Arduino, Adafruit SH110X, Adafruit GFX, FastLED.

## Logging

Each workout is written to LittleFS as `/workout_NNNN.csv`, one row per second:

```
t,hr,state,ready
```

`t` = seconds since workout start, `hr` = bpm (0 = no data), `state` = state index
(0 SETUP … 4 SUMMARY), `ready` = algorithm output. Send `DUMP` over the serial monitor to
print the last workout.

## Serial simulator

Set `SIMULATION = true` in [include/config.h](include/config.h) to test without a strap or
buttons. Send these commands from the serial monitor:

```
HR,140        set simulated heart rate
BTN,A         short press A
BTN,A_LONG    long press A
BTN,B         short press B
BTN,B_LONG    long press B
DUMP          print the last workout log (works in normal mode too)
```

## Project structure

```
include/config.h        pins and tuning constants
src/main.cpp            wiring of all modules, main loop
src/StateMachine        workout states and statistics
src/Button              debouncing, short / long press detection
src/BleHeartRate        BLE scan, connect, HR notifications (NimBLE)
src/HrBuffer            1 Hz ring buffer of HR samples
src/RecoveryAlgorithm   peak detection, exponential fit, plateau fallback
src/DisplayUI           OLED screens
src/LedController       status LED
src/Logger              CSV logging to LittleFS
src/BatteryMonitor      battery voltage → percent
src/SerialSimulator     serial commands for testing
```
