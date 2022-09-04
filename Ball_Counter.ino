#include <SoftwareSerial.h>

SoftwareSerial arduino(2, 3);

const int ir0pin = A0, ir1pin = A1, ir2pin = A2, ir3pin = A3;
int last0ir = 0, last1ir = 0, last2ir = 0, last3ir = 0,lastLast0ir = 0, lastLast1ir = 0, lastLast2ir = 0, lastLast3ir = 0; 

const int minForCount = 120;
int count = 0, lastCount = 1;

void setup(){
  Serial.begin(9600);
  arduino.begin(9600);
}
 
void loop(){
  readIRs();
  if(count != lastCount){
    Serial.print("Count: ");
    Serial.println(count);
    arduino.println(count);
    arduino.flush();
  }
  lastCount = count;
  delay(50);
}

void readIRs(){
  int ir0 = analogRead(A0);
  int ir1 = analogRead(A1);
  int ir2 = analogRead(A2);
  int ir3 = analogRead(A3);
  
  if(ir0 > minForCount && last0ir > minForCount && lastLast0ir < minForCount){
    count++;
  }if(ir1 > minForCount && last1ir > minForCount && lastLast1ir < minForCount){
    count++;
  }if(ir2 > minForCount && last2ir > minForCount && lastLast2ir < minForCount){
    count++;
  }if(ir3 > minForCount && last3ir > minForCount && lastLast3ir < minForCount){
    count++;
  }

  lastLast0ir = last0ir;
  lastLast1ir = last1ir;
  lastLast2ir = last2ir;
  lastLast3ir = last3ir;  
  last0ir = ir0;
  last1ir = ir1;
  last2ir = ir2;
  last3ir = ir3;
}
