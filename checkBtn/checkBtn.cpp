#include <arduino.h>
/**
 * @desp检测物理按键
 * @params pin 按键引脚
 *         state 按键有效状态
 *         ptr 长按执行函数
 * @return 0 无按键操作，1 短按 2长按
 **/
int checkBtn(int pin, int state)
{
  if (digitalRead(pin) == state)
  {
    delay(5);
    if (digitalRead(pin) == state){
      delay(300);
      if(digitalRead(pin) != state){
        return 1;
      }else{
        while(digitalRead(pin) == state){
          // *(ptr)();
        }
        return 2;
      }
    }
  }else{
    return 0;
  }
}

int checkBtn(){
  return 0;
}
// int checkBtn(int pin, int state, int(*ptr)())
// {
//   if (digitalRead(pin) == state)
//   {
//     delay(5);
//     if (digitalRead(pin) == state)
//     {
//       if (LEDState)
//       {
//         turnOffLed();
//         pubMQTTmsg("off");
//       }
//       else
//       {
//         turnOnLed();
//         pubMQTTmsg("on");
//       }
//       while (digitalRead(BUTTON) == LOW)
//       {
//       };
//     }
//   }
// }