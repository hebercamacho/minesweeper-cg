#ifndef OPENGLWINDOW_HPP_
#define OPENGLWINDOW_HPP_

#include <array>
#include <glm/vec2.hpp>
#include <random>

#include "abcg.hpp"

class OpenGLWindow : public abcg::OpenGLWindow {
 protected:
  void initializeGL() override;
  void paintGL() override;
  void paintUI() override;
  void resizeGL(int width, int height) override;
  void terminateGL() override;

 private:
  GLuint m_vao{};
  GLuint m_vboPositions{};
  GLuint m_vboColors{};
  GLuint m_program{};

  int m_viewportWidth{};
  int m_viewportHeight{};

  std::default_random_engine m_randomEngine;

  std::array<glm::vec4, 3> m_vertexColors{glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f}, 
                                          glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f},
                                          glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f}};
  bool pausado = true; //pausa o programa se false
  bool random_colors = true; //cores aleatórias se true
  bool flat_colors = true; //cores sólidas se true
  int cont = 0;
  int delay = 100;
  void setupModel();
};
#endif