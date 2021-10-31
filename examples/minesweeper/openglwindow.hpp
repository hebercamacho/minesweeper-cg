#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <imgui.h>

#include <array>
#include <random>
#include "abcg.hpp"

class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;

 private:
  enum class GameState { Play, Won, Lost };
  static const int m_N{9};  // Board size is m_N x m_N

  GameState m_gameState{GameState::Play};
  bool m_turn{true};                      // true = X, false = O
  std::array<char, m_N * m_N> m_bombas{};  // '\0', 'X' or 'O'
  std::array<bool, m_N * m_N> m_clicado{}; //true = revelado, false = oculto

  ImFont* m_font{};

  std::default_random_engine m_randomEngine;

  void checkBoard(); //função que checa constantemente se o jogo terminou
  void restart(); //função que reinicia o jogo
  void preencher_tabuleiro(); //função que cria as bombas em posições aleatórias
};

#endif
