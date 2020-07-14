#include <MotorController.h>

MotorController::MotorController(/* args */)
{
    sysConfig = new SysConfig(2, 20, 0);
    sysConfig->setParams(6e-1, TIME_STEP, MINIUM_MOTOR_SPEED_IN_PWM);

    curveFit = new CurveFit(2, curveFitRatios);

    degreeTracker = new DegreeTracker(DESIRED_ROTATION, sysConfig->duration, sysConfig->timeStep);
}

void MotorController::startReciporating(){
    this->reciprocateStart = true;   
}

void MotorController::stopReciporating(){
    this->reciprocateStop = true;
}

void MotorController::updatePots(int IERatio, int respCycle){
    sysConfig->set_IE_Ratio(IERatio);
    sysConfig->set_Resp_Rate(respCycle);
    inhaleTime = sysConfig->get_Inh_Time()/1000;
    exhaleTime = sysConfig->get_Exh_Time()/1000;  
}

void MotorController::motorControllerHandler()
{
    timeStepValid = 0;

    switch (controllerState)
    {
    case initialCalibration:
        initialMotorCalibrationHandler();
        break;
    case beforeReciprocating:
        if (reciprocateStart){
            reciprocateStart = false;
            controllerState = reciprocating;
        }
        break;
    case reciprocating:  
        reciprocatingHandler();      
        if(reciprocateStop){            
            if(openingCycleFinished){
                reciprocateStop = false; 
                onMotorStop();
                reciprocatingState=closingCycleStart;  
                controllerState = beforeReciprocating; 
                printLog();                       
            }                        
        }        
        break;

    default:
        break;
    }
}

void MotorController::initialMotorCalibrationHandler()
{

    switch (calibrationState)
    {
    case initialCalibrationStart:

        setMotorOnMinimumSpeed(DIRECTION_OPEN);
        calibrationState = initialCalibrationInProgress;
        break;

    case initialCalibrationInProgress:
        motorSpeedCheck();
        if (open_uSwitch->get_Status() == BSTATE_HIGH)
        {
            setMotorOnMinimumSpeed(DIRECTION_OPEN);
        }
        else
        {
            open_uSwitch->set_Clicked(false);
            Motor::getInstance()->motorStop();
            calibrationState = initialCalibrationStopping;
        }
        break;

    case initialCalibrationStopping:
        motorSpeedCheck();
        if (lastEncoderPulseCount == Motor::getInstance()->getPC())
        {
            if (motorStopDoubleGaurd == motorStopDoubleGaurdLimit)
            {
                int motorPulseCount = Motor::getInstance()->getPC();
                positionError = ((float)motorPulseCount - (float)on_uSwithHitPC) * (float)360 / (float)MOTOR_PULSE_PER_TURN;
                calibrationState = initialCalibrationDone;
            }
            motorStopDoubleGaurd++;
        }
        else
        {
            lastEncoderPulseCount = Motor::getInstance()->getPC();
            motorStopDoubleGaurd = 0;
        }

        break;
    case initialCalibrationDone:
        lastEncoderPulseCount = 0;
        controllerState = beforeReciprocating;        
        break;

    default:
        break;
    }
}

