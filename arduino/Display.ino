extern int BALL_RADIUS;
extern uint16_t ballsCount;
extern int ballsX[];
extern int ballsY[];
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
