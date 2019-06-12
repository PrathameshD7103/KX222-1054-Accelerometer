#include <SPI.h>
int READ = B10000000;                                                             //Required to carry out register READ 
int WRITE = B00000000;                                                            //Required to carry out writing of values to registers 
const int chipSelectPin = 53;                                                      
const int sdi = 51;                                                               //Pin connections for SPI from Accelerometer to Arduino
const int sdo = 50;

void setup() {
  SPI.begin();  
  pinMode(chipSelectPin, OUTPUT);
  digitalWrite(chipSelectPin, HIGH);
  writeRegister(0x18,B00000000);
  writeRegister(0x3B,B11100000);                                                   //Buffer resolution is set to 16-bit and buffer operation mode selected is FILO
  writeRegister(0x1F,B01010000);                                                   //Buffer full interrupt,data ready interrupt enabled
  TCNT1 = ;                                                                        //Set the counter value to obtain desired timer interrupts.64435/1/6400Hz.64987/1/12800Hz.65293/1/25600Hz
  TCCR1B |= (1 << CS10);                                                           //Prescaler set to 1
  TIMSK1 |= (1 << TOIE1);
  interrupts();
}

void loop() {
  // put your main code here, to run repeatedly:

}

ISR(TIMER1_OVF_vect)                                                                 //ISR for data acquisition at ODR 
{
  TCNT1=;                                                                            //Match counter here  
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
