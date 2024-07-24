#include <stdint.h>

//
//      A
//     ---
//  F |   | B
//     -G-
//  E |   | C
//     ---
//      D
#define SEG_A   0b00000001
#define SEG_B   0b00000010
#define SEG_C   0b00000100
#define SEG_D   0b00001000
#define SEG_E   0b00010000
#define SEG_F   0b00100000
#define SEG_G   0b01000000
#define SEG_DOT 0b10000000

#define SEG_MINUS = SEG_G


typedef struct {
    uint8_t digit0;
    uint8_t digit1;
    uint8_t digit2;
    uint8_t digit3;
    uint8_t dots;
} display_state_t;

void initDisplay();

// Display HH:mm (d0d1:d2d3)
void display(display_state_t*);

// Display raw data
void displayRaw(uint8_t*);
