#ifndef __BSP_LIGHT_H
#define __BSP_LIGHT_H
#include "light.h"
// /* 继承灯类，并且将灯类的部分成员函数进行扩展，实现控制硬件的目的 */
class ESP01Light : public Light
{
private:
  unsigned char count=0;
public:
  bool LEDDirect = true; //定义灯渐变方向
  void setLedState(bool state);
  void setLedBright(int num);
  void slowBright(void);
  void awake(void);
};
#endif