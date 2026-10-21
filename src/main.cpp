#include <iostream>
#include <SFML/System.hpp>
#include <chrono>
#include <thread>
#include <math.h>
#include "config.h"
#include "renderer.h"

using namespace std;

int main(int argc, char** argv) {
  sf::RenderWindow window(sf::VideoMode({ windowWidth, windowHeight }), "3D_renderer");
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
  bool mouseActive = false;
  //renderer.update();
  while (window.isOpen()) {
    while (const optional<sf::Event> event = window.pollEvent()) {
      if (event->is<sf::Event::Closed>()) {
        window.close();
      }
      if (const sf::Event::KeyPressed* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
        switch (keyPressed->scancode) {
        case sf::Keyboard::Scancode::Up:
          renderer.move({ 0.1, 0.0, 0.0 });
          break;
        case sf::Keyboard::Scancode::Down:
          renderer.move({ -0.1, 0.0, 0.0 });
          break;
        case sf::Keyboard::Scancode::Left:
          renderer.move({ 0.0, -0.1, 0.0 });
          break;
        case sf::Keyboard::Scancode::Right:
          renderer.move({ 0.0, 0.1, 0.0 });
          break;
        case sf::Keyboard::Scancode::W:
          renderer.move({ 0.0, 0.0, 0.1 });
          break;
        case sf::Keyboard::Scancode::S:
          renderer.move({ 0.0, 0.0, -0.1 });
          break;
        case sf::Keyboard::Scancode::Escape:
          mouseActive = false;
          window.setMouseCursorGrabbed(false);
          window.setMouseCursorVisible(true);
          break;

        // case sf::Keyboard::Scancode::J:
        //   renderer.selectedSquare ++;
        //   break;
        // case sf::Keyboard::Scancode::K:
        //   renderer.selectedSquare --;
        //   break;
        
        default:
          break;
        }
      }
      if(mouseActive){
        if (const sf::Event::MouseMoved* mouseMoved = event->getIf<sf::Event::MouseMoved>()) {
          sf::Vector2i delta =  mouseMoved->position - sf::Vector2i(windowWidth, windowHeight) / 2;
          renderer.rotate(vec2<double>{(double)delta.x, (double)delta.y} / (windowWidth / 2));
        }
      }
      const sf::Event::MouseButtonPressed *mbEvent = nullptr;
      if((mbEvent = event->getIf<sf::Event::MouseButtonPressed>()) && mbEvent->button == sf::Mouse::Button::Left){
        mouseActive = true;
        window.setMouseCursorGrabbed(true);
        window.setMouseCursorVisible(false);
      }
    }
    if(mouseActive)
      sf::Mouse::setPosition({windowWidth / 2, windowHeight / 2}, window);
    auto start = chrono::system_clock::now();
    renderer.update();
    auto timediff = chrono::system_clock::now() - start;
    std::cout << std::setw(9) << timediff.count() / 1000 << "\r";
    std::flush(std::cout);
    this_thread::sleep_until(lastUpdate += chrono::microseconds(1000000/FPS));
  }
}