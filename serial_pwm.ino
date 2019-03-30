#define CAP_MINUS A0
#define MOTOR_MINUS A1
#define MOTOR_I A2
#define CHARGE_PWM 9
#define CAPACITOR_PWM 10
#define VOLT_COEFF 2.366
#define MOTOR_I_COEFF 8.371
#define MOTOR_I_ZERO 508.7 // ADC value at zero amps
#define OVERHEAT_SENSE 12 // gets shorted to ground when overheating
#define RED_LEDS 5
#define GREEN_LEDS 6
#define BLUE_LEDS 11
#define MAX_CURRENT 5

unsigned long displayTime = 0;
float cap_minus_v, motor_v, motor_i, peak_v;
int pwmValue = 0;
int oldPwmValue = 0;
int capPwmValue = 0;

byte state = 0;
#define STATE_OFF // engine off, relay open
#define STATE_PRECHARGE // relay has closed, capacitor precharging
#define STATE_STARTUP // relay has closed, voltage detected
#define STATE_RUNNING // relay closed, capacitor charged, start motor

void setup() {
  Serial.begin(9600);
  setPwmFrequency(9,8); // set pins 9,10 to 4KHz PWM
  pinMode(CHARGE_PWM, OUTPUT);
  pinMode(CAPACITOR_PWM, OUTPUT);
  pinMode(RED_LEDS,OUTPUT);
  pinMode(GREEN_LEDS,OUTPUT);
  pinMode(BLUE_LEDS,OUTPUT);
  analogWrite(BLUE_LEDS,5);
  digitalWrite(OVERHEAT_SENSE,HIGH);  // enable pullup resistor
  Serial.println("serial_pwm vanhydraulic");
  getVolts();
  peak_v = motor_v; // record voltage of system upon startup
}

void loop() {
  while (Serial.available() > 0) handleSerial();
  getVolts();
  if (motor_i > MAX_CURRENT) {
    pwmValue = 0; //constrain(pwmValue - 5,0,255);
    analogWrite(CHARGE_PWM,pwmValue);
    oldPwmValue = pwmValue;
    digitalWrite(RED_LEDS,HIGH);
    digitalWrite(GREEN_LEDS,HIGH);
  } else {
    digitalWrite(GREEN_LEDS,LOW);
    analogWrite(RED_LEDS,pwmValue);
  }

  if ( millis() - displayTime > 1000 ) {
    printDisplay();
    displayTime = millis();
  }
}

void handleSerial() {
  char inChar = Serial.read(); // read a char
  if (inChar == 'c') { // set capacitor PWM
    int inInt = Serial.parseInt(); // look for the next valid integer in the incoming serial stream:
    if ((inInt != capPwmValue) && (inInt < 256)) {
      capPwmValue = constrain(inInt, 0, 255);
      Serial.println(capPwmValue);
      analogWrite(CAPACITOR_PWM,capPwmValue);
      analogWrite(BLUE_LEDS,capPwmValue);
    }
  }
  if (inChar == 'p') {
    pwmValue = Serial.parseInt(); // look for the next valid integer in the incoming serial stream:
    if ((pwmValue != oldPwmValue) && (pwmValue < 256)) {
      pwmValue = constrain(pwmValue, 0, 255);
      oldPwmValue = pwmValue;
      Serial.println(pwmValue);
      analogWrite(CHARGE_PWM,pwmValue);
      analogWrite(RED_LEDS,pwmValue);
    } else {
      pwmValue = oldPwmValue;
    }
  } else if (inChar == 'k'){
    pwmValue = constrain(pwmValue + 1, 0, 255);
    oldPwmValue = pwmValue;
    Serial.println(pwmValue);
    analogWrite(CHARGE_PWM,pwmValue);
    analogWrite(RED_LEDS,pwmValue);
  } else if (inChar == 'j'){
    pwmValue = constrain(pwmValue - 1, 0, 255);
    oldPwmValue = pwmValue;
    Serial.println(pwmValue);
    analogWrite(CHARGE_PWM,pwmValue);
    analogWrite(RED_LEDS,pwmValue);
  } else if (inChar == 'K'){
    pwmValue = constrain(pwmValue + 10, 0, 255);
    oldPwmValue = pwmValue;
    Serial.println(pwmValue);
    analogWrite(CHARGE_PWM,pwmValue);
    analogWrite(RED_LEDS,pwmValue);
  } else if (inChar == 'J'){
    pwmValue = constrain(pwmValue - 10, 0, 255);
    oldPwmValue = pwmValue;
    Serial.println(pwmValue);
    analogWrite(CHARGE_PWM,pwmValue);
    analogWrite(RED_LEDS,pwmValue);
  } else if ((inChar >= 'a')&&(inChar <= 'z')) {
    delay(300); // wait for the user to press the same key again
    if (Serial.available() && inChar == Serial.read()) { // only if the same char pressed twice rapidly
      pwmValue = constrain((inChar - 97) * 11, 0, 255);
      oldPwmValue = pwmValue;
      Serial.println(pwmValue);
      analogWrite(CHARGE_PWM,pwmValue);
      analogWrite(RED_LEDS,pwmValue);
    }
  } else {
    Serial.println("p### to enter PWMval, j to -1, k to +1, J to -10, K to +10, aa - zz 0 to 255");
  }
}

void printDisplay() {
  if (!digitalRead(OVERHEAT_SENSE)) Serial.println("OVERHEATING!  ");
  Serial.print("pwmValue: ");
  Serial.print(pwmValue);

  Serial.print("  capPwmValue: ");
  Serial.print(capPwmValue);

  Serial.print("  CAP_MINUS_V: ");
  Serial.print(cap_minus_v,1);
  Serial.print(" (");
  Serial.print(averageRead(CAP_MINUS));

  Serial.print(")  MOTOR_V: ");
  Serial.print(motor_v,1);
  Serial.print(" (");
  Serial.print(averageRead(MOTOR_MINUS));

  Serial.print(")  MOTOR_I: ");
  Serial.print(motor_i,1);
  Serial.print(" (");
  Serial.print(averageRead(MOTOR_I));
  Serial.println(")");
}

void getVolts() {
  cap_minus_v = averageRead(CAP_MINUS) / VOLT_COEFF;
  motor_v = averageRead(MOTOR_MINUS) / VOLT_COEFF;
  motor_i = ( averageRead(MOTOR_I) - MOTOR_I_ZERO ) / MOTOR_I_COEFF;
}

float averageRead(int pin) {
  float analogAdder = 0;
  for (int i = 0; i < 50; i++) analogAdder += analogRead(pin);
  analogAdder /= 50;
  return analogAdder;
}

// Note that the base frequency for pins 3, 9, 10, and 11 is 31250 Hz
// Note that the base frequency for pins 5 and 6 is 62500 Hz
void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch(divisor) {
    case 1:
      mode = 0x01;
      break;
    case 8:
      mode = 0x02;
      break;
    case 64:
      mode = 0x03;
      break;
    case 256:
      mode = 0x04;
      break;
    case 1024:
      mode = 0x05;
      break;
    default:
      return;
    }
    if(pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    }
    else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  }
  else if(pin == 3 || pin == 11) {
    switch(divisor) {
    case 1:
      mode = 0x01;
      break;
    case 8:
      mode = 0x02;
      break;
    case 32:
      mode = 0x03;
      break;
    case 64:
      mode = 0x04;
      break;
    case 128:
      mode = 0x05;
      break;
    case 256:
      mode = 0x06;
      break;
    case 1024:
      mode = 0x7;
      break;
    default:
      return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}
