#include <pigpiod_if2.h>
#include <iostream>
#include <unistd.h>

// Define GPIO to LCD mapping
#define LCD_RS 21
#define LCD_E  17
#define LCD_D4 22
#define LCD_D5 23
#define LCD_D6 24
#define LCD_D7 25

// Define some device constants
#define LCD_WIDTH 16    // Maximum characters per line
#define LCD_CHR true
#define LCD_CMD false

#define LCD_LINE_1 0x80 // LCD RAM address for the 1st line
#define LCD_LINE_2 0xC0 // LCD RAM address for the 2nd line

// Timing constants
#define E_PULSE 0.00005
#define E_DELAY 0.00005

// Function declarations
void lcd_init(int pi);
void lcd_string(int pi, const char* message);
void lcd_byte(int pi, int bits, bool mode);

int main() {
    // Connect to pigpiod daemon
    int pi = pigpio_start(NULL, NULL);
    if (pi < 0) {
        std::cerr << "Failed to connect to pigpiod daemon" << std::endl;
        return 1;
    }

    // Initialise display
    lcd_init(pi);

    // Send some test
    lcd_byte(pi, LCD_LINE_1, LCD_CMD);
    lcd_string(pi, "Raspberry Pi");
    lcd_byte(pi, LCD_LINE_2, LCD_CMD);
    lcd_string(pi, "Model B");

    usleep(3000000); // 3-second delay

    // Send some text
    lcd_byte(pi, LCD_LINE_1, LCD_CMD);
    lcd_string(pi, "Raspberrypi-spy");
    lcd_byte(pi, LCD_LINE_2, LCD_CMD);
    lcd_string(pi, ".co.uk");

    usleep(20000000); // 20-second delay

    // Disconnect from pigpiod daemon
    pigpio_stop(pi);

    return 0;
}

void lcd_init(int pi) {
    // Initialise display
    lcd_byte(pi, 0x33, LCD_CMD);
    lcd_byte(pi, 0x32, LCD_CMD);
    lcd_byte(pi, 0x28, LCD_CMD);
    lcd_byte(pi, 0x0C, LCD_CMD);
    lcd_byte(pi, 0x06, LCD_CMD);
    lcd_byte(pi, 0x01, LCD_CMD);
}

void lcd_string(int pi, const char* message) {
    // Send string to display
    for (int i = 0; i < LCD_WIDTH; ++i) {
        lcd_byte(pi, message[i], LCD_CHR);
    }
}

void lcd_byte(int pi, int bits, bool mode) {
    // Send byte to data pins
    // bits = data
    // mode = true for character, false for command

    // High bits
    gpio_write(pi, LCD_RS, mode);
    gpio_write(pi, LCD_D4, (bits & 0x10) == 0x10);
    gpio_write(pi, LCD_D5, (bits & 0x20) == 0x20);
    gpio_write(pi, LCD_D6, (bits & 0x40) == 0x40);
    gpio_write(pi, LCD_D7, (bits & 0x80) == 0x80);

    // Toggle 'Enable' pin
    usleep(E_DELAY);
    gpio_write(pi, LCD_E, true);
    usleep(E_PULSE);
    gpio_write(pi, LCD_E, false);
    usleep(E_DELAY);

    // Low bits
    gpio_write(pi, LCD_D4, (bits & 0x01) == 0x01);
    gpio_write(pi, LCD_D5, (bits & 0x02) == 0x02);
    gpio_write(pi, LCD_D6, (bits & 0x04) == 0x04);
    gpio_write(pi, LCD_D7, (bits & 0x08) == 0x08);

    // Toggle 'Enable' pin
    usleep(E_DELAY);
    gpio_write(pi, LCD_E, true);
    usleep(E_PULSE);
    gpio_write(pi, LCD_E, false);
    usleep(E_DELAY);
}
