extern word ballsCount;
extern word ballsX[];
extern word ballsY[];
extern byte ballsR[];
extern byte ballsG[];
extern byte ballsB[];

void display_init() {
  
  // setup display
  display.begin(16); // 1/16 scan
  display.setFastUpdate(true);
  display_update_enable(true);
}

void display_update() {

  display.clearDisplay();

  // remember our ball positions are multiplied by 100 and ints, so they'll have to be divided back down to 'remove the decimal places'
  for (int i = 0; i < ballsCount; i ++)
    display.drawPixel(
      floor(ballsX[i] / VERLET_PRECISION), 
      floor(ballsY[i] / VERLET_PRECISION), 
      display.color565(
        ballsR[i], 
        ballsG[i], 
        ballsB[i]));
}

