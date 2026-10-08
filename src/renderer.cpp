#include "renderer.h"

#include <math.h>
#include <set>
#include <iostream>
#include "logging.h"
#include "game.h"

const square *selectedTile;

inline constexpr double scaleToWindowX(double coordinate) {
  return coordinate * windowWidth / 2 + windowWidth / 2;
}

inline constexpr double scaleToWindowY(double coordinate) {
  return coordinate * windowWidth / 2 + windowHeight / 2;
}

inline constexpr double scaleFromWindowX(double coordinate) {
  return (2.0 * coordinate - windowWidth) / windowWidth;
}

inline constexpr double scaleFromWindowY(double coordinate) {
  return (2.0 * coordinate - windowHeight) / windowWidth;
}

inline constexpr vec2<double> scaleToWindow(vec2<double> point){
  return { scaleToWindowX(point.x), scaleToWindowY(point.y) };
}

std::pair<double, double> square::intersectX(double y) const{
  double xMin = -1;
  double xMax = 1;
  for (const line2D &line : bounds)
  {
    if(line.v.y == 0){
      if((line.A.y <= y && line.v.x > 0) || (line.A.y > y && line.v.x < 0))
        continue; // scanline is inside
      else{
        return {1, -1}; // scanline is outside
      }
    }
    double intersection = line.A.x + ((y - line.A.y) / line.v.y) * line.v.x;
    if(line.v.y < 0)
      xMin = std::max(xMin, intersection);
    else
      xMax = std::min(xMax, intersection);
  }
  return {xMin, xMax};
}

namespace shmap{
  short shadowmap[windowHeight][windowWidth] = {};
  inline void insert(const square &sq){
    for(uint y = std::ceil(scaleToWindowY(sq.boundingBoxMinY)); y < std::ceil(scaleToWindowY(sq.boundingBoxMaxY)); y++){
      std::pair interval = sq.intersectX(scaleFromWindowY(y));
      if(interval.second <= interval.first)
        continue;
      uint start = std::min<uint>(std::ceil(scaleToWindowX(interval.first)), windowWidth);
      uint end = std::min<uint>(std::ceil(scaleToWindowX(interval.second)), windowWidth);
      if(end <= start)
        continue;
#ifdef DCHECK
      if(y < 0 || y >= windowHeight || start < 0 || start >= windowWidth || end < 0 || end > windowWidth){
        logError("trying to insert shadowmap value at impossible coordinates! y=" + std::to_string(y) +
        " start=" + std::to_string(start) + 
        " end=" + std::to_string(end));
        exit(1);
      }
#endif
      std::fill(&shadowmap[y][start], &shadowmap[y][end], end);
    }
  }
  inline uint nextX(uint x, uint y){
    uint nX;
    while(x < windowWidth && (nX = shadowmap[y][x]) != 0){
      x = nX;
    }
    return x;
  }
  inline void reset(){
    std::fill(&shadowmap[0][0], &shadowmap[0][0] + sizeof(shadowmap) / sizeof(shadowmap[0][0]), 0);
  }
}

std::vector<square> tiles;

