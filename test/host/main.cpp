#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Application.h"

#define CHECK(expression) do { if (!(expression)) { \
    fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #expression); exit(1); } } while (0)

void testTiming() {
    CHECK(Timing::elapsed(10, UINT32_MAX - 9) == 20);
    CHECK(Timing::remaining(10, UINT32_MAX - 9, 100) == 80);
    CHECK(Timing::remaining(500, 100, 200) == 0);
    CHECK(Timing::seconds(60000) == 60);
    CHECK(Timing::seconds(59999) == 60);
    CHECK(Timing::seconds(1) == 1);
    CHECK(Timing::seconds(0) == 0);
    CHECK(Timing::beepInterval(60000, 60000, 30, 5000) == 5000);
    CHECK(Timing::beepInterval(0, 60000, 30, 5000) == 30);
    CHECK(Timing::beepInterval(30000, 60000, 30, 5000) == 2515);
    CHECK(Timing::beepInterval(100, 0, 30, 5000) == 30);
    CHECK(Timing::beepInterval(59940000, 59940000, 30, 5000) == 5000);
}

void testDefuse() {
    DefuseGameController game;
    game.start(60000);
    CHECK(!game.plant("", "1234", 500));
    CHECK(!game.plant("123", "1234", 500));
    CHECK(!game.plant("01234", "1234", 500));
    CHECK(game.plant("1234", "1234", 10000));
    CHECK(game.remaining(10000) == 60000);
    game.update(10000, true, true);
    game.update(17000, true, true);
    CHECK(game.progress() == 87);
    game.update(17001, false, true);
    CHECK(game.progress() == 0);
    game.update(18000, true, true);
    game.update(25999, true, true);
    CHECK(game.state() == DefuseGameController::State::Armed);
    game.update(26000, true, true);
    CHECK(game.state() == DefuseGameController::State::Defused);
    CHECK(game.defusedAfter() == 16000);
    game.start(8000);
    CHECK(game.plant("0012", "0012", 0));
    game.update(0, true, true);
    game.update(8000, true, true);
    CHECK(game.state() == DefuseGameController::State::Exploded);
    game.start(60000);
    CHECK(game.plant("1", "1", UINT32_MAX - 3999));
    game.update(UINT32_MAX - 3999, true, true);
    game.update(4000, true, true);
    CHECK(game.state() == DefuseGameController::State::Defused);
    game.start(60000);
    CHECK(game.state() == DefuseGameController::State::Planting);
    game.stop();
    CHECK(game.state() == DefuseGameController::State::Aborted);
    CHECK(!game.plant("1", "1", 0));
}

void testDomination() {
    DominationGameController game;
    game.start(0, 60000);
    game.update(0, true, false);
    game.update(7999, true, false);
    CHECK(game.owner() == Team::Neutral);
    game.update(8000, true, false);
    CHECK(game.owner() == Team::Blue);
    game.update(9000, false, true);
    game.update(18000, false, true); // Late poll crosses neutralization and capture.
    CHECK(game.owner() == Team::Red);
    CHECK(game.blueTime() == 5000);
    CHECK(game.redTime() == 1000);
    game.update(70000, false, false); // Clamp scoring to deadline.
    CHECK(!game.running());
    CHECK(game.redTime() == 43000);
    game.start(100000, 60000);
    CHECK(game.owner() == Team::Neutral);
    CHECK(game.blueTime() == 0 && game.redTime() == 0);
    game.update(100000, true, false);
    game.update(107000, true, true); // Conflict cancels hold.
    CHECK(game.progress() == 0);
    game.update(108000, false, true); // Switching teams starts a fresh hold.
    game.update(115999, false, true);
    CHECK(game.owner() == Team::Neutral);
    game.update(116000, false, true);
    CHECK(game.owner() == Team::Red);
    game.stop();
    game.update(120000, false, true);
    CHECK(game.redTime() == 0);
    game.start(0, 59940000); // 999 minutes must not overflow an AVR's 16-bit int.
    CHECK(game.remaining(60000) == 59880000);
    game.start(UINT32_MAX - 3999, 10000);
    game.update(UINT32_MAX - 3999, true, false);
    game.update(4000, true, false);
    CHECK(game.owner() == Team::Blue);
    game.update(7000, true, false);
    CHECK(!game.running());
    CHECK(game.blueTime() == 2000);
}

void testInputAndButtons() {
    DigitInput input;
    CHECK(input.empty());
    for (char digit : "001234") if (digit) CHECK(input.append(digit));
    CHECK(!input.append('5'));
    CHECK(!input.append('B'));
    CHECK(strcmp(input.text(), "001234") == 0);
    input.erase();
    CHECK(strcmp(input.text(), "00123") == 0);
    input.clear();
    CHECK(!input.append('A'));
    CHECK(input.append('9', 3) && input.append('9', 3) && input.append('9', 3));
    CHECK(!input.append('9', 3));
    CHECK(input.number() == 999);
    Button button(Config::redButtonPin);
    button.begin();
    Fake::pins[Config::redButtonPin] = true;
    button.update(100);
    button.update(124);
    CHECK(!button.pressed());
    button.update(125);
    CHECK(button.pressed());
    Fake::pins[Config::redButtonPin] = false;
    button.update(126);
    button.update(151);
    CHECK(!button.pressed());
}

