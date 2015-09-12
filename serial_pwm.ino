byte sineTable[180];

void setup() {
  Serial.begin(57600);
  pinMode(9, OUTPUT); 
  setPwmFrequency(9, 1);
  Serial.println("serial_pwm");
  for (int i = 0; i < 180; i++) {
    sineTable[i] = (byte)(255.0 * sin((3.14159/180.0)*i));
  }
}

int lastPwm, pwm, cycle = 0;
int scale = 127;

void loop() {
  while (Serial.available() > 0) {

    float celsius = Serial.parseFloat();
    if (Serial.read() == '\n') {
      scale = constrain((int)(celsius), 0, 255);
      Serial.println(scale);
    }
  }
  /*
   * pwm = (int)(abs(sin((float)millis()/1000.0*2*3.14159*60.0)) * scale);
  if (lastPwm != pwm) {
    analogWrite(9,pwm);
    lastPwm = pwm;
    //Serial.println(pwm);
  }
  */
  analogWrite(9,sineTable[cycle]);
  cycle++;
  cycle %= 180;
}

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
