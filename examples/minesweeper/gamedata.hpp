#ifndef GAMEDATA_HPP_
#define GAMEDATA_HPP_

#include <bitset>

enum class Input { Right, Left };
enum class GameState { Start, Play, Won, Lost };

struct GameData {
  GameState m_gameState{GameState::Start};
  std::bitset<2> m_input;  // [left, right]
};

#endif