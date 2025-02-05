#include "Arduino_GigaDisplay_GFX.h"
#include "Arduino_GigaDisplayTouch.h"
#include <ArrayList.h>

Arduino_GigaDisplayTouch touchDetector;
GigaDisplay_GFX display;

// Colors
#define WHITE 0xffff
#define BLACK 0x0000
#define BLUE  0x001F
#define CORNFLOWERBLUE 0x649D
#define GREEN 0x07E0
#define RED 0xF800
#define YELLOW 0xFFE0
#define ORANGE 0xFC60
#define PURPLE 0x801F

#define screen_width 800
#define screen_height 480

const char* message = "";
bool firstButtonCorrect = false, gameOver = false;
ArrayList<char> playerSeq;
int touch_x, touch_y;
int sumAnswer = 0, level = 0, numbersDisplayed = 0;
int text_width, text_height;
// Font size multiplier
int text_size = 4; // Adjust as needed
// Character width and height (default font is 6x8 pixels per character)
int char_width = 6 * text_size; // Width of a single character
int char_height = 8 * text_size; // Height of a single character
int lastTouch, threshold = 250;
// Main Menu Button x Position
int mmButtonX = (screen_width / 2) - (250 / 2);

// Functions
void loadLevel() {
  if (!gameOver) {
    if (level == 1) {
      message = "Press the green square.";
      display.fillScreen(BLUE);
      display.fillRect(0, 0, 50, 50, GREEN);
    } else if (level == 2) {
      message = "Touch the shapes in order from smallest to largest";
      display.fillScreen(GREEN);
      display.fillRoundRect(300, 300, 100, 50, 45, BLUE);
      display.fillCircle(600, 100, 45, YELLOW);
      // Drawing a properly oriented triangle
      display.fillTriangle(100, 100, 200, 100, 150, 200, ORANGE);
    } else if (level == 3) {
      message = "Touch the bee!";
      display.fillScreen(PURPLE);
      display.setCursor(320, 320);
      display.setTextSize(10);
      display.print("B");

      // Draw a Bee
      // Draw body
      display.fillCircle(120, 80, 30, YELLOW);

      // Draw black lines (rings on the body)
      int bodyRadius = 30; // Radius of the body
      int stripeWidth = 5; // Width of each stripe
      display.fillRect(120 - bodyRadius, 100 - 10, 2 * bodyRadius, stripeWidth, BLACK); // Middle stripe
      //display.fillRect(120 - bodyRadius, 85 - 20, 2 * bodyRadius, stripeWidth, BLACK); // Top stripe
      display.fillRect(128 - bodyRadius, 110 - 10, 2 * bodyRadius - 15, stripeWidth - 2, BLACK);  // Bottom stripe

      // Draw wings
      int wingRadius = 30 / 2;
      display.fillCircle(120 - wingRadius, 80 - 30, wingRadius, WHITE);
      display.fillCircle(120 + wingRadius, 80 - 30, wingRadius, WHITE);

      // Draw eyes (small black circles)
      int eyeOffset = 30 / 2;
      display.fillCircle(120 - eyeOffset, 80 - 30 / 4, 5, BLACK);
      display.fillCircle(120 + eyeOffset, 80 - 30 / 4, 5, BLACK);

      // Draw smile (using short line segments)
      int smileY = 85;
      display.drawLine(120 - 5, smileY, 120, smileY + 2, BLACK); // Left curve
      display.drawLine(120, smileY + 2, 120 + 5, smileY, BLACK); // Right curve
    } else if (level == 4) {
        display.fillScreen(CORNFLOWERBLUE);
        if (!firstButtonCorrect) {
          playerSeq.clear();
          message = "Press Orange, Green, Purple, and then Blue";
        } 
        // Orange Button
        display.fillCircle(340, 160, 45, ORANGE);
        // Green Button
        display.fillCircle(480, 80, 45, GREEN);
        // Purple Button 
        display.fillCircle(55, 300, 45, PURPLE);
        // Blue Button
        display.fillCircle(450, 340, 45, BLUE);
      } else if (level == 5) {
        // LEVEL 5
        display.fillScreen(ORANGE);
        // Show user 3 random numbers quickly
        if (numbersDisplayed != 3) {
          long randomValue = random(99);
          char buffer[4];  // Temp buffer to hold the string
          sprintf(buffer, "%ld", randomValue);
          message = strdup(buffer);  // Allocate new memory
          sumAnswer += randomValue;
        } else {
          display.setTextColor(BLUE);
          // Display possible answers for selection
          // Correct Answer
          display.setCursor(50, 25);
          display.print(sumAnswer);

          int pos_x[] = {screen_width - 125, 50, screen_width - 125};
          int pos_y[] = {25, 325, 325};
          for (int i = 0; i < 3; i++) {
            int wrongSel = random(sumAnswer-15, sumAnswer+15);
            if (wrongSel == sumAnswer) {
              continue;
            } else {
              // Incorrect Answers
              display.setCursor(pos_x[i], pos_y[i]);
              display.print(wrongSel);
            }
          }
        }
      } else if (level == 6) {
        message = "You Won!";
        display.fillScreen(GREEN);
      } else {
        // Default - Main Menu
          message = "Touch Anywhere to Begin";
          display.fillScreen(WHITE);
          display.setTextColor(BLACK);
      } 
  } else {
    display.fillScreen(BLACK);
    message = "GAME OVER";
    display.setTextColor(RED);
    firstButtonCorrect = false;
    numbersDisplayed = 0;
    sumAnswer = 0;
  }
  // Calculate text dimensions
  text_width = strlen(message) * char_width;
  text_height = char_height;
  // Calculate centered position
  int x = (screen_width - text_width) / 2;
  int y = (screen_height - text_height) / 2;

  // If the text width exceeds the screen width, split into two lines
  if (text_width > screen_width) {
    int mid_point = strlen(message) / 2;
    
    // Print first part
    display.setCursor((screen_width - mid_point * char_width) / 2, y);
    display.print(String(message).substring(0, mid_point));

    // Print second part
    display.setCursor((screen_width - (strlen(message) - mid_point) * char_width) / 2, y + char_height);
    display.print(String(message).substring(mid_point));
  } else {
    // Display the message centered
    display.setTextSize(text_size);
    display.setCursor(x, y);
    if (!gameOver) {
      display.setTextColor(BLACK);
    }
    display.print(message);
  }

  // After displaying 3 numbers, ask player level prompt
  if (!gameOver && level == 5 && message != "What is the sum?") {
    delay(500);
    message = "What is the sum?";
    numbersDisplayed++;
    loadLevel();
  }

  // In Game Buttons
  if (level > 0) {
    // Draw the button
    display.fillRect(mmButtonX, 425, 250, 100, RED);

    // Approximate text width
    text_width = strlen("Main Menu") * char_width;

    // Center the text inside the button
    int textPosX = mmButtonX + (250 - text_width) / 2;
    int textPosY = 405 + (100 - text_height) / 2;

    // Print the text
    display.setCursor(textPosX, textPosY);
    display.setTextColor(BLACK);
    display.print("Main Menu");
  }
}

