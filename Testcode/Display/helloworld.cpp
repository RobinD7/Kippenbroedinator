#include <iostream>
#include <pigpiod_if2.h>
#include <unistd.h>
#include <chrono>
#include <ctime>

#define LCD_RS 2
#define LCD_E 3
#define LCD_D4 25
#define LCD_D5 8
#define LCD_D6 7
#define LCD_D7 1

#define LCD_WIDTH 16
#define LCD_CHR true
#define LCD_CMD false

#define LCD_LINE_1 0x80
#define LCD_LINE_2 0xC0

#define E_PULSE 0.00005
#define E_DELAY 0.00005

int pi = pigpio_start(NULL, NULL);

void lcd_byte(int bits, bool mode) {
    gpio_write(pi, LCD_RS, mode);

    gpio_write(pi, LCD_D4, bits & 0x10);
    gpio_write(pi, LCD_D5, bits & 0x20);
    gpio_write(pi, LCD_D6, bits & 0x40);
    gpio_write(pi, LCD_D7, bits & 0x80);

    time_sleep(E_DELAY);
    gpio_write(pi, LCD_E, true);
    time_sleep(E_PULSE);
    gpio_write(pi, LCD_E, false);
    time_sleep(E_DELAY);

    gpio_write(pi, LCD_D4, bits & 0x01);
    gpio_write(pi, LCD_D5, bits & 0x02);
    gpio_write(pi, LCD_D6, bits & 0x04);
    gpio_write(pi, LCD_D7, bits & 0x08);

    time_sleep(E_DELAY);
    gpio_write(pi, LCD_E, true);
    time_sleep(E_PULSE);
    gpio_write(pi, LCD_E, false);
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
    set_mode(pi, LCD_E, PI_OUTPUT);
    set_mode(pi, LCD_RS, PI_OUTPUT);
    set_mode(pi, LCD_D4, PI_OUTPUT);
    set_mode(pi, LCD_D5, PI_OUTPUT);
    set_mode(pi, LCD_D6, PI_OUTPUT);
    set_mode(pi, LCD_D7, PI_OUTPUT);

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
    if (pi < 0) {
        std::cerr << "Failed to initialize pigpio: " << pigpio_error(pi) << std::endl;
        return 1;
    }

    try {
        main_program();
    } catch (...) {
        pigpio_stop(pi);
        throw;
    }

    pigpio_stop(pi);
    return 0;
}
