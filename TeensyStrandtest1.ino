#include <Adafruit_NeoPixel.h>
#include <avr/power.h>

/* Teensy Serial BT pins are Serial 3 */
#define PIN 17

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(29, PIN, NEO_GRB + NEO_KHZ400);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.

uint8_t color1[3];
uint8_t color2[3];
uint8_t timedelay;
uint8_t modetype;
uint8_t brightval;
uint8_t direction = 0;
uint8_t colorhead = 0;
uint8_t colortail = 0;

enum{
    color = 0,
    colorspinner,
    theatrechaser,
    colorswipe,
    colordim,
    colorpulse,
    rainbowscene,
    primaryswipe,
    larsonscanner,
    greenlarsonscanner = 'a'
};

void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  //Setup BT Serial
  Serial3.begin(115200);
  Serial.begin(9600);//computer interface

  color1[0] = 0x00;
  color1[1] = 0x00;
  color1[2] = 0x00;

  color2[0] = 0x00;
  color2[1] = 0x00;
  color2[2] = 0x00;

  modetype=0;

  timedelay = 0x00;
}


/* Message Structure */
//Color1
//Color2
//Delay
//Type

uint8_t val = 0;
void loop() {
  if(Serial3.available()>9){
    val = Serial3.read();
    if(val == '#'){
        //Serial3.print("Gotcha\n");
        color1[0] = Serial3.read();
        Serial3.print(color1[0]);

        color1[1] = Serial3.read();
        Serial3.print(color1[1]);

        color1[2] = Serial3.read();
        Serial3.print(color1[2]);

        color2[0] = Serial3.read();
        Serial3.print(color2[0]);

        color2[1] = Serial3.read();
        Serial3.print(color2[1]);

        color2[2] = Serial3.read();
        Serial3.print(color2[2]);

        timedelay = Serial3.read();
        Serial3.print(timedelay);

        modetype = Serial3.read();
        Serial3.print(modetype);

        //setup the task
        switch(modetype){
            case color:
                //Nothing to do
                break;
            case colorspinner:

                //setup head and tail
                colorhead = 0;
                colortail = 14;
                break;
            case colorswipe:
                break;
            case colordim:
                brightval = 0;
                break;
            case colorpulse:
                brightval = 0;
                direction = 0;
                break;
            case rainbowscene:
                break;
            case primaryswipe:
                break;
            case larsonscanner:
                break;
            case greenlarsonscanner:
                break;
            default:
                ;
        }//end switch case

        //update the strand to maxbright
        strip.setBrightness(255);
        strip.show();
    }//endif val == '#'
  }//endif serial is available

    //Do the task
    switch(modetype){
        case color:
            colorWipe(strip.Color(color1[0], color1[1], color1[2]), 1);
            break;
        case colorspinner:
            //update head and tail
            updateSpinner(timedelay); //default 50 msec spinner
            break;
        case colorswipe:
            colorWipe(strip.Color(color1[0], color1[1], color1[2]), timedelay);
            break;
        case colordim:
            strip.setBrightness(brightval);
            setStrandColor(strip.Color(color1[0], color1[1], color1[2]));
            brightval = (brightval+2)%255;
            break;
        case colorpulse:
            strip.setBrightness(brightval);
            setStrandColor(strip.Color(color1[0], color1[1], color1[2]));
            if(direction  == 1){
                if(brightval < 170){
                    brightval = (brightval+2)%255;
                }
                else{
                    direction = 0;
                }
            }else{
                if(brightval > 0){
                    brightval -= 2;
                }
                else{
                    direction = 1;
                }
            }

            break;
        case rainbowscene:
            rainbow(timedelay);
            break;
        case primaryswipe:
            Serial3.print("Swiping");
            //red
            colorWipe(strip.Color(255, 0, 0), timedelay);
            //green
            colorWipe(strip.Color(0, 255, 0), timedelay);
            //blue
            colorWipe(strip.Color(0, 0, 255), timedelay);
            //cyan
            colorWipe(strip.Color(255, 255, 0), timedelay);
            //magenta
            colorWipe(strip.Color(0, 255, 255), timedelay);
            //yellow
            colorWipe(strip.Color(255, 0, 255), timedelay);
            //white
            colorWipe(strip.Color(255, 255, 255), timedelay);
            break;
        case larsonscanner:
            setStrandColor(strip.Color(0,0,0));
            updateLarsonScanner(25);
            break;
        case greenlarsonscanner:
            setStrandColor(strip.Color(0,0,0));
            updateGreenLarsonScanner(25);
            break;
        default:
            ;
    }//end switch case

    delay(20);
}//end loop

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

