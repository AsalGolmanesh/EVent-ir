#include <motor_driver.h>

Motor_Driver::Motor_Driver(Motor* motor)
{
    this->local_sysconfig = new SysConfig(0, 0, 0);
    this->pMotor = motor;
    this->inhaleExhale = INHALE;
}

void Motor_Driver::update_sysconfig(SysConfig* newconfig)
{
    this->local_sysconfig->update(newconfig);
}

void Motor_Driver::check()
{
    unsigned long current_Time = 0;
    unsigned long start_Time = 0;
    if(pMotor->getStatus()==MOTOR_IS_ON){
        start_Time = this->local_sysconfig->get_Start_Time();
        current_Time = millis();    
        if(this->inhaleExhale == INHALE){
            pMotor->setSpeed(100);
            pMotor->setDirection(DIRECTION_CLOSE);
            if (((signed long)(current_Time - start_Time))>=this->local_sysconfig->get_Inh_Time()){
                pMotor->setSpeed(50);
                pMotor->setDirection(DIRECTION_OPEN);
                this->local_sysconfig->set_Start_Time();
                this->inhaleExhale = EXHALE;
            }
        }
        else
        {
            Serial.println("here0");
            if(pMotor->getDirection()==DIRECTION_CLOSE)
            {
                pMotor->setSpeed(100);
                this->local_sysconfig->set_Start_Time();
                this->inhaleExhale = INHALE;                
            }
        }
        
    }   
}

