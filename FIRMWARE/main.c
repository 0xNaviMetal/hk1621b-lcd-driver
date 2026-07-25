//follow me plssss 

//iam adding all the comments so u can understand  if u didnt ask  ai hhhh  
const uint8_t PIN_CS   = 10; // u will find this labele in the lcd (con1). follow the connections  con1-> (+),  next->data, next->WR , next->cs , next->(-)
const uint8_t PIN_WR   = 11;
const uint8_t PIN_DATA = 12;

void ht_write_bit(bool b) {
  digitalWrite(PIN_DATA, b ? HIGH : LOW);
  digitalWrite(PIN_WR, LOW);
  delayMicroseconds(1);
  digitalWrite(PIN_WR, HIGH);   // rising edge -> SO  HK1621( the driver ) willl samples DATA here ok  
  delayMicroseconds(1); // we must add this delay and 1 ms will be enought 
  digitalWrite(PIN_WR, LOW);    //  and we prepare for next bit
  delayMicroseconds(1);
}

void writeBitsMSB(uint32_t value, uint8_t nBits) {
  for (int i = nBits - 1; i >= 0; --i) ht_write_bit((value >> i) & 1);
}

void writeBitsLSB(uint32_t value, uint8_t nBits) {
  for (uint8_t i = 0; i < nBits; ++i) ht_write_bit((value >> i) & 1);
}

void sendCommand(uint8_t cmd) {
  digitalWrite(PIN_CS, LOW);
  delayMicroseconds(2);
  writeBitsMSB(0b100, 3);
  writeBitsMSB(cmd, 8);
  digitalWrite(PIN_CS, HIGH);
  delayMicroseconds(4);
}

void write4bitsToAddr(uint8_t addr6, uint8_t nibble4) {
  addr6 &= 0x3F;
  nibble4 &= 0x0F;
  digitalWrite(PIN_CS, LOW);
  delayMicroseconds(2);
  writeBitsMSB(0b101, 3);
  writeBitsMSB(addr6, 6);
  writeBitsLSB(nibble4, 4);
  digitalWrite(PIN_CS, HIGH);
  delayMicroseconds(4);
}

 // HK1621B RAM is 4-bit per address (COM0-COM3). 
// n0 & n1 hold two 4-bit nibbles across 2 RAM addresses (4 bits + 4 bits = 8 segments ( 7 segments  and the dot ))
struct SegPair { uint8_t n0, n1; };
const SegPair DIGIT[10] = {
  {0b1011, 0b1110}, // 0
  {0b0000, 0b0110}, // 1
  {0b0111, 0b1100}, // 2
  {0b0101, 0b1110}, // 3
  {0b1100, 0b0110}, // 4
  {0b1101, 0b1010}, // 5
  {0b1111, 0b1010}, // 6
  {0b1000, 0b1110}, // 7
  {0b1111, 0b1110}, // 8
  {0b1101, 0b1110}  // 9
};
// ------------------------------------------------------------


void displayDigit(uint8_t digit, uint8_t addrBase) {
  // clear firsttt
  write4bitsToAddr(addrBase,   0);
  write4bitsToAddr(addrBase+1, 0);
  if (digit <= 9) {
    write4bitsToAddr(addrBase,   DIGIT[digit].n0);
    write4bitsToAddr(addrBase+1, DIGIT[digit].n1);
  }
}

// we show the  number on 3 digits
// Hundreds = 0&1,,,,,  Tens = 2&3,,,,, Ones = 4&5
void showNumber(uint16_t n) {
  uint8_t ones     =  n        % 10;
  uint8_t tens     = (n / 10)  % 10;
  uint8_t hundreds = (n / 100) % 10;

  // ones 
  displayDigit(ones, 4);

  // tens ( el3acharat hh)
  if (n >= 10) displayDigit(tens, 2);
  else { write4bitsToAddr(2,0); write4bitsToAddr(3,0); }

  // hundreds
  if (n >= 100) displayDigit(hundreds, 0);
  else { write4bitsToAddr(0,0); write4bitsToAddr(1,0); }
}

void setup() {
  pinMode(PIN_CS, OUTPUT);
  pinMode(PIN_WR, OUTPUT);
  pinMode(PIN_DATA, OUTPUT);

  digitalWrite(PIN_CS, HIGH);
  digitalWrite(PIN_WR, LOW);
  digitalWrite(PIN_DATA, LOW);

  delay(20);

  sendCommand(0x01); // SYS_EN
  sendCommand(0x18); // RC oscillator
  sendCommand(0x29); // BIAS/COM setting
  sendCommand(0x03); // LCD ON
  delay(5);
}

void loop() {
  for (uint16_t num = 0; num <= 150; ++num) {
    showNumber(num);
    
    delay(300); // speed adjust it kima 7abit 
    
  }
}