void setStrandColor(uint32_t c){
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
  }
  strip.show();
}

//void setStrand(){
//  for(uint16_t i=0; i<strip.numPixels(); i++) {
//      strip.setPixelColor(i, strip.Color(stripcolors[i*3], stripcolors[i*3+1], stripcolors[i*3+2]));
//  }
//  strip.show();

//}


void setStrandRange(uint8_t start, uint8_t stop, uint16_t c){
    for(uint16_t i=start;i<stop;i++){
        strip.setPixelColor(i,c);
    }
    strip.show();
}

void updateSpinner(uint8_t wait){
    strip.setPixelColor(colorhead, strip.Color(color1[0], color1[1], color1[2]));
    strip.setPixelColor(colortail, strip.Color(color2[0], color2[1], color2[2]));
    colorhead = (colorhead+1)%30;
    colortail = (colortail+1)%30;
    strip.show();
    delay(wait);
}

void updateLarsonScanner(uint8_t wait){

    //update head location
    if(direction == 1){
        if(colorhead < 21){
            colorhead += 1;
        }
        else{
            direction = 0;
            colorhead = 20;
        }

    }else{
        if(colorhead > 7){
            colorhead -= 1;
        }else{
            direction = 1;
            colorhead = 8;
        }
    }

    if(direction == 1){
        if(colorhead > 8){
            strip.setPixelColor(colorhead-1, strip.Color(color1[0]>>1, color1[1]>>1, color1[2]>>1));
        }
        if(colorhead > 9){
            strip.setPixelColor(colorhead-2, strip.Color(color1[0]>>3, color1[1]>>3, color1[2]>>3));
        }
        if(colorhead > 10){
            strip.setPixelColor(colorhead-3, strip.Color(color1[0]>>5, color1[1]>>5, color1[2]>>5));
        }
    }
    if(direction == 0){
        if(colorhead < 20){
            strip.setPixelColor(colorhead+1, strip.Color(color1[0]>>1, color1[1]>>1, color1[2]>>1));
        }
        if(colorhead < 19){
            strip.setPixelColor(colorhead+2, strip.Color(color1[0]>>3, color1[1]>>3, color1[2]>>3));
        }
        if(colorhead < 18){
            strip.setPixelColor(colorhead+3, strip.Color(color1[0]>>5, color1[1]>>5, color1[2]>>5));
        }
    }

    strip.setPixelColor(colorhead, strip.Color(color1[0], color1[1], color1[2]));

    strip.show();

    delay(wait);
}
void updateGreenLarsonScanner(uint8_t wait){

    //update head location
    if(direction == 1){
        if(colorhead < 21){
            colorhead += 1;
        }
        else{
            direction = 0;
            colorhead = 20;
        }

    }else{
        if(colorhead > 7){
            colorhead -= 1;
        }else{
            direction = 1;
            colorhead = 8;
        }
    }

    if(direction == 1){
        if(colorhead > 8){
            strip.setPixelColor(colorhead-1, strip.Color(color1[0]>>1, color1[1]>>1, color1[2]>>1));
        }
        if(colorhead > 9){
            strip.setPixelColor(colorhead-2, strip.Color(color1[0]>>3, color1[1]>>3, color1[2]>>3));
        }
        if(colorhead > 10){
            strip.setPixelColor(colorhead-3, strip.Color(color1[0]>>5, color1[1]>>5, color1[2])>>5);
        }
    }
    if(direction == 0){
        if(colorhead < 20){
            strip.setPixelColor(colorhead+1, strip.Color(color1[0]>>1, color1[1]>>1, color1[2]>>1));
        }
        if(colorhead < 19){
            strip.setPixelColor(colorhead+2, strip.Color(color1[0]>>3, color1[1]>>3, color1[2]>>3));
        }
        if(colorhead < 18){
            strip.setPixelColor(colorhead+3, strip.Color(color1[0]>>5, color1[1]>>5, color1[2]>>5));
        }
    }

    strip.setPixelColor(colorhead, strip.Color(color1[0], color1[1], color1[2]));

    strip.show();

    delay(wait);
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();
     
      delay(wait);
     
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
        }
        strip.show();
       
        delay(wait);
       
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, 0);        //turn every third pixel off
        }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else if(WheelPos < 170) {
    WheelPos -= 85;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}

