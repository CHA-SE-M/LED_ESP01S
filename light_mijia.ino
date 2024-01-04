/*
* 智能语言控制控制，支持同时天猫、小爱、小度、google Assistent控制
* 也同时支持web控制、小程序控制、app控制，定时控制等
* QQ群：566565915
* 项目示例：通过发送on或off控制开关
* 官网：bemfa.com
*/

#include <ESP8266WiFi.h> //默认，加载WIFI头文件
#include <ESP8266httpUpdate.h>
#include "PubSubClient.h" //默认，加载MQTT库文件
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <Ticker.h>
#include <string.h>
#include "light.h"

//********************需要修改的部分*******************//
int aaa = 0;
String upUrl = "http://bin.bemfa.com/b/1BcMTE3M2ZiYWQ3NjAzNGY3YzM2MjlhNGExMGQ1ZGVlNTc=light1002.bin"; // 升级网址
#define ID_MQTT "1173fbad76034f7c3629a4a10d5dee57"                                                   //用户私钥，控制台获取
const char *topic = "light1002";                                                                      //主题名字，可在巴法云控制台自行创建，名称随意
const char *topicUp = "light1002/up";                                                                 //只更新数据
#define LED 0                                                                                        //单片机LED引脚值
#define BUTTON 2                                                                                     //设备的物理按键引脚
#define INTERVAL 100                                                                                 //物理按键检测间隔
#define LONG_PRESS 300                                                                               //物理按键长按定义
//**************************************************//
class ESP01Light : public Light
{
public:
  boolean LEDDirect = true; //定义灯渐变方向
  void setLedState(bool state)
  {
    Light::setLedState(state);
    if (state == LIGHT_ON)
    {
      analogWrite(LED, 10 * this->ledBright);
    }
    else
    {
      analogWrite(LED, 0);
    }
  }
  void setLedBright(int num)
  {
    Light::setLedBright(num);
    analogWrite(LED, 10 * num);
  }
};

const char *mqtt_server = "bemfa.com"; //默认，MQTT服务器
const int mqtt_server_port = 9501;     //默认，MQTT服务器
WiFiClient espClient;
PubSubClient client(espClient);
Ticker ticker;

// 创建灯对象
ESP01Light light;

//打开灯泡
void turnOnLed()
{
  Serial.println("turn on light");
  light.setLedState(LIGHT_ON);
}

//关闭灯泡
void turnOffLed()
{
  Serial.println("turn off light");
  light.setLedState(LIGHT_OFF);
}

// 设置亮度
void setBright(int num)
{
  Serial.println("set light bright");
  light.setLedBright(num);
}

//逐渐开灯
void wake()
{
  if(light.getLedState() == false)
  {
    for (int i = 1; i <= 100; i++)
    {
      light.setLedBright(i);
      delay(30);
    }
  }
 
}

// 连接wifi
void setup_wifi()
{
  Serial.begin(9600);
  // 建立WiFiManager对象
  WiFiManager wifiManager;
  // 自动连接WiFi。以下语句的参数是连接ESP8266时的WiFi名称
  wifiManager.autoConnect("light");
  // WiFi连接成功后将通过串口监视器输出连接成功信息
  Serial.println("");
  Serial.print("ESP8266 Connected to ");
  Serial.println(WiFi.SSID()); // WiFi名称
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP()); // IP
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Topic:");
  Serial.println(topic);
  String msg = "";
  for (int i = 0; i < length; i++)
  {
    msg += (char)payload[i];
  }
  Serial.print("Msg:");
  Serial.println(msg);
  String cmd = strtok(&msg[0], "#");
  String num = strtok(NULL, "#");
  if (cmd == "on")
  {
    if (num != "")
    {
      // 设置亮度
      setBright(atoi(&num[0]));
    }
    else
    {
      // 开灯
      turnOnLed();
    }
  }
  else if (cmd == "off")
  {
    // 关灯
    turnOffLed();
  }
  else if (cmd == "wake")
  {
    // 缓慢开灯
    wake();
  }
  msg = "";
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(ID_MQTT))
    {
      Serial.println("connected");
      Serial.print("subscribe:");
      Serial.println(topic);
      //订阅主题，如果需要订阅多个主题，可发送多条订阅指令client.subscribe(topic2);client.subscribe(topic3);
      client.subscribe(topic);
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// 向巴法云发布信息
void pubMQTTmsg(char *msg)
{
  static int value; // 客户端发布信息用数字

  // // 建立发布信息。信息内容以Hello World为起始，后面添加发布次数。
  // String messageString = "Hello " + String(value++);
  // char pubMessage[messageString.length() + 1];
  // strcpy(pubMessage, messageString.c_str());

  // 实现ESP8266向主题发布信息
  if (client.publish(topicUp, msg))
  {
    Serial.println("Publish Topic:");
    Serial.println(topic);
    Serial.println("Publish message:");
    Serial.println(topic);
  }
  else
  {
    Serial.println("Message Publish Failed.");
  }
}

void btnOpera()
{
  int num = checkBtn(BUTTON, LOW);
  char str[10] = "on#";
  switch(num)
  {
    // 短按逻辑
    case 1:
    {
      if (light.getLedState() == true)
      {
        turnOffLed();
        pubMQTTmsg("off");
      }
      else
      {
        turnOnLed();
        pubMQTTmsg("on");
      }
      break;
    }
    case 2:
    {
      if (light.getLedBright() >= 100)
      {
        light.LEDDirect = false;
      }
      if (light.getLedBright() <= 1)
      {
        light.LEDDirect = true;
      }
      if (light.LEDDirect)
      {
        setBright(light.getLedBright() + 1);
      }
      else
      {
        setBright(light.getLedBright() - 1);
      }
      break;
    }
    case 3:
    {
      itoa(light.getLedBright(), &str[3], 10);
      pubMQTTmsg(str);
      break;
    }
    default:{
      ;
    }
  }
}
long prevTime = 0;
long nowTime = 0;
bool press = false;
int checkBtn(int pin, int state)
{
  if (digitalRead(BUTTON) == LOW)
  {
    nowTime = millis();

    if (nowTime - prevTime > LONG_PRESS)
    {
      return 2;
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

void setup()
{
  pinMode(LED, OUTPUT);                            //设置引脚为输出模式
  pinMode(BUTTON, INPUT_PULLUP);                   //设置按键上拉输出模式
  digitalWrite(LED, HIGH);                         //默认引脚上电高电平
  Serial.begin(9600);                              //设置波特率9600
  setup_wifi();                                    //设置wifi的函数，连接wifi
  client.setServer(mqtt_server, mqtt_server_port); //设置mqtt服务器
  client.setCallback(callback);                    //mqtt消息处理
  // ticker.attach_ms(INTERVAL, btnOpera);            //按键检测
  digitalWrite(LED, LOW);
}
void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
  btnOpera();
}
