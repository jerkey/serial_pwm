#define PFC_PLUS A0
#define BATT_MINUS A1
#define BATT_I A2
#define AC_I A4
#define CHARGE_PWM 9
#define VOLT_COEFF 1.0
#define AC_I_COEFF 1.0
#define AC_I_ZERO 178.5
#define BATT_I_COEFF 1.0
#define BATT_I_ZERO 512 // ADC value at zero amps
#define OVERHEAT_SENSE 11 // gets shorted to ground when overheating
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
