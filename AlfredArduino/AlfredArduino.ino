#define BUF_SIZE 1000
#define MESSAGE_SIZE 6
#define FUNC_NAME_SIZE 4
char in_buffer[BUF_SIZE];
char out_buffer[BUF_SIZE];

#define ROTATION_STOP 0
#define ROTATION_CLOCKWISE 1
#define ROTATION_COUNTERCLOCKWISE 2

#define CONTROL_SIGNAL_1 3
#define CONTROL_SIGNAL_2 5
#define CONTROL_SIGNAL_3 6
#define CONTROL_SIGNAL_4 9

#define SOLENOID_1 2
#define SOLENOID_2 4
#define SOLENOID_3 7
#define SOLENOID_4 8
#define VEND_TIME 3750

boolean initialized;

#define IR_PIN A1
#define ULTRASOUND_PIN 11
int ir_val = 9999;
int us_val = 9999;
int ds_val = 0;

void spin(int rotation_signal) {
  switch (rotation_signal) {
  case ROTATION_CLOCKWISE:
      Serial.write("ROTATION_CLOCKWISE");
//    digitalWrite(CONTROL_SIGNAL_1, LOW);
//    digitalWrite(CONTROL_SIGNAL_2, HIGH);
//    digitalWrite(CONTROL_SIGNAL_3, HIGH);
//    digitalWrite(CONTROL_SIGNAL_4, LOW);
    break;
  case ROTATION_COUNTERCLOCKWISE:
      Serial.write("ROTATION_COUNTERCLOCKWISE");
//    digitalWrite(CONTROL_SIGNAL_1, HIGH);
//    digitalWrite(CONTROL_SIGNAL_2, LOW);
//    digitalWrite(CONTROL_SIGNAL_3, LOW);
//    digitalWrite(CONTROL_SIGNAL_4, HIGH);
    break;
  case ROTATION_STOP:
      Serial.write("ROTATION_STOP");
//    digitalWrite(CONTROL_SIGNAL_1, LOW);
//    digitalWrite(CONTROL_SIGNAL_2, LOW);
//    digitalWrite(CONTROL_SIGNAL_3, LOW);
//    digitalWrite(CONTROL_SIGNAL_4, LOW);
    break;
  }
}

void vend(int vend_signal) {
  ds_val = 0;
  switch (vend_signal) {
  case 1:
//    Serial.write("Dispensing Soda 1");
    digitalWrite(SOLENOID_1, HIGH);
    delay(VEND_TIME);
    digitalWrite(SOLENOID_1, LOW);
    ds_val = 1;
    break;
  case 2:
//    Serial.write("Dispensing Soda 2");
    digitalWrite(SOLENOID_2, HIGH);
    delay(VEND_TIME);
    digitalWrite(SOLENOID_2, LOW);
    ds_val = 2;
    break;
  case 3:
//    Serial.write("Dispensing Soda 3");
    digitalWrite(SOLENOID_3, HIGH);
    delay(VEND_TIME);
    digitalWrite(SOLENOID_3, LOW);
    ds_val = 3;
    break;
  case 4:
//    Serial.write("Dispensing Soda 4");
    digitalWrite(SOLENOID_4, HIGH);
    delay(VEND_TIME);
    digitalWrite(SOLENOID_4, LOW);
    ds_val = 4;
    break;
  }
}

String SerialRead(char* buf)
{
  for(int i=0; i<MESSAGE_SIZE; i++) 
    buf[i] = Serial.read();
  buf[MESSAGE_SIZE] = '\0';
}

float irVoltsToCentimeters(int volts)
{
   return 60.495 * pow(volts * .0049, -1.1904);
}

void setup()
{
  Serial.begin(9600);
  initialized = false;
  pinMode(SOLENOID_1,OUTPUT);
  pinMode(SOLENOID_2,OUTPUT);
  pinMode(SOLENOID_3,OUTPUT);
  pinMode(SOLENOID_4,OUTPUT);
}

void loop()
{
    if(!initialized) {
       if (Serial.available()>=4) {
         char init_msg[] = "INIT";
         boolean is_init = true;
         for(int i=0; i<4; i++) 
           if(is_init && Serial.read() != init_msg[i])
             is_init = false;
         if(is_init) {
           initialized = true;
           Serial.write("INIT");
         }
       }
    } else {      
      if (Serial.available()>=MESSAGE_SIZE) {
        //Serial.println(Serial.available(),DEC);
        SerialRead(in_buffer);
        char spin_str[] = "SPIN"; boolean spin_valid = true;
        char vend_str[] = "VEND"; boolean vend_valid = true;
        for(int i=0; i<FUNC_NAME_SIZE && (spin_valid || vend_valid); i++) {
           if(spin_valid && in_buffer[i]!=spin_str[i])
             spin_valid = false;
           if(vend_valid && in_buffer[i]!=vend_str[i])
             vend_valid = false;
        }
        if(in_buffer[FUNC_NAME_SIZE] == ' ') {
          
          if(vend_valid) {
            int vend_signal = -1;
            switch(in_buffer[FUNC_NAME_SIZE+1]) {
              case '1':
                vend_signal = 1;
                break;
              case '2':
                vend_signal = 2;
                break;
              case '3':
                vend_signal = 3;
                break;
              case '4':
                vend_signal = 4;
                break;
            }
            if(vend_signal > 0 ) {
              vend(vend_signal);
            }
          } else if (spin_valid) {
            int spin_signal = -1;
            switch(in_buffer[FUNC_NAME_SIZE+1]) {
              case '0':
                spin_signal = 0;
                break;
              case '1':
                spin_signal = 1;
                break;
              case '2':
                spin_signal = 2;
                break;
            }
            if(spin_signal >= 0) {
              spin(spin_signal);
            } 
          }        
        }
      }
      delay(500);
      
      // For use with SHARP 2YOAO2 (http://sharp-world.com/products/device/lineup/data/pdf/datasheet/gp2y0a02_e.pdf)
      // Prints "CLOSE OBJECT : STOP" when object is approximately a meter away
      ir_val = analogRead(IR_PIN);
      float cmDistance = irVoltsToCentimeters(ir_val);
      if (cmDistance < 150 && cmDistance > 100){
        Serial.println("CLOSE OBJECT : STOP");
      }
    }     
}
