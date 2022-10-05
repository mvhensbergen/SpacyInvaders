#include <MCUFRIEND_kbv.h>

/*
 *  Defines the class with the state of the entire game
 */

#include "scores.h"

class GameSpace {
  private:
    SpaceShip* enemies[MAX_ENEMIES_PER_ROW][MAX_ROWS];  // enemy ships
    SpaceShip* player;  // player ship
    Laser* Lasers[MAX_PLAYER_LASERS];  // player lasers
    Laser* EnemyLasers[MAX_ENEMY_LASERS]; // enemy lasers
    PowerUp *powerup; // Powerup sprite
    Scores* score;
    int level;  // current level
    int active_enemies;  // amount of remaining enemies

    char textbuffer[128];  // buffer to render text in

    // Agressiveness factors
    int fire_chance;  // Percentage of how likely it is a spaceship will fire
    int max_lasers;   // maximum of simultaneous enemy lasers
    int speed;        // horizontal speed of the enemies

    // used for delaying the fire rate a bit
    long last_fired;

    // the tft screen class
    MCUFRIEND_kbv tft;

    // powerup handler
    bool powerup_showing; // whether or not powerup is showing
    int active_powerup; //  Currently active powerup
    long powerup_active_start = 0; //  Time when the powerup started
    long powerup_visible_start = 0; // Time when the powerup is drawn

    // get the nth active enemy counted from top left
    SpaceShip* get_nth_enemy(int n);

    void enemies_in_start_position(bool set_active) {
      speed = NORMAL_SPEED;
      for ( int row = 0; row < MAX_ROWS; row++ ) {
        for ( int i = 0; i < MAX_ENEMIES_PER_ROW; i++) {
          enemies[i][row]->set_coordinates(i*WIDTH*2, row * WIDTH*2 + HUD_OFFSET);
          enemies[i][row]->set_xspeed(speed);
          enemies[i][row]->set_color(colors[row]);
          enemies[i][row]->set_direction(1);

          if (set_active)
            enemies[i][row]->set_active();
          enemies[i][row]->move(VERTICAL); // Force complete redraw
        }
      }
    }

  public:
    GameSpace(MCUFRIEND_kbv t);

    // Movement functions
    void hover_enemies();
    void move_enemies();
    void move_player(int direction);
    void move_lasers();

    void set_xspeed();

    // Behaviour changes
    void anger_enemies();

    // Firing of lasers
    void enemy_fire();
    void player_fire();

    // Hit detection and end game end detection
    bool check_hits();
    bool detect_playerdeath();
    bool sprite_hit_by_laser(Sprite *enemy, Laser* laser);
    bool all_enemies_dead() { return (active_enemies == 0 ); };
    bool enemies_reached_base();
    void advance_level();
    void reset_level() { level = 1; fire_chance = 15; max_lasers = 1;};
    void reset_state();
    void game_over();

    // Powerup handling
    bool powerup_active() { return powerup_showing; };
    void do_powerup();
    void handle_active_powerup();
    void execute_kill_powerup();
    void execute_reset_powerup();
    int current_powerup() { return active_powerup;};

    // function to draw the hud
    void draw_hud();

    // aimbot functions
    SpaceShip* get_nearest_enemy();
    int get_spaceships_left_edge();
    int get_spaceships_right_edge();
};

SpaceShip* GameSpace::get_nearest_enemy() {
  const long maxdistance = tft.width() * tft.width() + tft.height() * tft.height();
  long distance = maxdistance;

  SpaceShip* nearest_enemy = NULL;

  for ( int row = 0; row < MAX_ROWS; row++ ) {
    for ( int i = 0; i < MAX_ENEMIES_PER_ROW; i++) {
      if (enemies[i][row] -> is_inactive())
        continue;

      long xposplayer = player->posx();
      long yposplayer = player->posy();

      long xpos = enemies[i][row]->posx();
      long ypos = enemies[i][row]->posy();

      /*Serial.print(xposplayer); Serial.print(" ");
      Serial.print(yposplayer); Serial.print(" ");
      Serial.print(xpos);Serial.print(" ");
      Serial.print(ypos);Serial.print(" ");*/

      long current_distance = (xposplayer-xpos)*(xposplayer-xpos) + (yposplayer-ypos)*(yposplayer-ypos);
      //Serial.println(current_distance);
      if (current_distance < distance) {
        distance = current_distance;
        nearest_enemy = enemies[i][row];
      }
    }
  }
  return nearest_enemy;
}

