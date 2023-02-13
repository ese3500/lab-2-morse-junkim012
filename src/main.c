#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "uart.h"
#include <string.h>

//#define F_CPU 16000000UL
#define BAUD_RATE 9600
#define BAUD_PRESCALER (((F_CPU / (BAUD_RATE * 16UL))) - 1)

#define total_ticks 65536
volatile int overflow_space_counter = 0;
char overflow_space_counter_string[25];
volatile int overflow_counter = 0;
char overflow_counter_string[25];

int prev_end = 0;
int new_start = 0;
volatile int space = 0;
volatile int start = 0;
volatile int end = 0;

volatile int print = 0;
volatile int print_space = 0;

char print_string[25];
char string[25];

char space_end_string[25];
char space_start_string[25];
char space_string[25];
char start_string[25];
char end_string[25];
char ms_string[25];
char code_string[25];
char decoded_string[25];

volatile int pressed = 0;

char code[25] = "";

void append(char* s, char c) {
    int len = strlen(s); // counts until it encounters the null character
    s[len] = c;
    s[len+1] = '\0';
}

void reset(char* code) {
    // reset code
    int i=0;
    for(i=0;i<strlen(code);i++)
    {
        code[i] = NULL;
    }
}

char decode(char* code) {
    if(!strcmp(code, ".\0")){
        return '.';
    } else if (!strcmp(code, ".---\0")) {
        return 'J';
    } else if (!strcmp(code, "..-\0")) {
        return 'U';
    } else if (!strcmp(code, "-.\0")) {
        return 'N';
    } else if (!strcmp(code, "-.\0")) {
        return 'A';
    } else if (!strcmp(code, "-...\0")) {
        return 'B';
    } else if (!strcmp(code, "-.-.\0")) {
        return 'C';
    } else if (!strcmp(code, "-..\0")) {
        return 'D';
    } else if (!strcmp(code, ".\0")) {
        return 'E';
    } else if (!strcmp(code, "..-.\0")) {
        return 'F';
    } else if (!strcmp(code, "--.\0")) {
        return 'G';
    } else if (!strcmp(code, "....\0")) {
        return 'H';
    } else if (!strcmp(code, "..\0")) {
        return 'I';
    } else if (!strcmp(code, ".---\0")) {
        return 'J';
    } else if (!strcmp(code, "-.-\0")) {
        return 'K';
    } else if (!strcmp(code, ".-..\0")) {
        return 'L';
    } else if (!strcmp(code, "--\0")) {
        return 'M';
    } else if (!strcmp(code, "-.\0")) {
        return 'N';
    } else if (!strcmp(code, "---\0")) {
        return 'O';
    } else if (!strcmp(code, ".--.\0")) {
        return 'P';
    } else if (!strcmp(code, "--.-\0")) {
        return 'Q';
    } else if (!strcmp(code, ".-.\0")) {
        return 'R';
    } else if (!strcmp(code, "...\0")) {
        return 'S';
    } else if (!strcmp(code, "-\0")) {
        return 'T';
    } else if (!strcmp(code, "..-\0")) {
        return 'U';
    } else if (!strcmp(code, "...-\0")) {
        return 'V';
    } else if (!strcmp(code, "-..-\0")) {
        return 'W';
    } else if (!strcmp(code, "-..-\0")) {
        return 'X';
    } else if (!strcmp(code, "-.--\0")) {
        return 'Y';
    } else if (!strcmp(code, "--..\0")) {
        return 'Z';
    } else if (!strcmp(code, ".----\0")) {
        return '1';
    } else if (!strcmp(code, "..---\0")) {
        return '2';
    } else if (!strcmp(code, "...--\0")) {
        return '3';
    } else if (!strcmp(code, "....-\0")) {
        return '4';
    } else if (!strcmp(code, ".....\0")) {
        return '5';
    } else if (!strcmp(code, "-....\0")) {
        return '6';
    } else if (!strcmp(code, "--...\0")) {
        return '7';
    } else if (!strcmp(code, "---..\0")) {
        return '8';
    } else if (!strcmp(code, "----.\0")) {
        return '9';
    } else if (!strcmp(code, "-----\0")) {
        return '0';
    }
}

