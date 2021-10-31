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
  enum class GameState { Start, Play, Won, Lost };
  static const int m_N{16};  // Board size is m_N x m_N
  //define o número de bombas como 12% do tabuleiro, arredondado pra cima
  int bombas = ceil(m_N * m_N * 0.12f);
  GameState m_gameState{GameState::Start};
  std::array<char, m_N * m_N> m_bombas{};  // '\0', 'X' or 'O'
  std::array<bool, m_N * m_N> m_clicado{}; //true = revelado, false = oculto

  ImFont* m_font{};

  std::default_random_engine m_randomEngine;

  void checkBoard(); //função que checa constantemente se o jogo terminou
  void restart(); //função que reinicia o jogo
  void preencher_tabuleiro(int); //função que cria as bombas em posições aleatórias exceto a clicada
  void somar_vizinhos(int); //função que recebe uma célula e faz a soma de seus vizinhos
  bool isVizinho(int, int); //função que responde se um n possui um vizinho válido v
  void clicar_nos_vizinhos(int); //função que clica em todos os vizinhos de uma celula com valor zero
};

#endif
