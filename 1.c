#include <avr/io.h>
#include <avr/interrupt.h>


// port C configuration
#define RS              PC0
#define RW              PC1
#define E               PC2
#define BTN_UP          PC3
#define BTN_DOWN        PC4
#define BTN_SWTCH       PC5

// some constant events codes
#define EVENT_BTN_UP    (1 << BTN_UP)
#define EVENT_BTN_DOWN  (1 << BTN_DOWN)
#define EVENT_BTN_SWTCH (1 << BTN_SWTCH)
#define BTN_UP_CHCK     ~PINC & EVENT_BTN_UP
#define BTN_DOWN_CHCK   ~PINC & EVENT_BTN_DOWN
#define BTN_SWTCH_CHCK  ~PINC & EVENT_BTN_SWTCH

// timer configuration
#define F_CPU           1000000
#define MS              1000
#define CONF_TIME1      (1<<WGM12) | (1<<CS12)
#define KOD_TIME1	      (F_CPU / 256) / MS
#define LOW_KOD_TIME1   KOD_TIME1
#define HIGH_KOD_TIME1  KOD_TIME1 >> 8

// lcd configuration
#define NUMBERS         2
#define LETTERS         24
#define LCD_BUFFER      NUMBERS + LETTERS
#define HIGH_CAPTION    " mode: fast  \0   delay: 00"
#define MIDDLE_CAPTION  " mode: middle\0   delay: 00"
#define LOW_CAPTION     " mode: low   \0   delay: 00"
#define LCD_DELAY       3

// sinus configuration
#define SIZE_LOW        512
#define SIZE_MIDDLE     256
#define SIZE_HIGH       64
#define MAX_SIN_DELAY   5          

unsigned const char sin_tab_low[SIZE_LOW] = {127, 128, 130, 131, 133, 134, 136, 
    137, 139, 141, 142, 144, 145, 147, 148, 150, 151, 153, 154, 156, 157, 159, 
    160, 162, 163, 165, 166, 168, 169, 171, 172, 174, 175, 177, 178, 179, 181, 
    182, 184, 185, 186, 188, 189, 191, 192, 193, 195, 196, 197, 198, 200, 201, 
    202, 204, 205, 206, 207, 208, 210, 211, 212, 213, 214, 215, 216, 218, 219, 
    220, 221, 222, 223, 224, 225, 226, 227, 228, 229, 230, 230, 231, 232, 233, 
    234, 235, 236, 236, 237, 238, 239, 239, 240, 241, 241, 242, 243, 243, 244, 
    245, 245, 246, 246, 247, 247, 248, 248, 249, 249, 249, 250, 250, 250, 251, 
    251, 251, 252, 252, 252, 252, 253, 253, 253, 253, 253, 253, 253, 253, 253, 
    253, 253, 253, 253, 253, 253, 253, 253, 253, 253, 253, 252, 252, 252, 252, 
    252, 251, 251, 251, 250, 250, 250, 249, 249, 248, 248, 247, 247, 246, 246, 
    245, 245, 244, 244, 243, 242, 242, 241, 240, 240, 239, 238, 238, 237, 236, 
    235, 234, 234, 233, 232, 231, 230, 229, 228, 227, 226, 225, 224, 223, 222, 
    221, 220, 219, 218, 217, 216, 215, 214, 212, 211, 210, 209, 208, 207, 205, 
    204, 203, 202, 200, 199, 198, 197, 195, 194, 193, 191, 190, 189, 187, 186, 
    184, 183, 182, 180, 179, 177, 176, 174, 173, 172, 170, 169, 167, 166, 164, 
    163, 161, 160, 158, 157, 155, 154, 152, 151, 149, 147, 146, 144, 143, 141, 
    140, 138, 137, 135, 134, 132, 130, 129, 127, 126, 124, 123, 121, 119, 118, 
    116, 115, 113, 112, 110, 109, 107, 106, 104, 102, 101, 99, 98, 96, 95, 93, 
    92, 90, 89, 87, 86, 84, 83, 81, 80, 79, 77, 76, 74, 73, 71, 70, 69, 67, 66,
    64, 63, 62, 60, 59, 58, 56, 55, 54, 53, 51, 50, 49, 48, 46, 45, 44, 43, 42,
    41, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 
    21, 20, 19, 19, 18, 17, 16, 15, 15, 14, 13, 13, 12, 11, 11, 10, 9, 9, 8, 8, 
    7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 3, 2, 2, 2, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 4, 
    5, 5, 6, 6, 7, 7, 8, 8, 9, 10, 10, 11, 12, 12, 13, 14, 14, 15, 16, 17, 17, 18, 
    19, 20, 21, 22, 23, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 37, 38, 
    39, 40, 41, 42, 43, 45, 46, 47, 48, 49, 51, 52, 53, 55, 56, 57, 58, 60, 61, 62, 
    64, 65, 67, 68, 69, 71, 72, 74, 75, 76, 78, 79, 81, 82, 84, 85, 87, 88, 90, 91, 
    93, 94, 96, 97, 99, 100, 102, 103, 105, 106, 108, 109, 111, 112, 114, 116, 117, 
    119, 120, 122, 123, 125, 127};

