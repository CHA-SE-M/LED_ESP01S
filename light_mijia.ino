/*
 * esp01S控制器，利用巴法云
 */
#include "main_config.h"

/********************** 全局变量 ********************/
char main_uid[UID_LEN];                                    // 用户私钥，控制台获取
char main_topic[TOPIC_LEN];                                // 主题名字，可在巴法云控制台自行创建，名称随意
char main_ssid[SSID_LEN];                                  // wifi名数组            
char main_passwd[PASSWD_LEN];                              // wifi密码数组
ESP01Light light;                                          // 创建灯对象
WiFiClient espClient;
Ticker ticker;
byte wifi_connectState = 0;
PubSubClient client(espClient);
/**********************************************/



/* 返回0初始化成功，且获得的信息不为空 */
uint8_t EEPROM_Init(void)
{
  EEPROM.begin(1024);
  byte ssidLen = EEPROM_readString(0, main_ssid, SSID_LEN);
  byte passwdLen = EEPROM_readString(20, main_passwd, PASSWD_LEN);
  byte uidLen = EEPROM_readString(40, main_uid, UID_LEN);
  byte topicLen = EEPROM_readString(80, main_topic, TOPIC_LEN);
  EEPROM.end();
  if(ssidLen && passwdLen && uidLen && topicLen)
  {
    Serial.println("EEPROM init success");
    return 0;
  }
  else
  {
    Serial.println("EEPROM init failed");
    return 1;
  }
}


void config_btn_check()
{
  if(digitalRead(BUTTON) == LOW)
  {
    Serial.println("WEB config");
    // 灯闪烁两次说明进入配置界面
    digitalWrite(LED, LOW);
    delay(500);
    digitalWrite(LED, HIGH);
    delay(500);
    digitalWrite(LED, LOW);
    delay(500);
    digitalWrite(LED, HIGH);
    ap_webConfig_setup();
  }
}

// void wifi_tryReconect(void)
// {
//   if(WiFi.waitForConnectResult() != WL_CONNECTED)
//   {
//     // 开始连接wifi
//     WiFi.mode(WIFI_STA);
//     WiFi.begin(main_ssid, main_passwd);
//   }
//   else
//   {
//     // 当检测到wifi连接成功后清除该定时器,将wifi_connectState置1
//     wifi_connectState = 1;
//     ticker.detach();
//   }
// }

unsigned char wifi_connect(char* ssid, char* passwd)
{
  Serial.printf("will to connect SSID: %s, PWD: %s\n",ssid,passwd);
  Serial.printf("uid:%s, topic:%s\n", main_uid, main_topic);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, passwd);
  /* 配置按键检测部分 */
  unsigned char delayTime = 0;
  while(delayTime < 40)
  {
    config_btn_check();
    delayTime++;
    delay(100);
  }
  /* wifi结果检测 */
   Serial.print("Connecting to WiFi...");
  unsigned char attempts = 0;
  while (WiFi.waitForConnectResult() != WL_CONNECTED && attempts < 8)
  {
    delay(1000);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("\nWiFi connected");
    return 0;
  }
  else
  {
    Serial.println("\nWiFi connection failed");
    return 1;
  }
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
      Serial.println("set light bright");
      light.setLedBright(atoi(&num[0])*10);
    }
    else
    {
      // 开灯
      Serial.println("turn on light");
      light.setLedState(LIGHT_ON);
    }
  }
  else if (cmd == "off")
  {
    // 关灯
    Serial.println("turn off light");
    light.setLedState(LIGHT_OFF);
  }
  else if (cmd == "wake")
  {
    // 缓慢开灯
    Serial.println("Slow brightening");
    light.awake();
  }
  msg = "";
}

void reconnect()
{
  // 设置5s的重连间隔
  static unsigned long nextTime = 0;
  if (!client.connected())
  {
    if(nextTime - millis() < 5000) return;
    nextTime += 5000;
    Serial.print("Attempting MQTT connection...");
    /* 尝试连接 */
    if (client.connect(main_uid))
    {
      Serial.println("connected");
      Serial.print("subscribe:");
      Serial.println(main_topic);
      //订阅主题，如果需要订阅多个主题，可发送多条订阅指令client.subscribe(topic2);client.subscribe(topic3);
      client.subscribe(main_topic);
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
    }
  }
}

// 向巴法云发布信息
void pubMQTTmsg(const char *msg)
{
  if(WiFi.waitForConnectResult() != WL_CONNECTED) return;
  // 实现ESP8266向主题发布信息
  char dest[TOPIC_LEN+3]={0};
  strcat(dest, main_topic);
  strcat(dest, "/up");
  if (client.publish(dest, msg))
  {
    Serial.println("Publish Topic:");
    Serial.println(main_topic);
    Serial.println("Publish message:");
    Serial.println(dest);
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
        light.setLedState(LIGHT_OFF);
        pubMQTTmsg("off");
        Serial.println("off");
      }
      else
      {
        light.setLedState(LIGHT_ON);
        pubMQTTmsg("on");
        Serial.println("on");
      }
      break;
    }
    case 2:
    {
      light.slowBright();
      break;
    }
    case 3:
    {
      itoa(light.getLedBright()/10, &str[3], 10);
      pubMQTTmsg(str);
      break;
    }
    default:{
      ;
    }
  }
}

void setup()
{
  pinMode(LED, OUTPUT);                            //设置引脚为输出模式
  pinMode(BUTTON, INPUT_PULLUP);                   //设置按键上拉输出模式
  digitalWrite(LED, HIGH);                         //默认引脚上电高电平
  Serial.begin(9600);                              //设置波特率9600
  Serial.println(" ");
  Serial.println("start");
  
  /* EEPROM初始化，并且读取ssid,passwd等数据 */
  if(EEPROM_Init())
  {
    /* EEPROM初始化失败进行网络配网 */
    ap_webConfig_setup();
  }
  
  /* EEPROM初始化成功则连接wifi，在wifi连接期间进行配置界面按键检测，代码在wifi_connect函数里 */
  if(wifi_connect(main_ssid, main_passwd))
  {
    /* 如果未连接上wifi，使用ticker每隔一段时间进行WiFi联网 */
    Serial.println("start wifi connect loop");
    // ticker.attach(10, wifi_tryReconect);
    wifi_connectState == 0;
    while(1)
    {
      if(WiFi.waitForConnectResult() == WL_CONNECTED) break;
      btnOpera();
      delay(20);
    }
  }
  wifi_connectState = 1;
  client.setServer(MQTT_SERVER, MQTT_SERVER_PORT); //设置mqtt服务器
  client.setCallback(callback);                    //mqtt消息处理
  // ticker.attach_ms(INTERVAL, btnOpera);            //按键检测
  digitalWrite(LED, LOW);
}

void loop()
{
  /* 检测是否连接上物联网云平台 */
  if (!client.connected())
  {
    reconnect();
  }
  else
  {
    client.loop();
  }
  btnOpera();
}
