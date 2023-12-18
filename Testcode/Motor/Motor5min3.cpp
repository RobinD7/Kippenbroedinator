#include <iostream>
#include <pigpiod_if2.h>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <ctime>

int pi = pigpio_start(NULL, NULL); // Connect to the pigpiod daemon

// define Motor pins
#define MOTOR_PIN1 6
#define MOTOR_PIN2 13
#define MOTOR_PIN3 19
#define MOTOR_PIN4 26

#define MOTOR_DELAY 10 // Delay in ms
#define MOTOR_STEPS 150 // Number of steps

class Motor {
public:
    Motor();
    ~Motor();

    void setStep(int w1, int w2, int w3, int w4);
    void rotateClockwise();//void forward(double delay, int steps);
    void rotateCounterClockwise(); //void backwards(double delay, int steps);
    void startMotorEveryFiveMinutes(double delay, int steps);

private:
    static const int Seq[][4];
};

const int Motor::Seq[][4] = {
    {0, 1, 0, 0},
    {0, 1, 0, 1},
    {0, 0, 0, 1},
    {1, 0, 0, 1},
    {1, 0, 0, 0},
    {1, 0, 1, 0},
    {0, 0, 1, 0},
    {0, 1, 1, 0}
};

Motor::Motor() {
    set_mode(pi, MOTOR_PIN1, PI_OUTPUT);
        set_mode(pi, MOTOR_PIN2, PI_OUTPUT);
        set_mode(pi, MOTOR_PIN3, PI_OUTPUT);
        set_mode(pi, MOTOR_PIN4, PI_OUTPUT);


    if (gpioInitialise() < 0) {
        std::cerr << "pigpio initialization failed." << std::endl;
        // Handle initialization failure
    }
}

Motor::~Motor() {
    pigpio_stop(pi);
}

void Motor::setStep(int w1, int w2, int w3, int w4) {
    gpio_write(pi, MOTOR_PIN2, w1);
    gpio_write(pi, MOTOR_PIN4, w2);
    gpio_write(pi, MOTOR_PIN3, w3);
    gpio_write(pi, MOTOR_PIN1, w4);
}

void Motor::rotateClockwise() { //forward
for (int i = 0; i < MOTOR_STEPS; ++i) {
        for (int j = 0; j < 8; ++j) {
            setStep(Seq[j][0], Seq[j][1], Seq[j][2], Seq[j][3]);
            time_sleep(MOTOR_DELAY/ 1000.0);
        }
    }
}

void Motor::rotateCounterClockwise() { //backward
    for (int i = 0; i < MOTOR_STEPS; ++i) {
        for (int j = 7; j >= 0; --j) {
            setStep(Seq[j][0], Seq[j][1], Seq[j][2], Seq[j][3]);
            time_sleep(MOTOR_DELAY/ 1000.0);
        }
    }
}

void Motor::startMotorEveryFiveMinutes(double delay, int steps) {
    time_t now = time(0);
    tm* ltm = localtime(&now);

    if (ltm->tm_min % 5 == 0) {
        std::cout << "Motor gestart op: " << asctime(ltm);
        rotateClockwise();
    } else {

    }
}

int main() {
    if (pi < 0) {
        std::cerr << "Failed to initialize pigpio: " << pigpio_error(pi) << >
        return 1;
    }

    Motor motor;

    while (true) {
        motor.startMotorEveryFiveMinutes(MOTOR_DELAY, MOTOR_STEPS);

        sleep(50); // Wacht 1 minuut voordat de cyclus opnieuw begint
    }

    return 0;
}
