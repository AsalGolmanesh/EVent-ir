#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <configuration.h>
#include <avr/wdt.h>
#include <openGLCD.h>
#include "sysconfig.h"
#include <motor.h>
#include <motor_driver.h>
#include <buzzer.h>
#include <button.h>
#include <led.h>
#include <Potentiometer.h>
#include <LCD.h>
#include <timers.h>
#include <callBacks.h>
#include <PID.h>
#include <trajectory.h>
#include <string.h>
#include <Stream.h>
#include <math.h>
#include "DegreeTracker.h"
#include "MotorController.h"
#include <alarm.h>

/* Global Objects */
SysConfig *Global_SysConfig;

Button *onButton;
Button *applyButton;
Button *open_uSwitch;

LED *bLED;
LED *wLED;
LED *ardLED;
LED *pLED1;
LED *pLED2;

Buzzer *coolBuzz;

Potentiometer *respVolume;
Potentiometer *respCycle;
Potentiometer *IERatio;

PressureSensor *PR;

MotorController *motorController;
alarm *Error;

char message[21] = "mini Vent";
int table_RV[] = {300, 700};
int table_RC[4];
int table_IE[] = {1, 2, 3, 4};

volatile int timeStepValid = 0;

float calcedLeftTime[420];
float calcedLeftDegree[420];
int MotorSpeedActual[420];
float MotorPwm[420];

int on_uSwithHitPC = 0;
int stopPC = 0;

#endif
