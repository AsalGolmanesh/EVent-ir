#include "alarm.h"

alarm *alarm::INSTANCE = NULL;

alarm *alarm::getInstance()
{
    if (INSTANCE == NULL)
    {
        INSTANCE = new alarm();
    }
    return INSTANCE;
}

void alarm::alarmPin(int buzzer_Pin, int LED_Pin)
{
    this->buzzer_Pin = buzzer_Pin;
    this->LED_Pin = LED_Pin;
}

void alarm::warning()
{
    this->counterValue = floor(this->desiredAlarmInterval / (2 * this->blinkInterval));
    if (this->counterTime < this->counterValue)
    {
        digitalWrite(this->LED_Pin, HIGH);
        delay(this->blinkInterval);
        digitalWrite(this->LED_Pin, LOW);
        delay(this->blinkInterval);
        this->counterTime++;
    }
}
void alarm::error(char message[21])
{
    this->counterValue = floor(this->desiredAlarmInterval / (2 * this->blinkInterval));
    GLCD.ClearScreen();
    GLCD.Init();
    GLCD.DrawRoundRect(0, 0, GLCD.Width, GLCD.Height, 4);
    GLCD.SelectFont(cp437font8x8);
    GLCD.GotoXY((GLCD.Width / 2) - 30, GLCD.Height / 2);
    GLCD.print(message);

    // if (this->counterTime < this->counterValue)          // if one wants to set the alarm for just 5seconds
    // {
    tone(this->buzzer_Pin, this->sound_freq, this->blinkInterval);
    digitalWrite(this->LED_Pin, HIGH);
    delay(this->blinkInterval);
    noTone(this->buzzer_Pin);
    digitalWrite(this->LED_Pin, LOW);
    delay(this->blinkInterval);
    this->counterTime++;
    // }
}
