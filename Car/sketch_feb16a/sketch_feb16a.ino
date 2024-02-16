#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

Servo myservo;
LiquidCrystal_I2C lcd(0x27, 16, 2);

  int bremse;
  int gas;
  int lenkungval;
  int disconnect;

  int motor1 = 3;
  int motor2 = 5;


void setup() {


  myservo.attach(9);
  disconnect = 0;
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);

/*  while(!Serial.available()){
  }
  Serial.println("ready! ");

/
}




void loop() {

  int gasbremse[3];
  int lenkung[3];


  int werte[4];
  int wert = 0;
  while(!Serial.available()){

  }
  if(Serial.available() > 0){
    wert = Serial.parseInt();
//    disconnect = 0;
  }
  else{
//   disconnect++;
  }

    int num = wert;
    int length = 0;
    int temp = num;

    while (temp != 0) {
        length++;
        temp /= 10;
    }

    int digits[length];
    temp = num;

    for (int i = length - 1; i >= 0; i--) {
        digits[i] = temp % 10;
        temp /= 10;
    }



  if(digits[0] == 1){
    int numlenkung = wert -1000;
    lenkungval = map(numlenkung,0,200,0,180);

  }

  if(digits[0] == 2){
    int gasbremse = wert -2000;

      if(gasbremse == 100){
        bremse = 0;
        gas = 0;
      }
      if(gasbremse < 100){
        bremse = map(gasbremse, 101,0,0,255);
        gas = 0;
      }
      if(gasbremse > 100){
        gas = map(gasbremse,100,200,0,255);
        bremse = 0;
      }

  }
/
  if(disconnect>100){
    gas = 0;
    bremse = 0;
    lenkungval = 90;
  }
*/
//  Serial.print("gas: ");
//  Serial.print(gas);
//  Serial.print(" bremse: ");
//  Serial.print(bremse);
//  Serial.print(" Lenkung: ");
//  Serial.println(lenkungval);
  lcd.setCursor(0,0);
  lcd.print("   ");
  lcd.setCursor(0,0);
  lcd.print(lenkungval);
  lcd.setCursor(0,1);
  lcd.print("   ");
  lcd.setCursor(0,1);
  lcd.print(gas);
  lcd.setCursor(4,0);
  lcd.print("   ");
  lcd.setCursor(4,0);
  lcd.print(bremse);

  myservo.write(lenkungval); 

  delay(5);
  analogWrite(motor1, gas);
  analogWrite(motor2, bremse);
}