void MotorController::reciprocatingHandler()
{
    switch (reciprocatingState)
    {
    case closingCycleStart:
        openingCycleFinished = false;
        inhaling = true;
        ieRatio    = sysConfig->get_IE_Ratio();
        respRate   = sysConfig->get_Resp_Rate();
        inhaleTime = sysConfig->get_Inh_Time()/1000;
        exhaleTime = sysConfig->get_Exh_Time()/1000;

        Motor::getInstance()->setDirection(DIRECTION_CLOSE);
        motorGoToPosition(inhaleTime, DESIRED_ROTATION + positionError*EXHALE_DEGREE_RATIO);
        onMotorStart();
        logCounter=0;
        logMotor();
        reciprocatingState = closingCycleinProgrees;
        break;

    case closingCycleinProgrees:
        motorSpeedCheck();
        degreeTracker->updatePosition(Motor::getInstance()->getPC());
        if (degreeTracker->getLeftTime() > MOTOR_STOP_TIME)
        {
            setRequiredSpeed(degreeTracker->updateDesiredRPM(Motor::getInstance()->getEncRPM()));
        }
        else
        {
            setRequiredSpeed(MINIUM_MOTOR_SPEED_IN_RPM);
            Motor::getInstance()->motorStop();
            onStopCommandPulseCount = Motor::getInstance()->getPC();
            reciprocatingState = closingCycleStopping;
        }
        logMotor();
        break;

    case closingCycleStopping:
        motorSpeedCheck(); 
        logMotor();     
        if (lastEncoderPulseCount == Motor::getInstance()->getPC())
        {
            positionError = degreeTracker->getLeftDeltaDegree();
            reciprocatingState = closingCycleStopped;
        }
        else
        {
            degreeTracker->updatePosition(Motor::getInstance()->getPC());
            lastEncoderPulseCount = Motor::getInstance()->getPC();
        }
        break;

    case closingCycleStopped:
        setRequiredSpeed(MINIUM_MOTOR_SPEED_IN_RPM-2);
        logMotor();
        onMotorStop();
        lastEncoderPulseCount = 0;
        delay(INHALE_TO_EXHALE_PAUSE_TIME*1e3);        
        reciprocatingState = openningCycleStart;
        break;

    case openningCycleStart:
        inhaling = false;
        Motor::getInstance()->setDirection(DIRECTION_OPEN);
        motorGoToPosition(OPENING_CYCLE_TIME, (DESIRED_ROTATION - positionError)* EXHALE_DEGREE_RATIO);
        onMotorStart();
        logMotor();
        reciprocatingState = openningCycleInProgress;
        break;

    case openningCycleInProgress:
        motorSpeedCheck();
        degreeTracker->updatePosition(Motor::getInstance()->getPC());    
        if (degreeTracker->getLeftTime() > MOTOR_STOP_TIME_OPEN_CYCLE)
        {            
            setRequiredSpeed(degreeTracker->updateDesiredRPM(Motor::getInstance()->getEncRPM()));
        }
        else
        {
            setRequiredSpeed(MINIUM_MOTOR_SPEED_IN_RPM-2);
            onStopCommandPulseCount = Motor::getInstance()->getPC();
            reciprocatingState = openningCycleReaching_uSwitch;
        }
        logMotor();
        break;

    case openningCycleReaching_uSwitch:
        motorSpeedCheck();
        degreeTracker->updatePosition(Motor::getInstance()->getPC());     
        if (open_uSwitch->get_Status() == BSTATE_HIGH)
        {
            setRequiredSpeed(MINIUM_MOTOR_SPEED_IN_RPM-2);
            float passedDegreeFromStop = (Motor::getInstance()->getPC()-onStopCommandPulseCount)*(float)360 / (float)MOTOR_PULSE_PER_TURN;
            if(passedDegreeFromStop>BEFORE_OUSWITCH_MAX_DEGREE){
                Motor::getInstance()->motorStop();
                reciprocatingState = openSwitchNotHit;
            }            
        }
        else
        {
            open_uSwitch->set_Clicked(false);
            Motor::getInstance()->motorStop();
            reciprocatingState = openningCycleStopping;
        }
        logMotor();
        break;

    case openningCycleStopping:
        motorSpeedCheck();
        degreeTracker->updatePosition(Motor::getInstance()->getPC());  
        logMotor();
        if (lastEncoderPulseCount == Motor::getInstance()->getPC())
        {
            if (motorStopDoubleGaurd == motorStopDoubleGaurdLimit)
            {
                int motorPulseCount = Motor::getInstance()->getPC();
                positionError = ((float)motorPulseCount - (float)on_uSwithHitPC) * (float)360 / (float)MOTOR_PULSE_PER_TURN;
                reciprocatingState = openningCycleStopped;
                leftTime = degreeTracker->getLeftTime();
                onMotorStop();
            }
            motorStopDoubleGaurd++;
        }
        else
        {
            lastEncoderPulseCount = Motor::getInstance()->getPC();
            motorStopDoubleGaurd = 0;
        }
        break;

    case openningCycleStopped:
        if(exhaleTime>=inhaleTime){
            float puaseTime = exhaleTime+leftTime-OPENING_CYCLE_TIME-INHALE_TO_EXHALE_PAUSE_TIME; 
            delay(round(puaseTime*1000));   
        }
        lastEncoderPulseCount = 0;
        openingCycleFinished = true;
        reciprocatingState = closingCycleStart;
        break;

    case openSwitchNotHit:
        reciprocatingState = openSwitchNotHit;
        Serial.print("NotHit");
        break;  

    default:
        break;
    }
}

