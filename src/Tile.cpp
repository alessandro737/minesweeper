//
// Created by Alessandro Giusti on 4/20/2024.
//

#include "Tile.h"


Tile::Tile(int i_x, int i_y) : x(i_x), y(i_y) {
  reset();
}

void Tile::setBomb() {
  isBomb = true;
}
void Tile::toggleFlag() {
  if(!isRevealed){
    isFlagged = 1 - isFlagged;
  }
}
void Tile::calculateNeighbors() {
  mines_around = 0;

  if(!isBomb){
    for(Tile* neighbor : neighbors){
      if(neighbor == nullptr){
        continue;
      }
      if(neighbor->isBomb){
        mines_around++;
      }
    }
  }

}
bool Tile::get_isBomb() { return isBomb; }
bool Tile::get_isRevealed() { return isRevealed; }
bool Tile::get_isFlagged() { return isFlagged; }
void Tile::addNeighbor(Tile* neighbor) {
  neighbors.push_back(neighbor);
}
void Tile::reset() {
  isBomb = false;
  isRevealed = false;
  isFlagged = false;
  mines_around = 0;
  neighbors.clear();
}
bool Tile::open() {
  if(!isRevealed){
    isRevealed = true;

    if(!isBomb && mines_around == 0){
      for(auto neighbor : neighbors){
        if(neighbor == nullptr) {
          continue;
        } else{
          neighbor->open();
        }
      }
    }
  }
  return isBomb;
}
int Tile::get_mines_around() { return mines_around; }
