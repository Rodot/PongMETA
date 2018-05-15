#include <Gamebuino-Meta.h>

//player variables
int player_score = 0;
int player_h = 8;
int player_w = 2;
int player_x = 4;
int player_y = (gb.display.height() - player_h) / 2;
int player_vymax = 4;
int player_vy = 0;
int player_timeoutmax = 120;
int player_timeout = 0;

//opponent variables
int opponent_score = 0;
int opponent_h = 8;
int opponent_w = 2;
int opponent_x = gb.display.width() - opponent_w - 4;
int opponent_y = (gb.display.height() - opponent_h) / 2;
int opponent_vymax = 4;
int opponent_vy = 0;
int opponent_timeoutmax = 120;
int opponent_timeout = 0;

//ball variables
int ball_size = 3;
int ball_x = opponent_x - ball_size - 5;
int ball_y = (gb.display.height() - ball_size) / 2;
int ball_vx = 2;
int ball_vy = 0;
int ball_vymax = 6;

#define PLAYER 1;
#define OPPONENT 2;
int winner_timer;
int winner_timermax = 40;
int winner;

///////////////////////////////////// MOVE PLAYER
void movePlayer() {
  //move the player

  //human control
  if (gb.buttons.repeat(BUTTON_UP, 1)) {
    player_timeout = player_timeoutmax;
    player_vy -= 1;
  } else if (gb.buttons.repeat(BUTTON_DOWN, 1)) {
    player_timeout = player_timeoutmax;
    player_vy += 1;
  } else if (player_timeout > 0) {
    player_vy = player_vy / 2;
  }


  //computer control after timeout
  if (player_timeout > 0) {
    player_timeout--;
  } else if ((player_y + (player_h / 2)) < (ball_y + (ball_size / 2))) { //if the ball is below the opponent
    player_vy = player_vy + 1; // accelerate down
  } else {
    player_vy = player_vy - 1; // accelerate up
  }

  player_vy = constrain(player_vy, (-player_vymax), player_vymax);
  player_y = player_y + player_vy;
  player_y = constrain(player_y, (-player_h), gb.display.height());
}

///////////////////////////////////// MOVE OPPONENT
void moveOpponent() {
  //move the opponent

  //human control
  if (gb.buttons.repeat(BUTTON_B, 1)) {
    opponent_timeout = opponent_timeoutmax;
    opponent_vy -= 1;
    opponent_vy = constrain(opponent_vy, (-opponent_vymax), opponent_vymax);
  } else if (gb.buttons.repeat(BUTTON_A, 1)) {
    opponent_timeout = opponent_timeoutmax;
    opponent_vy += 1;
    opponent_vy = constrain(opponent_vy, (-opponent_vymax), opponent_vymax);
  } else if (opponent_timeout > 0) {
    opponent_vy = opponent_vy / 2;
  }

  //computer control after timeout
  if (opponent_timeout > 0) {
    opponent_timeout--;
  } else if ((opponent_y + (opponent_h / 2)) < (ball_y + (ball_size / 2))) { //if the ball is below the opponent
    opponent_vy = opponent_vy + 1; //move down
  } else {
    opponent_vy = opponent_vy - 1; //move up
  }

  opponent_vy = constrain(opponent_vy, (-opponent_vymax), opponent_vymax);
  opponent_y = opponent_y + opponent_vy;
  opponent_y = constrain(opponent_y, (-opponent_h), gb.display.height()); //don't go out of the screen
}

