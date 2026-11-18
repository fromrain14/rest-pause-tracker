#include "DisplayUI.h"

DisplayUI::DisplayUI() : display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire) {}

bool DisplayUI::begin() {
    return display.begin(SCREEN_I2C_ADDRESS, true);
}

void DisplayUI::draw(const StateMachine& fsm, uint32_t now, uint8_t batteryPercent) {
    if (now - lastDrawTime < 200) {
        return;
    }
    else {
        if (fsm.getState() == State::READY) {
            display.invertDisplay(true);
        } else {
            display.invertDisplay(false);
        }
        lastDrawTime = now;
        display.clearDisplay();
        display.setTextColor(SH110X_WHITE);
        switch (fsm.getState()) {
            case State::SETUP:
                drawSetup(fsm, batteryPercent);
                break;
            case State::READY:
                drawReady(fsm);
                break;
            case State::REST_RED:
                drawRestRed(fsm, now);
                break;
            case State::REST_ORANGE:
                drawRestOrange(fsm, now);
                break;
            case State::SUMMARY:
                drawSummary(fsm);
                break;
        }
        display.display();
        
    }
}

void DisplayUI::drawSetup(const StateMachine& fsm, uint8_t batteryPercent) {
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("MIN PAUZA");

    // Battery in the top-right corner, right-aligned (text size 1 char = 6 px)
    uint8_t chars = (batteryPercent >= 100) ? 4 : (batteryPercent >= 10) ? 3 : 2; // digits + '%'
    display.setCursor(SCREEN_WIDTH - chars * 6, 0);
    display.print(batteryPercent);
    display.print('%');

    display.setTextSize(3);
    display.setCursor(28, 20);
    printTime(fsm.getMinPauseMs());

    display.setTextSize(1);
    display.setCursor(0, 56);
    display.print("A=START       B=+15s");
}

void DisplayUI::drawRestRed(const StateMachine& fsm, uint32_t now) {
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("PAUZA");

    drawRestBody(fsm, now);
}

void DisplayUI::drawRestOrange(const StateMachine& fsm, uint32_t now) {
    display.setTextSize(1);
    display.setCursor(0, 0);
    if (fsm.getRestElapsedMs(now) >= fsm.getMinPauseMs()) {
        display.print("CEKAM NA TEP");
    } else {
        display.print("CEKAM NA CAS");
    }

    drawRestBody(fsm, now);
}

// Shared part of both rest screens: large elapsed time + bottom line
void DisplayUI::drawRestBody(const StateMachine& fsm, uint32_t now) {
    display.setTextSize(4);
    display.setCursor(16, 18);
    printTime(fsm.getRestElapsedMs(now));

    display.setTextSize(1);
    display.setCursor(0, 56);
    display.print("MIN ");
    printTime(fsm.getMinPauseMs());
    display.print("  HOLD A=SKIP");
}

// Background is inverted by draw() via display.invertDisplay(true)
void DisplayUI::drawReady(const StateMachine& fsm) {
    display.setTextSize(4);
    display.setCursor(28, 16);
    display.print("GO!");

    display.setTextSize(1);
    display.setCursor(0, 56);
    if (fsm.getSetCount() == 0) {
        display.print("A=KONEC SERIE");
    } else {
        display.print("SERIE ");
        display.print(fsm.getSetCount());
        display.print(" HOTOVA");
    }
}

void DisplayUI::drawSummary(const StateMachine& fsm) {
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("SOUHRN");

    display.setTextSize(2);
    display.setCursor(0, 14);
    display.print("SERIE: ");
    display.print(fsm.getSetCount());

    display.setTextSize(1);
    display.setCursor(0, 36);
    display.print("CELKEM:      ");
    printTime(fsm.getWorkoutDurationMs());

    display.setCursor(0, 46);
    display.print("PRUM. PAUZA: ");
    printTime(fsm.getAvgRestMs());

    display.setCursor(0, 56);
    display.print("STISK = NOVY TRENINK");
}

// Prints ms as m:ss at the current cursor position (90000 -> "1:30")
void DisplayUI::printTime(uint32_t ms) {
    uint32_t totalSeconds = ms / 1000;
    uint32_t minutes = totalSeconds / 60;
    uint32_t seconds = totalSeconds % 60;

    display.print(minutes);
    display.print(':');
    if (seconds < 10) {
        display.print('0');
    }
    display.print(seconds);
}


