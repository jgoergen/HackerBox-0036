// Verlet integration based on https://github.com/jgoergen/VerletAlgorythm
// All values multiplied up by 100 to move 2 decimal points into whole number range. This is done to avoid floating point math and use words
// instead of floats. This is much less accurate, but should be faster and much less memory.
// note word used on numbers that will not require negative numbers, otherwise int

// constants
const word BALL_RADIUS = 1 * VERLET_PRECISION;
const word STAGE_WIDTH = 63 * VERLET_PRECISION;
const word STAGE_HEIGHT = 31 * VERLET_PRECISION;
float WALL_COLLISION_RESPONSE_DAMPENING = 1.60f;
const word MAX_BALLS = MAX_BALLS_IN_SCENE;
word ballsX[MAX_BALLS];
word ballsY[MAX_BALLS];
word ballsCount = 0;
word ballsOldX[MAX_BALLS];
word ballsOldY[MAX_BALLS];
int gravityX = 0;
int gravityY = 0;
byte ballsR[MAX_BALLS];
byte ballsG[MAX_BALLS];
byte ballsB[MAX_BALLS];
word ballRadiusDoubled = BALL_RADIUS * 2;
int velocityX;
int velocityY;
int velocity2X;
int velocity2Y;
int collisionVelocityX;
int collisionVelocityY;
int index1;
int index2;
int iterations;
int diffVectX;
int diffVectY;
int magnitude;
int collisionDistance;

void verlet_init() {

  ballsCount = 0;
  gravityX = 0;
  gravityY = 0;

  // zero out all values
  for (int i = 0; i < MAX_BALLS; i++) {

    ballsX[i] = 0;
    ballsY[i] = 0;
    ballsOldX[i] = 0;
    ballsOldY[i] = 0;
    ballsR[i] = 0;
    ballsG[i] = 0;
    ballsB[i] = 0;      
  }
}

void verlet_updateGravity(float ax, float ay) {
  
  // clamp gravity
  // we take a float for this, and turn it into ints so the integration isn't so confusion from outside the lib
  gravityX = int(max(-0.05f, min(0.05f, ax)) * VERLET_PRECISION);
  gravityY = int(max(-0.05f, min(0.05f, ay)) * VERLET_PRECISION);
}

void verlet_update() {

  // process physics for each ball
  for (index1 = 0; index1 < ballsCount; index1 ++) {
    
    // derive velocity, add gravity to it
    velocityX = (ballsX[index1] - ballsOldX[index1] + gravityX);
    velocityY = (ballsY[index1] - ballsOldY[index1] + gravityY);

    // apply velocity
    ballsOldX[index1] = ballsX[index1];
    ballsOldY[index1] = ballsY[index1];
    ballsX[index1] += velocityX;
    ballsY[index1] += velocityY;
        
    // collide with other balls
    for (index2 = 0; index2 < ballsCount; index2 ++) {

      // don't collide with itself
      if (index1 == index2)
          continue;
          
      // first pass quick distance check
      if (abs(ballsX[index2] - ballsX[index1]) < BALL_RADIUS &&
          abs(ballsY[index2] - ballsY[index1]) < BALL_RADIUS) {
    
        collisionDistance = 
          fastSQRT(
            (ballsX[index1] - ballsX[index2]) * (ballsX[index1] - ballsX[index2]) + 
            (ballsY[index1] - ballsY[index2]) * (ballsY[index1] - ballsY[index2]));

        if (collisionDistance < BALL_RADIUS && collisionDistance != 0) { 
      
          collisionVelocityX = 
            (ballsX[index1] - ballsOldX[index1]) - 
            (ballsX[index2] - ballsOldX[index2]);
            
          collisionVelocityY = 
            (ballsY[index1] - ballsOldY[index1]) - 
            (ballsY[index2] - ballsOldY[index2]);
          
          ballsX[index1] -= collisionVelocityX;
          ballsY[index1] -= collisionVelocityY;
          ballsX[index2] += collisionVelocityX;
          ballsY[index2] += collisionVelocityY;
        }
      }
    }
    
    // bounce off walls
    if (ballsX[index1] <= 0 || ballsX[index1] >= STAGE_WIDTH)
      ballsX[index1] -= int(velocityX * WALL_COLLISION_RESPONSE_DAMPENING);

    if (ballsY[index1] <= 0 || ballsY[index1] >= STAGE_HEIGHT)
      ballsY[index1] -= int(velocityY * WALL_COLLISION_RESPONSE_DAMPENING);
  }
}

void verlet_add_ball(float x, float y, float velocityX, float velocityY, int r, int g, int b) {

  // limit the amount we can add
  if (ballsCount == MAX_BALLS)
      return;

  ballsX[ballsCount] = int(x * VERLET_PRECISION);
  ballsY[ballsCount] = int(y * VERLET_PRECISION);
  ballsOldX[ballsCount] = int((x - velocityX) * VERLET_PRECISION);
  ballsOldY[ballsCount] = int((y - velocityY) * VERLET_PRECISION);
  ballsR[ballsCount] = r;
  ballsG[ballsCount] = g;
  ballsB[ballsCount] = b;    
  ballsCount ++;
}

void verlet_changeWallBounce(float value) {

  WALL_COLLISION_RESPONSE_DAMPENING = value;
}

void verlet_remove_ball() {

  if (ballsCount > 0)
    ballsCount --;
}

void verlet_clear_all() {

  ballsCount = 0;
}

int fastSQRT(int n) {
   
  int c = 0x8000; 
  int g = 0x8000; 
  
  for(;;) {
    
     if (g * g > n)
       g ^= c; 
          
     c >>= 1; 

     if (c == 0)     
       return g;
       
     g |= c;   
  } 
}