int GameSpace::get_spaceships_left_edge() {
  int xpos = tft.width();
  for ( int row = 0; row < MAX_ROWS; row++ ) {
    for ( int i = 0; i < MAX_ENEMIES_PER_ROW; i++) {
      if (enemies[i][row] -> is_inactive())
        continue;
      xpos = min(xpos, enemies[i][row]->posx());
    }
  }
}

int GameSpace::get_spaceships_right_edge() {
  int xpos = 0;
  for ( int row = 0; row < MAX_ROWS; row++ ) {
    for ( int i = 0; i < MAX_ENEMIES_PER_ROW; i++) {
      if (enemies[i][row] -> is_inactive())
        continue;
      xpos = max(xpos, enemies[i][row]->posx() + WIDTH);
    }
  }
}


// Constructor - initialize all the elements of the board just once
GameSpace::GameSpace(MCUFRIEND_kbv t) {
  tft = t;

  // Initialize all enemy ships
  for ( int row = 0; row < MAX_ROWS; row++ ) {
    for ( int i = 0; i < MAX_ENEMIES_PER_ROW; i++) {
      enemies[i][row] = new SpaceShip(tft);
    }
  }

  // Define player ship
  player = new SpaceShip(tft);
  player -> set_player();

  powerup = new PowerUp(tft);

  // Define score tracking
  score = new Scores();

  // Initialize all laser objects for enemies and player
  for ( int i = 0; i < MAX_PLAYER_LASERS; i++ ) {
    Laser* laser = new Laser(tft);
    Lasers[i] = laser;
  }

  for ( int i = 0; i < MAX_ENEMY_LASERS; i++ ) {
    Laser* laser = new Laser(tft);
    laser -> set_enemy();
    EnemyLasers[i] = laser;
  }

  active_powerup = NO_POWERUP;
  powerup_showing = false;
}

void GameSpace::reset_state() {
  tft.fillScreen(BLACK);

  // Set spaceships in start position
  enemies_in_start_position(true);

  player -> set_coordinates(tft.width()/2-WIDTH/2, tft.height()-HEIGHT);
  player -> set_xspeed(10);
  player -> set_color(WHITE);
  player -> set_active();
  player -> move(HORIZONTAL);

  // Register total nummer of alive enemies
  active_enemies = MAX_ROWS * MAX_ENEMIES_PER_ROW;

  // Turn of all lasers
  for ( int i = 0; i < MAX_PLAYER_LASERS; i++ ) {
    Lasers[i] -> deactivate();
    Lasers[i] -> set_width(NORMAL_LASER_WIDTH);
  }

  for ( int i = 0; i < MAX_ENEMY_LASERS; i++ ) {
    EnemyLasers[i] -> deactivate();
    EnemyLasers[i] -> set_width(NORMAL_LASER_WIDTH);
  }

  // turn off powerup
  powerup -> deactivate();
  active_powerup = NO_POWERUP;

  last_fired = 0;
}

// Do a 'fake' draw operation to remain constant drawing speed
void GameSpace::hover_enemies() {
  for ( int row = 0; row < MAX_ROWS; row++) {
    for ( int i = 0; i < MAX_ENEMIES_PER_ROW; i++ ) {
      enemies[i][row] -> hover();
    }
  }
}

