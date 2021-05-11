#include <DS3231.h>//Memanggil RTC3231 Library
#include <Wire.h>  // i2C Conection Library
#include <LiquidCrystal.h> //Libraries
#include <EEPROM.h>

LiquidCrystal lcd(2, 3, 4, 5, 6, 7); //Arduino pins to lcd

#define times   A0
#define up     A1
#define down   A2
#define alarm  A3

#define buzzer 8

DS3231  rtc(SDA, SCL);

// Init a Time-data structure
Time  t; //pencacah string times()

int hour = 0, minute = 0, second = 0, day = 0, months = 0, set_day;
int years = 0;
String Day = "  ";

int AlarmHour  = 21, AlarmMinute  = 22, AlarmSecond  = 23, setMode = 0, setAlarm = 0, alarmMode=0;

int stop =0, mode=0, flag=0;

//Eeprom Store Variable
uint8_t Hour;
uint8_t Minute;

 byte bell_symbol[8] = {
        B00100,
        B01110,
        B01110,
        B01110,
        B01110,
        B11111,
        B01000,
        B00100};
byte thermometer_symbol[8] = {
        B00100,
        B01010,
        B01010,
        B01110,
        B01110,
        B11111,
        B11111,
        B01110};


void setup(){
// Setup Serial connection
  Serial.begin(9600);

  rtc.begin(); // memulai koneksi i2c dengan RTC

 pinMode(times,  INPUT_PULLUP);
 pinMode(up,    INPUT_PULLUP);
 pinMode(down,  INPUT_PULLUP);
 pinMode(alarm, INPUT_PULLUP);
 pinMode(buzzer, OUTPUT);

  lcd.createChar(1, thermometer_symbol);
  lcd.createChar(2, bell_symbol);
  
  lcd.begin(16, 2); // Configura lcd numero columnas y filas
  lcd.setCursor(0,0);  //Show "TIME" on the LCD

  lcd.setCursor (0,0);
  lcd.print(" Real Time Clock ");
  lcd.setCursor (0,1);
  lcd.print("   With Alarm ");
  delay (2000);
  lcd.clear();

stop=EEPROM.read(50);
if(stop==0){  
}else{WriteEeprom ();}

EEPROM.write(50,0); 

ReadEeprom();
//Set RTC Untuk Pertama kali
//rtc.setDOW(2);     // Set Day-of-Week to SUNDAY
//rtc.setTime (00, 9, 50); 
//rtc.setDate(12, 11, 2017);  
}

void loop(){  
t = rtc.getTime();
Day = rtc.getDOWStr(1);

if (setMode == 0){
hour = t.hour,DEC;
minute = t.min,DEC;
second = t.sec,DEC;
day = t.date,DEC;
months = t.mon,DEC;
years = t.year,DEC;
}  

  if(setAlarm==0){
  lcd.setCursor(0,0); 
  lcd.print((hour/10)%10);
  lcd.print(hour % 10); 
  lcd.print(":");
  lcd.print((minute/10)%10);
  lcd.print(minute % 10);
  lcd.print(":");
  lcd.print((second/10)%10);
  lcd.print(second % 10);
  lcd.print(" ");  
  if(mode==1){lcd.write(2);}
  else{lcd.print(" ");}   
  lcd.print(" "); 
  lcd.write(1); 
  lcd.print(rtc.getTemp(),0);
  lcd.write(223); 
  lcd.print("C");
  lcd.print("  "); 

  lcd.setCursor(1,1);
  lcd.print(Day);
  lcd.print(" ");
  lcd.print((day/10)%10);
  lcd.print(day % 10); 
  lcd.print("/");
  lcd.print((months/10)%10);
  lcd.print(months % 10);
  lcd.print("/"); 
  lcd.print((years/1000)%10);
  lcd.print((years/100)%10);
  lcd.print((years/10)%10);
  lcd.print(years % 10);
  }


setupClock();
setTimer();
delay (100);
blinking();

//Alarm
if (alarmMode==1 && mode==1 && hour==AlarmHour && minute==AlarmMinute && second>=AlarmSecond) {
for(int i = 0 ; i< 20 ; i++){
  digitalWrite(buzzer, HIGH);
  delay (300);
  digitalWrite(buzzer, LOW);
  delay (300);
  if(digitalRead (times) == 1 || digitalRead (up) == 0 || digitalRead (down) == 0 || digitalRead (alarm) == 1){
    break;
  }
}
}else{digitalWrite(buzzer, LOW);}


delay (100);
}

void blinking (){
//BLINKING SCREEN
if (setAlarm <2 && setMode == 1){lcd.setCursor(0,0);  lcd.print("  ");}
if (setAlarm <2 && setMode == 2){lcd.setCursor(3,0);  lcd.print("  ");}
if (setAlarm <2 && setMode == 3){lcd.setCursor(6,0);  lcd.print("  ");}
if (setAlarm <2 && setMode == 4){lcd.setCursor(1,1);  lcd.print("   ");}
if (setAlarm <2 && setMode == 5){lcd.setCursor(5,1);  lcd.print("  ");}
if (setAlarm <2 && setMode == 6){lcd.setCursor(8,1);  lcd.print("  ");}
if (setAlarm <2 && setMode == 7){lcd.setCursor(11,1); lcd.print("    "); }
//Alarm
if (setMode == 0 && setAlarm == 1){lcd.setCursor(6,0); lcd.print("           "); }
if (setMode == 0 && setAlarm == 2){lcd.setCursor(4,1); lcd.print("  "); }
if (setMode == 0 && setAlarm == 3){lcd.setCursor(7,1); lcd.print("  "); }
if (setMode == 0 && setAlarm == 4){lcd.setCursor(10,1);lcd.print("  "); }
}

