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
      if (preencherSelected) preencher_tabuleiro(0);
    }

    // Texto explicativo (ganhou/perdeu/jogando)
    std::string text;
    switch (m_gameState) {
      case GameState::Start:
        text = "Clique em um quadrado para começar";
        break;
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
        auto offset{i * m_N + j}; //offset = posicao do botão atual no vetor
        std::string text = m_clicado.at(offset) ? fmt::format("{}", m_bombas.at(offset)) : fmt::format(""); //texto pra ser colocado dentro do botão, dependendo se ele já foi clicado
        ImGui::Button(text.c_str(), ImVec2(appWindowWidth / m_N, gridHeight / m_N));
        
        if (!m_clicado.at(offset)) { //esse if permite clicar só se estiver vazio
          if (ImGui::IsItemClicked()) {
            if (m_gameState == GameState::Start)
            {
              preencher_tabuleiro(offset);
            }
            if (m_gameState == GameState::Play)
            {
              fmt::print(stdout, "Clicado na celula {}X{}.\n", i, j);
              m_clicado.at(offset) = true; //revelado o que está ocultado
              checkBoard(); //decidir se perdeu ou ganhou
            }
          }
        }
        ImGui::NextColumn();
      }
    }
    ImGui::Columns(1); //coluna do botão de restart
    ImGui::PopFont(); //se tirar quebra
    //faz um espacinho pra separar o botão de restart
    ImGui::Spacing(); 
    ImGui::Spacing();

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
  //se o número for zero, clicar também em todos envolta (e chamar essa função recursivamente pra cada um)
}

//função para reiniciar o jogo para as configurações iniciais
void OpenGLWindow::restart() {
  m_gameState = GameState::Start;
  m_bombas.fill('0');
  m_clicado.fill(false);
  fmt::print(stdout, "Jogo reiniciado.\n");
}

void OpenGLWindow::preencher_tabuleiro(int clicada)
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
    const int offset = floor(realDistribution(m_randomEngine));
    fmt::print(stdout, "Sorteada posicao {}.\n", offset);

    //Preencher essa célula com uma bomba, se  não for a clicada e se já não for uma bomba
    if(offset != clicada && m_bombas.at(offset) != 'X')
    {
      m_bombas.at(offset) = 'X';
      i++;
      fmt::print(stdout, "Bomba colocada na posicao {}.\n", offset);
      fmt::print(stdout, "Agora existem {} bombas.\n", i);

      //Pra cada vizinho, somar 1 ao número, mas só se esse vizinho não for uma bomba
      somar_vizinhos(offset);
    }
  }

  m_gameState = GameState::Play;
}

bool OpenGLWindow::isVizinho(int n, int v)
{
  //regras que fazem de v um não-vizinho de n
  if(  v < 0
    || v >= m_N * m_N
    || (n % m_N == 0 && v % m_N == m_N - 1)
    || (n % m_N == m_N - 1 && v % m_N == 0)
  )
    return false;
  return true;
}

void OpenGLWindow::somar_vizinhos(int n)
{
  int v = n - m_N - 1;
  if(isVizinho(n, v) && m_bombas.at(v) != 'X')
    m_bombas.at(v) = m_bombas.at(v) + 1;
  
  v = n - m_N;
  if(isVizinho(n, v) && m_bombas.at(v) != 'X')
    m_bombas.at(v) = m_bombas.at(v) + 1;

  v = n - m_N + 1;
  if(isVizinho(n, v) && m_bombas.at(v) != 'X')
    m_bombas.at(v) = m_bombas.at(v) + 1;

  v = n - 1;
  if(isVizinho(n, v) && m_bombas.at(v) != 'X')
    m_bombas.at(v) = m_bombas.at(v) + 1;

  v = n + 1;
  if(isVizinho(n, v) && m_bombas.at(v) != 'X')
    m_bombas.at(v) = m_bombas.at(v) + 1;

  v = n + m_N - 1;
  if(isVizinho(n, v) && m_bombas.at(v) != 'X')
    m_bombas.at(v) = m_bombas.at(v) + 1;

  v = n + m_N;
  if(isVizinho(n, v) && m_bombas.at(v) != 'X')
    m_bombas.at(v) = m_bombas.at(v) + 1;

  v = n + m_N + 1;
  if(isVizinho(n, v) && m_bombas.at(v) != 'X')
    m_bombas.at(v) = m_bombas.at(v) + 1;
}
