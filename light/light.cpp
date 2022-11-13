#include "light.h"

// 构造函数，初始化灯
Light::Light(){
  this->ledState = false;
  this->ledBright = 100;
}
Light::Light(void (*ledStateCb)(bool ledState, int ledBright),void (*ledBrightCb)(bool ledState, int ledBright)){
  this->ledState = false;
  this->ledBright = 100;
  this->ledStateCb = ledStateCb;
  this->ledBrightCb = ledBrightCb;
}

void Light::setLedState(bool state){
  this->ledState = state;
  if(this->ledStateCb){
    this->ledStateCb(this->ledState, this->ledBright);
  }
}

bool Light::getLedState(){
  return this->ledState;
}

void Light::turnLedState(){
  this->setLedState(!this->ledState);
}

void Light::setLedBright(int num){
  this->ledBright = num;
  if(this->ledBrightCb){
    this->ledBrightCb(this->ledState, this->ledBright);
  }
}

int Light::getLedBright(){
  return this->ledBright;
}