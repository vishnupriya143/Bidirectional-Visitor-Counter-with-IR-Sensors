#include <LPC21XX.H>
#include "delay.h"
#include "4bitcode.h"
#include "i2c_eeprom.h"

#define SW1_PIN 14 //insted of IR sensors used switch
#define SW2_PIN 15
#define LED_PIN (1 << 17)

#define BCD_TO_DEC(x) (((x >> 4) * 10) + (x & 0x0F))

void rtc_display(void);
void update_display(int ci, int co);
void control_led(int total);
int debounce_switch(int pin);
void read_counts(int *ci, int *co);
void write_counts(int ci, int co);

int main() {
    int ci, co, total;

    init_i2c();
    lcd_config();

    // Set directions
    IODIR0 |= LED_PIN;           // LED as output
    IODIR0 &= ~(1 << SW1_PIN);   // SW1 as input
    IODIR0 &= ~(1 << SW2_PIN);   // SW2 as input

    IOSET0 |= LED_PIN;  // Turn off LED (active-low)

    // Welcome screen
    lcd_com(0x80);
    lcd_str("Bidirectional");
    lcd_com(0xC0);
    lcd_str("Visitor Counter");
    delay_ms(1000);
    lcd_com(0x01);

    // Read initial counts
    read_counts(&ci, &co);
    total = ci - co;

    update_display(ci, co);
    control_led(total);

    while (1) {
        if (debounce_switch(SW1_PIN)) {
            read_counts(&ci, &co);
            ci++;
            write_counts(ci, co);
            total = ci - co;
            update_display(ci, co);
            control_led(total);
        }

        if (debounce_switch(SW2_PIN)) {
            read_counts(&ci, &co);
            co++;
            write_counts(ci, co);
            total = ci - co;
            update_display(ci, co);
            control_led(total);
        }

        rtc_display();
    }
}

// Read ci and co from EEPROM
void read_counts(int *ci, int *co) {
    *ci = i2c_eeprom_read(0x50, 0x00);
    *co = i2c_eeprom_read(0x50, 0x01);
}

// Write ci and co to EEPROM
void write_counts(int ci, int co) {
    i2c_eeprom_write(0x50, 0x00, ci);
    delay_ms(10);
    i2c_eeprom_write(0x50, 0x01, co);
    delay_ms(10);
}

void update_display(int ci, int co) {
    int total = ci - co;

    lcd_com(0x01);           // Clear display
    delay_ms(2);

    lcd_com(0x80);           // First row
    lcd_str("IN:");
    lcd_integer(ci);
    lcd_com(0x87);
    lcd_str("OUT:");
    lcd_integer(co);

    lcd_com(0xC0);           // Second row
    lcd_str("TOT:");
    lcd_integer(total);
}

void control_led(int total) {
    if (total > 0)
        IOCLR0 = LED_PIN;  // Turn on LED
    else
        IOSET0 = LED_PIN;  // Turn off LED
}

int debounce_switch(int pin) {
    if (((IOPIN0 >> pin) & 1) == 0) {
        delay_ms(20);
        if (((IOPIN0 >> pin) & 1) == 0) {
            while (((IOPIN0 >> pin) & 1) == 0);  // Wait for release
            return 1;
        }
    }
    return 0;
}

void rtc_display(void) {
    unsigned char h, m, s, hr;
    int ci = 0, co = 0, total = 0;

    hr = h = i2c_eeprom_read(0x68, 0x02);
    delay_ms(50);
    m = i2c_eeprom_read(0x68, 0x01);
    delay_ms(50);
    s = i2c_eeprom_read(0x68, 0x00);
    delay_ms(50);

    h = BCD_TO_DEC(h);
    m = BCD_TO_DEC(m);
    s = BCD_TO_DEC(s);

    lcd_com(0xC7);  // Safe position on second line
    if (h < 10) lcd_data('0');
    lcd_integer(h);
    lcd_data(':');
    if (m < 10) lcd_data('0');
    lcd_integer(m);
    lcd_data(':');
    if (s < 10) lcd_data('0');
    lcd_integer(s);

    if (((hr >> 5) & 1) == 1)
        lcd_str("PM");
    else
        lcd_str("AM");

    // Reset counts every 2 minutes at second 51
    if ((m % 2 == 0) && (s == 51)) {
        write_counts(0, 0);
        update_display(0, 0);
        control_led(0);
    }
}
