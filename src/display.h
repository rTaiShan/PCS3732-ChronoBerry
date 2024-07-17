#include <stdint.h>

typedef struct {
    uint8_t digit0;
    uint8_t digit1;
    uint8_t digit2;
    uint8_t digit3;
    uint8_t dots;
} display_state_t;

// Display HH:mm
void display(display_state_t*);
