#include <Rotary.h>
#include <LiquidCrystal_I2C.h>
Rotary r = Rotary(2, 3);
LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

const int buzzer=12;
const int ok_pin=7;
const int relay_A=6;
const int relay_B=5;
const int relay_C=4;

const int batt_A=A0;
const int batt_B=A1;
const int batt_C=A2;

const float r_A=2050;
const float r_B=9900;


unsigned long capacity_A;
unsigned long capacity_B;
unsigned long capacity_C;

unsigned long lin_A;
unsigned long lin_B;
unsigned long lin_C;
unsigned long millis_A;
unsigned long millis_B;
unsigned long millis_C;
unsigned long Millis;
unsigned long hour;
unsigned long minute;
unsigned long second;
unsigned long minute_factor;
unsigned long second_factor;
//unsigned long capacity_A;
//unsigned long capacity_B;
//unsigned long capacity_C;

volatile int current_count=0;
volatile int disp_count=0;
volatile int done_disp_count=0;
volatile float cut_off_count=0.0;

float max_lvl;
float low_batt=3.2;
float full_batt=4.2;
float last_float1=0.0;
float last_float2=0.0;
float last_float3=0.0;
float start_VA;
float start_VB;
float start_VC;

int start_PA;
int start_PB;
int start_PC;
int status_A;
int status_B;
int status_C;
int current_A = 500;
int current_B = 500;
int current_C = 500;
int last_int1=0; // the variables with the prefix "last" are used to detect changes 
int last_int2=0; // and thereby help in properly clearing the lcd to revent flicker
int last_int3=0;
int last_intB1=101; // the variables with the prefix "last" are used to detect changes 
int last_intB2=101; // and thereby help in properly clearing the lcd to revent flicker
int last_intB3=101; // the B is for battery status as it proved better to separate each status flag
int last_second=0;
int last_option=0;
int last_capacity_A;
int last_capacity_B;
int last_capacity_C;
//int capacity_A;
//int capacity_B;
//int capacity_C;

bool busy = false;
bool alert_A=false;
bool alert_B=false;
bool alert_C=false;
bool change=false;
bool done = false;
bool A_done=false;
bool B_done=false;
bool C_done=false;

void setup() {
  // put your setup code here, to run once:
pinMode(relay_C,OUTPUT);
pinMode(relay_B,OUTPUT);
pinMode(relay_A,OUTPUT);
pinMode(buzzer,OUTPUT);
pinMode(ok_pin,INPUT_PULLUP);


  PCICR |= (1 << PCIE2);
  PCMSK2 |= (1 << PCINT18) | (1 << PCINT19);
  sei();
  
  lcd.begin(16,2);
  lcd.clear();
 
 
while(digitalRead(ok_pin) != false)
{   
    lcd.setCursor(1,0);
    lcd.print("cutoff voltage");
    lcd.setCursor(5,1);
    lcd.print(low_batt);
    lcd.print("V");
    if(change==true){
      if(low_batt != cut_off_count)lcd.clear();
      }
    if (change==true)low_batt=cut_off_count;
    if(digitalRead(ok_pin) == false)
    {
     beep(1,100);
     change = false;
      break;
      }
    }
lcd.clear();  
cut_off_count=0;
delay(100); 
while(digitalRead(ok_pin) != false)
{   
    lcd.setCursor(2,0);
    lcd.print("full voltage");
    lcd.setCursor(5,1);
    lcd.print(full_batt);
    lcd.print("V");
    if(change==true){
      if(full_batt != cut_off_count)lcd.clear();
    }
    if (change==true)full_batt=cut_off_count;
    if(digitalRead(ok_pin) == false)
    {
      beep(1,100);
      change= false;
      break;
      }
  }
cut_off_count=0;  


}

