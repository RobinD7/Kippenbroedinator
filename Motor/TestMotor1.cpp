// g++ -g Testmotor.cpp -o Testmotor -lpigpiod_if2 -lrt -lpigpio

#include <iostream>
#include <pigpiod_if2.h>
#include <unistd.h>
#include <ctime>

int pi = pigpio_start(NULL, NULL); // Connect to the pigpiod daemon

// Define pigpio pins
#define MOTOR_PIN1 6
#define MOTOR_PIN2 13
#define MOTOR_PIN3 19
#define MOTOR_PIN4 26

#define STEPCOUNT 8

class Motor {
public:
    Motor();
    ~Motor();
    void start();
    void stop();

private:
    //int pi;
};

Motor::Motor() {
    //pi = pigpio_start(NULL, NULL);
    set_mode(pi, MOTOR_PIN1, PI_OUTPUT);
    set_mode(pi, MOTOR_PIN2, PI_OUTPUT);
    set_mode(pi, MOTOR_PIN3, PI_OUTPUT);
    set_mode(pi, MOTOR_PIN4, PI_OUTPUT);

    // initialize GPIO
    gpioInitialise();
}

Motor::~Motor() {
    gpio_write(pi, MOTOR_PIN1, 0);
    gpio_write(pi, MOTOR_PIN2, 0);
    gpio_write(pi, MOTOR_PIN3, 0);
    gpio_write(pi, MOTOR_PIN4, 0);
}

void Motor::start() {
    // Stappenmotor sequentie (volgens datasheet 28BYJ-48)
    /*
    int sequence[][4] = {{1, 0, 0, 1},
                         {1, 0, 0, 0},
                         {1, 1, 0, 0},
                         {0, 1, 0, 0},
                         {0, 1, 1, 0},
                         {0, 0, 1, 0},
                         {1, 0, 1, 0},
                         {1, 0, 0, 1}};
    */
    int sequence[][4] = {{0, 0, 0, 1},
                         {0, 0, 1, 1},
                         {0, 0, 1, 0},
                         {0, 1, 1, 0},
                         {0, 1, 0, 0},
                         {1, 1, 0, 0},
                         {1, 0, 0, 0},
                         {1, 0, 0, 1}};
    
    for (int i = 0; i < 512; ++i) { // 512 stappen voor volledige omwenteling
        for (int j = 0; j < 8; ++j) {
            gpio_write(pi, MOTOR_PIN1, sequence[j][0]);
            gpio_write(pi, MOTOR_PIN2, sequence[j][1]);
            gpio_write(pi, MOTOR_PIN3, sequence[j][2]);
            gpio_write(pi, MOTOR_PIN4, sequence[j][3]);
            time_sleep(0.002); // Korte pauze tussen stappen
        }
    }
}

void Motor::stop() {
    // Motor stoppen
    gpio_write(pi, MOTOR_PIN1, 0);
    gpio_write(pi, MOTOR_PIN2, 0);
    gpio_write(pi, MOTOR_PIN3, 0);
    gpio_write(pi, MOTOR_PIN4, 0);
}

int main() {
    std::cout << "Programma gestart ";
    Motor motor;

    std::cout << "Motor gestart ";
        motor.start();
        sleep(30);
        std::cout << "Motor gestopt ";
        motor.stop();
        sleep(30);

    //while (true) {
        /*
        // De motor starten om de vijf minuten
        time_t now = time(0);
        tm* ltm = localtime(&now);

        if (ltm->tm_min % 5 == 0) {
            std::cout << "Motor gestart op: " << asctime(ltm);
            motor.start();
        } else {
            motor.stop();
        }

        sleep(60); // Wacht 1 minuut voordat de cyclus opnieuw begint
        */
    //}

    pigpio_stop(pi); // Disconnect from the pigpiod daemon
    return 0;
}
