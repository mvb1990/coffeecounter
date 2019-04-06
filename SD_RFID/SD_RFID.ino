

/*  Code für ein RFID-basiertes Abrechnungssystem mit LED-Anzeige
 * 
 */

/*Pakete einbinden*/
#include <LiquidCrystal.h>
#include <SPI.h>
#include <SD.h>
#include <MFRC522.h>

/*RFID aufsetzen*/
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key; 

/*LCD aufsetzen*/
LiquidCrystal lcd(7, 6, 5, 4, 3, 2); 

/*SD aufsetzen*/
Sd2Card card;
SdVolume volume;
SdFile root;
const int sdcs = 8;

/*Datenstrukturen aufsetzen*/
byte nuidPICC[4];
long cardNr;
long HashNr;
int newHash;

File myFile;

const int n = 60;

int c;
int buttonState;

char sdbuffer [4];
String buffer;
String user_name;
String current_beverage;

long position_coffeeblack;
long position_coffeeblonde;
long position_milk;

int user_coffeeblack;
int user_coffeeblonde;
int user_milk;

boolean isKnown = false;
boolean sdsearch = true;
int index;

void setup() { 

  pinMode (sdcs, OUTPUT);
  pinMode (SS_PIN, OUTPUT);
  digitalWrite(sdcs, HIGH);
  digitalWrite(SS_PIN, HIGH);

  Serial.begin(9600);
  SPI.begin(); // Init SDI Bus
  mfrc522.PCD_Init(); // Init MFRC522 

  lcd.begin(16, 2);

 if(!SD.begin(sdcs)){
    lcd.clear();
    lcd.print("Datenbank fehlt!");
    while(!SD.begin(sdcs)){
    }
  }

  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);

  pinMode(A4, INPUT);
  digitalWrite(A0,HIGH);

  //while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  //}
}
 
void loop() {

  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()){
    Serial.println("Noch nix...");
    return;
  }
  // Verify if the NUID has been read
  if ( ! mfrc522.PICC_ReadCardSerial())
    return;

  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
  
  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    return;
  }

    // Store NUID into nuidPICC array
    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = mfrc522.uid.uidByte[i];
    }
    
    Serial.println("Karte erkannt!");


    for (byte i = 0; i < 4; i++) {
      if(i==0){
        cardNr = 0;
        newHash= 0;
        HashNr = 0;
      }
      cardNr = cardNr*256 + nuidPICC[i];
      newHash = nuidPICC[i]%7;
      HashNr = HashNr*10+newHash;
    }

    isKnown = false;
    
    myFile = SD.open("kaffee.txt");

    sdsearch = true;
    
    if(myFile.available()&&sdsearch==true) {
          myFile.close();
          Serial.println("Datei erkannt, kaffee.txt vorhanden");
    }
    
  // Halt PICC
  mfrc522.PICC_HaltA();

  // Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();

}
