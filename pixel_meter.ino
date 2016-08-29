#include <SPI.h>
#include <Adafruit_WS2801.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

uint8_t dataPin  = 2;    // White wire
uint8_t clockPin = 3;    // Green wire

int input;
String msg;
int waitBlink = 250;
int lastInput = 0;

char inByte;
char inBytes[100];
int index = 0;
float bright = 0;
byte r, g, b;

LiquidCrystal_I2C lcd(0x20, 16, 2);
Adafruit_WS2801 strip = Adafruit_WS2801((uint16_t)1, dataPin, clockPin);

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(10); 

  strip.begin();
  strip.show();

  lcd.init();
  //test_lcd();
}

void loop(){
  if(Serial.available() > 0){
    inByte = Serial.read();
    if(inByte=='\n'){
      parseInput(inBytes);

      //input = Serial.parseInt();
      lastInput = input;
      lcdPrint(msg);
      light(input, bright);
    }
    else{
      inBytes[index] = inByte;
      index++;
    }
  }
  else{
    if(lastInput>200 && lastInput<=300){
      blinkPixel(getPixelHealth(lastInput-200), 3, waitBlink);
    }
  }
}

/**
 * @return {String} substring of str from begin of line to delim
 */
String firstPart(String str, char delim='|'){
  String out;
  int ind = str.indexOf(delim);
  if(ind==-1) out = str;
  else out = str.substring(0, ind);
  return out;
}

/** parse 199|message|50 to vars */
void parseInput(char inBytes[]){
  input = -2;
  msg = "-2";
  bright = -2.0;
  
  String str = String(inBytes);
  while(true){
    int ind = str.indexOf('|');
    
    if(input==-2){
      input = firstPart(str).toInt();
    }
    else if(msg=="-2"){
      msg = firstPart(str);
    }
    else if(bright==-2){
      bright = (firstPart(str).toInt() / 100.0);
      //Serial.println(bright);
    }

    str = str.substring(ind+1);
    if(ind==-1) break;
  }

  for (int i=0; i<100; i++) {
    inBytes[i] = 0;
  }
  index = 0;

  if(input==-2 && str.length()>0) input = str.toInt();
  if(msg=="-2") msg = "";
  if(bright==-2.0) bright = 1.0;
  else if(bright<0) bright = 0;
  else if(bright>1.0) bright = 1.0;
  
  /*Serial.print("input: ");
  Serial.println(input);
  Serial.print("msg: ");
  Serial.println(msg);*/
}

/**
 * Light pixel with [input] color and [bright] brightness
 */
void light(int input, float bright){
  //Serial.print(input);
  //Serial.print("/");
  //Serial.println();
  //blinkPixel(getPixelHealth(50), 3, 50);
  if(input>0 && input<=100){
    setPixelColor(
      setColorBright( getPixelHealth(input), bright )
    );
  }
  else if(input>100 && input<=200){
    input = input-100;
    blinkPixel(
      setColorBright( getPixelHealth(input), bright ),
      3,
      waitBlink
    );
  }
  else if(input>200 && input<=300){
    input = input-100;
    blinkPixel(
      setColorBright( getPixelHealth(input), bright ),
      3,
      waitBlink
    );
  }
  else if(input==-1){
    offPixel();
  }
  else{
    blinkPixel(getPixelHealth(0), 3, 50);
  }
}

void light(int input){
  light(input, 1.0);
}

/** 0 - red, 100 - green */
uint32_t getPixelHealth(float percent){
  uint8_t r=255, g=255, b=0;

  if(percent<50){
    r = 255;
    g = round(percent/50.0 * 255);
  }
  if(percent>50){
    r = round((100-percent)/50.0 * 255);
    g = 255;
  }
  return Color(r, g, b);
}

void setPixelColor(uint32_t color){
  strip.setPixelColor(0, color);
  strip.show();
}

void offPixel(){
  strip.setPixelColor(0, 0, 0, 0);
  strip.show();
}

