#include <PressureSensor.h>

PressureSensor::PressureSensor(int Out_pin, int Sck_pin, 
                               int ofst_e2p_addr = 0, int scale_e2p_addr = 4)
{
    this->Sck_pin = Sck_pin;
    this->Out_pin = Out_pin;
    this->PR = PR;
    this->HX711_inst = new HX711();

    this->e2p_offset_addr = ofst_e2p_addr;
    this->e2p_scale_addr = scale_e2p_addr;

    this->HX711_inst->begin(this->Out_pin, this->Sck_pin, 64); //64 is not real gain, it is based on HX710B

    this->peak_pr = 0.0;

    long offset;
    EEPROM.get(this->e2p_offset_addr, offset);
    this->HX711_inst->set_offset(offset);
    Serial.println("Offset read from EEPROM:");
    Serial.println(this->HX711_inst->get_offset());

    double scale;
    EEPROM.get(this->e2p_scale_addr, scale);
    this->HX711_inst->set_scale(scale);
    Serial.println("Scale read from EEPROM:");
    Serial.println(this->HX711_inst->get_scale());
}

void PressureSensor::Calibrate(int times)
{
    this->HX711_inst->tare(times);  

    EEPROM.put(this->e2p_offset_addr, this->HX711_inst->get_offset());
    Serial.println(this->HX711_inst->get_offset());
    Serial.println("Tare updated. Press any key to continue ...");
    while (!Serial.available()) {}  

    double scale = this->HX711_inst->get_value(times);
    this->HX711_inst->set_scale(scale/CALIBRATION_HEIGHT);

    EEPROM.put(this->e2p_scale_addr, this->HX711_inst->get_scale());
    Serial.println(this->HX711_inst->get_scale());

    Serial.println("Scale updated. Press any key to continue ...");
    while (!Serial.available()) {}
}

float PressureSensor::Read_Pressure()
{
    this->PR = this->HX711_inst->get_units(1);
    return this->PR;
}

float PressureSensor::get_peak_pr()
{
    return this->peak_pr;
}
    
void  PressureSensor::set_peak_pr(float new_peak_pr)
{
    this->peak_pr = new_peak_pr;
}

float PressureSensor::update_peak_pr()
{
    float current_pressure = this->Read_Pressure();

    if(current_pr > this->peak_pr)
        this->peak_pr = current_pr;

    return this->peak_pr;
}

void  PressureSensor::reset_peak_pr()
{
    this->peak_pr = 0.0;
}
