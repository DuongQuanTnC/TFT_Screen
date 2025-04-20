#include "stm32f10x.h"
#include "TFT.h"
#include "control.h"
#include <stdio.h>
#include "button.h"

int main(void) {
    TFT_Init();       // Kh?i t?o màn hình
    Systick_Init();
    Button_Init();
    TFT_FillScreen(COLOR_BLUE);            // Màu d? (F800 là RED, không ph?i xanh)
    TFT_DrawString(20, 50, "Duty: ", 0xFFFF, 3); // V? ch? tr?ng
    display();

    while (1);
}
