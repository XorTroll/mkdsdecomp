#include <nds.h>
#include <stdio.h>

// NOTE: this is a temporary entrypoint/setup for testing purposes, while the game is being fully reversed
// this will soon be split into ARM7 and ARM9 (and common code), won't make use of libnds at all, etc.

int main(void) {
    consoleDemoInit();

    printf("Hello!\n");
    swiWaitForVBlank();
    while(1);
}