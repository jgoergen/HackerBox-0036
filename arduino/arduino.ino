// Core display library provided by the PxMatrix library https://github.com/2dom/PxMatrix
// Basic integration provided by Hackerboxes https://www.instructables.com/id/HackerBox-0036-JumboTron/
// Verlet integration based on https://github.com/jgoergen/VerletAlgorythm

#include <PxMatrix.h>

// ################# SETTINGS ###########################################

// instead of using a delay on our loop, we'll be watching the time elapsed so we have independent
// update speeds on our processes.

#define INPUT_WAIT    100 // ~10 fps
#define DISPLAY_WAIT  33 // ~30 fps
#define VERLET_WAIT   15 // ~60 fps

#define MIN_BALLS_IN_SCENE  100
#define MAX_BALLS_IN_SCENE  400

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

  // setup display
  display.begin(16); // 1/16 scan
  display.setFastUpdate(true);
  display_update_enable(true);

  // initialize the verlet integration
  verlet_init();

  // generate a scene
  generateScene();
}

// ################################# LOOP ###################################

void loop() {
  
  // read inputs?
  if (millis() - lastInput > INPUT_WAIT) {
    
    lastInput = millis();

    if (digitalRead(KEY_1) == LOW)
      verlet_clear_all();

    if (digitalRead(KEY_2) == LOW)
      generateRandomBall();
      
    if (digitalRead(KEY_3) == LOW)
      generateScene();

    if (digitalRead(KEY_4) == LOW)
      verlet_remove_ball();    
  }
  
  // update verlet integration
  if (millis() - lastVerlet > VERLET_WAIT) {
    
    // translate joystick x/y into gravity between -0.25 and 0.25, reverse y as well.
    float x = (0.25f - (0.5f * ((float)analogRead(JOY_X) / 4096.0f))) * -1;
    float y = (0.25f - (0.5f * ((float)analogRead(JOY_Y) / 4096.0f)));

    // joystick deadzone
    if (x < 0.05f && x > -0.05f)
      x = 0;

    if (y < 0.05f && y > -0.05f)
      y = -0.03;
    
    lastVerlet = millis();
    verlet_update(x, y);
  }

  // update display
  if (millis() - lastDisplay > DISPLAY_WAIT) {
    
    lastDisplay = millis();
    verlet_draw();
  }  
}

// ####################### MISC FUNCTIONS ##################################

void generateScene() {

  Serial.println("generating scene");
  verlet_clear_all();
  verlet_set_friction(random(0, 40) / 1000.0f);
      
  int numberOfBalls = random(MIN_BALLS_IN_SCENE, MAX_BALLS_IN_SCENE);

  Serial.print("Adding ");
  Serial.print(numberOfBalls);
  Serial.println(" balls");
  
  for (int i = 0; i < numberOfBalls; i++)
    generateRandomBall();
}

void generateRandomBall() {

  verlet_add_ball(
    random(1, 6300) / 100.0f, 
    random(1, 3100) / 100.0f, 
    (random(1, 200) / 100.0f) - 1.0f, 
    (random(1, 200) / 100.0f) - 1.0f,
    random(1, 255),
    random(1, 255),
    random(1, 255));
}

void IRAM_ATTR display_updater() {
  
  // Increment the counter and set the time of ISR
  portENTER_CRITICAL_ISR(&timerMux);
  display.display(display_draw_time);
  portEXIT_CRITICAL_ISR(&timerMux);
}

void display_update_enable(bool is_enable) {
  
  if (is_enable) {
  
    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, &display_updater, true);
    timerAlarmWrite(timer, 2000, true);
    timerAlarmEnable(timer);
    
  } else {
    
    timerDetachInterrupt(timer);
    timerAlarmDisable(timer);
  }
}
