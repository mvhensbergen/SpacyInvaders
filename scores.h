#ifndef _SCORES
#define _SCORES

#include<EEPROM.h>

#define ADDRESS 0  // Location of score

class Scores {
  private:
    int highscore;
    void _set_highscore_from_EEPROM();

  public:
    Scores() { 
      _set_highscore_from_EEPROM(); 
    };

    void set_highscore(int score);
    int get_highscore();

    void clear_highscore() {
      set_highscore(0);
    }
};

void Scores::_set_highscore_from_EEPROM() {
  int r = EEPROM.read(ADDRESS);
  if ( r == 255)
    r = 0;
  highscore = r;
}


int Scores::get_highscore() {
  return highscore;
}


void Scores::set_highscore(int score) {
  highscore = score;
  EEPROM.write(ADDRESS, highscore);
}

#endif
