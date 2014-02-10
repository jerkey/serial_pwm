void setup() {
  Serial.begin(9600);
  pinMode(9, OUTPUT); 
}

void loop() {
  while (Serial.available() > 0) {

    // look for the next valid integer in the incoming serial stream:
    int pwmValue = Serial.parseInt(); 
    // look for the newline. That's the end of your
    if (Serial.read() == '\n') {
      // constrain the values to 0 - 255
      pwmValue = constrain(pwmValue, 0, 255);
      Serial.println(pwmValue);
      analogWrite(9,pwmValue);
    }
  }
}
