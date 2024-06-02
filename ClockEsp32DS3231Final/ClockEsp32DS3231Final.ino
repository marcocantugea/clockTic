#include <RTClib.h>
#include "LedController.hpp"
#include <String>

RTC_DS3231 rtc;

char daysOfWeek[7][12] = {
  "Sunday",
  "Monday",
  "Tuesday",
  "Wednesday",
  "Thursday",
  "Friday",
  "Saturday"
};

char meses[12][4]={
  "ENE",
  "FEB",
  "MAR",
  "ABR",
  "MAY",
  "JUN",
  "JUL",
  "AGO",
  "SEP",
  "OCT",
  "NOV",
  "DIC"
};

#define DIN 23
#define CS 5
#define CLK 18
#define BUTTON_PIN 15 

LedController<1,1> lc;
unsigned long delaytime=250;
long lastTime = 0;
unsigned long timerDelay = 20000;
unsigned long lastTimeDate = 0;
unsigned long timerDelayDate = 20000;
unsigned long lastTimeTicker = 0;
unsigned long timerDelayTicket = 1000;
unsigned long lastTimeDisplayedDate = 0;
unsigned long timerDelayDisplayedDate = 25000;
bool showThrs=false;
bool tickSecond=true;
bool showDate=false;
int lastState = HIGH; 
int currentState;
int ledIntensity=0;
bool manualSet=false;

void getDigits(float number, int * arrayToReturn){
  
   if(number<10){
      arrayToReturn[0]=0;
      arrayToReturn[1]=int(round(number));
    }else{
      float desNumber=number/10;
      int PartOne=abs(desNumber);
      int PartTwo=int(round((desNumber-float(PartOne))*10));

      arrayToReturn[0]=PartOne;
      arrayToReturn[1]=PartTwo;
    }
}

void changeBrigthness(){

  currentState = digitalRead(BUTTON_PIN);
  if (lastState == HIGH && currentState == LOW){
    ledIntensity+=1;
    if(ledIntensity>8){
      ledIntensity=0;
    }
    Serial.print("Set led intensity to:");
    Serial.println(ledIntensity);
    lc.setIntensity(ledIntensity);
    manualSet=true;
  }

  DateTime now = rtc.now();
  int hrsRtc = now.hour();

  //reset manualSet after 1pm
  if(manualSet && hrsRtc==13 ){
    manualSet=false;
  }

  if(manualSet && hrsRtc==1 ){
    manualSet=false;
  }

  if(!manualSet){

    if(hrsRtc>=0 &&  hrsRtc<=5) lc.setIntensity(0);
    if(hrsRtc>=6 &&  hrsRtc<18) lc.setIntensity(8);
    if(hrsRtc>18 &&  hrsRtc<=20) lc.setIntensity(4);
    if(hrsRtc>20 &&  hrsRtc<=23) lc.setIntensity(0);

  }
}

void setup () {
  Serial.begin(9600);

  if (! rtc.begin()) {
    Serial.println("RTC module is NOT found");
    Serial.flush();
    while (1);
  }

  lc=LedController<1,1>(DIN,CLK,CS);

  /* Set the brightness to a medium values */
  lc.setIntensity(ledIntensity);
  /* and clear the display */
  lc.clearMatrix();

  // automatically sets the RTC to the date & time on PC this sketch was compiled
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  // manually sets the RTC with an explicit date & time, for example to set
  // January 21, 2021 at 3am you would call:
  // rtc.adjust(DateTime(2021, 1, 21, 3, 0, 0));

  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop () {
  changeBrigthness();
  DateTime now = rtc.now();
  lc.clearMatrix();
  long time = millis();
  
  if(lastTimeDate < timerDelayDate){
    //Serial.println("entro en if show date");
    //Serial.println(lastTimeDate);
    //Serial.println(timerDelayDate);
    
    float dia=now.day();

    int digitsDay[2]={0,0};
    getDigits(dia,digitsDay);
    lc.setDigit(0,7,digitsDay[0],false);
    lc.setDigit(0,6,digitsDay[1],false);

    lc.setChar(0,5,'-',false);

    float mes= now.month();
    int digitsMoths[2]={0,0};
    getDigits(mes,digitsMoths);
    lc.setDigit(0,4,digitsMoths[0],false);
    lc.setDigit(0,3,digitsMoths[1],false);
    
    lc.setChar(0,2,'-',false);

    float anio=now.year();
    anio-=2000;
    int digitsYear[2]={0,0};
    getDigits(anio,digitsYear);
    
    lc.setDigit(0,1,digitsYear[0],false);
    lc.setDigit(0,0,digitsYear[1],false);

    //lastTimeDate = millis();
    lastTimeDate+= 100;
    lastTime=time;
  }else{

    if ((time - lastTime) > timerDelay) {
      //Serial.println("entro en if hrs 12 ");
      showThrs=!showThrs;
      lastTime = millis();
    }
    
    //calculate minutes and display on the 7 segment display
    float minutos = now.minute();

    int digitsMinutos[2] = { 0, 0 };
    getDigits(minutos, digitsMinutos);
    lc.setDigit(0, 1, digitsMinutos[0], false);
    lc.setDigit(0, 0, digitsMinutos[1], tickSecond);

    lc.setChar(0, 2, '-', false);

    //calculate hrs and display on the 7 segment display
    float hrsRtc = now.hour();

    if (showThrs) {
      if(hrsRtc==0) hrsRtc=12;
      if(hrsRtc>12) hrsRtc -= 12;
    }

    int digitsHrs[2] = { 0, 0 };
    getDigits(hrsRtc, digitsHrs);
    lc.setDigit(0, 4, digitsHrs[0], false);
    lc.setDigit(0, 3, digitsHrs[1], false);

    lc.setChar(0, 7, 'H', false);
    lc.setChar(0, 6, 'R', false);

    if ((time - lastTimeTicker) > timerDelayTicket) {
      tickSecond=!tickSecond;
      lastTimeTicker = millis();
    }

    if(showThrs && (lastTimeDisplayedDate <  timerDelayDisplayedDate)){
      //Serial.print("switch a fecha");
      lastTimeDisplayedDate+=100;
      //Serial.println(lastTimeDisplayedDate);
    }

    if(showThrs && (lastTimeDisplayedDate >=  timerDelayDisplayedDate) ){
      lastTimeDate=0;
      lastTimeDisplayedDate=0;
    }
    
    // {
    //   lastTimeDate=0;
    //   lastTimeDisplayedDate=0;
    // }
  }

  lastState = currentState;
  delay(100); // delay 1 seconds
  
}
