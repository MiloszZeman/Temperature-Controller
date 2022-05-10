#include <util\delay.h>
#include <util\crc16.h>
#include "1wire_basic.h"
#include "1wire.h"

void OW_Write(uint8_t byte)
{
	unsigned char loop;
	for(loop=0; loop<8; loop++)
	{
		OW_SendBit(byte & 0x01);
		byte>>=1;
   		_delay_us(130);
    }
}

uint8_t OW_Read()
{
	unsigned char loop;
	unsigned char result=0;

	for(loop=0; loop<8; loop++)
	{
		result>>=1;
		if(OW_ReadBit()) result|=0x80;
   		_delay_us(150);
  	} 
 	return result;
}

void OW_SelectDevice(const uint8_t *aID)
{
	if(OW_WaitForPresencePulse()==false) return;
	OW_Write(OW_MatchROM);
	uint8_t crc=0;
	for(uint8_t a=0;a<7;a++)
	{
	 crc=_crc_ibutton_update(crc, aID[a]);
	 OW_Write(aID[a]);
	}
	OW_Write(crc);
}

uint8_t OWI_Search(uint8_t cmd, uint8_t *aID, uint8_t deviationpos)
{
 uint8_t newPos=0;
 uint8_t bitMask=0x01;
 uint8_t bitA, bitB;

 OW_WaitForPresencePulse();	//Inicjalizacja magistrali
 OW_Write(cmd);     //Wyœlij polecenie SearchROM
    
 for(uint8_t currentBit=1; currentBit<=64;currentBit++)
  {
   _delay_us(60);
   bitA=OW_ReadBit();    //Odczyt bitu
   _delay_us(60);
   bitB=OW_ReadBit();    //i jego dope³nenia
   _delay_us(60);
		
   if(bitA && bitB)
    {
     Error=OW_SearchNoResponse; //B³¹d - oba bity równe 1
     return 0; //Zwróæ cokolwiek dla unikniêcia ostrze¿enia kompilatora
    }
   
   if(bitA ^ bitB)
    { //Na tej pozycji wszystkie urz¹dzenia maj¹ bity o tej samej wartoœci
     if(bitA) (*aID)|=bitMask; //Ustaw odpowiedni bit ID
      else (*aID)&=~bitMask;
    }
     else // oba bity równe 0
      {
       if(currentBit==deviationpos) (*aID)|=bitMask; //Tu ostatnio wybrano 0, teraz bêdzie 1
       
	   if(currentBit>deviationpos)
        {
         (*aID)&=~bitMask;
         newPos=currentBit;
        }
         else if(!(*aID & bitMask)) newPos=currentBit;
      }
                
   OW_SendBit((*aID) & bitMask); //Wyœlij wartoœæ wybranego bitu
   bitMask<<=1;
   if (!bitMask)
    {
     bitMask=0x01;
     aID++;
    }
  }
 return newPos;
}
