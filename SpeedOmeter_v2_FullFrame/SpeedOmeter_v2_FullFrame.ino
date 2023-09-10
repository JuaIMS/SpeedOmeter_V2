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
