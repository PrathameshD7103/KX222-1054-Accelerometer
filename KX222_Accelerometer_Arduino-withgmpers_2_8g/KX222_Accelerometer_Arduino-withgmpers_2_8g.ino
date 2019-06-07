/*
 * This program is for +-8g 16-Bit Res 3 axis readings both in g and m/(sec)^2 of KX222-1054 Accelerometer  
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
}

void loop() {
    //Select High Resolution Mode
    writeRegister(0x3B, 0xE0);                                   //Buffer resolution is 16-bit and buffer operation mode selected is FILO
    writeRegister(0x1B, 0x13);                                   //6400hz,For 12800 change last four bits to 1110 and for 25600 change last four bits to 1111
    writeRegister(0x18, 0xE0);                                   //+-8g, For +-16g change bit 4 and 5 to 01 nad 10 for +-32g 
    writeRegister(0x1C, 0x38);                                   //Physical interrupt register
    writeRegister(0x1F, 0x10);                                   //buffer full interrupt,data ready interrupt
    //Reading the data of axes data
    Serial.println("X Axis>>>>"); 
    Serial.println("X HIGH"); 
    long a=Serial.println(readRegister(0x07),BIN);               //X High register
    Serial.println("X LOW"); 
    long b=Serial.println(readRegister(0x06),BIN);               //X Low register
    float x= (readRegister(0x07)*256) + readRegister(0x06);      //2 8-BIT combined to obtain 1 16-BIT axis reading
    float xg=x*0.00024;                                          //Resolution for 8g mode
    Serial.print(xg);
    Serial.println("g");
    float xa=xg* 9.80665;                                        //g value multiplied
    Serial.print(xa);
    Serial.println("m/(s)^2");
    Serial.println(" ");
    
    Serial.println("Y Axis>>>>");
    Serial.println("Y HIGH"); 
    float c=Serial.println(readRegister(0x09),BIN);              //Y High register
    Serial.println("Y LOW"); 
    float d=Serial.println(readRegister(0x08),BIN);              //Y Low register
    float y= (readRegister(0x09)*256) + readRegister(0x08);      //2 8-BIT combined to obtain 1 16-BIT axis reading
    float yg=y*0.00024;                                          //Resolution for 8g mode
    Serial.print(yg);
    Serial.println("g");
    float ya = yg * 9.80665;                                     //g value multiplied
    Serial.print(ya); 
    Serial.println("m/(s)^2");
    Serial.println(" ");
    
    Serial.println("Z Axis>>>>");
    Serial.println("Z HIGH"); 
    long e=Serial.println(readRegister(0x0B),BIN);               //Z High register
    Serial.println("Z LOW"); 
    long f=Serial.println(readRegister(0x0A),BIN);               //Z Low register
    float z= (readRegister(0x0B)*256) + readRegister(0x0A);      //2 8-BIT combined to obtain 1 16-BIT axis reading
    //Serial.println(z);
    float zg=z*0.00024;                                          //Resolution for 8g mode
    Serial.print(zg);
    Serial.println("g");
    float za=zg*9.80665;                                         //g value multiplied
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
int readRegister(int registerAddress){
   int result = 0;
   int dataToSend = READ + registerAddress;                       //dataToSend is a prerequisite for data acquisition 
   digitalWrite(chipSelectPin, LOW);                              //Slave select pin set to low to initiate SPI transfer
   SPI.transfer(dataToSend);                                      //send the device the register you want to read
   result=SPI.transfer(0x00);                                     
   digitalWrite(chipSelectPin, HIGH);                             //Slave select pin set to high to terminate SPI operation
   return(result);                                                //Register value received
}

void writeRegister(int registerAddress, int value) {              
  int dataToSend = WRITE + registerAddress;                      //writing a value to register requires first 8-bits to be 0s and then the next bits are the register address
  digitalWrite(chipSelectPin, LOW);                              //Slave select pin set to low to initiate SPI transfer
  SPI.transfer(dataToSend);                                      
  SPI.transfer(value);                                           //Value to be written to the register
  digitalWrite(chipSelectPin, HIGH);                             //Slave select pin set to high to terminate SPI operation
}
