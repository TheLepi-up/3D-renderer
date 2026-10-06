#include "renderer.h"

#include <math.h>
#include <set>
#include "logging.h"
#include "game.h"

std::pair<double, double> square::intersectX(double y) const{
  double xMin = -windowWidth / 2;
  double xMax = windowWidth / 2 - 1;
  for (const line2D &line : bounds)
  {
    if(line.v.y == 0){
      if((line.A.y < y && line.v.x > 0) || (line.A.y > y && line.v.x < 0))
        continue; // scanline is inside
      else{
        return {1, -1}; // scanline is outside
      }
    }
    double intersection = line.A.x + ((y - line.A.y) / line.v.y) * line.v.x;
    if(line.v.y > 0)
      xMin = std::max(xMin, intersection);
    else
      xMax = std::min(xMax, intersection);
  }
  return {xMin, xMax};
}

namespace shmap{
  short shadowmap[windowHeight][windowWidth] = {};
  inline void insert(const square &sq){
    for(int y = std::ceil(sq.boundingBoxMinY); y <= sq.boundingBoxMaxY; y++){
      std::pair interval = sq.intersectX(y);
      if(interval.second < interval.first)
        continue;
      uint start = std::ceil(interval.first) + windowWidth / 2;
      uint end = std::floor(interval.second) + windowWidth / 2 + 1;
      short* scanline = shadowmap[y + windowHeight / 2];
      std::fill(&scanline[start], &scanline[end], end);
    }
  }
  inline uint nextX(uint x, uint y){
    uint nX;
    while(x < windowWidth && (nX = shadowmap[y][x]) != 0){
      x = nX;
    }
    return x;
  }
  inline uint nextXCentered(int x, int y){
    return nextX(x + windowWidth / 2, y + windowHeight / 2) - windowWidth / 2;
  }
  inline void reset(){
    std::fill(&shadowmap[0][0], &shadowmap[0][0] + sizeof(shadowmap) / sizeof(shadowmap[0][0]), 0);
  }
}

std::multiset<square> tiles;

void Renderer::update(){
  shmap::reset();
  tiles.clear();
  for (uint j = 0; j < windowHeight; j++) {
    for (uint i = 0; i < windowWidth; i = shmap::nextX(i + 1, j)) {
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
        
        vec3<int> point00;
        vec3<int> point01;
        vec3<int> point10;
        vec3<int> point11;
        switch(side){
          case 0:
            point00 = cell;
            point01 = cell + vec3<int>({0, 1, 0});
            point10 = cell + vec3<int>({0, 0, 1});
            point11 = cell + vec3<int>({0, 1, 1});
            break;
          case 1:
            point00 = cell + vec3<int>({1, 0, 0});
            point01 = cell + vec3<int>({1, 1, 0});
            point10 = cell + vec3<int>({1, 0, 1});
            point11 = cell + vec3<int>({1, 1, 1});
            break;
          case 2:
            point00 = cell;
            point01 = cell + vec3<int>({0, 0, 1});
            point10 = cell + vec3<int>({1, 0, 0});
            point11 = cell + vec3<int>({1, 0, 1});
            break;
          case 3:
            point00 = cell + vec3<int>({0, 1, 0});
            point01 = cell + vec3<int>({0, 1, 1});
            point10 = cell + vec3<int>({1, 1, 0});
            point11 = cell + vec3<int>({1, 1, 1});
            break;
          case 4:
            point00 = cell;
            point01 = cell + vec3<int>({1, 0, 0});
            point10 = cell + vec3<int>({0, 1, 0});
            point11 = cell + vec3<int>({1, 1, 0});
            break;
          case 5:
            point00 = cell + vec3<int>({0, 0, 1});
            point01 = cell + vec3<int>({1, 0, 1});
            point10 = cell + vec3<int>({0, 1, 1});
            point11 = cell + vec3<int>({1, 1, 1});
            break;
          default:
            logError("unreachable!");
            continue;
        }
        vec3<double> diffpoint00 = (vec3<double>)point00 - position;
        vec3<double> diffpoint01 = (vec3<double>)point01 - position;
        vec3<double> diffpoint10 = (vec3<double>)point10 - position;
        vec3<double> diffpoint11 = (vec3<double>)point11 - position;
        double dist00 = diffpoint00 * dir;
        double dist01 = diffpoint01 * dir;
        double dist10 = diffpoint10 * dir;
        double dist11 = diffpoint11 * dir;
        vec3<double> screenPoint00 = diffpoint00 / dist00 - dir;
        vec3<double> screenPoint01 = diffpoint01 / dist01 - dir;
        vec3<double> screenPoint10 = diffpoint10 / dist10 - dir;
        vec3<double> screenPoint11 = diffpoint11 / dist11 - dir;
        vec2<double> projectedPoint00 = {screenPoint00 * planeX, screenPoint00 * planeY};
        vec2<double> projectedPoint01 = {screenPoint01 * planeX, screenPoint01 * planeY};
        vec2<double> projectedPoint10 = {screenPoint10 * planeX, screenPoint10 * planeY};
        vec2<double> projectedPoint11 = {screenPoint11 * planeX, screenPoint11 * planeY};

        
        square tile = {
          .distance = std::min({dist00, dist01, dist10, dist11}),
          .point00 = point00,
          .point01 = point01,
          .point10 = point10,
          .point11 = point11,
          .bounds = {
            {projectedPoint01 - projectedPoint00, projectedPoint00},
            {projectedPoint11 - projectedPoint01, projectedPoint01},
            {projectedPoint10 - projectedPoint11, projectedPoint11},
            {projectedPoint00 - projectedPoint10, projectedPoint10}
          },
          .boundingBoxMinY = std::min({projectedPoint00.y, projectedPoint01.y, projectedPoint10.y, projectedPoint11.y, -windowHeight / 2.0}),
          .boundingBoxMaxY = std::max({projectedPoint00.y, projectedPoint01.y, projectedPoint10.y, projectedPoint11.y, windowHeight / 2.0 - 1}),
          .cell = cell,
          .cellType = getCell(cell),
          .side = side
        };
        tiles.insert(tile);
        shmap::insert(tile);
        goto break_all;
      }
    }
  }
