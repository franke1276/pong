//www.elegoo.com
//2016.12.9

//We always have to include the library
#include "LedControl.h"

/*
 Now we need a LedControl to work with.
 ***** These pin numbers will probably not work with your hardware *****
 pin 12 is connected to the DataIn 
 pin 11 is connected to LOAD(CS)
 pin 10 is connected to the CLK 
 We have only a single MAX72XX.
 */
LedControl lc=LedControl(12,10,11,1);

/* we always wait a bit between updates of the display */
unsigned long delaytime1=500;
unsigned long delaytime2=50;

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


byte ballPos[2]={BALL_START_POS_COL,BALL_START_POS_ROW};
int bouncerPos = BOUNCER_START_POS;
byte ballVector[2]={BALL_START_VECTOR_COL,BALL_START_VECTOR_ROW};


void setup() {
  Serial.begin(9600);
  
  lc.shutdown(0,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0,8);
  /* and clear the display */
  lc.clearDisplay(0);

  pinMode(buttonApin, INPUT_PULLUP);  
  pinMode(buttonBpin, INPUT_PULLUP);  
}




void render(){
  lc.clearDisplay(0);
  // ball
  lc.setLed(0,ballPos[0],ballPos[1],true);  

  // bouncer
  lc.setLed(0,7,bouncerPos,true);  
  lc.setLed(0,7,bouncerPos + 1,true);  
  lc.setLed(0,7,bouncerPos + 2,true);  
  
}

unsigned long lastRead = millis();

void handleBouncerHit() {
  if (ballPos[COL] == 6){
    if (ballPos[ROW] == bouncerPos || ballPos[ROW] == bouncerPos + 1 || ballPos[ROW] == bouncerPos + 2) {
      // middle hit
      ballVector[COL] = ballVector[COL] * -1;
    }
    if ((ballPos[ROW] == (bouncerPos + 3)) && (ballVector[ROW] == 255)) {
      // right edge hit
      Serial.println("right edge hit");
      ballVector[COL] = ballVector[COL] * -1;
      ballVector[ROW] = 1;
    }
    if ((ballPos[ROW] == (bouncerPos - 1)) && (ballVector[ROW] == 1)) {
      // right edge hit
      Serial.println("left edge hit");
      ballVector[COL] = ballVector[COL] * -1;
      ballVector[ROW] = -1;
    }
    
  }
}

void handleWallHit() {
  if (ballPos[ROW] >= 7 ){
    ballVector[ROW] = ballVector[ROW] * -1;
  }

  if (ballPos[ROW] <= 0 ){
    ballVector[ROW] = ballVector[ROW] * -1;
  }
}

void handleHitA(){
  if (ballPos[0] <= 0) {
    ballPos[COL]=BALL_START_POS_COL;
    ballPos[ROW]=BALL_START_POS_ROW;
    bouncerPos=BOUNCER_START_POS;
    ballVector[COL] = BALL_START_VECTOR_COL;
    ballVector[ROW] = BALL_START_VECTOR_ROW;
  }
}

void handleHitB() {
  if (ballPos[COL] >=7) {
    ballPos[COL]=BALL_START_POS_COL;
    ballPos[ROW]=BALL_START_POS_ROW;
    bouncerPos=BOUNCER_START_POS;
    ballVector[COL] = BALL_START_VECTOR_COL;
    ballVector[ROW] = BALL_START_VECTOR_ROW;
  }
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

  
  if ((currentMillis - lastRead) > 250) {

    int a = digitalRead(buttonApin);  

    if (a == LOW) {
      if (bouncerPos + 1 < 6) {
        bouncerPos += 1;
      }
    }

    int b = digitalRead(buttonBpin);
  
    if (b == LOW) {
      if (bouncerPos - 1 >= 0) {
        bouncerPos -= 1;
      }
    }
    lastRead = currentMillis;
  } 
  
  render();
}
