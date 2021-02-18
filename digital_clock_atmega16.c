#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "HD44780.h"

#define KEY1 (1 << PB0)
#define KEY2 (1 << PB1)
#define KEY3 (1 << PB2)
#define KEY4 (1 << PB3)

#define KEY1_UP (PINB & KEY1)
#define KEY2_UP (PINB & KEY2)
#define KEY3_UP (PINB & KEY3)
#define KEY4_UP (PINB & KEY4)

uint8_t modeFlag = 1;
uint8_t able_to_edit = 1;
uint8_t flag = 0;
uint8_t hours, minutes, seconds;

volatile uint8_t timer_debounce;
volatile uint16_t timer_wait;
volatile uint8_t timer_add;

char hoursBuffer[3];
char minutesBuffer[3];
char secondsBuffer[3];

void keys_pressed(void);
void io_ports_init(void);
void timers_init(void);

int main(void)
{
    io_ports_init();
    timers_init();
    LCD_Initalize();

    while(1)
    {
        if(hours >= 24)
        {
            hours = 0;
        }
        if(minutes >= 60)
        {
            minutes = 0;
        }
        if(seconds >= 60)
        {
            seconds = 0;
        }

        if(modeFlag > 3)
        {
            modeFlag = 1;
        }

        LCD_Clear();

        itoa(hours, hoursBuffer, 10);
        itoa(minutes, minutesBuffer, 10);
        itoa(seconds, secondsBuffer, 10);

        if(able_to_edit && modeFlag == 1)
        {
            LCD_GoTo(5, 0);
            LCD_WriteText("Godziny");
        }
        else if(able_to_edit && modeFlag == 2)
        {
            LCD_GoTo(5, 0);
            LCD_WriteText("Minuty");
        }
        else if(able_to_edit && modeFlag == 3)
        {
            LCD_GoTo(5, 0);
            LCD_WriteText("Sekundy");
        }
        else
        {
            LCD_GoTo(0, 0);
            LCD_WriteText("Aktualna godzina");
        }

        LCD_GoTo(4, 1);
        switch(hours)
        {
            case 24:
                LCD_WriteText("00");
                break;
            case 0:
                LCD_WriteText("00");
                break;
            case 1:
                LCD_WriteText("01");
                break;
            case 2:
                LCD_WriteText("02");
                break;
            case 3:
                LCD_WriteText("03");
                break;
            case 4:
                LCD_WriteText("04");
                break;
            case 5:
                LCD_WriteText("05");
                break;
            case 6:
                LCD_WriteText("06");
                break;
            case 7:
                LCD_WriteText("07");
                break;
            case 8:
                LCD_WriteText("08");
                break;
            case 9:
                LCD_WriteText("09");
                break;
            default:
                LCD_WriteText(hoursBuffer);
        }
        LCD_GoTo(6, 1);
        LCD_WriteText(":");
        LCD_GoTo(7, 1);
        switch(minutes)
        {
            case 0:
                LCD_WriteText("00");
                break;
            case 1:
                LCD_WriteText("01");
                break;
            case 2:
                LCD_WriteText("02");
                break;
            case 3:
                LCD_WriteText("03");
                break;
            case 4:
                LCD_WriteText("04");
                break;
            case 5:
                LCD_WriteText("05");
                break;
            case 6:
                LCD_WriteText("06");
                break;
            case 7:
                LCD_WriteText("07");
                break;
            case 8:
                LCD_WriteText("08");
                break;
            case 9:
                LCD_WriteText("09");
                break;
            default:
                LCD_WriteText(minutesBuffer);
        }
        LCD_GoTo(9, 1);
        LCD_WriteText(":");
        LCD_GoTo(10, 1);
        switch(seconds)
        {
            case 0:
                LCD_WriteText("00");
                break;
            case 1:
                LCD_WriteText("01");
                break;
            case 2:
                LCD_WriteText("02");
                break;
            case 3:
                LCD_WriteText("03");
                break;
            case 4:
                LCD_WriteText("04");
                break;
            case 5:
                LCD_WriteText("05");
                break;
            case 6:
                LCD_WriteText("06");
                break;
            case 7:
                LCD_WriteText("07");
                break;
            case 8:
                LCD_WriteText("08");
                break;
            case 9:
                LCD_WriteText("09");
                break;
            default:
                LCD_WriteText(secondsBuffer);
        }

        _delay_ms(200);
    }

    return 0;
}


