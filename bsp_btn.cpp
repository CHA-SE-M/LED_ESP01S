#include "main_config.h"

static long prevTime = 0;
static long nowTime = 0;
static bool press = false;
int checkBtn(int pin, int state)
{
  if (digitalRead(pin) == state)
  {
    nowTime = millis();

    if (nowTime - prevTime > LONG_PRESS)
    {
      return 2;
      delay(10);
    }
    else if (nowTime - prevTime > 5)
    {
      press = true;
    }
    return 0;
  }
  else if (press)
  {
    press = false;
    nowTime = millis();
    if (nowTime - prevTime > LONG_PRESS)
    {
      return 3;
    }
    else if (nowTime - prevTime > 5)
    {
      return 1;
    }
  }
  prevTime = millis();
  return 0;
}