#include <MCUFRIEND_kbv.h>

#ifndef _SPRITE
#define _SPRITE

/* 
 *  Defines the basics of a sprite which will be used for spaceships and lasers
 *  
 */
class Sprite {
  public:
    float x;
    float y;
    int xspeed;
    int yspeed;
    int xdirection;
    int ydirection;
    int color;
    bool inactive = true;
    MCUFRIEND_kbv tft;

    Sprite(MCUFRIEND_kbv t) { tft = t; };

    bool is_inactive() { return inactive; }
    void set_active() { inactive = false; }
    
    virtual int posx() { return x; }
    virtual int posy() { return y; }

    void set_coordinates(int xx, int yy) {
      x = xx;
      y = yy;
    };
    void set_color(int c) { color = c; };
    void set_xspeed(int s) {xspeed = s;};
    void set_yspeed(int s) {yspeed = s;};

    virtual void move(int direction) {};
    virtual void deactivate() { inactive = true; }
};

#endif
