#include "bsp_EEPROM.h"
#include <EEPROM.h>


/* 返回读出字符串长度 */
unsigned char EEPROM_readString(int addr, char* readStr, char maxLen)
{
  unsigned char i=0;
  while (EEPROM.read(addr+i) != '\0' && i < maxLen-1)
  {
    *(readStr+i) = EEPROM.read(addr+i);
    i++;
  }
  *(readStr+i) = '\0';
  return i;
}

/* 写字符串函数 */
void EEPROM_writeString(int addr,const char* writeStr, char maxLen)
{
  unsigned char i = 0;
  while(*(writeStr+i) != '\0' && i < maxLen-1)
  {
    EEPROM.write(addr+i, *(writeStr+i));
    i++;
  }
  EEPROM.write(addr+i, '\0');
  EEPROM.commit();
}