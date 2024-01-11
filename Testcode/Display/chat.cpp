#include <iostream>
#include <pigpio.h>
#include <unistd.h>

#define LCD_RS 25  // GPIO pin for Register Select (RS)
#define LCD_E  24  // GPIO pin for Enable (E)
#define LCD_D4 23  // GPIO pin for data bit D4
#define LCD_D5 17  // GPIO pin for data bit D5
#define LCD_D6 18  // GPIO pin for data bit D6
#define LCD_D7 22  // GPIO pin for data bit D7

void LCD_Init();
void LCD_Command(unsigned char cmnd);
void LCD_Char(char data);
void LCD_String(const char* str);
void LCD_Clear();
void LCD_SetCursor(int row, int column);

int main() {
    if (gpioInitialise() < 0) {
        std::cerr << "Error initializing pigpio." << std::endl;
        return 1;
    }

    LCD_Init();

    // Print "Hello, World!" on the LCD
    LCD_String("Hello, World!");

    gpioTerminate();
    return 0;
}

void LCD_Init() {
    gpioSetMode(LCD_RS, PI_OUTPUT);
    gpioSetMode(LCD_E, PI_OUTPUT);
    gpioSetMode(LCD_D4, PI_OUTPUT);
    gpioSetMode(LCD_D5, PI_OUTPUT);
    gpioSetMode(LCD_D6, PI_OUTPUT);
    gpioSetMode(LCD_D7, PI_OUTPUT);

    usleep(15000);

    LCD_Command(0x02);  // Initialize
    LCD_Command(0x28);  // 4-bit mode, 2 lines, 5x8 font
    LCD_Command(0x0C);  // Display on, cursor off, blink off
    LCD_Command(0x06);  // Entry mode, set increment
    LCD_Clear();
}

void LCD_Command(unsigned char cmnd) {
    gpioWrite(LCD_RS, 0);  // RS = 0 for command
    gpioWrite(LCD_D4, (cmnd & 0x10) >> 4);
    gpioWrite(LCD_D5, (cmnd & 0x20) >> 5);
    gpioWrite(LCD_D6, (cmnd & 0x40) >> 6);
    gpioWrite(LCD_D7, (cmnd & 0x80) >> 7);
    gpioWrite(LCD_E, 1);
    usleep(1000);
    gpioWrite(LCD_E, 0);

    usleep(1000);

    gpioWrite(LCD_D4, (cmnd & 0x01));
    gpioWrite(LCD_D5, (cmnd & 0x02) >> 1);
    gpioWrite(LCD_D6, (cmnd & 0x04) >> 2);
    gpioWrite(LCD_D7, (cmnd & 0x08) >> 3);
    gpioWrite(LCD_E, 1);
    usleep(1000);
    gpioWrite(LCD_E, 0);

    usleep(1000);
}

void LCD_Char(char data) {
    gpioWrite(LCD_RS, 1);  // RS = 1 for data
    gpioWrite(LCD_D4, (data & 0x10) >> 4);
    gpioWrite(LCD_D5, (data & 0x20) >> 5);
    gpioWrite(LCD_D6, (data & 0x40) >> 6);
    gpioWrite(LCD_D7, (data & 0x80) >> 7);
    gpioWrite(LCD_E, 1);
    usleep(1000);
    gpioWrite(LCD_E, 0);

    usleep(1000);

    gpioWrite(LCD_D4, (data & 0x01));
    gpioWrite(LCD_D5, (data & 0x02) >> 1);
    gpioWrite(LCD_D6, (data & 0x04) >> 2);
    gpioWrite(LCD_D7, (data & 0x08) >> 3);
    gpioWrite(LCD_E, 1);
    usleep(1000);
    gpioWrite(LCD_E, 0);

    usleep(1000);
}

void LCD_String(const char* str) {
    while (*str != '\0') {
        LCD_Char(*str);
        str++;
    }
}

void LCD_Clear() {
    LCD_Command(0x01);  // Clear display
    usleep(2000);
}

void LCD_SetCursor(int row, int column) {
    int position = column + (row == 1 ? 0x40 : 0x00);
    LCD_Command(0x80 | position);
}
