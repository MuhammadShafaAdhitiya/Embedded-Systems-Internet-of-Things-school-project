#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

// Display size
#define sWidth 128
#define sHeight 64

#define PI 3.1415926535897932384626433832795

// Pins
const int PinDT = 4;    // DT Pin
const int PinCLK = 2;   // CLK Pin
const int PinSW = 5;    // Button Pin

Adafruit_SSD1306 oled(sWidth, sHeight, &Wire, 4);

const int NUM_ITEMS = 3;

// Rotary variables
int lastCLKState;
int currentCLKState;
int currentDTState;

// Variables
volatile int counter = 0;
int itemSelected = 0;
char msg[] = "This function is currently unavailable";
int msglen = strlen(msg);
int currentScreen = 0;
int screenDisplay;
bool button_select_clicked = 0;

unsigned long lastDebounceTime = 0;
unsigned long buttonLastDebounceTime = 0;
const int debounceDelay = 10; // 5ms debounce delay untuk rotary
const int buttonDelay = 200; // 50ms debounce delay untuk push button

// variables for wave animation
float phase = 0.0;
float freq = 1.0;
float freqStep = 0.5;
const float phaseStep = 0.1;
float fr = 1.0;
const float frStep = 0.5;

void setup() {
  // put your setup code here, to run once:
  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  oled.clearDisplay();

  pinMode(PinDT, INPUT);
  pinMode(PinCLK, INPUT);
  pinMode(PinSW, INPUT);

  lastCLKState = digitalRead(PinCLK);

  // Gunakan interrupt untuk membaca perubahan rotary encoder
  attachInterrupt(digitalPinToInterrupt(PinCLK), readEncoder, CHANGE);
  // Gunakan interrupt untuk membaca perubahan pada button
  attachInterrupt(digitalPinToInterrupt(PinSW), readButton, CHANGE);

}

void loop() {
  // put your main code here, to run repeatedly:

  readEncoder();
  readButton();

  freq = fr;
  freqStep = frStep;
  itemSelected = counter;
  screenDisplay = currentScreen;
  
  if (screenDisplay == 0){
    oled.clearDisplay();
    oled.setTextSize(1);
    oled.setTextColor(WHITE);
    oled.setCursor(30, 0);
    oled.print("Select Menu");

    if (itemSelected == 0){
      oled.setCursor(5, 17);
      oled.setTextColor(WHITE);
      oled.print("> Sine Wave ");      
    }
    else {
      oled.setCursor(5, 17);
      oled.setTextColor(WHITE);
      oled.print("  Sine Wave");
    }

    if (itemSelected == 1){
      oled.setCursor(5, 27);
      oled.setTextColor(WHITE);
      oled.print("> Pulse Wave ");       
    }
    else {
      oled.setCursor(5, 27);
      oled.setTextColor(WHITE);
      oled.print("  Pulse Wave ");         
    }

    if (itemSelected == 2){
      oled.setCursor(5, 37);
      oled.setTextColor(WHITE);
      oled.print("> Triangle Wave ");   
    }
    else {
      oled.setCursor(5, 37);
      oled.setTextColor(WHITE);
      oled.print("  Triangle Wave ");   
    }
  }

  else if (screenDisplay == 1){
    oled.clearDisplay();

    if (itemSelected == 0){
      // Display current frequency
      oled.setCursor(0, 0);
      oled.print("Freq: ");
      oled.print(freq);
      oled.print(" Hz");


      // Draw animated sine wave
      for (int x = 0; x < sWidth; x++) {
        // Calculate y position with phase shift for animation
        float y = (sHeight/2) + (sin(2 * PI * freq * x / sWidth + phase) * (sHeight/3));
    
        // Draw current point
        oled.drawPixel(x, (int)y, SSD1306_WHITE);
    
        // Draw line to next point for continuous wave
        if (x < sWidth-1) {
          float nextY = (sHeight/2) + (sin(2 * PI * freq * (x+1) / sWidth + phase) * (sHeight/3));
          oled.drawLine(x, (int)y, x+1, (int)nextY, SSD1306_WHITE);
        }
      }

      // Update phase for animation
      phase += phaseStep * freq;  // Animation speed proportional to frequency
      if (phase >= 2 * PI) {
      phase -= 2 * PI;  // Keep phase between 0 and 2π
      }

      oled.display();
      delay(20);  // Shorter delay for smoother animation
    }

    if (itemSelected == 1){
      oled.setCursor(0 ,sHeight/2 );
      oled.setTextColor(WHITE);
      oled.print(msg);  
    }

    if (itemSelected == 2){
      oled.setCursor(0 ,sHeight/2 );
      oled.setTextColor(WHITE);
      oled.print(msg);  
    }
  }

  oled.display();
}

void readEncoder() {
  unsigned long currentMillis = millis();
  currentCLKState = digitalRead(PinCLK);
  currentDTState = digitalRead(PinDT);
  
  if (currentScreen == 0){

    if (currentMillis - lastDebounceTime > debounceDelay) {
      if (currentCLKState != lastCLKState) {
      
        //increment atau decrement counter
        counter += (currentDTState != currentCLKState) ? 1 : -1;
      
        // Pastikan counter tetap dalam rentang
        counter = (counter + NUM_ITEMS) % NUM_ITEMS;

        // Untuk debugging
      }
    lastDebounceTime = currentMillis;
    }
    lastCLKState = currentCLKState;
  }

  if(currentScreen == 1){

    if(itemSelected == 0){
      if (currentMillis - lastDebounceTime > debounceDelay){
        if (currentCLKState != lastCLKState) {
          if (currentCLKState != lastCLKState){
            if (currentDTState != currentCLKState){
              fr += frStep;
            }
            else{
              fr -= frStep;
              if(fr < 0.5){
                fr = 0.5;
              }
            }
          }
        lastDebounceTime = currentMillis;
        lastCLKState = currentCLKState;
        }
      }
    }
  }
}

void readButton() {
  
  bool buttonState = digitalRead(PinSW);
  unsigned long buttonMillis = millis();
  if (buttonState == LOW && button_select_clicked == 0 && ( buttonMillis - buttonLastDebounceTime > buttonDelay)) { 
    button_select_clicked = 1; 
    buttonLastDebounceTime = buttonMillis;  // Simpan waktu debounce terakhir

    // Toggle antara 0 dan 1
    currentScreen = (currentScreen == 0) ? 1 : 0;
  }

  if (buttonState == HIGH && button_select_clicked == 1) {  // Jika tombol dilepas
    button_select_clicked = 0;
  }
}
