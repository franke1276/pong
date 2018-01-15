//www.elegoo.com
//2016.12.9

//We always have to include the library
#include "LedControl.h"

LedControl lc = LedControl(12, 10, 11, 1);

/* we always wait a bit between updates of the display */
unsigned long delaytime1 = 500;
unsigned long delaytime2 = 50;

int buttonApin = 3;
int buttonBpin = 4;

unsigned long previousMillis = millis();


const int COL = 0;
const int ROW = 1;

const int BOUNCER_START_POS = 4;

const int BALL_START_POS_COL = 4;
const int BALL_START_POS_ROW = 1;

const int BALL_START_VECTOR_COL = 1;
const int BALL_START_VECTOR_ROW = 1;


int ballPos[2] = {BALL_START_POS_COL, BALL_START_POS_ROW};
int bouncerPos = BOUNCER_START_POS;
int ballVector[2] = {BALL_START_VECTOR_COL, BALL_START_VECTOR_ROW};


int buttonAState = HIGH;
int buttonBState = HIGH;
int lastButtonAState = LOW;
int lastButtonBState = LOW;
unsigned long lastDebounceATime = 0;
unsigned long lastDebounceBTime = 0;
unsigned long debounceDelay = 50;


void setup() {
  Serial.begin(9600);

  lc.shutdown(0, false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0, 8);
  /* and clear the display */
  lc.clearDisplay(0);

  pinMode(buttonApin, INPUT_PULLUP);
  pinMode(buttonBpin, INPUT_PULLUP);
}




void render() {
  lc.clearDisplay(0);
  // ball
  lc.setLed(0, ballPos[0], ballPos[1], true);

  // bouncer
  lc.setLed(0, 7, bouncerPos, true);
  lc.setLed(0, 7, bouncerPos + 1, true);
  lc.setLed(0, 7, bouncerPos + 2, true);

}

unsigned long lastRead = millis();

void handleBouncerHit() {
  if (ballPos[COL] == 6) {
    if (ballPos[ROW] == bouncerPos || ballPos[ROW] == bouncerPos + 1 || ballPos[ROW] == bouncerPos + 2) {
      // middle hit
      ballVector[COL] = ballVector[COL] * -1;
    }
    if ((ballPos[ROW] == (bouncerPos + 3)) && (ballVector[ROW] == -1)) {
      // right edge hit
      ballVector[COL] = ballVector[COL] * -1;
      ballVector[ROW] = 1;
    }
    if ((ballPos[ROW] == (bouncerPos - 1)) && (ballVector[ROW] == 1)) {
      // right edge hit
      ballVector[COL] = ballVector[COL] * -1;
      ballVector[ROW] = -1;
    }

  }
}

void handleWallHit() {
  if (ballPos[ROW] >= 7 ) {
    ballVector[ROW] = ballVector[ROW] * -1;
  }

  if (ballPos[ROW] <= 0 ) {
    ballVector[ROW] = ballVector[ROW] * -1;
  }
}

void handleHitA() {
  if (ballPos[0] <= 0) {
    ballPos[COL] = BALL_START_POS_COL;
    ballPos[ROW] = BALL_START_POS_ROW;
    bouncerPos = BOUNCER_START_POS;
    ballVector[COL] = BALL_START_VECTOR_COL;
    ballVector[ROW] = BALL_START_VECTOR_ROW;
  }
}

void handleHitB() {
  if (ballPos[COL] >= 7) {
    ballPos[COL] = BALL_START_POS_COL;
    ballPos[ROW] = BALL_START_POS_ROW;
    bouncerPos = BOUNCER_START_POS;
    ballVector[COL] = BALL_START_VECTOR_COL;
    ballVector[ROW] = BALL_START_VECTOR_ROW;
  }
}

void handlePuttons() {
    int a = digitalRead(buttonApin);
  if (a != lastButtonAState) {
    // reset the debouncing timer
    lastDebounceATime = millis();
  }
  if ((millis() - lastDebounceATime) > debounceDelay) {
    if (a != buttonAState) {
      buttonAState = a;

      Serial.print("buttonAState: ");
      Serial.println(buttonAState);
      if (buttonAState == LOW) {
        if (bouncerPos + 1 < 6) {
          bouncerPos += 1;
        }
      }
    }
  }
  lastButtonAState = a;

  int b = digitalRead(buttonBpin);
  if (b != lastButtonBState) {
    // reset the debouncing timer
    lastDebounceBTime = millis();
  }
  if ((millis() - lastDebounceBTime) > debounceDelay) {
    if (b != buttonBState) {
      buttonBState = b;
      Serial.print("buttonBState: ");
      Serial.println(buttonBState);
      if (buttonBState == LOW) {
        if (bouncerPos - 1 >= 0) {
          bouncerPos -= 1;
        }
      }
    }
  }
  lastButtonBState = b;

}

void loop() {

  unsigned long currentMillis = millis();

  if ((currentMillis - previousMillis) > 1000) {
    ballPos[COL] = (ballPos[COL] + ballVector[COL]);
    ballPos[ROW] = (ballPos[ROW] + ballVector[ROW]);
    if (ballPos[COL] > 200) {
      // uebelauf
      ballPos[COL] = 0;
    }

    if (ballPos[ROW] > 200) {
      // uebelauf
      ballPos[ROW] = 0;
    }

    handleBouncerHit();
    handleWallHit();
    handleHitA();
    handleHitB();
    previousMillis = currentMillis;
  }

  handlePuttons();
  render();
}