void loop() {
  start_VA=volt(1);
  start_VB=volt(2);
  start_VC=volt(3);

  start_PA=percent(1);
  start_PB=percent(2);
  start_PC=percent(3);

 Millis=millis();
 busy=true;
 digitalWrite(relay_A,HIGH);
 digitalWrite(relay_B,HIGH);
 digitalWrite(relay_C,HIGH);
 beep(1,300);
lcd.clear();

 while(done==false)
 {
 if(!A_done){
   status_A=100-(percent(1));
unsigned long time_A =  (millis()-Millis)/1000;
    capacity_A = (time_A * current_A)/3600; 

}
if(!B_done)
{
   status_B=100-(percent(2));
unsigned long time_B =  (millis()-Millis)/1000;
    capacity_B = (time_B * current_B)/3600; 
 
}
if(!C_done)
{
   status_C=100-(percent(3));
unsigned long time_C =  (millis()-Millis)/1000;
    capacity_C = (time_C * current_C)/3600; 

}
 
if(volt(1)<low_batt)
{
   millis_A=millis();
   A_done =true;
   digitalWrite(relay_A,LOW);
  }
if(volt(2)<low_batt)
{  
    millis_B=millis();
   B_done=true;
   digitalWrite(relay_B,LOW);
 }  
if(volt(3)<low_batt)
{
    millis_C=millis();
   C_done=true;
   digitalWrite(relay_C,LOW);
 }  

if (A_done==true && alert_A== false)
{ 
  status_A=100;
  beep(2,80);
  alert_A=true;
  }
if (B_done==true && alert_B== false)
{ 
  status_B=100;
  beep(2,80);
  alert_B=true;
  }
    
if (C_done==true && alert_C== false)
{ 
  status_C=100;
  beep(2,80);
  alert_C=true;
  }
disp(disp_count);

if(A_done==true && B_done==true && C_done==true)
{  

  lin_A=(capacity_A*100)/start_PA;
  lin_B=(capacity_B*100)/start_PB;
  lin_C=(capacity_C*100)/start_PC;
  
  beep(3,150);
  disp(2);
  busy=false;
  done=true;
  break;
  }
}
while(1)
{
done_disp(done_disp_count);  
}  
}





ISR(PCINT2_vect) 
 {
  unsigned char result = r.process();
  if (result == DIR_NONE ) {
    // do nothing
  }
  else if (result == DIR_CW && busy==false && done==false) {
    current_count += 1;
    cut_off_count += 0.1;
  if (current_count>1000)current_count=0;
  if (cut_off_count>12.0)cut_off_count=0.0;
  change=true;
  }
  else if (result == DIR_CCW && busy==false && done==false) {
  current_count-=1;
  cut_off_count -= 0.1;
  if (current_count<0)current_count=1000;
  if (cut_off_count<0.0)cut_off_count=12.0;
  change=true;
  }
  else if (result == DIR_CW && busy==true && done==false) {
    disp_count+=1;
    if (disp_count>4)disp_count=0;
    }
  else if (result == DIR_CCW && busy==true && done==false) {
  disp_count-=1;
   if (disp_count<0)disp_count=4;
    }

  else if (result == DIR_CW && busy==false && done==true) {
    done_disp_count+=1;
    if (done_disp_count>3)done_disp_count=0;
    }
  else if (result == DIR_CCW && busy==false && done==true) {
  done_disp_count-=1;
   if (done_disp_count<0)done_disp_count=3;
    }
}

void disp(int option)
{
 if (last_option != option ){
  last_option = option;
  beep(1,60);
  lcd.clear();
 }
 
 if(option==0)
 {
 
  if( percent(1) < last_intB1 || percent(2) < last_intB2  ||   percent(3)<last_intB3){
   last_intB1 = percent(1);
   last_intB2 = percent(2);
   last_intB3 = percent(3);   
   lcd.clear();
   }
   
   lcd.setCursor(1,0);
   lcd.print("Battery Level!");
   lcd.setCursor(0,1);   
   lcd.print(percent(1));
   lcd.print("%");
   lcd.setCursor(6,1);
   lcd.print(percent(2));
   lcd.print("%");
   lcd.setCursor(12,1);
   lcd.print(percent(3));
   lcd.print("%");

   
   
 }
else if (option==1){
        
   if ( (volt(1)+0.2)<last_float1 || volt(1)>last_float1+0.2|| (volt(2)+0.2)<last_float2|| volt(2)>last_float2+0.2 || (volt(3)+0.2)<last_float3 || volt(3)>last_float3+0.2)
   {       
   last_float1 = volt(1);
   last_float2 = volt(2);
   last_float3 = volt(3);
   lcd.clear();
     }
   lcd.setCursor(0,0);
   lcd.print("Battery Voltage!");
   lcd.setCursor(1,1);   
   lcd.print(volt(1),1);
   lcd.print("V");
   lcd.setCursor(6,1);
   lcd.print(volt(2),1);
   lcd.print("V");
   lcd.setCursor(11,1);
   lcd.print(volt(3),1);
   lcd.print("V"); 
   
  }
 else if (option==2){
    hour = (millis()-Millis)/3600000;
    minute_factor=hour*3600000;        // this line would prevent a time like "01:54:60" or "0x:0y:60"
    minute = ((millis()-Millis)-minute_factor)/60000;
    second_factor= minute * 60000;  // this line would prevent a time like "01:60:05" or "0x:60:0y"
    second = ((millis()-Millis)-minute_factor-second_factor)/1000;

   if (second != last_second)   // this block would maintain a 1 Hz refresh rate
   {
    last_second = second;
    lcd.clear();
    }
   
   
   lcd.setCursor(5,0);
   lcd.print("Time !");
   lcd.setCursor(4,1); 
   if (hour<10)lcd.print("0");
   lcd.print(hour);
   lcd.print(":");
   if (minute<10)lcd.print("0");
   lcd.print(minute);
   lcd.print(":");
   if (second<10)lcd.print("0"); 
   lcd.print(second);  
 }
 else if (option==3)
 { 
   if(last_capacity_A < capacity_A || last_capacity_B < capacity_B|| last_capacity_C < capacity_C){
   last_capacity_A = percent(1);
   last_capacity_B = capacity_B;
   last_capacity_C = capacity_C;   
   lcd.clear();
   }
   
   lcd.setCursor(0,0);
   lcd.print("Capacity (mAh) !");
   lcd.setCursor(0,1); 
   lcd.print(capacity_A);
   lcd.setCursor(5,1); 
   lcd.print(capacity_B);
   lcd.setCursor(10,1); 
   lcd.print(capacity_C);  
  }
 
 else {
  if( percent(1) < last_int1 || percent(2) < last_int2  ||   percent(3)<last_int3)
  {
   last_int1 = percent(1);
   last_int2 = percent(2);
   last_int3 = percent(3);     
   lcd.clear();
   }
   
   lcd.setCursor(5,0);   
   lcd.print("status");
   lcd.setCursor(0,1);   
   lcd.print(status_A);
   lcd.print("%");
   lcd.setCursor(6,1);
   lcd.print(status_B);
   lcd.print("%");
   lcd.setCursor(12,1);
   lcd.print(status_C);
   lcd.print("%"); 

 }
 //delay(200);
}

