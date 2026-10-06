#include <iostream>
#include <SFML/System.hpp>
#include <chrono>
#include <thread>
#include <math.h>
#include "config.h"
#include "renderer.h"

using namespace std;

int main(int argc, char** argv) {
  sf::RenderWindow window(sf::VideoMode({ windowWidth, windowHeight }), "3D_maze");
  Renderer renderer(window);
  for (uint k = 0; k < gridSizeZ; k++)
  {
    for (uint j = 0; j < gridSizeY; j++)
    {
      for (uint i = 0; i < gridSizeX; i++)
      {
        int x = (int)i - gridSizeX / 2;
        int y = (int)j - gridSizeY / 2;
        int z = (int)k - gridSizeZ / 2;
        renderer.setCell({i, j, k}, x*x + y*y + z*z > gridSizeX*gridSizeX / 4 - 16);
      }
    }
  }

  auto lastUpdate = chrono::system_clock::now();
  renderer.update();
  while (window.isOpen()) {
    while (const optional<sf::Event> event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        window.close();
      }
      if (const sf::Event::KeyPressed* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
        switch (keyPressed->scancode) {
        case sf::Keyboard::Scancode::Up:
          position.x += 0.1;
          break;
        case sf::Keyboard::Scancode::Down:
          position.x -= 0.1;
          break;
        case sf::Keyboard::Scancode::Left:
          position.y -= 0.1;
          break;
        case sf::Keyboard::Scancode::Right:
          position.y += 0.1;
          break;
        case sf::Keyboard::Scancode::W:
          position.z += 0.1;
          break;
        case sf::Keyboard::Scancode::S:
          position.z -= 0.1;
          break;
        default:
          break;
        }
      }
      if (const sf::Event::MouseMoved* mouseMoved = event->getIf<sf::Event::MouseMoved>()) {
        sf::Vector2f pos = static_cast<sf::Vector2f>(mouseMoved->position - sf::Vector2i(windowWidth, windowHeight) / 2);
        sf::Vector2f angle = pos.componentWiseDiv({windowWidth / 2 / M_PI, windowHeight / 2 / M_PI});
        renderer.setPlaneX({-sin(angle.x), cos(angle.x), 0});
        renderer.setPlaneY({-cos(angle.x) * sin(angle.y), -sin(angle.x) * sin(angle.y), cos(angle.y)});
        renderer.setDir({cos(angle.x) * cos(angle.y), sin(angle.x) * cos(angle.y), sin(angle.y)});
      }
    }
    //renderer.update();
    this_thread::sleep_until(lastUpdate += chrono::microseconds(1000000/FPS));
  }
}