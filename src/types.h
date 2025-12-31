#ifndef TYPES_H_
#define TYPES_H_

#include <iostream>
#include <cmath>
#include <vector>
#include <stack>
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

bool operator==(const QuadNum& x, const QuadNum& y);
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

bool operator==(const Point& p, const Point& q);
std::ostream& operator<<(std::ostream& os, const Point& p);

struct Segment {
  Point p1;
  Point p2;

  Segment(const Point& p1, const Point& p2);
};

bool operator==(const Segment& s1, const Segment& s2);
std::ostream& operator<<(std::ostream& os, const Segment& s);

struct Line {
  // ax + by = c
  QuadNum a, b, c;

  Line(const Point& p, const Point& q);
  Line(const Segment& s);

  private:
  void normalize();
};

bool operator==(const Line& l1, const Line& l2);
std::ostream& operator<<(std::ostream& os, const Line& l);

struct Circle {
  QuadNum x0, y0, r2;  // (x0, y0) = center, r2 = squared radius

  Circle(const Point& p, const Point& q);
};

bool operator==(const Circle& c1, const Circle& c2);
std::ostream& operator<<(std::ostream& os, const Circle& c);

vector<Point> ll_sol(const Line& line1, const Line& line2);
vector<Point> lc_sol(const Line& line, const Circle& circle);
vector<Point> cc_sol(const Circle& circle1, const Circle& circle2);

struct State {
  vector<Point> points;
  vector<Line> lines;
  vector<Circle> circles;

  State(const vector<Point>& points, const vector<Line>& lines, const vector<Circle>& circles);
  vector<State> children() const;
  bool contains(const Point& point) const;
  bool contains(const Line& line) const;
  bool contains(const Circle& circle) const;
  void add(const Point& point);
  void add(const Line& line);
  void add(const Circle& circle);
  size_t size() const;
};

struct Goal : State {
  vector<Segment> segments;

  Goal(const vector<Point>& points, const vector<Line>& lines, const vector<Circle>& circles, const vector<Segment>& segments);
  bool contained_in(const State& state) const;
};

#endif
