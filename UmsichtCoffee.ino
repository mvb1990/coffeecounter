

/*  Code für ein RFID-basiertes Abrechnungssystem mit LED-Anzeige
 * 
 */

/*Pakete einbinden*/
#include <LiquidCrystal.h>
#include <SPI.h>
#include <SdFat.h>
#include <MFRC522.h>

/*RFID aufsetzen*/
#define MFRC_SS_PIN 10
#define MFRC_RST_PIN 9
#define SD_SS_PIN 8

MFRC522 mfrc522(MFRC_SS_PIN, MFRC_RST_PIN);
MFRC522::MIFARE_Key key;

/*LCD aufsetzen*/
LiquidCrystal lcd(7, 6, 5, 4, 3, 2); 

/*SD aufsetzen*/
SdFat sd;
//File myFile;

/*Datenstrukturen aufsetzen*/
byte nuidPICC[4];
long cardNr;
long HashNr;
int newHash;

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

void checkButton(){
  buttonState=digitalRead(A4);

  if((buttonState==HIGH)&&(c==0)){
    c=1;
    current_beverage="Kaffee m. Milch";
    welcomeScreen();
    delay(400);
  }
  else if((buttonState==HIGH)&&(c==1)){
    current_beverage="Milch";
    welcomeScreen();
    c=2;
    delay(400);
  }
  else if((buttonState==HIGH)&&(c==2)){
    c=3;
    current_beverage="Zusammenfassung";
    welcomeScreen();    
    delay(400);
  }
    else if((buttonState==HIGH)&&(c==3)){
    c=0;
    current_beverage="Kaffee schwarz";
    welcomeScreen();    
    delay(400);
  }
}
void Bev2File(int beverage, File thisFile, int pos){  
  sprintf(sdbuffer,"%4d;",beverage);
  Serial.println("Auf SD geschrieben:");
  Serial.println(sdbuffer);
  thisFile.seek(pos);
  thisFile.write(sdbuffer);
}
void welcomeScreen(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Willkommen!");
  lcd.setCursor(0,1);
  lcd.print(current_beverage+"?");
}

void setup() { 

  //Serial.begin(9600);
  SPI.begin(); // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 
  lcd.begin(16, 2);
  
  if(!sd.begin(SD_SS_PIN, SPI_HALF_SPEED)){
    lcd.clear();
    lcd.print("not working");
    while(!sd.begin(SD_SS_PIN, SPI_HALF_SPEED)){
    }
  }
  
  pinMode(A0, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A2, OUTPUT);

  pinMode(A4, INPUT);
  c=0;  
  current_beverage="Kaffee schwarz";
  welcomeScreen();

  //while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  //}
}
 
void loop() {

  /*checkButton();

  // Look for new cards
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been read
  if ( ! rfid.PICC_ReadCardSerial())
    return;

  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  
  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    return;
  }

    // Store NUID into nuidPICC array
    for (byte i = 0; i < 4; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Karte erkannt!");


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
    
    while(myFile.available()&&sdsearch==true) {
      buffer=myFile.readStringUntil(';');
      if(HashNr==buffer.toInt()){
      
        Serial.println("Hash gefunden!");
        
        user_name=myFile.readStringUntil(';');
        position_coffeeblack=myFile.position();
        user_coffeeblack=myFile.readStringUntil(';').toInt();
        position_coffeeblonde=myFile.position();
        user_coffeeblonde=myFile.readStringUntil(';').toInt();
        position_milk=myFile.position();
        user_milk=myFile.readStringUntil(';').toInt();

        Serial.println(user_name);
        Serial.println(user_coffeeblack);
        Serial.println(user_coffeeblonde);
        Serial.println(user_milk);

        Serial.println(position_coffeeblack);
        Serial.println(position_coffeeblonde);
        Serial.println(position_milk);

        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Hallo "+user_name);
        delay(2000);
        
        if(c==0){
          user_coffeeblack++;
        }else if(c==1){
          user_coffeeblonde++;
        }else if(c==2){
          user_milk++;
        }else if(c==3){
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Kaffee schwarz:");
          lcd.setCursor(0,1);
          lcd.print(user_coffeeblack);
          delay(2000);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Kaffee m. Milch:");
          lcd.setCursor(0,1);
          lcd.print(user_coffeeblonde);
          delay(2000);
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Milch:");
          lcd.setCursor(0,1);
          lcd.print(user_milk);
          delay(2000);
        }
        
        Serial.println(user_coffeeblack);
        Serial.println(user_coffeeblonde);
        Serial.println(user_milk);

        isKnown = true;
        sdsearch = false;
      }
      if(buffer=="ENDE"){
        sdsearch = false;
      }
    }

    myFile.close();
    if(c!=3){
    myFile = SD.open("kaffee.txt",FILE_WRITE);
    
    Bev2File(user_coffeeblack, myFile, position_coffeeblack);
    Bev2File(user_coffeeblonde, myFile, position_coffeeblonde);
    Bev2File(user_milk, myFile, position_milk);

    Serial.println("Finished Data Transfer");

    myFile.close();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(current_beverage);
    lcd.setCursor(0,1);
    lcd.print("wurde gebucht!");
    delay(2000);
    }
    if(isKnown==false){
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Karten-ID:"); 
      lcd.setCursor(0, 1);
      lcd.print(HashNr,DEC);
      delay(2000);
    }

  welcomeScreen();
    
  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();*/

}