bool checkTouch(int x, int y, int width, int height, int radius = 0) {
    if (radius > 0) {
        // Circle detection: Check if touch is within the radius
        return ((touch_x - x) * (touch_x - x) + (touch_y - y) * (touch_y - y)) <= (radius * radius);
    } else {
        // Rectangle detection: Check if touch is within the bounding box
        return (touch_x >= x && touch_x <= (x + width) && touch_y >= y && touch_y <= (y + height));
    }
}

void setup() {
  Serial.begin(9600);
  display.begin();
  display.setRotation(1);
  display.setTextSize(text_size);

  if (touchDetector.begin()) {
    Serial.print("Touch controller init - OK");
  } else {
    Serial.print("Touch controller init - FAILED");
    while(1);
  }
  playerSeq.clear();
  loadLevel();
}

void loop() {
  uint8_t contacts;
  GDTpoint_t points[5];
  contacts = touchDetector.getTouchPoints(points);
  
  // Only 1 tap allowed at a time
  if (contacts == 1 && (millis() - lastTouch > threshold)) {
    //record the x, y coordinates 
    touch_x = points[0].x;
    touch_y = points[0].y;

    // Detect Level Based Code (Player Input)
    switch(level) {
      // Main Menu Screen
      case 0: 
        // Touch Anywhere on screen to continue
        if (touch_x < screen_width && touch_y < screen_height) {
          level++;
          loadLevel();
          break;
        }
      // Level 1
      case 1:
        if (checkTouch(0, 0, 50, 50, 0)) {
          level++;
          loadLevel();
          break;
        } 
      // Level 2
      case 2: { // Orange Triangle
        // Calculate areas of the full triangle and the sub-triangles formed by the point and the triangle vertices
        int x1 = 100, y1 = 100;
        int x2 = 200, y2 = 100;
        int x3 = 150, y3 = 200;
        
        float A = abs((x1*(y2 - y3) + x2*(y3 - y1) + x3*(y1 - y2)) / 2.0);
        float A1 = abs((touch_x*(y2 - y3) + x2*(y3 - touch_y) + x3*(touch_y - y2)) / 2.0);
        float A2 = abs((x1*(touch_y - y3) + touch_x*(y3 - y1) + x3*(y1 - touch_y)) / 2.0);
        float A3 = abs((x1*(y2 - touch_y) + x2*(touch_y - y1) + touch_x*(y1 - y2)) / 2.0);

        // Blue Round Rectangle
        if (checkTouch(300, 300, 100, 50, 0)) {
          if (playerSeq.size() == 0) {
            playerSeq.add('B');
            break;
          } // Yellow Circle
        } else if (checkTouch(600, 100, 0, 0, 45) && playerSeq.size() == 1) {
          playerSeq.add('Y');
          break;
          // Orange Triangle
        } else if (A == A1 + A2 + A3 && playerSeq.size() == 2) {
          level++;
          loadLevel();
          break;
        } 
      }
      case 3: {
        // Check if pressed the bee on first try
        if (checkTouch(90, 50, 60, 60)) {
          level++;
          loadLevel();
          break;
        } 
      }
      // Level 4
      case 4: {
        if (playerSeq.size() == 0 && (touch_x - 340) * (touch_x - 340) + (touch_y - 160) * (touch_y - 160) <= 45 * 45) {
          message = "";
          firstButtonCorrect = true;
          playerSeq.add('O');
          loadLevel();
          break;
        } else if (playerSeq.size() == 1 && (touch_x - 480) * (touch_x - 480) + (touch_y - 80) * (touch_y - 80) <= 45 * 45) {
          playerSeq.add('G');
          loadLevel();
          break;
        } else if (playerSeq.size() == 2 && (touch_x - 55) * (touch_x - 55) + (touch_y - 300) * (touch_y - 300) <= 45 * 45) {
          playerSeq.add('P');
          loadLevel();
          break;
        } else if (playerSeq.size() == 3 && (touch_x - 450) * (touch_x - 450) + (touch_y - 340) * (touch_y - 340) <= 45 * 45) {
          level++;
          playerSeq.clear();
          loadLevel();
          break;
        } 
      } 
      case 5: {
        if (checkTouch(50, 25, 50, 70, 0)) {
            level++;
            loadLevel();
            break;
        } 
        // Every level game over 
        else if (touch_x < screen_width && touch_y < screen_height) {
          gameOver = true;
          loadLevel();
        }
      }
      // Display main menu button on levels only
      default: {
        if (level > 0 && checkTouch(mmButtonX, 425, 250, 100, 0)) {
          level = 0;
          firstButtonCorrect = false;
          numbersDisplayed = 0;
          sumAnswer = 0;
          playerSeq.clear();
          gameOver = false;
          loadLevel();
          display.setTextColor(BLACK);
          break;
        }
      } 
    } // End Switch Cases
    lastTouch = millis();
  }
}
