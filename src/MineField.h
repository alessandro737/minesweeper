//
// Created by Alessandro Giusti on 4/20/2024.
//
#pragma once
#include "Tile.h"
#include <vector>
#include <cstdlib>
#include <ctime>
using namespace std;
using namespace sf;


class MineField {
  int columns, rows;
  int mines;
  int remainingMines;

  //game_over = 0 means game in progress
  //game_over = 1 means game was won
  //game_over = -1 means game was lost
  int game_over = 1;
  vector<vector<Tile>> field;

  void setupField();
  void placeMines();
  void setupNeighbors();

  // textures and sprites
  Texture defaultTileTexture;
  Texture openTileTexture;
  Texture bombTexture;
  Texture flagTexture;
  Texture numberTextures[9]; // Array to store textures for numbers 1-8
  Sprite tileSprite;

  Texture digitsTexture;
  vector<Sprite> digitSprites; // for the counter and timer

  Texture winFaceTexture;
  Texture loseFaceTexture;
  Texture happyFaceTexture;
  Sprite faceButton;

  Texture debugTexture;
  Sprite debugButton;

  Texture leaderboardTexture;
  Sprite leaderboardButton;

  Texture playTexture;
  Texture pauseTexture;
  Sprite play_pauseButton;

  Sprite digitSpriteTensMinutes;
  Sprite digitSpriteOnesMinutes;
  Sprite digitSpriteTensSeconds;
  Sprite digitSpriteOnesSeconds;

public:
  MineField(int c, int r, int m);
  int getGame_over();
  void restart();
  void open_tile(int i_x, int i_y);
  void flag_tile(int i_x, int i_y);
  void draw(RenderWindow& window);
  void loadTextures();
  void extractDigitSprites();
  void drawCounter(RenderWindow& window);
  void drawFace(RenderWindow& window);
  Sprite& getFaceSprite();
  Sprite& getDebugSprite();
  Sprite& getPlay_PauseSprite();
  Sprite& getLeaderBoardSprite();
  void drawDebug(RenderWindow& window);
  void drawLeaderboardButton(RenderWindow& window);
  void drawPlay_Pause(RenderWindow& window);
  void drawTimer(RenderWindow& window);
  int minutes = 0, seconds = 0;
  bool isPaused = false;
  bool isDebug = false;
  bool isShowLeaderboard = false;
  string getFormattedTime();
};