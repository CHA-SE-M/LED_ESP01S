#include "light.h"

// 构造函数，初始化灯
Light::Light(){
  this->ledState = false;
  this->ledBright = 100;
}

// 设置灯的状态
void Light::setLedState(bool state){
  this->ledState = state;
}

// 得到灯的状态
bool Light::getLedState(){
  return this->ledState;
}

// 切换灯的开关
void Light::turnLedState(){
  this->setLedState(!this->ledState);
}

void Light::setLedBright(int num){
  this->ledBright = num;
  if(num == 0)
  {
    this->ledState = false;
  }else
  {
    this->ledState=true;
  }
  
  
}

int Light::getLedBright(){
  return this->ledBright;
}
