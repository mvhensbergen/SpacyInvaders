#include "sprite.h"
#include<MCUFRIEND_kbv.h>

#define POWERUP_RADIUS 5

class PowerUp: public Sprite {
  private:
    int powerup_type;
    char* powerup_label;
  public:
    PowerUp(MCUFRIEND_kbv t) : Sprite(t) {};

    void activate();
    int get_powerup_type() { return powerup_type;};
    char* get_powerup_label() { return powerup_label;};
    void draw();
    void deactivate();
};


// Activate powerup -> make it visible and also select a random powerup to be
void PowerUp::activate() {
  // Draw sprite
  inactive = false;

  // Choose random powerup
  int r = random(0, MAX_POWERUPS);

  powerup_type = r;
  set_color(colors[r]);
  powerup_label = powerups[r];
}

void PowerUp::draw() {
  tft.fillCircle(x + POWERUP_RADIUS + 1, y+POWERUP_RADIUS, POWERUP_RADIUS  , color);
}

void PowerUp::deactivate() {
  Sprite::deactivate();
  tft.fillCircle(x + POWERUP_RADIUS + 1, y+POWERUP_RADIUS, POWERUP_RADIUS , BLACK);
}
