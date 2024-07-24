#include "display.h"
#include "timer.h"
#include "GPIO.h"

#define DIOPIN 27
#define CLKPIN 22
#define BIT_DELAY 100 // In Microseconds
#define BRIGHTNESS 15 // (Display On << 3) | (Display brightness (0 - 7))
#define TM1637_I2C_COMM1 0x40
#define TM1637_I2C_COMM2 0xC0
#define TM1637_I2C_COMM3 0x80

const uint8_t digitToSegment[] = {
    // XGFEDCBA
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111, // 9
    0b01110111, // A
    0b01111100, // b
    0b00111001, // C
    0b01011110, // d
    0b01111001, // E
    0b01110001  // F
};

uint8_t encodeDigit(uint8_t digit) {
    return digitToSegment[digit & (~SEG_DOT)];
}

void bitDelay() {
    delayMicros(BIT_DELAY);
}

void initDisplay() {
    pinMode(CLKPIN, INPUT);
    pinMode(DIOPIN, INPUT);
    digitalWrite(CLKPIN, LOW);
    digitalWrite(DIOPIN, LOW);
}

void startSignal() {
    pinMode(DIOPIN, OUTPUT);
    bitDelay();
}

void stopSignal() {
    pinMode(DIOPIN, OUTPUT);
    bitDelay();
    pinMode(CLKPIN, INPUT);
    bitDelay();
    pinMode(DIOPIN, INPUT);
    bitDelay();
}

uint8_t writeByte(uint8_t b) {
    uint8_t data = b;

    // 8 Data Bits
    for (uint8_t i = 0; i < 8; i++)
    {
        // CLKPIN low
        pinMode(CLKPIN, OUTPUT);
        bitDelay();

        // Set data bit
        if (data & 0x01)
            pinMode(DIOPIN, INPUT);
        else
            pinMode(DIOPIN, OUTPUT);

        bitDelay();

        // CLKPIN high
        pinMode(CLKPIN, INPUT);
        bitDelay();
        data = data >> 1;
    }

    // Wait for acknowledge
    // CLKPIN to zero
    pinMode(CLKPIN, OUTPUT);
    pinMode(DIOPIN, INPUT);
    bitDelay();

    // CLKPIN to high
    pinMode(CLKPIN, INPUT);
    bitDelay();
    uint8_t ack = digitalRead(DIOPIN);
    if (ack == 0)
        pinMode(DIOPIN, OUTPUT);

    bitDelay();
    pinMode(CLKPIN, OUTPUT);
    bitDelay();

    return ack;
}

void displayRaw(uint8_t *data) {
    // Write COMM1
    startSignal();
    writeByte(TM1637_I2C_COMM1);
    stopSignal();

    // Write COMM2 + first digit address (0)
    startSignal();
    writeByte(TM1637_I2C_COMM2);

    // Write the data bytes
    for (uint8_t k = 0; k < 4; k++)
        writeByte(data[k]);

    stopSignal();

    // Write COMM3 + brightness
    startSignal();
    writeByte(TM1637_I2C_COMM3 + BRIGHTNESS);
    stopSignal();
}

void encodeState(display_state_t* state, uint8_t* data) {
    data[0] = encodeDigit(state->digit0);
    data[1] = encodeDigit(state->digit1);
    data[2] = encodeDigit(state->digit2);
    data[3] = encodeDigit(state->digit3);

    if (state->dots) {
        data[1] |= 0x80;
    }
}

void display(display_state_t *state) {
    uint8_t data[4];
    encodeState(state, data);
    displayRaw(data);
}
