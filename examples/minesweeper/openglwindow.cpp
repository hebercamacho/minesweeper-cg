#include "openglwindow.hpp"

#include <fmt/core.h>
#include <imgui.h>
#include <chrono>
#include <cppitertools/itertools.hpp>

void OpenGLWindow::initializeGL() {
  // Load a new font
  ImGuiIO &io{ImGui::GetIO()};
  //const auto filename{getAssetsPath() + "Inconsolata-Medium.ttf"};
  m_font = io.Fonts->AddFontDefault();
  if (m_font == nullptr) {
    throw abcg::Exception{abcg::Exception::Runtime("Cannot load font file")};
  }

  abcg::glClearColor(0, 0, 0, 1);

  restart();
}

void OpenGLWindow::paintGL() { 
  abcg::glClear(GL_COLOR_BUFFER_BIT); 
}

void OpenGLWindow::paintUI() {
  const auto appWindowWidth{static_cast<float>(getWindowSettings().width)};
  const auto appWindowHeight{static_cast<float>(getWindowSettings().height)};

  // "Campo minado" window
  {
    ImGui::SetNextWindowSize(ImVec2(appWindowWidth, appWindowHeight));
    ImGui::SetNextWindowPos(ImVec2(0, 0));

    const auto flags{ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize};
    ImGui::Begin("Campo minado", nullptr, flags);

    // Menu com botão de restart pra reiniciar o jogo
    {
      bool restartSelected{};
      bool preencherSelected{};
      if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Game")) {
          ImGui::MenuItem("Restart", nullptr, &restartSelected);
          ImGui::MenuItem("Preencher", nullptr, &preencherSelected);
          ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
      }
      if (restartSelected) restart();
      if (preencherSelected) preencher_tabuleiro();
    }

    // Texto explicativo (ganhou/perdeu/jogando)
    std::string text;
    switch (m_gameState) {
      case GameState::Play:
        // text = fmt::format("{} turn", m_turn ? 'X' : 'O');
        text = "";
        break;
      case GameState::Won:
        text = "Você ganhou!";
        break;
      case GameState::Lost:
        text = "Você perdeu!";
        break;
    }
    //Centralizar texto na tela
    ImGui::SetCursorPosX(
        (appWindowWidth - ImGui::CalcTextSize(text.c_str()).x) / 2);
    ImGui::Text("%s", text.c_str());
    ImGui::Spacing();
    ImGui::Spacing();

    // Criar tabuleiro
    const auto gridHeight{appWindowHeight - 22 - 58 - (m_N * 10) - 58};
    ImGui::PushFont(m_font);
    // Pra cada linha, criar uma coluna
    for (auto i : iter::range(m_N)) {
      ImGui::Columns(m_N);
      // Pra cada coluna, criar um botão
      for (auto j : iter::range(m_N)) {
        auto offset{i * m_N + j}; //offset = posição do botão atual no vetor
        std::string text = m_clicado.at(offset) ? fmt::format("{}", m_bombas.at(offset)) : fmt::format(""); //texto pra ser colocado dentro do botão, dependendo se ele já foi clicado
        ImGui::Button(text.c_str(), ImVec2(-1, gridHeight / m_N)); //pra que esse -1?
        if (m_gameState == GameState::Play && !m_clicado.at(offset)) { //esse if permite clicar só se estiver vazio
          if (ImGui::IsItemClicked()) {
            fmt::print(stdout, "Clicado na celula {}X{}.\n", i, j);
            m_clicado.at(offset) = true; //revelado o que está ocultado
            checkBoard(); //decidir se perdeu ou ganhou
            //m_turn = !m_turn; //troca de turno
          }
        }
        ImGui::NextColumn();
      }
      if (i < 2) ImGui::Separator(); //precisa?
    }
    ImGui::Columns(1); //não sei o que faz
    ImGui::PopFont(); //não sei o que faz

    ImGui::Spacing(); //não sei o que faz
    ImGui::Spacing(); //não sei o que faz

    // "Restart game" button
    {
      if (ImGui::Button("Reiniciar jogo", ImVec2(-1, 50.0f))) {
        restart();
      }
    }

    ImGui::End();
  }
}

void OpenGLWindow::checkBoard() {
  if (m_gameState != GameState::Play) return; //se ganhou ou perdeu, manter a tela igual

  // checar nas linhas e colunas se tem uma bomba clicada
  for (const auto i : iter::range(m_N)) {
    for (const auto j : iter::range(m_N)) {
      const auto offset{i * m_N + j};
      if(m_bombas.at(offset) == 'X' && m_clicado.at(offset) == true)
      {
        m_gameState = GameState::Lost;
      }
    }
  }

  //TODO:
  //aparecer os números
  //se o número for zero, clicar também em todos envolta (e chamar essa função recursivamente pra cada um)
}

//função para reiniciar o jogo para as configurações iniciais
void OpenGLWindow::restart() {
  m_gameState = GameState::Play;
  m_bombas.fill('0');
  m_clicado.fill(false);
  fmt::print(stdout, "Jogo reiniciado.\n");
}

void OpenGLWindow::preencher_tabuleiro()
{
  //define o número de bombas como 12% do tabuleiro, arredondado pra cima
  const auto bombas = ceil(m_N * m_N * 0.12f);
  fmt::print(stdout, "{} bombas geradas.\n", bombas);

  int i = 0; //número de bombas já colocadas
  while(i < bombas){
    // Iniciar gerador de números aleatórios
    m_randomEngine.seed(std::chrono::steady_clock::now().time_since_epoch().count());

    // Pegar uma célula aleatória (de zero a m_N^2 - 1)
    std::uniform_real_distribution<float> realDistribution(0.0f, m_N * m_N - 1.0f);
    const auto offset = floor(realDistribution(m_randomEngine));
    fmt::print(stdout, "Sorteada posição {}.\n", offset);

    //Preencher essa célula com uma bomba, se já não for uma
    if(m_bombas.at(offset) != 'X')
    {
      m_bombas.at(offset) = 'X';
      i++;
      fmt::print(stdout, "Bomba colocada na posição {}.\n", offset);
      fmt::print(stdout, "Agora existem {} bombas.\n", bombas);
    }

    //Pra cada vizinho, somar 1 ao número, mas só se esse vizinho não for uma bomba
  }
}