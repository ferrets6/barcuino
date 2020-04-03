#include <Arduino.h>

enum LedType { UNDEFINED, RED, GREEN, BLUE, BACK };

class LedController
{
public:
    LedController();
    uint8_t attach(int pinm, LedType type); // attach the given pin and initialize pin
    void detach();
    void on();  // switch on LED with max value supported by led type
    void toggle(); // switch on/off
    void off();
    void setIntensity(int intensity); // set intensity to the one desired
    bool isOn;
private:
    bool    _attached;
    uint8_t _pin;
    LedType _type;
};