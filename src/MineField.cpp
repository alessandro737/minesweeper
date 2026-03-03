//
// Created by Alessandro Giusti on 4/20/2024.
//
#include "MineField.h"
#include "Tile.h"
#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <random>
#include <sstream>
#include <iomanip>
using namespace std;

void MineField::setupField() {
  field.resize(rows);

  for(int y = 0; y < rows; y++){
    for(int x = 0; x < columns; x++){
      Tile tile(x, y);
      field[y].push_back(tile);
    }
  }

}
void MineField::placeMines() {
  // Seed the random number generator
  srand(time(NULL));


  int minesPlaced = 0;

  while(minesPlaced < mines){
    int x = rand() % columns;
    int y = rand() % rows;

    if(!field[y][x].get_isBomb()){
      field[y][x].setBomb();
      minesPlaced++;
    }
  }
}
void MineField::setupNeighbors() {
  int dx[] = {-1, 0, 1, -1, 1, -1, 0, 1};
  int dy[] = {-1, -1, -1, 0, 0, 1, 1, 1};


  for (int y = 0; y < rows; ++y) {
    for (int x = 0; x < columns; ++x) {

      for (int i = 0; i < 8; ++i) {
        int nx = x + dx[i];
        int ny = y + dy[i];

        if (nx >= 0 && nx < columns && ny >= 0 && ny < rows) {
          field[y][x].addNeighbor(&field[ny][nx]);
        } else{
          field[y][x].addNeighbor(nullptr);
        }
      }
    }
  }
}
MineField::MineField(int c, int r, int m) : columns(c), rows(r), mines(m) {
  loadTextures();
  extractDigitSprites();
  setupField();
  placeMines();
  setupNeighbors();
  for(int y = 0; y < rows; y++){
    for(int x = 0; x < columns; x++){
      field[y][x].calculateNeighbors();
    }
  }
  game_over = 0;
  remainingMines = mines;
}
int MineField::getGame_over() { return game_over; }
void MineField::restart() {
  for(int y = 0; y < rows; y++){
    for(int x = 0; x < columns; x++){
      field[y][x].reset();
    }
  }
  setupField();
  placeMines();
  setupNeighbors();
  for(int y = 0; y < rows; y++){
    for(int x = 0; x < columns; x++){
      field[y][x].calculateNeighbors();
    }
  }
  game_over = 0;
  remainingMines = mines;
}
void MineField::open_tile(int i_x, int i_y) {
  if(game_over == 0 && field[i_y][i_x].get_isFlagged() == 0){
    if(field[i_y][i_x].open() == 1){
      game_over = -1;
    } else{
      int closedTiles = 0;
      for(int y = 0; y < rows; y++){
        for(int x = 0; x < columns; x++){
          closedTiles += 1 - field[y][x].get_isRevealed();
        }
      }
      if(mines == closedTiles){
        game_over = 1;
      }
    }
  }

}
void MineField::flag_tile(int i_x, int i_y) {
  if(game_over == 0){
    field[i_y][i_x].toggleFlag();
    if(field[i_y][i_x].get_isFlagged() == 1){
      remainingMines--;
    }else{
      remainingMines++;
    }
  }
}
void MineField::draw(RenderWindow &window) {

  Vector2u tileSize = defaultTileTexture.getSize();

  if(isPaused == 1 || isShowLeaderboard == 1){
    for(int y = 0; y < rows; y++) {
      for (int x = 0; x < columns; x++) {
        tileSprite.setPosition(static_cast<float>(x * tileSize.x), static_cast<float>(y * tileSize.y));
        tileSprite.setTexture(openTileTexture);
        window.draw(tileSprite);
      }
    }
  } else if(isDebug == 1 && isPaused == 0){
    for(int y = 0; y < rows; y++) {
      for (int x = 0; x < columns; x++) {
        tileSprite.setPosition(static_cast<float>(x * tileSize.x), static_cast<float>(y * tileSize.y));


        if(field[y][x].get_isRevealed()){
          tileSprite.setTexture(openTileTexture);
          window.draw(tileSprite);

          //check for bomb first else check neighbors
          if(field[y][x].get_isBomb() == 1){
            tileSprite.setTexture(bombTexture);
            window.draw(tileSprite);
          } else if(field[y][x].get_mines_around() > 0){
            tileSprite.setTexture(numberTextures[field[y][x].get_mines_around()]);
            tileSprite.setPosition(static_cast<float>(x * tileSize.x), static_cast<float>(y * tileSize.y));
            window.draw(tileSprite);
          }
        } else { // tile is closed
          tileSprite.setTexture(defaultTileTexture);
          window.draw(tileSprite);

          if(field[y][x].get_isBomb()){
            tileSprite.setTexture(bombTexture);
            window.draw(tileSprite);
          }

          // check flags
          if (field[y][x].get_isFlagged()) {
            tileSprite.setTexture(flagTexture);
            window.draw(tileSprite);
          }
        }
      }
    }
  } else{
    for(int y = 0; y < rows; y++) {
      for (int x = 0; x < columns; x++) {
        tileSprite.setPosition(static_cast<float>(x * tileSize.x),
                               static_cast<float>(y * tileSize.y));

        // check if tile is open
        if (field[y][x].get_isRevealed()) {
          tileSprite.setTexture(openTileTexture);
          window.draw(tileSprite);

          // check for bomb first else check neighbors
          if (field[y][x].get_isBomb() == 1) {
            tileSprite.setTexture(bombTexture);
            window.draw(tileSprite);
          } else if (field[y][x].get_mines_around() > 0) {
            tileSprite.setTexture(
                numberTextures[field[y][x].get_mines_around()]);
            tileSprite.setPosition(static_cast<float>(x * tileSize.x),
                                   static_cast<float>(y * tileSize.y));
            window.draw(tileSprite);
          }
        } else { // tile is closed
          tileSprite.setTexture(defaultTileTexture);
          window.draw(tileSprite);

          // check flags
          if (field[y][x].get_isFlagged()) {
            tileSprite.setTexture(flagTexture);
            window.draw(tileSprite);
          }
        }
      }
    }
  }

  // loss condition
  if(game_over == -1){
    for(int y = 0; y < rows; y++){
      for(int x = 0; x < columns; x++){
        if(field[y][x].get_isBomb() == 1){
          tileSprite.setTexture(bombTexture);
          tileSprite.setPosition(static_cast<float>(x * tileSize.x), static_cast<float>(y * tileSize.y));
          window.draw(tileSprite);
        }
      }
    }
  } else if(game_over == 1){ // win condition
    for(int y = 0; y < rows; y++){
      for(int x = 0; x < columns; x++){
        if(field[y][x].get_isRevealed() == 0){
          tileSprite.setTexture(flagTexture);
          tileSprite.setPosition(static_cast<float>(x * tileSize.x), static_cast<float>(y * tileSize.y));
          window.draw(tileSprite);
        }
      }
    }
  }
}
void MineField::loadTextures() {
  if (!defaultTileTexture.loadFromFile("assets/images/tile_hidden.png")) {
    cerr << "Error loading default tile texture\n";
  }
  if (!openTileTexture.loadFromFile("assets/images/tile_revealed.png")){
    cerr << "Error loading open tile texture\n";
  }
  if (!bombTexture.loadFromFile("assets/images/mine.png")){
    cerr << "Error loading bomb texture\n";
  }
  if (!flagTexture.loadFromFile("assets/images/flag.png")) {
    cerr << "Error loading flag texture\n";
  }
  for (int i = 1; i < 9; i++) {
    if (!numberTextures[i].loadFromFile("assets/images/number_" + to_string(i) + ".png")) {
      cerr << "Error loading number texture for " + to_string(i) + "\n";
    }
  }
  if (!digitsTexture.loadFromFile("assets/images/digits.png")) {
    cerr << "Error loading digits texture\n";
  }
  if(!debugTexture.loadFromFile("assets/images/debug.png")){
    cerr << "Error loading debug texture\n";
  }
  if(!winFaceTexture.loadFromFile("assets/images/face_win.png")){
    cerr << "Error loading debug texture\n";
  }
  if(!loseFaceTexture.loadFromFile("assets/images/face_lose.png")){
    cerr << "Error loading debug texture\n";
  }
  if(!happyFaceTexture.loadFromFile("assets/images/face_happy.png")){
    cerr << "Error loading debug texture\n";
  }
  if(!leaderboardTexture.loadFromFile("assets/images/leaderboard.png")){
    cerr << "Error loading debug texture\n";
  }
  if(!playTexture.loadFromFile("assets/images/play.png")){
    cerr << "Error loading debug texture\n";
  }
  if(!pauseTexture.loadFromFile("assets/images/pause.png")){
    cerr << "Error loading debug texture\n";
  }
  tileSprite.setTexture(defaultTileTexture);
  debugButton.setTexture(debugTexture);
  leaderboardButton.setTexture(leaderboardTexture);
}
void MineField::extractDigitSprites() {
  // loading vector with different sprites for each digit
  Vector2u textureSize = digitsTexture.getSize();
  Vector2u digitSize(21, textureSize.y);

  for(int i = 0; i < 11; i++){
    IntRect digitRect(i * digitSize.x, 0, digitSize.x, digitSize.y);
    Sprite digitSprite(digitsTexture, digitRect);
    digitSprites.push_back(digitSprite);
  }
}
void MineField::drawCounter(RenderWindow &window) {
  string minesLeft = to_string(abs(remainingMines));
  minesLeft = string(3 - minesLeft.length(), '0') + minesLeft;

  float x = 33.f;
  float y = 32.f * (static_cast<float>(rows) + 0.5f) + 16.f;

  bool negativeDrawn = false;

  if (remainingMines < 0) {
    // Draw negative symbol sprite
    digitSprites[10].setPosition(12.f, y); // Assuming index 10 is for the negative symbol sprite
    window.draw(digitSprites[10]);
    negativeDrawn = true;
  }

  for(char digitChar : minesLeft){
    int digit;
    if(digitChar == '-'){
      if(negativeDrawn){
        continue;
      }
      digit = 10;
      negativeDrawn = true;
    } else{
      digit = digitChar - '0';
    }

    digitSprites[digit].setPosition(x, y);
    window.draw(digitSprites[digit]);

    x += 21.f;
  }
}
void MineField::drawFace(RenderWindow &window) {

  // Set texture based on game ending condition
  if (game_over == 1) {
    faceButton.setTexture(winFaceTexture);
  } else if (game_over == -1) {
    faceButton.setTexture(loseFaceTexture);
  } else {
    faceButton.setTexture(happyFaceTexture);
  }
  // Adjust position and draw the face sprite
  faceButton.setPosition( ( (columns/2.0f) * 32 ) - 32, 32 * ( rows + 0.5f ) );
  window.draw(faceButton);
}
Sprite &MineField::getFaceSprite() {
  return faceButton;
}
Sprite &MineField::getDebugSprite() {
  return debugButton;
}
Sprite &MineField::getPlay_PauseSprite() {
  return play_pauseButton;
}
Sprite &MineField::getLeaderBoardSprite() {
  return leaderboardButton;
}
void MineField::drawDebug(RenderWindow &window) {
  debugButton.setPosition( (columns * 32) - 304, 32 * (rows +0.5f) );
  window.draw(debugButton);
}
void MineField::drawLeaderboardButton(RenderWindow &window) {
  leaderboardButton.setPosition( (columns * 32) - 176, 32 * ( rows + 0.5f ) );
  window.draw(leaderboardButton);
}
void MineField::drawPlay_Pause(RenderWindow &window) {
  play_pauseButton.setPosition((columns * 32) - 240, 32 * ( rows + 0.5f ));
  if(!isPaused){
    play_pauseButton.setTexture(pauseTexture);
  } else {
    play_pauseButton.setTexture(playTexture);
  }
  window.draw(play_pauseButton);
}
void MineField::drawTimer(RenderWindow &window) {

  int tensMinutes;
  tensMinutes = minutes / 10;
  int onesMinutes;
  onesMinutes = minutes % 10;
  int tensSeconds;
  tensSeconds = seconds / 10;
  int onesSeconds;
  onesSeconds = seconds % 10;

  // Draw tens minute digit
  digitSprites[tensMinutes].setPosition((columns * 32) - 97, 32 * (rows + 0.5) + 16);
  window.draw(digitSprites[tensMinutes]);

  // Draw ones minute digit
  digitSprites[onesMinutes].setPosition((columns * 32) - 76, 32 * (rows + 0.5) + 16);
  window.draw(digitSprites[onesMinutes]);

  // Draw tens second digit
  digitSprites[tensSeconds].setPosition((columns * 32) - 54, 32 * (rows + 0.5) + 16);
  window.draw(digitSprites[tensSeconds]);

  // Draw ones second digit
  digitSprites[onesSeconds].setPosition((columns * 32) - 33, 32 * (rows + 0.5) + 16);
  window.draw(digitSprites[onesSeconds]);

}
string MineField::getFormattedTime() {
  stringstream ss;
  ss << setw(2) << setfill('0') << minutes << ":" << setw(2) << setfill('0') << seconds;
  return ss.str();
}
