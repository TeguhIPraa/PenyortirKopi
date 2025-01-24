#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL6vvnOjByU"
#define BLYNK_TEMPLATE_NAME "penyortir biji kopi"
#define BLYNK_AUTH_TOKEN "ne2IjE3vw630ByGkq5JcUGmeKdlMhEWR"

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "EmonLib.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "HX711.h"

const int LOADCELL1_DOUT_PIN  = 19;
const int LOADCELL1_SCK_PIN  = 18;
const int LOADCELL2_DOUT_PIN  = 32;
const int LOADCELL2_SCK_PIN  = 33;
const int LOADCELL3_DOUT_PIN  = 25;
const int LOADCELL3_SCK_PIN  = 26;
const int trigPin = 13; 
const int echoPin = 12; 
#define relay 23 

#define CALIBRATION_FACTOR1 444.925
#define CALIBRATION_FACTOR2 467.85
#define CALIBRATION_FACTOR3 475.24

#define vCalibration 93.3
#define currCalibration 0.50

HX711 scale1;
HX711 scale2;
HX711 scale3;

#define SOUND_VELOCITY 0.034

LiquidCrystal_I2C lcd(0x27, 20, 4);
EnergyMonitor emon;
 
BlynkTimer timer;

char ssid[] = "ISTIANA BENGKEL";
char pass[] = "isriana123";
 
float kWh = 0;
unsigned long lastmillis = millis();
long duration;
float distanceCm;
unsigned long WaktuMulaiDeteksiKopi = 0;
unsigned long WaktuMulaiKopiTidakTerdeteksi = 0;
bool kopiTerdeteksi = false;
bool waktuAktif = false;

void myTimerEvent()
{
  emon.calcVI(20, 2000);
  kWh = kWh + emon.apparentPower * (millis() - lastmillis) / 3600000000.0;
  yield();
  lastmillis = millis();
  Serial.print("Vrms: ");
  Serial.print(emon.Vrms, 2);
  Serial.print("V");
 
  Serial.print("\tIrms: ");
  Serial.print(emon.Irms, 4);
  Serial.print("A");
 
  Serial.print("\tPower: ");
  Serial.print(emon.apparentPower, 4);
  Serial.print("W");
 
  Serial.print("\tkWh: ");
  Serial.print(kWh, 5);
  Serial.println("kWh");
  
  // Update data ke Blynk
  Blynk.virtualWrite(V2, emon.Vrms);
  Blynk.virtualWrite(V3, emon.Irms);
  Blynk.virtualWrite(V4, emon.apparentPower);
  Blynk.virtualWrite(V5, kWh);
}

