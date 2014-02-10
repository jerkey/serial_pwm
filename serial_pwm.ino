void setup() {
  Serial.begin(57600);
  pinMode(9, OUTPUT); 
}

void loop() {
  while (Serial.available() > 0) {

    // look for the next valid integer in the incoming serial stream:
    // int pwmValue = Serial.parseInt(); 
    float celsius = Serial.parseFloat();
    // look for the newline. That's the end of your
    if (Serial.read() == '\n') {
  // PWM of 24 = 0 celsius
  //  114 = 20 C
  //  210 = 40 C
  //  from 20 to 40 degrees = 96 PWM so 4.8 PWM per degree
  //  PWM = (celsius * 4.8) + 18
      int pwm = constrain((int)(celsius * 4.8) + 18, 0, 255);
      analogWrite(9,pwm);
      Serial.println(pwm);
    }
  }
}