void testEffectsAndDisplay() {
    LEDDriver leds;
    leds.begin();
    EffectsController effects(leds);
    effects.begin();
    Fake::sounds.clear();
    Fake::now = 0;
    effects.countdown(0);
    for (Fake::now = 1; Fake::now < 1000; ++Fake::now)
        effects.update(Fake::now, 1, 60000, Light::Blue);
    CHECK(Fake::sounds.size() >= 30); // Real scheduler produces >1 beep/sec.
    CHECK(Fake::sounds[1].at - Fake::sounds[0].at == Config::fastestBeepMs);
    leds.pulse(Light::Blue, 1000);
    CHECK(Fake::stripColor.b == 255 && Fake::stripColor.g == 0);
    leds.update(1012);
    CHECK(Fake::stripColor.b == 0);
    Fake::sounds.clear();
    effects.countdown(59000);
    for (Fake::now = 59001; Fake::now < 60000; ++Fake::now)
        effects.update(Fake::now, 60000 - Fake::now, 60000, Light::Red);
    CHECK(Fake::sounds.size() > 10); // Actual final second of a one-minute countdown.
    effects.stop();
    const auto count = Fake::sounds.size();
    effects.update(2000, 100, 60000, Light::Red);
    CHECK(Fake::sounds.size() == count && Fake::frequency == 0);
    effects.alarm(3000);
    effects.update(5999, 0, 0, Light::Off);
    CHECK(Fake::frequency == Config::alarmFrequency);
    effects.update(6000, 0, 0, Light::Off);
    CHECK(Fake::frequency == 0);
    Fake::sounds.clear();
    effects.victory(7000);
    effects.update(7180, 0, 0, Light::Off);
    effects.update(7360, 0, 0, Light::Off);
    effects.update(7610, 0, 0, Light::Off);
    CHECK(Fake::sounds.size() == 4);
    effects.update(7960, 0, 0, Light::Off);
    CHECK(Fake::frequency == 0);

    DisplayController display;
    display.begin();
    display.text(0, 0, F("A long label"));
    display.flush();
    const auto writes = Fake::lcdWrites;
    display.flush();
    CHECK(Fake::lcdWrites == writes); // Unchanged frames perform no LCD writes.
    display.clearFrame();
    display.text(0, 0, F("Short"));
    display.flush();
    CHECK(strcmp(Fake::lcd[0], "Short               ") == 0);
}

void tick(Application& app, uint32_t at, char key = 0) {
    Fake::now = at;
    Fake::key = key;
    app.update();
}
void testApplication() {
    memset(Fake::pins, 0, sizeof(Fake::pins));
    Fake::now = 0;
    Application app;
    app.begin();
    tick(app, 1, '1');
    tick(app, 2, '2'); // Defuse setup.
    tick(app, 3, '1');
    CHECK(strstr(Fake::lcd[3], "Set time") != nullptr);
    tick(app, 4, '2');
    tick(app, 5, '0');
    tick(app, 6, 'A');
    CHECK(strstr(Fake::lcd[2], "valid") != nullptr);
    tick(app, 7, '*');
    tick(app, 8, '1');
    tick(app, 9, 'A');
    tick(app, 10, '3');
    tick(app, 11, '0');
    tick(app, 12, '0');
    tick(app, 13, '1');
    tick(app, 14, '2');
    tick(app, 15, 'A');
    tick(app, 16, '1');
    tick(app, 17, '9');
    tick(app, 18, '#');
    CHECK(strstr(Fake::lcd[1], "Wrong code") != nullptr);
    tick(app, 19, '0');
    tick(app, 20, '0');
    tick(app, 21, '1');
    tick(app, 22, '2');
    tick(app, 23, '#');
    CHECK(strstr(Fake::lcd[0], "ARMED") != nullptr);
    tick(app, 24, 'B'); // Red is not held: cannot abort.
    CHECK(strstr(Fake::lcd[0], "ARMED") != nullptr);
    Fake::pins[Config::redButtonPin] = true;
    tick(app, 25);
    tick(app, 50);
    tick(app, 51, 'B');
    CHECK(strstr(Fake::lcd[0], "Aborted") != nullptr);
    tick(app, 52, 'B');
    tick(app, 53, '1');
    tick(app, 54, 'B'); // Can leave planting without getting trapped.
    CHECK(strstr(Fake::lcd[0], "Defuse Mode") != nullptr);
    Fake::pins[Config::redButtonPin] = false;
    tick(app, 55, '1');
    tick(app, 56, '0');
    tick(app, 57, '0');
    tick(app, 58, '1');
    tick(app, 59, '2');
    tick(app, 60, '#');
    Fake::pins[Config::blueButtonPin] = true;
    Fake::pins[Config::redButtonPin] = true;
    tick(app, 61);
    tick(app, 86);
    tick(app, 8086);
    CHECK(strstr(Fake::lcd[0], "DEFUSED") != nullptr);
    tick(app, 8087, 'B'); // Victory tune must not block leaving the result.
    CHECK(strstr(Fake::lcd[0], "Defuse Mode") != nullptr && Fake::frequency == 0);
    tick(app, 8088, '1');
    tick(app, 8089, '0');
    tick(app, 8090, '0');
    tick(app, 8091, '1');
    tick(app, 8092, '2');
    tick(app, 8093, '#');
    memset(Fake::pins, 0, sizeof(Fake::pins));
    tick(app, 8094);
    tick(app, 8119);
    tick(app, 68093);
    CHECK(strstr(Fake::lcd[0], "EXPLODED") != nullptr);
    tick(app, 68094, 'B'); // Alarm must not block leaving the result.
    CHECK(strstr(Fake::lcd[0], "Defuse Mode") != nullptr && Fake::frequency == 0);
}

int main() {
    testTiming();
    testDefuse();
    testDomination();
    testInputAndButtons();
    testEffectsAndDisplay();
    testApplication();
    puts("PASS: 6 regression groups (timing, defuse, domination, input, effects/display, application)");
}