void DeteksiKopi(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distanceCm = duration * SOUND_VELOCITY/2;
  Serial.print("Jarak : ");
  Serial.println(distanceCm);

   if (distanceCm <= 20) {
    if(!kopiTerdeteksi){
      WaktuMulaiDeteksiKopi = millis();
      kopiTerdeteksi = true;
      waktuAktif = false;
    }
    digitalWrite(relay, HIGH);
    lcd.setCursor(0,3);
    lcd.print("                    ");
    lcd.setCursor(0,3);
    lcd.print("Ada kopi");
    else{
      if (kopiTerdeteksi)
      WaktuMulaiKopiTidakTerdeteksi = millis();
      kopiTerdeteksi = false;
    }
    if ((millis() - WaktuMulaiDeteksiKopi >= 30000) && !waktuAktif){
      waktuAktif = true;
      WaktuMulaiKopiTidakTerdeteksi = millis();
    }
    if (waktuAktif && milis() - WaktuMulaiKopiTidakTerdeteksi >=60000){
      digitalWrite(relay, LOW);
      waktuAktif = false;
    }
    lcd.setCursor(0, 3);
    lcd.print("                    ");
    lcd.setCursor(0, 3);
    lcd.print("Tidak Ada Kopi");
}

void Loudcells(){
  if (scale1.is_ready()) {
    long TimbanganGrade1 = scale1.get_units();
    Serial.print("HX711 #1 reading: ");
    Serial.println(TimbanganGrade1);
    Blynk.virtualWrite(V6, TimbanganGrade1);
    lcd.setCursor(0, 0);
    lcd.print("                "); // Membersihkan seluruh baris
    lcd.setCursor(0, 0);
    lcd.print("Grade 1 :"); 
    lcd.setCursor(11, 0);
    lcd.print(TimbanganGrade1); 
    lcd.setCursor(17, 0);
    lcd.print("gr");
  } else {
    Serial.println("HX711 #1 not found.");
    lcd.setCursor(0, 0);
    lcd.print("                "); // Membersihkan seluruh baris
    lcd.setCursor(0, 0);
    lcd.print("HX711 #1 not found."); 
  }

  if (scale2.is_ready()) {
    long TimbanganGrade2 = scale2.get_units();
    Serial.print("HX711 #2 reading: ");
    Serial.println(TimbanganGrade2);
    Blynk.virtualWrite(V7, TimbanganGrade2);
    lcd.setCursor(0, 1);
    lcd.print("                    "); // Membersihkan seluruh baris
    lcd.setCursor(0, 1);
    lcd.print("Grade 2 :"); 
    lcd.setCursor(11, 1);
    lcd.print(TimbanganGrade2); 
    lcd.setCursor(17, 1);
    lcd.print("gr");
  } else {
    Serial.println("HX711 #2 not found.");
    lcd.setCursor(0, 1);
    lcd.print("                    "); // Membersihkan seluruh baris
    lcd.setCursor(0, 1);
    lcd.print("HX711 #2 not found."); 
  }

  if (scale3.is_ready()) {
    long TimbanganGrade3 = scale3.get_units();
    Serial.print("HX711 #3 reading: ");
    Serial.println(TimbanganGrade3);
    Blynk.virtualWrite(V8, TimbanganGrade3);
    lcd.setCursor(0, 2);
    lcd.print("                    "); // Membersihkan seluruh baris
    lcd.setCursor(0, 2);
    lcd.print("Grade 3 :"); 
    lcd.setCursor(11, 2);
    lcd.print(TimbanganGrade3); 
    lcd.setCursor(17, 2);
    lcd.print("gr");
  } else {
    Serial.println("HX711 #3 not found.");
    lcd.setCursor(0, 2);
    lcd.print("                    "); // Membersihkan seluruh baris
    lcd.setCursor(0, 2);
    lcd.print("HX711 #3 not found."); 
  }
}
 
void setup()
{
  Serial.begin(9600);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(relay, OUTPUT);

  scale1.begin(LOADCELL1_DOUT_PIN, LOADCELL1_SCK_PIN);
  scale2.begin(LOADCELL2_DOUT_PIN, LOADCELL2_SCK_PIN);
  scale3.begin(LOADCELL3_DOUT_PIN, LOADCELL3_SCK_PIN);
  lcd.init();
  emon.voltage(35, vCalibration, 1.7); // Voltage: input pin, calibration, phase_shift
  emon.current(34, currCalibration); // Current: input pin, calibration.

  scale1.set_scale(CALIBRATION_FACTOR1);
  scale2.set_scale(CALIBRATION_FACTOR2);
  scale3.set_scale(CALIBRATION_FACTOR3);

  scale1.tare();
  scale2.tare();
  scale3.tare();
  
  timer.setInterval(2000L, myTimerEvent); // Perbarui data listrik setiap 2 detik
  timer.setInterval(2000L, DeteksiKopi); // Perbarui data jarak setiap 2 detik
  timer.setInterval(2000L, Loudcells); //Perbarui data Timabangan setiap 2 detik
  lcd.backlight();
  lcd.setCursor(4,0);
  lcd.print("Sortir  Kopi");
  lcd.setCursor(4,1);
  lcd.print("Berbasis IoT!");
  lcd.setCursor(5,2);
  lcd.print("Power By :");
  lcd.setCursor(2,3);
  lcd.print("ISTech  Dynamics");
  delay(5000);
  lcd.clear();
}
 
void loop(){
  Blynk.run();
  timer.run();
}#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL6vvnOjByU"
#define BLYNK_TEMPLATE_NAME "penyortir biji kopi"
#define BLYNK_AUTH_TOKEN "ne2IjE3vw630ByGkq5JcUGmeKdlMhEWR"

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "EmonLib.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "HX711.h"

const int LOADCELL1_DOUT_PIN  = 19;
const int LOADCELL1_SCK_PIN  = 18;
const int LOADCELL2_DOUT_PIN  = 32;
const int LOADCELL2_SCK_PIN  = 33;
const int LOADCELL3_DOUT_PIN  = 25;
const int LOADCELL3_SCK_PIN  = 26;
const int trigPin = 13; 
const int echoPin = 12; 
#define relay 23 

#define CALIBRATION_FACTOR1 444.925
#define CALIBRATION_FACTOR2 467.85
#define CALIBRATION_FACTOR3 475.24

#define vCalibration 93.3
#define currCalibration 0.50

HX711 scale1;
HX711 scale2;
HX711 scale3;

#define SOUND_VELOCITY 0.034

LiquidCrystal_I2C lcd(0x27, 20, 4);
EnergyMonitor emon;
 
BlynkTimer timer;

char ssid[] = "ISTIANA BENGKEL";
char pass[] = "isriana123";
 
float kWh = 0;
unsigned long lastmillis = millis();
long duration;
float distanceCm;

void myTimerEvent()
{
  emon.calcVI(20, 2000);
  kWh = kWh + emon.apparentPower * (millis() - lastmillis) / 3600000000.0;
  yield();
  lastmillis = millis();
  Serial.print("Vrms: ");
  Serial.print(emon.Vrms, 2);
  Serial.print("V");
 
  Serial.print("\tIrms: ");
  Serial.print(emon.Irms, 4);
  Serial.print("A");
 
  Serial.print("\tPower: ");
  Serial.print(emon.apparentPower, 4);
  Serial.print("W");
 
  Serial.print("\tkWh: ");
  Serial.print(kWh, 5);
  Serial.println("kWh");
  
  // Update data ke Blynk
  Blynk.virtualWrite(V2, emon.Vrms);
  Blynk.virtualWrite(V3, emon.Irms);
  Blynk.virtualWrite(V4, emon.apparentPower);
  Blynk.virtualWrite(V5, kWh);
}

void DeteksiKopi(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distanceCm = duration * SOUND_VELOCITY/2;
  Serial.print("Jarak : ");
  Serial.println(distanceCm);

   if (distanceCm > 20) {
    digitalWrite(relay, LOW);
    lcd.setCursor(0, 3);
    lcd.print("                "); // Membersihkan seluruh baris
    lcd.setCursor(0, 3);
    lcd.print("Tidak ada Kopi");
  } else {
    digitalWrite(relay, HIGH);
    lcd.setCursor(0, 3);
    lcd.print("                "); // Membersihkan seluruh baris
    lcd.setCursor(0, 3);
    lcd.print("Ada Kopi");
  }
}

void Loudcells(){
  if (scale1.is_ready()) {
    long TimbanganGrade1 = scale1.get_units();
    Serial.print("HX711 #1 reading: ");
    Serial.println(TimbanganGrade1);
    Blynk.virtualWrite(V6, TimbanganGrade1);
    lcd.setCursor(0, 0);
    lcd.print("                "); // Membersihkan seluruh baris
    lcd.setCursor(0, 0);
    lcd.print("Grade 1 :"); 
    lcd.setCursor(11, 0);
    lcd.print(TimbanganGrade1); 
    lcd.setCursor(17, 0);
    lcd.print("gr");
  } else {
    Serial.println("HX711 #1 not found.");
    lcd.setCursor(0, 0);
    lcd.print("                "); // Membersihkan seluruh baris
    lcd.setCursor(0, 0);
    lcd.print("HX711 #1 not found."); 
  }

  if (scale2.is_ready()) {
    long TimbanganGrade2 = scale2.get_units();
    Serial.print("HX711 #2 reading: ");
    Serial.println(TimbanganGrade2);
    Blynk.virtualWrite(V7, TimbanganGrade2);
    lcd.setCursor(0, 1);
    lcd.print("                    "); // Membersihkan seluruh baris
    lcd.setCursor(0, 1);
    lcd.print("Grade 2 :"); 
    lcd.setCursor(11, 1);
    lcd.print(TimbanganGrade2); 
    lcd.setCursor(17, 1);
    lcd.print("gr");
  } else {
    Serial.println("HX711 #2 not found.");
    lcd.setCursor(0, 1);
    lcd.print("                    "); // Membersihkan seluruh baris
    lcd.setCursor(0, 1);
    lcd.print("HX711 #2 not found."); 
  }

  if (scale3.is_ready()) {
    long TimbanganGrade3 = scale3.get_units();
    Serial.print("HX711 #3 reading: ");
    Serial.println(TimbanganGrade3);
    Blynk.virtualWrite(V8, TimbanganGrade3);
    lcd.setCursor(0, 2);
    lcd.print("                    "); // Membersihkan seluruh baris
    lcd.setCursor(0, 2);
    lcd.print("Grade 3 :"); 
    lcd.setCursor(11, 2);
    lcd.print(TimbanganGrade3); 
    lcd.setCursor(17, 2);
    lcd.print("gr");
  } else {
    Serial.println("HX711 #3 not found.");
    lcd.setCursor(0, 2);
    lcd.print("                    "); // Membersihkan seluruh baris
    lcd.setCursor(0, 2);
    lcd.print("HX711 #3 not found."); 
  }
}
 
void setup()
{
  Serial.begin(9600);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  pinMode(relay, OUTPUT);

  scale1.begin(LOADCELL1_DOUT_PIN, LOADCELL1_SCK_PIN);
  scale2.begin(LOADCELL2_DOUT_PIN, LOADCELL2_SCK_PIN);
  scale3.begin(LOADCELL3_DOUT_PIN, LOADCELL3_SCK_PIN);
  lcd.init();
  emon.voltage(35, vCalibration, 1.7); // Voltage: input pin, calibration, phase_shift
  emon.current(34, currCalibration); // Current: input pin, calibration.

  scale1.set_scale(CALIBRATION_FACTOR1);
  scale2.set_scale(CALIBRATION_FACTOR2);
  scale3.set_scale(CALIBRATION_FACTOR3);

  scale1.tare();
  scale2.tare();
  scale3.tare();
  
  timer.setInterval(2000L, myTimerEvent); // Perbarui data listrik setiap 2 detik
  timer.setInterval(2000L, DeteksiKopi); // Perbarui data jarak setiap 2 detik
  timer.setInterval(2000L, Loudcells); //Perbarui data Timabangan setiap 2 detik
  lcd.backlight();
  lcd.setCursor(4,0);
  lcd.print("Sortir  Kopi");
  lcd.setCursor(4,1);
  lcd.print("Berbasis IoT!");
  lcd.setCursor(5,2);
  lcd.print("Power By :");
  lcd.setCursor(2,3);
  lcd.prin
