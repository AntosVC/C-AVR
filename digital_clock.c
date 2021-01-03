/*
     Digital clock
*/

#include <avr/io.h>
#include <avr/interrupt.h>

#include "HD44780.h"

#include <stdio.h>
#include <stdlib.h>

#define F_CPU 1000000UL

#define KEY1_UP (PIND & (1 << PD2))
#define KEY2_UP (PIND & (1 << PD3))
#define KEY3_UP (PINB & (1 << PB6))
#define KEY4_UP (PINB & (1 << PB7))

#define timer0_start 196
#define timer2_start 6

uint8_t modeFlag = 1;
uint8_t ableToEdit = 1;
volatile uint8_t seconds = 0;
volatile uint8_t minutes = 0;
volatile uint8_t hours = 0;
char secondsBuffer[3];
char minutesBuffer[3];
char hoursBuffer[3];
volatile uint8_t key1_Status;
int real_time_sec;

ISR(TIMER0_OVF_vect)
{
    TCNT0 = timer0_start;
    static uint8_t lkey1 = 0;
    static uint8_t lkey2 = 0;
    static uint8_t lkey3 = 0;
    static uint8_t lkey4 = 0;

    if(!KEY1_UP)
    {
        if(!lkey1)
        {
            lkey1 = 1;
            key1_Status = lkey1;
            if(ableToEdit == 1)
            {
                if(modeFlag > 3)
                {
                    modeFlag = 1;
                }
                modeFlag++;
            }
        }
    }
    else
    {
        if(lkey1)
        {
            lkey1 = 0;
            key1_Status = lkey1;
        }
    }

    if(!KEY2_UP)
    {
        if(!lkey2)
        {
            lkey2 = 1;
            if(ableToEdit == 1)
            {
				const int clicked_time = real_time_sec;
				
                switch(modeFlag)
                {
                case 1:
                    hours ++;
                    if(hours >= 24)
                    {
                        hours = 0;
                    }
                    break;
                case 2:
                    minutes ++;
                    if(minutes >= 60)
                    {
                        minutes = 0;
                    }
                    break;
                case 3:
                    seconds ++;
                    if(seconds >= 60)
                    {
                        seconds == 0;
                    }
                    break;
                }
                while(!KEY2_UP && (real_time_sec - clicked_time) >= 3)
                {
                    switch(modeFlag)
                    {
                    case 1:
                        hours ++;
                        if(hours >= 24)
                        {
                            hours = 0;
                        }
                        break;
                    case 2:
                        minutes ++;
                        if(minutes >= 60)
                        {
                            minutes = 0;
                        }
                        break;
                    case 3:
                        seconds ++;
                        if(seconds >= 60)
                        {
                            seconds == 0;
                        }
                        break;
                    }
                }
            }
        }
    }
    else
    {
        if(lkey2)
        {
            lkey2 = 0;
        }
    }

    if(!KEY3_UP)
    {
        if(!lkey3)
        {
            lkey3 = 1;
            ableToEdit = 1;
            modeFlag = 1;
            TCCR1B &= ~(1 << CS12);
            TCCR1B &= ~(1 << CS10);
        }
    }
    else
    {
        if(lkey3)
        {
            lkey3 = 0;
        }
    }

    if(!KEY4_UP)
    {
        if(!lkey4)
        {
            lkey4 = 1;
            ableToEdit = 0;
            modeFlag = 1;
            TCCR1B |= (1 << CS12) | (1 << CS10);
        }
    }
    else
    {
        if(lkey4)
        {
            lkey4 = 0;
        }

    }
}

ISR(TIMER1_COMPA_vect)
{
    real_time_sec++;
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

int main(void)
{
    DDRD &= ~(1 << PD2);
    DDRD &= ~(1 << PD3);
    DDRB &= ~(1 << PB6);
    DDRB &= ~(1 << PB7);
    DDRB |= (1 << PB0);

    PORTD |= (1 << PD2);
    PORTD |= (1 << PD3);
    PORTB |= (1 << PB6);
    PORTB |= (1 << PB7);

    _delay_ms(10);

    // LCD init
    LCD_Initalize();

    // Timer1 init
    TCCR1B |= (1 << WGM12);
    OCR1A = 977;
    TIMSK |= (1 << OCIE1A);

    TCNT0 = timer0_start;
    TIMSK |= (1 << TOIE0);
    TCCR0 |= (1 << CS02) | (1 << CS00);

    sei();

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

        itoa(seconds, secondsBuffer, 10);
        itoa(minutes, minutesBuffer, 10);
        itoa(hours, hoursBuffer, 10);

        LCD_Clear();
        LCD_GoTo(0, 0);
        if(ableToEdit == 1 && modeFlag == 1)
        {
            LCD_GoTo(4, 0);
            LCD_WriteText("Godziny");
        }
        else if(ableToEdit == 1 && modeFlag == 2)
        {
            LCD_GoTo(5, 0);
            LCD_WriteText("Minuty");
        }
        else if(ableToEdit == 1 && modeFlag == 3)
        {
            LCD_GoTo(4, 0);
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
