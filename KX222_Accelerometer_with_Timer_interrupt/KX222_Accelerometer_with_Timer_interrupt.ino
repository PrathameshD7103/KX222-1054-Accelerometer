/*
In this program the data acquisition for the data is done at the same frequency as that ofthe ODR using
Timer interrupt of Timer 1 of Arduino. In this program the G mode is set and the ODR too , and are not selectable. 
Changes to that can be made by changing the register values and is mentioned in the comments. 
*/



#include <SPI.h>
int READ = B10000000;
int WRITE = B00000000;

const int chipSelectPin = 53;
const int sdi = 51;
const int sdo = 50;

void setup() {
  Serial.begin(115200);
  SPI.begin();
  pinMode(chipSelectPin, OUTPUT);
  digitalWrite(chipSelectPin, HIGH);
  writeRegister(0x3B, B11100000);                                                  //Buffer resolution is set to 16-bit and buffer operation mode selected is FILO
  writeRegister(0x1B, B00000111);                                                  //6400hz,For 12800 change last four bits to 1110 and for 25600 change last four bits to 1111
  writeRegister(0x18, B11100000);                                                  //CNTL1 register is set to record values for +-8g.....for +-16g 11101000.....for +-32g 11110000
  writeRegister(0x1C, B00111000);                                                  //Physical interrupt pin enabled
  writeRegister(0x1F, B00010000);                                                  //Buffer full interrupt,data ready interrupt enabled
  noInterrupts();           
  TCCR1A = 0;
  TCCR1B = 0;

  TCNT1 = 64435;                                                                   //Preloading timer 64435/1/6400Hz......64987/1/12800Hz......65293/1/25600Hz
  TCCR1B |= (1 << CS10);                                                           //Prescaler set to 1
  TIMSK1 |= (1 << TOIE1);                                                          //Timer overflow interrupt enabled
  interrupts();   
  }

void loop() {
 }

ISR(TIMER1_OVF_vect)        
{
    TCNT1 = 64435;             
    Serial.println("X Axis>>>>");                                     
    Serial.println("X HIGH"); 
    long a=Serial.println(readRegister(0x07),BIN);//X Low
    Serial.println("X LOW"); 
    long b=Serial.println(readRegister(0x06),BIN);// X High
    float x= (readRegister(0x07)*256) + readRegister(0x06);                         //This is for getting 1 16-bit value from 2 8-bit values of LOW and HIGH registers
    float xg=x*0.00024;                                                             //Resolution for +-8g.....for +-16g 0.00048.....for +-32g 0.00096
    Serial.print(xg); 
    Serial.println("g");
    float xa=xg* 9.80665;                                                           //G(9.80665) value multiplied to accleration value in g
    Serial.print(xa);
    Serial.println("m/(s)^2");
    Serial.println(" ");
    
    Serial.println("Y Axis>>>>");
    Serial.println("Y HIGH"); 
    float c=Serial.println(readRegister(0x09),BIN);// Y Low
    Serial.println("Y LOW"); 
    float d=Serial.println(readRegister(0x08),BIN);//Y High
    float y= (readRegister(0x09)*256) + readRegister(0x08);
    float yg=y*0.00024;
    Serial.print(yg);
    Serial.println("g");
    float ya = yg * 9.80665;
    Serial.print(ya); 
    Serial.println("m/(s)^2");
    Serial.println(" ");
    
    Serial.println("Z Axis>>>>");
    Serial.println("Z HIGH"); 
    long e=Serial.println(readRegister(0x0B),BIN);
    Serial.println("Z LOW"); 
    long f=Serial.println(readRegister(0x0A),BIN);//Z High
    float z= (readRegister(0x0B)*256) + readRegister(0x0A);
    float zg=z*0.00024;
    Serial.print(zg);
    Serial.println("g");
    float za=zg*9.80665;
    Serial.print(za);
    Serial.println("m/(s)^2");
    Serial.println(" ");

    Serial.println("INS 2: ");
    Serial.println(readRegister(0x13),BIN);
    Serial.println("Status Reg: ");
    Serial.println(readRegister(0x15),BIN);
    Serial.println("Buffer read: ");
    Serial.println(readRegister(0x3F),BIN);
}

int readRegister(int registerAddress){                                               //This function block performs the reading of acceleration values from registers 
   int result = 0;
   int dataToSend = READ + registerAddress;                                          //dataToSend is a prerequisite for data acquisition 
   digitalWrite(chipSelectPin, LOW);                                                 //Slave select pin set to low to initiate SPI transfer
   SPI.transfer(dataToSend);                                                         //send the device the register you want to read
   result=SPI.transfer(0x00);                                     
   digitalWrite(chipSelectPin, HIGH);                                                //Slave select pin set to high to terminate SPI operation
   return(result);                                                                   //Register value received
}

void writeRegister(int registerAddress, int value) {                                 //This function block performs the writing of values to the registers
  int dataToSend = WRITE + registerAddress;                                          //writing a value to register requires first 8-bits to be 0s and then the next bits are the register address
  digitalWrite(chipSelectPin, LOW);                                                  //Slave select pin set to low to initiate SPI transfer
  SPI.transfer(dataToSend);                                      
  SPI.transfer(value);                                                               //Value to be written to the register
  digitalWrite(chipSelectPin, HIGH);                                                 //Slave select pin set to high to terminate SPI operation
}