void blinkPixel(uint32_t color, uint8_t count, uint16_t wait, uint16_t waitOff){
  uint32_t oldColor = strip.getPixelColor(0);
  for(uint8_t c=0; c<count; c++){
    setPixelColor(color);
    delay(wait);
    offPixel();
    delay(waitOff);
  }
  setPixelColor(oldColor);
}

void blinkPixel(uint32_t color, uint8_t count, uint16_t wait){
  blinkPixel(color, count, wait, wait);
}

/* Helper functions */

// Create a 24 bit color value from R,G,B
uint32_t Color(byte r, byte g, byte b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

/** write int32 c to rgb array */
void Decolor(uint32_t c, byte *rgb){
  rgb[0] = c >> 16;
  rgb[1] = c << 16 >> 24;
  rgb[2] = c << 24 >> 24;
}

/** darken color to bright 0.0 to 1.0 */ 
uint32_t setColorBright(uint32_t c, float bright){
  byte rgb[3];
  Decolor(c, rgb);
  r = rgb[0];
  g = rgb[1];
  b = rgb[2];
  uint32_t cb = Color(round(r*bright), round(g*bright), round(b*bright));
  /*Serial.print(r);
  Serial.print(",");
  Serial.print(g);
  Serial.print(",");
  Serial.print(b);
  Serial.print(" to ");
  Serial.print(round(r*bright));
  Serial.print(",");
  Serial.print(round(g*bright));
  Serial.print(",");
  Serial.print(round(b*bright));
  Serial.println("");*/
  return cb;
}


//void lcdPrint(const char c[]){
void lcdPrint(String c){
  lcd.clear();
  if(c==""){
    lcd.noBacklight();
  }
  else{
    lcd.backlight();
    if(c.indexOf("\\")==-1){
      lcd.print(c);
    }
    else{
      lcd.print(firstPart(c, '\\'));
      c = c.substring(c.indexOf("\\")+1);
      lcd.setCursor(0, 1);
      lcd.print(firstPart(c, '\\'));
    }
  }
}

/** 0 - blue, 100 - red */
uint32_t getPixelTemp(float grad){
  uint8_t r=0, g=0, b=255;

  if(grad<25){
    r = 0;
    g = round(grad/25.0 * 255);
    b = 255;
  }
  else if(grad<50){
    r = 0;
    g = 255;
    b = round((50-grad)/25.0 * 255);
  }
  else if(grad<75){
    r = round((grad-50)/25.0 * 255);
    g = 255;
    b = 0;
  }
  else if(grad<=100){
    r = 255;
    g = round((100-grad)/25.0 * 255);;
    b = 0;
  }
  /*Serial.print(grad);
  Serial.print(" - ");
  Serial.print(r);
  Serial.print(", ");
  Serial.print(g);
  Serial.print(", ");
  Serial.print(b);
  Serial.print(" - ");
  Serial.print(Color(r, g, b));
  Serial.println();*/
  float bright = 1;
  return Color(r*bright, g*bright, b*bright);
}

void test_lcd(){
  delay(100);
  lcd.backlight();
  //lcd.autoscroll();
  for(int i=1; i<15; i++){
    lcd.print(i);
    lcd.print(" ");
    delay(200);
  }
  for(int i=1; i<10; i++){
    lcd.scrollDisplayLeft();
    delay(200);
  }
  delay(2000);
  for(int i=1; i<10; i++){
    lcd.scrollDisplayRight();
    delay(200);
  }

  //delay(2000);
  //lcd.clear();
  //lcd.noBacklight();
}

void test(){
  float i;
  uint16_t wait = 3;
  
  for(i=1; i<100; i+=0.05){
    //setPixelLife(i);
    setPixelColor(getPixelTemp(i));
    delay(wait);
    //if((int)i%50==0) blinkPixel(getPixelTemp(i), 3, waitBlink);
  }
  blinkPixel(getPixelTemp(100), 3, waitBlink);
  for(i=100; i>0; i-=0.01){
    setPixelColor(getPixelTemp(i));
    delay(wait);
    //if((int)i%50==0) blinkPixel(getPixelTemp(i), 3, waitBlink);
  }
  blinkPixel(getPixelTemp(0), 3, waitBlink);
}
