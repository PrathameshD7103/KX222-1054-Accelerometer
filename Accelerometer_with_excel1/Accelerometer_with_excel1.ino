/* 
This program takes the acquired values of the 3 axes and stores in MS Excel. 
PLX-DAQ has been used for doing this and the values that are acquired in Excel are in g.
*/

#include <SPI.h>

int READ = B10000000;     
int WRITE = B00000000;

const int chipSelectPin = 53;
const int sdi = 51;
const int sdo = 50;

void setup() {
  Serial.begin(115200);
  Serial.println("CLEARDATA");
  Serial.println("LABEL,TIME,TIMER,X low,X high,Y low,Y high,Z low,Z high");
  Serial.println("RESETTIMER");
  SPI.begin();
  pinMode(chipSelectPin, OUTPUT);
  digitalWrite(chipSelectPin, HIGH);
  
}
void loop() {
  //Select High Resolution Mode
    writeRegister(0x3B, 0xE3);
    writeRegister(0x1B, 0x05);
    writeRegister(0x18, 0xC0);//+-8g
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
  int dataToSend = WRITE + registerAddress;
  digitalWrite(chipSelectPin, LOW);
  SPI.transfer(dataToSend); 
  SPI.transfer(value);  
  digitalWrite(chipSelectPin, HIGH);
}
