#ifndef LIGHT_H
  #define LIGHT_H
  #define LIGHT_ON true       // 定义开灯状态
  #define LIGHT_OFF false     // 定义关灯灯状态

class Light {
  protected:
    bool ledState;  // 灯的状态true:开灯，false：关灯
    int ledBright;  // 灯的亮度
  public:
    Light();                // 构造函数
    void setLedState(bool state); // 设置灯的状态
    bool getLedState();           // 获得灯的状态
    void turnLedState();          // 切换灯的状态
    void setLedBright(int num);   // 设置灯的亮度
    int getLedBright();           // 获得灯的亮度
    
};

#endif