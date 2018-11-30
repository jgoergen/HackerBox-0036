// Core display library provided by the PxMatrix library https://github.com/2dom/PxMatrix
// Basic integration provided by Hackerboxes https://www.instructables.com/id/HackerBox-0036-JumboTron/
#include <PxMatrix.h>

// ################# SETTINGS ###########################################

// instead of using a delay on our loop, we'll be watching the time elapsed so we have independent
// update speeds on our processes.

#define INPUT_WAIT          100 // ~10 fps
#define DISPLAY_WAIT        33 // ~30 fps
#define VERLET_WAIT         10 // ~100 fps
#define VERLET_PRECISION    100

#define MIN_BALLS_IN_SCENE  100
#define MAX_BALLS_IN_SCENE  400
#define IDLE_GRAVITY_X      0
#define IDLE_GRAVITY_Y      0//-0.03

// ################ PINS ################################################

// ESP32 Pins for JoyStick
#define JOY_X 35
#define JOY_Y 34
#define JOY_Z 26
#define KEY_1 27
#define KEY_2 32
#define KEY_3 33
#define KEY_4 25

// ESP32 Pins for LED MATRIX
#define P_LAT 4
#define P_A 19
#define P_B 23
#define P_C 18
#define P_D 5
#define P_E 15  // NOT USED for 1/16 scan
#define P_OE 2

// ################ MISC ################################################

hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
PxMATRIX display(64,32,P_LAT, P_OE,P_A,P_B,P_C,P_D);
uint8_t display_draw_time=0;
float ax, ay, az;
int lastInput = 0;
int lastVerlet = 0;
int lastDisplay = 0;

// ################################# SETUP ###################################

void setup() {

  while(!Serial);
  Serial.begin(115200);
  Serial.println("System starting");

  // setup joystick pins  
  pinMode(JOY_Z, INPUT_PULLUP);
  pinMode(KEY_1, INPUT_PULLUP);
  pinMode(KEY_2, INPUT_PULLUP);
  pinMode(KEY_3, INPUT_PULLUP);
  pinMode(KEY_4, INPUT_PULLUP);

  display_init();
  verlet_init();
  generateScene();
}

// ################################# LOOP ###################################

void loop() {
  
  // read inputs?
  if (millis() - lastInput > INPUT_WAIT) {
    
    lastInput = millis();

    if (digitalRead(KEY_1) == LOW)
      verlet_clearAll();

    if (digitalRead(KEY_2) == LOW)
      generateRandomBall();
      
    if (digitalRead(KEY_3) == LOW)
      generateScene();

    if (digitalRead(KEY_4) == LOW)
      verlet_removeBall();    

    // translate joystick x/y into gravity between -0.25 and 0.25, reverse y as well.
    float x = (0.025f - (0.05f * ((float)analogRead(JOY_X) / 4096.0f))) * -1;
    float y = (0.025f - (0.05f * ((float)analogRead(JOY_Y) / 4096.0f)));

    // joystick deadzone
    if (x < 0.01f && x > -0.01f) x = IDLE_GRAVITY_X;
    if (y < 0.01f && y > -0.01f) y = IDLE_GRAVITY_Y;
      
    verlet_updateGravity(x, y);
  }
  
  // update verlet integration
  if (millis() - lastVerlet > VERLET_WAIT) {
    
    lastVerlet = millis();
    verlet_update();
  }

  // update display
  if (millis() - lastDisplay > DISPLAY_WAIT) {
    
    lastDisplay = millis();
    display_update();
  }  
}

// ####################### MISC FUNCTIONS ##################################

void generateScene() {

  Serial.println("generating scene");
  verlet_clearAll();
  //verlet_changeWallBounce(random(100, 200) / 100);
  
  int numberOfBalls = random(floor(MIN_BALLS_IN_SCENE), floor(MAX_BALLS_IN_SCENE));

  Serial.print("Adding ");
  Serial.println(numberOfBalls);
  
  for (int i = 0; i < numberOfBalls; i++)
    generateRandomBall();
}

void generateRandomBall() {

  verlet_addBall(
    random(1, 6300) / 100.0f, 
    random(1, 3100) / 100.0f, 
    (random(1, 200) / 100.0f) - 1.0f, 
    (random(1, 200) / 100.0f) - 1.0f,
    random(1, 255),
    random(1, 255),
    random(1, 255));
}