unsigned const char sin_tab_middle[SIZE_MIDDLE] = {127, 130, 133, 136, 139, 142, 145, 
    148, 151, 154, 157, 161, 164, 166, 169, 172, 175, 178, 181, 184, 187, 189, 192, 195, 
    197, 200, 202, 205, 207, 210, 212, 214, 217, 219, 221, 223, 225, 227, 229, 231, 232, 
    234, 236, 237, 239, 240, 242, 243, 244, 245, 246, 247, 248, 249, 250, 251, 251, 252, 
    252, 253, 253, 253, 253, 253, 253, 253, 253, 253, 253, 252, 252, 251, 251, 250, 249, 
    249, 248, 247, 246, 245, 243, 242, 241, 239, 238, 236, 235, 233, 231, 230, 228, 226, 
    224, 222, 220, 218, 215, 213, 211, 209, 206, 204, 201, 199, 196, 193, 191, 188, 185, 
    182, 180, 177, 174, 171, 168, 165, 162, 159, 156, 153, 150, 147, 144, 141, 137, 134, 
    131, 128, 125, 122, 119, 116, 112, 109, 106, 103, 100, 97, 94, 91, 88, 85, 82, 79, 
    76, 73, 71, 68, 65, 62, 60, 57, 54, 52, 49, 47, 44, 42, 40, 38, 35, 33, 31, 29, 27, 
    25, 23, 22, 20, 18, 17, 15, 14, 12, 11, 10, 8, 7, 6, 5, 4, 4, 3, 2, 2, 1, 1, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 14, 16, 17, 19, 
    21, 22, 24, 26, 28, 30, 32, 34, 36, 39, 41, 43, 46, 48, 51, 53, 56, 58, 61, 64, 66, 
    69, 72, 75, 78, 81, 84, 87, 89, 92, 96, 99, 102, 105, 108, 111, 114, 117, 120, 123, 127};

unsigned const char sin_tab_high[SIZE_HIGH] = {127, 139, 151, 163, 175, 186, 197, 207, 216, 225, 
    232, 239, 244, 248, 251, 253, 254, 253, 251, 248, 244, 239, 232, 225, 216, 207, 197, 186, 175, 
    163, 151, 139, 126, 114, 102, 90, 78, 67, 56, 46, 37, 28, 21, 14, 9, 5, 2, 0, 0, 0, 2, 5, 9, 14, 
    21, 28, 37, 46, 56, 67, 78,  90,  102, 114};

// VOLATILE FIELDS
volatile uint16_t sinDelay = 0;
volatile uint16_t mCount1ms = 0;
volatile unsigned char dispData[LCD_BUFFER] = HIGH_CAPTION;
volatile unsigned char eventController = 0;

// MAIN FUNCTIONS >>>>
// SINUS 
void sinus (unsigned char *arr, uint16_t size);

// PORT
void port_ini (void);

// sinDelay
volatile uint16_t get_sin_delay(void);

// EVENT
void get_up_down_event (void);
volatile unsigned char get_swtch_event (void);

// TIMER
void timer1_ini (void);
void sync_timer1ms(uint16_t);

// LCD
void lcd_init();    
void display_str (unsigned char str[LCD_BUFFER]);
void display_delay (void);

int main (void)
{
  port_ini();
  timer1_ini();
  lcd_init();

  while (1) {

    display_str(HIGH_CAPTION);
    while (!get_swtch_event()) {
      sinus(sin_tab_high, SIZE_HIGH);
    }
    
    display_str(MIDDLE_CAPTION);
    while (!get_swtch_event()) {
      sinus(sin_tab_middle, SIZE_MIDDLE);
    }

    display_str(LOW_CAPTION);
    while (!get_swtch_event()) {
      sinus(sin_tab_low, SIZE_LOW);
    }

  }
}

