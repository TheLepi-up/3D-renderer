#include "renderer.h"

#include <math.h>
#include <set>
#include "logging.h"
#include "game.h"

struct square{
  double distance;
  vec2<double> apparentPos;
  vec2<double> apparentDelta1;
  vec2<double> apparentDelta2;
  vec3<uint> cell;
  uint8_t cellType;
  uint8_t side;
  bool operator<(const square &other) const { return distance < other.distance; }
};

std::multiset<square> tiles;

void Renderer::update(){
  short shadowmap[windowWidth][windowHeight] = {};

  for (uint j = 0; j < windowHeight; j++) {
    for (uint i = 0; i < windowWidth; i++) {
      if(shadowmap[i][j]){
        // TODO: implement shadowmap skipping 
        continue;
      }

      vec3<double> ray = dir + planeX * (2.0 * i / windowWidth - 1) + planeY * (2.0 * j / windowHeight - 1) + (vec3<double>){1e-10, 1e-10, 1e-10};
      vec3<double> step = ray.apply<double>([](double a){ return abs(1/a); });
      vec3<int8_t> orientation = ray.apply<int8_t>([](double a){ return (int8_t)(a >= 0 ? 1 : -1); });
      vec3<int> cell = position.apply(static_cast<double (*)(double)>(floor));
      vec3<double> current = ray.apply<double, double>([](double r, double p){ return r >= 0 ? (1 - p) / r : -p / r; }, position - cell);
      bool oob = false;
      uint8_t side;
      while(true){
        if(current.x < current.y && current.x < current.z){
          current.x += step.x;
          cell.x += orientation.x;
          side = 0;
        } else if(current.y < current.z){
          current.y += step.y;
          cell.y += orientation.y;
          side = 2;
        } else {
          current.z += step.z;
          cell.z += orientation.z;
          side = 4;
        }
        if(cell.apply<bool, int>([](int a, int b){ return a < 0 || a >= b; }, 
                                  {gridSizeX, gridSizeY, gridSizeZ}).any()){
          oob = true;
          break;
        }
        if(getCell(cell))
          break;
      }
      if(!oob){
        if((side == 0 && orientation.x < 0) || 
           (side == 2 && orientation.y < 0) || 
           (side == 4 && orientation.z < 0))
          side ++;
        
        vec3<int> point1;
        vec3<int> point2;
        vec3<int> point3;
        vec3<int> point4;
        switch(side){
          case 0:
            point1 = cell;
            point2 = cell + vec3<int>({0, 1, 0});
            point3 = cell + vec3<int>({0, 0, 1});
            point4 = cell + vec3<int>({0, 1, 1});
          case 1:
            point1 = cell + vec3<int>({1, 0, 0});
            point2 = cell + vec3<int>({1, 1, 0});
            point3 = cell + vec3<int>({1, 0, 1});
            point4 = cell + vec3<int>({1, 1, 1});
          case 2:
            point1 = cell;
            point2 = cell + vec3<int>({0, 0, 1});
            point3 = cell + vec3<int>({1, 0, 0});
            point4 = cell + vec3<int>({1, 0, 1});
          case 3:
            point1 = cell + vec3<int>({0, 1, 0});
            point2 = cell + vec3<int>({0, 1, 1});
            point3 = cell + vec3<int>({1, 1, 0});
            point4 = cell + vec3<int>({1, 1, 1});
          case 4:
            point1 = cell;
            point2 = cell + vec3<int>({1, 0, 0});
            point3 = cell + vec3<int>({0, 1, 0});
            point4 = cell + vec3<int>({1, 1, 0});
          case 5:
            point1 = cell + vec3<int>({0, 0, 1});
            point2 = cell + vec3<int>({1, 0, 1});
            point3 = cell + vec3<int>({0, 1, 1});
            point4 = cell + vec3<int>({1, 1, 1});
          default:
            logError("unreachable!");
            continue;
        }
        vec3<double> diffpoint1 = (vec3<double>)point1 - position;
        vec3<double> diffpoint2 = (vec3<double>)point2 - position;
        vec3<double> diffpoint3 = (vec3<double>)point3 - position;
        vec3<double> diffpoint4 = (vec3<double>)point4 - position;
        double dist1 = diffpoint1 * dir;
        double dist2 = diffpoint2 * dir;
        double dist3 = diffpoint3 * dir;
        double dist4 = diffpoint4 * dir;
        vec3<double> screenPoint1 = diffpoint1 / dist1 - dir;
        vec3<double> screenPoint2 = diffpoint2 / dist2 - dir;
        vec3<double> screenPoint3 = diffpoint3 / dist3 - dir;
        vec2<double> apparentPos1 = {screenPoint1 * planeX, screenPoint1 * planeY};
        vec2<double> apparentPos2 = {screenPoint2 * planeX, screenPoint2 * planeY};
        vec2<double> apparentPos3 = {screenPoint3 * planeX, screenPoint3 * planeY};

        
        square tile = {
          .distance = std::min({dist1, dist2, dist3, dist4}),
          .apparentPos = apparentPos1,
          .apparentDelta1 = apparentPos2 - apparentPos1,
          .apparentDelta2 = apparentPos3 - apparentPos1,
          .cell = cell,
          .cellType = getCell(cell),
          .side = side
        };
        tiles.insert(tile);

        // TODO: insert into shadowmap
      }



    }
  }

  sf::Image image({windowWidth, windowHeight});
  // TODO: render tiles

  // for (uint j = 0; j < windowHeight; j++) {
  //   for (uint i = 0; i < windowWidth; i++) {
  //       sf::Color col;
  //     if(oob){
  //       col = sf::Color::Black;
  //     }else{
  //       col = sf::Color((cell.x + 1)*10, (cell.y + 1)*10, (cell.z + 1)*10);
  //     }
  //     image.setPixel(sf::Vector2u(i, j), col);
  //   }
  // }
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