#ifndef VECTOR_H
#define VECTOR_H

template<typename T>
struct vec2{
  T x;
  T y;
  template<typename T2>
  inline vec2<T> operator+(vec2<T2> other){ return {x + other.x, y + other.y}; }
  template<typename T2>
  inline vec2<T> operator-(vec2<T2> other){ return {x - other.x, y - other.y}; }
  template<typename T2>
  inline vec2<T> operator*(T2 other){ return {x * other, y * other}; }
  template<typename T2>
  inline auto operator*(vec2<T2> other){ return x * other.x + y * other.y; }
  template<typename T2>
  inline vec2<T> operator/(T2 other){ return {x / other, y / other}; }
  template<typename T2>
  inline operator vec2<T2>(){ return {(T2)x, (T2)y}; }
  template<typename T2 = T>
  inline vec2<T2> apply(T2 (*f)(T)){ return {f(x), f(y)}; }
  template<typename T2 = T, typename T3>
  inline vec2<T2> apply(T2 (*f)(T, T3), vec2<T3> other){ return {f(x, other.x), f(y, other.y)}; }
  inline bool all(){ return x && y; }
  inline bool any(){ return x || y; }
  inline T cross(vec2<T> other){ return {x * other.y - y * other.x}; }
};

template<typename T>
struct vec3{
  T x;
  T y;
  T z;
  template<typename T2>
  inline vec3<T> operator+(vec3<T2> other){ return {x + other.x, y + other.y, z + other.z}; }
  template<typename T2>
  inline vec3<T> operator-(vec3<T2> other){ return {x - other.x, y - other.y, z - other.z}; }
  template<typename T2>
  inline vec3<T> operator*(T2 other){ return {x * other, y * other, z * other}; }
  template<typename T2>
  inline auto operator*(vec3<T2> other){ return x * other.x + y * other.y + z * other.z; }
  template<typename T2>
  inline vec3<T> operator/(T2 other){ return {x / other, y / other, z / other}; }
  template<typename T2>
  inline operator vec3<T2>(){ return {(T2)x, (T2)y, (T2)z}; }
  template<typename T2 = T>
  inline vec3<T2> apply(T2 (*f)(T)){ return {f(x), f(y), f(z)}; }
  template<typename T2 = T, typename T3>
  inline vec3<T2> apply(T2 (*f)(T, T3), vec3<T3> other){ return {f(x, other.x), f(y, other.y), f(z, other.z)}; }
  inline bool all(){ return x && y && z; }
  inline bool any(){ return x || y || z; }
  inline vec3<T> cross(vec3<T> other){ return {y * other.z - z * other.y, z * other.x - x * other.z, x * other.y - y * other.x}; }
};
template<typename T>
struct vec4{
  T x;
  T y;
  T z;
  T w;
};

#endif
