/*
* 智能语言控制控制，支持同时天猫、小爱、小度、google Assistent控制
* 也同时支持web控制、小程序控制、app控制，定时控制等
* QQ群：566565915
* 项目示例：通过发送on或off控制开关
* 官网：bemfa.com
*/

#include <ESP8266WiFi.h>  //默认，加载WIFI头文件
#include <ESP8266httpUpdate.h>
#include "PubSubClient.h" //默认，加载MQTT库文件
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <Ticker.h>
#include <string.h>

//********************需要修改的部分*******************//
String upUrl = "http://bin.bemfa.com/b/1BcMTE3M2ZiYWQ3NjAzNGY3YzM2MjlhNGExMGQ1ZGVlNTc=light002.bin"; // 升级网址
#define ID_MQTT "1173fbad76034f7c3629a4a10d5dee57" //用户私钥，控制台获取
const char *topic = "light002";                    //主题名字，可在巴法云控制台自行创建，名称随意
const char *topicUp = "light002/up";               //只更新数据
#define LED 0                                      //单片机LED引脚值
#define BUTTON 2                                   //设备的物理按键引脚
#define INTERVAL 100                               //物理按键检测间隔
//**************************************************//

const char *mqtt_server = "bemfa.com"; //默认，MQTT服务器
const int mqtt_server_port = 9501;     //默认，MQTT服务器
WiFiClient espClient;
PubSubClient client(espClient);
Ticker ticker;

//灯光函数及引脚定义
void turnOnLed();
void turnOffLed();

// 定义灯的亮度
int lightNum = 100;
//定义灯的状态
boolean LEDState = false;
//定义灯渐变方向
boolean LEDDirect = true;

void setup_wifi()
{
  Serial.begin(9600);
  // 建立WiFiManager对象
  WiFiManager wifiManager;

  // 自动连接WiFi。以下语句的参数是连接ESP8266时的WiFi名称
  wifiManager.autoConnect("linght1");

  // 如果您希望该WiFi添加密码，可以使用以下语句：
  // wifiManager.autoConnect("AutoConnectAP", "12345678");
  // 以上语句中的12345678是连接AutoConnectAP的密码

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
  //  Serial.print("cmd:");
  //  Serial.println(cmd);
  String num = strtok(NULL, "#");
  //  Serial.print("lightNum:");
  //  Serial.println(num);
  if (num != NULL)
  {
    lightNum = atoi(&num[0]);
    //      Serial.print("lightNum");
    //      Serial.println(lightNum);
  }
  if (cmd == "on")
  {              //如果接收字符on，亮灯
    turnOnLed(); //开灯函数
  }
  else if (cmd == "off")
  {               //如果接收字符off，关灯
    turnOffLed(); //关灯函数
  }
  else if (cmd == "wake")
  {
    wake();
  }
  else if (cmd == "update")
  {
    updateBin();
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

//处理长按逻辑
void longPress()
{
  if(LEDDirect){
    lightNum++;
  }else{
    lightNum--;
  }
  turnOnLed();
  if(lightNum >= 100) LEDDirect = false;
  if(lightNum <= 1) LEDDirect = true;
}

void btnOpera()
{
  int num = checkBtn(BUTTON, LOW, longPress);
  // 短按逻辑
  if (num == 1)
  {
    if (LEDState)
    {
      turnOffLed();
      pubMQTTmsg("off");
    }
    else
    {
      turnOnLed();
      pubMQTTmsg("on");
    }
  }else if (num == 2){
      char str[10];
      char str1[10] = "on#";
      itoa(lightNum,str,10);
      strcat(str1, str);
      pubMQTTmsg(strcat("on#",str1));
  }
}

int checkBtn(int pin, int state, void (*ptr)())
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
          ptr();
        }
        return 2;
      }
    }
    return 0;
  }else{
    return 0;
  }
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

//打开灯泡
void turnOnLed()
{
  Serial.println("turn on light");
  if (lightNum == 100)
  {
    digitalWrite(LED, HIGH);
  }
  else
  {
    analogWrite(LED, 10 * lightNum);
  }
  LEDState = true;
}
//关闭灯泡
void turnOffLed()
{
  Serial.println("turn off light");
  digitalWrite(LED, LOW);
  LEDState = false;
}
//逐渐开灯
void wake()
{
  for (int i = 1; i < 1000; i++)
  {
    analogWrite(LED, i);
    delay(3);
  }
  digitalWrite(LED, HIGH);
  LEDState = true;
}

//当升级开始时，打印日志
void update_started() {
  Serial.println("CALLBACK:  HTTP update process started");
  digitalWrite(LED,HIGH);
  delay(100);
  digitalWrite(LED,LOW);
  delay(100);
  digitalWrite(LED,HIGH);
  delay(100);
  digitalWrite(LED,LOW);
  delay(100);
}

//当升级结束时，打印日志
void update_finished() {
  Serial.println("CALLBACK:  HTTP update process finished");
  digitalWrite(LED,HIGH);
}

//当升级中，打印日志
void update_progress(int cur, int total) {
  Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
}

//当升级失败时，打印日志
void update_error(int err) {
  Serial.printf("CALLBACK:  HTTP update fatal error code %d\n", err);
}

/**
 * 固件升级函数
 * 在需要升级的地方，加上这个函数即可，例如setup中加的updateBin(); 
 * 原理：通过http请求获取远程固件，实现升级
 */
void updateBin(){
  Serial.println("start update");    
  WiFiClient UpdateClient;

  ESPhttpUpdate.onStart(update_started);//当升级开始时
  ESPhttpUpdate.onEnd(update_finished); //当升级结束时
  ESPhttpUpdate.onProgress(update_progress); //当升级中
  ESPhttpUpdate.onError(update_error); //当升级失败时

  t_httpUpdate_return ret = ESPhttpUpdate.update(UpdateClient, upUrl);
  switch(ret) {
    case HTTP_UPDATE_FAILED:      //当升级失败
        Serial.println("[update] Update failed.");
        break;
    case HTTP_UPDATE_NO_UPDATES:  //当无升级
        Serial.println("[update] Update no Update.");
        break;
    case HTTP_UPDATE_OK:         //当升级成功
        Serial.println("[update] Update ok.");
        digitalWrite(LED,HIGH);
        break;
  }
}
