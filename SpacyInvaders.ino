// CONFIGURED FOR EITHER THE TFT SHIELD OR THE BREAKOUT BOARD.
// SEE RELEVANT COMMENTS IN Adafruit_TFTLCD.h FOR SETUP.

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library

#include <TouchScreen.h>

#include "globals.h"
#include "spaceship.h"
#include "laser.h"
#include "powerup.h"
#include "gamespace.h"

#include "scores.h"

// Touch screen pressure threshold
#define MINPRESSURE 0
#define MAXPRESSURE 1000

// Touch screen calibration
const int16_t XP = 8, XM = A2, YP = A3, YM = 9; //240x320 ID=0x9341
const int16_t TS_LEFT = 122, TS_RT = 929, TS_TOP = 77, TS_BOT = 884;

const TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;

// Set to 1 to reset highscore
#define CLEAR 0

void setup(void) {
  Serial.begin(9600);
  Serial.println(F("TFT LCD test"));
  Serial.print("TFT size is "); Serial.print(tft.width()); Serial.print("x"); Serial.println(tft.height());

  tft.reset();

  uint16_t identifier=0x9341;

  tft.begin(identifier);

  tft.setRotation(1); // Turn the display 270 degrees

  tft.fillScreen(BLACK);

  if (CLEAR) {
    Scores s;
    s.set_highscore(0);
  }
}

#define PLAYER_FIRE 0
#define PLAYER_LEFT 1
#define PLAYER_RIGHT 2
#define NO_ACTION 3

// Read touch display input
int read_input() {
  TSPoint tp = ts.getPoint();

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  if (tp.z > MINPRESSURE) {
    int ypos = map(tp.x, TS_RT, TS_LEFT, 0, tft.height());
    int xpos = map(tp.y, TS_BOT, TS_TOP, 0, tft.width());

    if ( ypos < tft.height() /2 ) {
      return PLAYER_FIRE;    
    } else {
      if (xpos < tft.width()/2 ) {
       return PLAYER_RIGHT;
      } else {
        return PLAYER_LEFT;
      }
    }
  }
  return NO_ACTION;
}

void loop(void) {
  // Initialize the game
  GameSpace g = GameSpace(tft);

  // bookkeeping to reset everything to start position
  g.reset_level();
  g.reset_state();
  g.draw_hud();

  bool should_redraw_hud = false;
  bool game_over = false;

  while (true) {
    should_redraw_hud = false;
    game_over = false;

    // Handle whether a powerup should be shown or if it should be erased
    g.do_powerup();

    g.cleanup_aimbot();
    g.aimbot();
    
    // If a powerup is in progress, handle it
    if ( g.current_powerup() != NO_POWERUP) {
      g.handle_active_powerup();
    }

    // move enemies unless HOVER_POWERUP is active
    if ( g.current_powerup() == HOVER_POWERUP) {
      g.hover_enemies();
    } else {
      g.move_enemies();
    }

    // react to player input
    int action = read_input();
    switch (action) {
      case PLAYER_FIRE:
        g.player_fire();
        break;
      case PLAYER_LEFT:
        g.move_player(-1);
        break;
      case PLAYER_RIGHT:
        g.move_player(1);
        break;
    }
    
    // move lasers
    g.move_lasers();    

    // Enemy fire chance unless STOP_FIRING powerup is active
    if (g.current_powerup() != STOP_FIRING_POWERUP)
      g.enemy_fire();
     
    // Did we hit an enemy?
    if ( g.check_hits() ) {
      should_redraw_hud = true;
      g.anger_enemies();
    }

    // Did we eliminate all enemeis?
    if (g.all_enemies_dead()) {
      should_redraw_hud = true;
      g.advance_level();
    }

    // Did we get hit?
    if (g.detect_playerdeath()) {
      should_redraw_hud = true;
      game_over = true;
    }

    // Did enemies reach our base?
    if (g.enemies_reached_base()) {
      should_redraw_hud = true;
      game_over = true;
    }

    if (game_over) {
      g.game_over();
    }

    if (should_redraw_hud)
      g.draw_hud();
  }
}
