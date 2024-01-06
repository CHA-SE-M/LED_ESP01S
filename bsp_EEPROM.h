#ifndef __BSP_EEPROM_H
#define __BSP_EEPROM_H
unsigned char EEPROM_readString(int addr, char* readStr, char maxLen);
void EEPROM_writeString(int addr, const char* writeStr, char maxLen);
#endif