///////////////////////////////////// UPDATE COLLISIONS
void updateCollisions() {
  //check for the main ball's collisions
  
  //collision with the top border
  if (ball_y < 0) {
    ball_y = 0;
    ball_vy = -ball_vy;
    gb.sound.playTick();
  }

  //collision with the bottom border
  if ((ball_y + ball_size) > gb.display.height()) {
    ball_y = gb.display.height() - ball_size;
    ball_vy = -ball_vy;
    gb.sound.playTick();
  }

  //collision with the player
  if (gb.collideRectRect(ball_x, ball_y, ball_size, ball_size, player_x, player_y, player_w, player_h)) {
    ball_x = player_x + player_w;
    ball_vx = -ball_vx;
    ball_vy += player_vy / 2; //add some effet to the ball
    ball_vy = constrain(ball_vy, (-ball_vymax), ball_vymax);
    gb.sound.playTick();
    gb.lights.drawPixel(0, map(ball_y, 0, gb.display.height(), 0, 4));
  }

  //collision with the opponent
  if (gb.collideRectRect(ball_x, ball_y, ball_size, ball_size, opponent_x, opponent_y, opponent_w, opponent_h)) {
    ball_x = opponent_x - ball_size;
    ball_vx = -ball_vx;
    ball_vy += (opponent_vy / 2);
    gb.sound.playTick();
    gb.lights.drawPixel(1, map(ball_y, 0, gb.display.height(), 0, 4));
  }

  //collision with the left side
  if (ball_x < 0) {
    opponent_score = opponent_score + 1;
    gb.sound.playCancel();
    gb.lights.setColor(RED);
    gb.lights.drawPixel(0, map(ball_y, 0, gb.display.height(), 0, 4));
    player_y = (gb.display.height() / 2) - (player_h / 2);
    opponent_y = player_y;
    ball_x = opponent_x - 4;
    ball_vx = -abs(ball_vx);
    ball_vy = 0;
    ball_y = random(0, gb.display.height() - ball_size);
  }

  //collision with the right side
  if ((ball_x + ball_size) > gb.display.width()) {
    player_score = player_score + 1;
    gb.sound.playOK();
    gb.lights.setColor(RED);
    gb.lights.drawPixel(1, map(ball_y, 0, gb.display.height(), 0, 4));
    player_y = (gb.display.height() / 2) - (player_h / 2);
    opponent_y = player_y;
    ball_x = player_x + player_w + 5;
    ball_vx = abs(ball_vx);
    ball_vy = 0;
    ball_y = random(0, gb.display.height() - ball_size);
  }
}

void draw() {
  //winner screen
  if (winner_timer > 0) {
    winner_timer--;
    gb.display.setColor(gb.createColor(0, winner_timer * 255 / winner_timermax, 0));
    if (winner == 1) {
      int t = winner_timermax - winner_timer;
      gb.display.fillRect(player_x - t, player_y - t, player_w + t * 2, player_h + t * 2);
      gb.lights.fillRect(0, 0, 1, 4);
    }
    if (winner == 2) {
      int t = winner_timermax - winner_timer;
      gb.display.fillRect(opponent_x - t, opponent_y - t, opponent_w + t * 2, opponent_h + t * 2);
      gb.lights.fillRect(1, 0, 1, 4);
    }
  }

  gb.display.setColor(WHITE);

  //draw the score
  gb.display.setCursor(15, 16);
  gb.display.print(player_score);

  gb.display.setCursor(gb.display.width() - 3 - 15, 16);
  gb.display.print(opponent_score);

  //draw the middle line
  for (int i = 0; i < gb.display.height(); i += 4) {
    if (gb.metaMode.isActive()) gb.display.setColor((gb.frameCount + i / 4) % 4 >= 2 ? WHITE : ORANGE);
    gb.display.drawFastVLine(gb.display.width() / 2, i, 2);
  }

  //draw the ball
  gb.display.fillRect(ball_x, ball_y, ball_size, ball_size);

  //draw the player
  if (player_timeout == 0) {
    gb.display.setColor(GRAY);
  } else {
    gb.display.setColor(WHITE);
  }
  gb.display.fillRect(player_x, player_y, player_w, player_h);

  //draw the opponent
  if (opponent_timeout == 0) {
    gb.display.setColor(GRAY);
  } else {
    gb.display.setColor(WHITE);
  }
  gb.display.fillRect(opponent_x, opponent_y, opponent_w, opponent_h);

  //TRY ME! message
  if ((player_timeout == 0) && (opponent_timeout == 0)) {
    gb.display.setCursor(27, 16);
    gb.display.setColor(WHITE, BLACK);
    if ((gb.frameCount % 20) > 10) {
      gb.display.print("TRY ME!");
    }
  }
}

///////////////////////////////////// SETUP
void setup() {
  gb.begin();
  gb.pickRandomSeed(); //pick a different random seed each time for games to be different
}

///////////////////////////////////// LOOP
void loop() {
  if (gb.update()) {
    // clear the previous screen
    gb.display.clear();
    gb.lights.clear();

    movePlayer();
    moveOpponent();

    //move the ball
    ball_x = ball_x + ball_vx;
    ball_y = ball_y + ball_vy;
    
    // Shhhhhhhhhhh ;)
    if (gb.metaMode.isActive() && random(0, 30) == 0) 
      ball_vy = random(-ball_vymax + 1, ball_vymax);

    
    updateCollisions();

    //reset score when 10 is reached
    if (player_score == 10) {
      winner = PLAYER;
      winner_timer = winner_timermax;
      player_score = 0;
      opponent_score = 0;
    }
    if (opponent_score == 10) {
      winner = OPPONENT;
      winner_timer = winner_timermax;
      player_score = 0;
      opponent_score = 0;
    }

    draw();
  }
}