// Move enemies sideways and vertically when boundaries of screen have been reached
void GameSpace::move_enemies() {
  // Boundary check calculation to see if the enemies should change direction
  int lowest_x = tft.width();
  int highest_x = 0;

  for ( int row = 0; row < MAX_ROWS; row++) {
    for ( int i = 0; i < MAX_ENEMIES_PER_ROW; i++ ) {
      enemies[i][row] -> move(HORIZONTAL);
      if (!(enemies[i][row] -> is_inactive())) {
        lowest_x = min(lowest_x, enemies[i][row]->posx());
        highest_x = max(highest_x, enemies[i][row]->posx());
      }
    }
  }

  // Should change direction?
  if (lowest_x <= 0 || highest_x + WIDTH >= tft.width() ) {
    for ( int row = 0; row < MAX_ROWS; row++) {
      for ( int i = 0; i < MAX_ENEMIES_PER_ROW; i++ ) {
        enemies[i][row] -> change_direction();
        enemies[i][row] -> move(VERTICAL);
      }
    }
  }
}

// change the horizontal speed of all enemies
void GameSpace::set_xspeed() {
  for ( int row = 0; row < MAX_ROWS; row++) {
    for ( int i = 0; i < MAX_ENEMIES_PER_ROW; i++ ) {
      enemies[i][row]->set_xspeed(speed);
    }
  }
}

// make the enemies move faster if more get eliminated
void GameSpace::anger_enemies() {
  speed = NORMAL_SPEED;
  int c = MAX_ROWS * MAX_ENEMIES_PER_ROW - active_enemies;

  if ( c > MAX_ROWS * MAX_ENEMIES_PER_ROW /4 ) speed = AGITATED_SPEED;
  if ( c > MAX_ROWS * MAX_ENEMIES_PER_ROW  * 3 /4 ) speed = ANGRY_SPEED;

  set_xspeed();
}

// Check is a sprite is hit by a laser
// A sprite is already regarded as a rectangle even though the actual sprite may look different
bool GameSpace::sprite_hit_by_laser(Sprite *enemy, Laser* laser) {
  if (  laser->posx() + laser->get_laser_width() > enemy -> posx() &&
        laser->posx() < enemy->posx() + WIDTH)

        if (!( laser->posy() + HEIGHT < enemy -> posy()) &&
            !( laser->posy() > enemy -> posy() + HEIGHT))
              return true;
  return false;
}

// Check if player laser hits an enemy
bool GameSpace::check_hits() {
  bool hit = false;

  for ( int i = 0; i < MAX_PLAYER_LASERS; i++) {
    Laser* laser = Lasers[i];
    if (laser -> is_inactive()) {
      continue;
    }

    for ( int row = 0; row < MAX_ROWS; row++) {
      for (int i =0 ; i < MAX_ENEMIES_PER_ROW; i++ ) {
        SpaceShip *enemy = enemies[i][row];
        if (enemy -> is_inactive()) {
          continue;
        }
        if (sprite_hit_by_laser(enemy,laser) ) {
            enemy -> deactivate();
            laser -> deactivate();
            hit = true;
            active_enemies--;
          }
        }
      }
    }
  return hit;
}


// Get nth active enemy counted from top left
SpaceShip* GameSpace::get_nth_enemy(int n) {
  int counter = 0;
  for ( int row = 0; row < MAX_ROWS; row++) {
    for ( int i=0; i< MAX_ENEMIES_PER_ROW; i++) {
      SpaceShip *enemy = enemies[i][row];
      if (enemy -> is_inactive()) {
        continue;
      }

      if ( counter != n ) {
        counter++;
        continue;
      }

      return enemy;
    }
  }
}

// Select a random enemy to fire based on chance
// There must be an inactive laser slot available to shoot
void GameSpace::enemy_fire() {
  int chance = random(0,100);

  if ( chance <= fire_chance ) {
    int enemy_to_fire = random(0, active_enemies);
    SpaceShip* enemy = get_nth_enemy(enemy_to_fire);

    // Find an available laser slot
    for (int i = 0; i < max_lasers; i++ ) {
      if ( EnemyLasers[i]->is_inactive()) {
        EnemyLasers[i] -> fire(enemy->posx() + WIDTH/2, enemy->posy() + HEIGHT);
        return;
      }
    }
  }
}

