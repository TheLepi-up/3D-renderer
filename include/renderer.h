#ifndef RENDERER_H
#define RENDERER_H
#include <SFML/Graphics.hpp>
#include <inttypes.h>
#include "config.h"
#include "game.h"
#include "vector.h"
#include "logging.h"

struct line2D{
  vec2<double> A;
  vec2<double> v;
};

struct square{
  double distance;
  vec3<int> point00;
  vec3<int> point01;
  vec3<int> point10;
  vec3<int> point11;
  line2D bounds[4];
  double boundingBoxMinY;
  double boundingBoxMaxY;
  vec3<uint> cell;
  uint8_t cellType;
  uint8_t side;
  // vec2<uint> raycastingPos;
  bool operator<(const square &other) const { return distance < other.distance; }
  std::pair<double, double> intersectX(double y) const;
};

class Renderer
{
private:
  sf::RenderWindow &window;

  vec3<double> position = {8, 8, 8};
  vec3<double> dir = {1, 0, 0};
  vec3<double> planeX = {0, 1, 0};
  vec3<double> planeY = {0, 0, 1};


  u_int8_t grid[gridSizeX*gridSizeY*gridSizeZ] = {};
  sf::Color textureColor(vec2<double> position, const square& sq) const;
public:
  // uint selectedSquare = 0;
  Renderer(sf::RenderWindow &window) : window(window){}
  inline uint8_t getCell(vec3<uint> pos){ return grid[pos.x + pos.y * gridSizeX + pos.z * gridSizeX * gridSizeY]; }
  inline void setCell(vec3<uint> pos, uint8_t state){ grid[pos.x + pos.y * gridSizeX + pos.z * gridSizeX * gridSizeY] = state; }
  void update();
  inline void setDir(vec3<double> dir){ this->dir = dir; }
  inline void setPlaneX(vec3<double> planeX){ this->planeX = planeX; }
  inline void setPlaneY(vec3<double> planeY){ this->planeY = planeY; }
  inline vec3<double> getDir(){ return this->dir; }
  inline vec3<double> getPlaneX(){ return this->planeX; }
  inline vec3<double> getPlaneY(){ return this->planeY; }
  inline void move(vec3<double> step){ position += step; }
  inline void rotate(vec2<double> direction){
    if(!direction.any()){ // rotation by angle 0
      return;
    }

    double mag = direction.magnitude();
    vec3<double> rotationDir = (planeX * direction.x + planeY * direction.y) / mag;
    double angle = mag * M_PI;
    vec3<double> axis = dir.cross(rotationDir);
    mat3<double> rot = rotationMatrix(axis, angle);
    dir = rot * dir;
    planeX = rot * planeX;
    //orthogonalize and normalize
    dir /= dir.magnitude();
    planeX -= dir * (dir * planeX);
    planeX /= planeX.magnitude();
    planeY = dir.cross(planeX);
  }
};
#endif