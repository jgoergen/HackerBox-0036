// constants

const float BALL_RADIUS = 1;
const int STAGE_WIDTH = 63;
const int STAGE_HEIGHT = 31;
const float CIRCULAR_COLLISION_RESPONSE_DAMPENING = 0.15f;
const float WALL_COLLISION_RESPONSE_DAMPENING = 1.8f;
float FRICTION = 0.05f;
const int MAX_BALLS = MAX_BALLS_IN_SCENE;
float ballsX[MAX_BALLS];
float ballsY[MAX_BALLS];
int ballsCount = 0;
float ballsOldX[MAX_BALLS];
float ballsOldY[MAX_BALLS];
float gravityX = 0.0f;
float gravityY = 0.0f;
int ballsR[MAX_BALLS];
int ballsG[MAX_BALLS];
int ballsB[MAX_BALLS];
float ballRadiusDoubled = BALL_RADIUS * 2.0f;
float velocityX;
float velocityY;
float collisionVelocityX;
float collisionVelocityY;
int index1;
int index2;
int iterations;
float diffVectX;
float diffVectY;
float magnitude;

void verlet_init() {

  ballsCount = 0;
  gravityX = 0.0f;
  gravityY = 0.0f;

  // zero out all values
  for (int i = 0; i < MAX_BALLS; i++) {

    ballsX[i] = 0.0f;
    ballsY[i] = 0.0f;
    ballsOldX[i] = 0.0f;
    ballsOldY[i] = 0.0f;
    ballsR[i] = 0;
    ballsG[i] = 0;
    ballsB[i] = 0;      
  }
}

void verlet_update(float ax, float ay) {

  // clamp gravity  
  gravityX = max(-0.2f, min(0.2f, ax));
  gravityY = max(-0.2f, min(0.2f, ay));

  // process physics for each ball
  for (index1 = 0; index1 < ballsCount; index1 ++) {
  
    // derive velocity, add gravity to it, add friction to it
    float velocityX = (ballsX[index1] - ballsOldX[index1] + gravityX) * (1 - FRICTION);
    float velocityY = (ballsY[index1] - ballsOldY[index1] + gravityY) * (1 - FRICTION);
            
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
      if (abs(ballsX[index1] - ballsX[index2]) > ballRadiusDoubled ||
          abs(ballsY[index1] - ballsY[index2]) > ballRadiusDoubled)
          continue;
    
      // second pass, much slower but accurate check
      if (sqrt((ballsX[index2] - ballsX[index1]) * 
          (ballsX[index2] - ballsX[index1]) + 
          (ballsY[index2] - ballsY[index1]) * 
          (ballsY[index2] - ballsY[index1])) <= BALL_RADIUS) {

        collisionVelocityX = ballsX[index2] - ballsX[index1];
        collisionVelocityY = ballsY[index2] - ballsY[index1];
        
        // normalize the velocity vector 
        magnitude = sqrt(collisionVelocityX * collisionVelocityX + collisionVelocityY * collisionVelocityY);
        collisionVelocityX /= magnitude;
        collisionVelocityY /= magnitude;

        // dampen it
        collisionVelocityX *= CIRCULAR_COLLISION_RESPONSE_DAMPENING;
        collisionVelocityY *= CIRCULAR_COLLISION_RESPONSE_DAMPENING;

        // push the balls away from eachother
        ballsX[index1] -= collisionVelocityX;
        ballsY[index1] -= collisionVelocityY;
        ballsX[index2] += collisionVelocityX;
        ballsY[index2] += collisionVelocityY;
      }
    }

    // bounce off walls
    if (ballsX[index1] < 0.0f || ballsX[index1] >= STAGE_WIDTH)
      ballsX[index1] -= velocityX * WALL_COLLISION_RESPONSE_DAMPENING;

    if (ballsY[index1] < 0.0f || ballsY[index1] >= STAGE_HEIGHT)
      ballsY[index1] -= velocityY * WALL_COLLISION_RESPONSE_DAMPENING;
  }
}

void verlet_draw() {

  display.clearDisplay();
  
  for (index1 = 0; index1 < ballsCount; index1 ++)
    display.drawPixel(
      floor(ballsX[index1]), 
      floor(ballsY[index1]), 
      display.color565(
        ballsR[index1], 
        ballsG[index1], 
        ballsB[index1]));
}

void verlet_add_ball(float x, float y, float velocityX, float velocityY, int r, int g, int b) {
  
    // limit the amount we can add
    if (ballsCount == MAX_BALLS)
        return;

    ballsX[ballsCount] = x;
    ballsY[ballsCount] = y;
    ballsOldX[ballsCount] = x - velocityX;
    ballsOldY[ballsCount] = y - velocityY;
    ballsR[ballsCount] = r;
    ballsG[ballsCount] = g;
    ballsB[ballsCount] = b;    
    ballsCount ++;    
}

void verlet_remove_ball() {

  if (ballsCount > 0)
    ballsCount --;
}

void verlet_clear_all() {

  Serial.println("Verlet Clearing");
  ballsCount = 0;
}

void verlet_set_friction(float friction) {  
  
  Serial.print("Verlet setting friction to");
  Serial.println(friction);
  FRICTION = friction;
}

