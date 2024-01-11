#include <iostream>
#include <pigpio.h>
#include <unistd.h>

#define BITS 4

#define PIN_RS 27
#define PIN_EN 22
#define PIN_D {12, 16, 20, 21, 18, 23, 24, 25}

#define DELAY 1.0e-3

void clock() {
    time_sleep(DELAY);
    gpioWrite(PIN_EN, 1);
    time_sleep(DELAY);
    gpioWrite(PIN_EN, 0);
    time_sleep(DELAY);
}

void write4(int value, int rs_val) {
    int signal[4] = {
        (value >> 0) & 1,
        (value >> 1) & 1,
        (value >> 2) & 1,
        (value >> 3) & 1
    };

    gpioWrite(PIN_RS, rs_val);
    std::cout << "rs=" << rs_val << " ";

    time_sleep(1e-3);

    for (int i = 4; i < 8; ++i) {
        gpioWrite(PIN_D[i], signal[i - 4]);
        std::cout << PIN_D[i] << "=" << signal[i - 4] << " ";
    }
    std::cout << " CLK" << std::endl;

    clock();
}

void write8(int value, int rs_val, int bits = BITS) {
    int signal[8] = {
        (value >> 0) & 1,
        (value >> 1) & 1,
        (value >> 2) & 1,
        (value >> 3) & 1,
        (value >> 4) & 1,
        (value >> 5) & 1,
        (value >> 6) & 1,
        (value >> 7) & 1
    };

    gpioWrite(PIN_RS, rs_val);
    std::cout << "rs=" << rs_val << " ";

    time_sleep(1e-3);

    if (bits == 4) {
        gpioWrite(PIN_D[4], signal[0]);
        gpioWrite(PIN_D[5], signal[1]);
        gpioWrite(PIN_D[6], signal[2]);
        gpioWrite(PIN_D[7], signal[3]);
        clock();

        for (int i = 4; i < 8; ++i) {
            gpioWrite(PIN_D[i], signal[i - 4]);
            std::cout << PIN_D[i] << "=" << signal[i - 4] << " ";
        }
    } else {
        for (int i = 0; i < 4; ++i) {
            gpioWrite(PIN_D[i], signal[i]);
            std::cout << PIN_D[i] << "=" << signal[i] << ", ";
        }
    }

    clock();
    std::cout << " CLK" << std::endl;
}

void prog_printmsg(const std::string& msg) {
    write4(0b0011, GPIO_LOW);
    write4(0b0011, GPIO_LOW);
    write4(0b0011, GPIO_LOW);

    if (BITS == 4) {
        write4(0b0010, GPIO_LOW);
        write8(0b00100000, GPIO_LOW);
    } else {
        write8(0b00110000, GPIO_LOW);
    }

    write8(0b00100000 | 0b00001100, GPIO_LOW);
    write8(0b00001111, GPIO_LOW);
    write8(0b00000110, GPIO_LOW);
    write8(0b00000001, GPIO_LOW);

    for (char c : msg) {
        write8(static_cast<int>(c), GPIO_HIGH);
    }
}

void prog_arbitrary() {
    std::cout << "Input can be one of the following:" << std::endl;
    std::cout << "  X YYYY ZZZZ = RS=X, YYYY=d7-d4, clock, ZZZZ=d7-d4 (4-bit interface)" << std::endl;
    std::cout << "  X YYYYYYYY  = RS=X, YYYYYYYY=d7-d0 (8-bit interface)" << std::endl;
    std::cout << "  1 X         = RS=1, X is the character to print\n" << std::endl;
    std::cout << "Initialization sequence:" << std::endl;
    std::cout << "  0 00101000 (4-bit; repeat four times)" << std::endl;
    std::cout << "  0 00111000 (8-bit; repeat three times)" << std::endl;
    std::cout << "  0 00001111 (enable display, cursor, and blinking)" << std::endl;
    std::cout << "  0 00000110 (shift right after character is displayed)" << std::endl;
    std::cout << "  0 00000001 (clear display)" << std::endl;

    while (true) {
        std::string input;
        std::cout << "input> ";
        std::getline(std::cin, input);

        if (input.empty()) {
            break;
        }

        std::vector<std::string> tokens;
        size_t pos = 0;
        while ((pos = input.find(' ')) != std::string::npos) {
            tokens.push_back(input.substr(0, pos));
            input.erase(0, pos + 1);
        }
        tokens.push_back(input);

        if (tokens.size() == 2) {
            int rs_val = std::stoi(tokens[0]);
            int value = std::stoi(tokens[1], 0, 2);
            write8(value, rs_val, 8);
        } else if (tokens.size() == 3) {
            int rs_val = std::stoi(tokens[0]);
            int value1 = std::stoi(tokens[1], 0, 2);
            int value2 = std::stoi(tokens[2], 0, 2);
            write4(value1, rs_val);
            write4(value2, rs_val);
        } else {
            std::cerr << "Invalid format: X YYYYYYYY, X YYYY ZZZZ, or 1 X" << std::endl;
        }
    }
}

int main() {
    if (gpioInitialise() < 0) {
        std::cerr << "GPIO Initialization failed." << std::endl;
        return 1;
    }

    gpioSetMode(PIN_RS, PI_OUTPUT);
    gpioSetMode(PIN_EN, PI_OUTPUT);
    for (int pin : PIN_D) {
        gpioSetMode(pin, PI_OUTPUT);
    }

    try {
        prog_arbitrary();
    } catch (...) {
        gpioTerminate();
        throw;
    }

    gpioTerminate();
    return 0;
}
