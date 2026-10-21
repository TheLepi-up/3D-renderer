#ifndef VECTOR_H
#define VECTOR_H
#include <type_traits>
#include <math.h>

template<class T, template<class> class U>
inline constexpr bool is_instance_of_v = std::false_type{};

template<template<class> class U, class V>
inline constexpr bool is_instance_of_v<U<V>,U> = std::true_type{};

template<typename T>
struct vec2{
  T x;
  T y;
  template<typename T2>
  inline vec2<T> operator+(vec2<T2> other) const { return {x + other.x, y + other.y}; }
  template<typename T2>
  inline vec2<T>& operator+=(vec2<T2> other) { x += other.x; y += other.y; return *this; }
  template<typename T2>
  inline vec2<T> operator-(vec2<T2> other) const { return {x - other.x, y - other.y}; }
  template<typename T2>
  inline vec2<T>& operator-=(vec2<T2> other) { x -= other.x; y -= other.y; return *this; }
  template<typename T2>
  inline vec2<T> operator*(T2 other) const requires (std::is_arithmetic<T2>::value) { return {x * other, y * other}; }
  template<typename T2>
  inline vec2<T>& operator*=(T2 other) { x *= other; y *= other; return *this; }
  template<typename T2>
  inline auto operator*(vec2<T2> other) const requires (!is_instance_of_v<T, vec2>) { return x * other.x + y * other.y; }
  template<typename T2>
  inline T operator*(vec2<T2> other) const requires (is_instance_of_v<T, vec2>) { return {x * other, y * other}; }
  template<typename T2>
  inline vec2<T> operator/(T2 other) const { return {x / other, y / other}; }
  template<typename T2>
  inline vec2<T>& operator/=(T2 other) { x /= other; y /= other; return *this; }
  template<typename T2>
  inline operator vec2<T2>() const requires (std::is_convertible<T, T2>::value) { return {(T2)x, (T2)y}; }
  inline T operator[](uint i) const { return ((T*)this)[i]; }
  template<typename F>
  inline auto apply(F f) const { return vec2<decltype(f(x))>{f(x), f(y)}; }
  template<typename F, typename T2>
  inline auto apply(F f, vec2<T2> other) const { return vec2<decltype(f(x, other.x))>{f(x, other.x), f(y, other.y)}; }
  inline bool all() const { return x && y; }
  inline bool any() const { return x || y; }
  inline T cross(vec2<T> other) const { return {x * other.y - y * other.x}; }
  inline vec2<T> rotateRight() const { return {-y, x}; }
  inline vec2<T> rotateLeft() const { return {y, -x}; }
  inline double magnitude(){ return sqrt(*this * *this); }
};

template<typename T>
struct vec3{
  T x;
  T y;
  T z;
  template<typename T2>
  inline vec3<T> operator+(vec3<T2> other) const { return {x + other.x, y + other.y, z + other.z}; }
  template<typename T2>
  inline vec3<T>& operator+=(vec3<T2> other) { x += other.x; y += other.y; z += other.z; return *this; }
  template<typename T2>
  inline vec3<T> operator-(vec3<T2> other) const { return {x - other.x, y - other.y, z - other.z}; }
  template<typename T2>
  inline vec3<T>& operator-=(vec3<T2> other) { x -= other.x; y -= other.y; z -= other.z; return *this; }
  template<typename T2>
  inline vec3<T> operator*(T2 other) const requires (std::is_arithmetic<T2>::value) { return {x * other, y * other, z * other}; }
  template<typename T2>
  inline vec3<T>& operator*=(T2 other) { x *= other; y *= other; z *= other; return *this; }
  template<typename T2> // Dot product
  inline auto operator*(vec3<T2> other) const requires (!is_instance_of_v<T, vec3>) { return x * other.x + y * other.y + z * other.z; }
  template<typename T2> // Matrix Vector product
  inline T operator*(vec3<T2> other) const requires (is_instance_of_v<T, vec3>){ return { x  * other, y * other, z * other }; }
  template<typename T2>
  inline vec3<T> operator/(T2 other) const { return {x / other, y / other, z / other}; }
  template<typename T2>
  inline vec3<T>& operator/=(T2 other) { x /= other; y /= other; z /= other; return *this; }
  template<typename T2>
  inline operator vec3<T2>() const requires (std::is_convertible<T, T2>::value) { return {(T2)x, (T2)y, (T2)z}; }
  inline T operator[](uint i) const { return ((T*)this)[i]; }
  template<typename F>
  inline auto apply(F f) const { return vec3<decltype(f(x))>{f(x), f(y), f(z)}; }
  template<typename F, typename T2>
  inline auto apply(F f, vec3<T2> other) const { return vec3<decltype(f(x, other.x))>{f(x, other.x), f(y, other.y), f(z, other.z)}; }
  inline bool all() const { return x && y && z; }
  inline bool any() const { return x || y || z; }
  inline vec3<T> cross(vec3<T> other) const { return {y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x}; }
  inline double magnitude(){ return sqrt(*this * *this); }
};
template<typename T>
using mat3 = vec3<vec3<T>>;

inline constexpr mat3<double> rotationMatrix(vec3<double> axis, double angle) {
    double cosA = cos(angle);
    double sinA = sin(angle);
    return {
      {axis.x * axis.x * (1.0 - cosA) + cosA, axis.x * axis.y * (1.0 - cosA) - axis.z * sinA, axis.x * axis.z * (1.0 - cosA) + axis.y * sinA},
      {axis.x * axis.y * (1.0 - cosA) + axis.z * sinA, axis.y * axis.y * (1.0 - cosA) + cosA, axis.y * axis.z * (1.0 - cosA) - axis.x * sinA},
      {axis.x * axis.z * (1.0 - cosA) - axis.y * sinA, axis.y * axis.z * (1.0 - cosA) + axis.x * sinA, axis.z * axis.z * (1.0 - cosA) + cosA}
    };
  }

template<typename T>
struct vec4{
  T x;
  T y;
  T z;
  T w;
};

#endif
