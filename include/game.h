#ifndef GAME_H
#define GAME_H
#include "config.h"
#include <sys/types.h>
#include <inttypes.h>
#include "vector.h"


extern vec3<double> position;
extern vec4<double> orientation;

u_int8_t getCell(vec3<uint> pos);
void setCell(vec3<uint> pos, uint8_t state);




#endif