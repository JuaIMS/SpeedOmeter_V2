//Code was finalized on 10 Sep 23.


#include <SPI.h>
#include <U8g2lib.h>


const float tyrcircumference = 1.83f; //*** Expressed in meters, alter for your own tyre circumference. ***  


#define BackLight 8
#define Battpin A3
#define Ledpin A0

U8G2_PCD8544_84X48_F_4W_HW_SPI nokia(U8G2_R2, /* cs=*/4, /* dc=*/3, /* reset=*/5);  // Nokia 5110 Display

float adcVol, batvol, refvol = 1.1, R1 = 4700, R2 = 1000;
uint16_t adcRaw;

volatile unsigned long last_time_we_measured = 0, Period_btw_pulses = 0;
uint8_t Kph, MaxKph;

const unsigned char logo [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 
	0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xff, 0xff, 0x3f, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0xf8, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 
	0xff, 0xff, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xff, 0xff, 0xff, 0xff, 0x0f, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0xc0, 0xff, 0xff, 0xff, 0xef, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 
	0xff, 0xff, 0xff, 0xe3, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xff, 0xff, 0xff, 0xf3, 0xff, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xff, 0xff, 0xff, 0xf9, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 
	0xfe, 0xff, 0xff, 0xff, 0xfc, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00, 0xff, 0x1f, 0x00, 0x00, 0x0e, 
	0xe0, 0x07, 0x00, 0x00, 0x00, 0x00, 0xff, 0x03, 0x00, 0x00, 0x00, 0x07, 0x0e, 0x00, 0x00, 0x00, 
	0x80, 0xff, 0x00, 0x00, 0x00, 0x0f, 0x1f, 0x1c, 0x00, 0x00, 0x00, 0xc0, 0x3f, 0xfe, 0xc1, 0xc7, 
	0x0f, 0x3f, 0x18, 0x00, 0x00, 0x00, 0xc0, 0x9f, 0xff, 0xe1, 0xe3, 0x8f, 0x7f, 0x38, 0x00, 0x00, 
	0x00, 0xe0, 0xcf, 0xff, 0xe1, 0xf3, 0x87, 0x7f, 0x38, 0x00, 0x00, 0x00, 0xe0, 0xcf, 0xff, 0xe1, 
	0xf1, 0x87, 0x7f, 0x78, 0x00, 0x00, 0x00, 0xe0, 0xc7, 0xff, 0xe1, 0xf8, 0x87, 0x7f, 0x78, 0x00, 
	0x00, 0x00, 0xf0, 0xe7, 0xff, 0xe0, 0xf8, 0x87, 0x7f, 0x7c, 0x00, 0x00, 0x00, 0xf0, 0xe3, 0xff, 
	0xf0, 0xf8, 0x83, 0x3f, 0xfc, 0x00, 0x00, 0x00, 0xf0, 0xe3, 0xff, 0xf0, 0xfc, 0xc3, 0x1f, 0xfe, 
	0x00, 0x00, 0x00, 0xf0, 0xfb, 0xff, 0xf0, 0xff, 0xc3, 0x0f, 0xff, 0x00, 0x00, 0x00, 0xf0, 0xff, 
	0xff, 0xf0, 0xff, 0xc3, 0xc7, 0xff, 0x00, 0x00, 0x00, 0xf0, 0xff, 0xff, 0xf8, 0xff, 0x03, 0xf8, 
	0xff, 0x00, 0x00, 0x00, 0xf0, 0xff, 0x7f, 0xf8, 0xff, 0x03, 0xf0, 0xff, 0x00, 0x00, 0x00, 0xf0, 
	0xff, 0x7f, 0xf8, 0xff, 0xe1, 0xc3, 0xff, 0x00, 0x00, 0x00, 0xf0, 0xff, 0x7f, 0xf8, 0xff, 0xe1, 
	0x0f, 0x7f, 0x00, 0x00, 0x00, 0xf0, 0xff, 0x7f, 0xf8, 0xff, 0xe1, 0x0f, 0x7e, 0x00, 0x00, 0x00, 
	0xe0, 0xff, 0x3f, 0xfc, 0xff, 0xe1, 0x1f, 0x7e, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x3f, 0xfc, 0xff, 
	0xf0, 0x1f, 0x7c, 0x00, 0x00, 0x00, 0xe0, 0xff, 0x3f, 0xfc, 0xff, 0xf0, 0x1f, 0x3c, 0x00, 0x00, 
	0x00, 0xc0, 0xff, 0x3f, 0xfe, 0xff, 0xf8, 0x1f, 0x3c, 0x00, 0x00, 0x00, 0xc0, 0xff, 0x3f, 0xfe, 
	0xff, 0xf8, 0x1f, 0x1e, 0x00, 0x00, 0x00, 0x80, 0xff, 0x1f, 0xff, 0x7f, 0xfc, 0x1f, 0x1e, 0x00, 
	0x00, 0x00, 0x00, 0xff, 0x8f, 0xff, 0x7f, 0xfe, 0x1f, 0x0f, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xe7, 
	0xff, 0x9f, 0xff, 0x8f, 0x07, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x00, 0xf8, 0x03, 0xfe, 0xc3, 0x03, 
	0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0xf8, 0x01, 0x00, 0xf0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x30, 
	0x00, 0xfc, 0x01, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xff, 0xff, 0xff, 0xff, 0x7f, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xff, 0xff, 0xff, 0xff, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0xff, 0xff, 0xff, 0xff, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xff, 0xff, 0xff, 
	0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x80, 0xff, 0xff, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xff, 
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void updisp(void) {
  nokia.clearBuffer();

  nokia.drawFrame(0, 0, 84, 48);
  nokia.drawVLine(44, 0, 32);
  nokia.drawHLine(44, 32, 40);
  nokia.drawHLine(44, 17, 40);

  nokia.setFont(u8g2_font_7_Seg_41x21_mn);
  nokia.setCursor(1, 4);
  nokia.print(u8x8_u8toa(Kph, 2));


  nokia.setFont(u8g2_font_courB10_tr);
  nokia.drawStr(44, 44, "Kp/H");

  nokia.setFont(u8g2_font_bauhaus2015_tn);
  nokia.setCursor(48, 31);
  nokia.print(batvol, 2);

  nokia.setCursor(48, 16);
  nokia.print(u8x8_u8toa(MaxKph, 2));
  nokia.sendBuffer();
}


void Pulse_Event(void){
  Period_btw_pulses = millis() - last_time_we_measured;
  last_time_we_measured = millis();
}


void setup(void) {
  pinMode(BackLight, OUTPUT);
  pinMode(Ledpin, OUTPUT);
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(0, Pulse_Event, FALLING);
  analogReference(INTERNAL);
  digitalWrite(BackLight, HIGH);
  nokia.begin();
  nokia.setContrast(145);
  for(int8_t i=-100; i <=0; i+=5){
    nokia.clearBuffer();
    nokia.drawXBMP(i, i, 84, 48, logo);
    nokia.sendBuffer();
    delay(25);
  }
  delay(3000);
}

void loop(void) {
  updisp();

  Kph = ((3600000 / Period_btw_pulses) * tyrcircumference) / 1000; // This formula calculates the speed in KPH.
  
  if (Kph >= 99) {
    Kph = 99;
  }
  if (Kph > MaxKph) {
    MaxKph = Kph;
  }

  if(millis() - last_time_we_measured >= 6000){
    Period_btw_pulses =0;
    last_time_we_measured =0;
    Kph =0;
    MaxKph =0;
  }
  
  adcRaw = analogRead(Battpin);
  adcVol = (adcRaw * refvol)/1024;
  batvol = adcVol/(R2/(R1+R2));
}
