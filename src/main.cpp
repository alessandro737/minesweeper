#include "MineField.h"
#include <SFML/Graphics.hpp>
#include <cctype>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <thread>

using namespace std;
using namespace sf;

struct Player {
  string time;
  string name;
};

void setText(Text &text, float x, float y);
string readLeaderboard(const string& filename);
void drawLeaderboard(RenderWindow& window, string& leaderboard);
string updateLeaderboard(const string& leaderboard, const string& playerTime, const string& playerName);


int main() {

    // Debugging statement to confirm program start
    cout << "Starting..." << endl;


    int COLUMNS = 0, ROWS = 0, NUM_MINES = 0;

    ifstream configFile("assets/config.cfg");
    if(configFile.is_open()){
        configFile >> COLUMNS >> ROWS >> NUM_MINES;
        configFile.close();
    }else{
        cerr << "Couldn't open config file." << endl;
        configFile.close();
        return -1;
    }

    int winWidth = ( COLUMNS * 32 ), winHeight = ( (ROWS * 32) + 100 );

    //All of this to get tile size
    Texture defaultTileTexture;
    if(!defaultTileTexture.loadFromFile("assets/images/tile_hidden.png")){
      cerr << "Error loading texture\n";
    }
    Vector2u tileSize = defaultTileTexture.getSize();

    RenderWindow welcomeWin(VideoMode(winWidth, winHeight),
                            "Welcome to Minesweeper!",
                            Style::Close);
    welcomeWin.setFramerateLimit(60);


    Font font;
    if(!font.loadFromFile("assets/font.ttf")){
      cerr << "Error loading font\n";
      return -1;
    }

    Text welcomeLine;
    welcomeLine.setFont(font);
    welcomeLine.setString("Welcome to Minesweeper!");
    welcomeLine.setCharacterSize(24); // in pixels, not points!
    welcomeLine.setFillColor(Color::White);
    welcomeLine.setStyle(Text::Bold | Text::Underlined);
    setText(welcomeLine, winWidth / 2.0f, winHeight / 2.0f - 150);

    Text inputLine;
    inputLine.setFont(font);
    inputLine.setString("Enter your name:");
    inputLine.setCharacterSize(20); // in pixels, not points!
    inputLine.setFillColor(Color::White);
    inputLine.setStyle(Text::Bold);
    setText(inputLine, winWidth / 2.0f, winHeight / 2.0f - 75);

    Text nameLine;
    nameLine.setFont(font);
    nameLine.setCharacterSize(18); // in pixels, not points!
    nameLine.setFillColor(Color::Yellow);
    nameLine.setStyle(Text::Bold);


    string playerName;
    char inputChar;
    bool playGame = false;

    while (welcomeWin.isOpen()){

        Event e;
        while (welcomeWin.pollEvent(e)){

            switch(e.type)
            {
              case Event::Closed:
                  welcomeWin.close();
                  break;

              case Event::TextEntered:
                inputChar = static_cast<char>(e.text.unicode);
                if(isalpha(inputChar) && playerName.size() < 10){
                  if(playerName.empty()){
                    playerName += toupper(inputChar);
                  } else {
                    playerName += tolower(inputChar);
                  }
                } else if(inputChar == '\b' && !playerName.empty()){
                  playerName.pop_back();
                }
                nameLine.setString(playerName + '|');
                setText(nameLine, winWidth / 2.0f, winHeight / 2.0f - 45);
                break;

              case Event::KeyPressed:
                if(e.key.code == Keyboard::Enter && !playerName.empty()){
                welcomeWin.close();
                playGame = true;
                }
                break;
            }
        }
        welcomeWin.clear(Color::Blue);
        welcomeWin.draw(welcomeLine);
        welcomeWin.draw(inputLine);
        welcomeWin.draw(nameLine);
        welcomeWin.display();
    }

    if(playGame)
    {
      RenderWindow gameWin(VideoMode(winWidth, winHeight),
                           "Minesweeper",
                           Style::Close);

      gameWin.setFramerateLimit(60);
      MineField field(COLUMNS, ROWS, NUM_MINES);
      bool debugMode = false;
      bool paused = false;
      bool leaderboard = false;
      int pausedSeconds = 0;
      auto startTime = chrono::system_clock::now();

      while (gameWin.isOpen() )
      {

        Event e;
        while (gameWin.pollEvent(e))
        {
          switch(e.type)
          {
            case Event::Closed:
              gameWin.close();
              break;

            case Event::MouseButtonPressed:
              auto mousePosition = Mouse::getPosition(gameWin);
              auto worldCoords = gameWin.mapPixelToCoords(mousePosition);

              int x_index = static_cast<int>(worldCoords.x / tileSize.x),
                  y_index = static_cast<int>(worldCoords.y / tileSize.y);

              //tile coordinates for fun
              //might delete ltr
              cout << "(" << x_index << ", " << y_index << ")" << endl;

              // tile handling
              if(x_index >= 0 && x_index < COLUMNS && y_index >= 0 && y_index < ROWS)
              {
                switch(e.mouseButton.button)
                {
                  case Mouse::Left:
                    field.open_tile(x_index, y_index);
                    if(field.getGame_over() != 0){
                      paused = true;
                      if(field.getGame_over() == 1) {
                        string leaderboardStr =
                            readLeaderboard("leaderboard.txt");
                        string updatedLeaderboard = updateLeaderboard(
                            leaderboardStr, field.getFormattedTime(),
                            playerName);
                        ofstream leaderboardFile("leaderboard.txt", ios::app);
                        if (leaderboardFile.is_open()) {
                          leaderboardFile << updatedLeaderboard;
                          leaderboardFile.close();
                        }
                      }
                    }
                    cout << "Game over status: " << field.getGame_over() << endl;
                    break;

                  case Mouse::Right:
                    field.flag_tile(x_index, y_index);
                    break;
                }// switch for mouse buttons
              } else
              { // else
                Vector2f mousePositionFloat(static_cast<float>(mousePosition.x), static_cast<float>(mousePosition.y));

                // face button functionality
                if(field.getFaceSprite().getGlobalBounds().contains(mousePositionFloat)){
                  field.restart();
                  field.minutes = 0;
                  field.seconds = 0;
                  startTime = chrono::system_clock::now();
                  paused = false;
                  field.isPaused = false;
                }

                //debug button
                if(field.getDebugSprite().getGlobalBounds().contains(mousePositionFloat)){
                  if(field.getGame_over() == 0) {
                    if (debugMode) {
                      field.isDebug = false;
                      debugMode = false;
                    } else {
                      field.isDebug = true;
                      debugMode = true;
                    }
                  }
                }

                //play/pause button
                if(field.getPlay_PauseSprite().getGlobalBounds().contains(mousePositionFloat)){
                  if(field.getGame_over() == 0) {
                    if (paused) {
                      startTime = chrono::system_clock::now() -
                                  chrono::seconds(pausedSeconds);
                      paused = false;
                      field.isPaused = false;
                    } else {
                      auto currentTime = chrono::system_clock::now();
                      chrono::duration<double> elapsedSeconds =
                          currentTime - startTime;
                      pausedSeconds = static_cast<int>(elapsedSeconds.count());
                      paused = true;
                      field.isPaused = true;
                    }
                  }
                }

                //leaderboard button
                if(field.getLeaderBoardSprite().getGlobalBounds().contains(mousePositionFloat)){
                  field.isShowLeaderboard = true;
                  leaderboard = true;

                  RenderWindow leaderboardWin(VideoMode((ROWS * 16) + 50, COLUMNS * 16),
                                       "Leaderboard",
                                       Style::Close);
                  leaderboardWin.setFramerateLimit(60);

                  while(leaderboardWin.isOpen())
                  {
                    string leaderboardStr = readLeaderboard("leaderboard.txt");
                    Event lEvent;
                    while(leaderboardWin.pollEvent(lEvent))
                    {
                      switch(lEvent.type)
                      {
                        case Event::Closed:
                          leaderboardWin.close();
                          leaderboard = false;
                          field.isShowLeaderboard = false;
                          if(!paused) {
                            field.isPaused = false;
                          }
                          break;
                      }
                    }//poll events
                    leaderboardWin.clear(Color::Blue);
                    drawLeaderboard(leaderboardWin, leaderboardStr);
                    leaderboardWin.display();
                  }//while -- leaderboardWin

                }//if leaderboard window
              }// else if not tile clicked
              break;
          }//switch for gameWin events

        }//while -- gameWin event loop

        if(!paused && !leaderboard){
          auto currentTime = chrono::system_clock::now();
          chrono::duration<double> elapsedSeconds = currentTime - startTime;
          int totalSeconds = static_cast<int>(elapsedSeconds.count());
          field.minutes = totalSeconds / 60;
          field.seconds = totalSeconds % 60;
        }

        gameWin.clear(Color::White);
        field.draw(gameWin);
        field.drawCounter(gameWin);
        field.drawFace(gameWin);
        field.drawDebug(gameWin);
        field.drawPlay_Pause(gameWin);
        field.drawLeaderboardButton(gameWin);
        field.drawTimer(gameWin);
        gameWin.display();
      }//while -- game window

    } //if


    return 0;
}

