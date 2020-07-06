
#include <SPI.h> 
#include <MFRC522.h> 
 
#define SS_PIN 10 
#define RST_PIN 9 
  
MFRC522 mfrc522(SS_PIN, RST_PIN); // MFRC522 인스턴스 생성 
 
void setup() 
{ 
  Serial.begin(9600); // PC와 시리얼 통신을 위한 초기화
  SPI.begin();        // SPI 버스 사용을 위한 초기화
  mfrc522.PCD_Init(); // MFRC522를 초기화
  Serial.println("A 키를 이용하여 데이터를 쓰고 읽는 방법을 보여주는 예제"); 
} 
 
void loop() 
{    
  byte buffer[18]; 
  byte size = sizeof(buffer); 
 
  // 새로운 카드가 접촉되었는지 확인합니다: 
  if ( ! mfrc522.PICC_IsNewCardPresent()) { 
    return; 
  } 
 
  // 카드들 중 하나를 선택합니다: 
  if ( ! mfrc522.PICC_ReadCardSerial()) { 
    return; 
  } 
   
  // 카드가 선택되었고, mfrc522.uid에 UID와 SAK 정보가 들어 있다.
  // UID 정보를 16 진수 형태로 출력: 
  Serial.print("카드 UID:"); 
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  { 
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "); 
    Serial.print(mfrc522.uid.uidByte[i], HEX); 
  }  
  Serial.println(); 
 
  // PICC 타입 출력: 
  byte piccType = mfrc522.PICC_GetType(mfrc522.uid.sak); 
  Serial.print("PICC 타입: "); 
  Serial.println(mfrc522.PICC_GetTypeName(piccType)); 
  if ( piccType != MFRC522::PICC_TYPE_MIFARE_MINI  
    && piccType != MFRC522::PICC_TYPE_MIFARE_1K 
    && piccType != MFRC522::PICC_TYPE_MIFARE_4K) { 
    Serial.println("이 예제는 MIFARE Classic 카드에서만 동작합니다."); 
    return; 
  }

  // keyA를 초기화
  MFRC522::MIFARE_Key keyA;
  //                         0   1   2   3   4   5   6 7 8 9    10  11  12  13  14  15    
  byte trailerBuffer[] = { 255,255,255,255,255,255,  0,0,0,0,  255,255,255,255,255,255  }; // Keep default keys. 
 
  for (byte i = 0; i < 6; i++) { 
    keyA.keyByte[i] = 0xFF; 
  }

  // 이 예제에서는 1번째 섹터의 0 번과 3 번 블록(Sector Trailer)을 사용
  byte valueBlockA = 8; 
  byte trailerBlock = 11; 

  // A 키로 인증: 
  Serial.println("A 키로 인증 중..."); 
  byte status; 
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &keyA, &(mfrc522.uid)); 
  if (status != MFRC522::STATUS_OK) { 
    Serial.print("PCD_Authenticate() 실패: "); 
    Serial.println(mfrc522.GetStatusCodeName(status)); 
    return; 
  } 

  // 저장할 데이터(16btes)
  byte value1Block[] = {  
    4,4,3,4,  5,6,7,8, 9,10,255,12,  13,14,15,16,   valueBlockA,~valueBlockA,valueBlockA,~valueBlockA   }; 
 
  // 1 번째 블록(val..ue block)에 데이터를 기록합니다: 
  Serial.println("1 번째 블록 기록 중: 2 번째 섹터의 첫번째 블록"); 
  print_buffer(value1Block); 
  status = mfrc522.MIFARE_Write(valueBlockA, value1Block, 16); 
  if (status != MFRC522::STATUS_OK) { 
    Serial.print("MIFARE_Write() 실패: "); 
    Serial.println(mfrc522.GetStatusCodeName(status)); 
    return; 
  } 
 
  Serial.println("1 번째 블록 읽는 중: 2 번째 섹터의 첫번째 불록");         
  size = sizeof(buffer);   
  status = mfrc522.MIFARE_Read(valueBlockA, buffer, &size); 
  if (status != MFRC522::STATUS_OK) { 
    Serial.print("MIFARE_Read() 실패: "); 
    Serial.println(mfrc522.GetStatusCodeName(status)); 
    return; 
  } 
   
  print_buffer(buffer); 
    
  // PICC를 멈춥니다 
  mfrc522.PICC_HaltA(); 
 
  // PCD의 Crypto를 멈춥니다 
  mfrc522.PCD_StopCrypto1(); 
 
  // 스케치도 멈추게 합니다: 
  while(true) {  
    ; 
  } 
} 
 
void print_buffer(byte buffer[]) 
{ 
  Serial.print(String("buffer[] = {")); 
  for(int i = 0; i < 16; i++) { 
    if(i != 0) 
      Serial.print(','); 
       
    Serial.print(buffer[i]); 
  } 
  Serial.println("}");   
} 
