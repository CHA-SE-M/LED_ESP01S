#include "light.h"

// 构造函数，初始化灯
Light::Light(){
  this->ledState = false;
  this->ledBright = 100;
}

void Light::setLedState(bool state){
  this->ledState = state;
}

bool Light::getLedState(){
  return this->ledState;
}

void Light::turnLedState(){
  this->setLedState(!this->ledState);
}

void Light::setLedBright(int num){
  this->ledBright = num;
}

int Light::getLedBright(){
  return this->ledBright;
}