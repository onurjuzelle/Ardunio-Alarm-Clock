
#include <LiquidCrystal.h>
#include <Wire.h>
 
LiquidCrystal lcd(3, 4, 5, 6, 7, 8);
 
const int button1   =  9;                   // button1 pin number
const int button2   = 10;                   // button1 pin number
const int button3   = 11;                   // button1 pin number
const int alarm_pin = 12;                   // Alarms pin number
 
void setup() {
  pinMode(9,  INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  pinMode(11, INPUT_PULLUP);
  pinMode(12, OUTPUT);
  digitalWrite(alarm_pin, LOW);
  
  lcd.begin(20, 4);
  Wire.begin();                             
  attachInterrupt(digitalPinToInterrupt(2), Alarm, FALLING);
}
 

bool alarm1_status, alarm2_status;
char Time[]     = "  :  :  ",
     calendar[] = "      /  /20  ",
     alarm1[]   = "A1:   :  :00", alarm2[]   = "A2:   :  :00",
     temperature[] = "T:   .   C";
byte  i, second, minute, hour, day, date, month, year,
      alarm1_minute, alarm1_hour, alarm2_minute, alarm2_hour,
      status_reg;
 
void Alarm(){
  digitalWrite(alarm_pin, HIGH);
}
void DS3231_read(){                             
  Wire.beginTransmission(0x68);                 
  Wire.write(0);                                
  Wire.endTransmission(false);                  
  Wire.requestFrom(0x68, 7);                    
  second = Wire.read();                         
  minute = Wire.read();                         
  hour   = Wire.read();                         
  day    = Wire.read();                         
  date   = Wire.read();                         
  month  = Wire.read();                         
  year   = Wire.read();                         
}
void alarms_read_display(){                    
  byte control_reg, temperature_lsb;
  char temperature_msb;
  Wire.beginTransmission(0x68);                
  Wire.write(0x08);                            
  Wire.endTransmission(false);                 
  Wire.requestFrom(0x68, 11);                  
  alarm1_minute = Wire.read();                
  alarm1_hour   = Wire.read();                
  Wire.read();                                
  alarm2_minute = Wire.read();                 
  alarm2_hour   = Wire.read();                
  Wire.read();                                 
  control_reg = Wire.read();                   
  status_reg  = Wire.read();                  
  Wire.read();                                 
  temperature_msb = Wire.read();             
  temperature_lsb = Wire.read();              
    
  alarm1_minute = (alarm1_minute >> 4) * 10 + (alarm1_minute & 0x0F);
  alarm1_hour   = (alarm1_hour   >> 4) * 10 + (alarm1_hour & 0x0F);
  alarm2_minute = (alarm2_minute >> 4) * 10 + (alarm2_minute & 0x0F);
  alarm2_hour   = (alarm2_hour   >> 4) * 10 + (alarm2_hour & 0x0F);
   
  alarm1[8]     = alarm1_minute % 10  + 48;
  alarm1[7]     = alarm1_minute / 10  + 48;
  alarm1[5]     = alarm1_hour   % 10  + 48;
  alarm1[4]     = alarm1_hour   / 10  + 48;
  alarm2[8]     = alarm2_minute % 10  + 48;
  alarm2[7]     = alarm2_minute / 10  + 48;
  alarm2[5]     = alarm2_hour   % 10  + 48;
  alarm2[4]     = alarm2_hour   / 10  + 48;
  alarm1_status = bitRead(control_reg, 0);      
  alarm2_status = bitRead(control_reg, 1);     
  if(temperature_msb < 0){
    temperature_msb = abs(temperature_msb);
    temperature[2] = '-';
  }
  else
    temperature[2] = ' ';
  temperature_lsb >>= 6;
  temperature[4] = temperature_msb % 10  + 48;
  temperature[3] = temperature_msb / 10  + 48;
  if(temperature_lsb == 0 || temperature_lsb == 2){
    temperature[7] = '0';
    if(temperature_lsb == 0) temperature[6] = '0';
    else                     temperature[6] = '5';
  }
  if(temperature_lsb == 1 || temperature_lsb == 3){
    temperature[7] = '5';
    if(temperature_lsb == 1) temperature[6] = '2';
    else                     temperature[6] = '7';
  }
  temperature[8]  = 223;                        
  lcd.setCursor(10, 0);
  lcd.print(temperature);                       
  lcd.setCursor(0, 2);
  lcd.print(alarm1);                            
  lcd.setCursor(17, 2);
  if(alarm1_status)  lcd.print("ON ");          
  else               lcd.print("OFF");         
  lcd.setCursor(0, 3);
  lcd.print(alarm2);                            
  lcd.setCursor(17, 3);
  if(alarm2_status)  lcd.print("ON ");          
  else               lcd.print("OFF");          
}
void calendar_display(){                        
  switch(day){
    case 1:  strcpy(calendar, "Sun   /  /20  "); break;
    case 2:  strcpy(calendar, "Mon   /  /20  "); break;
    case 3:  strcpy(calendar, "Tue   /  /20  "); break;
    case 4:  strcpy(calendar, "Wed   /  /20  "); break;
    case 5:  strcpy(calendar, "Thu   /  /20  "); break;
    case 6:  strcpy(calendar, "Fri   /  /20  "); break;
    case 7:  strcpy(calendar, "Sat   /  /20  "); break;
    default: strcpy(calendar, "Sat   /  /20  ");
  }
  calendar[13] = year  % 10 + 48;
  calendar[12] = year  / 10 + 48;
  calendar[8]  = month % 10 + 48;
  calendar[7]  = month / 10 + 48;
  calendar[5]  = date  % 10 + 48;
  calendar[4]  = date  / 10 + 48;
  lcd.setCursor(0, 1);
  lcd.print(calendar);                        
}
void DS3231_display(){

  second = (second >> 4) * 10 + (second & 0x0F);
  minute = (minute >> 4) * 10 + (minute & 0x0F);
  hour = (hour >> 4) * 10 + (hour & 0x0F);
  date = (date >> 4) * 10 + (date & 0x0F);
  month = (month >> 4) * 10 + (month & 0x0F);
  year = (year >> 4) * 10 + (year & 0x0F);
 
  Time[7]     = second % 10  + 48;
  Time[6]     = second / 10  + 48;
  Time[4]     = minute % 10  + 48;
  Time[3]     = minute / 10  + 48;
  Time[1]     = hour   % 10  + 48;
  Time[0]     = hour   / 10  + 48;
  calendar_display();                          
  lcd.setCursor(0, 0);
  lcd.print(Time);                          
}
void Blink(){
  byte j = 0;
  while(j < 10 && (digitalRead(button1) || i >= 5) && digitalRead(button2) && (digitalRead(button3) || i < 5)){
    j++;
    delay(25);
  }
}
byte edit(byte x, byte y, byte parameter){
  char text[3];
  while(!digitalRead(button1) || !digitalRead(button3));    
  while(true){
    while(!digitalRead(button2)){                           
      parameter++;
      if(((i == 0) || (i == 5)) && parameter > 23)         
        parameter = 0;
      if(((i == 1) || (i == 6)) && parameter > 59)        
        parameter = 0;
      if(i == 2 && parameter > 31)                         
        parameter = 1;
      if(i == 3 && parameter > 12)                        
        parameter = 1;
      if(i == 4 && parameter > 99)                        
        parameter = 0;
      if(i == 7 && parameter > 1)                         
        parameter = 0;
      lcd.setCursor(x, y);
      if(i == 7){                                          
        if(parameter == 1)  lcd.print("ON ");
        else                lcd.print("OFF");
      }
      else{
        sprintf(text,"%02u", parameter);
        lcd.print(text);
      }
      if(i >= 5){
        DS3231_read();                        
        DS3231_display();                      
      }
      delay(200);                            
    }
    lcd.setCursor(x, y);
    lcd.print("  ");                          
    if(i == 7) lcd.print(" ");                
    Blink();                                   
    lcd.setCursor(x, y);
    if(i == 7){                                
      if(parameter == 1)  lcd.print("ON ");
      else                lcd.print("OFF");
    }
    else{
      sprintf(text,"%02u", parameter);
      lcd.print(text);
    }
    Blink();
    if(i >= 5){
      DS3231_read();
      DS3231_display();}
    if((!digitalRead(button1) && i < 5) || (!digitalRead(button3) && i >= 5)){
      i++;                                      
      return parameter;                        
    }
  }
}
 
void loop() {
  if(!digitalRead(button1)){                
      i = 0;
      hour   = edit(0, 0, hour);
      minute = edit(3, 0, minute);
      while(!digitalRead(button1));           
      while(true){
        while(!digitalRead(button2)){          
          day++;                               
          if(day > 7) day = 1;
          calendar_display();                 
          lcd.setCursor(0, 1);
          lcd.print(calendar);             
          delay(200);
        }
        lcd.setCursor(0, 1);
        lcd.print("   ");                     
        Blink();
        lcd.setCursor(0, 1);
        lcd.print(calendar);                  
        Blink();                              
        if(!digitalRead(button1))              
          break;
      }
      date = edit(4, 1, date);                
      month = edit(7, 1, month);              
      year = edit(12, 1, year);               
     
      minute = ((minute / 10) << 4) + (minute % 10);
      hour = ((hour / 10) << 4) + (hour % 10);
      date = ((date / 10) << 4) + (date % 10);
      month = ((month / 10) << 4) + (month % 10);
      year = ((year / 10) << 4) + (year % 10);
      
     
      Wire.beginTransmission(0x68);             
      Wire.write(0);                            
      Wire.write(0);                          
      Wire.write(minute);                      
      Wire.write(hour);                        
      Wire.write(day);                        
      Wire.write(date);                      
      Wire.write(month);                      
      Wire.write(year);                        
      Wire.endTransmission();                  
      delay(200);
    }
    if(!digitalRead(button3)){                 
      while(!digitalRead(button3));             
      i = 5;
      alarm1_hour   = edit(4,  2, alarm1_hour);
      alarm1_minute = edit(7,  2, alarm1_minute);
      alarm1_status = edit(17, 2, alarm1_status);
      i = 5;
      alarm2_hour   = edit(4,  3, alarm2_hour);
      alarm2_minute = edit(7,  3, alarm2_minute);
      alarm2_status = edit(17, 3, alarm2_status);
      alarm1_minute = ((alarm1_minute / 10) << 4) + (alarm1_minute % 10);
      alarm1_hour   = ((alarm1_hour   / 10) << 4) + (alarm1_hour % 10);
      alarm2_minute = ((alarm2_minute / 10) << 4) + (alarm2_minute % 10);
      alarm2_hour   = ((alarm2_hour   / 10) << 4) + (alarm2_hour % 10);
      // Write alarms data to DS3231
      Wire.beginTransmission(0x68);             
      Wire.write(7);                            
      Wire.write(0);                             
      Wire.write(alarm1_minute);                
      Wire.write(alarm1_hour);                    
      Wire.write(0x80);                        
      Wire.write(alarm2_minute);                
      Wire.write(alarm2_hour);               
      Wire.write(0x80);                          
      Wire.write(4 | alarm1_status | (alarm2_status << 1));    
      Wire.write(0);                             
      Wire.endTransmission();                   
      delay(200);                                
    }
    if(!digitalRead(button2) && digitalRead(alarm_pin)){       
      digitalWrite(alarm_pin, LOW);              
      Wire.beginTransmission(0x68);              
      Wire.write(0x0E);                          
      
      Wire.write(4 | (!bitRead(status_reg, 0) & alarm1_status) | ((!bitRead(status_reg, 1) & alarm2_status) << 1));
      Wire.write(0);                              
      Wire.endTransmission();                    
    }
    DS3231_read();                               
    alarms_read_display();                      
    DS3231_display();
    delay(50);                                  
}
