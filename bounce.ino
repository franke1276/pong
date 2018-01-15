//www.elegoo.com
//2016.12.9

//We always have to include the library
#include "LedControl.h"
#include "pitches.h"
LedControl lc = LedControl(12, 10, 11, 1);

/* we always wait a bit between updates of the display */
unsigned long delaytime1 = 500;
unsigned long delaytime2 = 50;

int melody[] = {
  NOTE_C5, NOTE_D5, NOTE_E5, NOTE_F5, NOTE_G5, NOTE_A5, NOTE_B5, NOTE_C6};


int buttonA1pin = 3;
int buttonA2pin = 4;
int buttonB1pin = 5;
int buttonB2pin = 6;

int soundPin = 8;

unsigned long previousMillis = millis();


const int ROW = 0;
const int COL = 1;

const int BOUNCER_A_START_POS = 4;
const int BOUNCER_B_START_POS = 4;

const int BALL_START_POS_ROW = 4;
const int BALL_START_POS_COL = 1;

const int BALL_START_VECTOR_ROW = 1;
const int BALL_START_VECTOR_COL = 1;


int ballPos[2] = {BALL_START_POS_ROW, BALL_START_POS_COL};
int bouncerAPos = BOUNCER_A_START_POS;
int bouncerBPos = BOUNCER_B_START_POS;
int ballVector[2] = {BALL_START_VECTOR_ROW, BALL_START_VECTOR_COL};

int pointsA = 9;
int pointsB = 0;

int buttonA1State = HIGH;
int buttonA2State = HIGH;
int lastButtonA1State = LOW;
int lastButtonA2State = LOW;
unsigned long lastDebounceA1Time = 0;
unsigned long lastDebounceA2Time = 0;

int buttonB1State = HIGH;
int buttonB2State = HIGH;
int lastButtonB1State = LOW;
int lastButtonB2State = LOW;
unsigned long lastDebounceB1Time = 0;
unsigned long lastDebounceB2Time = 0;


unsigned long debounceDelay = 50;


const byte NUMBERS[10][5] = {
  {B00000111, B00000101, B00000101, B00000101, B00000111},
  {B00000010, B00000010, B00000010, B00000010, B00000010},
  {B00000111, B00000001, B00000111, B00000100, B00000111},
  {B00000111, B00000001, B00000111, B00000001, B00000111},
  {B00000101, B00000101, B00000111, B00000001, B00000001},
  {B00000111, B00000100, B00000111, B00000001, B00000111},
  {B00000111, B00000100, B00000111, B00000101, B00000111},
  {B00000111, B00000001, B00000010, B00000010, B00000010},
  {B00000111, B00000101, B00000111, B00000101, B00000111},
  {B00000111, B00000101, B00000111, B00000001, B00000111}
};

const byte A[8] = {B00000000, B00011000, B00100100, B00100100, B00111100, B00100100, B00100100, B00100100};

const byte B[8] = {B00000000, B00111000, B00100100, B00100100, B00111000, B00100100, B00100100, B00111000};

void showWinner(byte* winner) {
  for (int i = 0; i < 8; i++) {
    lc.setRow(0, i , winner[i]);
  }
}


void showWon(byte* winner) {
  lc.clearDisplay(0);
  for (int i=0; i< 4;i++){
    showWinner(winner); 
    for (int i=0;i<8;i++){
      tone(soundPin,melody[i], 1000/16);
      delay(1000/16 * 1.3);   
    }
    lc.clearDisplay(0);
    delay(500);
  }
}


void showResult() {
  lc.clearDisplay(0);
  if (pointsA > 9) {
    Serial.println("A won!");
    showWon(A);
    pointsA = 0;
    pointsB = 0;
  }
  if (pointsB > 9) {
    Serial.println("B won!");
    showWon(B);
    pointsA = 0;
    pointsB = 0;
  }
  for (int i = 0; i < 5; i++) {
    lc.setRow(0, i + 1, (NUMBERS[pointsA][i] << 5)| (NUMBERS[pointsB][i]));
  }
  tone(soundPin,NOTE_C5, 1000/4);
  delay(2000);
  lc.clearDisplay(0);
}

void setup() {
  Serial.begin(9600);

  lc.shutdown(0, false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0, 8);
  /* and clear the display */
  lc.clearDisplay(0);

  pinMode(buttonA1pin, INPUT_PULLUP);
  pinMode(buttonA2pin, INPUT_PULLUP);
  pinMode(buttonB1pin, INPUT_PULLUP);
  pinMode(buttonB2pin, INPUT_PULLUP);

  pinMode(soundPin, OUTPUT);

}


int prevBouncerAPos = 0;
int prevBouncerBPos = 0;

int prevBallPos[2] = {BALL_START_POS_ROW, BALL_START_POS_COL};


void render() {
  
  lc.setLed(0, 0, prevBouncerBPos, false);
  lc.setLed(0, 0, prevBouncerBPos + 1, false);
  lc.setLed(0, 0, prevBouncerBPos + 2, false);

  lc.setLed(0, 0, bouncerBPos, true);
  lc.setLed(0, 0, bouncerBPos + 1, true);
  lc.setLed(0, 0, bouncerBPos + 2, true);

  
  lc.setLed(0, 7, prevBouncerAPos, false);
  lc.setLed(0, 7, prevBouncerAPos + 1, false);
  lc.setLed(0, 7, prevBouncerAPos + 2, false);
  
  lc.setLed(0, 7, bouncerAPos, true);
  lc.setLed(0, 7, bouncerAPos + 1, true);
  lc.setLed(0, 7, bouncerAPos + 2, true);

  
  lc.setLed(0, prevBallPos[ROW], prevBallPos[COL], false);
  // ball
  lc.setLed(0, ballPos[ROW], ballPos[COL], true);
  prevBouncerAPos = bouncerAPos;
  prevBouncerBPos = bouncerBPos;
  prevBallPos[ROW] = ballPos[ROW];
  prevBallPos[COL] = ballPos[COL];
}

