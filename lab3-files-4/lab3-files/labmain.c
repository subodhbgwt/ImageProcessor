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

int mytime = 0x5957;
char textstring[] = "text, more text, and even more text!";

/* Below is the function that will be called when an interrupt is triggered. */
void handle_interrupt(unsigned cause) 
{}

/* Add your code here for initializing interrupts. */

void labinit(void)
{}


void set_leds(int led_mask) {
  volatile int *LED_REG = (volatile int*)0x04000000; // create a pointer to the memory address of the led registry.
  *LED_REG = led_mask & 0x3ff; // leave the 10 lsb
}

void set_displays(int display_number, int value) {
  int disp_offset = 0x04000050 + (0x10*display_number); // from zero to five (6 displays)
  volatile int *DISPLAY_REG = (volatile int*)disp_offset; // pointer to the memory address of the display.

  switch (value) { // cases for which output we want on the display. highest bit is the dot, 1 is off. 
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
  return *SWITCH_REG & 0x3ff; // return the 10 lsb
}

int get_bt(void) { // print the least significant bit that corresponds to whether the button is pressed or not
  volatile unsigned int *BUTTON_REG = (volatile unsigned int *)0x040000d0;
  return *BUTTON_REG & 1;
}

/* Your code goes into main as well as any needed functions. */
int main() {
   //ASSIGNMENT 1-D 
   /* for (int i = 0x0; i <= 0xF; i++) {
    set_leds(i); 
    delay(700);
  } */

  // ASSIGNMENT 1-E
   /* for (int i = 0; i <= 5; i++) {
    set_displays(i, 2);
  } */

   // ASSIGNMENT 1-F 
   /* while (1) {
    int sw_value = get_sw();
    print_dec(sw_value);
    delay(1000);
  } */

  // ASSIGNMENT 1-G 
  /* while (1) {
    int bt_value = get_bt();
    print_dec(bt_value);
    delay(1000);
  } */

  // ASSIGNMENT 1A Enter a forever loop
  /* while (1) {
    time2string( textstring, mytime ); // Converts mytime to string
    display_string( textstring ); //Print out the string 'textstring'
    delay( 1000 );          // Delays 1 sec (adjust this value)
    tick( &mytime );     // Ticks the clock once
  } */


  // Assignment 1H
/* int initializing_count = 0; // new implementation of the leds instead of the for loop to ensure they do it exactly every second
set_leds(0);

while (1) {
  int four_lsb = initializing_count & 0xf; // mask the 4 lsb.
  set_leds(four_lsb);

  tick(&mytime); // tick clock
  time2string(textstring, mytime);
  display_string(textstring);
  delay(1000);
  initializing_count++;

  if (four_lsb == 0xF) { // if all 4 lowest lights are on, 1111, then reenable them and start another inf loop.
    set_leds(0xf);

    int sec = 0; // create our own local timer since the time2string they gave doesnt have hours
    int min = 0;
    int hr = 0;

  while (1) {
    int switch_update = 0;
    if (get_bt()) { // get the state of the button
      int two_msb = get_sw() >> 8; // shift the msb down to the bottom of the bit.
      int six_lsb = get_sw() & 0x3f; // mask only leaving the 6 lsb.

      switch (two_msb) { // 3 cases for lsb
        case 0b01:
        sec = six_lsb;
        if (sec > 59) {
          sec = 59;
        }
        switch_update = 1;
        break;
        case 0b10:
        min = six_lsb;
        if ( min > 59) {
          min = 59;
        }
        switch_update = 1;
        break;
        case 0b11:
        hr = six_lsb;
        if (hr > 99) {
            hr = 99;
        }
        switch_update = 1; // for every case where we update it, increment switch upd.
        break;
        default:
        break;
      }
    }

        delay(1000); // a simple delay for around a second before ticking and showing to the terminal
        tick(&mytime);
        time2string(textstring, mytime);
        display_string(textstring);

        set_displays(0, sec % 10); // divide each part of the time into tens and ones, and print them respectively.
        set_displays(1, sec / 10);
        set_displays(2, min % 10);
        set_displays(3, min / 10);
        set_displays(4, hr % 10);
        set_displays(5, hr / 10);
        
        if (!switch_update) {
          sec++; // if we have not switched the time, we increment seconds. Otherwise it waits.
        }
        if (sec > 59) { // simple logic about wrapping.
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
  } */
}


