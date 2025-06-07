//#include<LPC21xx.h>
void delay_ms(unsigned ms)
{
T0PR=15000-1;
T0TCR=0x1;
while(T0TC<ms);
T0TCR=0x3;
T0TCR=0x0;
}
