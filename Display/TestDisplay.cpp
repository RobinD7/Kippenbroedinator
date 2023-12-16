#include <pigpiod_if2.h>
#include <iostream>
#include <unistd.h>

#define LCD_pinRS 2
#define LCD_pinRW 
#define LCD_pinE 3
#define LCD_pinD4 17
#define LCD_pinD5 27
#define LCD_pinD6 22
#define LCD_pinD7 10

int pi = pigpio_start(NULL, NULL); // Connect to the pigpiod daemon

class Display {
public:
    Display();
    ~Display();
    
    void initialize();
    void clear();
    void sendCommand(uint8_t command);
    void sendData(uint8_t data);
    void print(const char* message);
private: 
    void sendNibble(uint8_t value);
    void sendByte(uint8_t value);
    void pulseEnable();
};

Display::Display() {
    if (pi < 0) {
        std::cerr << "Failed to initialize pigpiod_if2" << std::endl;
        std::exit(EXIT_FAILURE);
        }

    initialize();
}

Display::~Display() {
    pigpio_stop(pi);
    }

void Display::initialize() {
    // Initialize LCD
    gpio_write(pi, LCD_pinRS, 0);  // Set RS to command mode
    sleep_ms(50);               // Wait for initialization

    sendNibble(0x20);  // 4-bit mode
    sleep_us(100);

    // Initialize LCD in 4-bit mode
    sendCommand(0x28);  // 4-bit, 2 lines, 5x8 font
    sendCommand(0x0C);  // Display ON, cursor OFF, blink OFF
    sendCommand(0x06);  // Entry mode set: increment
    clear();
    }

void Display::clear() {
    sendCommand(0x01);  // Clear display
    sleep_ms(2);
    }

void Display::sendCommand(uint8_t command) {
    gpio_write(pi, LCD_pinRS, 0);  // Command mode
    sendByte(command);
    }

void Display::sendData(uint8_t data) {
    gpio_write(pi, LCD_pinRS, 1);  // Data mode
    sendByte(data);
    }

void Display::print(const char* message) {
    while (*message) {
        sendData(static_cast<uint8_t>(*message));
        ++message;
        }
    }

void Display::sendNibble(uint8_t value) {
    gpio_write(pi, LCD_pinD4, (value >> 0) & 1);
    gpio_write(pi, LCD_pinD5, (value >> 1) & 1);
    gpio_write(pi, LCD_pinD6, (value >> 2) & 1);
    gpio_write(pi, LCD_pinD7, (value >> 3) & 1);
    pulseEnable();
    }

void Display::sendByte(uint8_t value) {
    sendNibble((value >> 4) & 0x0F);
    sendNibble(value & 0x0F);
    }

void Display::pulseEnable() {
    gpio_write(pi, LCD_pinE, 1);
    usleep(1);  // 1 microsecond
    gpio_write(pi, LCD_pinE, 0);
    usleep(1);  // 1 microsecond
    }


int main() {
    Display lcd;

    lcd.print("Hello, World!");

    return 0;
}