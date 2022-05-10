#include "DS18B20.h"
#include "1wire_basic.h"
#include "1wire.h"

void OW_Start_Conversion(uint8_t block)
{
 if(OW_WaitForPresencePulse()==0) return;
 OW_Write(OW_SkipROM);
 OW_Write(OW_CONVERT);
 if(block) while(!OW_ReadBit());
}

int16_t OW_GetTemperature(uint8_t *aID)
{
 OW_SelectDevice(aID);
 if(Error!=OW_OK) return 0xFFFF;

 OW_Write(OW_READ_SCRATCHPAD);
 uint16_t temp=OW_Read();
 temp=temp+(OW_Read()<<8);
 if(temp & 0x8000) temp=1-(temp ^ 0xFFFF); //Konwersja kodów
 return temp;
}
