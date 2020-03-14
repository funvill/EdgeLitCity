#include <FastLED.h>

FASTLED_USING_NAMESPACE

// FastLED "100-lines-of-code" demo reel, showing just a few 
// of the kinds of animation patterns you can quickly and easily 
// compose using FastLED.  
//
// This example also shows one easy way to define multiple 
// animations patterns and have them automatically rotate.
//
// -Mark Kriegsman, December 2014

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    D4
//#define CLK_PIN   4
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    100
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          255 
#define FRAMES_PER_SECOND  120

void city();

/**
 *                                   *-----*
 *                               ==> |00-02|  
 *                                   |05-03|  <==
 *                                   *-----*
 *                        *-----*    *-----*    *-----*
 *                  ==>   |06-08|    |09-11| 12 |13-15|
 *                        |26-24| 23 |22-20| 19 |18-16| <==
 *                        *-----*    *-----*    *-----*
 *             *-----*    *-----*    *-----*    *-----*    *-----*
 *         ==> |27-29| 30 |31-33| 34 |35-37|    |38-40| 41 |42-44|
 *             |62-60| 59 |58-56|    |55-53| 52 |51-49| 48 |47-45|  <==
 *             *-----*    *-----*    *-----*    *-----*    *-----*
 *                        *-----*    *-----*    *-----*
 *                    ==> |63-65| 66 |67-69| 70 |71-73|
 *                        |84-82| 81 |80-78| 77 |76-74|  <==
 *                        *-----*    *-----*    *-----*
 *                                   *-----*
 *                               ==> |85-87|
 *                                   |90-88|   <== 
 *                                   *-----*
 *                        
 */

#define LED_PER_BUILDING  3
#define BUILDING_COUNT 5 

CRGB buildingColors[BUILDING_COUNT] ;


