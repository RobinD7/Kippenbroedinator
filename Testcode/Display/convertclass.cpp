#include <pigpiod_if2.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <bitset>

// Define GPIO to LCD mapping
#define LCD_RS 2
#define LCD_E  3
#define LCD_D4 25
#define LCD_D5 8
#define LCD_D6 7
#define LCD_D7 1

#define LCD_LINE_1 0x80
#define LCD_LINE_2 0xC0

int pi = pigpio_start(NULL, NULL);

class Display {
public:
    Display();
    ~Display();

    void init();
    void print(const int row, const char *line1);

private:
    static const int LCD_WIDTH = 16;
    static const int LCD_CHR = 1;
    static const int LCD_CMD = 0;

    //static const int LCD_LINE_1 = 0x80;
    //static const int LCD_LINE_2 = 0xC0;

    static const int E_PULSE = 1000;
    static const int E_DELAY = 1000;

    //int pi;
    int lcd_handle;

    void byte(int bits, int mode);
    void string(const char *message);
};

Display::Display() {
    // Set up GPIO
    set_mode(pi, LCD_E, PI_OUTPUT);
    set_mode(pi, LCD_RS, PI_OUTPUT);
    set_mode(pi, LCD_D4, PI_OUTPUT);
    set_mode(pi, LCD_D5, PI_OUTPUT);
    set_mode(pi, LCD_D6, PI_OUTPUT);
    set_mode(pi, LCD_D7, PI_OUTPUT);

    // Initialize display
    init();
}

Display::~Display() {
    // Clean up and close connection to pigpiod
    pigpio_stop(pi);
}

void Display::init() {
    // Initialize display
    //byte(0x33, LCD_CMD);
    //byte(0x32, LCD_CMD);
    byte(0x28, LCD_CMD);//4 bit, 2 line display, 5x8 dot matrix
    byte(0x0C, LCD_CMD);//display on, cursor off
    byte(0x06, LCD_CMD);//entry mode set
    byte(0x01, LCD_CMD);//clear display
}

void Display::print(const int row, const char *line1) {
    // Send some text to the display
    byte(row, LCD_CMD);
    std::cerr << "string 1 " << std::endl;
    string(line1);

    sleep(20);
}

void Display::byte(int bits, int mode) {
    // Send byte to data pins
    // bits = data
    // mode = true  for character
    //        false for command
	std::bitset<8> x(bits);
	std::cout << x << '\n';
    
    //int filter = (bits & 0x08) == 0x08;
    //std::bitset<8> x(filter);
	//std::cout << x << '\n';
    
    gpio_write(pi, LCD_RS, mode); // RS

    // High bits
    gpio_write(pi, LCD_D4, (bits & 0x10) == 0x10);
    gpio_write(pi, LCD_D5, (bits & 0x20) == 0x20);
    gpio_write(pi, LCD_D6, (bits & 0x40) == 0x40);
    gpio_write(pi, LCD_D7, (bits & 0x80) == 0x80);
    
    // Toggle 'Enable' pin
    usleep(E_DELAY);
    gpio_write(pi, LCD_E, 1);
    usleep(E_PULSE);
    gpio_write(pi, LCD_E, 0);
    usleep(E_DELAY);

    // Low bits
    gpio_write(pi, LCD_D4, (bits & 0x01) == 0x01);
    gpio_write(pi, LCD_D5, (bits & 0x02) == 0x02);
    gpio_write(pi, LCD_D6, (bits & 0x04) == 0x04);
    gpio_write(pi, LCD_D7, (bits & 0x08) == 0x08);

	

    // Toggle 'Enable' pin
    usleep(E_DELAY);
    gpio_write(pi, LCD_E, 1);
    usleep(E_PULSE);
    gpio_write(pi, LCD_E, 0);
    usleep(E_DELAY);
}

void Display::string(const char *message) {
    
    // Send string to display

    std::string paddedMessage(message);
    paddedMessage.resize(LCD_WIDTH, ' ');

    for (size_t i = 0; i < LCD_WIDTH; ++i) {
        byte(static_cast<int>(paddedMessage[i]), LCD_CHR);
    
	}

    
    /*
    // Send string to display
    size_t len = strlen(message);
    char paddedMessage[LCD_WIDTH + 1];

    strncpy(paddedMessage, message, LCD_WIDTH);
    paddedMessage[LCD_WIDTH] = '\0';

    for (size_t i = 0; i < LCD_WIDTH; ++i) {
        byte(paddedMessage[i], LCD_CHR);
    }
    */
}

int main() {
	if (pi < 0) {
        std::cerr << "Failed to initialize pigpio: " << pigpio_error(pi) << std::endl;
        return 1;
    }
	
    Display display;
    //display.print("α 7GWgw 8HXhx", "Model B");
	display.print(LCD_LINE_1, "Water % Dag");
    return 0;
}
