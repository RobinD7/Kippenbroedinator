#include <LiquidCrystal.h>

class MyLCD {
public:
  MyLCD(int rs, int en, int d4, int d5, int d6, int d7) : lcd(rs, en, d4, d5, d6, d7) {}

  void initialize(int columns, int rows) {
    lcd.begin(columns, rows);
  }

  void printMessage(const char* message, int column, int row) {
    lcd.setCursor(column, row);
    lcd.print(message);
  }

  void printMillis(int column, int row) {
    lcd.setCursor(column, row);
    lcd.print(millis() / 1000);
  }

private:
  LiquidCrystal lcd;
};

// Maak een instantie van de klasse MyLCD
MyLCD myLCD(7, 8, 9, 10, 11, 12);

void setup() {
  // Initialiseer het LCD-scherm
  myLCD.initialize(16, 2);

  // Print een bericht op het LCD-scherm
  myLCD.printMessage("hello, world!", 0, 0);
}

void loop() {
  // Print het aantal seconden sinds reset op het LCD-scherm
  myLCD.printMillis(0, 1);
}
