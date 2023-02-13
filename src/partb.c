////
//// Created by Jun Kim on 2/10/23.
////
//#include <avr/io.h>
//#include <util/delay.h>
//#include <stdio.h>
//#include <avr/interrupt.h>
//
//// Timer/Counter1 capture event
//ISR(TIMER1_CAPT_vect) {
//
//    // clearing the input capture flag in TIFR1 by setting it to 1
//    TIFR1 |= (1<<ICF1);
//
//    // toggle LED
//    PORTB ^= (1<<PORTB5); // on
//
//    // toggle looking for falling/rising edge
//    TCCR1B ^= (1<<ICES1);
//}
//
//void Initialize() {
//    // disable global interrupts;
//    cli();
//
//    // clear input capture flag
//    TIFR1 |= (1<<ICF1);
//    // set PB5 to be output and pull high
//    DDRB |= (1<<DDB5);
//    PORTB &= ~(1<<PORTB5); // output off
//
//    // set PB0 (ICP1) to be input
//    DDRB &= ~(1<<DDB0);
//
//    // Timer1 set up
//    // Set Timer 1 clock to be internal div by 8
//    // 2MHz timer clock, 1 tick = (1/2M) second
//
//    // "set appropriate edge control bits in the TCCR1B register
//    TCCR1B |= (1<<CS10);
//    TCCR1B &= ~(1<<CS11);
//    TCCR1B &= ~(1<<CS12);
//
//    // "enable the clock using the TCCR1B register"
//    TCCR1B &= ~(1<<WGM12);
//    TCCR1B &= ~(1<<WGM13);
//
//    // Look for rising edge
//    TCCR1B |= (1<<ICES1);
//
//    // Enable input capture interrupt
//    // "Enable the interrupt in the TIMSK1 register"
//    TIMSK1 |= (1<<ICIE1);
//
//    // enable global interrupts
//    sei();
//}
//
//int main(void) {
//    Initialize();
//
//    // TCNT1 register overflows approximately every 4ms
//    //  a timer overflow interrupt or use of the prescaler in TCCR1B
//    //  is necessary to keep track of the more significant bits of time
//    //
//    while(1) {
//
//    }
//}
