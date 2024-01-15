// g++ -g Testmotor.cpp -o Testmotor -lpigpiod_if2 -lrt -lpigpio

#include <pigpiod_if2.h>
#include <unistd.h>
#include <iostream>

#define LCD_LINE_1 = 0x80 # LCD RAM address for the 1st line
#define LCD_LINE_2 = 0xC0 # LCD RAM address for the 2nd line 

//Timing constants
#define E_PULSE = 0.00005
#define E_DELAY = 0.00005

int pi = pigpio_start(NULL, NULL);

class Display {
public:
    Display();
    ~Display();

    void initialize();
    void sendCommand(uint8_t command);
    void sendData(uint8_t data);
    void sendString(const char* str);
    void clearDisplay();
    void setPosition(uint8_t row, uint8_t col);

private:
    int pi;
    int rs;
    int e;
    int d4;
    int d5;
    int d6;
    int d7;
};

Display::Display() {
    rs = 2;  // LCD_RS = 2
    e = 3;   // LCD_E  = 3
    d4 = 25; // LCD_D4 = 25
    d5 = 8;  // LCD_D5 = 8
    d6 = 7;  // LCD_D6 = 7
    d7 = 1;  // LCD_D7 = 1
}

Display::~Display() {
    pigpio_stop(pi);
}

void Display::initialize() {
    std::cout << "Initialize...\n";
    set_mode(pi, rs, PI_OUTPUT);
    set_mode(pi, e, PI_OUTPUT);
    set_mode(pi, d4, PI_OUTPUT);
    set_mode(pi, d5, PI_OUTPUT);
    set_mode(pi, d6, PI_OUTPUT);
    set_mode(pi, d7, PI_OUTPUT);
    
    gpio_write(pi, rs, PI_LOW);
    gpio_write(pi, e, PI_LOW);

    usleep(50000); // 50ms delay

    sendCommand(0x33);
    sendCommand(0x32);
    sendCommand(0x28); // Function set: 4-bit, 2-line, 5x8 font
    sendCommand(0x0C); // Display ON, Cursor OFF, Blink OFF
    sendCommand(0x06); // Entry mode set: Increment, no shift
    sendCommand(0x01); // Clear display

    usleep(2000); // 2ms delay
}

void Display::sendCommand(uint8_t command) {
    gpio_write(pi, rs, PI_LOW);
    sendData(command);
}

void Display::sendData(uint8_t data) {
    std::cout << "Send data...\n";
    gpio_write(pi, d4, (data & 0x10) >> 4);
    gpio_write(pi, d5, (data & 0x20) >> 5);
    gpio_write(pi, d6, (data & 0x40) >> 6);
    gpio_write(pi, d7, (data & 0x80) >> 7);

    gpio_write(pi, e, PI_HIGH);
    usleep(1000);
    gpio_write(pi, e, PI_LOW);
    usleep(1000);

    gpio_write(pi, d4, (data & 0x01));
    gpio_write(pi, d5, (data & 0x02) >> 1);
    gpio_write(pi, d6, (data & 0x04) >> 2);
    gpio_write(pi, d7, (data & 0x08) >> 3);

    gpio_write(pi, e, PI_HIGH);
    usleep(1000);
    gpio_write(pi, e, PI_LOW);
    usleep(1000);
    }

void Display::sendString(const char* str) {
    std::cout << "Send string...\n";
    while (*str != '\0') {
        std::cout << "Sending...\n";
        sendData(*str);
        str++;
    }
}

void Display::clearDisplay() {
    std::cout << "Clear...\n";
    sendCommand(0x01); // Clear display
    usleep(2000); // 2ms delay
}

void Display::setPosition(uint8_t row, uint8_t col) {
    std::cout << "Set position...\n";
    uint8_t address = 0x80;

    switch (row) {
    case 1:
        address += 0x40;
        break;
    case 2:
        address += 0x14;
        break;
    case 3:
        address += 0x54;
        break;
    }

    address += col;
    sendCommand(address);
}

int main() {
    
    if (pi < 0) {
        std::cerr << "Failed to initialize pigpio: " << pigpio_error(pi) << std::endl;
        return 1;
    }
    
    Display lcd;
    lcd.initialize();
    lcd.clearDisplay();
    lcd.setPosition(1, 0); // Set cursor to the first row, first column
    lcd.sendString("Hello");

    return 0;
}
