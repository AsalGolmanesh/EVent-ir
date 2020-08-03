#include "alarm.h"

alarm::alarm(int buzzer_Pin, int LED_Pin)
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
void alarm::error()
{
    this->counterValue = floor(this->desiredAlarmInterval / (2 * this->blinkInterval));
    if (this->counterTime < this->counterValue)
    {
        tone(this->buzzer_Pin, this->sound_freq, this->blinkInterval);
        digitalWrite(this->LED_Pin, HIGH);
        delay(this->blinkInterval);
        noTone(this->buzzer_Pin);
        digitalWrite(this->LED_Pin, LOW);
        delay(this->blinkInterval);
        this->counterTime++;
    }
}