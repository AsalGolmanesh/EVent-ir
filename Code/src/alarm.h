#ifndef ALARM_H
#define ALARM_H
#include "Arduino.h"
class alarm
{
private:
    unsigned long blinkInterval = 250;
    unsigned long counterTime = 0;
    unsigned long desiredAlarmInterval = 5000;
    int counterValue = 0;
    int LED_Pin;
    int buzzer_Pin;
    int sound_freq = 1000;

public:
    alarm(int buzzer_Pin, int LED_Pin);
    void warning();
    void error();
};

#endif
