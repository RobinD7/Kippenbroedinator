#include <pigpiod_if2.h>
#include <unistd.h>

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
    int pi_;
    int rs_;
    int e_;
    int d4_;
    int d5_;
    int d6_;
    int d7_;
};

Display::Display() {
    pi_ = pigpio_start(NULL, NULL);
    if (pi_ < 0) {
        fprintf(stderr, "Unable to initialize pigpio\n");
        exit(EXIT_FAILURE);
    }

    rs_ = 2;  // LCD_RS = 2
    e_ = 3;   // LCD_E  = 3
    d4_ = 25; // LCD_D4 = 25
    d5_ = 8;  // LCD_D5 = 8
    d6_ = 7;  // LCD_D6 = 7
    d7_ = 1;  // LCD_D7 = 1
}

Display::~Display() {
    pigpio_stop(pi_);
}

void Display::initialize() {
    gpio_write(pi_, rs_, PI_LOW);
    gpio_write(pi_, e_, PI_LOW);

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
    gpio_write(pi_, rs_, PI_LOW);
    sendData(command);
}

void Display::sendData(uint8_t data) {
    gpio_write(pi_, d4_, (data & 0x10) >> 4);
    gpio_write(pi_, d5_, (data & 0x20) >> 5);
    gpio_write(pi_, d6_, (data & 0x40) >> 6);
    gpio_write(pi_, d7_, (data & 0x80) >> 7);

    gpio_write(pi_, e_, PI_HIGH);
    usleep(1);
    gpio_write(pi_, e_, PI_LOW);
    usleep(1);

    gpio_write(pi_, d4_, (data & 0x01));
    gpio_write(pi_, d5_, (data & 0x02) >> 1);
    gpio_write(pi_, d6_, (data & 0x04) >> 2);
    gpio_write(pi_, d7_, (data & 0x08) >> 3);

    gpio_write(pi_, e_, PI_HIGH);
    usleep(1);
    gpio_write(pi_, e_, PI_LOW);
    usleep(1);
}

void Display::sendString(const char* str) {
    while (*str) {
        sendData(*str++);
    }
}

void Display::clearDisplay() {
    sendCommand(0x01); // Clear display
    usleep(2000); // 2ms delay
}

void Display::setPosition(uint8_t row, uint8_t col) {
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
    Display lcd;
    lcd.initialize();
    lcd.clearDisplay();
    lcd.setPosition(1, 0); // Set cursor to the first row, first column
    lcd.sendString("Hello, World!");

    return 0;
}
