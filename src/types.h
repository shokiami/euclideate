#ifndef TYPES_H_
#define TYPES_H_

#include <iostream>
#include <cmath>
#include <vector>
#include <deque>
#include <string>
#include <fstream>
#include <unordered_set>
#include <numeric>
#include <stdexcept>
#include <functional>

using std::vector;
using std::string;
using std::cout;
using std::endl;
using std::unordered_set;
using std::abs;
using std::gcd;
using int64 = long long;

constexpr int ROOT = 3;
constexpr size_t DEGREE = 2;

struct QuadNum {
  int64 a, b, d;  // {a + b sqrt(ROOT)} / d

  QuadNum();
  QuadNum(int64 a);
  QuadNum(int64 a, int64 b, int64 d);
  int sign() const;
  QuadNum& operator+=(const QuadNum& other);
  QuadNum& operator-=(const QuadNum& other);
  QuadNum& operator*=(const QuadNum& other);
  QuadNum& operator/=(const QuadNum& other);
  QuadNum operator-() const;

  private:
  void normalize();
};

QuadNum operator+(const QuadNum& x, const QuadNum& y);
QuadNum operator-(const QuadNum& x, const QuadNum& y);
QuadNum operator*(const QuadNum& x, const QuadNum& y);
QuadNum operator/(const QuadNum& x, const QuadNum& y);
std::ostream& operator<<(std::ostream& os, const QuadNum& q);

struct Point {
  QuadNum x, y;

  Point(const QuadNum& x, const QuadNum& y);
};

struct Line {
  // ax + by + c = 0
  QuadNum a, b, c;

  Line(const Point& p, const Point& q);

  private:
  void normalize();
};

struct Circle {
  QuadNum x, y, r2;  // (x, y) = center, r2 = squared radius

  Circle(const Point& p, const Point& q);
};

bool operator==(const QuadNum& x, const QuadNum& y);
bool operator==(const Point& p, const Point& q);
bool operator==(const Line& l1, const Line& l2);
bool operator==(const Circle& c1, const Circle& c2);

struct QuadNumHash {
  std::size_t operator()(const QuadNum& x) const noexcept;
};

struct PointHash {
  std::size_t operator()(const Point& p) const noexcept;
};

struct LineHash {
  std::size_t operator()(const Line& l) const noexcept;
};

struct CircleHash {
  std::size_t operator()(const Circle& c) const noexcept;
};

inline void hash_combine(std::size_t& seed, std::size_t h) {
  seed ^= h + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
};

template <class T, class Hash>
inline std::size_t hash_unordered_set(const std::unordered_set<T, Hash>& set) {
  std::size_t h = 0;
  for (const auto& x : set) {
    hash_combine(h, Hash{}(x));
  }
  hash_combine(h, std::hash<std::size_t>{}(set.size()));
  return h;
};

struct State {
  unordered_set<Point, PointHash> points;
  unordered_set<Line, LineHash> lines;
  unordered_set<Circle, CircleHash> circles;

  size_t size() const;
};

bool operator==(const State& s1, const State& s2);

struct StateHash {
  std::size_t operator()(const State& s) const noexcept;
};

#endif
