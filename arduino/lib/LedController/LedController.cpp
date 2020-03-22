#include <Arduino.h>
#include <LedController.h>

#define MAX_INTENSITY_RED 700
#define MAX_INTENSITY_GREEN 800
#define MAX_INTENSITY_BLUE 900
#define MAX_INTENSITY_BACK 1023

#define MAX_ANALOG_WRITE 1023

LedController::LedController()
{
  _attached = false;
  isOn = false;
}

uint8_t LedController::attach (int pin, LedType type) {
  if (!_attached) {
    _pin = pin;
    _type = type;
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);
    _attached = true;
  }

  return pin;
}

void LedController::on () {
  if (_attached) {
    setLedIntensity(MAX_ANALOG_WRITE);
    isOn = true;
  }
}

void LedController::off () {
  if (_attached) {
    setLedIntensity(0);
    isOn = false;
  }
}

void LedController::toggle () {
  if (_attached) {
    if (isOn)
      on();
    else
      off();
  }
}

void LedController::setLedIntensity(int intensity) {
  switch (_type)
  {
    case RED:
      intensity = min(intensity,MAX_INTENSITY_RED);
      break;
    case GREEN:
      intensity = min(intensity,MAX_INTENSITY_GREEN);
      break;
    case BLUE:
      intensity = min(intensity,MAX_INTENSITY_BLUE);
      break;
    case BACK:
      intensity = min(intensity,MAX_INTENSITY_BACK);
      break;
    case UNDEFINED:
      intensity = min(intensity,MAX_ANALOG_WRITE);
      break;    
    default:
      return;
      break;
  }
  analogWrite(_pin, intensity);
  isOn = false;
}
