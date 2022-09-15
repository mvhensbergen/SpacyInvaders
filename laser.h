#include "sprite.h"

#include <MCUFRIEND_kbv.h>

/*
 * Defines Lasers
 */

class Laser : public Sprite {
  private:
    int ydirection;
    int yspeed;
    bool enemy = false;
    int laser_width;

    int get_laser_color() {
      int color;
      if (!enemy) {
        color = colors[random(0, 6)];
      } else {
        color = MAGENTA;
      }
      return color;
    }

  public:
    Laser(MCUFRIEND_kbv t) : Sprite(t)
      {
         yspeed = 10;
         ydirection = -1;
         inactive = true;
         laser_width = NORMAL_LASER_WIDTH;
      };

    void fire(int posx, int posy) {
      x = posx;
      y = posy;
      inactive = false;
    }

    void set_enemy() { enemy = true; yspeed = 5; ydirection=1;};
    void set_width(int w) { laser_width = w; };
    int get_laser_width() { return laser_width;} ;
    void deactivate();
    void move(int direction=VERTICAL);
};

void Laser::move(int direction=VERTICAL) {
  if (inactive) return;

  // Remove old position
  // tft.drawFastVLine(x,y , HEIGHT, BLACK);
  tft.fillRect(x,y, laser_width, HEIGHT, BLACK);

  y += ydirection * yspeed;
  // Cleanup lasers out of bounds
  if ( y < HUD_OFFSET - HEIGHT  || y>= tft.height()) {
    inactive = true;
    return;
  }

  int color = get_laser_color();
  tft.fillRect(x,y, laser_width, HEIGHT, color);
}


void Laser::deactivate() {
  Sprite::deactivate();
  tft.fillRect(x,y, laser_width, HEIGHT, BLACK);
  //tft.drawFastVLine(x,y, HEIGHT, BLACK);
}
