#include "main_config.h"
#include "html.h"

const byte DNS_PORT = 53;             // dns端口号
IPAddress apIP(192, 168, 4, 1);       // ip地址
ESP8266WebServer server(80);          // http服务端口
DNSServer dnsServer;


/**
 * @descr 响应“/”的get请求
 * @param void
 * @return void
 */
void handleRoot()
{
  int numNetworks = WiFi.scanNetworks();

  /* HTML字符串拼接 */
  String content = htmlStart;
  // 添加扫描的wifi名称
  for (int i = 0; i < min(5, numNetworks); i++)
  {
    content += "<option value='"+ WiFi.SSID(i) +"'>" + WiFi.SSID(i) + "</option>";
  }
  content += htmlEnd;
  // 将字符串返回到浏览器
  server.send(200, "text/html", content);
}

/**
 * @desc 响应/save的post请求处理函数
 * @param void
 * @return void
 */
void handleSave()
{
  /* 获得表单信息 */
  String newSSID, newPassword, newUID, newTopic;
  if(server.hasArg("ssid")) newSSID = server.arg("ssid");
  if(server.hasArg("passwd")) newPassword = server.arg("passwd");
  if(server.hasArg("uid")) newUID = server.arg("uid");
  if(server.hasArg("topic")) newTopic = server.arg("topic");
  Serial.printf("ssid:%s, pwd:%s, uid: %s, topic:%s\n", newSSID.c_str(), newPassword.c_str(),newUID.c_str(), newTopic.c_str());
  /* 连接wifi */
  WiFi.begin(newSSID.c_str(), newPassword.c_str());
  Serial.print("Connecting to WiFi...");
  int attempts = 0;
  while (WiFi.waitForConnectResult() != WL_CONNECTED && attempts < 10)
  {
    delay(1000);
    Serial.print(".");
    attempts++;
  }
  /* 判断wifi状态 */
  if (WiFi.status() == WL_CONNECTED)
  {
    /* 连接成功 */
    Serial.println("\nWiFi connected");
    server.send(200, "text/html", "Configuration successful. Device connected to new Wi-Fi.");
    EEPROM.begin(1024);
    EEPROM_writeString(0, newSSID.c_str(), SSID_LEN);
    EEPROM_writeString(20, newPassword.c_str(), PASSWD_LEN);
    if(!newUID.isEmpty()) EEPROM_writeString(40, newUID.c_str(), UID_LEN);
    if(!newTopic.isEmpty()) EEPROM_writeString(80, newTopic.c_str(), TOPIC_LEN);
    EEPROM.end();
    WiFi.softAPdisconnect(true);            // 关闭接入点
    ESP.restart();                          // 系统重启
  }
  else
  {
    /* 连接失败则重新进入配置界面 */
    Serial.println("\nWiFi connection failed");
    handleRoot();
  }
}

void ap_webConfig_setup()
{
  // 设置AP模式
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssidAP, passwordAP);
  // 网络服务路由
  server.on("/", HTTP_GET, handleRoot);
  server.on("/save", HTTP_POST, handleSave);
  server.onNotFound(handleRoot);
  // 开启服务
  server.begin();
  // 开启DNS服务，实现强制门户
  if (!dnsServer.start(DNS_PORT, "*", apIP))
  {
    Serial.println("dnsServer failed");
  }
  while (1)
  {
    server.handleClient();
    dnsServer.processNextRequest();
  }
}