// Move enemy and player lasers
void GameSpace::move_lasers() {
  for (int l = 0; l < MAX_PLAYER_LASERS; l++ ) {
      if (!(Lasers[l] -> is_inactive() )) {
        Lasers[l] -> move(VERTICAL);
      }
    }

   for (int l = 0; l < MAX_ENEMY_LASERS; l++ ) {
     if (!(EnemyLasers[l] -> is_inactive() )) {
       EnemyLasers[l] -> move(VERTICAL);
     }
  }
}

// Fire a player laser from the players position
// there is a small delay in firing which is lowered given the RAPID FIRE powerup
void GameSpace::player_fire() {
  long now = millis();

  int delay = 500;
  if ( active_powerup==RAPID_FIRE_POWERUP ) {
    delay = 30;
  }

  if (now - last_fired < delay )
    return;

  for (int i = 0; i < MAX_PLAYER_LASERS; i++ ) {
    if ( Lasers[i]->is_inactive()) {
      Lasers[i] -> fire(player->posx() + WIDTH/2 - Lasers[i]->get_laser_width()/2, player->posy() - HEIGHT);
      last_fired = millis();
      return;
    }
  }
}

// See if the player got hit by an enemy lser
bool GameSpace::detect_playerdeath() {
  for (int i = 0; i< MAX_ENEMY_LASERS; i++) {
    if (EnemyLasers[i] -> is_inactive()) {
      continue;
    }
    if (sprite_hit_by_laser(player, EnemyLasers[i])) {
      return true;
    }
  }
  return false;
}

// Check if the enemies reached the bottom row
bool GameSpace::enemies_reached_base() {
  int height = 0;
  // Calculate lowest pixel line
  for ( int row = 0; row < MAX_ROWS; row++) {
    for ( int i = 0; i < MAX_ENEMIES_PER_ROW; i++ ) {
      height = max(height, enemies[i][row]->posy() + HEIGHT);
    }
  }

  if (height > tft.height() - HEIGHT)
    return true;

  return false;
}


// Draw the screen green and pause for a little while
// Then increase difficulty
void GameSpace::advance_level() {
  draw_hud();
  tft.fillRect(0,HUD_OFFSET, tft.width(), tft.height(), GREEN);

  tft.setTextSize(3);
  tft.setCursor(0, tft.height()/3);
  tft.setTextColor(BLACK);
  sprintf(textbuffer, "Prepare for\n\nlevel %d!", level+1);
  tft.print(textbuffer);

  delay(PAUSE);
  tft.fillScreen(BLACK);
  reset_state();

  // increase difficulty
  level += 1;
  fire_chance += 5;
  if (max_lasers < MAX_ENEMY_LASERS)
    max_lasers += 1;
}

// Game over
void GameSpace::game_over() {
  draw_hud();
  tft.fillRect(0,HUD_OFFSET, tft.width(), tft.height(), RED);

  tft.setTextSize(3);
  tft.setCursor(0, tft.height()/3);
  tft.setTextColor(BLACK);

  if ( level > score -> get_highscore()) {
    score -> set_highscore(level);
    sprintf(textbuffer, "Game Over!\n\nNew highscore: %d", level);
  } else {
    sprintf(textbuffer, "Game Over!");
  }

  tft.print(textbuffer);

  delay(PAUSE);
  tft.fillScreen(BLACK);
  reset_state();
  reset_level();
}

// Draw the stats of the game
void GameSpace::draw_hud() {
  // Alive and level

  if (active_powerup != NO_POWERUP) {
    sprintf(textbuffer, "Level: %d  Enemies: %d High Score: %d, %s", level, active_enemies, score->get_highscore(), powerup->get_powerup_label());
  } else {
    sprintf(textbuffer, "Level: %d  Enemies: %d High Score: %d", level, active_enemies, score->get_highscore());
  }

  tft.fillRect(0,0, tft.width(), HUD_OFFSET, BLACK);

  tft.setTextSize(1);
  tft.setCursor(0, 4);
  tft.setTextColor(WHITE);
  tft.print(textbuffer);

}

