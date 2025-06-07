//#include"delay.h"
#define lcd_d 0xf<<20                                                
#define RS 1<<17
#define RW 1<<18
#define e 1<<19
typedef unsigned char u8;
void lcd_config(void);
void lcd_com(u8);
void lcd_data(u8);
void lcd_str(u8*);
				    			      
void lcd_config(void)
{
//PINSEL0 | =0;
IODIR1|=lcd_d|RS|e|RW;
lcd_com(0x01);
lcd_com(0x02);
lcd_com(0x0c);
lcd_com(0x28);
lcd_com(0x80);
}
void lcd_com(u8 c)
{
IOCLR1=lcd_d;
IOSET1=(c&0xf0)<<16;
IOCLR1=RS;
IOCLR1=RW;
IOSET1=e;
delay_ms(2);
IOCLR1=e;

IOCLR1=lcd_d;
IOSET1=(c&0x0f)<<20;
IOCLR1=RS;
IOCLR1=RW;
IOSET1=e;
delay_ms(2);
IOCLR1=e;
}
void lcd_data(u8 d)
{
IOCLR1=lcd_d;
IOSET1=(d&0xf0)<<16;
IOSET1=RS;
IOCLR1=RW;
IOSET1=e;
delay_ms(2);
IOCLR1=e;

IOCLR1=lcd_d;
IOSET1=(d&0x0f)<<20;
IOSET1=RS;
IOCLR1=RW;
IOSET1=e;
delay_ms(2);
IOCLR1=e;
}
void lcd_scroll(u8*s)
{
int i;
while(1)
{
for(i=0x8f;i>=0x80;i--)
{
lcd_com(i);
lcd_str(s);
delay_ms(500);
lcd_com(0x01);

}

}
}
void lcd_str(u8*s)
{
while(*s)
lcd_data(*s++);
}
void lcd_integer(int n)
{
unsigned char arr[5];
signed int i=0;
if(n==0)
lcd_data('0');
else
{
if(n<0)
{
lcd_data('-');
 n=-n;
}
while(n>0)
{
arr[i++]=n%10;
n=n/10;
}
for(--i;i>=0;i--)
lcd_data(arr[i]+48);
}
}



