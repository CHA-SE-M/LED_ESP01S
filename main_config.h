
#ifndef __MAIN_CONFIG_H
#define __MAIN_CONFIG_H

#include <ESP8266WiFi.h> 
#include <ESP8266httpUpdate.h>
#include "PubSubClient.h" //默认，加载MQTT库文件
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <Ticker.h>
#include <string.h>
#include "light.h"
#include <EEPROM.h>
#include "ap_webconfig.h"
#include "bsp_btn.h"
#include "bsp_light.h"
#include "bsp_EEPROM.h"

//********************需要修改的部分*******************//
#define LED 0                                               // 单片机LED引脚值
#define BUTTON 2                                            // 设备的物理按键引脚
#define INTERVAL 100                                        // 物理按键检测间隔
#define LONG_PRESS 300                                      // 物理按键长按定义
#define SSID_LEN 20                                         // WIFI名称最大长度
#define PASSWD_LEN 20                                       // WIFI密码最大长度
#define UID_LEN 40                                          // 巴法云uid最大长度
#define TOPIC_LEN 20                                        // 主题最大长度
#define MQTT_SERVER "bemfa.com"                             // 默认，MQTT服务网址
#define MQTT_SERVER_PORT 9501                               // 默认，MQTT服务端口
#define ssidAP "LEDConfig"
#define passwordAP "password"
//**************************************************//
#endif
