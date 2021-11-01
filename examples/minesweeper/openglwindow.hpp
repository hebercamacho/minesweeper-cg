#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <imgui.h>

#include <array>
#include <random>
#include "abcg.hpp"
#include "gamedata.hpp"

class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void handleEvent(SDL_Event& event) override;
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;
  void terminateGL() override;

 private:
  GLuint m_objectsProgram{};

  int m_viewportWidth{};
  int m_viewportHeight{};
  GameData m_gameData;

  // enum class GameState { Start, Play, Won, Lost };
  static const int m_N{9};  // tamanho do tabuleiro
  //define o número de bombas como 12% do tabuleiro, arredondado pra cima
  int bombas = ceil(m_N * m_N * 0.12f);
  // GameState m_gameState{GameState::Start};
  std::array<char, m_N * m_N> m_bombas{};  // representação do conteudo de uma célula, onde X = bomba e 0-9 = número de bombas vizinhas
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