void Renderer::update(){
  shmap::reset();
  tiles.clear();
  for (uint j = 0; j < windowHeight; j++) {
    for (uint i = 0; i < windowWidth; i++) {
      uint next = shmap::nextX(i, j);
      if(next != i){
        i = next;
        continue;
      }
      vec3<double> ray = dir + planeX * scaleFromWindowX(i) + planeY * scaleFromWindowY(j) + (vec3<double>){1e-10, 1e-10, 1e-10};
      vec3<double> step = ray.apply<double>([](double a){ return abs(1/a); });
      vec3<int8_t> orientation = ray.apply<int8_t>([](double a){ return (int8_t)(a >= 0 ? 1 : -1); });
      vec3<int> cell = position.apply(static_cast<double (*)(double)>(floor));
      vec3<double> current = ray.apply<double, double>([](double r, double p){ return r >= 0 ? (1 - p) / r : -p / r; }, position - cell);
      //bool oob = false;
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
          //oob = true;
          break;
        }
        if(getCell(cell)){
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
              point01 = cell + vec3<int>({1, 0, 1});
              point10 = cell + vec3<int>({1, 1, 0});
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
              point01 = cell + vec3<int>({1, 1, 0});
              point10 = cell + vec3<int>({0, 1, 1});
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
              point01 = cell + vec3<int>({0, 1, 1});
              point10 = cell + vec3<int>({1, 0, 1});
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
          if(dist00 < 0 || dist01 < 0 || dist10 < 0 || dist11 < 0)
            continue; //find next tile, this one intersects the viewing plane

          vec3<double> screenPoint00 = diffpoint00 / dist00 - dir;
          vec3<double> screenPoint01 = diffpoint01 / dist01 - dir;
          vec3<double> screenPoint10 = diffpoint10 / dist10 - dir;
          vec3<double> screenPoint11 = diffpoint11 / dist11 - dir;
          vec2<double> projectedPoint00 = {screenPoint00 * planeX, screenPoint00 * planeY};
          vec2<double> projectedPoint01 = {screenPoint01 * planeX, screenPoint01 * planeY};
          vec2<double> projectedPoint10 = {screenPoint10 * planeX, screenPoint10 * planeY};
          vec2<double> projectedPoint11 = {screenPoint11 * planeX, screenPoint11 * planeY};

          square tile = {
            .distance = (dist00 + dist01 + dist10 + dist11) / 4,
            .point00 = point00,
            .point01 = point01,
            .point10 = point10,
            .point11 = point11,
            .bounds = {
              {projectedPoint00, projectedPoint01 - projectedPoint00},
              {projectedPoint01, projectedPoint11 - projectedPoint01},
              {projectedPoint11, projectedPoint10 - projectedPoint11},
              {projectedPoint10, projectedPoint00 - projectedPoint10}
            },
            .boundingBoxMinY = std::max(std::min({projectedPoint00.y, projectedPoint01.y, projectedPoint10.y, projectedPoint11.y}), scaleFromWindowY(0)),
            .boundingBoxMaxY = std::min(std::max({projectedPoint00.y, projectedPoint01.y, projectedPoint10.y, projectedPoint11.y}), scaleFromWindowY(windowHeight)),
            .cell = cell,
            .cellType = getCell(cell),
            .side = side,
            .raycastingPos = {i, j}
          };
#ifdef DCHECK
          if(tile.bounds[0].v.cross(tile.bounds[1].v) < 0 ||
             tile.bounds[1].v.cross(tile.bounds[2].v) < 0 ||
             tile.bounds[2].v.cross(tile.bounds[3].v) < 0 ||
             tile.bounds[3].v.cross(tile.bounds[0].v) < 0){
            logError("tile should not be visible!")
            break;
          }
#endif
          tiles.push_back(tile);
          shmap::insert(tile);
          break;
        }
          
      }
    }
  }
  std::sort(tiles.begin(), tiles.end());
  sf::Image image({windowWidth, windowHeight}, sf::Color::Black);
  // TODO: render tiles
  shmap::reset();
  selectedTile = nullptr;
  // uint i = 0;
  for(const square &tile : tiles){
    vec3<double> W = position - tile.point00;
    for(uint y = std::ceil(scaleToWindowY(tile.boundingBoxMinY)); y < scaleToWindowY(tile.boundingBoxMaxY); y++){
      std::pair interval = tile.intersectX(scaleFromWindowY(y));
      uint end = std::clamp<double>(std::ceil(scaleToWindowX(interval.second)), 0, windowWidth);
      for(uint x = std::ceil(scaleToWindowX(interval.first)); x < end; x++){
        uint next = shmap::nextX(x, y);
        if(next != x){
          x = next;
          continue;
        }
#ifdef DCHECK
        if(x >= windowWidth){
          logError("x out of bounds!");
          exit(1);
        }
        if(y >= windowHeight){
          logError("y out of bounds!");
          exit(1);
        }
#endif
        vec2<double> texturePos;

        vec3<double> ray = dir + planeX * scaleFromWindowX(x) + planeY * scaleFromWindowY(y);
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
        
        image.setPixel({x, y}, textureColor(texturePos, tile));
        shmap::shadowmap[y][x] = end;
      }
    }
    //shmap::insert(tile);
    //image.setPixel({tile.raycastingPos.x, tile.raycastingPos.y}, sf::Color::White);

    //i++;
    //if(i == selectedSquare){
    //}
  }
  //const square* selectedTile = &tiles[std::clamp<uint>(selectedSquare, 0, tiles.size())];
  if(selectedTile){
    for (double i = 0; i < 1; i += 0.01)
    {
      vec2<double> pixel = scaleToWindow(selectedTile->bounds[0].A + selectedTile->bounds[0].v * i);
      image.setPixel({(uint)std::clamp<int>(pixel.x, 0, windowWidth - 1), (uint)std::clamp<int>(pixel.y, 0, windowHeight - 1)}, sf::Color::Green);
      pixel = scaleToWindow(selectedTile->bounds[1].A + selectedTile->bounds[1].v * i);
      image.setPixel({(uint)std::clamp<int>(pixel.x, 0, windowWidth - 1), (uint)std::clamp<int>(pixel.y, 0, windowHeight - 1)}, sf::Color::Green);
      pixel = scaleToWindow(selectedTile->bounds[2].A + selectedTile->bounds[2].v * i);
      image.setPixel({(uint)std::clamp<int>(pixel.x, 0, windowWidth - 1), (uint)std::clamp<int>(pixel.y, 0, windowHeight - 1)}, sf::Color::Green);
      pixel = scaleToWindow(selectedTile->bounds[3].A + selectedTile->bounds[3].v * i);
      image.setPixel({(uint)std::clamp<int>(pixel.x, 0, windowWidth - 1), (uint)std::clamp<int>(pixel.y, 0, windowHeight - 1)}, sf::Color::Green);
    }
    image.setPixel({selectedTile->raycastingPos.x, selectedTile->raycastingPos.y}, sf::Color::Red);
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
  // std::cout << std::setw(8) << tiles.size() << "\r";
  // std::flush(std::cout);
}

sf::Color Renderer::textureColor(vec2<double> position, const square &sq) const{
  // oob detection
  if(position.x < -EPS || position.x >= 1 + EPS || position.y < -EPS || position.y >= 1+EPS){
    selectedTile = &sq;
    // std::cout << std::setw(20) << position.x << std::setw(20) << position.y << "\r";
    // std::flush(std::cout);
    return sf::Color::White;
  }
  // edge
  if(position.x < 0.01 || position.x > 0.99 || position.y < 0.01 || position.y > 0.99)
    return sf::Color::Blue;

  // checkerboard pattern
  vec2<int> pixel = position * 8;
  double brightness = (16 - ((pixel.x ^ pixel.y) & 1) * 5);
  return sf::Color((sq.cell.x + 1)*brightness, (sq.cell.y + 1)*brightness, (sq.cell.z + 1)*brightness);

  // gradient
  //return sf::Color(position.x * 256, position.y * 256, 0);
}