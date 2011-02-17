/*

 Accelero 2010-06
 
 */
//Include arduino headers
#include "WProgram.h"
#include "pins_arduino.h"

// display pinout
#define __spi_clock   SCK // SCK  (SH_CP of 74HC595)
#define __spi_latch   SS //      (ST_CP of 74HC595)
#define __spi_data    MOSI // MOSI (DS    of 74HC595)
#define __spi_data_in MISO // MISO - unused
//const uint8_t __spi_clock_PORTB = __spi_clock - 8;
//const uint8_t __spi_latch_PORTB = __spi_latch - 8;
//const uint8_t __spi_data_PORTB  = __spi_data  - 8;
#define __spi_clock_PORTB  5
#define __spi_latch_PORTB  2
#define __spi_data_PORTB   3
#define __SPI_LATCH_LOW   PORTB &= ~(1 << __spi_latch_PORTB)
#define __SPI_LATCH_HIGH  PORTB |=  (1 << __spi_latch_PORTB)
#define __display_enable_pin 9
const uint8_t __display_enable_PORTB = __display_enable_pin - 8;
#define __DISPLAY_OFF  PORTB &= ~(1 << __display_enable_PORTB)
#define __DISPLAY_ON   PORTB |= (1 << __display_enable_PORTB)

// feature
#define TIMER1_AUTO_PRECHARGE 1
#define PANEL_ONE_ROW_AT_A_TIME 1
#define LCDDISPLAY 1

// config
#define LCDDISPLAY_REFRESH_RATE    10 // Hz
#define ACCEL_READ_RATE    100 // Hz
#define TIMER1_SAFE_VALUE_MIN    4
#define TIMER1_PRECHARGE_UPDATE_RATE   10 // Hz
#define TIMER1_TARGET    600000 // us
#include "AnalogReader.h"
#include "SmoothReader.h"

#include <avr/pgmspace.h>

// functions prototypes
long maplimit(long, long, long, long, long);
inline void spi_transfer(uint8_t );
void setup_hardware_spi();
void setup_timer1_ovf();

AnalogReader accx(7, -8.0, 8.0);
AnalogReader accy(6, -8.0, 8.0);
AnalogReader batt(0, 0, 15.6);

#if LCDDISPLAY
#define ARDUINO 22
#include "Streaming.h"
#include "LiquidCrystal.h"
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
#endif

// display refresh rate
#define __TIMER1_MAX 0xFFFF // 16 bit CTR
volatile uint32_t timer1_precharge = 0x90;
volatile uint32_t timer1_timer = 0;
volatile uint16_t timer1_exec = 0;

// display spec
#define __rows 16
#define __cols 24
#define __brightness_level 16

#define __panel_count 6
#define __color_count 2
#define GREEN 0
#define RED 1

uint8_t panel[__color_count][__panel_count][8][__brightness_level];

void pixel(uint8_t row, uint8_t col, uint8_t color, uint8_t brightness) {
	uint8_t panel_row = row % 8;
	uint8_t panel_col = col % 8;
	uint8_t panel_id = col / 8 + 3 * (row / 8);

	for (uint8_t curr_level = 0; curr_level < __brightness_level; curr_level++) {
		if (brightness > curr_level)
			panel[color][panel_id][panel_row][curr_level] &= ~(1 << panel_col);
		else
			panel[color][panel_id][panel_row][curr_level] |= (1 << panel_col);
	}
}

void setup() {
	randomSeed(555);
	analogReference( DEFAULT); // 5V

	pinMode(__spi_latch, OUTPUT);
	pinMode(__spi_data, OUTPUT);
	pinMode(__spi_clock, OUTPUT);
	pinMode(__spi_data_in, INPUT);
	pinMode(__display_enable_pin, OUTPUT);
	digitalWrite(__spi_latch, LOW);
	digitalWrite(__spi_data, LOW);
	digitalWrite(__spi_clock, LOW);
	__DISPLAY_ON;

	setup_hardware_spi();
	delay(10);

	setup_timer1_ovf();

	for (uint8_t r = 0; r < __rows; r++) {
		for (uint8_t c = 0; c < __cols; c++) {
			//pixel(r, c, RED, __max_brightness);
			//pixel(r, c, GREEN, __max_brightness);
			pixel(r, c, RED, 0);
			pixel(r, c, GREEN, 0);
			//pixel(r, c, RED, (__max_brightness * r) / (__rows-1));
			//pixel(r, c, GREEN, (__max_brightness * c) / (__cols-1));
		}
	}

#if LCDDISPLAY
	lcd.begin(20, 4);
	lcd.print("ready");
#endif
}

