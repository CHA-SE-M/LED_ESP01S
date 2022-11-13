#ifndef LIGHT_H
  #define LIGHT_H
#endif

#ifndef LIGHT_ON
  #define LIGHT_ON true       // 定义开灯状态
#endif

#ifndef LIGHT_OFF
  #define LIGHT_OFF false     // 定义关灯灯状态
#endif

class Light {
  private:
    bool ledState;  // 灯的状态true:开灯，false：关灯
    int ledBright;  // 灯的亮度
    void (*ledStateCb)(bool ledState, int ledBright);// 灯状态改变时回调
    void (*ledBrightCb)(bool ledState, int ledBright);// 灯亮度改变时回调
  public:
    Light();                // 构造函数
    Light(void (*ledStateCb)(bool ledState, int ledBright), void (*ledBrightCb)(bool ledState, int ledBright)); // 构造函数
    void setLedState(bool state); // 设置灯的状态
    bool getLedState();           // 获得灯的状态
    void turnLedState();          // 切换灯的状态
    void setLedBright(int num);   // 设置灯的亮度
    int getLedBright();           // 获得灯的亮度
};