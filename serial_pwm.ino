#define PFC_PLUS A0
#define BATT_MINUS A1
#define BATT_I A2
#define AC_I A4
#define CHARGE_PWM 9
#define VOLT_COEFF 2.366
#define AC_I_COEFF 8.888
#define AC_I_ZERO 181
#define BATT_I_COEFF 8.371
#define BATT_I_ZERO 508.7 // ADC value at zero amps
#define OVERHEAT_SENSE 12 // gets shorted to ground when overheating
#define RED_LEDS 5
#define GREEN_LEDS 6
#define BLUE_LEDS 11

void setup() {
  Serial.begin(9600);
  pinMode(CHARGE_PWM, OUTPUT);
  pinMode(RED_LEDS,OUTPUT);
  pinMode(GREEN_LEDS,OUTPUT);
  pinMode(BLUE_LEDS,OUTPUT);
  digitalWrite(BLUE_LEDS,HIGH);
  digitalWrite(OVERHEAT_SENSE,HIGH);  // enable pullup resistor
  Serial.println("serial_pwm vancharge");
}

unsigned long displayTime = 0;
float pfc_v, batt_v, ac_i, batt_i;
int pwmValue,oldPwmValue = 0;

void loop() {
  while (Serial.available() > 0) {

    // look for the next valid integer in the incoming serial stream:
    pwmValue = Serial.parseInt();
    // look for the newline. That's the end of your
    // if (Serial.read() == '/n') {
      if ((pwmValue != oldPwmValue) && (pwmValue < 128)) {
      pwmValue = constrain(pwmValue, 0, 127);
      oldPwmValue = pwmValue;
      Serial.println(pwmValue);
      analogWrite(CHARGE_PWM,pwmValue);
      analogWrite(RED_LEDS,pwmValue);
    } else {
      pwmValue = oldPwmValue;
    }
  }
  getVolts();
  if ( millis() - displayTime > 1000 ) {
    printDisplay();
    displayTime = millis();
  }
}

void printDisplay() {
  if (!digitalRead(OVERHEAT_SENSE)) Serial.println("OVERHEATING!  ");
  Serial.print("PWM: ");
  Serial.print(pwmValue);

  Serial.print("  AC_I: ");
  Serial.print(ac_i,1);
  Serial.print(" (");
  Serial.print(averageRead(AC_I));

  Serial.print(")  PFC_V: ");
  Serial.print(pfc_v,1);
  Serial.print(" (");
  Serial.print(averageRead(PFC_PLUS));

  Serial.print(")  BATT_V: ");
  Serial.print(batt_v,1);
  Serial.print(" (");
  Serial.print(averageRead(BATT_MINUS));

  Serial.print(")  BATT_I: ");
  Serial.print(batt_i,1);
  Serial.print(" (");
  Serial.print(averageRead(BATT_I));
  Serial.println(")");
}

void getVolts() {
  pfc_v = averageRead(PFC_PLUS) / VOLT_COEFF;
  float batt_minus_v = averageRead(BATT_MINUS) / VOLT_COEFF;
  batt_v = pfc_v - batt_minus_v; // pluses are connected together
  batt_i = ( averageRead(BATT_I) - BATT_I_ZERO ) / BATT_I_COEFF;
  ac_i = ( averageRead(AC_I) - AC_I_ZERO ) / AC_I_COEFF;
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