void MotorController::setMotorOnMinimumSpeed(bool direction)
{
    Motor::getInstance()->setDirection(direction);
    Motor::getInstance()->setSpeed(MINIUM_MOTOR_SPEED_IN_PWM);
    onMotorStart();
}

void MotorController::setRequiredSpeed(float requiredSpeed)
{
    
    if(inhaling){
        if(ieRatio<2){
            if(respRate < 22)
                motorPWM = curveFit->fit(requiredSpeed, MINIUM_MOTOR_SPEED_IN_RPM+8)+respRate+24;        
            if(respRate < 25)
                motorPWM = curveFit->fit(requiredSpeed, MINIUM_MOTOR_SPEED_IN_RPM-2)+respRate-13;
            else
                motorPWM = curveFit->fit(requiredSpeed, MINIUM_MOTOR_SPEED_IN_RPM-2)+respRate-14;
        }
        else if(ieRatio<3){
            if(respRate < 21)
                motorPWM = curveFit->fit(requiredSpeed, MINIUM_MOTOR_SPEED_IN_RPM-2)+respRate;
            else if(respRate < 26)
                motorPWM = curveFit->fit(requiredSpeed, MINIUM_MOTOR_SPEED_IN_RPM-4)+respRate-12;
            else
                motorPWM = curveFit->fit(requiredSpeed, MINIUM_MOTOR_SPEED_IN_RPM-3)+respRate-17;
        }
        else        
            motorPWM = curveFit->fit(requiredSpeed, MINIUM_MOTOR_SPEED_IN_RPM)+22;
        
    }   
    else        
        motorPWM = curveFit->fit(requiredSpeed, MINIUM_MOTOR_SPEED_IN_RPM-2)-1;        
        
    Motor::getInstance()->setSpeed(motorPWM);
}

void MotorController::logMotor()
{
	calcedLeftTime[logCounter]    = degreeTracker->getLeftTime();
	calcedLeftDegree[logCounter]  = degreeTracker->getLeftDeltaDegree();
	//MotorPwm[logCounter] 		  = motorPWM;
	MotorPwm[logCounter] 		  = reciprocatingState; //degreeTracker->getDesiredRPM();
    //MotorSpeedActual[logCounter]  = motorPWM;
	MotorSpeedActual[logCounter]  = round(Motor::getInstance()->getEncRPM());
    if (logCounter<419)
        logCounter++;
}

void MotorController::printLog(){
	for (int i = 0; i < logCounter; i++)
	{
		Serial.print(i);
		Serial.print("\t");
		Serial.print(calcedLeftTime[i],3);
		Serial.print("\t");
		Serial.print(calcedLeftDegree[i],3);
		Serial.print("\t");
		Serial.print(MotorPwm[i]);
		Serial.print("\t");
		Serial.println(MotorSpeedActual[i]);
	}
	Serial.println("End");				
	logCounter=0;
}

void MotorController::motorSpeedCheck(){
	if(round(Motor::getInstance()->getEncRPM())>MAXIMUM_MOTOR_SPEED_IN_RPM)
	{
        reciprocatingState = openSwitchNotHit;
		onMotorStop();
		Serial.println("check");
	}
}

void MotorController::onMotorStart()
{
    resetParams();
    Motor::getInstance()->motorStart();
}

void MotorController::onMotorStop()
{
    Motor::getInstance()->motorStop();
    resetParams();
}

void MotorController::motorGoToPosition(float deltaTime, float deltaDegree){
    degreeTracker->updateDesiredDelatTime(deltaTime);
    degreeTracker->updateDesiredDeltaDegree(deltaDegree);
    setRequiredSpeed(degreeTracker->updateDesiredRPM(Motor::getInstance()->getEncRPM()));
}

void MotorController::resetParams()
{
    Motor::getInstance()->resetEncPeriod();
    Motor::getInstance()->resetPC();
    degreeTracker->resetPosition();
    onStopCommandPulseCount = 0;
    motorStopDoubleGaurd = 0;
}
