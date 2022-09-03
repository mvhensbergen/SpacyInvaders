#include "sprite.h"
#include<MCUFRIEND_kbv.h>

/*
 * Defines SpaceShip
 */

class SpaceShip: public Sprite {
  private:
    bool is_player = false;

  public:
    SpaceShip(MCUFRIEND_kbv t) : Sprite(t) { xdirection=1; ydirection = 1; yspeed=SPACESHIP_VERTICAL_SPEED; };
    void set_player() { is_player = true; };
    
    void change_direction() { xdirection = -xdirection;};
    void set_direction(int direction) { xdirection = direction; };
  
    void deactivate();
    void hover();
    void move(int direction);
};

void SpaceShip::deactivate() {
  Sprite::deactivate();
  tft.fillRect(x,y,WIDTH, HEIGHT, BLACK);
}

void SpaceShip::hover() {
  tft.fillRect(tft.width()-WIDTH,0,WIDTH, HEIGHT, BLACK);
  tft.fillRect(tft.width()-WIDTH,0,WIDTH, HEIGHT, BLACK);
}

void SpaceShip::move(int direction) {
  if (inactive) { 
    // Do 2 bogus draw operations to remain constant drawing time
    tft.fillRect(tft.width()-WIDTH,0,WIDTH, HEIGHT, BLACK);
    tft.fillRect(tft.width()-WIDTH,0,WIDTH, HEIGHT, BLACK);
    return;
  }

  // if ENEMY
  if (!is_player) {
    if (direction == HORIZONTAL) {
      if (xdirection > 0 ) {
        for ( int line = x; line < x + xdirection*xspeed; line ++) {
          tft.drawFastVLine(line,y, HEIGHT, BLACK);
        }
        for ( int line = x + WIDTH; line < x + WIDTH + (xdirection*xspeed); line ++) {
          tft.drawFastVLine(line, y, HEIGHT, color);
        }
        x += xdirection * xspeed;
      } else {
        for ( int line = x  + xdirection*xspeed+WIDTH; line < x+ WIDTH; line ++) {
          tft.drawFastVLine(line, y, HEIGHT, BLACK);
        }
        for ( int line = x + xdirection*xspeed; line < x ; line ++) {
          tft.drawFastVLine(line, y, HEIGHT, color);
        }
        x += xdirection * xspeed;
      }
    } else {
      tft.fillRect(x,y, WIDTH, HEIGHT, BLACK);
      y += ydirection * yspeed;
      tft.fillRect(x,y, WIDTH, HEIGHT, color);
    }
  } else {
    if (direction == HORIZONTAL ) {
       tft.fillTriangle(x,y+HEIGHT, x+WIDTH, y+HEIGHT, x + WIDTH/2, y, BLACK);
       x += xdirection * xspeed;
       tft.fillTriangle(x,y+HEIGHT, x+WIDTH, y+HEIGHT, x + WIDTH/2, y, color); 
    }
  }
}
