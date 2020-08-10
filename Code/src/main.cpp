// by Amir Sk , MHG, MHM, ASM
// Apr. 5rd 2020

#include "main.h"

void setup()
{
	noInterrupts();

	for (int i = 0; i < 4; i++)
		table_RC[i] = 15 + i * 5;

	Init_Timer1();
	Init_Timer3();
	Init_Timer4();
	Init_Timer5();

	Serial.begin(115200);

	Global_SysConfig = new SysConfig(2, 20, 0);
	Global_SysConfig->setParams(6e-1, 5e-3, 37);

	PinConfiguration::getInstance()->pinConfiguration();

	coolBuzz = new Buzzer(PinConfiguration::buzzerPin);

	onButton = new Button(PinConfiguration::onButton_pin);
	onButton->setPressCallback(onButton_callback);

	applyButton = new Button(PinConfiguration::applyButton_pin);
	applyButton->setPressCallback(applyButton_callback);

	// ardLED = new LED(PinConfiguration::ardLED);

	bLED = new LED(PinConfiguration::bLED_pin);
	wLED = new LED(PinConfiguration::wLED_pin);
	pLED1 = new LED(PinConfiguration::pLED1_pin);
	pLED2 = new LED(PinConfiguration::pLED2_pin);

	respVolume = new Potentiometer(PinConfiguration::Potentiometer_Volume, 2);
	respCycle = new Potentiometer(PinConfiguration::Potentiometer_Cycle, 4);
	IERatio = new Potentiometer(PinConfiguration::Potentiometer_IE, 4);

	PR = new PressureSensor(PinConfiguration::PR_Out, PinConfiguration::PR_Sck);

	respCycle->set_Range(table_RC, sizeof table_RC);
	respVolume->set_Range(table_RV, sizeof table_RV);
	IERatio->set_Range(table_IE, sizeof table_IE);

	motorController = new MotorController();

	interrupts();

	LCD::getInstance()->LCD_Logo();
	LCD::getInstance()->LCD_Menu(respVolume->Potentiometer_Read(), respCycle->Potentiometer_Read(), IERatio->Potentiometer_Read(), 0 /*PR->Read_Pressure()*/);
	delay(100);

	pLED1->set_high();
	pLED2->set_high();

	//digitalWrite(PinConfiguration::motorDriverOnOff, HIGH);
	Motor::getInstance()->initEnc(PinConfiguration::motorEncoderPin, INPUT, enc_callback, FALLING);
	Timer1Start(round(15625 * Global_SysConfig->timeStep) - 1);

	alarm::getInstance()->alarmPin(PinConfiguration::buzzerPin, PinConfiguration::pLED2_pin);
	motorController->updatePots(IERatio->Potentiometer_Read(), respCycle->Potentiometer_Read(), respVolume->Potentiometer_Read());
}

void loop()
{

	alarm::getInstance()->error(message);

	LCD::getInstance()->LCD_Menu(respVolume->Potentiometer_Read(), respCycle->Potentiometer_Read(), IERatio->Potentiometer_Read(), 0 /*PR->Read_Pressure()*/);
	// //LCD::getInstance()->LCD_graph();

	onButton->check();
	applyButton->check();
	if (onButton->get_Clicked() == true && onButton->get_On_Off() == BSTATE_ON)
	{
		onButton->set_Clicked(false);
		motorController->startReciporating();
	}
	else if (onButton->get_Clicked() == true && onButton->get_On_Off() == BSTATE_OFF)
	{
		motorController->stopReciporating();
		onButton->set_Clicked(false);
	}

	if (applyButton->get_Clicked() == true)
	{
		applyButton->set_Clicked(false);
		motorController->updatePots(IERatio->Potentiometer_Read(), respCycle->Potentiometer_Read(), respVolume->Potentiometer_Read());
	}

	if (timeStepValid)
	{
		//motorController->updatePots(IERatio->Potentiometer_Read(), respCycle->Potentiometer_Read(), respVolume->Potentiometer_Read());
		motorController->motorControllerHandler();
	}

	wdt_reset();
}
