#include <iostream>
#include <pigpio.h>
#include <unistd.h>

#define LCD_RS 21
#define LCD_E 17
#define LCD_D4 22
#define LCD_D5 23
#define LCD_D6 24
#define LCD_D7 25

#define LCD_WIDTH 16
#define LCD_CHR true
#define LCD_CMD false

#define LCD_LINE_1 0x80
#define LCD_LINE_2 0xC0

#define E_PULSE 0.00005
#define E_DELAY 0.00005

void lcd_byte(int bits, bool mode) {
    gpioWrite(LCD_RS, mode);

    gpioWrite(LCD_D4, bits & 0x10);
    gpioWrite(LCD_D5, bits & 0x20);
    gpioWrite(LCD_D6, bits & 0x40);
    gpioWrite(LCD_D7, bits & 0x80);

    time_sleep(E_DELAY);
    gpioWrite(LCD_E, true);
    time_sleep(E_PULSE);
    gpioWrite(LCD_E, false);
    time_sleep(E_DELAY);

    gpioWrite(LCD_D4, bits & 0x01);
    gpioWrite(LCD_D5, bits & 0x02);
    gpioWrite(LCD_D6, bits & 0x04);
    gpioWrite(LCD_D7, bits & 0x08);

    time_sleep(E_DELAY);
    gpioWrite(LCD_E, true);
    time_sleep(E_PULSE);
    gpioWrite(LCD_E, false);
    time_sleep(E_DELAY);
}

void lcd_init() {
    lcd_byte(0x33, LCD_CMD);
    lcd_byte(0x32, LCD_CMD);
    lcd_byte(0x28, LCD_CMD);
    lcd_byte(0x0C, LCD_CMD);
    lcd_byte(0x06, LCD_CMD);
    lcd_byte(0x01, LCD_CMD);
}

void lcd_string(const std::string& message) {
    std::string paddedMessage = message;
    paddedMessage.resize(LCD_WIDTH, ' ');

    for (char c : paddedMessage) {
        lcd_byte(static_cast<int>(c), LCD_CHR);
    }
}

void main_program() {
    gpioSetMode(LCD_E, PI_OUTPUT);
    gpioSetMode(LCD_RS, PI_OUTPUT);
    gpioSetMode(LCD_D4, PI_OUTPUT);
    gpioSetMode(LCD_D5, PI_OUTPUT);
    gpioSetMode(LCD_D6, PI_OUTPUT);
    gpioSetMode(LCD_D7, PI_OUTPUT);

    lcd_init();

    lcd_byte(LCD_LINE_1, LCD_CMD);
    lcd_string("Raspberry Pi");
    lcd_byte(LCD_LINE_2, LCD_CMD);
    lcd_string("Model B");

    time_sleep(3);

    lcd_byte(LCD_LINE_1, LCD_CMD);
    lcd_string("Raspberrypi-spy");
    lcd_byte(LCD_LINE_2, LCD_CMD);
    lcd_string(".co.uk");

    time_sleep(20);
}

int main() {
    if (gpioInitialise() < 0) {
        std::cerr << "GPIO Initialization failed." << std::endl;
        return 1;
    }

    try {
        main_program();
    } catch (...) {
        gpioTerminate();
        throw;
    }

    gpioTerminate();
    return 0;
}
