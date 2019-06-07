/*
 * This program can record g values for +- 8,16,32 gs but only at 1 ODR. 
 * For +-8g give the serial input as 1 and for +-16g and +-32g , 2 and 3 respectively. 
 * In this program the acceleration data is acquired by accessing the X,Y and Z registers then converting the 2 8-bit register values (HIGH and LOW) to a single 16-bit value  
 * then the 16-bit is multiplied with the resoective resolution of the G mode selected by choosing from 1,2,3. 
 */


#include <SPI.h>

int READ = B10000000;     
int WRITE = B00000000;
char rx_mode = 0;
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
   writeRegister(0x18,B00000000);
   writeRegister(0x3B,B11100000);                                                    //Buffer resolution is set to 16-bit and buffer operation mode selected is FILO
   writeRegister(0x1B,B00000111);                                                    //6400hz,For 12800 change last four bits to 1110 and for 25600 change last four bits to 1111
   writeRegister(0x1F,B01010000);                                                    //Buffer full interrupt,data ready interrupt enabled
   writeRegister(0x1C,B00111000);                                                    //Physical interrupt pin enabled
   
   if (Serial.available() > 0) {    
       if(rx_mode=='1'){
        Serial.println("G Range:+-8 ");
        writeRegister(0x18, B11100000);                                              //CNTL1 register is set to record values for +-8g 
        printvalues();
        while(1){
        xaxis(readRegister(0x07),readRegister(0x06));                                //X axis values
        yaxis(readRegister(0x09),readRegister(0x08));                                //Y axis values
        zaxis(readRegister(0x0B),readRegister(0x0A));                                //Z axis values
        Serial.println("Buffer Values");                                             //Reading Buffer  
        Serial.println(readRegister(0x3F),BIN);
        }
        }
    
       if(rx_mode=='2'){
        Serial.print("G Range:+-16 ");
        writeRegister(0x18, B11101000);                                              //CNTL1 register is set to record values for +-16g 
        printvalues();
        while(1){
        xaxis(readRegister(0x07),readRegister(0x06));
        yaxis(readRegister(0x09),readRegister(0x08));
        zaxis(readRegister(0x0B),readRegister(0x0A));
        Serial.println("Buffer Values");
        Serial.println(readRegister(0x3F),BIN);
        }
        }
     
      if(rx_mode=='3'){
        Serial.print("G Range:+-32 ");
        writeRegister(0x18, B11110000);                                              //CNTL1 register is set to record values for +-32g 
        printvalues();
        while(1){
        xaxis(readRegister(0x07),readRegister(0x06));
        yaxis(readRegister(0x09),readRegister(0x08));
        zaxis(readRegister(0x0B),readRegister(0x0A));
        Serial.println("Buffer Values");
        Serial.println(readRegister(0x3F),BIN);
        } 
        }
}
}

void xaxis(int xhigh,int xlow){                                                      //This function block calculates the acceleration values of x-axis movement in g as well as m/(sec^2)
  float x_g=0;
  float x = (xhigh*256) + xlow;                                                      //This is for getting 1 16-bit value from 2 8-bit values
  if(rx_mode=='1'){
    x_g=x*0.00024;                                                                   //Resolution for calculation of +-8g mode
  }
  if(rx_mode=='2'){
    x_g=x*0.00049;                                                                   //Resolution for calculation of +-16g mode
  }
  if(rx_mode=='3'){
    x_g=x*0.00098;                                                                   //Resolution for calculation of +-32g mode
  }
  float x_acc=x_g* 9.80665;                                                          //Multiplying the value of g which is taken as 9.80665
  Serial.print(x_g); Serial.print("g");
  Serial.print("   "); Serial.print(x_acc); Serial.print("m/s^2");
}

void yaxis(int yhigh,int ylow){                                                      //This function block calculates the acceleration values of Y-axis movement in g as well as m/(sec^2)
  float y_g=0;
  float y = (yhigh*256) + ylow;
  if(rx_mode=='1'){
    y_g=y*0.00024;
  }
  if(rx_mode=='2'){
    y_g=y*0.00049;
  }
  if(rx_mode=='3'){
    y_g=y*0.00098;
  }
  float y_acc=y_g* 9.80665;
  Serial.print("   "); Serial.print(y_g); Serial.print("g");
  Serial.print("   "); Serial.print(y_acc); Serial.print("m/s^2");
}

void zaxis(int zhigh,int zlow){                                                      //This function block calculates the acceleration values of Z-axis movement in g as well as m/(sec^2)
  float z_g=0;
  float z = (zhigh*256) + zlow;
  if(rx_mode=='1'){
    z_g=z*0.00024;
  }
  if(rx_mode=='2'){
    z_g=z*0.00049;
  }
  if(rx_mode=='3'){
    z_g=z*0.00098;
  }
  float z_acc=z_g* 9.8065;
  Serial.print("   "); Serial.print(z_g); Serial.print("g");
  Serial.print("   "); Serial.print(z_acc); Serial.print("m/s^2");
  Serial.println(""); 
}

void printvalues(){                                                                  //This Function block is for notations of Axes and the type of reading i.e., g or m/(sec^2)
    Serial.print("X (g)   "); 
    Serial.print("X (m/s^2)   ");
    Serial.print("Y (g)   "); 
    Serial.print("Y (m/s^2)   ");
    Serial.print("Z (g)   "); 
    Serial.print("Z (m/s^2)   ");
    Serial.println(" ");
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
