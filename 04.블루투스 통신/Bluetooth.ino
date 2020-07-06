#include <SoftwareSerial.h>// 시리얼 라이브러리 임포트

SoftwareSerial mySerial(3, 2); //RX, TX  블루투스 모듈 핀 번호 설정

void setup()
{
  Serial.begin(9600);  // 9600의 속도로 시리얼 모니터와의 통신 시작
  mySerial.begin(9600); // 9600의 속도로 블루투스 통신 시작
}

void loop()
{
  // 데이터 받기(핸드폰 -> 아두이노 -> 시리얼 모니터)
  if(mySerial.available())
    Serial.write(mySerial.read());

  // 데이터 보내기(시리얼 모니터 -> 아두이노 -> 핸드폰)
  if(Serial.available())
    mySerial.write(Serial.read());
}
