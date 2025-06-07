#include<LPC21XX.H>

#include"define.h"

#include"I2C.h"

#include"lcd.h"

#include"EEPROM.h"


#define sw1 6

#define sw2 7
#define LED 1<<8


void rtc(void);

int main()


{

	
 /*
	i2c_int();

	eeprom_write(0X50,0X00,0);

	  eeprom_write(0X50,0X01,0);

	eeprom_write(0x68,0x00,0x00);

	eeprom_write(0x68,0x01,0x25);

	eeprom_write(0x068,0x02,0x09|1<<6);

*/

      int ci,co,tot;
	  IODIR0|=LED;
	  IOSET0|=LED;
	  
      i2c_int();
      LCD_INT();
	  LCD_SRL("Bidirectional visitor counter ");
	  LCD_CMD(0X01);
	  ci=eeprom_read(0X50,0X00);
	  LCD_CMD(0x80);
	  LCD_STR("IN:");
	  LCD_IN(ci);
  	  co=eeprom_read(0X50,0X01);
	  LCD_CMD(0x87);
	  LCD_STR("OUT:");
	  LCD_IN(co);
	  tot=ci-co;
	  LCD_CMD(0XC0);
	  LCD_STR("TOL:");
	  LCD_IN(tot);
	  if(tot>0)
	    IOCLR0=LED;
		else if(tot==0)
		IOSET0=LED;

 while(1)
 {
	 if (((IOPIN0>>sw1)&1)==0)
 {

     ci=eeprom_read(0X50,0X00);
	 delay(50);
	 co=eeprom_read(0X50,0X01);
	 delay(50);
     eeprom_write(0X50,0X00,++ci);

	 LCD_CMD(0x01);
	 LCD_CMD(0x80);
	 LCD_STR("IN:");
     LCD_CMD(0X83);
	 LCD_IN(ci);
   	 LCD_CMD(0x87);
	 LCD_STR("OUT:"); 
	 LCD_CMD(0X8B);
	  LCD_IN(co);
      tot=ci-co;
	  LCD_CMD(0XC0);
	  LCD_STR("TOL:");
	  LCD_IN(tot);
	   
	  rtc();	

  //while(((IOPIN0>>sw1)&1)==0);	 

  }

  else if (((IOPIN0>>sw2)&1)==0)

 {

     co=eeprom_read(0X50,0X01);
	 delay(50);
	 ci=eeprom_read(0X50,0X00);
	 delay(50);
     eeprom_write(0X50,0X01,++co);

      tot=ci-co;
	  LCD_CMD(0X01);
	  LCD_CMD(0x80);
	  LCD_STR("IN:");
      LCD_CMD(0X83);
	  LCD_IN(ci);
	  LCD_CMD(0x87);
	  LCD_STR("OUT:"); 
	  LCD_CMD(0X8B);
	  LCD_IN(co);
      tot=ci-co;
	  LCD_CMD(0XC0);
	  LCD_STR("TOL:");
	  LCD_IN(tot);
	    
	  rtc();
	//  while(((IOPIN0>>sw2)&1)==0);

  }
  if(tot>0)
	    IOCLR0=LED;
		else if(tot==0)
		IOSET0=LED;
  rtc();

  }
	
}

void rtc(void)
{
	unsigned char h,m,s,hr;
	int ci,co,tot,min,sec;
	h=eeprom_read(0x68,0x02);
	delay(50);
	m=eeprom_read(0x68,0x01);
	delay(50);
	s=eeprom_read(0x68,0x00);
	delay(50);
	 hr=h;

	//hr=((h/16)*10)+(h%10);
	//min=((m/16)*10)+(m%10);
	//sec=((s/16)*10)+(s%10);
	m=((m>>4)*10)+(m&0x0f);
	s=((s>>4)*10)+(s&0x0f);
	
	h=(((h>>4)&1)*10)+(h&0x0f);
	min=m;
	sec=s;
	LCD_CMD(0Xc7);
	LCD_IN(h);
	LCD_DATA(':');
	LCD_IN(m);
	LCD_DATA(':');
	LCD_IN(s);

	if(((hr>>5)&1)==1)
	   LCD_STR("PM");

	else
       LCD_STR("AM");

	if(((min%2)==0)&&(sec==51))
	{
	  eeprom_write(0X50,0X00,0);
	  eeprom_write(0X50,0X01,0);
	  ci=eeprom_read(0X50,0X00);
	  LCD_CMD(0x01);
	  LCD_CMD(0x80);
	  LCD_STR("IN:");
	  LCD_IN(ci);
  	  co=eeprom_read(0X50,0X01);
	  LCD_CMD(0x87);
	  LCD_STR("OUT:");
	  LCD_IN(co);
	  tot=ci-co;
	  LCD_CMD(0XC0);
	  LCD_STR("TOL:");
	  LCD_IN(tot);
	  if(tot==0)
		IOSET0=LED;

	}

 	
}  