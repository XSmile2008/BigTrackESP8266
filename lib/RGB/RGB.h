#ifndef RGB_h
#define  RGB_h
#include "Arduino.h"

class RGB {
  public:
    RGB(uint8_t pin_r, uint8_t pin_g, uint8_t pin_b);
    void color(uint8_t r, uint8_t g, uint8_t b);

    void on();
    void off();

    void inc();
    void dec();

    void red();
    void green();
    void blue();
  private:
    uint8_t pin_r, pin_g, pin_b;
    uint8_t r, g, b;
};

  RGB::RGB(uint8_t pin_r, uint8_t pin_g, uint8_t pin_b) {
    pinMode(this->pin_r = pin_r, OUTPUT);
    pinMode(this->pin_g = pin_g, OUTPUT);
    pinMode(this->pin_b = pin_b, OUTPUT);
  }

  void RGB::color(uint8_t r, uint8_t g, uint8_t b) {
    analogWrite(pin_r, this->r = r);
    analogWrite(pin_g, this->g = g);
    analogWrite(pin_b, this->b = b);
  }

  void RGB::on() {
    analogWrite(pin_r, r);
    analogWrite(pin_g, g);
    analogWrite(pin_b, b);
  }

  void RGB::off() {
    analogWrite(pin_r, 0);
    analogWrite(pin_g, 0);
    analogWrite(pin_b, 0);
  }

  void RGB::inc() {
    if (r > 0 && b == 0) {
      r--;
      g++;
    } else if (g > 0) {
      g--;
      b++;
    } else if (b > 0) {
      b--;
      r++;
    }
    analogWrite(pin_r, r);
    analogWrite(pin_g, g);
    analogWrite(pin_b, b);
  }

  void RGB::dec() {

  }

  void RGB::red() {
    analogWrite(pin_r, 255);
    analogWrite(pin_g, 0);
    analogWrite(pin_b, 0);
  }

  void RGB::green() {
    analogWrite(pin_r, 0);
    analogWrite(pin_g, 255);
    analogWrite(pin_b, 0);
  }

  void RGB::blue() {
    analogWrite(pin_r, 0);
    analogWrite(pin_g, 0);
    analogWrite(pin_b, 255);
  }

#endif