// whenever timer overflows
ISR(TIMER1_OVF_vect) {
//    if (pressed) {
        overflow_counter = overflow_counter + 1;
//    }
    if(!pressed) {
        overflow_space_counter = overflow_space_counter + 1;
    }
}

// Timer/Counter1 capture event
ISR(TIMER1_CAPT_vect) {
    // toggle LED
    PORTB ^= (1<<PORTB5); // on

    if (!pressed) {

        // reset overflow counter
        overflow_counter = 0;

        start = TCNT1;
        pressed = 1;
        print_space = 1;
    } else {
        end = TCNT1;

        pressed = 0;
        print = 1;

        // reset space counter
        overflow_space_counter = 0;
        // reset the clock ?
//        TCNT1 = 0;
    }

    // toggle looking for falling/rising edge
    TCCR1B ^= (1<<ICES1);
    // clearing the input capture flag in TIFR1 by setting it to 1
    TIFR1 |= (1<<ICF1);

}

void Initialize() {
    // disable global interrupts;
    cli();

    DDRB = 0b00001111;
    PORTB = 0b00000010;

    // set PB5 to be output and pull high
    DDRB |= (1<<DDB5);
    PORTB &= ~(1<<PORTB5); // output off

    DDRB |= (1<<DDB1);
    DDRB |= (1<<DDB2);
    PORTB &= ~(1<<PORTB1);
    PORTB &= ~(1<<PORTB2);

    // set PB0 (ICP1) to be input
    DDRB &= ~(1<<DDB0);

    // Timer1 set up
    // Set Timer 1 clock to be internal div by 8
    // 2MHz timer clock, 1 tick = (1/2M) second

    // "set appropriate edge control bits in the TCCR1B register
    // 1 0 0=> clk/256 overflow approx every second
    TCCR1B &= ~(1<<CS10);
    TCCR1B &= ~(1<<CS11);
    TCCR1B |= (1<<CS12);

    // "enable the clock using the TCCR1B register"
    TCCR1A &= ~(1<<WGM10);
    TCCR1A &= ~(1<<WGM11);
    TCCR1B &= ~(1<<WGM12);
    TCCR1B &= ~(1<<WGM13);

    // Look for rising edge
    TCCR1B |= (1<<ICES1);

    // Enable input capture interrupt
    // "Enable the interrupt in the TIMSK1 register"
    TIMSK1 |= (1<<ICIE1);

    // enable timer overflow interrupt
    TIMSK1 |= (1<<TOIE1);   // enable timer overflow interrupt


    // clear input capture flag
    TIFR1 |= (1<<ICF1);

    // enable global interrupts
    sei();
}

