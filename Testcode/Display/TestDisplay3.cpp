#include <pigpiod_if2.h>
#include <iostream>
#include <unistd.h>
#include <vector>

#define BITS 4

#define PIN_RS 27
#define PIN_EN 22

// Define pins D0 through D7
#define PIN_D0 12
#define PIN_D1 16
#define PIN_D2 20
#define PIN_D3 21
#define PIN_D4 18
#define PIN_D5 23
#define PIN_D6 24
#define PIN_D7 25

std::vector<int> PIN_D = {PIN_D0, PIN_D1, PIN_D2, PIN_D3, PIN_D4, PIN_D5, PIN_D6, PIN_D7};

std::map<int, std::string> PIN_NAMES = {
    {PIN_RS, "rs"},
    {PIN_EN, "en"},
    {PIN_D0, "d0"},
    {PIN_D1, "d1"},
    {PIN_D2, "d2"},
    {PIN_D3, "d3"},
    {PIN_D4, "d4"},
    {PIN_D5, "d5"},
    {PIN_D6, "d6"},
    {PIN_D7, "d7"}
};

float DELAY = 1.0e-3; // in seconds

class Display {
public:
    Display() {
        pi = pigpio_start(0, 0);
        if (pi < 0) {
            std::cerr << "Failed to initialize pigpiod" << std::endl;
            return;
        }

        gpio_set_mode(pi, PIN_RS, PI_OUTPUT);
        gpio_set_mode(pi, PIN_EN, PI_OUTPUT);

        for (int pin : PIN_D) {
            gpio_set_mode(pi, pin, PI_OUTPUT);
        }

        //BITS = 4; // Default to 4 bits
        //DELAY = 1.0e-3; // Default delay
    }

    ~Display() {
        pigpio_stop(pi);
    }


    void printMessage(const std::string& message) {
        for (char c : message) {
            write8(c, PI_HIGH);
        }
    }

private:
    int pi;
    int BITS;

    void write4(int value, int rs_val=PI_LOW) {
        // Implement write4
        std::vector<int> signal(4, 0);
        for (int i = 0; i < 4; ++i) {
        signal[i] = ((value >> i) & 1) > 0;
        }

        std::cout << (rs_val ? "1 " : "0 ");
        gpio_write(PIN_RS, rs_val);
        std::cout << "rs=" << rs_val << " ";

        time_sleep(1e-3);

        for (int i = 4; i < 8; ++i) {
            gpio_write(PIN_D[i], signal[i-4]);
            std::cout << PIN_NAMES[PIN_D[i]] << "=" << signal[i-4] << " ";
        }
        std::cout << " CLK" << std::endl;

        clock();
    }

    void write8(int value, int rs_val=PI_LOW, int bits=BITS) {
        // Implement write8
        std::vector<int> signal(8, 0);
    for (int i = 0; i < 8; ++i) {
        signal[i] = ((value >> i) & 1) > 0;
    }

    std::cout << (rs_val ? "1 " : "0 ");
    gpio_write(PIN_RS, rs_val);
    std::cout << "rs=" << rs_val << " ";

    time_sleep(1e-3);

    if (bits == 4) {
        for (int i = 4; i < 8; ++i) {
            gpio_write(PIN_D[i], signal[i-4]);
            std::cout << PIN_NAMES[PIN_D[i]] << "=" << signal[i-4] << " ";
        }

        clock();

        for (int i = 4; i < 8; ++i) {
            gpio_write(PIN_D[i], signal[i-4]);
            std::cout << PIN_NAMES[PIN_D[i]] << "=" << signal[i-4] << " ";
        }
    } else {
        for (int i = 0; i < 4; ++i) {
            gpio_write(PIN_D[i], signal[i]);
            std::cout << PIN_NAMES[PIN_D[i]] << "=" << signal[i] << " ";
        }

        clock();
    }

    std::cout << " CLK" << std::endl;
    }

    void clock() {
        // Implement clock
        time_sleep(DELAY);
        gpio_write(PIN_EN, PI_HIGH);
        time_sleep(DELAY);
        gpio_write(PIN_EN, PI_LOW);
        time_sleep(DELAY);
    }
};

int main(int argc, char *argv[]) {
    Display display;

    if (argc == 2) {
        std::string message(argv[1]);
        display.printMessage(message);
    } else {
        std::cerr << "Invalid arguments" << std::endl;
    }

    return 0;
}
