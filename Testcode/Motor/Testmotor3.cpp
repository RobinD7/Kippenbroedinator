#include <iostream>
#include <pigpiod_if2.h>
#include <unistd.h>

#define MOTOR_PIN1 13
#define MOTOR_PIN2 26
#define MOTOR_PIN3 6
#define MOTOR_PIN4 19

#define DELAY_US 1000

int main() {
    // Initialize pigpio
    int pi = pigpio_start(NULL, NULL);
    if (pi < 0) {
        std::cerr << "Failed to initialize pigpio: " << pigpio_error(pi) << >
        return 1;
    }

    // Set motor pins as outputs
    set_mode(pi, MOTOR_PIN1, PI_OUTPUT);
    set_mode(pi, MOTOR_PIN2, PI_OUTPUT);
    set_mode(pi, MOTOR_PIN3, PI_OUTPUT);
    set_mode(pi, MOTOR_PIN4, PI_OUTPUT);

    // Define the sequence
    int sequence[][4] = {
        {0, 1, 0, 0},
        {0, 1, 0, 1},
        {0, 0, 0, 1},
        {1, 0, 0, 1},
        {1, 0, 0, 0},
        {1, 0, 1, 0},
        {0, 0, 1, 0},
        {0, 1, 1, 0}
    };

    // Run the sequence
    for (int i = 0; i < sizeof(sequence) / sizeof(sequence[0]); ++i) {
        gpio_write(pi, MOTOR_PIN1, sequence[i][0]);
        gpio_write(pi, MOTOR_PIN2, sequence[i][1]);
        gpio_write(pi, MOTOR_PIN3, sequence[i][2]);
        gpio_write(pi, MOTOR_PIN4, sequence[i][3]);

        usleep(DELAY_US);
    }

    // Cleanup
    pigpio_stop(pi);

    return 0;
}


