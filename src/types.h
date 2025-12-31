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
#include <chrono>
#include <cstdlib>

using std::vector;
using std::string;
using std::cout;
using std::abs;
using std::gcd;

using int64 = long long;
using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;

constexpr int ROOT = 3;

struct QuadNum {
  int64 a, b, d;  // {a + b√ROOT} / d

  QuadNum();
  QuadNum(int64 a);
  QuadNum(int64 a, int64 b, int64 d);
  QuadNum& operator+=(const QuadNum& other);
  QuadNum& operator-=(const QuadNum& other);
  QuadNum& operator*=(const QuadNum& other);
  QuadNum& operator/=(const QuadNum& other);
  QuadNum operator-() const;
  double to_double() const;

  private:
  void normalize();
};

QuadNum operator+(const QuadNum& x, const QuadNum& y);
QuadNum operator-(const QuadNum& x, const QuadNum& y);
QuadNum operator*(const QuadNum& x, const QuadNum& y);
QuadNum operator/(const QuadNum& x, const QuadNum& y);

int64 isqrt(int64 n);  // return -1 if sol != integer
QuadNum qsqrt(const QuadNum& x);  // return -1 if sol != QuadNum
int sign(const QuadNum& x);
std::ostream& operator<<(std::ostream& os, const QuadNum& x);

struct Point {
  QuadNum x, y;

  Point(const QuadNum& x, const QuadNum& y);
};

std::ostream& operator<<(std::ostream& os, const Point& p);

struct Segment {
  Point p1;
  Point p2;

  Segment(const Point& p1, const Point& p2);
};

std::ostream& operator<<(std::ostream& os, const Segment& s);

struct Line {
  // ax + by = c
  QuadNum a, b, c;

  Line(const Point& p, const Point& q);
  Line(const Segment& s);

  private:
  void normalize();
};

std::ostream& operator<<(std::ostream& os, const Line& l);

struct Circle {
  QuadNum x0, y0, r2;  // (x0, y0) = center, r2 = squared radius

  Circle(const Point& p, const Point& q);
};

std::ostream& operator<<(std::ostream& os, const Circle& c);

bool operator==(const QuadNum& x, const QuadNum& y);
bool operator==(const Point& p, const Point& q);
bool operator==(const Line& l1, const Line& l2);
bool operator==(const Circle& c1, const Circle& c2);
bool operator==(const Segment& s1, const Segment& s2);

inline void hash_combine(std::size_t& seed, std::size_t h) {
  seed ^= h + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
};

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

struct SegmentHash {
  std::size_t operator()(const Segment& s) const noexcept;
};

using PointSet = std::unordered_set<Point, PointHash>;
using LineSet = std::unordered_set<Line, LineHash>;
using CircleSet = std::unordered_set<Circle, CircleHash>;
using SegmentSet = std::unordered_set<Segment, SegmentHash>;

vector<Point> ll_sol(const Line& line1, const Line& line2);
vector<Point> lc_sol(const Line& line, const Circle& circle);
vector<Point> cc_sol(const Circle& circle1, const Circle& circle2);

struct State {
  PointSet points;
  LineSet lines;
  CircleSet circles;

  State(const PointSet& points, const LineSet& lines, const CircleSet& circles);
  vector<State> children() const;
  bool contains(const Point& point) const;
  bool contains(const Line& line) const;
  bool contains(const Circle& circle) const;
  void insert(const Point& point);
  void insert(const Line& line);
  void insert(const Circle& circle);
  size_t size() const;
};

bool operator==(const State& s1, const State& s2);

template <class T, class Hash>
inline std::size_t hash_unordered_set(const std::unordered_set<T, Hash>& set) {
  std::size_t h = 0;
  for (const auto& x : set) {
    hash_combine(h, Hash{}(x));
  }
  hash_combine(h, std::hash<std::size_t>{}(set.size()));
  return h;
};

struct StateHash {
  std::size_t operator()(const State& s) const noexcept;
};

using StateSet = std::unordered_set<State, StateHash>;

struct Goal : State {
  SegmentSet segments;

  Goal(const PointSet& points, const LineSet& lines, const CircleSet& circles, const SegmentSet& segments);
  bool contained_in(const State& state) const;
};

#endif
