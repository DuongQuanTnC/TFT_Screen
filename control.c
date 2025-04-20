#include "control.h"
#include <stdio.h>
#include "TFT.h"

volatile uint16_t duty = 50;
volatile uint16_t freq = 1000;


void increaseDuty(void) {
    if (duty < 1000) duty += 10;
}

void decreaseDuty(void) {
    if (duty > 50) duty -= 10;
}

void increaseFreq(void) {
    if (freq < 20000) freq += 100;
}

void decreaseFreq(void) {
    if (freq > 100) freq -= 100;
}

void display() {
    char str[20];
    sprintf(str, "%d", duty); 
    clearString(str, 150, 50, 3);
    TFT_DrawString(150, 50, str, 0xFFFF, 3);
}