unsigned long lastRead = millis();



void handleBouncerHit(int bouncerPos, int bouncerHitROW) {
  if (ballPos[ROW] == bouncerHitROW) {
    if (ballPos[COL] == bouncerPos || ballPos[COL] == bouncerPos + 1 || ballPos[COL] == bouncerPos + 2) {
      // middle hit
      ballVector[ROW] = ballVector[ROW] * -1;
    }
    if ((ballPos[COL] == (bouncerPos + 3)) && (ballVector[COL] == -1)) {
      // right edge hit
      ballVector[ROW] = ballVector[ROW] * -1;
      ballVector[COL] = 1;
    }
    if ((ballPos[COL] == (bouncerPos - 1)) && (ballVector[COL] == 1)) {
      // right edge hit
      ballVector[ROW] = ballVector[ROW] * -1;
      ballVector[COL] = -1;
    }
  }
}
void handleAllBouncerHit() {
  handleBouncerHit(bouncerAPos, 6);
  handleBouncerHit(bouncerBPos, 1);
}

void handleWallHit() {
  if (ballPos[COL] >= 7 ) {
    ballVector[COL] = ballVector[COL] * -1;
  }

  if (ballPos[COL] <= 0 ) {
    ballVector[COL] = ballVector[COL] * -1;
  }
}

void handleBLostBall() {
  if (ballPos[0] <= 0) {
    pointsA += 1;
    Serial.print("B lost a ball! ");
    Serial.print(pointsA);
    Serial.print(":");
    Serial.println(pointsB);
    showResult();
    ballPos[ROW] = BALL_START_POS_ROW;
    ballPos[COL] = BALL_START_POS_COL;
    bouncerAPos = BOUNCER_A_START_POS;
    bouncerBPos = BOUNCER_B_START_POS;
    ballVector[ROW] = BALL_START_VECTOR_ROW;
    ballVector[COL] = BALL_START_VECTOR_COL;
  }
}

void handleALostBall() {
  if (ballPos[ROW] >= 7) {
    pointsB += 1;
    Serial.print("A lost a ball! ");
    Serial.print(pointsA);
    Serial.print(":");
    Serial.println(pointsB);
    showResult();
    ballPos[ROW] = BALL_START_POS_ROW;
    ballPos[COL] = BALL_START_POS_COL;
    bouncerAPos = BOUNCER_A_START_POS;
    bouncerBPos = BOUNCER_B_START_POS;
    ballVector[ROW] = BALL_START_VECTOR_ROW;
    ballVector[COL] = BALL_START_VECTOR_COL;
  }
}

void handleButtons() {

  /* ---- A -----*/
  int a1 = digitalRead(buttonA1pin);
  if (a1 != lastButtonA1State) {
    // reset the debouncing timer
    lastDebounceA1Time = millis();
  }
  if ((millis() - lastDebounceA1Time) > debounceDelay) {
    if (a1 != buttonA1State) {
      buttonA1State = a1;
      if (buttonA1State == LOW) {
        if (bouncerAPos + 1 < 6) {
          bouncerAPos += 1;
        }
      }
    }
  }
  lastButtonA1State = a1;

  int a2 = digitalRead(buttonA2pin);
  if (a2 != lastButtonA2State) {
    // reset the debouncing timer
    lastDebounceA2Time = millis();
  }
  if ((millis() - lastDebounceA2Time) > debounceDelay) {
    if (a2 != buttonA2State) {
      buttonA2State = a2;
      if (buttonA2State == LOW) {
        if (bouncerAPos - 1 >= 0) {
          bouncerAPos -= 1;
        }
      }
    }
  }
  lastButtonA2State = a2;


  /* ---- B -----*/


  int b1 = digitalRead(buttonB1pin);
  if (b1 != lastButtonB1State) {
    // reset the debouncing timer
    lastDebounceB1Time = millis();
  }
  if ((millis() - lastDebounceB1Time) > debounceDelay) {
    if (b1 != buttonB1State) {
      buttonB1State = b1;
      if (buttonB1State == LOW) {
        if (bouncerBPos + 1 < 6) {
          bouncerBPos += 1;
        }
      }
    }
  }
  lastButtonB1State = b1;

  int b2 = digitalRead(buttonB2pin);
  if (b2 != lastButtonB2State) {
    // reset the debouncing timer
    lastDebounceB2Time = millis();
  }
  if ((millis() - lastDebounceB2Time) > debounceDelay) {
    if (b2 != buttonB2State) {
      buttonB2State = b2;
      if (buttonB2State == LOW) {
        if (bouncerBPos - 1 >= 0) {
          bouncerBPos -= 1;
        }
      }
    }
  }
  lastButtonB2State = b2;

}

void loop() {

  unsigned long currentMillis = millis();

  if ((currentMillis - previousMillis) > 500) {
    ballPos[ROW] = (ballPos[ROW] + ballVector[ROW]);
    ballPos[COL] = (ballPos[COL] + ballVector[COL]);
    if (ballPos[ROW] > 200) {
      // uebelauf
      ballPos[ROW] = 0;
    }

    if (ballPos[COL] > 200) {
      // uebelauf
      ballPos[COL] = 0;
    }

    handleAllBouncerHit();
    handleWallHit();
    handleALostBall();
    handleBLostBall();
    previousMillis = currentMillis;
    tone(soundPin,NOTE_C6, 1000/16);
  }

  handleButtons();
  render();
}
