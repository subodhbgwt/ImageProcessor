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

#define LED_ADDRESS 0x04000000
#define LEDSPOINT (*(volatile unsigned int *)LED_ADDRESS)
#define DISPLAY_ADDRESS 0x04000050
#define DISPLAYOFFSET 0x10
#define SWITCH_ADDRESS 0x04000010
#define BUTTON2_ADDRESS 0x040000d0

void set_leds(int led_mask) {
  led_mask &= 0x3FF; // mask the 10 lsb
  LEDSPOINT = led_mask;
}

void set_displays(int display_number, int value) {
  volatile unsigned int *disp = (volatile unsigned int *)(DISPLAY_ADDRESS + display_number * DISPLAYOFFSET);
  *disp = value;
}

int get_sw(void) { // print values corresponding to which switch is on in decimal form. 
  volatile unsigned int *sw = (volatile unsigned int *)(SWITCH_ADDRESS);
  int value = *sw & 0x3FF; // dereferencing switch and masking it
  return value;
}

int get_bt(void) { // print the least significant bit that corresponds to whether the button is pressed or not
  volatile unsigned int *btn = (volatile unsigned int *)(BUTTON2_ADDRESS);
  int value = *btn & 0x1; // dereferencing button and masking it to only the lsb
  return value;
}


/* Your code goes into main as well as any needed functions. */
int main() {
  /* ASSIGNMENT 1-D for (int i = 0x0; i <= 0xF; i++) {
    set_leds(i); 
    delay(700);
  } */

  /* for (int i = 0; i <= 5; i++) {
    set_displays(i, 0x2); // 0xFF turns all off, 0x00 turns all on. Reverse logic, zeroes enable them. 
  } */

   /* ASSIGNMENT 1-F while (1) {
    int sw_value = get_sw();
    print_dec(sw_value);
    delay(1000);
  } */

  /* ASSIGNMENT 1-G while (1) {
    int bt_value = get_bt();
    print_dec(bt_value);
    delay(1000);
  } */ 

  // Call labinit()
  // labinit();

  // Enter a forever loop
  /*while (1) {
    time2string( textstring, mytime ); // Converts mytime to string
    display_string( textstring ); //Print out the string 'textstring'
    delay( 700 );          // Delays 1 sec (adjust this value)
    tick( &mytime );     // Ticks the clock once
  } */

  while (1) {
    // allow button to change seconds/minutes here if you want
    if (get_bt()) {
        int sw = get_sw();
        int sel = (sw >> 8) & 3;
        int val = sw & 0x3F;
        int bcd = ((val / 10) << 4) | (val % 10); /// make val binary converted decimal so it doesnt print A-F.
        if (sel == 1)
            mytime = (mytime & 0xFF00) | bcd;          // set seconds
        else if (sel == 2)
            mytime = (mytime & 0x00FF) | (bcd << 8);   // set minutes
    }

    time2string(textstring, mytime);  
    display_string(textstring);   
    delay(1000);
    tick(&mytime);
}

}


