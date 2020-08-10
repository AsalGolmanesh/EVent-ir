#ifndef ALARM_H
#define ALARM_H
#include "Arduino.h"
#include "openGLCD.h"

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
    static alarm *INSTANCE;

public:
    static alarm *getInstance();
    void alarmPin(int buzzer_Pin, int LED_Pin);
    //alarm(int buzzer_Pin, int LED_Pin);
    void warning();
    void error(char message[21]);
};

#endif
