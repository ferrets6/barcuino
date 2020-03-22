#include <Arduino.h>

enum LedType { UNDEFINED, RED, GREEN, BLUE, BACK };

class LedController
{
public:
    LedController();
    uint8_t attach(int pinm, LedType type); // attach the given pin to the next free channel, sets pinMode, returns channel number or 0 if failure
    void detach();
    void on();
    void toggle();
    void off();
    void setIntensity();
    bool isOn;
private:
    bool    _attached;
    uint8_t _pin;
    LedType _type;
    void setLedIntensity(int intensity);
};