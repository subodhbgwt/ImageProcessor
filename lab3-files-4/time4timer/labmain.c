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

#define LED_ADDRESS 0x04000000
#define LEDSPOINT (*(volatile unsigned int *)LED_ADDRESS)
#define DISPLAY_ADDRESS 0x04000050
#define DISPLAYOFFSET 0x10
#define SWITCH_ADDRESS 0x04000010
#define BUTTON2_ADDRESS 0x040000d0
#define TIMER_BASE 0x04000020u
#define TMR_STATUS (*(volatile unsigned int *)(TIMER_BASE + 0x00)) 
#define TMR_CONTROL (*(volatile unsigned int *)(TIMER_BASE + 0x04)) 
#define TMR_PERIODL (*(volatile unsigned int *)(TIMER_BASE + 0x08)) 
#define TMR_PERIODH (*(volatile unsigned int *)(TIMER_BASE + 0x0C)) 


int mytime = 0x5957;
char textstring[] = "text, more text, and even more text!";
volatile unsigned timeoutcount = 0;
volatile int suppress_wrap = 1;

/* Below is the function that will be called when an interrupt is triggered. */
void handle_interrupt(unsigned cause) 
{}

/* Add your code here for initializing interrupts. */

void labinit(void)
{
  const unsigned period = 3000000u - 1u;
  TMR_CONTROL = 0;          // stop/config while we program
  TMR_STATUS  = 0;          // clear any stale TO
  TMR_PERIODL = (period & 0xFFFFu);
  TMR_PERIODH = (period >> 16);
  TMR_CONTROL = (1u<<1) | (1u<<2);  // CONT | START  (ITO=0 since we poll)
}

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

static const unsigned char SEGMENTS[10] = {
  0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90 // all possible combinations of binary to make zero-nine in binary on the board leds.
};

static inline unsigned char seg_of(char c) {
  return (c>='0' && c<='9') ? SEGMENTS[c-'0'] : 0xFF; // blank if false, otherwise if character is between zero and it will reflect on the board
}

void disp_str(char *s) {
  unsigned hb = (unsigned)(mytime >> 16) & 0xFF; // seconds bcd
  set_displays(5, SEGMENTS[(hb >> 4) & 0xF]); // hours tens
  set_displays(4, SEGMENTS[hb & 0xF]); // hours ones, crappy time2string
  set_displays(3, seg_of(s[0])); 
  set_displays(2, seg_of(s[1])); 
  set_displays(1, seg_of(s[3]));
  set_displays(0, seg_of(s[4]));

}

/* Your code goes into main as well as any needed functions. */
int main() {
   //ASSIGNMENT 1-D 
   /* for (int i = 0x0; i <= 0xF; i++) {
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

  // Assignment 2-C Call labinit()
  /* labinit();

  while (1) {
  if (get_bt()) {
      int sw = get_sw();
      int sel = (sw >> 8) & 3; // move the 2 significant bits to the far right.
      int val = sw & 0x3F; // the value we want to change it to
      if (sel == 1)  {
        if (val > 59) val = 59;
          int bcd = ((val / 10) << 4) | (val % 10);
          mytime = (mytime & 0xFFFF00) | bcd; // mask the relevant bits
        } else if (sel == 2) {
          if ( val > 59) val = 59;
          int bcd = ((val / 10) << 4) | (val % 10);
          mytime = (mytime & 0xFF00FF) | (bcd << 8);   // set minutes
        } else if (sel == 3) {
          if (val > 99) val = 99;
          int hbcd = ((val/10)<<4) | (val%10); // local hour binary code. 
          mytime = (mytime & 0x00FFFF) | (hbcd << 16);  // set hours
          }
  }

    if (TMR_STATUS & 1u) {
    TMR_STATUS = 0;
    if (++timeoutcount >= 10) { // about 1 second has passed
      timeoutcount = 0;

      unsigned prevminutes = (unsigned)(mytime >> 8) & 0xFF;
      unsigned oldhour = (unsigned)(mytime >> 16) & 0xFF;

      tick(&mytime);
      unsigned newminutes = (unsigned)(mytime >> 8) & 0xFF;

      if (prevminutes == 0x59 && newminutes == 0x00) {
        if (suppress_wrap) { // handle the very first increment, ensure that we simply set it to zero.
          suppress_wrap = 0;
          mytime = (mytime & 0x00FFFF) | ((unsigned)oldhour << 16);
        } else {
          unsigned tens = (oldhour >> 4) & 0xF;
          unsigned ones = oldhour & 0xF;

          if (ones < 9){ 
              ones++;
          } else {
            ones = 0; 
            tens++;
          }
          unsigned hbcd = (tens << 4) | ones;
          mytime = (mytime & 0x00FFFF) | (hbcd << 16);
        }
      }

      time2string(textstring, mytime);
      // display_string(textstring); // print to terminal and board
      disp_str(textstring);
    }
  }
} */

  // Assignment 1-A Enter a forever loop
  /* while (1) {
    time2string( textstring, mytime ); // Converts mytime to string
    display_string( textstring ); //Print out the string 'textstring'
    delay( 700 );          // Delays 1 sec (adjust this value)
    tick( &mytime );     // Ticks the clock once
  } */

  // Assignment 1H
  while (1) {
  if (get_bt()) {
      int sw = get_sw();
      int sel = (sw >> 8) & 3; // move the 2 significant bits to the far right.
      int val = sw & 0x3F; // the value we want to change it to
      if (sel == 1)  {
        if (val > 59) val = 59;
          int bcd = ((val / 10) << 4) | (val % 10);
          mytime = (mytime & 0xFFFF00) | bcd; // mask the relevant bits
        } else if (sel == 2) {
          if ( val > 59) val = 59;
          int bcd = ((val / 10) << 4) | (val % 10);
          mytime = (mytime & 0xFF00FF) | (bcd << 8);   // set minutes
        } else if (sel == 3) {
          if (val > 99) val = 99;
          int hbcd = ((val/10)<<4) | (val%10); // local hour binary code. 
          mytime = (mytime & 0x00FFFF) | (hbcd << 16);  // set hours
          }
  }
  time2string(textstring, mytime);
  disp_str(textstring); 
  unsigned prevminutes = (unsigned)(mytime >> 8) & 0xFF;
  unsigned oldhour = (unsigned)(mytime >> 16) & 0xFF;
  delay(1000);
  tick(&mytime);
  unsigned newminutes = (unsigned)(mytime >> 8) & 0xFF;
  if (prevminutes == 0x59 && newminutes == 0x00) {
    if (suppress_wrap) {
      suppress_wrap = 0;
      // undo tick's first-hour increment
      mytime = (mytime & 0x00FFFF) | ((unsigned)oldhour << 16);
    } else {
    unsigned tens = (oldhour >> 4) & 0xF;
    unsigned ones = oldhour & 0xF;

    if (ones < 9){ 
        ones++;
    } else {
      ones = 0; 
      tens++;
    }
    unsigned hbcd = (tens << 4) | ones;
    mytime = (mytime & 0x00FFFF) | (hbcd << 16);
      }
    }
  }
}