void setText(Text &text, float x, float y) {
    FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width/2.0f,
                   textRect.top + textRect.height/2.0f);
    text.setPosition(sf::Vector2f(x, y));
}
string readLeaderboard(const string &filename) {
  ifstream file(filename);
  string leaderboard;

  if(file.is_open()){
    string line;
    vector<Player> players;
    while(getline(file, line)){
      size_t pos = line.find(',');
      if (pos != string::npos) {
        string time = line.substr(0, pos);
        string name = line.substr(pos + 1);
        players.push_back({time, name});
      }
    }
    file.close();

    sort(players.begin(), players.end(), [](const Player &a, const Player &b) {
      return a.time < b.time;
    });

    // Add rankings
    int rank = 1;
    for (const auto &player : players) {
      leaderboard += to_string(rank++) + "\t " + player.time + "\t " + player.name + "\n\n";
    }
  }

  return leaderboard;
}
string updateLeaderboard(const string& leaderboard, const string& playerTime, const string& playerName) {
  istringstream iss(leaderboard);
  vector<pair<string, string>> entries; // Vector to store time-player pairs

  // Parse the existing leaderboard entries
  string line;
  while (getline(iss, line)) {
    size_t pos = line.find(',');
    if (pos != string::npos && pos >= 5) { // Check if comma is found and after the fifth character
      entries.push_back({line.substr(0, 5), line.substr(pos + 1)}); // Extract time and name
    } else {
      cerr << "Invalid format in leaderboard file: " << line << endl;
    }
  }

  // Add the new entry to the vector
  entries.push_back({playerTime, playerName});

  // Sort the entries based on time (ascending order)
  sort(entries.begin(), entries.end());

  // Truncate the vector to keep only the top five entries
  if (entries.size() > 5) {
    entries.resize(5);
  }

  // Reconstruct the leaderboard string
  ostringstream oss;
  for (const auto& entry : entries) {
    oss << entry.first << "," << entry.second << '\n';
  }

  return oss.str();
}
void drawLeaderboard(RenderWindow &window, string& leaderboard) {
  Font font;
  if(!font.loadFromFile("assets/font.ttf")){
    cerr << "Error loading font\n";
    return;
  }

  Text mainText;
  mainText.setFont(font);
  mainText.setString("LEADERBOARD");
  mainText.setCharacterSize(20); // in pixels, not points!
  mainText.setFillColor(Color::White);
  mainText.setStyle(Text::Bold | Text::Underlined);
  Vector2u windowSize = window.getSize();
  setText(mainText, float(windowSize.x) / 2.f, (float(windowSize.y) / 2.f) - 120);

  Text statsText;
  statsText.setFont(font);

  stringstream ss(leaderboard);
  vector<string> lines;
  string line;
  while (getline(ss, line)) {
    lines.push_back(line);
  }

  // Draw only the top five entries
  string displayText;
  for (int i = 0; i < 5; ++i) {
    displayText += lines[i] + "\n";
  }

  statsText.setString(displayText);
  statsText.setCharacterSize(18); // in pixels, not points!
  statsText.setFillColor(Color::White);
  statsText.setStyle(Text::Bold);
  setText(statsText, float(windowSize.x) / 2.f, (float(windowSize.y) / 2.f) + 20);

  window.draw(mainText);
  window.draw(statsText);
}