int main(void) {
    Initialize();


    UART_init(BAUD_PRESCALER);

    while(1) {
        // 16MHz clock, how many ticks are in 30ms, 200ms, 400ms?
        // 16,000,000 ticks / second
        // 16,000,000 / 1024 = 15,625 ticks/second
        // 400 ms => ? ticks => does not overflow
        // 30ms -> 480,000 ticks
        // ms = ticks / (ticks / ms) = ticks / 15.625
        // OVERFLOW CALCULATION IS WRONG not working properly should not be getting negatives

        if (print_space) {
            if (overflow_space_counter > 4) {
                UART_putstring("NEW CHARACTER\n");
                char decoded = decode(code);
                sprintf(decoded_string, "DECODED CHAR: %c\n", decoded);
                UART_putstring(decoded_string);
                reset(code);
            }
//            space = (start - end);
//            if (space < 0) {
//                space = space * -1;
//            }
//            float space_ms = space / 15.625;
//            sprintf(space_end_string, "space prev end: %d\n", end);
//            sprintf(space_start_string, "space new start: %d\n", start);
//            sprintf(space_string, "space ms: %d\n", space);
//
//            UART_putstring(space_end_string);
//            UART_putstring(space_start_string);
//            UART_putstring(space_string);
////            if (space_ms > 3000) {
////                UART_putstring(space_string);
////            }
//            if (space > 20000) {
//                UART_putstring("NEW CHARACTER\n");
//                char decoded = decode(code);
//                sprintf(decoded_string, "DECODED CHAR: %c\n", decoded);
//                UART_putstring(decoded_string);
//                reset(code);
//            }
            print_space = 0;
        }

        if (print) {

            sprintf(overflow_counter_string, "overflow_counter: %d\n", overflow_counter);
            UART_putstring(overflow_counter_string);
            if (overflow_counter >= 0 && overflow_counter <= 3) {
                // dot
                append(code, '.');
                sprintf(string, "DOT\n");
                PORTB |= (1 << PORTB1); // dot
                _delay_ms(50);
                PORTB &= ~(1 << PORTB1);
            } else {
//                if ( overflow_counter > 3) {
                append(code, '-');
                sprintf(string, "DASH\n");
                PORTB |= (1 << PORTB2);
                _delay_ms(50);
                PORTB &= ~(1 << PORTB2);
            }


//            int ticks = end - start;
//            if (ticks < 0) {
//                ticks = ticks * -1;
//            }
//            float ms = ticks / 15.625;
//
//            if (ms >= 1 && ms <= 500) {
//                // dot
//                append(code, '.');
//                sprintf(string, "DOT\n");
//            } else if (ms > 500) {
//                // dash
//                sprintf(string, "DASH\n");
//                append(code, '-');
//            }

            sprintf(code_string, "MORSE: %s", code);
            UART_putstring(code_string);
            UART_putstring("\n");
            _delay_ms(100);

            sprintf(print_string, "%d", print);
            UART_putstring("boolean: ");
            _delay_ms(100);
            UART_putstring(print_string);
            _delay_ms(100);
            UART_putstring("\n");
            _delay_ms(100);

//            sprintf(ms_string, "ms: %d\n", ms);
//            sprintf(start_string, "start: %d\n", start);
//            sprintf(end_string, "end: %d\n", end);

            UART_putstring("test: ");
            _delay_ms(100);
            UART_putstring(string);

            _delay_ms(100);
//            UART_putstring(start_string);
//            _delay_ms(100);
//            UART_putstring(end_string);
//            _delay_ms(100);
//            UART_putstring(ms_string);
//            _delay_ms(100);
//            UART_putstring("\n\n");
//            _delay_ms(100);
            print = 0;
        }

        // led if dot
        // led if dash

    }
}

// part 1
//int main(void) {
//
//    // make PB1, PB2, PB3, PB4 output pins, and pull them all high
//    // set PB1 and PB3 high on PORTB
//    // Port B maps to digital pins 8 to 13, PortB 6,7 maps to crystal pins and are not usable
//    DDRB = 0b00001111;
//    PORTB = 0b00000010;
//
//    // set PD7 (Digital Pin 7) as an input pin
//    DDRD &= ~(1<<DDD7);
//    int i = 0;
//    bool pressed = tru
//    PORTB |= (1 << PORTB0);
//    while(1) {
//        // if button is pressed
//        if (PIND & (1<<PIND7)) {
//            if (i == 0) {
//                PORTB &= ~(1 << PORTB1);
//                PORTB |= (1 << PORTB2); // on
//                PORTB &= ~(1 << PORTB3);
//                PORTB &= ~(1 << PORTB4);
//                i = i + 1;
//                _delay_ms(2000);
//            }
//            else if (i == 1) {
//                PORTB &= ~(1 << PORTB1);
//                PORTB &= ~(1 << PORTB2);
//                PORTB |= (1 << PORTB3); // on
//                PORTB &= ~(1 << PORTB4);
//                i = i + 1;
//                _delay_ms(2000);
//
//            }
//            else if (i == 2) {
//                PORTB &= ~(1 << PORTB1);
//                PORTB &= ~(1 << PORTB2);
//                PORTB &= ~(1 << PORTB3);
//                PORTB |= (1 << PORTB4); // on
//                i += 1;
//                _delay_ms(2000);
//
//            }
//            else if (i == 3) {
//                PORTB |= (1 << PORTB1);
//                PORTB &= ~(1 << PORTB2); // on
//                PORTB &= ~(1 << PORTB3);
//                PORTB &= ~(1 << PORTB4);
//                i = 0;
//                _delay_ms(2000);
//            }
//        }
//    }
//}