void GameSpace::move_player(int direction) {
  player -> set_direction(direction);

  if (direction < 0 && player->posx() >0 )
    player -> move(HORIZONTAL);
  if (direction > 0 && player->posx() + WIDTH < tft.width())
    player -> move(HORIZONTAL);
}

void GameSpace::handle_active_powerup() {
  switch (active_powerup) {
    case(NO_POWERUP):
      return;
      break;
    case(KILL_POWERUP):
      execute_kill_powerup();
      active_powerup = NO_POWERUP;
      draw_hud();
      break;
    case(RESET_POWERUP):
      execute_reset_powerup();
      active_powerup = NO_POWERUP;
      draw_hud();
      break;
    case(HOVER_POWERUP):
      if (millis() - powerup_active_start > POWERUP_DURATION) {
        active_powerup = NO_POWERUP;
        draw_hud();
      }
      break;
    case(STOP_FIRING_POWERUP):
      if (millis() - powerup_active_start > POWERUP_DURATION) {
        active_powerup = NO_POWERUP;
        draw_hud();
      }
      break;
    case(RAPID_FIRE_POWERUP):
      if (millis() - powerup_active_start > POWERUP_DURATION) {
        active_powerup = NO_POWERUP;
        draw_hud();
      }
      break;
    case(WIDE_LASER_POWERUP):
      for ( int i = 0; i < MAX_PLAYER_LASERS; i++ ) {
        Lasers[i] -> set_width(POWERUP_LASER_WIDTH);
      }
      if (millis() - powerup_active_start > POWERUP_DURATION) {
        active_powerup = NO_POWERUP;
        for ( int i = 0; i < MAX_PLAYER_LASERS; i++ ) {
          Lasers[i] -> deactivate();
          Lasers[i] -> set_width(NORMAL_LASER_WIDTH);
        }
        draw_hud();
      }
      break;
    default:
      active_powerup = NO_POWERUP;
  }
}

void GameSpace::execute_kill_powerup() {
  int amount = 1;
  if ( active_enemies > 2 ) {
    amount = floor(active_enemies/2);
  }

  for ( int i = 0; i < amount; i++) {
    SpaceShip* enemy = get_nth_enemy(random(0, active_enemies));
    enemy -> deactivate();
    active_enemies--;
  }
}

void GameSpace::execute_reset_powerup() {
  tft.fillRect(0, HUD_OFFSET, tft.width(), tft.height() - HEIGHT - HUD_OFFSET, BLACK);

  enemies_in_start_position(false);
}

void GameSpace::do_powerup() {
  if ( powerup_showing ) {
    // Has the powerup been up too long
    if ( millis() - powerup_visible_start > POWERUP_VISIBLE_DURATION ) {
      powerup -> deactivate();
      powerup_showing = false;
      draw_hud();
      return;
    }
    // Has the powerup been hit
    for ( int i = 0; i < MAX_PLAYER_LASERS; i++) {
      Laser* laser = Lasers[i];
      if (laser -> is_inactive()) {
        continue;
      }
      if ( sprite_hit_by_laser(powerup, laser)) {
          powerup -> deactivate();
          laser -> deactivate();
          powerup_showing = false;

          powerup_active_start = millis();
          active_powerup = powerup->get_powerup_type();
          draw_hud();
      }
    }
  } else {
    if (active_powerup == NO_POWERUP) {
      // don't generate powerups unless upper row is cleared
      int ypos = 0;
      for ( int i = 0; i < MAX_ENEMIES_PER_ROW; i++ ) {
        ypos = max(ypos, enemies[i][0]->posy());
      }
      if ( ypos < HUD_OFFSET + 2 * HEIGHT)
        return;

      // do a chance game to see if we need to draw
      int percent = 1;
      if ( random(0,100) < percent ) {
        powerup_visible_start = millis();
        powerup -> set_color(WHITE);
        powerup -> set_xspeed(0);
        powerup -> activate();
        powerup -> set_coordinates(random(WIDTH, tft.width() - WIDTH), HUD_OFFSET + HEIGHT);
        powerup -> draw();
        powerup_showing = true;

        draw_hud();
      }
    }
  }
}
