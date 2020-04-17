#include <sysconfig.h>
#include <math.h>

SysConfig::SysConfig(int IE_Ratio, int Resp_Rate, int Tidal_Volume)
{
    this->IE_Ratio = IE_Ratio;
    this->Resp_Rate = Resp_Rate;
    this->Tidal_Volume = Tidal_Volume;
}

int SysConfig::get_IE_Ratio()
{
    return this->IE_Ratio;
}

int SysConfig::get_Inh_Time()
{
    return this->Inhale_Time;
}

int SysConfig::get_Exh_Time()
{
    return this->Exhale_Time;
}

void SysConfig::set_IE_Ratio(int IE_Ratio)
{
    this->IE_Ratio = IE_Ratio;
}

void SysConfig::set_Resp_Rate(int Resp_Rate)
{
    this->Resp_Rate = Resp_Rate;
    this->set_Inh_Time();
}

void SysConfig::set_Tidal_Volume(int Tidal_Volume)
{
    this->Tidal_Volume = Tidal_Volume;
}

void SysConfig::set_Inh_Time()
{
    this->Inhale_Time=(int)(60000)/(get_Resp_Rate()*(get_IE_Ratio()+1));
}

void SysConfig::set_Exh_Time()
{
    this->Exhale_Time=(int)(60000)*(get_IE_Ratio())/(get_Resp_Rate()*(get_IE_Ratio()+1));
}

void SysConfig::set_Start_Time(){
    this->Start_Time=millis();
}

void SysConfig::update(SysConfig* new_config)
{
    this->set_IE_Ratio(new_config->get_IE_Ratio());
    this->set_Resp_Rate(new_config->get_Resp_Rate());
    this->set_Tidal_Volume(new_config->get_Tidal_Volume());
    this->set_Inh_Time();
    this->set_Exh_Time();
}
