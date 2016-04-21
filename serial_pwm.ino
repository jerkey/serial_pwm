void setup() {
  Serial.begin(57600);
  pinMode(9, OUTPUT);
}

void loop() {
  while (Serial.available() > 0) {

    // look for the next valid integer in the incoming serial stream:
    int pwmValue = Serial.parseInt();
    // look for the newline. That's the end of your
    if (Serial.read() == '\n') {
      int pwm = constrain(pwmValue, 0, 255);
      analogWrite(9,pwm);
      Serial.println(pwm);
    }
  }
}
