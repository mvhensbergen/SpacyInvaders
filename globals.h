// Assign human-readable names to some common 16-bit color values:
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define MAX_PLAYER_LASERS 3
#define MAX_ENEMY_LASERS 5
#define MAX_ENEMIES_PER_ROW 8
#define MAX_ROWS 4

#define HEIGHT 13
#define WIDTH 13
#define NORMAL_LASER_WIDTH 3
#define POWERUP_LASER_WIDTH 24
#define HORIZONTAL 0
#define VERTICAL 1

#define AIMBOT_LASERSPEED  1;

#define HUD_OFFSET 24

#define SPACESHIP_VERTICAL_SPEED 6
#define NORMAL_SPEED 3
#define AGITATED_SPEED 4
#define ANGRY_SPEED 6

#define POWERUP_DURATION 5000
#define POWERUP_VISIBLE_DURATION 5000

// Delay in milliseconds for level cleared or game over screen
#define PAUSE 2000

#define NO_POWERUP -1
// Enemies stand still for a period of time (but still fire)
#define HOVER_POWERUP 0
// Enemies are transported back into their starting position
#define RESET_POWERUP 1
// Half of the enemies are killed
#define KILL_POWERUP 2
// Enemies stop firing for a period of time
#define STOP_FIRING_POWERUP 3
// Player can fire more lasers at once
#define RAPID_FIRE_POWERUP 4
// Player laser widens
#define WIDE_LASER_POWERUP 5

#define MAX_POWERUPS 6


const char* powerups[7] = { "HOVER", "RESET", "KILL", "NO FIRE", "RAPID FIRE", "WIDE LASER"}; 

// The flashy colors of player's lasers
unsigned int colors[7] = { RED, GREEN, BLUE, YELLOW, MAGENTA, CYAN, WHITE };