ISR(TIMER1_OVF_vect) {
	TCNT1 = __TIMER1_MAX - timer1_precharge;

#if TIMER1_AUTO_PRECHARGE
	uint32_t timer = micros();
#endif

	static uint8_t row;

	for(uint8_t pwm_cycle = 0; pwm_cycle < __brightness_level; pwm_cycle++) {
		__SPI_LATCH_LOW;
		spi_transfer(1 << row);
		spi_transfer(panel[RED][5][row][pwm_cycle]);
		spi_transfer(panel[GREEN][5][row][pwm_cycle]);
		spi_transfer(1 << row);
		spi_transfer(panel[RED][4][row][pwm_cycle]);
		spi_transfer(panel[GREEN][4][row][pwm_cycle]);
		spi_transfer(1 << row);
		spi_transfer(panel[RED][3][row][pwm_cycle]);
		spi_transfer(panel[GREEN][3][row][pwm_cycle]);
		spi_transfer(1 << row);
		spi_transfer(panel[RED][2][row][pwm_cycle]);
		spi_transfer(panel[GREEN][2][row][pwm_cycle]);
		spi_transfer(1 << row);
		spi_transfer(panel[RED][1][row][pwm_cycle]);
		spi_transfer(panel[GREEN][1][row][pwm_cycle]);
		spi_transfer(1 << row);
		spi_transfer(panel[RED][0][row][pwm_cycle]);
		spi_transfer(panel[GREEN][0][row][pwm_cycle]);
		__SPI_LATCH_HIGH;
	}

	row++; // next time the ISR runs, the next row will be dealt with
	if(row >= __rows) {
		row = 0;
	}

#if TIMER1_AUTO_PRECHARGE
	timer1_exec++;
	timer1_timer += micros() - timer;
#endif

}

uint8_t lastrow = 0xFF, lastcol = 0xFF;
uint32_t t1 = 0, t2 = 0, t3 = 0;
const uint32_t t1out = 1000 / TIMER1_PRECHARGE_UPDATE_RATE;
const uint32_t t2out = 1000 / LCDDISPLAY_REFRESH_RATE;
const uint32_t t3out = 1000 / ACCEL_READ_RATE;

uint16_t loop_exec = 0;
void loop() {
	uint32_t now = millis();

	if (now - t3 > t3out) {
		t3 = now;

		long row = maplimit(-accx.getInt(), -800, 800, 0, __rows - 1);
		long col = maplimit(-accy.getInt(), -800, 800, 0, __cols - 1);

		if (lastrow != row || lastcol != col) {
			pixel(lastrow, lastcol, RED, 0);
			pixel(lastrow, lastcol, GREEN, 0);
			lastrow = row;
			lastcol = col;
		}

	}

	//brightness_green[random(__rows)][random(__cols)] = random(__max_brightness / 6) + 1;
	//brightness_red[random(__rows)][random(__cols)] = random(__max_brightness / 8);

	pixel(lastrow, lastcol, RED, __brightness_level);
	pixel(lastrow, lastcol, GREEN, __brightness_level);

	/*
	 const uint subdivision = 60;
	 const uint deadzonetop = 0;
	 const uint deadzonebottom = subdivision / 10;

	 const uint range = subdivision * __cols;

	 int pos_in_range = map((int) (-y * 1000), -800, 800, 0, range);
	 pos_in_range = max(0, pos_in_range);
	 pos_in_range = min(range, pos_in_range);

	 for (int led = 0; led <= __max_led; led++) {
	 uint ledrange = led * subdivision;
	 int diff = ledrange - pos_in_range;
	 if (diff < 0) {diff = -diff;}

	 if (diff <= deadzonetop) {
	 brightness_green[0][led] = __max_brightness;
	 } else if (diff <= subdivision - deadzonebottom) {
	 brightness_green[0][led] = map(diff, subdivision - deadzonebottom, deadzonetop, 0, __max_brightness);
	 } else {
	 brightness_green[0][led] = 0;
	 }
	 }
	 */

#if TIMER1_AUTO_PRECHARGE
	// timer1 precharge auto-adjust
	if (now - t1 > t1out) {
		t1 = now;

		const uint32_t t1_timer = timer1_timer * 1000UL / t1out;
		timer1_timer = 0;

		const int32_t diff = t1_timer - TIMER1_TARGET;
		if (diff + 150000 < 0 && timer1_precharge >= TIMER1_SAFE_VALUE_MIN + 5) {
			timer1_precharge -= 5;
		} else if (diff + 30000 < 0 && timer1_precharge
				>= TIMER1_SAFE_VALUE_MIN) {
			timer1_precharge -= 1;
		} else if (diff - 150000 > 0) {
			timer1_precharge += 5;
		} else if (diff - 30000 > 0) {
			timer1_precharge += 1;
		}

#if LCDDISPLAY
		const uint32_t t1_exec = timer1_exec * 1000UL / t1out;
		timer1_exec = 0;
		static uint8_t t1ctn;
		t1ctn++;
		if (t1ctn % 2 == 0) {
			lcd.setCursor(6, 3);
			lcd << _DEC(t1_timer) << " ";
			lcd.setCursor(15, 3);
			lcd << _DEC(timer1_precharge) << " ";
			lcd.setCursor(0, 3);
			lcd << _DEC(t1_exec) << "  ";
			t1ctn = 0;
		}
#endif

	}
#endif

#if LCDDISPLAY
	if (now - t2 > t2out) {
		t2 = now;

		/*
		 lcd.setCursor(0, 1);
		 for (int led = 5; led <= __max_led - 5; led++) {
		 word ledrange = led * subdivision;
		 int diff = ledrange - pos_in_range;
		 if (diff < 0)
		 diff = -diff;
		 lcd << _DEC(diff) << " ";
		 }

		 lcd.setCursor(0, 2);
		 for (int led = 5; led <= __max_led - 5; led++) {
		 lcd << _DEC(brightness_red[0][led]) << " ";
		 }
		 */

		/*
		 lcd.setCursor(10, 1);
		 lcd << _DEC(pos_in_range) << "/" << _DEC(range) << "  ";
		 */

		/*
		 */
		lcd.setCursor(0, 0);
		lcd << "x:" << _FLOAT(-accx.get(), 2) << " y:"
				<< _FLOAT(-accy.get(), 2) << "  ";

		lcd.setCursor(13, 2);
		lcd << _FLOAT(batt.get(), 2) << "v ";

		static uint8_t t2ctn;
		t2ctn++;
		if (t2ctn % 2 == 0) {
			const uint32_t lo_exec = loop_exec * 1000UL / t2out;
			lcd.setCursor(0, 2);
			lcd << _DEC(lo_exec) << " ";
			t2ctn = 0;
		}
		loop_exec = 0;

	}

	loop_exec++;
#endif

}

