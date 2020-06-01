#ifndef MOTORCONTROLLER_H
#define MOTORCONTROLLER_H

#include "PID.h"
#include "DegreeTracker.h"
#include "button.h"
#include "configuration.h"
#include "callBacks.h"
#include "motor.h"
#include "sysconfig.h"
#include <timers.h>

#define MAXIMUM_MOTOR_SPEED_IN_RPM 40
#define MINIUM_MOTOR_SPEED_IN_RPM  4
#define MINIUM_MOTOR_SPEED_IN_PWM  10
#define TIME_STEP                  5e-3
#define DESIRED_ROTATION           33
#define EXHALE_DEGREE_RATIO        0.9
#define BEFORE_OUSWITCH_MAX_DEGREE 15
#define MOTOR_STOP_TIME            35e-3
#define KP                         2.5
#define KI                         32                          
#define KD                         25e-3
#define PID_IGNORE_COUNT           20
#define PID_GAURD_COUNT            20

extern Button *open_uSwitch;
extern int on_uSwithHitPC;
extern volatile int timeStepValid;

extern float calcedLeftTime[400];
extern float calcedLeftDegree[400];
extern int   MotorSpeedActual[400];
extern float MotorPwm[400];

enum ControllerStates
{
    initialCalibration,
    beforeReciprocating,
    reciprocating
};

enum InitialCalibrationStates
{
    initialCalibrationStart,
    initialCalibrationInProgress,
    initialCalibrationStopping,
    initialCalibrationDone
};

enum ReciprocatingStates
{
    closingCycleStart,
    closingCycleinProgrees,
    closingCycleStopping,
    closingCycleStopped,
    openningCycleStart,
    openningCycleInProgress,
    openningCycleReaching_uSwitch,
    openningCycleStopping,
    openningCycleStopped,
    openSwitchNotHit

};

class MotorController
{
private:
    SysConfig *sysConfig;
    PID *pid;
    DegreeTracker *degreeTracker;

    ControllerStates controllerState = initialCalibration;
    InitialCalibrationStates calibrationState = initialCalibrationStart;
    ReciprocatingStates reciprocatingState = closingCycleStart;

    float positionError             = 0;
    int   lastEncoderPulseCount     = 0;
    int   onStopCommandPulseCount   = 0;
    float desiredRotation           = 0;
    float desiredRotationTime       = 0.7;
    float inhaleExhaleRatio         = 2;
    int   motorSpeed                = 0;
    int   motorStopDoubleGaurd      = 0;
    int   motorStopDoubleGaurdLimit = 4;
    bool  reciprocateStart          = false;
    bool  reciprocateStop           = false;
    float inhaleTime                = 0;
    float exhaleTime                = 0;
    int   logCounter=0;

    void setRequiredSpeed(float requiredSpeed);
    void setMotorOnMinimumSpeed(bool direction);
    void initialMotorCalibrationHandler();
    void reciprocatingHandler();
    void logMotor();
    void printLog();
    void motorSpeedCheck();
    void onMotorStart();
    void onMotorStop();
    void motorGoToPosition(float deltaTime, float deltaDegree);
    void resetParams();

public:
    MotorController(/* args */);
    void updatePots(int IERatio, int respCycle);
    void motorControllerHandler();
    void startReciporating();
    void stopReciporating();
};

#endif