break_all:
  sf::Image image({windowWidth, windowHeight}, sf::Color::Black);
  // TODO: render tiles
  shmap::reset();
  for(const square &tile : tiles){
    vec3<double> W = position - tile.point00;
    for(int y = std::ceil(tile.boundingBoxMinY); y < tile.boundingBoxMaxY; y++){
      std::pair interval = tile.intersectX(y);
      for(int x = std::ceil(interval.first); x < interval.second; x = shmap::nextXCentered(x + 1, y)){
        vec2<double> texturePos;

        vec3<double> ray = dir + planeX * (2.0 * x / windowWidth - 1) + planeY * (2.0 * y / windowHeight - 1);
        double dot = ray[tile.side / 2]; //dot product with normal of square
        if(dot != 0){
          double fac = -W[tile.side / 2] / dot; // fac = - W * N / dot
          vec3<double> I = position + ray * fac;
          vec3<double> relativePos = I - tile.point00;
          texturePos = {
            //std::clamp(relativePos[(tile.side / 2 + 1) % 3], 0.0, 1.0),
            relativePos[(tile.side / 2 + 1) % 3],
            //std::clamp(relativePos[(tile.side / 2 + 2) % 3], 0.0, 1.0)
            relativePos[(tile.side / 2 + 2) % 3]
          };
          
        } else
          texturePos = {0, 0};
        
        image.setPixel(sf::Vector2u(x + windowWidth / 2, y + windowHeight / 2), textureColor(texturePos, tile));
      }
    }
    shmap::insert(tile);

    for (double i = 0; i < 1; i += 0.01)
    {
      vec2<double> pixel = (tile.bounds[0].A + tile.bounds[0].v * i + vec2<double>({1.0, 1.0}));
      image.setPixel({std::clamp<uint>(pixel.x * windowWidth / 2, 0, windowWidth), std::clamp<uint>(pixel.y * windowHeight / 2, 0, windowHeight)}, sf::Color::White);
      pixel = (tile.bounds[1].A + tile.bounds[1].v * i + vec2<double>({1.0, 1.0}));
      image.setPixel({std::clamp<uint>(pixel.x * windowWidth / 2, 0, windowWidth), std::clamp<uint>(pixel.y * windowHeight / 2, 0, windowHeight)}, sf::Color::White);
      pixel = (tile.bounds[2].A + tile.bounds[2].v * i + vec2<double>({1.0, 1.0}));
      image.setPixel({std::clamp<uint>(pixel.x * windowWidth / 2, 0, windowWidth), std::clamp<uint>(pixel.y * windowHeight / 2, 0, windowHeight)}, sf::Color::White);
      pixel = (tile.bounds[3].A + tile.bounds[3].v * i + vec2<double>({1.0, 1.0}));
      image.setPixel({std::clamp<uint>(pixel.x * windowWidth / 2, 0, windowWidth), std::clamp<uint>(pixel.y * windowHeight / 2, 0, windowHeight)}, sf::Color::White);
    }
    
    break;
  }
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

sf::Color Renderer::textureColor(vec2<double> position, const square &sq) const{
  //vec2<int> pixel = position * 8;
  //double brightness = (16 - ((pixel.x ^ pixel.y) & 1) * 5);
  //return sf::Color((sq.cell.x + 1)*brightness, (sq.cell.y + 1)*brightness, (sq.cell.z + 1)*brightness);
  if(position.x < 0 || position.x >= 1 || position.y < 0 || position.y >= 1)
    return sf::Color(position.x + 128, position.y + 128, 128);
  return sf::Color(position.x * 256, position.y * 256, 0);
}