/*
 *  This class is used for the aimbot and can predict the location of an enemy in time
 *  based on: current location of enemy, the x-ordinates of the outmost enemies, their speed
 *  and direction and the screens width.
 *
 *  The method calculate_time_to_intercept can be used to measure the time it takes for a laser
 *  to be able to reach the y-ordinate of an enemy, which can be used to calculate the necessary
 *  horizontal velocity of the aimbot laser
 */
class LocationPredictor {
  public:
    int enemyx; // Coordinates of the enemy
    int enemyy;

    int lborder;  // The x-ordinates of the leftmost and rightmost enemy
    int rborder;
    int screenwidth;

    int xspeed; // Current speed and riection of enemy
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

    // Calculate the position of the enemy after one time unit has passed
    advance_tick() {
      enemyx += xdirection * xspeed;
      lborder += xdirection * xspeed;
      rborder += xdirection * xspeed;

      if (lborder <= 0 || rborder >= screenwidth) {
        xdirection = -xdirection;
        enemyy+=SPACESHIP_VERTICAL_SPEED;
      }
    }

    // Calculate how lont it takes for the aimbot laser to reach the moving enemy vertically
    int calculate_time_to_intercept(int lasery) {
      int ticks = 0;
      while (enemyy + HEIGHT < lasery) {
        ticks++;
        advance_tick();
        lasery -= AIMBOT_LASERSPEED;
      }
      return ticks;
    }
};
