# Airsoft Bomb Emulator

Arduino Nano (ATmega328P) and Uno firmware for a 20x4 I2C LCD, 4x4 keypad, two team buttons, a passive buzzer and a 60-pixel NeoPixel strip. Supports Domination and Defuse.

## Build and upload

Install PlatformIO, then run from the project directory:

```powershell
pio run
pio run -t upload
```

The default environment is `nanoatmega328`, configured to upload on COM3 using the Arduino protocol at 115200 baud. Use `--upload-port COMx` to override the port. Uno remains available with `pio run -e uno` or `pio run -e uno -t upload`, with automatic port detection. Both boards share the pinned AVR platform, Keypad and FastLED versions in `platformio.ini`. The existing NewLiquidCrystal implementation is vendored in `lib/`; keep it because other libraries named LiquidCrystal_I2C have different constructors and backpack mappings.

## Configuration and wiring

Edit `src/Config.h` for pin assignments, keypad mapping, LCD backpack mapping, LED count/brightness, input debounce, display cadence, capture/defuse holds, beep cadence, pulse lengths, alarm and victory melody, duration limits and game defaults. Invalid combinations are rejected at compile time. Screen layouts require a 20x4 display and support up to six code digits and three time digits.

| Device | Default connection |
| --- | --- |
| Buzzer | D10 |
| Blue button | D11 to GND, internal pull-up |
| Red button | D12 to GND, internal pull-up |
| NeoPixel data | D13 |
| Keypad rows | D9, D8, D7, D6 |
| Keypad columns | D5, D4, D3, D2 |
| LCD | I2C address 0x27; Nano/Uno SDA A4, SCL A5 |

Game durations default to zero, requiring a time to be set before starting. The plant code defaults to `1234`. Menu changes remain available between games until power-off/reset; they are not written to EEPROM. Change the defaults in `Config.h` for a different startup configuration.

## Controls and rules

- Main menu: `1` opens game selection. Choose `1` for Domination or `2` for Defuse.
- Setup: `1` starts, `2` edits minutes, `3` edits the code in Defuse, `B` goes back.
- Editors: type digits, `*` deletes the last digit, `A` saves, `B` cancels. Times must be 1–999 minutes; codes are 1–6 digits. Leading zeros in codes are significant. Reaching the digit limit does not save automatically.
- Domination: hold one team's button for eight seconds to capture. An enemy-owned objective becomes neutral after four seconds of that hold. Both buttons together cancel capture progress. Releasing or switching buttons restarts the hold. Scores measure ownership time in milliseconds, displayed as whole seconds.
- Defuse: enter the code and press `#` to plant. The full countdown begins only after a correct code. Hold both buttons continuously for eight seconds to defuse. Releasing either button resets progress. Expiration wins a tie with defuse completion.
- During either active game, hold the red button and press `B` to abort. An abort has its own result, without an explosion or victory. `B` leaves a result screen immediately, including while its sound is playing. `B` also leaves the planting screen.

## Structure and timing

`main.cpp` owns one `Application` and delegates Arduino setup/loop to it.

| File or directory | Responsibility |
| --- | --- |
| `Application.cpp` | Menu transitions, settings editing, input polling and game/effect coordination |
| `ApplicationDisplay.cpp` | Screen composition |
| `Controllers/*GameController.*` | Hardware-independent game rules and state |
| `Controllers/EffectsController.h` | Countdown beep scheduling, alarm and melody sequencing |
| `Controllers/DisplayController.h` | Fixed-size frame buffers and changed-span LCD writes |
| `Core/` | Rollover-safe elapsed timing, cancellable holds and bounded numeric input |
| `Drivers/` | Debounced buttons and scheduled LED pulses |

There are no application `delay()` calls, nested game/menu loops, recursive keypad callbacks or dynamic `String` buffers. Hardware is initialized from `setup()`. Every loop polls input, updates the active game, advances effects and refreshes the display when needed.

Countdowns derive from elapsed milliseconds rather than decrementing a counter once per loop/second. Unsigned subtraction handles `millis()` rollover within a game. Ownership scoring is clamped at the deadline and split at hold thresholds when updates arrive late. Each game resets its own timing and state.

Beep intervals scale with the active game's remaining milliseconds and duration, from 5,000 ms down to 30 ms. The 12 ms tone/light pulses leave an off period at the fastest cadence. Sounds, button holds and LED pulses update independently of the 100 ms display cadence. Normal display updates send only changed spans, avoiding full-screen clears and flicker.

The loop is cooperative: I2C transfers and FastLED's serial pixel transmission still take finite time. NeoPixel transmission on AVR also affects interrupt timing, including buzzer output. The scheduler removes the previous one-second software limit; exact audible/physical timing still needs validation on the assembled hardware.

## Regression tests

On Windows, run:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File test/run.ps1
```

The runner uses GCC/Clang on PATH or discovers Visual Studio C++ Build Tools. Outputs stay under ignored `.pio/host-tests`. No Arduino board or additional test framework is needed.

On Linux/macOS, the same tests can be built with a C++17 compiler:

```sh
mkdir -p .pio/host-tests
c++ -std=c++17 -Wall -Wextra -Werror -Itest/host/fakes -Isrc \
  test/host/main.cpp src/Application.cpp src/ApplicationDisplay.cpp \
  src/Controllers/DefuseGameController.cpp src/Controllers/DominationGameController.cpp \
  -o .pio/host-tests/regression
.pio/host-tests/regression
```

Tests execute production game, input, application, display and effect code against fake Arduino hardware. They cover rollover, late updates, duration overflow, capture conflicts, hold cancellation, restart isolation, code validation, leading zeros, button debounce, rapid beeping during a real countdown, correct blue LED output, unchanged LCD frames, and leaving victory/alarm screens without waiting.

Before field use, check keypad orientation, LCD backpack mapping, button bounce, audible fast beeps and LED colors on the device. Firmware compilation and simulated tests cannot verify the wiring or physical outputs.
