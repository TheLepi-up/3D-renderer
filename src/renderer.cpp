#include "renderer.h"

#include <math.h>
#include "logging.h"
#include "game.h"


void Renderer::update(){
  sf::Image image({windowWidth, windowHeight});

  for (uint j = 0; j < windowHeight; j++)
  {
    for (uint i = 0; i < windowWidth; i++)
    {
      vec3<double> ray = dir + planeX * (2.0 * i / windowWidth - 1) + planeY * (2.0 * j / windowHeight - 1) + (vec3<double>){1e-10, 1e-10, 1e-10};
      vec3<double> step = ray.apply<double>([](double a){ return abs(1/a); });
      vec3<int> orientation = ray.apply<int>([](double a){ return a >= 0 ? 1 : -1; });
      vec3<long> cell = position.apply(static_cast<double (*)(double)>(floor));
      vec3<double> current = ray.apply<double, double>([](double r, double p){ return r >= 0 ? (1 - p) / r : -p / r; }, position - cell);
      bool oob = false;
      int side;
      while(true){
        if(current.x < current.y && current.x < current.z){
          current.x += step.x;
          cell.x += orientation.x;
          side = 0;
        } else if(current.y < current.z){
          current.y += step.y;
          cell.y += orientation.y;
          side = 1;
        } else {
          current.z += step.z;
          cell.z += orientation.z;
          side = 2;
        }
        if(cell.apply<bool, long>([](long a, long b){ return a < 0 || a >= b; }, 
                                  {gridSizeX, gridSizeY, gridSizeZ}).any()){
          oob = true;
          break;
        }
        if(getCell(cell))
          break;
      }
      sf::Color col;
      if(oob){
        col = sf::Color::Black;
      }else{
        col = sf::Color((cell.x + 1)*10, (cell.y + 1)*10, (cell.z + 1)*10);
      }

      image.setPixel(sf::Vector2u(i, j), col);
    }
  }
  sf::Texture t;
  if(t.loadFromImage(image)){
    sf::Sprite s(t);
    s.setPosition({0, 0});
    s.setScale({1, 1});
    window.draw(s);
    window.display();
  }
  logTrace("screen rendered");
}