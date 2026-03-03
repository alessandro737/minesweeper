//
// Created by Alessandro Giusti on 4/20/2024.
//
#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;


class Tile {

  bool isBomb;
  bool isRevealed;
  bool isFlagged;

  int x, y;
  int mines_around;

  vector<Tile*> neighbors;

public:
  Tile(int i_x, int i_y);
  void setBomb();
  void toggleFlag();
  void reset();
  void addNeighbor(Tile* neighbor);

  bool get_isBomb();
  bool get_isRevealed();
  bool get_isFlagged();
  int get_mines_around();

  bool open();

  void calculateNeighbors();

};



