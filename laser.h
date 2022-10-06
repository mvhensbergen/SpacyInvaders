#include "sprite.h"

#include <MCUFRIEND_kbv.h>

/*
 * Defines Lasers
 */

class Laser : public Sprite {
  private:
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
         yspeed = LASER_SPEED;
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

class AimbotLaser : public Laser {
  private:
    float xspeed;
    float yspeed;
  public:
    AimbotLaser(MCUFRIEND_kbv t) : Laser(t) { Serial.println("INIT"); };
    
    void fire(int posx, int posy, float xs, float ys, int xd) {
      inactive = false;
      x = posx;
      y = posy;
      xspeed = xs;
      yspeed = ys;
      xdirection = xd; 
      color = RED;

      Serial.println("Fired!");
      Serial.println(x);
      Serial.println(y);

      Serial.println(xspeed);
      Serial.println(yspeed);

      Serial.println(xdirection);

      inactive = false;
    };

    void move() {
      if (inactive)
        return;

      //Serial.println("-");
      //Serial.println(xspeed);
      //Serial.println(xdirection);
      //Serial.println(x);

      tft.fillRect(x,y, NORMAL_LASER_WIDTH, HEIGHT, BLACK);

      x += (float) xdirection * (float) xspeed;
      y -= yspeed;
      if ( y < HUD_OFFSET - HEIGHT  || y>= tft.height()) {
          inactive = true;
          return;
      }
      
      tft.fillRect(x,y, NORMAL_LASER_WIDTH, HEIGHT, color);
    }
};
