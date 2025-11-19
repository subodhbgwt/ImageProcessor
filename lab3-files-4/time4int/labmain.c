/* main.c

   This file written 2024 by Artur Podobas and Pedro Antunes

   For copyright and licensing, see file COPYING */

/* Below functions are external and found in other files. */
extern void print(const char*);
extern void print_dec(unsigned int);
extern void display_string(char*);
extern void time2string(char*,int);
extern void tick(int*);
extern void delay(int);
extern int nextprime( int );
extern void enable_interrupt(void);
void set_displays(int display_number, int value); // here we need to make a template of our function so it doesn't complain.

#define TIMER_BASE 0x04000020
volatile unsigned int *TMR_STATUS = ((volatile unsigned int *)(TIMER_BASE + 0x00));
volatile unsigned int *TMR_CONTROL = ((volatile unsigned int *)(TIMER_BASE + 0x04)); 
volatile unsigned int *TMR_PERIODL = ((volatile unsigned int *)(TIMER_BASE + 0x08));
volatile unsigned int *TMR_PERIODH = ((volatile unsigned int *)(TIMER_BASE + 0x0C));

// global instantiation of time because we don't need to use display
volatile int sec = 0; 
volatile int min = 0;
volatile int hr = 0;

int prime = 1234567;
int mytime = 0x5957;
char textstring[] = "text, more text, and even more text!";
int timeoutcount = 0;

/* Below is the function that will be called when an interrupt is triggered. */
void handle_interrupt(unsigned cause) { // now our interrupt directly handles ticking the time AND setting the displays along with the wrapping

  if (cause == 16) { // the cause looks for 16 because in mcause in our boot, we set it to 16 whenever an interrupt takes place. 
      *TMR_STATUS = 0; // same resetting of the timer status as before
      timeoutcount++; // increment the timeoutcount everytime we experience an interruption with cause = 16

      if (timeoutcount >= 10) { // when its done 10 times, 1 second has passed, i.e tick the time and set the displays.
        timeoutcount = 0;

        tick(&mytime);

        set_displays(0, sec % 10);
        set_displays(1, sec / 10);
        set_displays(2, min % 10);
        set_displays(3, min / 10);
        set_displays(4, hr % 10);
        set_displays(5, hr / 10);

        sec++;
        if (sec > 59) { 
          sec = 0; 
          min++;
        }
        if (min > 59) { 
          min = 0; 
          hr++;
        }
        if (hr > 99) { 
          hr = 0; 
        }
      }
    }
  }

/* Add your code here for initializing interrupts. */

void labinit(void)
{
  *TMR_STATUS = 0; // same as before
  *TMR_PERIODL = 0xC6BF; // low and high period 16 bits, same as before
  *TMR_PERIODH = 0x002D;
  *TMR_CONTROL = 0x7; // now 111 in binary, so we are also enabling bit 0, i.e for interrupts.
  enable_interrupt(); // now we are actively calling on the enable interrupt which does what its called in boot.S. Calls handle interrupt. When interrupt called 10 times we get the increment in time and timeoutcount back down.
}

void set_leds(int led_mask) {
volatile int *LED_REG = (volatile int*)0x04000000;
*LED_REG = led_mask & 0x3ff;
}

void set_displays(int display_number, int value) {
  int disp_offset = 0x04000050 + (0x10*display_number);
  volatile int *DISPLAY_REG = (volatile int*)disp_offset;

  switch (value) {
    case 0:
    *DISPLAY_REG = 0b11000000;
    break;
    case 1:
    *DISPLAY_REG = 0b11111001;
    break;
    case 2:
    *DISPLAY_REG = 0b10100100;
    break;
    case 3:
    *DISPLAY_REG = 0b10110000;
    break;
    case 4:
    *DISPLAY_REG = 0b10011001;
    break;
    case 5:
    *DISPLAY_REG = 0b10010010;
    break;
    case 6:
    *DISPLAY_REG = 0b10000010;
    break;
    case 7:
    *DISPLAY_REG = 0b11111000;
    break;
    case 8:
    *DISPLAY_REG = 0b10000000;
    break;
    case 9:
    *DISPLAY_REG = 0b10010000;
    break;
    default:
    *DISPLAY_REG = 0b00000000;
    break;
  }
}

int get_sw(void) { // print values corresponding to which switch is on in decimal form. 
  volatile unsigned int *SWITCH_REG = (volatile unsigned int *)0x04000010;
  return *SWITCH_REG & 0x3ff;
}

int get_bt(void) { // print the least significant bit that corresponds to whether the button is pressed or not
  volatile unsigned int *BUTTON_REG = (volatile unsigned int *)0x040000d0;
  return *BUTTON_REG & 1;
}

/* Your code goes into main as well as any needed functions. */
int main() {
  // Assignment 3C
   labinit(); // our labinit handles all the hard work now and looks for interrupts. 10 interrupts and we tick.
   while (1) {
    print("Prime: ");
    prime = nextprime(prime);
    print_dec(prime);
    print("\n");
   }
  } 


