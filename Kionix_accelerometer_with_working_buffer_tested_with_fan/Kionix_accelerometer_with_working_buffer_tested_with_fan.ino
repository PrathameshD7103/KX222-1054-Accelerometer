/*
 * This program acqruires values and stores them in buffer array.
 * This program collects values according to requirements but ODR and counts need to be set by changing the values of register and the needful changes are mentioned 
 * in line comments.Throughout this program, the datatype used is int to increase the array size to 3800.
 * Changes are mentioned in the excel sheet for G modes and ODRs.
 */

#include <SPI.h>
int READ = B10000000;
int WRITE = B00000000;
const int chipSelectPin = 53;
//const int sdi = 51;
//const int sdo = 50;
const int fifo_size=2970;
//int xar[fifo_size]={};                                                               //Unconmment this line in order to enable x axis buffer array for storing values 
//int yar[fifo_size]={};                                                               //Unconmment this line in order to enable y axis buffer array for storing values
int zar[fifo_size]={};
/*int x=0;int y=0;*/int z=0;
/*int xg=0;int yg=0; */int zg=0;int i=0;

//int indexCount =0;
bool bufferfull = false;


void setup() {
  Serial.begin(115200);
  SPI.begin();
  pinMode(chipSelectPin, OUTPUT);
  digitalWrite(chipSelectPin, HIGH);
  writeRegister(0x18, B0000000);
  writeRegister(0x3B, B11100000);                                                  //Buffer resolution is set to 16-bit and buffer operation mode selected is FILO
  writeRegister(0x1B, B00000111);                                                  //6400hz,For 12800 change last four bits to 1110 and for 25600 change last four bits to 1111
  writeRegister(0x1C, B00111000);                                                  //Physical interrupt pin enabled
  writeRegister(0x1F, B00010000);                                                  //Buffer full interrupt,data ready interrupt enabled
  writeRegister(0x18, B11100000);                                                  //CNTL1 register is set to record values for +-8g.....for +-16g 11101000.....for +-32g 11110000
  
  //noInterrupts();           
  TCCR1A = 0;
  TCCR1B = 0;

  TCNT1 = 64435;                                                                   //Preloading timer 64435/1/6400Hz......64987/1/12800Hz......65293/1/25600Hz
  TCCR1B |= (1 << CS10);                                                           //Prescaler set to 1
  TIMSK1 |= (1 << TOIE1);                                                          //Timer overflow interrupt enabled
  interrupts();   
  }

void loop() {
    if(bufferfull == true){
      noInterrupts();       
      print();
    }
}
 
ISR(TIMER1_OVF_vect)        
{
      TCNT1 = 64435;                                                               //Compare the counter value here ... 64435/1/6400Hz......64987/1/12800Hz......65293/1/25600Hz
//      for(i=0;i<fifo_size;i++){
//        int x = (readRegister(0x07)*256) + readRegister(0x06);
//        int xg = x*0.00024*10000;
//        int y = (readRegister(0x09)*256) + readRegister(0x08);
//        int yg=y*0.00024*10000;                           
         z = (readRegister(0x0B)*256) + readRegister(0x0A);                         //This is for getting 1 16-bit value from 2 8-bit values of LOW and HIGH registers
         zg=z*0.00024*10000;                                                        //As the datatype is not float the value is multiplied by 10k to accomodate 4 decimal places                 
//        xar[i]=xg;
//        yar[i]=yg;
        zar[i]=zg;                                                                     //Filling acceleration values in buffer array
        i++;
//      Serial.println(xar[i]);
//      Serial.println(yar[i]);
//      Serial.println(zar[i]);                                                        //Printing buffer array of z axis readings
        
        //print();
    //  }
      if(i >= fifo_size){
        bufferfull = true;
        //sprint();
      }
}

void print(){
  //noInterrupts();.
  Serial.print("[");
  for(int i=0;i<fifo_size;i++){
    
      Serial.print(zar[i]);Serial.print(",");
    }
    Serial.println("]");
    bufferfull = false;
    // enable ISR 
    interrupts();
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