void setup() {
  delay(3000); // 3 second delay for recovery
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
  
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { PaletteColors };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
  
void loop()
{
  city(); 
  gPatterns[gCurrentPatternNumber]();  

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_MILLISECONDS( 40 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS( 60 ) { 
    nextPattern(); 
  } // change patterns periodically
  
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void SetBuilding( const byte offsetBuilding, const CRGB color ) {
  if( offsetBuilding > BUILDING_COUNT ) {
    return ; 
  }
  for(byte offsetLed = 0 ; offsetLed < LED_PER_BUILDING ; offsetLed ++ ) {         
    leds[(offsetBuilding * 3) + offsetLed ] = color; 
  }
}

void city() {  
  for(byte offsetBuilding = 0 ; offsetBuilding < BUILDING_COUNT ; offsetBuilding ++ ) {   
    SetBuilding( offsetBuilding, buildingColors[offsetBuilding]); 
  }
}



void SolidColor() {
  for(byte offset = 0; offset < BUILDING_COUNT ; offset++) {
    buildingColors[offset] = gHue ; 
  }  
}


CRGBPalette16 currentPalette = PartyColors_p ; 
TBlendType    currentBlending = LINEARBLEND ; 
extern CRGBPalette16 myRedWhiteBluePalette;
extern CRGBPalette16 myPrimaryColorPalette;

extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

void PaletteColors() {

  ChangePalettePeriodically();
  
  static uint8_t paletteIndex = 0 ;   
  const byte COUNT_ROWS = 5 ;  
  
  for(byte offsetBuilding = 0; offsetBuilding < BUILDING_COUNT ; offsetBuilding++) {
    paletteIndex = ((0xFF / BUILDING_COUNT) * offsetBuilding) + gHue ; 
    SetBuilding( offsetBuilding, ColorFromPalette( currentPalette, paletteIndex, BRIGHTNESS, currentBlending));
  }  // End of offsetRow for loop 
}



void ChangePalettePeriodically()
{   
    static uint8_t palletOffset = 0 ; 
    static long lastUpdated = 0 ; 
    if( lastUpdated + 1000 * 15 < millis() ) {
      lastUpdated = millis(); 
      palletOffset++ ; 

      const byte palletCount = 7 ; 
           if( palletOffset % palletCount == 0 ) { SetupTotallyRandomPalette(); }
      // else if( palletOffset % palletCount == 1 ) { SetupBlackAndWhiteStripedPalette(); }
      // else if( palletOffset % palletCount == 2 ) { SetupPrimaryColorPalette(); }
      else if( palletOffset % palletCount == 3 ) { SetupPurpleAndGreenPalette(); }
      // else if( palletOffset % palletCount == 4 ) { currentPalette = RainbowColors_p; }
      else if( palletOffset % palletCount == 5 ) { currentPalette = PartyColors_p; }
      // else if( palletOffset % palletCount == 6 ) { currentPalette = CloudColors_p; }
      else                                       { SetupTotallyRandomPalette(); }      
    }




  /*
    uint8_t secondHand = (millis() / 1000) % 60;
    static uint8_t lastSecond = 99;
    
    if( lastSecond != secondHand) {
        lastSecond = secondHand;

        
        if( secondHand == 0)   { SetupTotallyRandomPalette();              currentBlending = LINEARBLEND; }
        if( secondHand == 15)  { SetupBlackAndWhiteStripedPalette();      currentBlending = LINEARBLEND; }
        if( secondHand == 30)  { SetupPrimaryColorPalette();              currentBlending = LINEARBLEND;  }
        if( secondHand == 45)  { SetupPurpleAndGreenPalette();            currentBlending = LINEARBLEND; }

        if( secondHand == 30)  { currentPalette = RainbowColors_p;         currentBlending = LINEARBLEND; }
        if( secondHand == 45)  { currentPalette = PartyColors_p;           currentBlending = LINEARBLEND; }
        if( secondHand == 30)  { currentPalette = CloudColors_p;           currentBlending = LINEARBLEND; }
        
   
        

        
        if( secondHand == 15)  { currentPalette = RainbowStripeColors_p;   currentBlending = LINEARBLEND; }
        
        
        // if( secondHand == 30)  { SetupBlackAndWhiteStripedPalette();       currentBlending = NOBLEND; }
        
        // if( secondHand == 50)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = NOBLEND;  }
        if( secondHand == 55)  { currentPalette = myRedWhiteBluePalette_p; currentBlending = LINEARBLEND; }

    }
    */
}

void SetupPrimaryColorPalette() {

/*
    CRGB purple = CHSV( HUE_PURPLE, 255, 255);
    CRGB green  = CHSV( HUE_GREEN, 255, 255);
    CRGB black  = CRGB::Black;
  */
    
    currentPalette = CRGBPalette16(
                                   CRGB::Red,     CRGB::Gray, CRGB::Red,    CRGB::Gray,
                                   CRGB::Green,   CRGB::Gray, CRGB::Green,  CRGB::Gray,
                                   CRGB::Blue,    CRGB::Gray, CRGB::Blue,   CRGB::Gray,
                                   CRGB::Yellow,  CRGB::Gray, CRGB::Yellow, CRGB::Gray );
}

// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette()
{
    for( int i = 0; i < 16; i++) {
        currentPalette[i] = CHSV( random8(), 255, random8());
    }
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette()
{
    // 'black out' all 16 palette entries...
    fill_solid( currentPalette, 16, CRGB::Black);
    // and set every fourth one to white.
    currentPalette[0] = CRGB::White;
    currentPalette[4] = CRGB::White;
    currentPalette[8] = CRGB::White;
    currentPalette[12] = CRGB::White;
    
}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette()
{
    CRGB purple = CHSV( HUE_PURPLE, 255, 255);
    CRGB green  = CHSV( HUE_GREEN, 255, 255);
    CRGB black  = CRGB::Black;
    
    currentPalette = CRGBPalette16(
                                   green,  green,  black,  black,
                                   purple, purple, black,  black,
                                   green,  green,  black,  black,
                                   purple, purple, black,  black );
}


// This example shows how to set up a static color palette
// which is stored in PROGMEM (flash), which is almost always more
// plentiful than RAM.  A static PROGMEM palette like this
// takes up 64 bytes of flash.
const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM =
{
    CRGB::Red,
    CRGB::Gray, // 'white' is too bright compared to red and blue
    CRGB::Blue,
    CRGB::Black,
    
    CRGB::Red,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Black,
    
    CRGB::Red,
    CRGB::Red,
    CRGB::Gray,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Blue,
    CRGB::Black,
    CRGB::Black
};











void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}