void keys_pressed(void)
{
    static uint8_t lkey1 = 0;
    static uint8_t lkey2 = 0;
    static uint8_t lkey3 = 0;

    // Klawisz NR 1
    if(!KEY1_UP && !lkey1 && able_to_edit)
    {
        lkey1 = 1;
        modeFlag++;
        if(modeFlag > 3)
        {
            modeFlag = 1;
        }
    }
    else if(KEY1_UP && lkey1)  lkey1 = 0;

    // Klawisz NR 2
    if(!KEY2_UP && !lkey2)
    {
        lkey2 = 1;
        timer_wait = 200;
        flag = 1;
        if(able_to_edit)
        {
            switch(modeFlag)
            {
            case 1:
                hours++;
                if(hours >= 24)
                {
                    hours = 0;
                }
                break;
            case 2:
                minutes++;
                if(minutes >= 60)
                {
                    minutes = 0;
                }
                break;
            case 3:
                seconds++;
                if(seconds >= 60)
                {
                    seconds = 0;
                }
                break;
            }
        }
    }
    else if(KEY2_UP && lkey2)
    {
        lkey2 = 0;
        flag = 0;
        timer_wait = 0;
    }
    else if(flag && !timer_wait && able_to_edit)
    {
        if(timer_add >= 10)
        {
            timer_add = 0;
            switch(modeFlag)
            {
            case 1:
                hours++;
                if(hours >= 24)
                {
                    hours = 0;
                }
                break;
            case 2:
                minutes++;
                if(minutes >= 60)
                {
                    minutes = 0;
                }
                break;
            case 3:
                seconds++;
                if(seconds >= 60)
                {
                    seconds = 0;
                }
                break;
            }
        }
    }

    // Klawisz NR 3
    if(!KEY3_UP && !lkey3)
    {
        lkey3 = 1;
        if(able_to_edit)
        {
            able_to_edit = 0;
            modeFlag = 1;
            TCCR1B |= (1 << CS12) | (1 << CS10);
        }
        else
        {
            able_to_edit = 1;
            TCCR1B &= ~(1 << CS12);
            TCCR1B &= ~(1 << CS10);
        }

    }
    else if(KEY3_UP && lkey3) lkey3 = 0;

}

void io_ports_init(void)
{
    DDRB &= ~KEY1;
    DDRB &= ~KEY2;
    DDRB &= ~KEY3;
    DDRB &= ~KEY4;

    PORTB |= KEY1 | KEY2 | KEY3 | KEY4;
}

void timers_init(void)
{
    // Timer1
    TCCR1B |= (1 << WGM12);
    OCR1A = 15625;
	TIMSK |= (1 << OCIE1A);

	// Timer0
	TCCR0 |= (1 << WGM01) | (1 << CS02) | (1 << CS00);
	OCR0 = 156;
	TIMSK |= (1 << OCIE0);

	sei();
}

ISR(TIMER1_COMPA_vect)
{
    seconds++;
	if(seconds >= 60)
	{
		seconds = 0;
		minutes ++;
		if(minutes >= 60)
		{
			seconds = 0;
			minutes = 0;
			hours ++;
			if(hours >= 24)
			{
				hours = minutes = seconds = 0;
			}
		}
	}
}

ISR(TIMER0_COMP_vect)
{
    timer_debounce++;
	if(timer_wait) timer_wait --;
	timer_add++;
	if(timer_debounce >= 6)
    {
        keys_pressed();
        timer_debounce = 0;
    }
}
