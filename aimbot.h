//
//
// |  x      x  |
// |      p     |
// |            |
//

class AimBot {
  public:
    int enemyx;
    int enemyy;

    int lborder;
    int rborder;
    int screenwidth;

    int xspeed;
    int xdirection;

    int max_ticks;

    AimBot (int x, int y, int l, int r, int w, int xs, int xd) {
      enemyx = x;
      enemyy = y;

      lborder = l;
      rborder = r;
      screenwidth = w;

      xspeed = xs;
      xdirection = xd;
    }

    advance_ticks(int ticks) {
      max_ticks = ticks;
      for ( int i = 0; i < ticks; i++ ) {
        /*Serial.println("---");
        Serial.print(enemyx); Serial.print(" "); Serial.println(enemyy);
        Serial.print(lborder); Serial.print(" "); Serial.println(rborder);
        Serial.println(screenwidth); 
        Serial.print(xdirection);Serial.print(" "); Serial.println(xspeed);
        */
        enemyx += xdirection * xspeed;
        lborder += xdirection * xspeed;
        rborder += xdirection * xspeed;

        if (lborder <= 0 || rborder >= screenwidth) {
          xdirection = -xdirection;
          enemyy++;
        }
        //delay(50);
      }
    }
};