//Seting Jam ,Tanggal,Alarm/Timer
void setupClock (void) {
   
    if (setMode == 8){
    lcd.setCursor (0,0);
    lcd.print (F("Set Date Finish "));
    lcd.setCursor (0,1);
    lcd.print (F("Set Time Finish "));
    delay (1000);
    rtc.setTime (hour, minute, second);
    rtc.setDate (day, months, years);  
    lcd.clear();
    setMode = 0;
    }

    if (setAlarm == 5){
    lcd.setCursor (0,0);
    lcd.print (F("Set Alarm Finish"));
    lcd.setCursor (0,1);
    lcd.print (F("-EEPROM Updated-"));
    WriteEeprom();
    delay (2000); 
    lcd.clear();
    setAlarm=0;
    alarmMode=1;
    }
    
 if (setAlarm >0){ alarmMode=0;}
    
 if(digitalRead (times) == 0 && flag==0) {flag=1;
 if(setAlarm>0){setAlarm=5;}
 else{setMode = setMode+1;}
 }
  
 if(digitalRead (alarm) == 0 && flag==0){flag=1;
 if(setMode>0){setMode=8;}
  else{setAlarm = setAlarm+1;} 
  lcd.clear();} 

if(digitalRead (times) == 1 && digitalRead (alarm) == 1){flag=0;}
  
 if(digitalRead (up) == 0){                          
            if (setAlarm<2 && setMode==1)hour=hour+1; 
            if (setAlarm<2 && setMode==2)minute=minute+1;
            if (setAlarm<2 && setMode==3)second=second+1;
            if (setAlarm<2 && setMode==4)set_day=set_day+1;
            if (setAlarm<2 && setMode==5)day=day+1;
            if (setAlarm<2 && setMode==6)months=months+1;
            if (setAlarm<2 && setMode==7)years=years+1;
            //Alarm
            if (setMode==0 && setAlarm==1)mode=1;
            if (setMode==0 && setAlarm==2 && AlarmHour<23)AlarmHour=AlarmHour+1;
            if (setMode==0 && setAlarm==3 && AlarmMinute<59)AlarmMinute=AlarmMinute+1;
            if (setMode==0 && setAlarm==4 && AlarmSecond<59)AlarmSecond=AlarmSecond+1;
if(AlarmHour>23)AlarmHour=0;
if(AlarmSecond>59)AlarmSecond=0;
if(AlarmSecond>59)AlarmSecond=0;
if(hour>23)hour=0;
if(minute>59)minute=0;
if(second>59)second=0;
if(set_day>7)set_day=0;
if(day>31)day=0;
if(months>12)months=0;
if(years>2030)years=2000;
rtc.setDOW(set_day);
 }        

if(digitalRead (down) == 0){                                      
            if (setAlarm<2 && setMode==1)hour=hour-1; 
            if (setAlarm<2 && setMode==2)minute=minute-1;
            if (setAlarm<2 && setMode==3)second=second-1;
            if (setAlarm<2 && setMode==4)set_day=set_day-1;
            if (setAlarm<2 && setMode==5)day=day-1;
            if (setAlarm<2 && setMode==6)months=months-1;
            if (setAlarm<2 && setMode==7)years=years-1;
            //Alarm
            if (setMode==0 && setAlarm==1 )mode=0;
            if (setMode==0 && setAlarm==2 && AlarmHour>0)AlarmHour=AlarmHour-1;
            if (setMode==0 && setAlarm==3 && AlarmMinute>0)AlarmMinute=AlarmMinute-1;
            if (setMode==0 && setAlarm==4 && AlarmSecond>0)AlarmSecond=AlarmSecond-1;
if(hour<0)hour=23;
if(minute<0)minute=59;
if(second<0)second=59;
if(set_day<0)set_day=7;
if(day<0)day=31;
if(months<0)months=12;
if(years<0)years=2030;
rtc.setDOW(set_day);
 }
 
}

void setTimer (){
  //Alarm
 if (setMode == 0 && setAlarm >0){
  lcd.setCursor (0,0);
  lcd.print("Alarm ");
 if(mode==0){lcd.print("Deactivate");}
        else{lcd.print("Activated ");}
        
  lcd.setCursor (4,1);
  lcd.print((AlarmHour/10)%10);
  lcd.print(AlarmHour % 10);
  lcd.print(":");
  lcd.print((AlarmMinute/10)%10);
  lcd.print(AlarmMinute % 10);
  lcd.print(":");
  lcd.print((AlarmSecond/10)%10);
  lcd.print(AlarmSecond % 10);
 }
}

void ReadEeprom () {
  AlarmHour=EEPROM.read(1);
  AlarmMinute=EEPROM.read(2);
  AlarmSecond=EEPROM.read(3);
  
  mode=EEPROM.read(4); 
}

void WriteEeprom () {
  EEPROM.write(1,AlarmHour);
  EEPROM.write(2,AlarmMinute);
  EEPROM.write(3,AlarmSecond);
  EEPROM.write(4,mode);
}
