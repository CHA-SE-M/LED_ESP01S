#include "main_config.h"

void ESP01Light::setLedState(bool state)
{
  Light::setLedState(state);
  if (state == LIGHT_ON)
  {
    analogWrite(LED, 255*this->ledBright/1000);
  }
  else
  {
    analogWrite(LED, 0);
  }
}
void ESP01Light::setLedBright(int num)
{
  Light::setLedBright(num);
  analogWrite(LED, 255*num/1000);
}

void ESP01Light::slowBright(void)
{
  this->count++;
  if(this->count/4 == 0)
  {
    if(this->ledBright >= 1000) this->LEDDirect = false;
    if(this->ledBright <= 0) this->LEDDirect = true;
    if(this->LEDDirect)
    {
      this->setLedBright(++(this->ledBright));
    }
    else
    {
      this->setLedBright(--(this->ledBright));
    }
  }
  
}

void ESP01Light::awake(void)
{
  if(this->ledState == false)
  {
    for (int i = 1; i <= 1000; i++)
    {
      this->setLedBright(i);
      delay(3);
    }
  }
}