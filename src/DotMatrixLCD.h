#ifndef DotMatrixLCD_h
#define DotMatrixLCD_h


class DotMatrixLCD : public LiquidCrystal {
public:
  DotMatrixLCD(uint8_t rs, uint8_t enable,
		uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)
  : LiquidCrystal(rs, enable, d0, d1, d2, d3) {}

  void printDot(uint8_t x, uint8_t y) {
    setCursor(x / 2, y / 4);
    write(((x % 2) << 2) + y % 4);
  }

  void clearDot(uint8_t x, uint8_t y) {
    setCursor(x / 2, y / 4);
    write(' ');
  }
  
  void createChars() {
    byte car[8];
    int carPos = 0;
    car[0] = B11000;
    car[1] = B11000;    
    car[2] = B00000;    
    car[3] = B00000;    
    car[4] = B00000;    
    car[5] = B00000;    
    car[6] = B00000;    
    car[7] = B00000;    
    createChar(carPos++,  car);
    
    car[0] = B00000;
    car[1] = B00000;    
    car[2] = B11000;    
    car[3] = B11000;    
    car[4] = B00000;    
    car[5] = B00000;    
    car[6] = B00000;    
    car[7] = B00000;    
    createChar(carPos++,  car);
    
    car[0] = B00000;
    car[1] = B00000;    
    car[2] = B00000;    
    car[3] = B00000;    
    car[4] = B11000;    
    car[5] = B11000;    
    car[6] = B00000;    
    car[7] = B00000;    
    createChar(carPos++,  car);
    
    car[0] = B00000;
    car[1] = B00000;    
    car[2] = B00000;    
    car[3] = B00000;    
    car[4] = B00000;    
    car[5] = B00000;    
    car[6] = B11000;    
    car[7] = B11000;    
    createChar(carPos++,  car);
    
    car[0] = B00011;
    car[1] = B00011;    
    car[2] = B00000;    
    car[3] = B00000;    
    car[4] = B00000;    
    car[5] = B00000;    
    car[6] = B00000;    
    car[7] = B00000;    
    createChar(carPos++,  car);
    
    car[0] = B00000;
    car[1] = B00000;    
    car[2] = B00011;    
    car[3] = B00011;    
    car[4] = B00000;    
    car[5] = B00000;    
    car[6] = B00000;    
    car[7] = B00000;    
    createChar(carPos++,  car);
    
    car[0] = B00000;
    car[1] = B00000;    
    car[2] = B00000;    
    car[3] = B00000;    
    car[4] = B00011;    
    car[5] = B00011;    
    car[6] = B00000;    
    car[7] = B00000;    
    createChar(carPos++,  car);
    
    car[0] = B00000;
    car[1] = B00000;    
    car[2] = B00000;    
    car[3] = B00000;    
    car[4] = B00000;    
    car[5] = B00000;    
    car[6] = B00011;    
    car[7] = B00011;    
    createChar(carPos++,  car);
  }
};

#endif // DotMatrixLCD_h
