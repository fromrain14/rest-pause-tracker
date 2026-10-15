#include "DisplayUI.h"

DisplayUI::DisplayUI() : display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire) {}

bool DisplayUI::begin() {
    return display.begin(SCREEN_I2C_ADDRESS, true);
}

void DisplayUI::draw(const StateMachine& fsm, uint32_t now) {
    if (now - lastDrawTime < 200) {
        return;
    }
    else {
        lastDrawTime = now;
        display.clearDisplay();
        // Add drawing logic here
        //nastavit velikost a bravu textu a pozici kursoru
        display.setTextSize(1);
        display.setCursor(0, 0);
        display.setTextColor(SH110X_WHITE);
        display.print("State: ");
        display.println(fsm.printState(fsm.getState()));
        display.display();
    }

}