/*
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
                GET/INC/DEC/RESET sinDelay
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/

volatile uint16_t get_sin_delay() 
{
  return sinDelay;
}

void inc_sin_delay(void) 
{
  if(sinDelay < MAX_SIN_DELAY) {
    sinDelay++;
  }
}

void dec_sin_delay(void) 
{
  if (sinDelay > 0) {
    sinDelay--;
  }
}

void reset_sin_delay(void)
{
  sinDelay = 0;
}

/*
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
                GET/INC mCount1ms 
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/

volatile uint16_t get_mcount1ms(void) 
{
  return mCount1ms;
}

void inc_mcount1ms(void)
{
  if (mCount1ms < MS) {
    mCount1ms++;
  } else {
    mCount1ms = 0;
  }
}

/*
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
                GET/SET dispData
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/

volatile unsigned char get_disp_data_char(unsigned char index) 
{
  return dispData[index];
}

void set_disp_data_char(unsigned char symb, unsigned char index) 
{
  dispData[index] = symb;
}

/*
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
                GET/SET eventController
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/

volatile unsigned char get_event(void) 
{
  return eventController;
}

void set_event (unsigned char eventCode) 
{
  eventController = eventCode;
}


/*
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
                PORTS FUNCTIONS
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/

void out_port_b (unsigned char data) 
{
  PORTB = data;
}

void out_port_c (unsigned char data) 
{
  data = data & ((1 << RS) | (1 << RW) | (1 << E));
  PORTC = PORTC | data;
  data = ~data ^ ((1 << RS) | (1 << RW) | (1 << E));
  PORTC = PORTC & data;
}

void out_port_d (unsigned char data) 
{
  PORTD = data;
}

void port_ini (void)
{
  out_port_b(0x00);
  PORTC   = 0xff;
  out_port_c(0x00);
  out_port_d(0x00);

  DDRB  = 0xFF;
  DDRC  = DDRC | ((1 << RS) | (1 << RW) | (1 << E));
  DDRC  = DDRC & ~((1 << BTN_UP) | (1 << BTN_DOWN) | (1 << BTN_SWTCH));
  DDRD  = 0xFF;
}		

/*
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
                SINUS FUNCTIONS
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/

void sinus (unsigned char *arr, uint16_t size) 
{
  static uint16_t index = 0;

  if (index > size) index = 0;

  out_port_d(arr[index]);

  get_up_down_event();

  sync_timer1ms(get_sin_delay());

  index++;
}						  


/*
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
                EVENT FUNCTIONS
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/


void get_up_down_event (void) 
{
  static unsigned char old_event;
  unsigned char new_event;

  new_event = get_event();

  if ((new_event & EVENT_BTN_UP) && (~old_event & EVENT_BTN_UP)) {
    inc_sin_delay();
    display_delay();
  }

  if ((new_event & EVENT_BTN_DOWN) && (~old_event & EVENT_BTN_DOWN)) {
    dec_sin_delay();
    display_delay();
  }

  old_event = new_event;
}

volatile unsigned char get_swtch_event (void) 
{
  static unsigned char old_event;
  unsigned char new_event;
  unsigned char ret = 0;

  new_event = get_event();

  if ((new_event & EVENT_BTN_SWTCH) && (~old_event & EVENT_BTN_SWTCH)) {
    reset_sin_delay();
    ret = 1;
  }

  old_event = new_event;

  return ret;
}


/*
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
                TIMER FUNCTIONS
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/


void timer1_ini (void)
{
  TCCR1B  = CONF_TIME1; 
  TIMSK   = TIMSK | (1<<OCIE1A);	
  OCR1AH  = HIGH_KOD_TIME1;
  OCR1AL  = LOW_KOD_TIME1;

  sei();
}

ISR (TIMER1_COMPA_vect)
{
  inc_mcount1ms();
  set_event( (BTN_UP_CHCK) | (BTN_DOWN_CHCK) | (BTN_SWTCH_CHCK) );
}

void sync_timer1ms (uint16_t inp_delay)
{
  if (inp_delay != 0) {
    inp_delay = (inp_delay + get_mcount1ms()) % MS;
    while(inp_delay != get_mcount1ms());
  }
}


/*
<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
                LCD DISPLAY FUNCTIONS
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
*/


void lcd_cmd (unsigned char command)  //Function to send command instruction to LCD
{
  out_port_b(command);
  out_port_c( ((0 << RS) | (0 << RW) | (1 << E)) );
  
  sync_timer1ms(LCD_DELAY);

  out_port_c( (0 << E) );

}

void lcd_data(unsigned char data)  //Function to send display data to LCD
{
  out_port_b(data);
  out_port_c( ((1 << RS) | (0 << RW) | (1 << E)) );

  sync_timer1ms(LCD_DELAY);

  out_port_c( ((1 << RS) | (0 << RW) | (0 << E)) );

}

void lcd_clr(void)
{
  lcd_cmd(0x01);  // clear screen
  lcd_cmd(0x02);  // return home
}

void lcd_init()    //Function to prepare the LCD  and get it ready
{
  lcd_cmd(0x38);  // for using 2 lines and 5X7 matrix of LCD
  lcd_cmd(0x0C);  // turn display ON
  lcd_cmd(0x01);  // clear screen
  lcd_cmd(0x81);  // bring cuRSor to position 1 of line 1
}


void lcd_show (void) 
{
  unsigned char iter = 0;
  unsigned char curr_char;

  lcd_clr();

  for (iter = 0; iter < LCD_BUFFER; iter++) {
    curr_char = get_disp_data_char(iter);

    if (curr_char == '\0') {
      lcd_cmd(0xC1);
    } else {
      lcd_data(curr_char);
    }
  }
}

void display_str (unsigned char str[LCD_BUFFER]) 
{
  unsigned char iter;

  for(iter = 0; iter < LCD_BUFFER; iter++) {
    set_disp_data_char(str[iter], iter);
  }
  lcd_show();
}

void display_delay(void)
{
  unsigned char curr_sin_delay;
  unsigned char iter;

  curr_sin_delay = get_sin_delay();

  for (iter = LETTERS; iter < LCD_BUFFER; iter++) {
    set_disp_data_char((curr_sin_delay / 10) + '0', iter);
    curr_sin_delay = (curr_sin_delay % 10) * 10;        
  } 

  lcd_show();
}