long maplimit(long x, long in_min, long in_max, long out_min, long out_max) {
	long res = map(x, in_min, in_max, out_min, out_max);
	res = max(out_min, res);
	res = min(out_max, res);
	return res;
}

inline void spi_transfer(uint8_t data) {
	SPDR = data; // Start the transmission
	while (!(SPSR & (1 << SPIF)))
		; // Wait the end of the transmission
}

void setup_hardware_spi(void) {
	byte clr;
	// spi prescaler:
	// SPI2X SPR1 SPR0
	//   0     0     0    fosc/4
	//   0     0     1    fosc/16
	//   0     1     0    fosc/64
	//   0     1     1    fosc/128
	//   1     0     0    fosc/2
	//   1     0     1    fosc/8
	//   1     1     0    fosc/32
	//   1     1     1    fosc/64
	SPCR |= ((1 << SPE) | (1 << MSTR)); // enable SPI as master
	//SPCR |= ( (1<<SPR1) ); // set prescaler bits
	SPCR &= ~((1 << SPR1) | (1 << SPR0)); // clear prescaler bits
	clr = SPSR; // clear SPI status reg
	clr = SPDR; // clear SPI data reg
	SPSR |= (1 << SPI2X); // set prescaler bits
	//SPSR &= ~(1<<SPI2X); // clear prescaler bits
}

void setup_timer1_ovf(void) {
	// Arduino runs at 16 Mhz...
	// Timer1 (16bit) Settings:
	// prescaler (frequency divider) values:
	// CS12    CS11   CS10
	//   0       0      0    stopped
	//   0       0      1      /1
	//   0       1      0      /8
	//   0       1      1      /64
	//   1       0      0      /256
	//   1       0      1      /1024
	//   1       1      0      external clock on T1 pin, falling edge
	//   1       1      1      external clock on T1 pin, rising edge
	TCCR1B &= ~((1 << CS11));
	TCCR1B |= ((1 << CS12) | (1 << CS10));
	//normal mode
	TCCR1B &= ~((1 << WGM13) | (1 << WGM12));
	TCCR1A &= ~((1 << WGM11) | (1 << WGM10));
	//Timer1 Overflow Interrupt Enable
	TIMSK1 |= (1 << TOIE1);
	TCNT1 = __TIMER1_MAX - timer1_precharge;
	// enable all interrupts
	sei();
}

