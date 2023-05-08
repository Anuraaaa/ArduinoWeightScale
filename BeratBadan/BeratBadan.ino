#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include<HX711_ADC.h>
#include<EEPROM.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int HX711_dout = 4;
const int HX711_sck = 5;

SoftwareSerial mySoftwareSerial(9, 8); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
HX711_ADC LoadCell(HX711_dout, HX711_sck);

const int calVal_eepromAdress = 0;
long t;

void setup() {
  Serial.begin(9600);
  Serial.println(F("Initializing ... (May take 3~5 seconds)"));

  //Initialize LoadCell
  pinMode(HX711_dout, INPUT);  
  LoadCell.begin();
  float calibrationValue; 
  calibrationValue = 696.0; 
  EEPROM.get(calVal_eepromAdress, calibrationValue);
  long stabilizingtime = 2000; 
  boolean _tare = true; 
  LoadCell.start(stabilizingtime, _tare);
  if (LoadCell.getTareTimeoutFlag()) {
    Serial.println("Timeout, cek kabel MCU>HX711 pastikan sudah tepat");
    while (1);
  }
  else {
    LoadCell.setCalFactor(calibrationValue);
    Serial.println("STATUS: LoadCell Connected");
  }

  mySoftwareSerial.begin(9600);
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(1);
  }
  else
  {
    Serial.println(F("STATUS: DFPlayer Mini connected."));
  }
  myDFPlayer.setTimeOut(500); //Set serial communictaion time out 500ms
  
  //----Set volume----
  myDFPlayer.volume(30);  //Set volume value (0~30).
  myDFPlayer.volumeUp(); //Volume Up
  myDFPlayer.volumeDown(); //Volume Down
  
  //----Set different EQ----
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
//  myDFPlayer.EQ(DFPLAYER_EQ_POP);
//  myDFPlayer.EQ(DFPLAYER_EQ_ROCK);
//  myDFPlayer.EQ(DFPLAYER_EQ_JAZZ);
//  myDFPlayer.EQ(DFPLAYER_EQ_CLASSIC);
//  myDFPlayer.EQ(DFPLAYER_EQ_BASS);
  
  //----Set device we use SD as default----
//  myDFPlayer.outputDevice(DFPLAYER_DEVICE_U_DISK);
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
 
//  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
//    Serial.println(F("STATUS: LCD SSD1306 allocation failed"));
//    while(1);
//  }
//  else
//  {
//    Serial.println(F("STATUS: LCD SSD1306 online"));   
//  }  
//  display.clearDisplay();
//  display.setTextSize(2);
//  display.setTextColor(WHITE);
//  display.setCursor(0, 0);
//  // Display static text
//  display.print("Berat: ");
//  display.setCursor(0, 20);
//  display.print("0 kg");
//  display.display();
}

void loop() {  
  static boolean newDataReady = 0;
  const int serialPrintInterval = 3000; 
  if (LoadCell.update()) newDataReady = true;
  if (newDataReady) {
    if (millis() > t + serialPrintInterval) {
      float i = LoadCell.getData();
      if(i<0){
        i=0;
      }
      tampil(i);
      newDataReady = 0;
      t = millis();
    }
  }
  if(Serial.available() > 0){
    float i;
    char inByte = Serial.read();
    if (inByte == 't') LoadCell.tareNoDelay();
  }
  if (LoadCell.getTareStatus() == true) {
    Serial.println("Tara selesai");
  }
}

void tampil(float j){
  
  if (j >= 1000)
  {
    float berat = j / 1000; 
    int beratAsli = berat;
//    display.clearDisplay();
//    display.setTextSize(2);
//    display.setTextColor(WHITE);
//    display.setCursor(0, 0);
//    // Display static text
//    display.print("Berat: ");
//    display.setCursor(0, 20);
//    display.print(beratAsli);
//    display.print(" kg");
//    display.display();

    Serial.print("Berat: ");
    Serial.print(beratAsli);
    Serial.println(" kg");  

    suaraBeratBadan(beratAsli);

  }
  else
  {
//    display.clearDisplay();
//    display.setTextSize(2);
//    display.setTextColor(WHITE);
//    display.setCursor(0, 0);
//    // Display static text
//    display.print("Berat: ");
//    display.setCursor(0, 20);
//    display.print("0 kg");
//    display.display();    
    Serial.println("Berat: 0 kg");  
  }
}

void suaraBeratBadan(uint32_t value)
{
    if (myDFPlayer.available())
    {
      if (value > 0 && value < 10)
      {
          myDFPlayer.playMp3Folder(value);
          delay(1000);
          myDFPlayer.playMp3Folder(13);
      }
      else if (value == 10)
      {
          myDFPlayer.playMp3Folder(10);                      
          delay(1000);              
          myDFPlayer.playMp3Folder(11);              
          delay(1000);
          myDFPlayer.playMp3Folder(13);
      }
      else if (value > 10 && value < 20)
      {
        if (value == 11)
        {
          myDFPlayer.playMp3Folder(10);              
          delay(1000);              
          myDFPlayer.playMp3Folder(12);                          
          delay(1000);
          myDFPlayer.playMp3Folder(13);
        }
        else
        {
          char str[2];
          sprintf(str, "%d", value);
          int satuan = str[1] - '0';
          myDFPlayer.playMp3Folder(satuan);
          delay(1000);              
          myDFPlayer.playMp3Folder(12);                                    
          delay(1000);
          myDFPlayer.playMp3Folder(13);
        }
      }
      else if (value == 20)
      {
          myDFPlayer.playMp3Folder(2);              
          delay(1000);              
          myDFPlayer.playMp3Folder(11);                                  
          delay(1000);
          myDFPlayer.playMp3Folder(13);
      }
      else if (value > 20)
      {
          char str[2];
          sprintf(str, "%d", value);
          int satuanAwal = str[0] - '0';
          myDFPlayer.playMp3Folder(satuanAwal);              
          delay(1000);              
          myDFPlayer.playMp3Folder(11);                                  
          delay(1000);              
          int satuanAkhir = str[1] - '0';
          myDFPlayer.playMp3Folder(satuanAkhir);              
          delay(1000);
          myDFPlayer.playMp3Folder(13);
      }
    }
    delay(1000);
}
