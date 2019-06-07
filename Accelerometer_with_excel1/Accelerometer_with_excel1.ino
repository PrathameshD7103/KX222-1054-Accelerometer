#include <SPI.h>

int READ = B10000000;     
int WRITE = B00000000;

const int chipSelectPin = 53;
const int sdi = 51;
const int sdo = 50;

void setup() {
  Serial.begin(9600);
  Serial.println("CLEARDATA");
  Serial.println("LABEL,TIME,TIMER,X low,X high,Y low,Y high,Z low,Z high");
  Serial.println("RESETTIMER");
  SPI.begin();
  pinMode(chipSelectPin, OUTPUT);
  digitalWrite(chipSelectPin, HIGH);
  
}
void loop() {
  //Select High Resolution Mode
    writeRegister(0x3B, B11100011);
    writeRegister(0x1B, B00000101);
    writeRegister(0x18, B11000000);//+-8g
    Serial.print("DATA,TIME,TIMER," );
    Serial.print(readRegister(0x06)* 0.0024);//X Low
    Serial.print(",");
    Serial.print(readRegister(0x07) * 0.0024);// X High
    Serial.print(",");
    Serial.print(readRegister(0x08)* 0.0024);// Y Low
    Serial.print(",");
    Serial.print(readRegister(0x09)* 0.0024);//Y High
    Serial.print(",");
    Serial.print(readRegister(0x0A)* 0.0024);//Z Low
    Serial.print(",");
    Serial.println(readRegister(0x0B)* 0.0024);//Z High
}
int readRegister(int registerAddress){
//, intlongsToRead)
   //intinByte= 0;
   int result = 0;
   int dataToSend = READ + registerAddress;
   // take the chip select low to select the device:
   digitalWrite(chipSelectPin, LOW);
   // send the device the register you want to read:
   SPI.transfer(dataToSend);
   result=SPI.transfer(0x00);
   digitalWrite(chipSelectPin, HIGH);
   return(result);
}

void writeRegister(int registerAddress, int value) {
  //now combine the register address and the command longo one long:
  int dataToSend = WRITE + registerAddress;

  digitalWrite(chipSelectPin, LOW);

  SPI.transfer(dataToSend); 
  SPI.transfer(value);  
  digitalWrite(chipSelectPin, HIGH);
  delay(1500);
}
