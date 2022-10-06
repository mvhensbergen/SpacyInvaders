//
//
// |  x      x  |
// |      p     |
// |            |
//

class LocationPredictor {
  public:
    int enemyx;
    int enemyy;

    int lborder;
    int rborder;
    int screenwidth;

    int xspeed;
    int xdirection;

    LocationPredictor () {};
    void init(int x, int y, int l, int r, int w, int xs, int xd) {
      enemyx = x;
      enemyy = y; 

      lborder = l;
      rborder = r;
      screenwidth = w;

      xspeed = xs;
      xdirection = xd;
    }

    advance_tick() {
      enemyx += xdirection * xspeed;
      lborder += xdirection * xspeed;
      rborder += xdirection * xspeed;

      if (lborder <= 0 || rborder >= screenwidth) {
        xdirection = -xdirection;
        enemyy+=SPACESHIP_VERTICAL_SPEED;
      }
    }

    advance_ticks(int ticks) {
      for ( int i = 0; i < ticks; i++ ) {
        advance_tick();
      }
    }

    int calculate_time_to_intercept(int y) {
      int ticks = 0;
      while (enemyy + HEIGHT < y) {
        ticks++;
        advance_tick();
        y -= AIMBOT_LASERSPEED;
      }
      return ticks;
    }
};