void done_disp(int option)
{
   if (last_option != option )
   {
   last_option = option;
   beep(1,60);
   lcd.clear();
     }
   if (option==0){
   lcd.setCursor(0,0);
   lcd.print("Capacity (mAh) !");
   lcd.setCursor(0,1); 
   lcd.print(capacity_A);
   lcd.setCursor(6,1); 
   lcd.print(capacity_B);
   lcd.setCursor(12,1); 
   lcd.print(capacity_C); }
   
   else if (option==1){
   lcd.setCursor(1,0);
   lcd.print("lin estm (mah)");
   lcd.setCursor(0,1); 
   lcd.print(lin_A);
   lcd.setCursor(6,1); 
   lcd.print(lin_B);
   lcd.setCursor(12,1); 
   lcd.print(lin_C); }
   
   else if (option==2){ 
   
   lcd.setCursor(5,0);
   lcd.print("Time !");
   lcd.setCursor(4,1); 
   if (hour<10)lcd.print("0");
   lcd.print(hour);
   lcd.print(":");
   if (minute<10)lcd.print("0");
   lcd.print(minute);
   lcd.print(":");
   if (second<10)lcd.print("0"); 
   lcd.print(second);
   }
   else {
   lcd.setCursor(0,0);
   lcd.print("start percentage");
   lcd.setCursor(0,1); 
   lcd.print(start_PA);
   lcd.print("%");
   lcd.setCursor(6,1); 
   lcd.print(start_PB);
   lcd.print("%");
   lcd.setCursor(12,1); 
   lcd.print(start_PC); 
   lcd.print("%"); 
   }
  
  }

 

int percent(int opt)
{ 
  float maxLvl;
  int x;
  float diff;
  float v=volt(opt);


  maxLvl =full_batt-low_batt;
  diff = v-low_batt;

   if(diff > maxLvl)
    {
     diff = maxLvl; 
      }
    else if(diff<0)
    {
     diff =0; 
      }
      
    x= (100.00*diff)/maxLvl;
    
    if (x>100)
    {
      x==100;
      }
     else if(x<=0)
     {
      x=0;
      }
 
return x;  
  }

float volt(int opt){

 int channel;
 if(opt==1)channel=batt_A;
 else if (opt==2)channel=batt_B;
 else if (opt==3)channel=batt_C;
 float analog_vol;
 float div_vol;
 float sample=0;

 for(int i=0;i< 100;i++)
  {
   sample=sample+analogRead(channel); 
   delayMicroseconds(50);
   }
  sample=sample/100; 
  analog_vol = sample / 204.8; 
  div_vol=   (analog_vol * (r_A + r_B)) / r_A;
 return div_vol;
}

void beep(int count, int d)
{
 for(int i=0;i<count;i++)
{
 digitalWrite(buzzer,HIGH);
  delay(d); 
 digitalWrite(buzzer,LOW);
 delay(80);
  }
 digitalWrite(buzzer,LOW);
  }
