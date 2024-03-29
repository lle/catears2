#include <Wire.h>
#include <Servo.h> 
#define CTRL_REG1 0x20
#define CTRL_REG2 0x21
#define CTRL_REG3 0x22
#define CTRL_REG4 0x23
#define CTRL_REG5 0x24

int L3G4200D_Address = 105; //I2C address of the L3G4200D
int x;
int y;
int z;
int threshold = 50;

Servo myservo;
int pos = 90;
int posMin = 100;
int posMax = 180;
int posChange = 5;

void setup()
{
  // INITIAL SETUP
  pinMode(2, INPUT);
  digitalWrite(2, HIGH);
  pinMode(3, OUTPUT); 
  Serial.begin(9600);
  for(int i=0; i<3; i++)
  {
    digitalWrite(3, HIGH);
    delay(100);
    digitalWrite(3, LOW);
    delay(100);
  }
  Serial.begin(9600);
  
  //SERVO SETUP
  myservo.attach(9);
  
  //GYRO SETUP
  Wire.begin();
  Serial.println("Test Servo");  
  testAngle();
  
  Serial.println("starting up L3G4200D");
  setupL3G4200D(2000); // Configure L3G4200  - 250, 500 or 2000 deg/sec
  Serial.println("Start Loop");
}

void loop(){
  //Serial.println("In Loop");
  getGyroValues();  // This will update x, y, and z with new values
  //Serial.println("Output Data");
  outputData();
  //Serial.println("Read Button and Adjust Servo");
  if(digitalRead(2)==LOW)
  {
    digitalWrite(3, HIGH);
    setServo();
  }
  else
  {
    digitalWrite(3, LOW);
  }
  Serial.println("End of Loop");
  delay(25); //Just here to slow down the serial to make it more readable
}

void testAngle()
{ 
  for(int i = posMin; i<posMax; i++)
  {
    Serial.println("a");
    myservo.write(i);
    delay(25);
  }
  for(int j = posMax; j>posMin; j--)
  {
    Serial.println("b");
    myservo.write(j);
    delay(25);
  }
}

void outputData()
{
  Serial.print("X:");
  Serial.print(x);

  Serial.print("\\tY:");
  Serial.print(y);

  Serial.print("\\tZ:");
  Serial.print(z);

  Serial.print("\\tpos:");
  Serial.println(pos); 
}

void setServo()
{
  if(pos>posMax)
  { pos = posMax;}
  if(pos<posMin)
  { pos = posMin;}
  
  if(x>100)
  {
    if(pos<posMax)
    { pos+=posChange;}
    myservo.write(pos);
  } 
  
  if(x<-100)
  {
    if(pos>posMin)
    { pos-=posChange;}
    myservo.write(pos);
  } 
}

void getGyroValues(){

  byte xMSB = readRegister(L3G4200D_Address, 0x29);
  byte xLSB = readRegister(L3G4200D_Address, 0x28);
  x = ((xMSB << 8) | xLSB);

  byte yMSB = readRegister(L3G4200D_Address, 0x2B);
  byte yLSB = readRegister(L3G4200D_Address, 0x2A);
  y = ((yMSB << 8) | yLSB);

  byte zMSB = readRegister(L3G4200D_Address, 0x2D);
  byte zLSB = readRegister(L3G4200D_Address, 0x2C);
  z = ((zMSB << 8) | zLSB);
}

int setupL3G4200D(int scale){
  //From  Jim Lindblom of Sparkfun's code

  // Enable x, y, z and turn off power down:
  writeRegister(L3G4200D_Address, CTRL_REG1, 0b00001111);

  // If you'd like to adjust/use the HPF, you can edit the line below to configure CTRL_REG2:
  writeRegister(L3G4200D_Address, CTRL_REG2, 0b00000000);

  // Configure CTRL_REG3 to generate data ready interrupt on INT2
  // No interrupts used on INT1, if you'd like to configure INT1
  // or INT2 otherwise, consult the datasheet:
  writeRegister(L3G4200D_Address, CTRL_REG3, 0b00001000);

  // CTRL_REG4 controls the full-scale range, among other things:

  if(scale == 250){
    writeRegister(L3G4200D_Address, CTRL_REG4, 0b00000000);
  }else if(scale == 500){
    writeRegister(L3G4200D_Address, CTRL_REG4, 0b00010000);
  }else{
    writeRegister(L3G4200D_Address, CTRL_REG4, 0b00110000);
  }

  // CTRL_REG5 controls high-pass filtering of outputs, use it
  // if you'd like:
  writeRegister(L3G4200D_Address, CTRL_REG5, 0b00000000);
}

void writeRegister(int deviceAddress, byte address, byte val) {
    Wire.beginTransmission(deviceAddress); // start transmission to device 
    Wire.write(address);       // send register address
    Wire.write(val);         // send value to write
    Wire.endTransmission();     // end transmission
}

int readRegister(int deviceAddress, byte address){

    int v;
    Wire.beginTransmission(deviceAddress);
    Wire.write(address); // register to read
    Wire.endTransmission();

    Wire.requestFrom(deviceAddress, 1); // read a byte

    while(!Wire.available()) {
        // waiting
    }

    v = Wire.read();
    return v;
}
}
